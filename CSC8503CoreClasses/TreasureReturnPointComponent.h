#pragma once
#include "Component.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "GameWorld.h"
namespace NCL::CSC8503 {
	class TreasureReturnPointComponent : public Component
	{
	public:

		TreasureReturnPointComponent(GameWorld* gw) {
			world = gw;
		}
		void OnCollisionBegin(GameObject* other)override {
			if (other->GetTag() == "Treasure") {
				world->WinGame();
			}
		}
	protected:
		GameWorld* world;

	};
}
