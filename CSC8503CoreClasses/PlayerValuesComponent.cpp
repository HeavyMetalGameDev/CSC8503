
#include "PlayerValuesComponent.h"
#include "Debug.h"


namespace NCL::CSC8503 {
	void PlayerValuesComponent::Update(float dt) {
		Debug::Print("Points: " + std::to_string(points), Vector2(75, 5), Debug::WHITE);
		Debug::Print("HP: " + std::to_string(health), Vector2(5, 95), Debug::RED);
		Debug::Print("Collectables: " + std::to_string(pickups), Vector2(60, 10), Debug::YELLOW);

		if (health <= 0) {
			health = 0;
			world->LoseGame();
		}
	}
}
