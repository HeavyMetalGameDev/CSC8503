#pragma once

namespace NCL::CSC8503 {
	class GameObject;
	class Component
	{
	public:
		Component(GameObject* gameObject) { this->gameObject = gameObject;}
		virtual void Update(float dt) {};
		virtual void OnCollisionBegin(GameObject* otherObject);
		virtual void OnCollisionEnd(GameObject* otherObject);

	protected:
		GameObject* gameObject;
	};
}
