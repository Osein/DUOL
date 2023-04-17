#include "DUOLClient/BehaviorTreeNode/Condition/Condition_IsLookTarget.h"

#include "DUOLClient/ECS/Component/Enemy/AI_EnemyBasic.h"
#include "DUOLGameEngine/ECS/GameObject.h"

namespace DUOLClient
{
	BT::NodeStatus Condition_IsLookTarget::tick()
	{
		if (!_gameObject)
		{
			_gameObject = getInput<DUOLGameEngine::GameObject*>("GameObject").value();

			_targetTransform = getInput<DUOLGameEngine::Transform*>("TargetTransform").value();

			_range = getInput<float>("Range").value();

			_ai = getInput<AI_EnemyBasic*>("AI").value();
		}

		auto tr = _gameObject->GetTransform();

		if (_targetTransform)
		{
			if (DUOLMath::Vector3::Distance(_targetTransform->GetWorldPosition(), tr->GetWorldPosition()) > _range) // 거리 밖이라면..
				return BT::NodeStatus::FAILURE;

			auto look = _targetTransform->GetWorldPosition() - tr->GetWorldPosition();
			look.Normalize();

			if (look.Dot(tr->GetLook()) > 0) // 타겟이 시야에 들어온다면... 양수라면 시야에 들어온 것이다..!
			{
				std::vector<DUOLPhysics::RaycastHit> hit;

				// 오브젝트의 피벗이 발에 있어서 레이가 발바닥에서 나간다...! 위로 옵셋 값을 조금 주자..!

				DUOLMath::Vector3 offset;

				offset.y = 1.5f;

				DUOLGameEngine::PhysicsManager::GetInstance()->RaycastAll(tr->GetWorldPosition() + offset, look, _range, hit);

				bool isBlock = false;

				for (auto& iter : hit) // 충돌체 중에 나와 상대가 아닌 것이 있다면 가리고 있다는 것.
				{
					if (static_cast<DUOLGameEngine::GameObject*>(iter._userData) != _gameObject &&
						static_cast<DUOLGameEngine::GameObject*>(iter._userData) != _targetTransform->GetGameObject() &&
						static_cast<DUOLGameEngine::GameObject*>(iter._userData)->GetLayer() != TEXT("EnemyRigidbody")
						)
					{
						isBlock = true;
						break;
					}
				}

				if (!isBlock)
				{
					_ai->SetIsGroupCheck();
					return BT::NodeStatus::SUCCESS;
				}

			}
		}
		return BT::NodeStatus::FAILURE;
	}

	BT::PortsList Condition_IsLookTarget::providedPorts()
	{
		BT::PortsList result = {
		BT::InputPort<DUOLGameEngine::GameObject*>("GameObject"),
		BT::InputPort<DUOLGameEngine::Transform*>("TargetTransform"),
		BT::InputPort<float>("Range"),
		BT::InputPort<AI_EnemyBasic*>("AI")
		};

		return result;
	}
}
