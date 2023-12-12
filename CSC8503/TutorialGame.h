#include "../NCLCoreClasses/KeyboardMouseController.h"

#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "PhysicsSystem.h"

#include "GameServer.h"
#include "GameClient.h"
#include "NetworkObject.h"

#include "StateGameObject.h"
#include "TestComponent.h"
#include "PushdownMachine.h"
#include "PushdownState.h"

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
#include "StateMachineEnemyComponent.h"
#include "UnlockGrappleComponent.h"
#include "KeyComponent.h"
#include "JumppadComponent.h"
#include "TreasureReturnPointComponent.h"
#include "ClientInputComponent.h"
#include "Assets.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"
#include <fstream>

namespace NCL {
	namespace CSC8503 {
		class TutorialGame : public PacketReceiver		{
		public:
			TutorialGame();
			~TutorialGame();
			void InitWorldSinglePlayer();
			void InitWorldClient();
			void InitWorldServer();

			GameState GetState() { return world->GetState(); }
			void SetState(GameState s) { world->SetState(s); }

			void ReceivePacket(int type, GamePacket* payload, int source = -1)override;

			virtual void UpdateGame(float dt);
			virtual void UpdateGameAsClient(float dt);
			virtual void UpdateGameAsServer(float dt);
		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();



			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);

			void InitDefaultFloor();

			bool SelectObject();
			void MoveSelectedObject();

			

			void BridgeConstraintTest();
			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f, bool isNetworked=false,bool isServerSide = false);
			GameObject* AddCapsuleToWorld(const Vector3& position, float size, float inverseMass = 10.0f, bool isNetworked = false, bool isServerSide = false);
			GameObject* AddPointPickupToWorld(const Vector3& position, int points, bool isNetworked = false, bool isServerSide = false);
			GameObject* AddGrappleUnlockerToWorld(const Vector3& position, bool isNetworked = false, bool isServerSide = false);
			GameObject* AddSphereTriggerToWorld(const Vector3& position, float radius, bool isNetworked = false, bool isServerSide = false);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f, bool isNetworked = false, bool isServerSide = false);
			void AddCubeWallToWorld(const Vector3& position, bool isNetworked = false, bool isServerSide = false);
			GameObject* AddOBBCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f, bool isNetworked = false, bool isServerSide = false);
			GameObject* AddWallToWorld(const Vector3& position, Vector3 dimensions, bool isNetworked = false, bool isServerSide = false);
			GameObject* AddJumppad(const Vector3& position, bool isNetworked = false, bool isServerSide = false);
			GameObject* AddTreasure(const Vector3& position, bool isNetworked = false, bool isServerSide = false);
			GameObject* AddTreasurePoint(const Vector3& position, bool isNetworked = false, bool isServerSide = false);
			void AddRopeToWorld(const Vector3& position, bool isNetworked = false, bool isServerSide = false);

			GameObject* AddPlayerToWorld(const Vector3& position, bool isNetworked = false, bool isServerSide = false);
			GameObject* AddEnemyToWorld(const Vector3& position, std::vector<Vector3>& patrolPoints, bool isNetworked = false, bool isServerSide = false);
			GameObject* AddKeyDoorPairToWorld(const Vector3& keyPosition,const Vector3& doorPosition, const Vector4& colour, bool isNetworked = false, bool isServerSide = false);

			void CreateStaticLevel(bool isNetworked = false, bool isServerSide = false);

#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem*		physics;
			GameWorld*			world;

			KeyboardMouseController controller;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;
			GameObject* player = nullptr;

			Mesh*	capsuleMesh = nullptr;
			Mesh*	cubeMesh	= nullptr;
			Mesh*	sphereMesh	= nullptr;
			Mesh* pickupMesh = nullptr;
			Mesh* coinMesh = nullptr;
			Mesh* gooseMesh = nullptr;

			Texture*	basicTex	= nullptr;
			Shader*		basicShader = nullptr;

			//Coursework Meshes
			Mesh*	charMesh	= nullptr;
			Mesh*	enemyMesh	= nullptr;
			Mesh*	bonusMesh	= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* objClosest = nullptr;
			float gameTimer = 0;
			int totalPickups = 0;

			//networking functionality
			int port;
			GameServer* server;
			GameClient* client;
			
			bool isClient;

			int currentNetworkObjectID = 0;

		};
	}

}

