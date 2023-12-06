#pragma once
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		enum CollisionLayers
		{
			DEFAULT_LAYER = 1, PLAYER_LAYER = 2, STATIC_LAYER = 4, PICKUP_SPHERE_LAYER = 8
		};
		class PhysicsSystem	{
		public:
			PhysicsSystem(GameWorld& g);
			~PhysicsSystem();

			void Clear();

			void Update(float dt);

			void UseGravity(bool state) {
				applyGravity = state;
			}

			void SetGlobalDamping(float d) {
				globalDamping = d;
			}

			void SetGravity(const Vector3& g);
			void BuildStaticQuadTree();
		protected:
			
			void BasicCollisionDetection();
			void BroadPhase();
			void NarrowPhase();

			void ClearForces();

			void IntegrateAccel(float dt);
			void IntegrateVelocity(float dt);

			void UpdateConstraints(float dt);

			void UpdateCollisionList();
			void UpdateObjectAABBs();

			void ImpulseResolveCollision(GameObject& a , GameObject&b, CollisionDetection::ContactPoint& p) const;

			GameWorld& gameWorld;

			bool	applyGravity;
			Vector3 gravity;
			float	dTOffset;
			float	globalDamping;

			float sleepTimeThreshold = .3f;
			float sleepVelocityThreshold = .3f;

			QuadTree<GameObject*>* staticTree; //did not get around to this

			std::map<int,bool> layerMatrix =
			{ {DEFAULT_LAYER | DEFAULT_LAYER,true},
				{DEFAULT_LAYER | PLAYER_LAYER,true},
				{DEFAULT_LAYER | STATIC_LAYER,true},
				{DEFAULT_LAYER | PICKUP_SPHERE_LAYER,true},

				{PLAYER_LAYER | PLAYER_LAYER,true},
				{PLAYER_LAYER | STATIC_LAYER,true},
				{PLAYER_LAYER | PICKUP_SPHERE_LAYER,false},

				{STATIC_LAYER | STATIC_LAYER,false},
				{STATIC_LAYER | PICKUP_SPHERE_LAYER,false},

				{PICKUP_SPHERE_LAYER | PICKUP_SPHERE_LAYER,false}
			};
			std::set<CollisionDetection::CollisionInfo> allCollisions;
			std::set<CollisionDetection::CollisionInfo> broadphaseCollisions;
			std::vector<CollisionDetection::CollisionInfo> broadphaseCollisionsVec;
			bool useBroadPhase		= true;
			int numCollisionFrames	= 5;
		};
	}
}

