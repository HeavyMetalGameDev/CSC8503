#pragma once
#include "Component.h"
#include "GameObject.h"
namespace NCL::CSC8503 {
    class ClientPlayerComponent :  public Component
    {
    public:
        ClientPlayerComponent(GameObject* g, Camera* c) { gameObject = g; camera = c; }
        void Update(float dt)override;
    protected:
        Camera* camera;
    };
}

