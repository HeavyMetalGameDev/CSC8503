#include "ServerPlayerComponent.h"
#define DEGREES_TO_RAD 0.0174532925f
namespace NCL::CSC8503 {

	void ServerPlayerComponent::Update(float dt) {
		float normYaw = cameraValues->yaw > 180 ? cameraValues->yaw - 360 : cameraValues->yaw; //get yaw between -180 and 180
		normYaw = normYaw * DEGREES_TO_RAD;

		//convert from pitch/yaw to directions
		float xDir = cos(normYaw) * cos(cameraValues->pitch * DEGREES_TO_RAD);
		float yDir = sin(cameraValues->pitch * DEGREES_TO_RAD);
		float zDir = sin(-normYaw) * cos(cameraValues->pitch * DEGREES_TO_RAD);

		lookDirection = Vector3(zDir, yDir, -xDir);

		sphereTrigger->GetTransform().SetPosition(gameObject->GetTransform().GetPosition() + (lookDirection * 2.5f) + Vector3(0, 0.3f, 0));
		isEPressed = inputs[7];
		inputs[7] = 0; //make sure we dont think e is held!

		if (inputs[5] && hasObject) {
			OnObjectDrop();
			heldObject->GetPhysicsObject()->AddForce(lookDirection * throwForce);
		}
		inputs[5] = 0;

		//from PlayerInputComponent
		if (inputs[4]) {
			Ray ray(gameObject->GetTransform().GetPosition(), { 0,-1,0 });
			RayCollision rc;
			if (worldRef->Raycast(ray, rc, true, raycastCollideMap, gameObject)) {
				if ((rc.collidedAt - gameObject->GetTransform().GetPosition()).Length() < 2.0f) {
					hasJumped = true;
				}
			}
		}
		inputs[4] = 0;


		//GRAPPLING-----------------------
		if (inputs[6] && !isGrappling) {
			Ray ray(gameObject->GetTransform().GetPosition() + Vector3(0, 0.3f, 0), Vector3(zDir, yDir, -xDir));
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
		else if (!inputs[6] && isGrappling) {
			isGrappling = false;
			isGrapplingStatic = false;
		}

	}

	void ServerPlayerComponent::PhysicsUpdate(float dt) {
		if (isEPressed) {
			if (!hasObject) {
				if (tc->IsTriggering()) {
					if (tc->GetTriggeredObject()->GetPhysicsObject()->IsDynamic()) {
						hasObject = true;
						heldObject = tc->GetTriggeredObject();
						heldObject->GetTransform().SetPosition(sphereTrigger->GetTransform().GetPosition());
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
					forceDirection.y *= 0.3f; //this is because the player has less y damping so the force needs to be lower
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

	void ServerPlayerComponent::ApplyForceToObject() {
		Vector3 objectDirection = sphereTrigger->GetTransform().GetPosition() - heldObject->GetTransform().GetPosition();
		float distance = objectDirection.Length();
		if (distance > maxDistance) {
			OnObjectDrop();
			return;
		}
		objectDirection.Normalise();
		float dragRatio = std::min(distance / dragDistance, 1.0f);
		Vector3 objectForce = objectDirection * (dragRatio * dragForce);
		//heldObject->GetPhysicsObject()->ClearForces();
		heldObject->GetPhysicsObject()->SetAwake();
		heldObject->GetPhysicsObject()->AddForce(objectForce);

		heldObject->GetPhysicsObject()->SetAngularVelocity(Vector3());

		if (heldObject->GetBoundingVolume()->type != VolumeType::AABB || heldObject->GetTag() == "Enemy") //dont rotate AABBs or enemies
			heldObject->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(cameraValues->pitch, cameraValues->yaw, 0));
	}

	void ServerPlayerComponent::OnObjectDrop() {
		hasObject = false;
		heldObject->GetPhysicsObject()->SetAwake();
		heldObject->GetPhysicsObject()->SetPhysMat(physMatCache);
	}
}