#pragma once
#include "Component.h"
namespace NCL::CSC8503 {
	class PlayerValuesComponent : public Component
	{
	public:
		void SetPoints(int p) { points = p; }
		void AddPoints(int p) { points += p; }
		int GetPoints() { return points; }

		void SetHealth(int h) { health = h; }
		void ChangeHealth(int h) { health += h; }
		int GetHealth() { return health; }

		void Update(float dt)override;
	protected:
		int points=0;
		int health=100;

	};
}

