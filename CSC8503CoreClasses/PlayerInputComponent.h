#pragma once
#include "Component.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
namespace NCL::CSC8503 {
    class PlayerInputComponent : public Component
    {
    public:
        PlayerInputComponent(GameObject* go) { gameObject = go; }
        void Start(GameWorld* gw)override { worldRef = gw; physObject = gameObject->GetPhysicsObject(); };
        virtual void Update(float dt) override;
        void SetJumpPower(float jp) { jumpPower = jp; }

    protected:
        float jumpPower = 15;
        GameWorld* worldRef;
        PhysicsObject* physObject;
    };
}

