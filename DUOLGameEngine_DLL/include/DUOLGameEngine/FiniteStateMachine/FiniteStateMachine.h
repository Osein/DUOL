#pragma once
#include <map>

#include "DUOLGameEngine/Export_Engine.h"
#include "DUOLCommon/StringHelper.h"

namespace DUOLGameEngine
{
	class StateBase;

	/**
	 * \brief ������ Finite State Machine
	 */
	class DUOL_GAMEENGINE_API FiniteStateMachine
	{
	public:
		FiniteStateMachine();

		~FiniteStateMachine();

	private:
		/**
		 * \brief All states in state machine.
		 */
		std::map<DUOLCommon::tstring, StateBase*> _states;

		/**
		 * \brief Current state.
		 */
		StateBase* _currentState;

		// next and prev state can ref in OnStateEnter (prev) / OnStateExit (next) !!
		/**
		 * \brief Next state.
		 */
		StateBase* _nextState;

		/**
		 * \brief Previous state.
		 */
		StateBase* _prevState;

	public:
		void TransitionTo(StateBase* nextState, float deltaTime);

		void TransitionTo(const DUOLCommon::tstring& nextStatesName, float deltaTime);

		template <typename TState, typename... Args>
		TState* AddState(Args... args);

		void RemoveState(const DUOLCommon::tstring& stateName);

		void RemoveState(StateBase* state);

		StateBase* GetState(const DUOLCommon::tstring& stateName) const;

		StateBase* GetNextState() const;

		StateBase* GetPrevState() const;

		StateBase* GetCurrentState() const;

	public:
		void UpdateStateMachine(float deltaTime);

		void LateUpdateStateMachine(float deltaTime);

		void FixedUpdateStateMachine(float fixedTimeStep);
	};

	template <typename TState, typename... Args>
	TState* FiniteStateMachine::AddState(Args... args)
	{
		static_assert(std::is_base_of<StateBase, TState>::value, "TState must inherit from StateBase.");
		
		TState* state = new TState(args...);

		state->_stateMachine = this;

		_states.insert({ static_cast<StateBase*>(state)->_stateName, static_cast<StateBase*>(state)});

		// Set entry state.
		if (_currentState == nullptr)
			_currentState = static_cast<StateBase*>(state);

		return state;
	}
}