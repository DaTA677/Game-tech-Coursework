#pragma once
#include"BehaviourGameObject.h"
#include"GameWorld.h"
#include "PositionConstraint.h"
namespace NCL {

	namespace CSC8503 {

		class DoorBehaviourObject : public BehaviourGameObject{
		public:
			DoorBehaviourObject(std::string name = "Door");
			BehaviourState CloseDoor(float dt, BehaviourState state);
			BehaviourState CheckItemCount(float dt, BehaviourState state);
			BehaviourState CheckSwitchOn(float dt, BehaviourState state);
			BehaviourState CheckSwitchOff(float dt, BehaviourState state);
			BehaviourState OpenDoor(float dt, BehaviourState state);
			~DoorBehaviourObject() { delete openNode; delete openNodeConstraint; };

			void Update(float dt) override;

			void SetOriginalPosition(const Vector3& pos) { originalPosition = pos; }
			Vector3 GetOriginalPosition() const { return originalPosition; }

			void SetOpenNodeConstraint(PositionConstraint* cos) { openNodeConstraint= cos; }
			PositionConstraint* GetOpenNodeConstraint() const { return openNodeConstraint; }

			void SetOpenNode(GameObject* o) { openNode = o; }
			GameObject* GetOpenNode() const { return openNode; }

			void SetTriggerBit(const int& v) { if (v < 32) triggerBit = v; }
			int GetTrigerBit() const { return triggerBit; }

			GameWorld* GetGameWorld()const { return world; }
			void SetGameWorld(GameWorld* w) { world = w; }

		protected:
			GameObject* openNode;

			GameWorld* world;

			Vector3 originalPosition;

			PositionConstraint* openNodeConstraint;

			int triggerBit=0;
		};

	}
}