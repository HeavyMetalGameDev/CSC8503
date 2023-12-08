
#include "SimpleChaseComponent.h"
#include "GameWorld.h"
namespace NCL::CSC8503 {
	void SimpleChaseComponent::Start(GameWorld* gw) {
		GameObject* playerOb;
		bool found = gw->TryGetObjectByTag("Player", playerOb);

		if (found) {
			targetTransform = &playerOb->GetTransform();
		}

		objectTransform = &gameObject->GetTransform();
	}

	NCL::Maths::Vector3 SimpleChaseComponent::GetMovementDirection() {
		Vector3 direction = targetTransform->GetPosition() - objectTransform->GetPosition();
		return direction.Normalised();
	}
}