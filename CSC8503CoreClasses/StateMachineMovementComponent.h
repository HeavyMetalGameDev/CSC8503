#pragma once
#include "Component.h"
#include "StateMachine.h"
#include "MovementInput.h"
namespace NCL::CSC8503 {
	class StateMachineMovementComponent : public MovementInput
	{
	public:
	protected:
		StateMachine stateMachine;
	};
}

