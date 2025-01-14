#include "DUOLClient/Player/FSM/PlayerState_Dash.h"

#include "DUOLClient/Manager/ParticleManager.h"
#include "DUOLCommon/Log/LogHelper.h"
#include "DUOLGameEngine/ECS/Component/Transform.h"
#include "DUOLGameEngine/ECS/Component/Animator.h"
#include "DUOLGameEngine/ECS/Component/Rigidbody.h"

#include "DUOLGameEngine/ECS/GameObject.h"
#include "DUOLGameEngine/ECS/Component/CapsuleCollider.h"
#include "DUOLGameEngine/ECS/Component/ParticleRenderer.h"

namespace DUOLClient
{
	PlayerState_Dash::PlayerState_Dash(DUOLClient::Player* player) :
		PlayerStateBase(TEXT("PlayerState_Dash"), player)
		, _particleRenderer(nullptr)
	{
		_player->AddEventFunction(TEXT("StartDash"), std::bind(&DUOLClient::PlayerState_Dash::StartDash, this));
		_player->AddEventFunction(TEXT("DashSFX"), std::bind(&DUOLClient::PlayerState_Dash::DashSoundEffect, this));
		_player->AddEventFunction(TEXT("EndDash"), std::bind(&DUOLClient::PlayerState_Dash::EndDash, this));
	}

	PlayerState_Dash::~PlayerState_Dash()
	{
		
	}

	void PlayerState_Dash::StartDash()
	{
		// 대쉬 소리 ?
	}

	void PlayerState_Dash::EndDash()
	{

		DUOL_TRACE(DUOL_CONSOLE, "Dash | EndDash");

		// 대쉬 끝. 다른 스테이트로 전환됩니다.
		if (MoveCheck() && RunCheck())
			_stateMachine->TransitionTo(TEXT("PlayerState_Run"), 0.f);
		else if (MoveCheck())
			_stateMachine->TransitionTo(TEXT("PlayerState_Move"), 0.f);
		else
			_stateMachine->TransitionTo(TEXT("PlayerState_Idle"), 0.f);

		if (_particleRenderer != nullptr)
		{
			_particleRenderer->GetTransform()->SetParent(nullptr);

			// 조금이라도 흔들리면서 생기는 파티클 이펙트를 없애기 위함.
			_particleRenderer->SetIsEnabled(false);

			// 파티클 매니저에 반환
			_particleRenderer->GetGameObject()->SetIsActiveSelf(false);
		}

	}

	void PlayerState_Dash::DashSoundEffect()
	{
		_player->PlaySoundClip(PlayerSoundTable::avoidSound, false);
		int voiceFactor = rand() % 10;
		if(voiceFactor > 6)
			_player->PlayVoiceSoundClip(PlayerVoiceSoundTable::Voice_Dash, false);
	}

	void PlayerState_Dash::OnStateEnter(float deltaTime)
	{
		PlayerStateBase::OnStateEnter(deltaTime);
		_player->GetGameObject()->SetLayer(TEXT("PlayerDash"));
		
		_animator->SetBool(TEXT("IsDash"), true);

		DUOL_TRACE(DUOL_CONSOLE, "Dash | StateEnter");
		_animator->SetCurrentLoopCount(0);
		//// Idle
		//if (_stateMachine->GetPrevState()->GetName() == TEXT("PlayerState_Idle"))
		//{
		//	// 그냥 앞에 보고 대쉬한다
		//}
		//// Move, Attack
		//else if (_stateMachine->GetPrevState()->GetName() == TEXT("PlayerState_Attack"))
		//{
		//	// 아니면 그냥 앞 보고 대쉬
		//}
		auto lookVec = _transform->GetLook();

		// 방향키 입력이 있다면, 대쉬 방향을 정한다.
		if (MoveCheck())
		{
			LookDirectionUpdate();

			_transform->LookAt(_transform->GetWorldPosition() + _desiredLook * 10.f);
			lookVec = _desiredLook;
		}

		lookVec.y = 0;
		lookVec.Normalize();

		_rigidbody->SetLinearVelocity(DUOLMath::Vector3::Zero);

		//물리버그
		//를 없애기위해 rootMotion을 쓰지 않습니다.
		auto mass = _rigidbody->GetMass();
		
		if (SlopeCheck())
		{
			auto moveVec = lookVec * mass * DASH_IMPULSE;
			//슬로프 계수
			_rigidbody->AddImpulse(moveVec.Projection(_slopeHit._hitNormal) * 0.8f );
		}
		else
		{
			_rigidbody->AddImpulse(lookVec * mass * DASH_IMPULSE);
		}

		_particleRenderer = DUOLClient::ParticleManager::GetInstance()->Pop(ParticleEnum::Dash, 1.f);

		_particleRenderer->SetIsEnabled(true);

		_particleRenderer->GetTransform()->SetParent(_transform, false);

		_player->_isDash = true;
	}

	void PlayerState_Dash::OnStateExit(float deltaTime)
	{
		PlayerStateBase::OnStateExit(deltaTime);


		DUOL_TRACE(DUOL_CONSOLE, "Dash | StateExit");
		// 대쉬 상태 끝 !
		_player->_isDash = false;
		_rigidbody->SetLinearVelocity(DUOLMath::Vector3::Zero);
		_player->GetGameObject()->SetLayer(TEXT("Player"));

		_animator->SetBool(TEXT("IsDash"), false);
	}
}