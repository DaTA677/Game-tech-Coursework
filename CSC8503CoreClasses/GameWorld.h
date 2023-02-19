#pragma once
#include <random>

#include "Ray.h"
#include "CollisionDetection.h"
#include "QuadTree.h"
namespace NCL {
		class Camera;
		using Maths::Ray;
	namespace CSC8503 {
		class GameObject;
		class Constraint;

		typedef std::function<void(GameObject*)> GameObjectFunc;
		typedef std::vector<GameObject*>::const_iterator GameObjectIterator;

		class GameWorld	{
		public:
			GameWorld();
			~GameWorld();

			void Clear();
			void ClearAndErase();

			void AddGameObject(GameObject* o);
			void RemoveGameObject(GameObject* o, bool andDelete = false);

			void AddConstraint(Constraint* c);
			void RemoveConstraint(Constraint* c, bool andDelete = false);

			Camera* GetMainCamera() const {
				return mainCamera;
			}

			void ShuffleConstraints(bool state) {
				shuffleConstraints = state;
			}

			void ShuffleObjects(bool state) {
				shuffleObjects = state;
			}

			bool Raycast(Ray& r, RayCollision& closestCollision, bool closestObject = false, GameObject* ignore = nullptr, int ignoreLayer =-1) const;

			virtual void UpdateWorld(float dt);

			void OperateOnContents(GameObjectFunc f);

			void GetObjectIterators(
				GameObjectIterator& first,
				GameObjectIterator& last) const;

			void GetConstraintIterators(
				std::vector<Constraint*>::const_iterator& first,
				std::vector<Constraint*>::const_iterator& last) const;

			int GetWorldStateID() const {
				return worldStateCounter;
			}

			int GetTrigger() const { return trigger; }
			void ToggleTriggerbit(const int& bit) { 
				if ((trigger & (1 << bit)) == 0) trigger = trigger | (1 << bit);
				else trigger = trigger & (~(1 << bit));
			}

			void SetTrigger(const int& val) { trigger = val; }

		protected:
			std::vector<GameObject*> gameObjects;
			std::vector<Constraint*> constraints;

			Camera* mainCamera;

			bool shuffleConstraints;
			bool shuffleObjects;
			int		worldIDCounter;
			int		worldStateCounter;

			int trigger = 0;
		};
	}
}

