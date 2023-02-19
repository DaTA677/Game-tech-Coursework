#pragma once
#include"StateGameObject.h"
namespace NCL {

	namespace CSC8503 {

		class ObstacleStateObject : public StateGameObject {
		public:
			ObstacleStateObject(std::string name = "Bonus");
			~ObstacleStateObject() {};

			void OnCollisionBegin(GameObject* object) override;
			void OnCollisionEnd(GameObject* object) override;

		protected:
			float health;
		};

	}
}