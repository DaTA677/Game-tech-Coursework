#include "EnemyGameObject.h"
#include "BonusStateObject.h"
#include "PhysicsObject.h"
#include "StateMachine.h"
#include "State.h"
#include "StateTransition.h"
#include "Matrix4.h"
#include "Quaternion.h"


using namespace NCL;
using namespace CSC8503;
using namespace NCL::Maths;

EnemyGameObject::EnemyGameObject(std::string name) :StateGameObject(name) {
	SetLayer(8);
	SetIgnoreLayer(36);
	stateMachine = new StateMachine();
	currentNodeIndex = 0;
	State* patrolState = new State([&](float dt)->void
		{
			this->Patrol(dt);
		});
	stateMachine->AddState(patrolState);

	State* runaway = new State([&](float dt)->void
		{
			RunAwayFromPlayer(dt);
		});
	stateMachine->AddState(runaway);
	speed = 25.0f;

	StateTransition* patrolToChase = new StateTransition(patrolState, runaway, [&](const StateTransition& s)->bool {
		return CheckChasePlayer();
		}
	);
	StateTransition* chaseToPatrol=new StateTransition(runaway, patrolState,  [&](const StateTransition& s)->bool {
		return !CheckChasePlayer();
		}
	);

	stateMachine->AddTransition(patrolToChase);
	stateMachine->AddTransition(chaseToPatrol);
	SetObjectType(8);
}

void EnemyGameObject::OnCollisionBegin(GameObject* object) {
	if (object->GetObjectType() == 4) {
		BonusStateObject* bonus = ((BonusStateObject*)object);
		GetPhysicsObject()->SetLinearVelocity(GetPhysicsObject()->GetLinearVelocity() * (1 + bonus->GetBonusValue()));
	}
}

void EnemyGameObject::LookAtPosition(float dt, Vector3 pos) {
	Quaternion orientation = Quaternion(Matrix4::BuildViewMatrix(GetTransform().GetPosition()* Vector3(1, 0, 1), pos * Vector3(1, 0, 1), Vector3(0, 1, 0)).Inverse());
	GetTransform().SetOrientation(orientation);
}
void EnemyGameObject::LookAwayFromPosition(float dt, Vector3 pos) {
	Quaternion orientation = Quaternion(Matrix4::BuildViewMatrix(pos*Vector3(1,0,1), GetTransform().GetPosition() * Vector3(1, 0, 1), Vector3(0, 1, 0)).Inverse());
	GetTransform().SetOrientation(orientation);
}

void EnemyGameObject::Patrol(float dt) {
	Vector3 destination = nodes[currentNodeIndex];
	
	float distance = (GetTransform().GetPosition() - destination).Length();
	if (distance < 3.0f)
	{
		currentNodeIndex++;
		if (currentNodeIndex > (int)nodes.size() - 1)
			currentNodeIndex = 0;
	}
	LookAtPosition(dt, nodes[currentNodeIndex]);
	MoveToPosition(dt, nodes[currentNodeIndex]);
}

bool EnemyGameObject::CheckChasePlayer() {
	Vector3 direction = (player->GetTransform().GetPosition()*Vector3(1,0,1)) - (GetTransform().GetPosition()*Vector3(1,0,1));
	Vector3 forward = GetTransform().GetOrientation() * Vector3(0, 0, -1);
	float radians = acos(Vector3::Dot(direction, forward)/(direction.Length()*forward.Length()));
	if ((abs(radians * (180 / 3.14)) < 60.0f && direction.Length()<50)|| direction.Length() < 20) return true;
	return false;

}
void EnemyGameObject::ChasePlayer(float dt) {
	Vector3 destination = player->GetTransform().GetPosition()*Vector3(1,0,1);
	LookAtPosition(dt,destination);
	MoveToPosition(dt, destination);
}
void EnemyGameObject::RunAwayFromPlayer(float dt) {
	Vector3 destination = player->GetTransform().GetPosition()*Vector3(1,0,1);
	LookAwayFromPosition(dt,destination);
	MoveAwayFromPosition(dt, destination);
}

void EnemyGameObject::AddPatrolNode(Vector3 pos) {
	nodes.emplace_back(pos);
}

