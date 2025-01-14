#pragma once

#include "DUOLClient/ECS/Component/Enemy/Enemy.h"
#include "DUOLClient/ECS/Component/Enemy/AI_EnemyBasic.h"
#include "DUOLGameEngine/ECS/Component/Transform.h"
#include "DUOLGameEngine/ECS/Component/Animator.h"
#include "DUOLGameEngine/ECS/Component/Rigidbody.h"

namespace DUOLClient
{
	bool NormalEnemyHit(DUOLClient::Enemy* thisEnemy, CharacterBase* other, float damage, AttackType attackType)
	{
		if (thisEnemy->GetIsInvincible())
			return false;

		const auto ai = thisEnemy->GetAIController();
		const auto animator = ai->GetAnimator();
		const auto parentTr = thisEnemy->GetParentTransform();

		if (animator->GetBool(TEXT("IsAirBorne"))) return false;

		if (!thisEnemy->GetIsDie())
		{
			ai->SetAnimConditionReset();
		}
		animator->SetFloat(TEXT("RandOffset"), DUOLMath::MathHelper::RandF(0, 0.5f));

		thisEnemy->SetParameter(TEXT("IsHit"), true);

		thisEnemy->SetHP(thisEnemy->GetHP() - damage);



		if (ai->GetAnimator()->GetSpeed() > 0.0f)
		{
			switch (thisEnemy->GetHitEnum())
			{
			case HitEnum::Front:
				animator->SetBool(TEXT("IsHit_Front"), true);
				thisEnemy->SetHitEnum(HitEnum::Back);
				break;
			case HitEnum::Back:
				animator->SetBool(TEXT("IsHit_Back"), true);
				thisEnemy->SetHitEnum(HitEnum::Front);
				break;
			default:
				break;
			}
		}

		if (!other) return true;

		switch (attackType)
		{
		case AttackType::HeavyAttack:
		{
			ai->SetNavOffRigidbodyOn(); // 바닥에 닿았을 때 다시 켜줘야한다.

			auto dir = thisEnemy->GetTransform()->GetWorldPosition() - other->GetTransform()->GetWorldPosition();
			dir.Normalize();
			const auto height = DUOLMath::MathHelper::RandF(12.0f, 15.0f);
			dir = dir * DUOLMath::MathHelper::RandF(12.0f, 15.0f) + DUOLMath::Vector3(0, height, 0);

			// 질량 줄이니까 난리나는데 ? => 질량과 관련된 문제
			thisEnemy->GetRigidbody()->AddImpulse(dir * 0.5f);

			if (animator->GetSpeed() > 0)
				animator->SetBool(TEXT("IsAirBorne"), true); // 공중 피격 애니메이션과 사망애니메이션이 같다.
		}
		break;
		default:
		{
			auto worldPos = parentTr->GetWorldPosition();
			auto dir = other->GetTransform()->GetLook();
			auto physManager = DUOLGameEngine::PhysicsManager::GetInstance();

			//navMesh때문에 물리로는 절때 안된다는걸 깨달아버렸다.....
			//DUOLPhysics::RaycastHit rayInfo;
			//DUOLGameEngine::PhysicsManager::GetInstance()->Raycast(worldPos, dir, 1.f, rayInfo);

			//if (rayInfo._isBlocking == false)
			//{
			//3에는 mass가 들어가야하긴하는데... 흠 매번가져올까?
			//dir.y = 0.f;
			//dir.Normalize();
			//auto mass = thisEnemy->GetRigidbody()->GetMass();
			//thisEnemy->GetRigidbody()->AddImpulse(dir * mass);
			//}
			//else
			//{
			//	auto hitPower = dir * rayInfo._hitDistance;
			//	//3에는 mass가 들어가야하긴하는데... 흠 매번가져올까?
			//	hitPower.y = 0.f;
			//	auto mass = thisEnemy->GetRigidbody()->GetMass();
			//	thisEnemy->GetRigidbody()->AddImpulse(hitPower * mass);
			//}

			DUOLPhysics::RaycastHit rayInfo;
			static int layerObstacle = physManager->GetLayerNumber(TEXT("Obstacle"));
			//현재포지션의 약간의 높이를 주자
			constexpr DUOLMath::Vector3 heightOffset = { 0.f, 0.7f, 0.f };
			//
			constexpr float extendCollider = 0.4f;
			DUOLGameEngine::PhysicsManager::GetInstance()->Raycast(worldPos + heightOffset, dir, 2.f, layerObstacle, rayInfo);

			if (rayInfo._isBlocking)
			{
				thisEnemy->SetPosition(worldPos + dir * std::min<float>(rayInfo._hitDistance - extendCollider, 1.f));
			}
			else
			{
				thisEnemy->SetPosition(worldPos + dir);
			}
		}
		break;
		}
		return true;
	}

	bool WeakEliteEnemyHit(DUOLClient::Enemy* thisEnemy, CharacterBase* other, float damage, AttackType attackType)
	{
		const auto ai = thisEnemy->GetAIController();
		const auto animator = ai->GetAnimator();

		if (!thisEnemy->GetIsDie())
			ai->SetAnimConditionReset();
		else
			return false;

		thisEnemy->SetParameter(TEXT("IsHit"), true);

		thisEnemy->SetHP(thisEnemy->GetHP() - damage);

		if (thisEnemy->GetParameter<bool>(TEXT("IsSuperArmor")))
		{
			thisEnemy->ChangeMaterialOnHit();
			return false;
		}

		switch (thisEnemy->GetHitEnum())
		{
		case HitEnum::Front:
			animator->SetBool(TEXT("IsHit_Front"), true);
			thisEnemy->SetHitEnum(HitEnum::Back);
			break;
		case HitEnum::Back:
			animator->SetBool(TEXT("IsHit_Back"), true);
			thisEnemy->SetHitEnum(HitEnum::Front);
			break;
		default:
			break;
		}

		if (thisEnemy->GetParameter<bool>(TEXT("IsCanSuperArmor"))) // 쿨타임이 다 지났다면,
		{
			thisEnemy->GetAIController()->AddSuperArmorGauge(damage);

			// 게이지를 넘으면 슈퍼아머 활성화 시키면서 콤보 어택 실행
			if (thisEnemy->GetAIController()->GetCurrentSuperArmorGauge() >= thisEnemy->GetAIController()->GetMaxSuperArmorGauge())
			{
				ai->SetAnimConditionReset();
				ai->SetSuperArmor(true, thisEnemy->GetAIController()->GetSuperArmorTime());
				animator->SetBool(TEXT("IsComboAttack"), true);
				thisEnemy->ChangeMaterialOnHit();
			}
		}
		return true;
	}

	bool EliteEnemyHit(DUOLClient::Enemy* thisEnemy, CharacterBase* other, float damage, AttackType attackType)
	{
		const auto ai = thisEnemy->GetAIController();
		const auto animator = ai->GetAnimator();

		if (thisEnemy->GetIsDie()) return false;

		thisEnemy->SetParameter(TEXT("IsHit"), true);

		thisEnemy->SetHP(thisEnemy->GetHP() - damage);

		if (!thisEnemy->GetParameter<bool>(TEXT("IsSuperArmor")) && !thisEnemy->GetParameter<bool>(TEXT("IsGroggy")))
			ai->SetAnimConditionReset();
		else
		{
			thisEnemy->ChangeMaterialOnHit();
			return true;
		}


		switch (thisEnemy->GetHitEnum())
		{
		case HitEnum::Front:
			animator->SetBool(TEXT("IsHit_Front"), true);
			thisEnemy->SetHitEnum(HitEnum::Back);
			break;
		case HitEnum::Back:
			animator->SetBool(TEXT("IsHit_Back"), true);
			thisEnemy->SetHitEnum(HitEnum::Front);
			break;
		default:
			break;
		}

		if (thisEnemy->GetParameter<bool>(TEXT("IsCanSuperArmor"))) // 쿨타임이 다 지났다면,
		{
			thisEnemy->GetAIController()->AddSuperArmorGauge(damage);

			// 게이지를 넘으면 슈퍼아머 활성화 시키면서 콤보 어택 실행
			if (thisEnemy->GetAIController()->GetCurrentSuperArmorGauge() >= thisEnemy->GetAIController()->GetMaxSuperArmorGauge())
			{
				ai->SetAnimConditionReset();
				ai->SetSuperArmor(true);
				animator->SetBool(TEXT("IsComboAttack"), true);
				thisEnemy->SetParameter(TEXT("IsOnSuperArmorEvent"), true);
				thisEnemy->ChangeMaterialOnHit();
			}
		}
		return true;
	}

	bool BossRandomDash(DUOLClient::Enemy* thisEnemy, DUOLGameEngine::Animator* animator)
	{
		if (thisEnemy->GetDashUsable()) return true;

		if (animator->GetBool(TEXT("IsUltimate")) || animator->GetBool(TEXT("IsGroggy"))) return true;

		if (!thisEnemy->GetParameter<bool>(TEXT("IsSuperArmor")))
		{
			int currentHitCount = thisEnemy->GetParameter<float>(TEXT("CurrentHitCount"));
			int maxHitCount = thisEnemy->GetParameter<float>(TEXT("MaxHitCount"));
			currentHitCount++;

			if (maxHitCount <= currentHitCount)
			{
				currentHitCount = 0;

				auto percent = DUOLMath::MathHelper::RandF(0, 100.0f);

				if (percent <= 60.0f)
				{
					animator->SetBool(TEXT("IsDash"), true);
					thisEnemy->SetParameter(TEXT("CurrentHitCount"), static_cast<float>(currentHitCount));
					return false;
				}
			}
			thisEnemy->SetParameter(TEXT("CurrentHitCount"), static_cast<float>(currentHitCount));
		}

		return true;
	}

	void BossHpAndFormChageHpUpdate(DUOLClient::Enemy* thisEnemy, float damage, DUOLGameEngine::Animator* animator)
	{
		thisEnemy->SetParameter(TEXT("IsHit"), true);

		thisEnemy->SetHP(thisEnemy->GetHP() - damage);

		if (animator->GetBool(TEXT("IsGroggy"))) return;

		if (thisEnemy->GetParameter<bool>(TEXT("IsCanGroggy")))
		{
			float currentGauge = thisEnemy->GetParameter<float>(TEXT("CurrentGroggyGauge")) + damage;
			thisEnemy->SetParameter(TEXT("CurrentGroggyGauge"), currentGauge);

			if (currentGauge > thisEnemy->GetParameter<float>(TEXT("MaxGroggyGauge")))
				animator->SetBool(TEXT("IsGroggy"), true);
		}

		if (animator->GetBool(TEXT("IsUltimate"))) return;

		float formChangeHp = thisEnemy->GetParameter<float>(TEXT("FormChangeHP"));
		float currentFormChangeHp = thisEnemy->GetParameter<float>(TEXT("CurrentFormChangeHP")) + damage;

		if (formChangeHp <= currentFormChangeHp)
		{
			animator->SetBool(TEXT("IsUltimate"), true);

			thisEnemy->SetParameter(TEXT("IsCanGroggy"), true);

			thisEnemy->SetParameter(TEXT("CurrentGroggyTime"), 0.0f);

			thisEnemy->SetParameter(TEXT("MaxGroggyGauge"), true);

			thisEnemy->SetParameter(TEXT("CurrentGroggyGauge"), 0.0f);

			thisEnemy->GetAIController()->SetSuperArmor(true);

			currentFormChangeHp = currentFormChangeHp - formChangeHp;
		}

		thisEnemy->SetParameter(TEXT("CurrentFormChangeHP"), currentFormChangeHp);
	}

	bool BossHitUpdate(DUOLClient::Enemy* thisEnemy, DUOLClient::AI_EnemyBasic* ai, DUOLGameEngine::Animator* animator)
	{
		if (!thisEnemy->GetParameter<bool>(TEXT("IsSuperArmor")) && !animator->GetBool(TEXT("IsUltimate")) && !animator->GetBool(TEXT("IsGroggy")))
		{
			std::vector<std::pair<DUOLCommon::tstring, bool>> saveConditions;

			saveConditions.push_back({ TEXT("IsFistForm"),animator->GetBool(TEXT("IsFistForm")) });
			saveConditions.push_back({ TEXT("IsSwordForm"),animator->GetBool(TEXT("IsSwordForm")) });
			saveConditions.push_back({ TEXT("IsFormChange"),animator->GetBool(TEXT("IsFormChange")) });

			ai->SetAnimConditionReset();

			for (auto& iter : saveConditions)
				animator->SetBool(iter.first, iter.second);
		}
		else
		{
			thisEnemy->ChangeMaterialOnHit();
			return true;
		}

		if (!thisEnemy->GetIsDie())
		{
			switch (thisEnemy->GetHitEnum())
			{
			case HitEnum::Front:
				animator->SetBool(TEXT("IsHit_Front"), true);
				thisEnemy->SetHitEnum(HitEnum::Back);
				break;
			case HitEnum::Back:
				animator->SetBool(TEXT("IsHit_Back"), true);
				thisEnemy->SetHitEnum(HitEnum::Front);
				break;
			default:
				break;
			}
		}

		return false;
	}

	void StartSuperArmor(DUOLClient::Enemy* thisEnemy, float damage, DUOLClient::AI_EnemyBasic* ai, DUOLGameEngine::Animator* animator)
	{
		if (thisEnemy->GetParameter<bool>(TEXT("IsCanSuperArmor"))) // 쿨타임이 다 지났다면,
		{
			thisEnemy->GetAIController()->AddSuperArmorGauge(damage);

			// 게이지를 넘으면 슈퍼아머 활성화 시키면서 콤보 어택 실행
			if (thisEnemy->GetAIController()->GetCurrentSuperArmorGauge() >= thisEnemy->GetAIController()->GetMaxSuperArmorGauge())
			{
				ai->SetSuperArmor(true);

				std::vector<std::pair<DUOLCommon::tstring, bool>> saveConditions;

				saveConditions.push_back({ TEXT("IsFistForm"),animator->GetBool(TEXT("IsFistForm")) });
				saveConditions.push_back({ TEXT("IsSwordForm"),animator->GetBool(TEXT("IsSwordForm")) });
				saveConditions.push_back({ TEXT("IsFormChange"),animator->GetBool(TEXT("IsFormChange")) });

				ai->SetAnimConditionReset();

				for (auto& iter : saveConditions)
					animator->SetBool(iter.first, iter.second);

				animator->SetBool(TEXT("IsHeavyAttack"), true);

				thisEnemy->ChangeMaterialOnHit();
			}
		}
	}

	bool BossEnemyHit(DUOLClient::Enemy* thisEnemy, CharacterBase* other, float damage, AttackType attackType)
	{
		if (thisEnemy->GetIsInvincible())
			return false;

		const auto ai = thisEnemy->GetAIController();
		const auto animator = ai->GetAnimator();

		if (thisEnemy->GetIsDie()) return false;

		if (animator->GetCurrentStateName() == TEXT("Dash")) return false;

		if (animator->GetBool(TEXT("IsFormChange"))) return false;

		if (!animator->GetBool(TEXT("IsUltimate"))) thisEnemy->SetParameter(TEXT("IsCanGroggy"), false);

		if (!BossRandomDash(thisEnemy, animator)) return false;

		BossHpAndFormChageHpUpdate(thisEnemy, damage, animator);

		if (BossHitUpdate(thisEnemy, ai, animator)) return true;

		StartSuperArmor(thisEnemy, damage, ai, animator);

		return true;
	}
}

