
#include "ObjectPickupComponent.h"
#include "PhysicsObject.h"
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

		lookDirection  = Vector3(zDir, yDir, -xDir);
	
		triggerObject->GetTransform().SetPosition(gameObject->GetTransform().GetPosition() + (lookDirection*2.5f));

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::E)) {
			isEPressed = true;
		}
		else isEPressed = false;

		if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::Left) && hasObject) {
			OnObjectDrop();
			heldObject->GetPhysicsObject()->AddForce(lookDirection * throwForce);
		}
	}

	void ObjectPickupComponent::PhysicsUpdate(float dt) {
		if (isEPressed) {
			if (!hasObject) {
				if (tc->IsTriggering()) {
					if (tc->GetTriggeredObject()->GetPhysicsObject()->IsDynamic()) {
						hasObject = true;
						heldObject = tc->GetTriggeredObject();
						heldObject->GetTransform().SetPosition(triggerObject->GetTransform().GetPosition());
						physMatCache = heldObject->GetPhysicsObject()->GetPhysMat();
						heldObject->GetPhysicsObject()->SetPhysMat(tempObjectPhysMat);
					}
				}
			}
			else {
				OnObjectDrop();
			}
		}
		if (hasObject) {
			ApplyForceToObject();
		}
	}

	void ObjectPickupComponent::ApplyForceToObject() {
		Vector3 objectDirection = triggerObject->GetTransform().GetPosition() - heldObject->GetTransform().GetPosition();
		float distance = objectDirection.Length();
		if (distance > maxDistance) {
			OnObjectDrop();
			return;
		}
		objectDirection.Normalise();
		float dragRatio = std::min(distance / dragDistance,1.0f);
		Vector3 objectForce = objectDirection * (dragRatio * dragForce);
		//heldObject->GetPhysicsObject()->ClearForces();
		heldObject->GetPhysicsObject()->SetAwake();
		heldObject->GetPhysicsObject()->AddForce(objectForce);

		heldObject->GetPhysicsObject()->SetAngularVelocity(Vector3());

		if(heldObject->GetBoundingVolume()->type != VolumeType::AABB) //dont rotate AABBs
			heldObject->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(camera->GetPitch(), camera->GetYaw(), 0));
	}

	void ObjectPickupComponent::OnObjectDrop() {
		hasObject = false;
		heldObject->GetPhysicsObject()->SetAwake();
		heldObject->GetPhysicsObject()->SetPhysMat(physMatCache);
	}
}