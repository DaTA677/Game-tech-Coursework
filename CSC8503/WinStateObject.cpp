#include "GameObject.h"
#include "GameWorld.h"
#include "WinStateObject.h"

using namespace NCL;
using namespace CSC8503;

WinStateObject::WinStateObject(std::string name):GameObject(name) {

	SetLayer(1);
	SetIgnoreLayer(128);
	SetObjectType(10);
}

void WinStateObject::OnCollisionBegin(GameObject* object) {
	if (GetObjectType()==10 && object->GetObjectType() == 2) {
		world->ToggleTriggerbit(0);
	}
	if(GetObjectType() == 11 && object->GetObjectType() == 2) {
		world->ToggleTriggerbit(3);
	}
}

void WinStateObject::OnCollisionEnd(GameObject* object) {
	if (GetObjectType() == 11 && object->GetObjectType() == 2) {
		isActive = false;
	}
}