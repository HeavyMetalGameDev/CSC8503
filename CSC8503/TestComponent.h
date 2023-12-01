#pragma once
#include "..\CSC8503CoreClasses\Component.h"
#include <iostream>
class TestComponent : public NCL::CSC8503::Component
{
public:
	TestComponent(NCL::CSC8503::GameObject* gameObject) { this->gameObject = gameObject; }
	void Update(float dt) override { std::cout << "TEST UPDATE!!!!!\n"; }
	void PhysicsUpdate(float dt) override { std::cout << "TEST PHYSICS!!!!!\n"; }
};

