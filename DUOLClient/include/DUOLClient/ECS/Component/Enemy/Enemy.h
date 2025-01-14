﻿/**

	@file      Enemy.h
	@brief     Enemy의 현재 정보 및 기능을 가지고 있는 클래스.
	@details   ~
	@author    Cool Guy
	@date      23.03.2023
	@copyright © Cool Guy, 2023. All right reserved.

**/
#pragma once

#include "DUOLClient/ECS/Component/CharacterBase.h"
#include "DUOLCommon/Log/LogHelper.h"

namespace DUOLGameEngine
{
	class AudioSource;
	class Material;
	class SkinnedMeshRenderer;
	class Rigidbody;
	class CapsuleCollider;
	class Animator;
	class NavMeshAgent;
}

namespace DUOLClient
{
	enum class EnemyAudioEnum;
	class EnemyAirborneCheck;
	class EnemyParentObjectObserver;
	class AI_EnemyBasic;
	enum class EnemyCode;
	struct EnemyData;

	enum class HitEnum // 히트의 종류는 현재 두가지
	{
		Front,
		Back,
	};

	enum class EnemyMaterial
	{
		RED,
		WHITE,
		DIE,
		APPEAR,
		NORMAL,
	};

	class DUOL_CLIENT_API Enemy : public CharacterBase
	{
	public:
		Enemy(DUOLGameEngine::GameObject* owner = nullptr, const DUOLCommon::tstring& name = TEXT("Enemy"));

		virtual ~Enemy() override;

	private:
		DUOLGameEngine::GameObject* _target = nullptr;

		DUOLGameEngine::NavMeshAgent* _navMeshAgent;

		DUOLGameEngine::Animator* _animator;

		DUOLGameEngine::CapsuleCollider* _capsuleCollider;

		DUOLGameEngine::CapsuleCollider* _parentCapsuleCollider;

		DUOLGameEngine::Rigidbody* _rigidbody;

		DUOLGameEngine::GameObject* _parentGameObject; // 물리 연산을 수행하기 위한 부모..! 

		DUOLGameEngine::Transform* _transform;

		DUOLGameEngine::AudioSource* _audioSource;

		DUOLGameEngine::AudioSource* _voiceAudioSource;

		EnemyAirborneCheck* _enemyAirborneCheck;

		EnemyParentObjectObserver* _parentObserver;

		std::vector<std::pair<DUOLCommon::tstring, DUOLCommon::EventListenerID>> _eventListenerIDs;

		const EnemyData* _enemyData;

		AI_EnemyBasic* _ai;

		DUOLGameEngine::SkinnedMeshRenderer* _skinnedMeshRenderer;

		std::vector<DUOLGameEngine::Material*> _originMaterials;

		EnemyMaterial _currentMaterial;

		HitEnum _hitEnum; // 이전과 다른 동작을 취해야한다...!

		EnemyAudioEnum _currentSoundEnum;

		EnemyAudioEnum _currentVoiceSoundEnum;

		std::function<bool(DUOLClient::Enemy*, CharacterBase*, float, AttackType)> _hitFunc;

		std::unordered_map<DUOLCommon::tstring, float> _floatParmeter;

		std::unordered_map<DUOLCommon::tstring, bool> _boolParmeter;

		std::unordered_map<DUOLCommon::tstring, void*> _voidParmeter;

		int _superArmorStack;

		bool _dashUsable;

		//러쉬에서 쓰는 value이다.. 흑흑 임시부울값
		bool _currentPaternHitPlayer;

	public:

		template<class T>
		void AddParameter(DUOLCommon::tstring key, T value);

		template<class T>
		void SetParameter(DUOLCommon::tstring key, T value);

		template<class T>
		T GetParameter(DUOLCommon::tstring key);

		template<class T>
		bool GetContainsParameter(DUOLCommon::tstring key);

		void InitializeData();

		void SetPosition(DUOLMath::Vector3 pos);

		void SetPosition(const float& x, const float& y, const float& z);

		void SetEnemyCode(EnemyData* enemyData);

		void EnemyAddEventFunc(const DUOLCommon::tstring& eventName, std::function<void()> functor);

		void SetEnemyHitFunc(std::function<bool(DUOLClient::Enemy*, CharacterBase*, float, AttackType)> func);

		void PlaySound(EnemyAudioEnum audioEnum, bool isOverride, unsigned offset = 0);

		void PlayVoiceSound(EnemyAudioEnum audioEnum, bool isOverride, unsigned offset = 0);

		void ReturnAllSound();

		//void IncreaseSuperArmorStack();

		//void DecreaseSuperArmorStack();

		//int GetSuperArmorStack();

		DUOLGameEngine::CoroutineHandler SoundStopCheck();

		DUOLGameEngine::CoroutineHandler VoiceSoundStopCheck();

		DUOLGameEngine::SkinnedMeshRenderer* GetSkinnedMeshRenderer();

		const EnemyData* GetEnemyData();

		bool GetIsAirBorne();

		AI_EnemyBasic* GetAIController();

		DUOLGameEngine::Rigidbody* GetRigidbody() const;

		DUOLGameEngine::Animator* GetAnimator() const;

		DUOLGameEngine::NavMeshAgent* GetNavMeshAgent() const;

		HitEnum GetHitEnum() const;

		void SetHitEnum(HitEnum hitEnum);

		void ChangeMaterial(EnemyMaterial enemyMat);

		void ChangeMaterialOnHit();

		void SetNavOnRigidbodyOff();

		void SetNavOffRigidbodyOn();

		void SetNavEnable(bool isBool);

		DUOLGameEngine::GameObject* GetTarget() const;

		DUOLGameEngine::Transform* GetParentTransform() const;

		void SetColiiderEnable(bool isBool);

		void LerpLookTarget();

		void PushedOut(DUOLMath::Vector3 otherPos);

		// 외부에서 코루틴을 접근할 수 있도록 만든 함수.
		void StartCoroutine_Manual(std::function<DUOLGameEngine::CoroutineHandler()> func);

		bool IsCurrentPaternHitPlayer() const;

		void SetCurrentPaternHitPlayer(bool currentPaternHitPlayer);

	public:
		virtual void OnEnable() override;

		virtual bool Attack(CharacterBase* target, float damage, AttackType attackType, float downPoint = DOWN_POINT_PER_ATTACK) override;

		virtual bool OnHit(CharacterBase* other, float damage, AttackType attackType, float downPoint = DOWN_POINT_PER_ATTACK) override;

		virtual bool GetIsInvincible() override;

		virtual void OnAwake() override;

		virtual void OnUpdate(float deltaTime) override;

		bool GetDashUsable() { return _dashUsable; }

		void SetDashUsable(bool dash);

		friend class AI_EnemyBasic;

		RTTR_ENABLE(CharacterBase)

			RTTR_REGISTRATION_FRIEND
	};

	template <>
	inline void Enemy::AddParameter<float>(DUOLCommon::tstring key, float value)
	{
		if (!_floatParmeter.contains(key))
			_floatParmeter.insert({ key, value });
	}

	template <>
	inline void Enemy::AddParameter<bool>(DUOLCommon::tstring key, bool value)
	{
		if (!_boolParmeter.contains(key))
			_boolParmeter.insert({ key, value });
	}

	template <>
	inline void Enemy::AddParameter<void*>(DUOLCommon::tstring key, void* value)
	{
		if (!_voidParmeter.contains(key))
			_voidParmeter.insert({ key, value });
	}

	template <>
	inline void Enemy::SetParameter(DUOLCommon::tstring key, float value)
	{
		if (_floatParmeter.contains(key))
			_floatParmeter[key] = value;
		//else
			//DUOL_TRACE(DUOL_CONSOLE, "Enemy Has Not Parameter : {0}", DUOLCommon::StringHelper::ToString(key));
	}

	template <>
	inline void Enemy::SetParameter(DUOLCommon::tstring key, bool value)
	{
		if (_boolParmeter.contains(key))
			_boolParmeter[key] = value;
	//	else
			//DUOL_TRACE(DUOL_CONSOLE, "Enemy Has Not Parameter : {0}", DUOLCommon::StringHelper::ToString(key));
	}

	template <>
	inline void Enemy::SetParameter(DUOLCommon::tstring key, void* value)
	{
		if (_voidParmeter.contains(key))
			_voidParmeter[key] = value;
		//else
			//DUOL_TRACE(DUOL_CONSOLE, "Enemy Has Not Parameter : {0}", DUOLCommon::StringHelper::ToString(key));
	}

	template <>
	inline float Enemy::GetParameter(DUOLCommon::tstring key)
	{
		if (_floatParmeter.contains(key))
			return _floatParmeter[key];
		//else
		//	DUOL_TRACE(DUOL_CONSOLE, "Enemy Has Not Parameter : {0}", DUOLCommon::StringHelper::ToString(key));

		return 0;
	}

	template <>
	inline bool Enemy::GetParameter(DUOLCommon::tstring key)
	{
		if (_boolParmeter.contains(key))
			return _boolParmeter[key];
		//else
		//	DUOL_TRACE(DUOL_CONSOLE, "Enemy Has Not Parameter : {0}", DUOLCommon::StringHelper::ToString(key));

		return false;
	}

	template <>
	inline void* Enemy::GetParameter(DUOLCommon::tstring key)
	{
		if (_voidParmeter.contains(key))
			return _voidParmeter[key];
		//else
		//	DUOL_TRACE(DUOL_CONSOLE, "Enemy Has Not Parameter : {0}", DUOLCommon::StringHelper::ToString(key));

		return nullptr;
	}

	template <>
	inline bool Enemy::GetContainsParameter<float>(DUOLCommon::tstring key)
	{
		return _floatParmeter.contains(key);
	}

	template <>
	inline bool Enemy::GetContainsParameter<bool>(DUOLCommon::tstring key)
	{
		return _boolParmeter.contains(key);
	}
}
