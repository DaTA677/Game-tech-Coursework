#pragma once
#include "StateGameObject.h"
#include<vector>
#include "Vector3.h"

namespace NCL {
    namespace CSC8503 {
        class StateMachine;
        class StateGameObject;
        class PlayerStateObject : public StateGameObject {
        public:
            PlayerStateObject(std::string name = "player");
            ~PlayerStateObject() {};
            
            void OnCollisionBegin(GameObject* object) override;
            void OnCollisionEnd(GameObject* object) override;

            bool IsIdle() const { return idle; }
            void ToggleIdle() { idle = !idle; }

            bool HasWon() const { return won; }
            bool ToggleWon() { won = !won; }

            bool CheckIdle();
            
            void IdleState(float dt);
            void NormalState(float dt);
            

        protected:

            void LookAtPosition(float dt, NCL::Maths::Vector3 pos);

            void Patrol(float dt);
            bool CheckChasePlayer();
            void ChasePlayer(float dt);

            std::vector<NCL::Maths::Vector3> nodes;
            int currentNodeIndex = 0;

            bool won;
            bool idle;
            
        };
    }
}
