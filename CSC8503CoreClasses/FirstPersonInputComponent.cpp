
#include "FirstPersonInputComponent.h"

namespace NCL::CSC8503 {
	Vector3 FirstPersonInputComponent::GetMovementDirection() {
		CameraInputStruct inputRotations = camera->GetRotationsFromController();
		Vector3 inputMovement = camera->GetMovementFromController();
		Matrix4 yawRotation = Matrix4::Rotation(inputRotations.yaw, Vector3(0, 1, 0));

		Vector3 direction;
		direction += yawRotation * Vector3(0, 0, inputMovement.z);
		direction += yawRotation * Vector3(inputMovement.x, 0, 0);

		return direction;
	}
}

