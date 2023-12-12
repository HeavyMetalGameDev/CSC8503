#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Window.h"
namespace NCL::CSC8503 {
	class ClientInputComponent : public Component
	{
	public:
		ClientInputComponent(GameObject* g, Camera* c, NetworkObject* n) { gameObject = g; camera = c;}
		void Update(float dt)override;
		bool buttoninputs[8];
		float cameraYaw = 0;
		float cameraPitch = 0;
	protected:
		Camera* camera;


	};
}

