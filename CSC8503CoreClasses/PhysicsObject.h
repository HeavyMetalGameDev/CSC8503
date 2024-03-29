#pragma once
#include "PhysicsMaterial.h"
using namespace NCL::Maths;

namespace NCL {
	class CollisionVolume;
	
	namespace CSC8503 {
		class Transform;

		class PhysicsObject	{
		public:
			PhysicsObject(Transform* parentTransform, const CollisionVolume* parentVolume,bool isDynamic = true, bool isTrigger = false, int colLayer = 1);
			~PhysicsObject();

			Vector3 GetLinearVelocity() const {
				return linearVelocity;
			}

			Vector3 GetAngularVelocity() const {
				return angularVelocity;
			}

			Vector3 GetTorque() const {
				return torque;
			}

			Vector3 GetForce() const {
				return force;
			}

			void SetInverseMass(float invMass) {
				inverseMass = invMass;
			}

			float GetInverseMass() const {
				return inverseMass;
			}

			void SetSleeping() {
				isSleeping = true;
				linearVelocity = Vector3();
				angularVelocity = Vector3();
				torque = Vector3();
				ClearForces();
			}

			void AddToSleepTimer(float delta) { sleepTimer += delta; }
			void ResetSleepTimer() { sleepTimer = 0; }
			float GetSleepTimer() { return sleepTimer; }
			bool IsSleeping() { return isSleeping; }
			bool IsDynamic() { return isDynamic; }
			bool IsTrigger() { return isTrigger; }
			bool CollidedLastFrame() { return collidedLastFrame; }
			void SetCollided(bool c) { collidedLastFrame = c; }
			bool IsGrounded() { return isGrounded; }
			void SetIsGrounded(bool g) { isGrounded = g; }

			void SetCollisionLayer(int l) { collisionLayer = l; }
			int GetCollisionLayer() { return collisionLayer; }

			void SetAwake() {
				isSleeping = false;
			}

			void ApplyAngularImpulse(const Vector3& force);
			void ApplyLinearImpulse(const Vector3& force);
			
			void AddForce(const Vector3& force);

			void AddForceAtPosition(const Vector3& force, const Vector3& position);

			void AddTorque(const Vector3& torque);


			void ClearForces();

			void SetLinearVelocity(const Vector3& v) {
				linearVelocity = v;
			}

			void SetAngularVelocity(const Vector3& v) {
				angularVelocity = v;
			}

			void InitCubeInertia();
			void InitSphereInertia();

			void UpdateInertiaTensor();

			Matrix3 GetInertiaTensor() const {
				return inverseInteriaTensor;
			}

			PhysicsMaterial* GetPhysMat() { return physMat; }
			void SetPhysMat(PhysicsMaterial* pm) { physMat = pm; }

		protected:
			const CollisionVolume* volume;
			Transform*		transform;

			int collisionLayer;

			float inverseMass;
			float elasticity;
			float friction;
			float sleepTimer = 0;

			bool isSleeping = false;
			bool isDynamic;
			bool isTrigger = false;
			bool collidedLastFrame = false;
			bool isGrounded = false;

			PhysicsMaterial* physMat;
			//linear stuff
			Vector3 linearVelocity;
			Vector3 force;
			
			//angular stuff
			Vector3 angularVelocity;
			Vector3 torque;
			Vector3 inverseInertia;
			Matrix3 inverseInteriaTensor;
		};
	}
}

