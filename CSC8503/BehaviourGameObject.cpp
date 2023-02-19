#include "BehaviourGameObject.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include "BehaviourNode.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

BehaviourGameObject::BehaviourGameObject(std::string name) : StateGameObject(name) {
	counter = 0.0f;
	rootSequence= new BehaviourSequence("root");

}

BehaviourGameObject::~BehaviourGameObject() {
	delete rootSequence;
}

void BehaviourGameObject::Update(float dt) {
	rootSequence->Execute(dt);
}
