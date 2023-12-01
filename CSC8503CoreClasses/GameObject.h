#pragma once
#include "Transform.h"
#include "CollisionVolume.h"
#include "Component.h"

using std::vector;

namespace NCL::CSC8503 {
	class NetworkObject;
	class RenderObject;
	class PhysicsObject;

	class GameObject	{
	public:
		GameObject(const std::string& name = "");
		~GameObject();

		void SetBoundingVolume(CollisionVolume* vol) {
			boundingVolume = vol;
		}

		const CollisionVolume* GetBoundingVolume() const {
			return boundingVolume;
		}

		bool IsActive() const {
			return isActive;
		}

		virtual void Update(float dt) { UpdateAllComponents(dt);}
		virtual void PhysicsUpdate(float dt) { UpdateAllComponents(dt);}

		void UpdateAllComponents(float dt) { for (Component* component : components)component->Update(dt); }
		void PhysicsUpdateAllComponents(float dt) { for (Component* component : components)component->PhysicsUpdate(dt); }

		Transform& GetTransform() {
			return transform;
		}

		RenderObject* GetRenderObject() const {
			return renderObject;
		}

		PhysicsObject* GetPhysicsObject() const {
			return physicsObject;
		}

		NetworkObject* GetNetworkObject() const {
			return networkObject;
		}

		void SetRenderObject(RenderObject* newObject) {
			renderObject = newObject;
		}

		void SetPhysicsObject(PhysicsObject* newObject) {
			physicsObject = newObject;
		}

		const std::string& GetName() const {
			return name;
		}

		virtual void OnCollisionBegin(GameObject* otherObject) {
			for (Component* component : components)component->OnCollisionBegin(otherObject);
		}

		virtual void OnCollisionEnd(GameObject* otherObject) {
			for (Component* component : components)component->OnCollisionEnd(otherObject);
		}

		bool GetBroadphaseAABB(Vector3&outsize) const;

		void UpdateBroadphaseAABB();

		void SetWorldID(int newID) {
			worldID = newID;
		}

		int		GetWorldID() const {
			return worldID;
		}
		template <typename T>
		bool TryGetComponent(T*& returnPointer) {
			for (Component* component : components) {
				T* typeCast = dynamic_cast<T*>(component);
				if (typeCast) {
					returnPointer = typeCast;
					return true;
				}
			}
			return false;
		}

		void AddComponent(Component* component) {
			components.push_back(component);
		}

	protected:
		Transform			transform;
		std::vector<Component*> components;
		CollisionVolume*	boundingVolume;
		PhysicsObject*		physicsObject;
		RenderObject*		renderObject;
		NetworkObject*		networkObject;

		bool		isActive;
		int			worldID;
		std::string	name;

		Vector3 broadphaseAABB;
	};
}

