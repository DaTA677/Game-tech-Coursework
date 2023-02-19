#include "ObstacleStateObject.h"
#include"PlayerStateObject.h"
#include"StateGameObject.h"
#include"StateMachine.h"
#include"State.h"
#include"StateTransition.h"
#include"PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

ObstacleStateObject::ObstacleStateObject(std::string name){
	SetLayer(8);
	SetIgnoreLayer(36);
	stateMachine = new StateMachine();
	currentNodeIndex = 0;
	health = 2;
	State* startState = new State([&](float dt)->void
		{
		});
	stateMachine->AddState(startState);

	State* endState = new State([&](float dt)->void
		{
			
		});
	stateMachine->AddState(endState);
	speed = 25.0f;

	StateTransition* starttoEnd = new StateTransition(startState, endState, [&](const StateTransition& s)->bool {
		return this->GetHealth()<0.0f;
		}
	);


	stateMachine->AddTransition(starttoEnd);

	SetObjectType(7);

}

void ObstacleStateObject::OnCollisionEnd(GameObject* object) {
	if (health <= 0.0f) {
		//toDelete = true;
		isActive = false;
	}
	
}
void ObstacleStateObject::OnCollisionBegin(GameObject* object) {
	if (object->GetObjectType() == 2 || object->GetObjectType() == 8) {
		health -= object->GetPhysicsObject()->GetLinearVelocity().Length();
	}
	if (object->GetObjectType() == 8) {
		score = score * 1.15f;
	}

}