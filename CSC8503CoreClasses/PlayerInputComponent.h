#pragma once
#include "GameObject.h"
#include "Component.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
namespace NCL::CSC8503 {
    class PlayerInputComponent : public Component
    {
    public:
        PlayerInputComponent(GameObject* go, Camera* c) { gameObject = go; camera = c;}
        void Start(GameWorld* gw)override { worldRef = gw; physObject = gameObject->GetPhysicsObject(); };
        virtual void Update(float dt) override;
        virtual void PhysicsUpdate(float dt) override;
        void SetJumpPower(float jp) { jumpPower = jp; }
        void UnlockGrapple() { hasUnlockedGrapple = true; }

    protected:
        Camera* camera;
        bool hasJumped=false;
        bool isGrappling = false;
        bool isGrapplingStatic = false;
        bool hasUnlockedGrapple = true;
        float jumpPower = 15;
        GameWorld* worldRef;
        PhysicsObject* physObject;
        GameObject* grappledObject;
        Vector3 staticGrapplePoint;
        const std::map<int, bool> raycastCollideMap = { {1,true},{2,false},{4,true},{8,false} };
    };
}

