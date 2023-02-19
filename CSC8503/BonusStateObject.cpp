#include "BonusStateObject.h"
#include"StateGameObject.h"


using namespace NCL;
using namespace CSC8503;

BonusStateObject::BonusStateObject(std::string name):StateGameObject(name) {
	SetLayer(4);
	ignoreLayer=32;
	

}

void BonusStateObject::OnCollisionEnd(GameObject* object) {
	if (GetLayer() == 4 && object->GetLayer() == 2) {
		toDelete = true;
	}
}