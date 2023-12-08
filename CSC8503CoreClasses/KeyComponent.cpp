
#include "KeyComponent.h"
#include "GameWorld.h"
namespace NCL::CSC8503 {
	void KeyComponent::OnCollisionBegin(GameObject* other) {
		if (other == door) {
			world->RemoveGameObject(door);
			world->RemoveGameObject(gameObject);
		}
	}
}