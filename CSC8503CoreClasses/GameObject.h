#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

using std::vector;



namespace NCL {
	enum class LayerMask {
		WORLD = 1,
		PLAYER = 2,
		BONUS = 4,
		ENEMY = 8,
		SECRET = 16,
		IGNORED = 32,
		INVALID = 256
	};
	enum class ObjectId {
		WORLD = 1,
		PLAYER = 2,
		HEALTHBONUS = 3,
		SPEEDBONUS = 4,
		HEALTHREDUCTION = 5,
		SPEEDREDUCTION = 6,
		OBSTACLE = 7,
		ENEMY = 8,
		SECRET = 9,
		WIN = 10,
		KEY=11,
		IGNORED=12,
		SPRING =14
	};

	namespace CSC8503 {
		class NetworkObject;
		class RenderObject;
		class PhysicsObject;

		class GameObject {
		public:

			GameObject(std::string name = "", int layer = 1, int ignoreLayer = 256);
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

			void SetName(std::string n) {
				name = n;
			}

			virtual void OnCollisionBegin(GameObject* otherObject) {
				//std::cout << "OnCollisionBegin event occured!\n";
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {
				//std::cout << "OnCollisionEnd event occured!\n";
			}

			virtual void Update(float dt) {

			}

			bool GetBroadphaseAABB(Vector3& outsize) const;

			void UpdateBroadphaseAABB();

			void SetWorldID(int newID) {
				worldID = newID;
			}

			int GetLayer() const { return layer; }
			void SetLayer(const int& l) { layer = l; }

			int GetIgnoreLayer() const { return ignoreLayer; }
			void SetIgnoreLayer(const int& l) { ignoreLayer = l; }

			void SetObjectType(ObjectId t) { type = (ObjectId)t; }
			ObjectId GetObjectType() const { return type; }

			int		GetWorldID() const {
				return worldID;
			}

		protected:
			Transform			transform;

			CollisionVolume* boundingVolume;
			PhysicsObject* physicsObject;
			RenderObject* renderObject;
			NetworkObject* networkObject;

			bool		isActive;
			int			worldID;
			std::string	name;

			int layer;
			int ignoreLayer;

			Vector3 broadphaseAABB;

			ObjectId type;
		};
	}
}

