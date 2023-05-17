#include "DUOLClient/BehaviorTreeNode/Action/Action_RushPattern.h"
#include "DUOLClient/ECS/Component/Enemy/AI_EnemyBasic.h"
#include "DUOLGameEngine/ECS/Component/Animator.h"
#include "DUOLGameEngine/ECS/Component/Transform.h"
#include "DUOLGameEngine/Manager/TimeManager.h"

BT::NodeStatus DUOLClient::Action_RushPattern::onStart()
{
	if (!_ai)
	{
		_ai = getInput<DUOLClient::AI_EnemyBasic*>("AI").value();

		_transform = _ai->GetParentTransform();

		_targetTransform = _ai->GetTargetTransform();

		_animator = _ai->GetAnimator();
	}

	_ai->SetSuperArmor(true);

	_rushCount = 0;

	_timer = 0;

	_isIdle = false;

	_ai->SetParameter(TEXT("IsRushHit_Target"), false);

	return BT::NodeStatus::RUNNING;

}

BT::NodeStatus DUOLClient::Action_RushPattern::onRunning()
{

	if (TEXT("Idle") == _animator->GetCurrentStateName() && _rushCount < 3)
	{
		_animator->SetBool(TEXT("IsRush"), true);
		_isIdle = true;
	}

	if (TEXT("Rush") == _animator->GetCurrentStateName())
	{
		_timer += DUOLGameEngine::TimeManager::GetInstance()->GetDeltaTime();

		if (_isIdle)
		{
			_isIdle = false;
			_rushCount++;
		}
	}

	auto distance = DUOLMath::Vector3::Distance(_transform->GetWorldPosition(), _targetTransform->GetWorldPosition());

	auto enemyToTarget = _targetTransform->GetWorldPosition() - _transform->GetWorldPosition();
	if (distance >= 5 && 0 < _transform->GetLook().Dot(enemyToTarget))
		_ai->LerpLookTarget();
	else if (0 > _transform->GetLook().Dot(enemyToTarget)) // 플레이어를 지나쳤다면,
	{
		if (_timer > 3 || distance >= 8) // 일정 시간이 지났거나 거리가 멀어지면 종료한다.
		{
			_animator->SetBool(TEXT("IsRush"), false);
			_timer = 0;

			if (_rushCount >= 3 || _ai->GetParameter<bool>(TEXT("IsRushHit_Target")))
			{
				_ai->SetSuperArmor(false, _ai->GetSuperArmorTime());

				return BT::NodeStatus::SUCCESS;
			}
		}
	}

	return BT::NodeStatus::RUNNING;
}

void DUOLClient::Action_RushPattern::onHalted()
{
}

BT::PortsList DUOLClient::Action_RushPattern::providedPorts()
{
	BT::PortsList result = {
BT::InputPort<DUOLClient::AI_EnemyBasic* >("AI")
	};

	return result;
}
