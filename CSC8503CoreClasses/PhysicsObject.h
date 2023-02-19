#pragma once
#include "Transform.h"
using namespace NCL::Maths;

namespace NCL {
	class CollisionVolume;
	
	namespace CSC8503 {
		class Transform;

		class PhysicsObject	{
		public:
			float friction;
			PhysicsObject(Transform* parentTransform, const CollisionVolume* parentVolume);
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
			void InitHollowSphereInertia();

			void UpdateInertiaTensor();

			void ToggleSleep() { sleep = !sleep; }
			bool IsSleeping()const { return sleep; }

			Matrix3 GetInertiaTensor() const {
				return inverseInteriaTensor;
			}

			float GetElasticity () const { return elasticity; }
			void SetElasticity(const float& e) { elasticity = e; }

			Vector3 GetForwardVector() { return (transform->GetOrientation() * Vector3(0, 0, -1)).Normalised(); }
			Vector3 GetBackwardVector() { return (transform->GetOrientation() * Vector3(0, 0, 1)).Normalised(); }
			Vector3 GetRightVector() { return (transform->GetOrientation() * Vector3(1, 0, 0)).Normalised(); }
			Vector3 GetLeftVector() { return (transform->GetOrientation() * Vector3(-1, 0, 0)).Normalised(); }
			Vector3 GetUpVector() { return (transform->GetOrientation() * Vector3(0, 1, 0)).Normalised(); }
			Vector3 GetDownVector() { return (transform->GetOrientation() * Vector3(0, -1,0)).Normalised(); }

		protected:
			const CollisionVolume* volume;
			Transform*		transform;

			float inverseMass;
			float elasticity;
			

			bool sleep;

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

