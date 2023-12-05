#pragma once

namespace NCL::CSC8503 {
	class GameObject;
	class GameWorld;
	class Component
	{
	public:
		Component() { };
		~Component() {};
		Component(GameObject* gameObject) { this->gameObject = gameObject;}
		virtual void Update(float dt) {};
		virtual void PhysicsUpdate(float dt) {};
		virtual void Start(GameWorld* gw) {};
		virtual void OnCollisionBegin(GameObject* otherObject) {};
		virtual void OnCollisionStay(GameObject* otherObject) {};
		virtual void OnCollisionEnd(GameObject* otherObject) {};

	protected:
		GameObject* gameObject;
	};
}

