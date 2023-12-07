#pragma once
#include "Component.h"
#include "GameObject.h"
#include "PhysicsObject.h"
namespace NCL::CSC8503 {
	class TriggerComponent : public Component
	{
	public:
		void OnCollisionBegin(GameObject* go)override { triggeredObject = go; isTriggering = true;}
		void OnCollisionEnd(GameObject* go)override { isTriggering = false; triggeredObject = nullptr; }
		bool IsTriggering() {return isTriggering;}
		GameObject* GetTriggeredObject() { return triggeredObject; }
	protected:
		bool isTriggering;
		GameObject* triggeredObject;
	};
}
