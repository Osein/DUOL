#pragma once
#include "DUOLClient/Export_Client.h"
#include "DUOLGameEngine/ECS/Component/MonoBehaviourBase.h"

namespace DUOLClient
{
	class DUOL_CLIENT_API PhysicsEventTest : public DUOLGameEngine::MonoBehaviourBase
	{
	public:
		PhysicsEventTest(DUOLGameEngine::GameObject* owner);

		virtual ~PhysicsEventTest() override;

	private:
		bool _isDynamic;

	public:
		virtual void OnStart() override;

		virtual void OnUpdate(float deltaTime) override;

		virtual void OnCollisionEnter(const std::shared_ptr<DUOLPhysics::Collision>& collision) override;

		virtual void OnCollisionStay(const std::shared_ptr<DUOLPhysics::Collision>& collision) override;

		virtual void OnCollisionExit(const std::shared_ptr<DUOLPhysics::Collision>& collision) override;

		virtual void OnTriggerEnter(const std::shared_ptr<DUOLPhysics::Trigger>& trigger) override;

		virtual void OnTriggerStay(const std::shared_ptr<DUOLPhysics::Trigger>& trigger) override;

		virtual void OnTriggerExit(const std::shared_ptr<DUOLPhysics::Trigger>& trigger) override;
	};
}