#include "DUOLClient/Player/FSM/PlayerState_Ultimate.h"

#include "DUOLClient/Camera/MainCameraController.h"
#include "DUOLClient/ECS/Component/ParticleData.h"
#include "DUOLClient/Manager/GameManager.h"
#include "DUOLClient/Player/Weapon_Sword.h"
#include "DUOLClient/Player/FSM/PlayerState_Idle.h"

#include "DUOLClient/Manager/ParticleManager.h"
#include "DUOLClient/Player/Weapon_AreaWave.h"
#include "DUOLClient/Player/Weapon_Wave.h"
#include "DUOLGameEngine/ECS/Component/Animator.h"
#include "DUOLGameEngine/ECS/Component/ParticleRenderer.h"
#include "DUOLGameEngine/Manager/ResourceManager.h"

#include "DUOLGameEngine/Util/Coroutine/WaitForSeconds.h"
#include "DUOLGameEngine/ECS/GameObject.h"
#include "DUOLGameEngine/ECS/Component/BoxCollider.h"
#include "DUOLGameEngine/ECS/Object/AudioClip.h"
#include "DUOLGameEngine/ECS/Object/AnimatorController/AnimatorState.h"
#include "DUOLGameEngine/Manager/SoundManager.h"
#include "DUOLGameEngine/Manager/TimeManager.h"
#include "DUOLGameEngine/Util/Coroutine/WaitForSecondsRealtime.h"

DUOLClient::PlayerState_Ultimate::PlayerState_Ultimate(DUOLClient::Player* player) :
	PlayerStateBase(TEXT("PlayerState_Ultimate"), player)
	, _ultFirstRun(true)
	, _ultimateSwordDamage(10.f)
	, _ultimateSwordWaveDamage(50.f)
	, _ultimateFistDamage(70.f)

{
#pragma region Ultimate_EVENT
	//이벤트가 
	_player->AddEventFunction(TEXT("EndUltimate"),
		std::bind(&DUOLClient::PlayerState_Ultimate::EndUltimateAnimation, this));
#pragma endregion

#pragma region Ultimate_EFFECT_EVENT
	//이벤트가 
	_player->AddEventFunction(TEXT("StartUltimateSwordTrail"),
		std::bind(&DUOLClient::PlayerState_Ultimate::StartSwordTrail, this));

	_player->AddEventFunction(TEXT("EndUltimateSwordTrail"),
		std::bind(&DUOLClient::PlayerState_Ultimate::EndSwordTrail, this));

	_player->AddEventFunction(TEXT("ChargingFist"),
		std::bind(&DUOLClient::PlayerState_Ultimate::ChargingFist, this));

	_player->AddEventFunction(TEXT("ChargingSword"),
		std::bind(&DUOLClient::PlayerState_Ultimate::ChargingSword, this));

	_player->AddEventFunction(TEXT("EndCharging"),
		std::bind(&DUOLClient::PlayerState_Ultimate::EndCharging, this));

	_player->AddEventFunction(TEXT("FistWaveHit"),
		std::bind(&DUOLClient::PlayerState_Ultimate::FistWaveHit, this));

	_player->AddEventFunction(TEXT("SwordWaveHit"),
		std::bind(&DUOLClient::PlayerState_Ultimate::SwordWaveHit, this));

	//_player->AddEventFunction(TEXT("StartSwordAttackFrame"),
	//	std::bind(&DUOLClient::PlayerState_Ultimate::StartSwordAttackFrame, this));

	//_player->AddEventFunction(TEXT("EndSwordAttackFrame"),
	//	std::bind(&DUOLClient::PlayerState_Ultimate::EndSwordAttackFrame, this));

	_player->AddEventFunction(TEXT("BulletTimeInUltimate"),
		std::bind(&DUOLClient::PlayerState_Ultimate::BulletTimeInUltimate, this));

	_player->AddEventFunction(TEXT("EndBulletTimeInUltimate"),
		std::bind(&DUOLClient::PlayerState_Ultimate::EndBulletTimeInUltimate, this));

#pragma endregion

#pragma region ANIMATOR_CONTROLLER_CACHING
	_playerNormalAnimCon = DUOLGameEngine::ResourceManager::GetInstance()
		->GetAnimatorController(TEXT("Player_NormalAnimatorController"));

	_playerOverdriveSwordAnimCon = DUOLGameEngine::ResourceManager::GetInstance()
		->GetAnimatorController(TEXT("Player_Overdrive_SwordAnimatorController"));

	_playerOverdriveFistAnimCon = DUOLGameEngine::ResourceManager::GetInstance()
		->GetAnimatorController(TEXT("Player_Overdrive_FistAnimatorController"));
#pragma endregion
}

DUOLClient::PlayerState_Ultimate::~PlayerState_Ultimate()
{
}

void DUOLClient::PlayerState_Ultimate::SetUltimateSwordDamage(float ultimateSwordDamage)
{
	_ultimateSwordDamage = ultimateSwordDamage;
}

void DUOLClient::PlayerState_Ultimate::SetUltimateSwordWaveDamage(float ultimateSwordWaveDamage)
{
	_ultimateSwordWaveDamage = ultimateSwordWaveDamage;
}

void DUOLClient::PlayerState_Ultimate::SetUltimateFistDamage(float ultimateFistDamage)
{
	_ultimateFistDamage = ultimateFistDamage;
}

void DUOLClient::PlayerState_Ultimate::BulletTimeInUltimate()
{
	DUOLGameEngine::TimeManager::GetInstance()->SetTimeScale(0.1f);
	_animator->SetFloat(TEXT("AnimationSpeed"), 10.f);
}

void DUOLClient::PlayerState_Ultimate::EndBulletTimeInUltimate()
{
	_animator->SetFloat(TEXT("AnimationSpeed"), 1.f);
	DUOLGameEngine::TimeManager::GetInstance()->SetTimeScale(1.0f);
}

void DUOLClient::PlayerState_Ultimate::RunUltimateAnimation()
{
	//궁극기가 가능한 시점은 ultimate 일때만입니다.

	_animator->SetBool(TEXT("IsUltimate"), true);

	//혹시 loop 카운터가 초기화 되지 않을 경우를 대비하여 bool 값 변수 추가
	if(_ultFirstRun)
	{
		//처음시작인데 loop 카운터가 초기화가 안되어있네..?
		if(_animator->GetCurrentLoopCount() > 0)
		{
			DUOL_TRACE(DUOL_CONSOLE, "Ultimate | LoopCounter Init");
			_animator->InitializeCurrentLoopCount();
			_ultFirstRun = false;
		}
	}

	//강제종료.
	//if(_animator->GetCurrentStateName() != TEXT("Player_Overdrive_Ultimate"))
	//{
	//	EndUltimateAnimation();
	//}
}

void DUOLClient::PlayerState_Ultimate::EndUltimateAnimation()
{
	_animator->SetBool(TEXT("IsUltimate"), false);
	_animator->SetFloat(TEXT("AnimationSpeed"), 1.f);

	DUOLGameEngine::TimeManager::GetInstance()->SetTimeScale(1.0f);

	_stateMachine->TransitionTo(TEXT("PlayerState_Idle"), 0.f);
	DUOL_TRACE(DUOL_CONSOLE, "Ultimate | EndUltimate");
}

void DUOLClient::PlayerState_Ultimate::StartSwordTrail()
{
	_player->_currentDamage = _ultimateSwordDamage;
	_overdriveSwordTrail = DUOLClient::ParticleManager::GetInstance()->Pop(ParticleEnum::OverdriveSwordTrail, 0.5f);

	if (_overdriveSwordTrail == nullptr)
		return;

	_overdriveSwordTrail->GetTransform()->SetParent(_player->_currentPlayerWeapon->GetTransform());

	auto transform = _overdriveSwordTrail->GetTransform();

	transform->SetLocalPosition(DUOLMath::Vector3(-0.029f, -0.612f, -0.01f));
	transform->SetLocalEulerAngle(DUOLMath::Vector3(0.f, 0.f, 90.f));

}

void DUOLClient::PlayerState_Ultimate::EndSwordTrail()
{
	if (_overdriveSwordTrail != nullptr)
	{
		_overdriveSwordTrail->GetTransform()->SetParent(nullptr);

		_overdriveSwordTrail->GetGameObject()->SetIsActiveSelf(false);

		_overdriveSwordTrail = nullptr;
	}
}

void DUOLClient::PlayerState_Ultimate::ChargingFist()
{
	if (_rightFistFormAura == nullptr)
	{
		_rightFistFormAura = DUOLClient::ParticleManager::GetInstance()->Pop(ParticleEnum::OverdriveChargingFist_Red);
		_rightFistFormAura->GetParticleData()._commonInfo.gSimulationSpeed = 12.f;

		for(auto& child : _rightFistFormAura->GetTransform()->GetChildren())
		{
			auto partComp = child->GetGameObject()->GetComponent<DUOLGameEngine::ParticleRenderer>();
			partComp->GetParticleData()._commonInfo.gSimulationSpeed = 12.f;
		}

		_rightFistFormAura->GetTransform()->SetParent(_player->_playerRightFistHolder->GetTransform(), false);

		_rightFistFormAura->GetTransform()->SetLocalPosition(DUOLMath::Vector3::Zero);
	}

	_player->PlaySoundClip(PlayerSoundTable::FistChargingSound, false);
	_player->PlayVoiceSoundClip(PlayerVoiceSoundTable::Voice_UltimateCharging, false);

	DUOL_TRACE(DUOL_CONSOLE, "Ultimate | ChargingFist");
}

void DUOLClient::PlayerState_Ultimate::ChargingSword()
{
	int randNum = rand();

	if(randNum%2)
		_player->PlaySoundClip(PlayerSoundTable::SwordChargingSound01, false);
	else
		_player->PlaySoundClip(PlayerSoundTable::SwordChargingSound02, false);

}

void DUOLClient::PlayerState_Ultimate::EndCharging()
{
	if (_rightFistFormAura != nullptr)
	{
		_rightFistFormAura->Stop();
		_rightFistFormAura->GetParticleData()._commonInfo.gSimulationSpeed = 1.f;

		for (auto& child : _rightFistFormAura->GetTransform()->GetChildren())
		{
			auto partComp = child->GetGameObject()->GetComponent<DUOLGameEngine::ParticleRenderer>();
			partComp->GetParticleData()._commonInfo.gSimulationSpeed = 1.f;
		}

		_rightFistFormAura->GetTransform()->SetParent(nullptr, false);

		_rightFistFormAura = nullptr;
	}
	DUOL_TRACE(DUOL_CONSOLE, "Ultimate | EndCharging");
}


DUOLGameEngine::CoroutineHandler DUOLClient::PlayerState_Ultimate::LaunchAreaWave()
{
	DUOLGameEngine::Transform* playerTransform = _player->GetTransform();

	DUOLMath::Vector3  hitCenterOffset = DUOLMath::Vector3(0.f, 0.5f, 0.2f);
	float hitRadius = 2.f;
	float hitMaxDistance = 6.f;
	DUOLMath::Vector3 waveVelocity = DUOLMath::Vector3::Forward * 40.f;
	float waveTime = 1.f;
	DUOLMath::Vector3 startWaveBoxHalfExtents = DUOLMath::Vector3(6.f, 3.5f, 1.3f);
	DUOLMath::Vector3 endWaveBoxHalfExtents = DUOLMath::Vector3(6.5f, 4.f, 1.5f);


	auto particleRenderer = DUOLClient::ParticleManager::GetInstance()->Pop(ParticleEnum::OverdriveUltimateFist, 5.f);
	particleRenderer->GetTransform()->SetPosition(_transform->GetWorldPosition() + _transform->GetLook() * 1.f, DUOLGameEngine::Space::World);

	//_player->_playerWeaponWave->StartWave(playerTransform->GetWorldPosition() + hitCenterOffset, startWaveBoxHalfExtents,
	//	(endWaveBoxHalfExtents - startWaveBoxHalfExtents) / waveTime,
	//	DUOLMath::Vector3::TransformNormal(waveVelocity.Normalized(), playerTransform->GetWorldMatrix()) * waveVelocity.Length(), playerTransform->GetWorldRotation(), waveTime);

	//co_yield nullptr;

	_player->_playerWeaponAreaWave->StartAreaWave(playerTransform->GetWorldPosition(), playerTransform->GetWorldRotation(), waveTime, PlayerSoundTable::Hit_Sound);
	_player->PlaySoundClipAndVoice(PlayerSoundTable::UltimateMagnumPunch,PlayerVoiceSoundSet::Overdrive_FistUlt);


	co_yield std::make_shared<DUOLGameEngine::WaitForSeconds>(waveTime);

	_player->_playerWeaponAreaWave->EndAreaWave();

	DUOL_TRACE(DUOL_CONSOLE, "Ultimate | LaunchWave");
}

DUOLGameEngine::CoroutineHandler DUOLClient::PlayerState_Ultimate::LaunchWave()
{
	DUOLGameEngine::Transform* playerTransform = _player->GetTransform();

	DUOLMath::Vector3  hitCenterOffset = DUOLMath::Vector3(0.f, 0.5f, 0.2f);
	float hitRadius = 2.f;
	float hitMaxDistance = 6.f;
	DUOLMath::Vector3 waveVelocity = DUOLMath::Vector3::Forward * 50.f;
	float waveTime = 0.3f;
	DUOLMath::Vector3 startWaveBoxHalfExtents = DUOLMath::Vector3(5.f, 2.5f, 0.75f) * 2;
	DUOLMath::Vector3 endWaveBoxHalfExtents = DUOLMath::Vector3(5.75f, 3.f, 1.f) * 2;


	auto particleRenderer = DUOLClient::ParticleManager::GetInstance()->Pop(ParticleEnum::OverdriveLastSword, 5.f);
	particleRenderer->GetTransform()->SetPosition(_transform->GetWorldPosition() + _transform->GetLook() * 1.f, DUOLGameEngine::Space::World);

	_player->_playerWeaponWave->StartWave(playerTransform->GetWorldPosition() + hitCenterOffset, startWaveBoxHalfExtents,
		(endWaveBoxHalfExtents - startWaveBoxHalfExtents) / waveTime,
		DUOLMath::Vector3::TransformNormal(waveVelocity.Normalized(), playerTransform->GetWorldMatrix()) * waveVelocity.Length(), playerTransform->GetWorldRotation(), waveTime, PlayerSoundTable::Hit_Sound);
	_player->PlaySoundClipAndVoice(PlayerSoundTable::OverdriveSwordFinal, PlayerVoiceSoundSet::Overdrive_FistUlt);

	co_yield nullptr;

	//_player->_playerWeaponAreaWave->StartAreaWave(playerTransform->GetWorldPosition(), playerTransform->GetWorldRotation(), waveTime);
	//co_yield std::make_shared<DUOLGameEngine::WaitForSeconds>(waveTime);
	//_player->_playerWeaponAreaWave->EndAreaWave();

	DUOL_TRACE(DUOL_CONSOLE, "Ultimate | LaunchAreaWave");
}

void DUOLClient::PlayerState_Ultimate::FistWaveHit()
{
	// 충격파 계열
	if (!_isOnStay)
		return;

	_player->_currentDamage = _ultimateFistDamage;
	// 웨이브 공격 날립니다.
	std::function<DUOLGameEngine::CoroutineHandler(void)> routine = std::bind(&DUOLClient::PlayerState_Ultimate::LaunchAreaWave, this);
	_player->StartCoroutine(routine);

	DUOLGameEngine::Transform* playerTransform = _player->GetTransform();


	// (Wave Hit 계열 == 막타) 만 카메라 쉐이크가 있다.
	_mainCamController->SetCameraShake(0.5f, DUOLMath::Vector2(5.f, 5.f) * 2.f);

	// Cannot Dash
	//std::function<DUOLGameEngine::CoroutineHandler()> cantDash = std::bind(&DUOLClient::PlayerState_Attack::SetCannotDash, this);
	//_player->StartCoroutine(cantDash);
	DUOL_TRACE(DUOL_CONSOLE, "Ultimate | FistWave");
}

void DUOLClient::PlayerState_Ultimate::SwordWaveHit()
{
	// 충격파 계열
	if (!_isOnStay)
		return;

	// 웨이브 공격 날립니다.
	_player->_currentDamage = _ultimateSwordWaveDamage;

	std::function<DUOLGameEngine::CoroutineHandler(void)> routine = std::bind(&DUOLClient::PlayerState_Ultimate::LaunchWave, this);
	_player->StartCoroutine(routine);

	DUOLGameEngine::Transform* playerTransform = _player->GetTransform();


	// (Wave Hit 계열 == 막타) 만 카메라 쉐이크가 있다.
	_mainCamController->SetCameraShake(0.5f, DUOLMath::Vector2(5.f, 5.f) * 2.f);

	// Cannot Dash
	//std::function<DUOLGameEngine::CoroutineHandler()> cantDash = std::bind(&DUOLClient::PlayerState_Attack::SetCannotDash, this);
	//_player->StartCoroutine(cantDash);
	DUOL_TRACE(DUOL_CONSOLE, "Ultimate | FistWave");
}

DUOLGameEngine::CoroutineHandler  DUOLClient::PlayerState_Ultimate::StartBulletTIme(float duration)
{
		DUOLGameEngine::TimeManager::GetInstance()->SetTimeScale(0.5f);

		co_yield std::make_shared<DUOLGameEngine::WaitForSecondsRealtime>(duration);

		DUOLGameEngine::TimeManager::GetInstance()->SetTimeScale(1.f);
}
void DUOLClient::PlayerState_Ultimate::PlaySoundEffect(PlayerSoundTable sounds)
{
	_player->PlaySoundClip(sounds, false);
}

void DUOLClient::PlayerState_Ultimate::OnStateEnter(float deltaTime)
{
	PlayerStateBase::OnStateEnter(deltaTime);

	_ultFirstRun = true;
	_player->_isUltimate = true;
	RunUltimateAnimation();

	//카메라 이벤트도 여기다 넣도록하자. 혹은 애니메이션에다 넣어도 괜찮을듯.

	DUOL_TRACE(DUOL_CONSOLE, "Ultimate | StateEnter");
}

void DUOLClient::PlayerState_Ultimate::OnStateStay(float deltaTime)
{
	PlayerStateBase::OnStateStay(deltaTime);

	//혹시 loop 카운터가 초기화 되지 않을 경우를 대비하여 bool 값 변수 추가
	if (_ultFirstRun)
	{
		//처음시작인데 loop 카운터가 초기화가 안되어있네..?
		if (_animator->GetCurrentLoopCount() > 0)
		{
			DUOL_TRACE(DUOL_CONSOLE, "Ultimate | LoopCounter Init");
			_animator->InitializeCurrentLoopCount();
			_ultFirstRun = false;
		}
	}

	//RunUltimateAnimation();
}

void DUOLClient::PlayerState_Ultimate::OnStateExit(float deltaTime)
{
	PlayerStateBase::OnStateExit(deltaTime);
	_player->_isUltimate = false;

	_animator->SetBool(TEXT("IsUltimate"), false);
}
