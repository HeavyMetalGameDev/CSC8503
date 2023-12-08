
#include "PointPickupComponent.h"


namespace NCL::CSC8503 {

	void PointPickupComponent::OnCollisionBegin(GameObject* other) {

		if (other->GetTag() == "Player") {
			PlayerValuesComponent* pvc;
			if (other->TryGetComponent<PlayerValuesComponent>(pvc)) {
				pvc->AddPoints(pointsToGive);
				gameObject->SetActive(false);
				world->RemoveGameObject(gameObject);
				
			}
		}
	}
}