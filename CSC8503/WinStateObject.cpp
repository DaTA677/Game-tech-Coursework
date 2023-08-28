#include "GameObject.h"
#include "GameWorld.h"
#include "WinStateObject.h"

using namespace NCL;
using namespace CSC8503;

WinStateObject::WinStateObject(std::string name):GameObject(name) {

	SetLayer(1);
	SetIgnoreLayer(128);
	SetObjectType(ObjectId::WIN);
}

void WinStateObject::OnCollisionBegin(GameObject* object) {
	if (GetObjectType()== ObjectId::WIN && object->GetObjectType() == ObjectId::PLAYER) {
		world->ToggleTriggerbit(0);
	}
	if(GetObjectType() == ObjectId::KEY && object->GetObjectType() == ObjectId::PLAYER) {
		world->ToggleTriggerbit(3);
	}
}

void WinStateObject::OnCollisionEnd(GameObject* object) {
	if (GetObjectType() == ObjectId::KEY && object->GetObjectType() == ObjectId::PLAYER) {
		isActive = false;
	}
}