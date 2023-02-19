#pragma once
#include "GameObject.h"
#include <string>

namespace NCL {
    namespace CSC8503 {
        class StateMachine;
        class StateGameObject : public GameObject  {
        public:
            StateGameObject(std::string name="");
            ~StateGameObject();

            virtual void Update(float dt);

            void GenerateStates();
            void AddPatrolNode(Vector3 pos);

            float GetScore() const {
                return score;
            }
            void SetScore(const float& v)  {
                score=v;
            }
            int GetMaxHealth() const {
                return maxHealth;
            }
            void SetMaxHealth(const int& v)  {
                maxHealth=v;
            } 
            float GetHealth() const {
                return health;
            }
            void SetHealth(const float& v)  {
                health=v;
            }


            StateMachine* GetStateMachine() const { return stateMachine; }
            void SetStateMachine(StateMachine* s) { stateMachine = s; }

            bool ToDelete()const { return toDelete; }
            
            bool IsAdded() const { return added; }
            void SetAdded(const bool& val) { added = val; }

        protected:
            void MoveLeft(float dt);
            void MoveRight(float dt);
            void MoveToPosition(float dt, Vector3 pos);

            void MoveAwayFromPosition(float dt, Vector3 pos);

         

            StateMachine* stateMachine;
            float counter;
            std::vector<Vector3> nodes;
            int currentNodeIndex = 0;
            float speed = 0.0f;

            int maxHealth=0;
            float health = 0.0f;
            float score=0;
            bool toDelete=false;
            bool added = false;
        };
    }
}
