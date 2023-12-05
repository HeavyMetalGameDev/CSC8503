#pragma once
#include "Component.h"
#include "GameObject.h"
#include "TriggerComponent.h"
namespace NCL::CSC8503 {
    class ObjectPickupComponent : public Component
    {
    public:
        ObjectPickupComponent(GameObject* gameObject, GameObject* triggerObject, Camera* camera, TriggerComponent* tc) 
        {
            this->triggerObject = triggerObject, this->gameObject = gameObject; this->camera = camera; this->tc = tc;
        };

        void Update(float dt)override;
        void PhysicsUpdate(float dt) override;

        //void OnObjectPickup();
        //void OnObjectDrop();
        //void OnObjectThrow();
    protected:
        GameObject* triggerObject; // the object used to check for picking up objects
        GameObject* heldObject; //the object we are holding (if any)
        TriggerComponent* tc;
        Camera* camera;
        float throwForce;
        float dragForce;
        float dragDistance;
        bool hasObject=false;
        bool isEPressed = false;
    };
}
