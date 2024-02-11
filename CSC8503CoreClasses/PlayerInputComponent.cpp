
#include "PlayerInputComponent.h"
#include "Window.h"
#include "Ray.h"
#define DEGREES_TO_RAD 0.0174532925f
#define STATIC_FORCE 200.0f
#define DYNAMIC_FORCE 100.0f

namespace NCL::CSC8503{

	void PlayerInputComponent::Update(float dt) {
		Debug::Print(".", { 49,50 }); //crosshair
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE)) {
            Jump();
		}
		if (Window::GetMouse()->ButtonPressed(MouseButtons::Right) && !isGrappling) {
            BeginGrapple();
		}
		else if (!Window::GetMouse()->ButtonHeld(MouseButtons::Right) && isGrappling) {
			isGrappling = false;
			isGrapplingStatic = false;
		}
	}

    void PlayerInputComponent::BeginGrapple()
    {
        Vector3 lookDir = CalculateLookDirection();
        Ray ray(gameObject->GetTransform().GetPosition() + Vector3(0, 0.3f, 0), lookDir);
        RayCollision rc;
        if (worldRef->Raycast(ray, rc, true, raycastCollideMap, gameObject)) {
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

    void PlayerInputComponent::Jump()
    {
        Ray ray(gameObject->GetTransform().GetPosition(), { 0,-1,0 });
        RayCollision rc;
        if (worldRef->Raycast(ray, rc, true, raycastCollideMap, gameObject)) {
            if ((rc.collidedAt - gameObject->GetTransform().GetPosition()).Length() < 2.0f) {
                hasJumped = true;
            }
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
                GrappleStatic();
			}
			else {
                GrappleDynamicObject();
			}
		}
	}

    void PlayerInputComponent::GrappleStatic()
    {
        Debug::DrawLine(gameObject->GetTransform().GetPosition(), staticGrapplePoint);
        if (hasUnlockedGrapple) {
            Vector3 forceDirection = staticGrapplePoint - gameObject->GetTransform().GetPosition();
            forceDirection.Normalise();
            forceDirection.y *= 0.3f; //this is because the player has less y damping so the force needs to be lower
            physObject->AddForce(forceDirection * STATIC_FORCE);
        }
        else {
            Debug::Print("Grapple Not Strong Enough!", { 30,80 });
        }
    }

    void PlayerInputComponent::GrappleDynamicObject()
    {
        Debug::DrawLine(grappledObject->GetTransform().GetPosition(), gameObject->GetTransform().GetPosition());
        Vector3 forceDirection = gameObject->GetTransform().GetPosition() - grappledObject->GetTransform().GetPosition();
        forceDirection.Normalise();
        grappledObject->GetPhysicsObject()->SetAwake();
        grappledObject->GetPhysicsObject()->AddForce(forceDirection * DYNAMIC_FORCE);
    }

    Vector3 PlayerInputComponent::CalculateLookDirection() {
        float camYaw = camera->GetYaw();
        float camPitch = camera->GetPitch();
        float normYaw = camYaw > 180 ? camYaw - 360 : camYaw; //get yaw between -180 and 180
        normYaw = normYaw * DEGREES_TO_RAD;

        //convert from pitch/yaw to directions
        float xDir = cos(normYaw) * cos(camPitch * DEGREES_TO_RAD);
        float yDir = sin(camPitch * DEGREES_TO_RAD);
        float zDir = sin(-normYaw) * cos(camPitch * DEGREES_TO_RAD);

        return Vector3(zDir, yDir, -xDir);
    }
}