#pragma once
#include <random>

#include "Ray.h"
#include "CollisionDetection.h"
#include "QuadTree.h"
#include "NavigationGrid.h"
#include "NavigationPath.h"

namespace NCL {
		class Camera;
		using Maths::Ray;
	namespace CSC8503 {
		class GameObject;
		class Constraint;
		enum GameState {
			STATE_MENU,STATE_PLAYING, STATE_WIN, STATE_LOSE
		};

		typedef std::function<void(GameObject*)> GameObjectFunc;
		typedef std::vector<GameObject*>::const_iterator GameObjectIterator;

		class GameWorld	{
		public:
			GameWorld();
			~GameWorld();

			void Clear();
			void ClearAndErase();

			void AddGameObject(GameObject* o);
			void RemoveGameObject(GameObject* o, bool andDelete = false);

			void AddConstraint(Constraint* c);
			void RemoveConstraint(Constraint* c, bool andDelete = false);

			PerspectiveCamera& GetMainCamera()  {
				return mainCamera;
			}

			GameState GetState(){return state;}
			void ShuffleConstraints(bool state) {
				shuffleConstraints = state;
			}

			void ShuffleObjects(bool state) {
				shuffleObjects = state;
			}

			void SetState(GameState s) { state = s; }
			void WinGame() { state = STATE_WIN; }
			void LoseGame() { state = STATE_LOSE; }

			bool Raycast(Ray& r, RayCollision& closestCollision, bool closestObject = false, GameObject* ignore = nullptr) const;
			bool Raycast(Ray& r, RayCollision& closestCollision, bool closestObject, const std::map<int, bool> raycastCollideMap, GameObject* ignore) const;
			virtual void UpdateWorld(float dt);
			virtual void StartWorld();
			virtual void UpdateWorldPhysics(float dt);

			void OperateOnContents(GameObjectFunc f);

			bool TryGetObjectByTag(std::string t, GameObject*& out);

			void GetObjectIterators(
				GameObjectIterator& first,
				GameObjectIterator& last) const;

			void GetConstraintIterators(
				std::vector<Constraint*>::const_iterator& first,
				std::vector<Constraint*>::const_iterator& last) const;

			int GetWorldStateID() const {
				return worldStateCounter;
			}

			void InitPhysicsMaterials();

			bool TryGetPhysMat(std::string name, PhysicsMaterial*& physMat);

			std::vector<Vector3> GetPath(Vector3 start, Vector3 end);

		protected:
			std::vector<GameObject*> gameObjects;
			std::vector<Constraint*> constraints;
			std::map<std::string, PhysicsMaterial> physicsMaterials;

			PerspectiveCamera mainCamera;

			GameState state = STATE_PLAYING;
			bool shuffleConstraints;
			bool shuffleObjects;
			int		worldIDCounter;
			int		worldStateCounter;
			const std::map<int, bool> defaultRaycastMap = { {1,true},{2,true},{4,true},{8,false} };
			NavigationGrid* grid;
		};
	}
}

