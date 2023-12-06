#include "C:/Users/c0034428/Documents/CSC8503/CSC8503CoreClasses/CMakeFiles/CSC8503CoreClasses.dir/Debug/cmake_pch.hxx"
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