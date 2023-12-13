
#include "PointPickupComponent.h"


namespace NCL::CSC8503 {

	void PointPickupComponent::OnCollisionBegin(GameObject* other) {

		if (other->GetTag() == "Player") {
			PlayerValuesComponent* pvc;
			ServerPlayerComponent* spc;
			if (other->TryGetComponent<PlayerValuesComponent>(pvc)) {
				pvc->AddPoints(pointsToGive);
				pvc->AddPickup();
				gameObject->SetActive(false);
				world->RemoveGameObject(gameObject);
				
			}
			else if(other->TryGetComponent<ServerPlayerComponent>(spc)){
				spc->AddPoints(pointsToGive);
				spc->AddPickup();
				spc->SendInfoPacket();
				gameObject->SetActive(false);
				world->RemoveGameObject(gameObject);
			}
		}
	}
}