﻿/**

    @file      AnimatorController.h
    @brief     애니메이터 컴포넌트를 컨트롤합니다.
    @details   ~
    @author    COMETLEE
    @date      2.01.2023
    @copyright © COMETLEE, 2023. All right reserved.

**/
#pragma once
#include "DUOLGameEngine/ECS/ObjectBase.h"

#include <map>

namespace DUOLGameEngine
{
	class Animator;
	class AnimatorController; 
	class AnimatorControllerLayer;
	class AnimatorStateMachine;
	class AnimatorState;
	class AnimatorStateTransition;

	class AnimationClip;
}

namespace DUOLGameEngine
{
	/**
	 * \brief The type of the parameter.
	 */
	enum class AnimatorControllerParameterType
	{
		Float
		, Int
		, Bool
	};

	/**
	 * \brief 애니메이터 별로 할당되는 Context. SetAnimatorController(...) 할 때 할당됩니다.
	 */
	struct AnimatorControllerContext
	{
		struct AnimatorStateMachineContext
		{
			DUOLGameEngine::AnimatorStateMachine* _currentStateMachine = nullptr;

			/**
			 * \brief 현재의 상태 머신에서 트랜지션이 적용되고 있는가 여부
			 */
			bool _isOnTransition = false;
		};

		struct AnimatorStateContext
		{
			/**
			 * \brief 현재 스테이트
			 */
			DUOLGameEngine::AnimatorState* _currentState = nullptr;

			// 벡터로 들어가는게 맞는 것 같다 .. (like blending tree ...)
			float _currentFrame = 0.f;
		};

		struct AnimatorStateTransitionContext
		{
			/**
			 * \brief 해당 컨텍스트가 대표하고 있는 AnimatorStateTransition.
			 */
			DUOLGameEngine::AnimatorStateTransition* _currentTransition = nullptr;

			/**
			 * \brief From state Animation의 현재 프레인
			 */
			float _currentFrameOfFrom = 0.f;

			/**
			 * \brief To State Animation의 현재 프레임
			 */
			float _currentFrameOfTo = 0.f;

			/**
			 * \brief 지금 진행 중인 트랜지션에 소요되는 총 시간 (초 단위)
			 */
			float _totalTransitionTime = 0.f;

			/**
			 * \brief 지금 진행 중인 트랜지션의 종료까지 남은 시간 (초 단위)
			 */
			float _remainTransitionTime = 0.f;
		};

		AnimatorControllerContext(DUOLGameEngine::Animator* animator, DUOLGameEngine::AnimatorController* controller);

		std::unordered_map<DUOLCommon::tstring, float> _floatParameters = {};

		std::unordered_map<DUOLCommon::tstring, int> _intParameters = {};

		std::unordered_map<DUOLCommon::tstring, bool> _boolParameters = {};

		// TODO - 상, 하체 등 여러 애니메이션을 섞기 위해서 Layer 별로 _currentStateMachine, _currentState가 있어야함.
		/**
		 * \brief 현재 컨텍스트에서 진행 중인 레이어 별 state machine contexts.
		 */
		std::vector<AnimatorStateMachineContext> _currentStateMachineContexts = {};

		/**
		 * \brief 현재 컨텍스트에서 진행 중인 레이어 별 state contexts.
		 */
		std::vector<AnimatorStateContext> _currentStateContexts = {};

		/**
		 * \brief 현재 컨텍스트에서 진행 중인 레이어 별 transition contexts.
		 */
		std::vector<AnimatorStateTransitionContext> _currentTransitionContexts = {};

		/**
		 * \brief 현재 컨텍스트가 대표하는 애니메이터 컴포넌트의 주소입니다.
		 */
		DUOLGameEngine::Animator* _animator;

		/**
		 * \brief 컨텍스트의 파라미터 리스트를 AnimatorController에 맞게 세팅합니다.
		 * \param controller 해당 컨트롤러에 대응되도록 컨텍스트 밸류를 다시 세팅합니다.
		 * 게임 실행 중에는 컨트롤러를 갱신할 수 없습니다. (비효율이고 의미 없음.)
		 */
		void SetParameters(DUOLGameEngine::AnimatorController* controller);
	};

	/**
	 * \brief Controls animation through layers with state machines, controlled by parameters.
	 */
	class AnimatorController final : public DUOLGameEngine::ObjectBase
	{
	public:
		AnimatorController(const DUOLCommon::tstring& name = TEXT("AnimatorController"));

		virtual ~AnimatorController() override;

	private:
		/**
		 * \brief 해당 애니메이터 컨트롤러를 사용하는 Animator 들에 생성된 References.
		 */
		std::unordered_map<DUOLGameEngine::UUID, DUOLGameEngine::AnimatorControllerContext*> _animatorControllerContexts;

		/**
		 * \brief 해당 애니메이터 컨트롤러에서 관리하는 모든 파라미터들의 이름과 타입 리스트
		 */
		std::unordered_map<DUOLCommon::tstring, AnimatorControllerParameterType> _allParameterTypes;

		/**
		 * \brief 일단 One-layer로 갑시다. TODO - 이 녀석은 추후 Editor 상에 띄워진 Layer를 나타낼 것입니다.
		 */
		DUOLGameEngine::AnimatorControllerLayer* _currentLayer;

	public:
		/**
		 * \brief Add a parameter to the controller.
		 * \param paramName The name of the parameter.
		 * \param paramType The type of the parameter.
		 */
		void AddParameter(const DUOLCommon::tstring& paramName, AnimatorControllerParameterType paramType);

		/**
		 * \brief Remove a parameter in the controller.
		 * \param paramName The name of the parameter.
		 */
		void RemoveParameter(const DUOLCommon::tstring& paramName);

		/**
		 * \brief Add a state machine to the controller.
		 * \param stateMachineName The name of the state machine.
		 * \return state machine created.
		 */
		DUOLGameEngine::AnimatorStateMachine* AddStateMachine(const DUOLCommon::tstring& stateMachineName);

		/**
		 * \brief creates a new state with the motion in it.
		 * \param animationClip Animation of state created.
		 */
		DUOLGameEngine::AnimatorState* AddMotion(DUOLGameEngine::AnimationClip* animationClip);

		/**
		 * \brief Update animator controller state of context.
		 * \param context for update.
		 */
		void UpdateAnimatorController(DUOLGameEngine::AnimatorControllerContext* context, float deltaTime);

#pragma region FRIEND_CLASS
		friend class AnimatorControllerContext;

		friend class AnimatorStateTransition;

		friend class Animator;
#pragma endregion
	};
}