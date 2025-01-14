#pragma once
#include "PlayerStateBase.h"

namespace DUOLGameEngine
{
	class ParticleRenderer;
}

namespace DUOLClient
{
	/**
	 * \brief 질주 상태. 해당 상태에서는 몬스터를 밀거나, 엎어트려서 데미지를 입히는 등의 특수 컨디션이 들어갈 수 있으니
	 * 따로 스테이트를 빼놓습니다.
	 */
	class PlayerState_Run : public DUOLClient::PlayerStateBase
	{
	public:
		PlayerState_Run(DUOLClient::Player* player);

		virtual ~PlayerState_Run() override;

	private:
		DUOLGameEngine::GameObject* _particleGameObject;

		DUOLGameEngine::ParticleRenderer* _particleRenderer;

		/**
		 * \brief 가속, 감속 lerp.
		 */
		float _runSpeedSmoothness = 10.f;

		void OnNormalStateStayFixed(float fixedTimeStep);

		void OnOverdriveStateStayFixed(float fixedTimeStep);

		void LeftFootStep();

		void RightFootStep();

	public:
		virtual void OnStateEnter(float deltaTime) override;

		virtual void OnStateStayFixed(float fixedTimeStep) override;

		virtual void OnStateStay(float deltaTime) override;

		virtual void OnStateStayLate(float deltaTime) override;

		virtual void OnStateExit(float deltaTime) override;
	};
}
