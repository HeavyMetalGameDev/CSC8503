#pragma once
#include "MovementInput.h"
#include "BehaviourNode.h"
#include "BehaviourAction.h"
#include "BehaviourNodeWithChildren.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "Debug.h"
#include "RenderObject.h"
namespace NCL::CSC8503 {
	class BehaviourTreeEnemyComponent : public MovementInput
	{
	public:
		BehaviourTreeEnemyComponent(GameObject* g);

		void SetTreasureObject(GameObject* t) { treasure = t; treasurePhys = t->GetPhysicsObject(); treasureStartPoint = t->GetTransform().GetPosition();
		}

		void Update(float dt)override;
	protected:
		BehaviourState state;
		PhysicsObject* thisPhys;
		PhysicsObject* treasurePhys;
		Vector3 homePoint;
		Vector3 treasureStartPoint;
		GameObject* treasure;
		BehaviourSelector* rootSelector;
		float colourTimer=0;
	};
}

