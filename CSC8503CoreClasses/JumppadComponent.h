#pragma once
#include "Component.h"
#include "GameObject.h"
#include "PhysicsObject.h"
namespace NCL::CSC8503 {
	class JumppadComponent : public Component
	{
		void OnCollisionBegin(GameObject* other)override {
			Vector3 v = other->GetPhysicsObject()->GetLinearVelocity();
			other->GetPhysicsObject()->SetLinearVelocity(Vector3(v.x, 35.0f, v.z));
			std::cout << "COLLISION\n";
		}

	};
}
