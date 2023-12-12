#include "ClientInputComponent.h"
namespace NCL::CSC8503 {
	void ClientInputComponent::Update(float dt) {
		buttoninputs[0] = Window::GetKeyboard()->KeyDown(KeyCodes::W);
		buttoninputs[1] = Window::GetKeyboard()->KeyDown(KeyCodes::A);
		buttoninputs[2] = Window::GetKeyboard()->KeyDown(KeyCodes::S);
		buttoninputs[3] = Window::GetKeyboard()->KeyDown(KeyCodes::D);
		buttoninputs[4] = Window::GetKeyboard()->KeyDown(KeyCodes::SPACE);
		buttoninputs[5] = Window::GetMouse()->ButtonDown(MouseButtons::Left);
		buttoninputs[6] = Window::GetMouse()->ButtonDown(MouseButtons::Right);
		buttoninputs[7] = Window::GetKeyboard()->KeyDown(KeyCodes::E);

		CameraInputStruct inp = camera->GetRotationsFromController();
		cameraYaw = inp.yaw;
		cameraPitch = inp.pitch;
	}
}