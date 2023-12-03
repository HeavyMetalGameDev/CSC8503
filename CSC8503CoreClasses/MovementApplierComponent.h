#pragma once
#include "Component.h"
#include "MovementInput.h"
#include "Transform.h"
#include "PhysicsObject.h"
namespace NCL::CSC8503 {
	class MovementApplierComponent : public Component
	{
	public:
		MovementApplierComponent(Transform* t, PhysicsObject* po) { transform = t; physicsObject = po; }
		void PhysicsUpdate(float dt) override;
		void SetInputComponent(MovementInput* ic) { inputComponent = ic; }
		void SetPhysicsObject(PhysicsObject* po) { physicsObject = po; }
	protected:
		Transform* transform;
		PhysicsObject* physicsObject;
		MovementInput* inputComponent;
		float forceAmount;
	};
}
