#include "StateMachineEnemyComponent.h"

namespace NCL::CSC8503 {
	StateMachineEnemyComponent::StateMachineEnemyComponent(GameObject* g, std::vector<Vector3> pp) {
		gameObject = g;
		counter = 0.0f;
		stateMachine = new StateMachine();
		patrolPoints = pp;
		State* patrolState = new State([&](float dt)->void {this->Patrol(dt); });
		State* chasePlayerState = new State([&](float dt)->void {this->ChasePlayer(dt); });
		State* shootPlayerState = new State([&](float dt)->void {this->ShootPlayer(dt); });

		stateMachine->AddState(patrolState);
		stateMachine->AddState(chasePlayerState);
		stateMachine->AddState(shootPlayerState);

		stateMachine->AddTransition(new StateTransition(patrolState, chasePlayerState, [&]()->bool {return canSeePlayer; })); //chase if can see player--------------------------------------
		stateMachine->AddTransition(new StateTransition(chasePlayerState, shootPlayerState,
			[&]()->bool {
				float distance = (playerObject->GetTransform().GetPosition() - gameObject->GetTransform().GetPosition()).Length();
				return canSeePlayer && distance <= 15.0f; })); //shoot if is close enough
		stateMachine->AddTransition(new StateTransition(shootPlayerState, chasePlayerState, 
			[&]()->bool {
				float distance = (playerObject->GetTransform().GetPosition() - gameObject->GetTransform().GetPosition()).Length(); 
				return !canSeePlayer || distance >= 20.0f; })); //chase if can no longer see player or too far away
		stateMachine->AddTransition(new StateTransition(chasePlayerState, patrolState, [&]()->bool {return !canSeePlayer && losePlayerTimer > 3.0f; })); //patrol if timer runs out
	}

	void StateMachineEnemyComponent::Start(GameWorld* gw) {
		world = gw;
		world->TryGetObjectByTag("Player", playerObject);
	}

	void StateMachineEnemyComponent::Update(float dt) {
		canSeePlayer = CheckForPlayerLOS();
		stateMachine->Update(dt);
	}

	void StateMachineEnemyComponent::ShootPlayer(float dt) {
		moveDirection = Vector3();
		shootTimer += dt;
		if (shootTimer >= 1.0f) {
			shootTimer -= 1.0f;
			PlayerValuesComponent* pvc;
			if (playerObject->TryGetComponent<PlayerValuesComponent>(pvc)) {
				pvc->ChangeHealth(-5);
			}
			std::cout << "SHOOT!" << dt << "\n";
		}
	}

	void StateMachineEnemyComponent::ChasePlayer(float dt) {
		std::cout << "CHASING\n";
		if (canSeePlayer) {
			moveDirection = (playerObject->GetTransform().GetPosition() - gameObject->GetTransform().GetPosition()).Normalised();
			losePlayerTimer = 0;
		}
		else {
			losePlayerTimer += dt;
			//DO PATHFINDING!!!!!!!!!!!!!
		}

	}

	void StateMachineEnemyComponent::Patrol(float dt) {
		Vector3 currentPos = gameObject->GetTransform().GetPosition();
		Vector3 targetPos = patrolPoints[(currentPatrolPoint + 1) % patrolPoints.size()];
		Vector3 delta = targetPos - currentPos;
		delta.y = 0;
		moveDirection = delta.Normalised();
		if (delta.Length() < 0.3f) {
			currentPatrolPoint = (currentPatrolPoint + 1) % patrolPoints.size();
		}

	}

	bool StateMachineEnemyComponent::CheckForPlayerLOS() {
		Vector3 thisPos = gameObject->GetTransform().GetPosition();
		Vector3 playerPos = playerObject->GetTransform().GetPosition();
		Vector3 playerDirection = playerPos - thisPos;
		RayCollision rc;
		Ray ray(gameObject->GetTransform().GetPosition(), playerDirection);
		if (world->Raycast(ray, rc, true,raycastCollideMap, gameObject)) {
			GameObject* hit = (GameObject*)rc.node;
			Debug::DrawLine(gameObject->GetTransform().GetPosition(), rc.collidedAt);
			if (hit->GetTag()=="Player"){
				return true;
			}
		}
		return false;
	}
	Vector3 StateMachineEnemyComponent::GetMovementDirection() {
		moveDirection.y = 0;
		return moveDirection;
	}
}