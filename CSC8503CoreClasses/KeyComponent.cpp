#include "C:/Users/c0034428/Documents/CSC8503/CSC8503CoreClasses/CMakeFiles/CSC8503CoreClasses.dir/Debug/cmake_pch.hxx"
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