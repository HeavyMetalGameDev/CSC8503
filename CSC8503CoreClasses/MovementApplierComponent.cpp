#include "C:/Users/c0034428/Documents/CSC8503/CSC8503CoreClasses/CMakeFiles/CSC8503CoreClasses.dir/Debug/cmake_pch.hxx"
#include "MovementApplierComponent.h"

namespace NCL::CSC8503 {
	void MovementApplierComponent::PhysicsUpdate(float dt) {
		Vector3 movementDirection = inputComponent->GetMovementDirection();
		physicsObject->AddForce(movementDirection * dt * forceAmount);
	}
}