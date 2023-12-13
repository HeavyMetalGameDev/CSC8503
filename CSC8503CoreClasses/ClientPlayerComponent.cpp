#include "ClientPlayerComponent.h"

namespace NCL::CSC8503 {
	void ClientPlayerComponent::Update(float dt) {
		Debug::Print("Points: " + std::to_string(points), Vector2(75, 5), Debug::WHITE);
		Debug::Print("HP: " + std::to_string(health), Vector2(5, 95), Debug::RED);
		Debug::Print("Collectables: " + std::to_string(collectables), Vector2(60, 10), Debug::YELLOW);
	}
}