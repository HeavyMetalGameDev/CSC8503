#pragma once
#include "StateMachine.h"
#include "State.h"
#include "StateTransition.h"
#include "GameWorld.h"
#include "MovementInput.h"
#include "Vector3.h"
#include "PlayerValuesComponent.h"
#include "NavigationGrid.h"
#include "NavigationMap.h"
#include <iostream>
#include <vector>
namespace NCL::CSC8503 {
	class StateMachineEnemyComponent : public MovementInput
	{
	public:
		StateMachineEnemyComponent(GameObject* g, std::vector<Vector3> pp);
		~StateMachineEnemyComponent() {};
		void Update(float dt)override;

		void ChasePlayer(float dt);
		void ShootPlayer(float dt);
		void Patrol(float dt);
		void ReturnToPatrol(float dt);

		bool CheckForPlayerLOS();



		Vector3 GetMovementDirection() override;
		void Start(GameWorld* gw)override;

	protected:
		GameWorld* world;
		float counter=0;
		int damage = 5;
		GameObject* playerObject;
		StateMachine* stateMachine;
		bool canSeePlayer=false;
		bool invalidPath = true;
		float losePlayerTimer=0;
		float shootTimer=0;

		std::vector<Vector3> patrolPoints;
		int currentPatrolPoint = 0;

		std::vector<Vector3> nodePath;
		int currentPathfindingNode = 0;
		Vector3 moveDirection;
		const std::map<int, bool> raycastCollideMap = { {1,true},{2,true},{4,true},{8,false} };
	};
}

