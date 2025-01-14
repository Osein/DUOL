#pragma once
#include "DUOLGameEngine/ECS/ObjectBase.h"

namespace DUOLGameEngine
{
	class AnimatorController;
	class AnimatorStateTransition;

	struct AnimatorControllerContext;

	class AnimatorControllerLayer;

	class AnimatorState;
}

namespace DUOLGameEngine
{
	/**
	 * \brief represent interaction of states.
	 */
	class DUOL_GAMEENGINE_API AnimatorStateMachine : public DUOLGameEngine::ObjectBase
	{
	public:
		AnimatorStateMachine(DUOLGameEngine::AnimatorController* animatorController = nullptr, const DUOLCommon::tstring& name = TEXT("AnimatorStateMachine"));

		virtual ~AnimatorStateMachine() override;

	private:
		/**
		 * \brief All states in this state machine.
		 */
		std::unordered_map<DUOLCommon::tstring, DUOLGameEngine::AnimatorState*> _animatorStates;

		/**
		 * \brief Start state of this state machine. registered for the first time.
		 */
		DUOLGameEngine::AnimatorState* _entryState;

		/**
		 * \brief 해당 AnimatorStateMachine 이 위치한 AnimatorControllerLayer
		 */
		DUOLGameEngine::AnimatorController* _animatorController;

	public:
		DUOLGameEngine::AnimatorState* AddState(const DUOLCommon::tstring& name);

		/**
		 * \brief Remove state has param name.
		 * \param name name of the state to remove.
		 */
		void RemoveState(const DUOLCommon::tstring& name);

		/**
		 * \brief Remove state of param.
		 * \param animatorState The state to remove.
		 */
		void RemoveState(DUOLGameEngine::AnimatorState* animatorState);

		/**
		 * \brief Get entry state of this state machine.
		 * \return The entry state. (registered for the first time to this state machine)
		 */
		DUOLGameEngine::AnimatorState* GetEntryState() const;

		/**
		 * \brief Update context that is in this state machine.
		 * \param context The context for update.
		 * \param deltaTime frame time.
		 */
		void UpdateAnimatorStateMachine(DUOLGameEngine::AnimatorControllerContext* context, float deltaTime);

	private:
		/**
		 * \brief Update context when does not transition for this frame.
		 * \param targetContext The context to update.
		 * \param deltaTime frame time.
		 */
		void NotTransition(DUOLGameEngine::AnimatorControllerContext* targetContext, float deltaTime);

		/**
		 * \brief When transition enter, will be called.
		 * \param targetTransition The transition to applying.
		 * \param context The context to update.
		 */
		void OnTransitionEnter(DUOLGameEngine::AnimatorControllerContext* context, DUOLGameEngine::AnimatorStateTransition* targetTransition);

		/**
		 * \brief When transition stay, will be called.
		 * \param context The context to update.
		 * \param deltaTime frame time.
		 */
		void OnTransitionStay(DUOLGameEngine::AnimatorControllerContext* context, float deltaTime);

		/**
		 * \brief When transition end up, will be called.
		 * \param context The context to update.
		 * \param transition The transition to end up.
		 */
		void OnTransitionExit(DUOLGameEngine::AnimatorControllerContext* context, DUOLGameEngine::AnimatorStateTransition* transition, float deltaTime);

		RTTR_ENABLE(DUOLGameEngine::ObjectBase)

		RTTR_REGISTRATION_FRIEND

#pragma region FRIEND_CLASS
		friend class AnimatorState;

		friend class AnimatorStateTransition;
#pragma endregion
	};
}