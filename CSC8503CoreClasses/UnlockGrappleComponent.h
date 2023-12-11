#pragma once
#include "Component.h"
#include "GameObject.h"
#include "PlayerInputComponent.h"
#include "Quaternion.h"
namespace NCL::CSC8503 {
	class UnlockGrappleComponent : public Component
	{
	public:
		void OnCollisionBegin(GameObject* go)override;
	protected:
		bool activated = true;
		float displayTextTime=0;
	};
}
