#include "BehaviourGameObject.h"
#include "DoorBehaviourObject.h"
#include "BehaviourSequence.h"
#include "BehaviourSelector.h"
#include "BehaviourAction.h"
#include "BehaviourNode.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;


DoorBehaviourObject::DoorBehaviourObject(std::string name) {
	SetLayer(1);
	SetIgnoreLayer(36);
	currentNodeIndex = 0;
	
	BehaviourAction* closeDoor = new BehaviourAction("CloseDoor", [&](float dt, BehaviourState state)->BehaviourState {
		return CloseDoor(dt, state);
		});

	BehaviourAction* checkItemCount= new BehaviourAction("Check Item", [&](float dt, BehaviourState state)->BehaviourState {
		return CheckItemCount(dt, state);
		});
	
	BehaviourAction* checkSwitchOn = new BehaviourAction("Check Switch On", [&](float dt, BehaviourState state)->BehaviourState {
		return CheckSwitchOn(dt, state);
		});

	BehaviourAction* checkSwitchoff = new BehaviourAction("Check Switch Off", [&](float dt, BehaviourState state)->BehaviourState {
		return CheckSwitchOff(dt, state);
		});

	BehaviourAction* openDoor = new BehaviourAction("Open Door", [&](float dt, BehaviourState state)->BehaviourState {
		return OpenDoor(dt, state);
		});

	BehaviourSelector* selection = new BehaviourSelector("Open Selection");
	selection->AddChild(checkItemCount);
	selection->AddChild(checkSwitchOn);
	
	rootSequence->AddChild(closeDoor);
	rootSequence->AddChild(selection);
	rootSequence->AddChild(openDoor);
	rootSequence->AddChild(checkSwitchoff);


	SetObjectType(1);

}

void DoorBehaviourObject::Update(float dt) {
	BehaviourState s=rootSequence->Execute(dt);
	if (s==Failure||s==Success) rootSequence->Reset();
}

BehaviourState DoorBehaviourObject::CloseDoor(float dt,BehaviourState state) {
	if (state == Initialise) {
		state = Ongoing;
		counter = 0.0f;
	}
	if (state == Ongoing) {
		counter += dt;
		if (counter >= 3.0f) state = Success;
	}
	return state;

}

BehaviourState DoorBehaviourObject::CheckItemCount(float dt,BehaviourState state) {
	if (state == Initialise) {
		state = Ongoing;
		counter = 0.0f;
	}
	if (state == Ongoing) {
		if ((world->GetTrigger() & (1 << 3)) != 0)
			state = Success;
		else state = Failure;
	}
	return state;
}

BehaviourState DoorBehaviourObject::CheckSwitchOn(float dt,BehaviourState state) {
	if (state == Initialise) {
		state = Ongoing;
		counter = 0.0f;
	}
	if (state == Ongoing) {
		if ((world->GetTrigger() & (1 << 2)) != 0)
			state = Success;
		else state = Failure;
	}
	return state;
}

BehaviourState DoorBehaviourObject::CheckSwitchOff(float dt, BehaviourState state) {
	if (state == Initialise) {
		state = Ongoing;
		counter = 0.0f;
	}
	if (state == Ongoing) {
		if ((world->GetTrigger() & (1 << 3)) == 0 &&((world->GetTrigger() & (1 << 2)) == 0)) {
			state = Success; 
			world->RemoveConstraint(openNodeConstraint); };
	}
	return state;
}

BehaviourState DoorBehaviourObject::OpenDoor(float dt, BehaviourState state) {
	if (state == Initialise) {
		state = Ongoing;
		counter = 0.0f;
		world->AddConstraint(openNodeConstraint);
	}
	if (state == Ongoing) {
		counter += dt;
		if (counter>3.0f) state = Success;
	}
	return state;
}

//void ObstacleStateObject::OnCollisionEnd(GameObject* object) {
//	if (health <= 0.0f) {
//		//toDelete = true;
//		isActive = false;
//	}
//
//}
//void ObstacleStateObject::OnCollisionBegin(GameObject* object) {
//	if (object->GetObjectType() == 2 || object->GetObjectType() == 8) {
//		health -= object->GetPhysicsObject()->GetLinearVelocity().Length();
//	}
//	if (object->GetObjectType() == 8) {
//		score = score * 1.15f;
//	}
//
//}