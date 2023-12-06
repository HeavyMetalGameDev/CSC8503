#pragma once
#include "Component.h"
#include "GameObject.h"
#include "PhysicsObject.h"
namespace NCL::CSC8503 {
	class TriggerComponent : public Component
	{
	public:
		void OnCollisionStay(GameObject* go)override { isTriggering = true;}
		void OnCollisionBegin(GameObject* go)override { triggeredObject = go; }
		void OnCollisionEnd(GameObject* go)override { isTriggering = false; triggeredObject = nullptr; }
		bool IsTriggering() {return isTriggering;}
		GameObject* GetTriggeredObject() { return triggeredObject; }
	protected:
		bool isTriggering;
		GameObject* triggeredObject;
	};
}
