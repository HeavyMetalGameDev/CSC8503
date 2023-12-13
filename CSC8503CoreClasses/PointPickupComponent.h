#pragma once
#include "Component.h"
#include "GameObject.h"
#include "PlayerValuesComponent.h"
#include "GameWorld.h"
#include "ServerPlayerComponent.h"
namespace NCL::CSC8503 {
    class PointPickupComponent : public Component
    {
    public:
        PointPickupComponent(GameWorld* gw, GameObject* gameObject, int ptg) { world = gw; this->gameObject = gameObject; pointsToGive = ptg; }

        void OnCollisionBegin(GameObject* other)override;
    protected:
        GameWorld* world;
        int pointsToGive;
    };
}

