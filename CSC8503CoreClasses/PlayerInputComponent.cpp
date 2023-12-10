
#include "PlayerInputComponent.h"
#include "Window.h"
#include "Ray.h"
#define DEGREES_TO_RAD 0.0174532925f

namespace NCL::CSC8503{

	void PlayerInputComponent::Update(float dt) {
		Debug::Print(".", { 49,50 });
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE)) {
			Ray ray(gameObject->GetTransform().GetPosition(), { 0,-1,0 });
			RayCollision rc;
			if (worldRef->Raycast(ray, rc, true, gameObject)) {
				if ((rc.collidedAt - gameObject->GetTransform().GetPosition()).Length() < 2.0f) {
					hasJumped = true;
				}
			}
		}
		if (Window::GetMouse()->ButtonPressed(MouseButtons::Right) && !isGrappling) {
			float normYaw = camera->GetYaw() > 180 ? camera->GetYaw() - 360 : camera->GetYaw(); //get yaw between -180 and 180
			normYaw = normYaw * DEGREES_TO_RAD;

			//convert from pitch/yaw to directions
			float xDir = cos(normYaw) * cos(camera->GetPitch() * DEGREES_TO_RAD);
			float yDir = sin(camera->GetPitch() * DEGREES_TO_RAD);
			float zDir = sin(-normYaw) * cos(camera->GetPitch() * DEGREES_TO_RAD);

			Ray ray(gameObject->GetTransform().GetPosition() + Vector3(0,0.3f,0), Vector3(zDir, yDir, -xDir));
			RayCollision rc;
			if (worldRef->Raycast(ray, rc, true, gameObject)) {
				if (((GameObject*)rc.node)->GetPhysicsObject()->IsDynamic()) {
					grappledObject = (GameObject*)rc.node;
					isGrapplingStatic = false;
				}
				else {
					staticGrapplePoint = rc.collidedAt;
					isGrapplingStatic = true;
				}
				isGrappling = true;
			}
		}
		else if (!Window::GetMouse()->ButtonHeld(MouseButtons::Right) && isGrappling) {
			isGrappling = false;
			isGrapplingStatic = false;
		}
	}

	void PlayerInputComponent::PhysicsUpdate(float dt) {
		if (hasJumped) {
			gameObject->GetTransform().SetPosition(gameObject->GetTransform().GetPosition() + Vector3(0, .1f, 0));
			physObject->ApplyLinearImpulse({ 0,jumpPower,0 });
		}
		hasJumped = false;

		if (isGrappling) {
			if (isGrapplingStatic) {
				Debug::DrawLine(gameObject->GetTransform().GetPosition(), staticGrapplePoint);
				if (hasUnlockedGrapple) {
					Vector3 forceDirection = staticGrapplePoint - gameObject->GetTransform().GetPosition();
					forceDirection.Normalise();
					physObject->AddForce(forceDirection * 200.0f);
				}
				else {
					Debug::Print("Grapple Not Strong Enough!", { 30,80 });
				}
			}
			else {
				Debug::DrawLine(grappledObject->GetTransform().GetPosition(), gameObject->GetTransform().GetPosition());
				Vector3 forceDirection = gameObject->GetTransform().GetPosition() - grappledObject->GetTransform().GetPosition();
				forceDirection.Normalise();
				grappledObject->GetPhysicsObject()->SetAwake();
				grappledObject->GetPhysicsObject()->AddForce(forceDirection * 100.0f);
			}
		}
	}
}