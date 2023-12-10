#pragma once
#include "Component.h"
#include "StateMachine.h"
#include "MovementInput.h"
namespace NCL::CSC8503 {
	class StateMachineMovementComponent : public MovementInput
	{
	public:
		StateMachineMovementComponent(GameWorld* gw, GameObject* gameObject);

	protected:
		StateMachine stateMachine;
	};
}

