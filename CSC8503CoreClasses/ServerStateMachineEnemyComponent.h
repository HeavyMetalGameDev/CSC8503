#pragma once
#include "Component.h"
#include "GameObject.h"
#include "StateMachine.h"
#include "State.h"
#include "StateTransition.h"
#include "PlayerValuesComponent.h"
#include "GameWorld.h"
#include "MovementInput.h"
#include "GameServer.h"
#include "ServerPlayerComponent.h"
#include <map>
#include <vector>
namespace NCL::CSC8503 {
	class ServerStateMachineEnemyComponent : public MovementInput
	{
	public:
		ServerStateMachineEnemyComponent(GameObject* g, std::vector<Vector3> pp, GameWorld* gw);
		~ServerStateMachineEnemyComponent() {};
		void Update(float dt)override;

		void ChasePlayer(float dt);
		void ShootPlayer(float dt);
		void Patrol(float dt);
		void ReturnToPatrol(float dt);

		bool CheckForPlayerLOS();

		void SetPlayerVector(std::vector<GameObject*>* v) { playerObjects = v; }

		Vector3 GetMovementDirection() override;

	protected:
		GameWorld* world;
		float counter = 0;
		int damage = 5;
		std::vector<GameObject*>* playerObjects;
		GameObject* currentPlayer;
		StateMachine* stateMachine;
		bool canSeePlayer = false;
		bool invalidPath = true;
		float losePlayerTimer = 0;
		float shootTimer = 0;

		std::vector<Vector3> patrolPoints;
		int currentPatrolPoint = 0;

		std::vector<Vector3> nodePath;
		int currentPathfindingNode = 0;
		Vector3 moveDirection;
		const std::map<int, bool> raycastCollideMap = { {1,true},{2,true},{4,true},{8,false} };
	};
}

