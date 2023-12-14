#include "BehaviourTreeEnemyComponent.h"
#define DISTANCE_BEFORE_STOLEN 100
#define PULL_TREASURE_AMOUNT 7000.0f
#define CHASE_DISTANCE 800

namespace NCL::CSC8503 {

	BehaviourTreeEnemyComponent::BehaviourTreeEnemyComponent(GameObject* g) {
		// in this behaviour tree, all actions are instantaneous, though I understand that actions can be made to take longer
		colourTimer = 0;
		gameObject = g;
		thisPhys = g->GetPhysicsObject();
		homePoint = g->GetTransform().GetPosition();

		BehaviourAction* pullTreasure = new BehaviourAction("Pull Treasure", [&](float dt, BehaviourState state)->BehaviourState {
			Vector3 pullDirection = (gameObject->GetTransform().GetPosition() - treasure->GetTransform().GetPosition()).Normalised();
			treasurePhys->SetAwake();
			treasurePhys->AddForce(pullDirection * PULL_TREASURE_AMOUNT *dt);
			std::cout << "PULLING!\n";
			return Success;
			}
		);

		BehaviourAction* isTreasureStolen = new BehaviourAction("Is Treasure Stolen", [&](float dt, BehaviourState state)->BehaviourState {
			std::cout << "Checking if stolen...\n";
			if ((treasure->GetTransform().GetPosition() - treasureStartPoint).LengthSquared() > DISTANCE_BEFORE_STOLEN) {
				std::cout << "STOLEN!\n";
				return Success;
			}
			else {
				std::cout << "NOT STOLEN!\n";
				return Failure; 
			}
			}
		);

		BehaviourAction* pulseColours = new BehaviourAction("Pulse Colours", [&](float dt, BehaviourState state)->BehaviourState {
			colourTimer += dt;
			float colourMult = (sin(colourTimer) + 1) * 0.5f;
			Vector4 colour = Debug::RED * colourMult + Debug::BLUE * (1 - colourMult);
			gameObject->GetRenderObject()->SetColour(colour);
			std::cout << "PULSING\n";
			return Success;
			}
		);

		BehaviourAction* stayAtHome = new BehaviourAction("Stay At Home", [&](float dt, BehaviourState state)->BehaviourState {
			Vector3 direction = (homePoint - gameObject->GetTransform().GetPosition()).Normalised();
			thisPhys->AddForce(direction * PULL_TREASURE_AMOUNT * dt);
			std::cout << "STAYING\n";
			return Success;
			}
		);

		BehaviourAction* chaseTreasure = new BehaviourAction("Chase Treasure", [&](float dt, BehaviourState state)->BehaviourState {
			if( (homePoint-treasure->GetTransform().GetPosition()).LengthSquared() >CHASE_DISTANCE)
			return Failure;
			Vector3 direction = (treasure->GetTransform().GetPosition() - gameObject->GetTransform().GetPosition()).Normalised();
			thisPhys->SetAwake();
			thisPhys->AddForce(direction * PULL_TREASURE_AMOUNT * dt);
			std::cout << "CHASING!\n";
			return Success;
			}
		);

		/*
											selection
										/				\
						stolenSequence						staySequence
				/			|			\						|				\
	isTreasureStolen	chaseTreasure	pullTreasure		pulseColours		stayAtHome
		
		*/

		BehaviourSequence* stolenSequence = new BehaviourSequence("Treasure Stolen Sequence");
		stolenSequence->AddChild(isTreasureStolen);
		stolenSequence->AddChild(chaseTreasure);
		stolenSequence->AddChild(pullTreasure);

		BehaviourSequence* staySequence = new BehaviourSequence("Stay Sequence");
		staySequence->AddChild(pulseColours);
		staySequence->AddChild(stayAtHome);

		rootSelector = new BehaviourSelector("Selector");
		rootSelector->AddChild(stolenSequence);
		rootSelector->AddChild(staySequence);
		
		state = Ongoing;

	}

	void BehaviourTreeEnemyComponent::Update(float dt) {
		state = rootSelector->Execute(dt);
		if (state == Success) {
			
			rootSelector->Reset();
		}
		else if (state == Failure) {
			rootSelector->Reset();
		}
	}
}