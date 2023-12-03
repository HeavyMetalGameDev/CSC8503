#include "Camera.h"
#include "Window.h"

using namespace NCL;

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float dt) {
	if (!activeController) {
		return;
	}

	//Update the mouse by how much
	pitch	-= activeController->GetNamedAxis("YLook");
	yaw		-= activeController->GetNamedAxis("XLook");

	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw <0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	float frameSpeed = 100 * dt;

	Matrix4 yawRotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

	position += yawRotation * Vector3(0, 0, -activeController->GetNamedAxis("Forward")) * frameSpeed;
	position += yawRotation * Vector3(activeController->GetNamedAxis("Sidestep"), 0, 0) * frameSpeed;

	position.y += activeController->GetNamedAxis("UpDown") * frameSpeed;

}

CameraInputStruct Camera::GetRotationsFromController() {
	CameraInputStruct input;
	input.pitch = pitch - activeController->GetNamedAxis("YLook");
	input.yaw = yaw - activeController->GetNamedAxis("XLook");

	input.pitch = std::min(input.pitch, 90.0f);
	input.pitch = std::max(input.pitch, -90.0f);

	if (input.yaw < 0) {
		input.yaw += 360.0f;
	}
	if (input.yaw > 360.0f) {
		input.yaw -= 360.0f;
	}
	return input;
}

Vector3 Camera::GetMovementFromController() {
	Vector3 input;
	input.z += -activeController->GetNamedAxis("Forward");
	input.x += activeController->GetNamedAxis("Sidestep");
	return input;
}


/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix() const {
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
};

Matrix4 PerspectiveCamera::BuildProjectionMatrix(float currentAspect) const {
	return Matrix4::Perspective(nearPlane, farPlane, currentAspect, fov);
}

Matrix4 OrhographicCamera::BuildProjectionMatrix(float currentAspect) const {
	return Matrix4::Orthographic(left, right, bottom, top, nearPlane, farPlane);
}