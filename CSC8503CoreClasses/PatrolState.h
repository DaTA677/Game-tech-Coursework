#pragma once
#include "State.h"

namespace NCL {
	namespace CSC8503 {
	
		class  PatrolState:public State {
		public:
			PatrolState() {}
			PatrolState(std::function<void(float)> someFunc) : State(someFunc) {};

			void Update(float dt) {
				if (func != nullptr) {
					func(dt);
				}
			}

			NCL::Maths::Vector3 GetCurrentDestination()const { return nodes[currentIndex]; }

			void AddNodes(const NCL::Maths::Vector3 pos) { nodes.emplace_back(pos); }

		protected:
			std::vector<NCL::Maths::Vector3> nodes;
			int currentIndex;
		
		};
	}
}