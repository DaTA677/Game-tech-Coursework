#pragma once

namespace NCL {
	namespace CSC8503 {
		class State;
		class StateTransition;
		typedef std::function<bool(const StateTransition&)> StateTransitionFunction;
		class StateTransition	{
		
		public:
			StateTransition(State* source, State* dest, StateTransitionFunction f) {
				sourceState		 = source;
				destinationState = dest;
				function		 = f;
			}

			bool CanTransition() const{
				return function(*this);
			}

			State* GetDestinationState()  const {
				return destinationState;
			}

			State* GetSourceState() const {
				return sourceState;
			}

		protected:
			State * sourceState;
			State * destinationState;
			StateTransitionFunction function;
		};
	}
}

