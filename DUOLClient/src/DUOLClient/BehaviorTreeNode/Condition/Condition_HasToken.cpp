#include "DUOLClient/BehaviorTreeNode/Condition/Condition_HasToken.h"

#include "DUOLClient/ECS/Component/Enemy/AI_EnemyBase.h"
#include "DUOLGameEngine/ECS/GameObject.h"

BT::NodeStatus DUOLClient::Condition_HasToken::tick()
{
	if (!_gameObject)
	{
		_gameObject = getInput<DUOLGameEngine::GameObject*>("GameObject").value();

		_ai = getInput<AI_EnemyBase*>("AI").value();
	}

	if (_ai->GetIsToken())
		return BT::NodeStatus::SUCCESS;
	else
		return BT::NodeStatus::FAILURE;
}

BT::PortsList DUOLClient::Condition_HasToken::providedPorts()
{
	BT::PortsList result = {
		BT::InputPort<DUOLGameEngine::GameObject*>("GameObject"),
		BT::InputPort<AI_EnemyBase*>("AI")
	};
	return result;
}
