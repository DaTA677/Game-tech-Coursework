#pragma once
#include"GameObject.h"
#include"GameWorld.h"
namespace NCL {

	namespace CSC8503 {

		class WinStateObject : public GameObject{
		public:
			WinStateObject(std::string name = "Win");
			~WinStateObject() {};

			void OnCollisionBegin(GameObject* object) override;
			void OnCollisionEnd(GameObject* object) override;
			
			GameWorld* GetWorld()const { return world; }
			void SetWorld(GameWorld* g) { world=g; }


		protected:
			GameWorld* world;
		};

	}
}