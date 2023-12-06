#include "C:/Users/c0034428/Documents/CSC8503/CSC8503CoreClasses/CMakeFiles/CSC8503CoreClasses.dir/Debug/cmake_pch.hxx"
#include "PlayerValuesComponent.h"
#include "Debug.h"

namespace NCL::CSC8503 {
	void PlayerValuesComponent::Update(float dt) {
		Debug::Print("Points: " + std::to_string(points), Vector2(75, 5), Debug::WHITE);
		Debug::Print("HP: " + std::to_string(health), Vector2(5, 95), Debug::RED);
	}
}
