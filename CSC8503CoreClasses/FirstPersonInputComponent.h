#pragma once
#include "..\NCLCoreClasses\Camera.h"
#include "MovementInput.h"
#include "Camera.h"
#include "Vector3.h"
namespace NCL::CSC8503 {
	class GameObject;
	class FirstPersonInputComponent : public MovementInput
	{
	public:
		FirstPersonInputComponent(Camera* c) {camera = c; }
		~FirstPersonInputComponent() {}

		void SetCamera(Camera* c) { camera = c; }
		Camera* GetCamera() { return camera; }
		NCL::Maths::Vector3 GetMovementDirection() override;

	protected:
		Camera* camera;
		bool isJumping=false;
	};
}

