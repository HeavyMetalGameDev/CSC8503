#include "C:/Users/c0034428/Documents/CSC8503/CSC8503CoreClasses/CMakeFiles/CSC8503CoreClasses.dir/Debug/cmake_pch.hxx"
#include "PlayerInputComponent.h"
#include "Window.h"
#include "Ray.h"

namespace NCL::CSC8503{

	void PlayerInputComponent::Update(float dt) {
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE)) {
			Ray ray(gameObject->GetTransform().GetPosition(), { 0,-1,0 });
			RayCollision rc;
			if (worldRef->Raycast(ray, rc, true, gameObject)) {
				if ((rc.collidedAt - gameObject->GetTransform().GetPosition()).Length() < 1.5f) {
					hasJumped = true;
				}
			}
		}
	}

	void PlayerInputComponent::PhysicsUpdate(float dt) {
		if(hasJumped)physObject->ApplyLinearImpulse({ 0,jumpPower,0 });
		hasJumped = false;
	}
}