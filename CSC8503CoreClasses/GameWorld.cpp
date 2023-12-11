#include "GameWorld.h"
#include "GameObject.h"
#include "Constraint.h"
#include "CollisionDetection.h"
#include "Camera.h"


using namespace NCL;
using namespace NCL::CSC8503;

GameWorld::GameWorld()	{
	shuffleConstraints	= false;
	shuffleObjects		= false;
	worldIDCounter		= 0;
	worldStateCounter	= 0;
	InitPhysicsMaterials();
	grid = new NavigationGrid("TestGrid1.txt");
}

GameWorld::~GameWorld()	{
}

void GameWorld::InitPhysicsMaterials() {
	PhysicsMaterial bouncy;
	bouncy.e = 0.8f;
	bouncy.linearDampHorizontal = 0.35f;
	bouncy.linearDampVertical = 0.35f;
	bouncy.angularDamp = 0.35f;
	physicsMaterials["Bouncy"] = bouncy;

	PhysicsMaterial standard;
	physicsMaterials["Standard"] = standard;

	PhysicsMaterial player;
	player.e = 0.0f;
	player.linearDampHorizontal = 8.0f;
	player.angularDamp = 8.0f;
	physicsMaterials["Player"] = player;
}

std::vector<Vector3> GameWorld::GetPath(Vector3 start, Vector3 end) {
	NavigationPath outPath;

	bool found = grid->FindPath(start, end, outPath);

	Vector3 pos;
	std::vector<Vector3> path;
	while (outPath.PopWaypoint(pos))path.push_back(pos);

	return path;
}

void GameWorld::Clear() {
	gameObjects.clear();
	constraints.clear();
	worldIDCounter		= 0;
	worldStateCounter	= 0;
}

void GameWorld::ClearAndErase() {
	for (auto& i : gameObjects) {
		delete i;
	}
	for (auto& i : constraints) {
		delete i;
	}
	Clear();
}

void GameWorld::AddGameObject(GameObject* o) {
	gameObjects.emplace_back(o);
	o->SetWorldID(worldIDCounter++);
	worldStateCounter++;
}

void GameWorld::RemoveGameObject(GameObject* o, bool andDelete) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
	if (andDelete) {
		delete o;
	}
	worldStateCounter++;
}

void GameWorld::GetObjectIterators(
	GameObjectIterator& first,
	GameObjectIterator& last) const {

	first	= gameObjects.begin();
	last	= gameObjects.end();
}

void GameWorld::OperateOnContents(GameObjectFunc f) {
	for (GameObject* g : gameObjects) {
		f(g);
	}
}

void GameWorld::UpdateWorld(float dt) {
	for (GameObject* gameObject : gameObjects)gameObject->Update(dt);
	auto rng = std::default_random_engine{};

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine e(seed);

	if (shuffleObjects) {
		std::shuffle(gameObjects.begin(), gameObjects.end(), e);
	}

	if (shuffleConstraints) {
		std::shuffle(constraints.begin(), constraints.end(), e);
	}
}

void GameWorld::UpdateWorldPhysics(float dt) {
	for (GameObject* gameObject : gameObjects)gameObject->PhysicsUpdate(dt);
}

void GameWorld::StartWorld() {
	for (GameObject* gameObject : gameObjects)gameObject->Start(this);
}

bool GameWorld::TryGetObjectByTag(std::string t, GameObject*& out) { //searches through game objects to find provided tag
	for (GameObject* gameObject : gameObjects) {
		if (gameObject->GetTag() == t) {
			out = gameObject;
			return true;
		}	
	}
	return false;
}

bool GameWorld::TryGetPhysMat(std::string name,PhysicsMaterial*& out) {
	if (physicsMaterials.find(name) == physicsMaterials.end())
	{
		return false;
	}
	out = &physicsMaterials[name];
	return true;
}

bool GameWorld::Raycast(Ray& r, RayCollision& closestCollision, bool closestObject, GameObject* ignoreThis) const {
	//The simplest raycast just goes through each object and sees if there's a collision
	RayCollision collision;

	for (auto& i : gameObjects) {
		if (!i->GetBoundingVolume()) { //objects might not be collideable etc...
			continue;
		}
		if (i == ignoreThis) {
			continue;
		}
		RayCollision thisCollision;
		if (CollisionDetection::RayIntersection(r, *i, thisCollision)) {
				
			if (!closestObject) {	
				closestCollision		= collision;
				closestCollision.node = i;
				return true;
			}
			else {
				if (thisCollision.rayDistance < collision.rayDistance) {
					thisCollision.node = i;
					collision = thisCollision;
				}
			}
		}
	}
	if (collision.node) {
		closestCollision		= collision;
		closestCollision.node	= collision.node;
		return true;
	}
	return false;
}
bool GameWorld::Raycast(Ray& r, RayCollision& closestCollision, bool closestObject, const std::map<int, bool> raycastCollideMap, GameObject* ignoreThis ) const {
	//The simplest raycast just goes through each object and sees if there's a collision
	RayCollision collision;

	for (auto& i : gameObjects) {
		if (!i->GetBoundingVolume()) { //objects might not be collideable etc...
			continue;
		}
		if (i == ignoreThis) {
			continue;
		}
		if (!raycastCollideMap.at(i->GetPhysicsObject()->GetCollisionLayer())) {
			continue;
		}
		RayCollision thisCollision;
		if (CollisionDetection::RayIntersection(r, *i, thisCollision)) {

			if (!closestObject) {
				closestCollision = collision;
				closestCollision.node = i;
				return true;
			}
			else {
				if (thisCollision.rayDistance < collision.rayDistance) {
					thisCollision.node = i;
					collision = thisCollision;
				}
			}
		}
		else {
			std::cout << "";
		}
	}
	if (collision.node) {
		closestCollision = collision;
		closestCollision.node = collision.node;
		return true;
	}
	return false;
}


/*
Constraint Tutorial Stuff
*/

void GameWorld::AddConstraint(Constraint* c) {
	constraints.emplace_back(c);
}

void GameWorld::RemoveConstraint(Constraint* c, bool andDelete) {
	constraints.erase(std::remove(constraints.begin(), constraints.end(), c), constraints.end());
	if (andDelete) {
		delete c;
	}
}

void GameWorld::GetConstraintIterators(
	std::vector<Constraint*>::const_iterator& first,
	std::vector<Constraint*>::const_iterator& last) const {
	first	= constraints.begin();
	last	= constraints.end();
}