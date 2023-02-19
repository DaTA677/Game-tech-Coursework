#pragma once
#include "StateGameObject.h"
#include <string>
#include "BehaviourSequence.h"

namespace NCL {
    namespace CSC8503 {
        class BehaviourGameObject: public StateGameObject {
        public:
            BehaviourGameObject(std::string name = "");
            ~BehaviourGameObject();

            virtual void Update(float dt);

            BehaviourSequence* GetRootSequence() const { return rootSequence; }
            void SetRootSequence(BehaviourSequence* s) { rootSequence = s; }

           
        protected:

            BehaviourSequence* rootSequence;
            BehaviourState currentState;
        };
    }
}
