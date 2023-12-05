#include "C:/Users/c0034428/Documents/CSC8503/CSC8503CoreClasses/CMakeFiles/CSC8503CoreClasses.dir/Debug/cmake_pch.hxx"
#include "ObjectPickupComponent.h"
#include "Window.h"
#define DEGREES_TO_RAD 0.0174532925f
namespace NCL::CSC8503 {
	void ObjectPickupComponent::Update(float dt) {
		float normYaw = camera->GetYaw() > 180 ? camera->GetYaw() - 360 : camera->GetYaw();
		normYaw = normYaw * DEGREES_TO_RAD;

		//convert from pitch/yaw to directions
		float xDir = cos(normYaw)*cos(camera->GetPitch()*DEGREES_TO_RAD);
		float yDir = sin(camera->GetPitch() * DEGREES_TO_RAD);
		float zDir = sin(-normYaw) * cos(camera->GetPitch() * DEGREES_TO_RAD);

		Vector3 lookDirection(zDir, yDir, -xDir);
	
		triggerObject->GetTransform().SetPosition(gameObject->GetTransform().GetPosition() + (lookDirection*2.5f));

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::E)) {
			isEPressed = true;
		}
		else isEPressed = false;
	}

	void ObjectPickupComponent::PhysicsUpdate(float dt) {
		if (isEPressed) {
			if (!hasObject) {
				if (tc->IsTriggering()) {
					hasObject = true;
					heldObject = tc->GetTriggeredObject();
				}
			}
			else {
				hasObject = false;
			}
		}
		if (hasObject) {
			heldObject->GetTransform().SetPosition(triggerObject->GetTransform().GetPosition());
		}
	}
}