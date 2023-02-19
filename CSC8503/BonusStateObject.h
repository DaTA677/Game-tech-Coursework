#pragma once
#include"StateGameObject.h"
namespace NCL {
		
	namespace CSC8503 {

		class BonusStateObject : public StateGameObject {
		public:
			BonusStateObject(std::string name="Bonus");
			~BonusStateObject() {};

			void OnCollisionEnd(GameObject* object) override;

			float GetBonusValue() const { return bonusValue; }
			void SetBonusValue(float v) { bonusValue = v; }

		protected:
			float bonusValue;
		};

	}
}