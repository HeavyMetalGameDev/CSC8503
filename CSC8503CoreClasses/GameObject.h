#pragma once
#include "Transform.h"
#include "CollisionVolume.h"
#include "Component.h"
#include "Camera.h"
#include "PhysicsMaterial.h"

using std::vector;

namespace NCL::CSC8503 {
	class NetworkObject;
	class RenderObject;
	class PhysicsObject;
	class GameWorld;

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

		void SetActive(bool a) { isActive = a; }
		bool IsActive() const {
			return isActive;
		}

		virtual void Update(float dt) { UpdateAllComponents(dt); if (attachedCamera)attachedCamera->SetPosition(transform.GetPosition() + Vector3(0,0.3f,0)); }
		virtual void PhysicsUpdate(float dt) { PhysicsUpdateAllComponents(dt);}
		virtual void Start(GameWorld* gw) { StartAllComponents(gw); }

		void UpdateAllComponents(float dt) { for (Component* component : components)component->Update(dt); }
		void PhysicsUpdateAllComponents(float dt) { for (Component* component : components)component->PhysicsUpdate(dt); }
		void StartAllComponents(GameWorld* gw) { for (Component* component : components)component->Start(gw); }

		Transform& GetTransform() {
			return transform;
		}
		Camera* GetCamera() {
			return attachedCamera;
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

		void SetNetworkObject(NetworkObject* n) { networkObject = n; }

		void SetRenderObject(RenderObject* newObject) {
			renderObject = newObject;
		}

		void SetCamera(Camera* c) {
			attachedCamera = c;
		}

		void SetPhysicsObject(PhysicsObject* newObject) {
			physicsObject = newObject;
		}

		const std::string& GetTag() const {
			return tag;
		}

		void SetTag(std::string t) { tag = t; }

		virtual void OnCollisionBegin(GameObject* otherObject) {
			for (Component* component : components)component->OnCollisionBegin(otherObject);
		}

		virtual void OnCollisionEnd(GameObject* otherObject) {
			for (Component* component : components)component->OnCollisionEnd(otherObject);
		}

		virtual void OnCollisionStay(GameObject* otherObject) {
			for (Component* component : components)component->OnCollisionStay(otherObject);
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
		Camera* attachedCamera;
		bool		isActive;
		int			worldID;
		std::string	tag;

		Vector3 broadphaseAABB;
	};
}

