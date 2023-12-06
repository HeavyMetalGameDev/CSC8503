#pragma once
#include "Component.h"
#include "GameObject.h"
#include "TriggerComponent.h"
#include "PhysicsMaterial.h"
namespace NCL::CSC8503 {
    class ObjectPickupComponent : public Component
    {
    public:
        ObjectPickupComponent(GameObject* gameObject, GameObject* triggerObject, Camera* camera, TriggerComponent* tc) 
        {
            this->triggerObject = triggerObject, this->gameObject = gameObject; this->camera = camera; this->tc = tc;
            tempObjectPhysMat = new PhysicsMaterial();
            tempObjectPhysMat->linearDampHorizontal = 8.0f;
            tempObjectPhysMat->linearDampVertical = 8.0f;
        };
        ~ObjectPickupComponent() { delete tempObjectPhysMat; }

        void Update(float dt)override;
        void PhysicsUpdate(float dt) override;

        void ApplyForceToObject();
        //void OnObjectPickup();
        void OnObjectDrop();
        //void OnObjectThrow();
    protected:
        GameObject* triggerObject; // the object used to check for picking up objects
        GameObject* heldObject; //the object we are holding (if any)
        TriggerComponent* tc;
        Vector3 lookDirection;
        Camera* camera;
        float throwForce = 1000.0f;
        PhysicsMaterial* tempObjectPhysMat;
        PhysicsMaterial* physMatCache;
        float dragForce = 200.0f;
        float dragDistance=0.5f;
        float maxDistance = 3.0f;
        bool hasObject=false;
        bool isEPressed = false;
    };
}
