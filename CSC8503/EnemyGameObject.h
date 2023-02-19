#pragma once
#include "StateGameObject.h"
#include<vector>
#include "Vector3.h"

namespace NCL {
    namespace CSC8503 {
        class StateMachine;
        class StateGameObject;
        class EnemyGameObject: public StateGameObject{
        public:
            EnemyGameObject(std::string name="");
            ~EnemyGameObject() {};

            void GenerateStates();
            void AddPatrolNode(NCL::Maths::Vector3 pos);

            void OnCollisionBegin(GameObject* g)override;

            void SetPlayer(GameObject* p) { player = p; }

        protected:

            void LookAtPosition(float dt, NCL::Maths::Vector3 pos);

            void LookAwayFromPosition(float dt, Vector3 pos);

            void Patrol(float dt);
            bool CheckChasePlayer();
            void ChasePlayer(float dt);

            void RunAwayFromPlayer(float dt);

            std::vector<NCL::Maths::Vector3> nodes;
            int currentNodeIndex = 0;
            GameObject* player;
        };
    }
}
