#pragma once
#include "Component.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "TriggerComponent.h"
#include "PhysicsMaterial.h"
#include "ClientInputComponent.h"
#include "PhysicsObject.h"
#include "Quaternion.h"
#include "GameServer.h"
#include "NetworkObject.h"
#include "Debug.h"
#include <algorithm>
#include <math.h>
namespace NCL::CSC8503 {
	class ServerPlayerComponent : public Component
	{
	public:
		ServerPlayerComponent(GameObject* g, GameObject* s, TriggerComponent* t, bool* ci,CameraInputStruct* camInput, GameWorld* gw, GameServer* ser) { 
            gameObject = g;
            sphereTrigger = s;
            tc = t;
            server = ser;
            inputs = ci; 
            cameraValues = camInput;
            worldRef = gw;
            tempObjectPhysMat = new PhysicsMaterial();
            tempObjectPhysMat->linearDampHorizontal = 8.0f;
            tempObjectPhysMat->linearDampVertical = 8.0f;
            physObject = g->GetPhysicsObject();
        }
        void Update(float dt)override;
        void PhysicsUpdate(float dt)override;

        void SendInfoPacket();

        void SetPoints(int p) { points = p; }
        void AddPoints(int p) { points += p; }
        int GetPoints() { return points; }

        void SetHealth(int h) { health = h; }
        void ChangeHealth(int h) { health += h; }
        int GetHealth() { return health; }

        void AddPickup() { collectables++; }

        void ApplyForceToObject();
        

	protected:
        void StartGrapple();
        void Jump();
        Vector3 CalculateLookDirection();
        void OnObjectDrop();

        int points=0;
        int health=100;
        int collectables=0;

        GameServer* server;
		GameObject* sphereTrigger;
		TriggerComponent* tc;
		bool* inputs;
        CameraInputStruct* cameraValues;

        GameObject* heldObject; //the object we are holding (if any)
        Vector3 lookDirection;
        float throwForce = 1000.0f;
        PhysicsMaterial* tempObjectPhysMat;
        PhysicsMaterial* physMatCache;
        float dragForce = 200.0f;
        float dragDistance = 0.5f;
        float maxDistance = 3.0f;
        bool hasObject = false;
        bool isEPressed = false;
        bool hasJumped = false;
        bool isGrappling = false;
        bool isGrapplingStatic = false;
        bool hasUnlockedGrapple = true;
        float jumpPower = 15;
        PhysicsObject* physObject;
        GameObject* grappledObject;
        GameWorld* worldRef;
        Vector3 staticGrapplePoint;
        const std::map<int, bool> raycastCollideMap = { {1,true},{2,false},{4,true},{8,false} };

	};
}
