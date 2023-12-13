#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Debug.h"
namespace NCL::CSC8503 {
    class ClientPlayerComponent :  public Component
    {
    public:
        ClientPlayerComponent() {}
        void Update(float dt)override;

        void SetHealth(int h) { health = h; }
        void SetPoints(int p) { points = p; }
        void SetCollectables(int c) { collectables = c; }
    protected:
        int health=100;
        int points=0;
        int collectables=0;
    };
}

