#pragma once
#include "Component.h"
namespace NCL::CSC8503 {
    class KeyComponent : public Component
    {
    public:
        KeyComponent(GameObject* keyObj, GameObject* doorObj, GameWorld* gw) { gameObject = keyObj; door = doorObj; world = gw; }
        void OnCollisionBegin(GameObject* other)override;
    protected:
        GameObject* door;
        GameWorld* world;
    };
}
