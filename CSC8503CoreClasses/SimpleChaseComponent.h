#pragma once
#include "MovementInput.h"
#include "Transform.h"
namespace NCL::CSC8503 {
	class SimpleChaseComponent : public MovementInput
	{
	public:
		SimpleChaseComponent(GameObject* go) { gameObject = go; }
		void Start(GameWorld* gw)override;

		Vector3 GetMovementDirection() override;

	protected:
		Transform* objectTransform;
		Transform* targetTransform;
	};
}

