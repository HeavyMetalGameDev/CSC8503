#pragma once
#include "Component.h"
#include "Vector3.h"
namespace NCL::CSC8503 {
	class MovementInput
	{
	public:
		virtual NCL::Maths::Vector3 GetMovementDirection() { return NCL::Maths::Vector3(); } //vector3 of direction to apply force in

	};
}
