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
		State* returnToPatrolState = new State([&](float dt)->void {this->ReturnToPatrol(dt); });

		stateMachine->AddState(patrolState);
		stateMachine->AddState(chasePlayerState);
		stateMachine->AddState(shootPlayerState);

		stateMachine->AddTransition(new StateTransition(patrolState, chasePlayerState, [&]()->bool {return canSeePlayer; })); //chase if can see player--------------------------------------
		stateMachine->AddTransition(new StateTransition(chasePlayerState, shootPlayerState,
			[&]()->bool {
				float distance = (playerObject->GetTransform().GetPosition() - gameObject->GetTransform().GetPosition()).Length();
				return canSeePlayer && distance <= 25.0f; })); //shoot if is close enough
		stateMachine->AddTransition(new StateTransition(shootPlayerState, chasePlayerState,
			[&]()->bool {
				float distance = (playerObject->GetTransform().GetPosition() - gameObject->GetTransform().GetPosition()).Length();
				return !canSeePlayer || distance >= 20.0f; })); //chase if can no longer see player or too far away
		stateMachine->AddTransition(new StateTransition(chasePlayerState, returnToPatrolState, [&]()->bool {return !canSeePlayer && losePlayerTimer > 7.0f; })); //return to patrol if timer runs out
		stateMachine->AddTransition(new StateTransition(returnToPatrolState, patrolState,
			[&]()->bool {
				float distance = (patrolPoints[currentPatrolPoint] - gameObject->GetTransform().GetPosition()).Length();
				return distance < 2.0f; })); //patrol if close to patrol point
		stateMachine->AddTransition(new StateTransition(returnToPatrolState, chasePlayerState,
			[&]()->bool {return canSeePlayer; })); //chase player if we spot them while returning

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
		if (canSeePlayer) {
			moveDirection = (playerObject->GetTransform().GetPosition() - gameObject->GetTransform().GetPosition());
			moveDirection.y = 0;
			moveDirection.Normalise();
			losePlayerTimer = 0;
			invalidPath = true;
		}
		else {
			losePlayerTimer += dt;
			if (invalidPath) {

				nodePath = world->GetPath(gameObject->GetTransform().GetPosition(), playerObject->GetTransform().GetPosition());
				if (nodePath.empty()) {

					moveDirection = Vector3();
					return;
				}
				currentPathfindingNode = 0;
				invalidPath = false;
			}

			if ((nodePath.back() - playerObject->GetTransform().GetPosition()).Length() > 10.0f) { // if player has moved to a different node
				invalidPath = true;
				moveDirection = Vector3();
				return;
			}
			if (currentPathfindingNode + 1 > nodePath.size())return;
			Vector3 nextNode = nodePath[currentPathfindingNode + 1];
			moveDirection = nextNode - gameObject->GetTransform().GetPosition();
			moveDirection.y = 0;
			moveDirection.Normalise();
			Vector3 posNoY = gameObject->GetTransform().GetPosition();
			posNoY.y = 0;
			if ((nextNode - posNoY).Length() <= 2.0f) {
				currentPathfindingNode += 1;
				moveDirection = Vector3();
			}

		}

	}
	void StateMachineEnemyComponent::ReturnToPatrol(float dt) {
		if (invalidPath) {
			nodePath = world->GetPath(gameObject->GetTransform().GetPosition(), patrolPoints[currentPatrolPoint]);
			if (nodePath.empty()) {

				moveDirection = Vector3();
				return;
			}
			currentPathfindingNode = 0;
			invalidPath = false;
		}
		if (currentPathfindingNode + 1 > nodePath.size())return;
		Vector3 nextNode = nodePath[currentPathfindingNode + 1];
		moveDirection = nextNode - gameObject->GetTransform().GetPosition();
		moveDirection.y = 0;
		moveDirection.Normalise();
		Vector3 posNoY = gameObject->GetTransform().GetPosition();
		posNoY.y = 0;
		if ((nextNode - posNoY).Length() <= 1.5f) {
			currentPathfindingNode += 1;
			moveDirection = Vector3();
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
		Vector3 playerDirection = (playerPos - thisPos).Normalised();
		RayCollision rc;
		Ray ray(thisPos, playerDirection);
		if (world->Raycast(ray, rc, true, raycastCollideMap, gameObject)) {
			GameObject* hit = (GameObject*)rc.node;
			if (hit->GetTag() == "Player") {
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