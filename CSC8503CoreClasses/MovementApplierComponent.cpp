
#include "MovementApplierComponent.h"

namespace NCL::CSC8503 {
	void MovementApplierComponent::PhysicsUpdate(float dt) {
		Vector3 movementDirection = inputComponent->GetMovementDirection();
		physicsObject->AddForce(movementDirection * dt * forceAmount);
		physicsObject->SetAwake();
	}
}