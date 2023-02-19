#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject(std::string name): GameObject(name) {
	counter = 0.0f;
	stateMachine = new StateMachine();

	State* stateA = new State([&](float dt)->void {
		this->GetPhysicsObject()->ClearForces();
		this->MoveLeft(dt);
		this->MoveRight(dt);
		this->MoveRight(dt);
		this->MoveLeft(dt);
		this->MoveLeft(dt);
		this->counter += (dt);
		}
	);
	State* stateB = new State([&](float dt)->void {
		this->GetPhysicsObject()->ClearForces();
		this->MoveRight(dt);
		this->MoveRight(dt);
		this->MoveLeft(dt);
		this->MoveLeft(dt);
		this->MoveRight(dt);
		this->counter -= (dt);
		}
	);

	stateMachine->AddState(stateA);
	stateMachine->AddState(stateB);

	stateMachine->AddTransition(new StateTransition(stateA, stateB,
		[&](const StateTransition& s)->bool {
			return this->counter > 3.0f;
		}
	));
	stateMachine->AddTransition(new StateTransition(stateB, stateA,
		[&](const StateTransition& s)->bool {
			return this->counter < 0.0f;
		}
	));

}

StateGameObject::~StateGameObject() {
	delete stateMachine;
}

void StateGameObject::Update(float dt) {
	stateMachine->Update(dt);
}

void StateGameObject::MoveLeft(float dt) {
	
	GetPhysicsObject()->AddForce({ -10,0,0 });
	//counter += dt;
}

void StateGameObject::MoveRight(float dt) {

	GetPhysicsObject()->AddForce({ 10,0,0 });
	//counter -= dt;
}

void StateGameObject::MoveToPosition(float dt, Vector3 pos)
{
	GetPhysicsObject()->AddForce((pos - GetTransform().GetPosition()).Normalised() * speed*30*dt);
}
void StateGameObject::MoveAwayFromPosition(float dt, Vector3 pos)
{
	PhysicsObject* phys = GetPhysicsObject();
	phys->ApplyLinearImpulse((phys->GetLinearVelocity().Length()<30.0f) ? ((GetTransform().GetPosition() - pos).Normalised() * speed * 30 * dt):Vector3(0,0,0));
}

void StateGameObject::GenerateStates() 
{
	stateMachine = new StateMachine();
	currentNodeIndex = 0;
	State* initialState = new State([&](float dt)->void {
		//this->GetPhysicsObject()->ClearForces();
		this->MoveToPosition(dt, this->nodes[0]);		//Start Position
		//counter += dt;
		std::cout << dt << std::endl;
		}
	);
	//State* patrolState = new State([&](float dt)->void
	//	{
	//		Vector3 destination = nodes[currentNodeIndex];
	//		float distance = (GetTransform().GetPosition() - destination).Length();
	//		if (distance < 1.0f)
	//		{
	//			currentNodeIndex++;
	//			if (currentNodeIndex > (int)nodes.size() - 1)		//Loop
	//				currentNodeIndex = 0;
	//		}

	//		MoveToPosition(dt, nodes[currentNodeIndex]);
	//	}
	//);

	//stateMachine->AddState(initialState);
	//stateMachine->AddState(patrolState);
	//stateMachine->AddTransition(new StateTransition(initialState, patrolState, [&]()->bool
	//	{
	//		return counter > 3.0f;		//Delayed start
	//	}
	//));
	initialState->SetStateNum(0);
	State* previousState = initialState;
	stateMachine->AddState(initialState);
	for (int i = 1; i < nodes.size(); i++) {
		currentNodeIndex++;
		State* currentState = new State([&](float dt)->void {
			this->GetPhysicsObject()->ClearForces();
			this->MoveToPosition(dt, this->nodes[currentNodeIndex]);
			}
		);
		currentState->SetStateNum(i);
		stateMachine->AddState(currentState);
		stateMachine->AddTransition(new StateTransition(previousState, currentState, [&](const StateTransition& s)->bool {
			Vector3 pos = (this->nodes.at(s.GetSourceState()->GetStateNum()) - this->GetTransform().GetPosition());
			
			return pos.Length() <= 3.0f;
			}
		));
		previousState = currentState;
	}
	stateMachine->AddTransition(new StateTransition(previousState, initialState, [&](const StateTransition& s)->bool {
		Vector3 pos = (this->nodes.at(s.GetSourceState()->GetStateNum()) - this->GetTransform().GetPosition());
		
		return pos.Length() <= 3.0f;
		}
	));
}

void StateGameObject::AddPatrolNode(Vector3 pos) {
	nodes.emplace_back(pos);
}