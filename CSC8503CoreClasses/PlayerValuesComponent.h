#pragma once
#include "Component.h"
#include "GameWorld.h"
namespace NCL::CSC8503 {
	class TutorialGame;
	class PlayerValuesComponent : public Component
	{
	public:
		PlayerValuesComponent(GameWorld* gw) { world = gw; }
		void SetPoints(int p) { points = p; }
		void AddPoints(int p) { points += p; }
		int GetPoints() { return points; }

		void SetHealth(int h) { health = h; }
		void ChangeHealth(int h) { health += h; }
		int GetHealth() { return health; }

		void AddPickup() { pickups++; }

		void Update(float dt)override;
	protected:
		int points=0;
		int health=100;
		int pickups = 0;
		GameWorld* world;

	};
}

