#include "PlayerStateObject.h"
#include "BonusStateObject.h"
#include "EnemyGameObject.h"
#include "StateMachine.h"
#include "State.h"
#include "StateTransition.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "PhysicsObject.h"


using namespace NCL;
using namespace CSC8503;
using namespace NCL::Maths;

PlayerStateObject::PlayerStateObject(std::string name) :StateGameObject(name) {
	SetLayer(2);
	SetIgnoreLayer(36);
	stateMachine = new StateMachine();
	currentNodeIndex = 0;
	State* idleState= new State([&](float dt)->void
		{
			this->IdleState(dt);
		});
	stateMachine->AddState(idleState);

	State* normalState= new State([&](float dt)->void
		{
			NormalState(dt);
		});
	stateMachine->AddState(normalState);
	speed = 25.0f;

	StateTransition* idleToNormal = new StateTransition(idleState, normalState, [&](const StateTransition& s)->bool {
		return !CheckIdle();
		}
	);
	StateTransition* normalToIdle= new StateTransition(normalState, idleState, [&](const StateTransition& s)->bool {
		return CheckIdle();
		}
	);

	idle = true;
	won = false;
	counter = 0.0f;

	stateMachine->AddTransition(idleToNormal);
	stateMachine->AddTransition(normalToIdle);

	SetObjectType(2);

}

void PlayerStateObject::OnCollisionBegin(GameObject* object)
{
	if (object->GetObjectType() == 3) {
		float tempHealth = (health + ((BonusStateObject*)object)->GetBonusValue());
		if (tempHealth > maxHealth) health = maxHealth;
		else health = tempHealth;
	}
	if (object->GetObjectType() == 5) health = health - ((BonusStateObject*)object)->GetBonusValue();
	if (object->GetObjectType() == 4) {
		BonusStateObject* bonus = ((BonusStateObject*)object);
		GetPhysicsObject()->SetLinearVelocity(GetPhysicsObject()->GetLinearVelocity() * (1 + bonus->GetBonusValue()));
	}
	if (object->GetObjectType() == 6) GetPhysicsObject()->SetLinearVelocity(GetPhysicsObject()->GetLinearVelocity()*(1-((BonusStateObject*)object)->GetBonusValue()));
}

void NCL::CSC8503::PlayerStateObject::OnCollisionEnd(GameObject* object)
{
	
}

bool PlayerStateObject::CheckIdle() {
	if ((GetPhysicsObject()->GetAngularVelocity() + GetPhysicsObject()->GetLinearVelocity()).Length() <= 0.5f) {
		if (!idle) ToggleIdle();
	}
	else if (idle) ToggleIdle();
	return idle;
}

void PlayerStateObject::IdleState(float dt) {
	counter++;
	this->GetTransform().SetOrientation(Quaternion(GetTransform().GetMatrix() * Matrix4::Rotation(45*sin(counter*3.14/180.0f)*dt, Vector3(0, 1, 0))));
	if (counter == 360.0f) counter = 0.0f;
}


void PlayerStateObject::NormalState(float dt) {
}
