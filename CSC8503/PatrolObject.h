#pragma once
#include "StateGameObject.h"


namespace NCL {
    namespace CSC8503 {
        class PatrolObject : public StateGameObject {
        public:
            PatrolObject() {};
            ~PatrolObject() {};

            void GenerateStates();
            void AddPatrolNode(Vector3 pos);

        protected:
            std::vector<Vector3> nodes;
        };
    }
}