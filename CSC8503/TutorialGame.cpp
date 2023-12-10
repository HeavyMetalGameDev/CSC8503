#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include "MovementApplierComponent.h"
#include "FirstPersonInputComponent.h"
#include "SimpleChaseComponent.h"
#include "PlayerInputComponent.h"
#include "ObjectPickupComponent.h"
#include "TriggerComponent.h"
#include "PointPickupComponent.h"
#include "PlayerValuesComponent.h"
#include "KeyComponent.h"
#include "Assets.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"
#include <fstream>



using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame() : controller(*Window::GetWindow()->GetKeyboard(), *Window::GetWindow()->GetMouse()) {
	world		= new GameWorld();
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
	renderer->Init();
	renderer->InitStructures();
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;

	world->GetMainCamera().SetController(controller);

	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");

	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");

	InitialiseAssets();
	physics->BuildStaticQuadTree();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	cubeMesh	= renderer->LoadMesh("cube.msh");
	sphereMesh	= renderer->LoadMesh("sphere.msh");
	charMesh	= renderer->LoadMesh("goat.msh");
	enemyMesh	= renderer->LoadMesh("Keeper.msh");
	bonusMesh	= renderer->LoadMesh("apple.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");

	basicTex	= renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitCamera();
	
	InitWorld();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void TutorialGame::UpdateGame(float dt) {
	if (!inSelectionMode) {
		world->GetMainCamera().UpdateCamera(dt);
	}
	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0,1,0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera().SetPosition(camPos);
		world->GetMainCamera().SetPitch(angles.x);
		world->GetMainCamera().SetYaw(angles.y);
	}

	UpdateKeys();

	if (testStateObject)testStateObject->Update(dt);

	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::K) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);

		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		}
	}

	Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));

	SelectObject();
	MoveSelectedObject();

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);
	

	
	
	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F8)) {
		world->ShuffleObjects(false);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera().BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();


	if (Window::GetKeyboard()->KeyDown(KeyCodes::UP)) {
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::DOWN)) {
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::NEXT)) {
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0,-10,0));
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyCodes::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera().SetNearPlane(0.1f);
	world->GetMainCamera().SetFarPlane(500.0f);
	world->GetMainCamera().SetPitch(-15.0f);
	world->GetMainCamera().SetYaw(315.0f);
	world->GetMainCamera().SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	//InitMixedGridWorld(15, 15, 3.5f, 3.5f);

	InitDefaultFloor();
	//BridgeConstraintTest();
	//testStateObject = AddStateObjectToWorld(Vector3(0, 10, 0));
	AddPlayerToWorld(Vector3(80, 0, 10));
	AddCubeToWorld(Vector3(-2, 0, 0), Vector3(.5f, .5f, .5f), 0.7f);
	//AddEnemyToWorld(Vector3(3, 10, 0));
	AddSphereToWorld(Vector3(4, 0, 0), 0.5f, 0.7f);
	AddSphereToWorld(Vector3(5, 0, 0), 0.5f, 0.7f);


	//------------ADD PICKUPS---------------------------------------------------------------------------------------------------------------------
	//AddPointPickupToWorld(Vector3(8, -2, 0),10);
	//AddPointPickupToWorld(Vector3(4, -2, 0), 10);
	//AddPointPickupToWorld(Vector3(5, -2, 4), 10);
	//AddPointPickupToWorld(Vector3(6, -2, 0), 10);

	AddKeyDoorPairToWorld(Vector3(50, 0, 10), Vector3(10, 0, 20), Debug::YELLOW);
	//AddKeyDoorPairToWorld(Vector3(12, -2, 12), Vector3(16, -2, 16), Debug::MAGENTA);
	CreateStaticLevel();
	AddCapsuleToWorld(Vector3(3, 5, 0), 1,0.7f);
	//AddSphereToWorld(Vector3(1, 0, 0), .3f, 0.7f);
	//AddTestComponentObjectToWorld(Vector3(5, 5, 5));
	world->StartWorld();
}

void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(8, 8, 8);

	float invCubeMass = 5;
	int numLinks = 10;
	float maxDistance = 30;
	float cubeDistance = 20;

	Vector3 startPos = Vector3(100, 100, 100);

	GameObject* start = AddCubeToWorld(startPos, cubeSize, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks+2)*cubeDistance,0,0), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; i++) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);

		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}
	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume(),false,false,STATIC_LAYER));

	PhysicsObject* fo = floor->GetPhysicsObject();
	fo->SetInverseMass(0);
	fo->InitCubeInertia();

	PhysicsMaterial* floorPhys;
	if (world->TryGetPhysMat("Standard", floorPhys))fo->SetPhysMat(floorPhys);

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();
	sphere->SetTag("Sphere");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	PhysicsObject* so = sphere->GetPhysicsObject();
	so->SetInverseMass(inverseMass);
	so->InitSphereInertia();

	PhysicsMaterial* spherePhys;
	if (world->TryGetPhysMat("Bouncy", spherePhys))so->SetPhysMat(spherePhys);

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddSphereTriggerToWorld(const Vector3& position, float radius) {
	GameObject* sphere = new GameObject();
	sphere->SetTag("SphereTrigger");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	//sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume(),true,true,PICKUP_SPHERE_LAYER));

	//PhysicsMaterial* spherePhys;
	//if (world->TryGetPhysMat("Bouncy", spherePhys))so->SetPhysMat(spherePhys);

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddPointPickupToWorld(const Vector3& position, int points) {
	GameObject* sphere = new GameObject();
	sphere->SetTag("PointPickup");

	Vector3 sphereSize = Vector3(.5f, .5f, .5f);
	SphereVolume* volume = new SphereVolume(.5f);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume(), true, true,STATIC_LAYER));
	sphere->GetRenderObject()->SetColour(Debug::BLUE);

	PointPickupComponent* ppc = new PointPickupComponent(world,sphere, points);
	sphere->AddComponent(ppc);

	//PhysicsMaterial* spherePhys;
	//if (world->TryGetPhysMat("Bouncy", spherePhys))so->SetPhysMat(spherePhys);

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCapsuleToWorld(const Vector3& position, float size, float inverseMass) {
	GameObject* capsule = new GameObject();
	capsule->SetTag("Capsule");

	CapsuleVolume* volume = new CapsuleVolume(size*0.5f,size*0.5f);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(Vector3(size, size, size))
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	PhysicsObject* co = capsule->GetPhysicsObject();
	co->SetInverseMass(inverseMass);
	co->InitSphereInertia();

	PhysicsMaterial* capPhys;
	if (world->TryGetPhysMat("Standard", capPhys))co->SetPhysMat(capPhys);

	world->AddGameObject(capsule);

	return capsule;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	PhysicsObject* co = cube->GetPhysicsObject();
	co->SetInverseMass(inverseMass);
	co->InitCubeInertia();

	PhysicsMaterial* cubePhys;
	if (world->TryGetPhysMat("Standard", cubePhys))co->SetPhysMat(cubePhys);

	world->AddGameObject(cube);

	return cube;
}
GameObject* TutorialGame::AddWallToWorld(const Vector3& position, Vector3 dimensions) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume(),false));

	PhysicsObject* co = cube->GetPhysicsObject();
	co->SetInverseMass(0);
	co->InitCubeInertia();
	co->SetCollisionLayer(STATIC_LAYER);

	PhysicsMaterial* cubePhys;
	if (world->TryGetPhysMat("Standard", cubePhys))co->SetPhysMat(cubePhys);

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize		= 1.0f;
	float inverseMass	= 0.8f;

	GameObject* character = new GameObject();
	CapsuleVolume* volume  = new CapsuleVolume(1.0f,0.5f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), capsuleMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume(),true,false,PLAYER_LAYER));

	PhysicsObject* co = character->GetPhysicsObject();
	co->SetInverseMass(inverseMass);
	co->InitSphereInertia();

	PhysicsMaterial* playerPhys;
	PhysicsMaterial* bouncyPhys;
	if (world->TryGetPhysMat("Player", playerPhys))co->SetPhysMat(playerPhys);
	world->TryGetPhysMat("Bouncy", bouncyPhys);


	character->SetCamera(&world->GetMainCamera());
	character->SetTag("Player");

	MovementApplierComponent* ma = new MovementApplierComponent(&character->GetTransform(), character->GetPhysicsObject(),7000.0f);
	FirstPersonInputComponent* fps = new FirstPersonInputComponent(&world->GetMainCamera());
	PlayerInputComponent* pic = new PlayerInputComponent(character, &world->GetMainCamera());
	PlayerValuesComponent* pvc = new PlayerValuesComponent();

	GameObject* pickupObject = AddSphereTriggerToWorld(Vector3(-1, 10, 0), 0.6f);

	TriggerComponent* tc = new TriggerComponent();

	pickupObject->AddComponent(tc);

	ObjectPickupComponent* opc = new ObjectPickupComponent(character,pickupObject,&world->GetMainCamera(),tc);

	ma->SetInputComponent(fps);
	character->AddComponent(ma);
	character->AddComponent(pic);
	character->AddComponent(opc);
	character->AddComponent(pvc);

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 1.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();

	CapsuleVolume* volume = new CapsuleVolume(meshSize*0.5f, meshSize * 0.33f);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), capsuleMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	PhysicsObject* co = character->GetPhysicsObject();
	co->SetInverseMass(inverseMass);
	co->InitSphereInertia();
	PhysicsMaterial* enemyPhys;
	if (world->TryGetPhysMat("Player", enemyPhys))co->SetPhysMat(enemyPhys);

	MovementApplierComponent* ma = new MovementApplierComponent(&character->GetTransform(), character->GetPhysicsObject(), 7000.0f);
	SimpleChaseComponent* sc = new SimpleChaseComponent(character);


	ma->SetInputComponent(sc);

	character->AddComponent(ma);
	character->AddComponent(sc);

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddKeyDoorPairToWorld(const Vector3& keyPosition, const Vector3& doorPosition, const Vector4& colour) { //returns the key object
	GameObject* key = new GameObject();
	GameObject* door = new GameObject();
	Vector3 keyScale(0.2f, 0.2f, 0.3f);
	Vector3 doorScale(5, 5,5);

	AABBVolume* keyVolume = new AABBVolume(keyScale);
	key->SetBoundingVolume((CollisionVolume*)keyVolume);

	AABBVolume* doorVolume = new AABBVolume(doorScale);
	door->SetBoundingVolume((CollisionVolume*)doorVolume);

	key->GetTransform()
		.SetPosition(keyPosition)
		.SetScale(keyScale * 2);

	door->GetTransform()
		.SetPosition(doorPosition)
		.SetScale(doorScale * 2);

	key->SetRenderObject(new RenderObject(&key->GetTransform(), cubeMesh, basicTex, basicShader));
	key->SetPhysicsObject(new PhysicsObject(&key->GetTransform(), key->GetBoundingVolume()));
	key->GetRenderObject()->SetColour(colour);

	door->SetRenderObject(new RenderObject(&door->GetTransform(), cubeMesh, basicTex, basicShader));
	door->SetPhysicsObject(new PhysicsObject(&door->GetTransform(), door->GetBoundingVolume(),false)); //door is static
	door->GetRenderObject()->SetColour(colour);

	PhysicsObject* ko = key->GetPhysicsObject();
	ko->SetInverseMass(0.7f);
	ko->InitCubeInertia();

	PhysicsObject* doorO = door->GetPhysicsObject();
	doorO->SetInverseMass(0);
	doorO->InitCubeInertia();

	PhysicsMaterial* keyPhys;
	if (world->TryGetPhysMat("Standard", keyPhys))ko->SetPhysMat(keyPhys);

	PhysicsMaterial* doorPhys;
	if (world->TryGetPhysMat("Standard", doorPhys))doorO->SetPhysMat(doorPhys);

	KeyComponent* kc = new KeyComponent(key, door, world);
	key->AddComponent(kc);

	world->AddGameObject(key);
	world->AddGameObject(door);

	return key;
}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position) {
	StateGameObject* stateObj = new StateGameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	stateObj->SetBoundingVolume((CollisionVolume*)volume);
	stateObj->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	stateObj->SetRenderObject(new RenderObject(&stateObj->GetTransform(), cubeMesh, nullptr, basicShader));
	stateObj->SetPhysicsObject(new PhysicsObject(&stateObj->GetTransform(), stateObj->GetBoundingVolume()));

	PhysicsObject* so = stateObj->GetPhysicsObject();
	so->SetInverseMass(0.5f);
	so->InitSphereInertia();
	PhysicsMaterial* statePhys;
	if (world->TryGetPhysMat("Standard", statePhys))so->SetPhysMat(statePhys);


	world->AddGameObject(stateObj);

	return stateObj;
}


void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -7, 0));
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 0.5f;
	Vector3 cubeDims = Vector3(.5f, .5f, .5f);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims,0.7f);
			}
			else {
				AddSphereToWorld(position, sphereRadius,0.7f);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::Left)) {
			if (selectionObject) {	//set colour to deselected;
				if (selectionObject->GetRenderObject())selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				if(selectionObject->GetRenderObject())selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyCodes::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
				}
			}
		}
	}
	else {
		Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	Debug::Print("Click Force:" + std::to_string(forceMagnitude), Vector2(5, 90));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::Right)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}

void TutorialGame::CreateStaticLevel() {
	std::ifstream infile(Assets::DATADIR + "TestGrid1.txt");
	int nodeSize;
	int gridWidth;
	int gridHeight;
	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;
	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			char type = 0;
			infile >> type;
			if (type == 'r') { //roof
				AddWallToWorld(Vector3((float)(x * nodeSize), nodeSize * 1, (float)(y * nodeSize)), Vector3(nodeSize, nodeSize, nodeSize) * 0.5f);
			}
			else if (type != '.') {
				int intType = type - '0';
				for(int i=0;i<intType;i++)
				AddWallToWorld(Vector3((float)(x * nodeSize), nodeSize*i, (float)(y * nodeSize)), Vector3(nodeSize, nodeSize, nodeSize) * 0.5f);
			}
			//if (type == 'x')AddCubeToWorld(Vector3(x, 0, y), Vector3(1,1,1));
		}
	}
	infile.close();
}


