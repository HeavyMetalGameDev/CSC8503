#include "UnlockGrappleComponent.h"

namespace NCL::CSC8503 {
	void UnlockGrappleComponent::OnCollisionBegin(GameObject* other) {
		if (other->GetTag() == "Player") {
			PlayerInputComponent* pic;
			if (other->TryGetComponent<PlayerInputComponent>(pic)) {
				pic->UnlockGrapple();
			}
		}
	}
}