#pragma once

namespace NCL {
	namespace CSC8503 {
		typedef std::function<void(float)> StateUpdateFunction;

		class  State		{
		public:
			State() {}
			State(StateUpdateFunction someFunc) {
				func		= someFunc;
			}
			void Update(float dt)  {
				if (func != nullptr) {
					func(dt);
				}
			}

			int GetStateNum() const { return stateNum; }
			void SetStateNum(int val) { stateNum = val; }

		protected:
			StateUpdateFunction func;
			int stateNum=0;
		};
	}
}