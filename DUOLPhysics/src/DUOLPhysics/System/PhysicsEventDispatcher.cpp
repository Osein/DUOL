#include "PhysicsEventDispatcher.h"

/* etc */
#include "../Util/PhysicsTypeConverter.h"

namespace DUOLPhysics
{
	void PhysicsEventDispatcher::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 countOfPairs)
	{
		for (PxU32 i = 0; i < countOfPairs; i++)
		{
			auto* actor1 = reinterpret_cast<PhysicsUserData*>(pairHeader.actors[0]->userData);
			auto* actor2 = reinterpret_cast<PhysicsUserData*>(pairHeader.actors[1]->userData);

			// 액터가 딜리트될 때 Collision Stay => Exit을 호출하지 않고, 그 정보를 그냥 없앤다. 
			if (actor1 == nullptr || actor2 == nullptr)
				break;

			auto contactCount = pairs[i].contactCount;

			auto collision = std::make_shared<Collision>();
			collision->_data.resize(contactCount);

			std::vector<PxContactPairPoint> points(contactCount);
			pairs[i].extractContacts(points.data(), contactCount);

			for (int j = 0; j < contactCount; j++)
			{
				collision->_data[j]._position = ConvertVector3(points[j].position);
				collision->_data[j]._normal = ConvertVector3(points[j].normal);
				collision->_data[j]._impulse = ConvertVector3(points[j].impulse);
				collision->_data[j]._separation = points[j].separation;
			}

			if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				collision->_other = actor2->GetUserData();
				actor1->OnCollisionEnter(collision);

				collision->_other = actor1->GetUserData();
				actor2->OnCollisionEnter(collision);
			}

			if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
			{
				collision->_other = actor2->GetUserData();
				actor1->OnCollisionStay(collision);

				collision->_other = actor1->GetUserData();
				actor2->OnCollisionStay(collision);
			}

			if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				collision->_other = actor2->GetUserData();
				actor1->OnCollisionExit(collision);

				collision->_other = actor1->GetUserData();
				actor2->OnCollisionExit(collision);
			}
		}
	}

	void PhysicsEventDispatcher::onTrigger(PxTriggerPair* pairs, PxU32 count)
	{
		PxTriggerPairFlags triggerFlag = PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER;

		for (PxU32 i = 0; i < count; i++)
		{
			auto* trigger = reinterpret_cast<PhysicsUserData*>(pairs[i].triggerActor->userData);
			auto* other = reinterpret_cast<PhysicsUserData*>(pairs[i].otherActor->userData);

			if (trigger == nullptr || other == nullptr)
				continue;

			auto triggerObj = std::make_shared<Trigger>();
			triggerObj->_other = other->GetUserData();

			auto otherObj = std::make_shared<Trigger>();
			otherObj->_other = trigger->GetUserData();

			if (pairs[i].flags & triggerFlag)
			{
				if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					trigger->OnTriggerExit(triggerObj);
					other->OnTriggerExit(otherObj);

					auto result = _triggerStayReciverList.find({ trigger, other });

					if (result != _triggerStayReciverList.end())
						_triggerStayReciverList.erase(result);
				}

				continue;
			}

			if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				trigger->OnTriggerEnter(triggerObj);
				other->OnTriggerEnter(otherObj);

				_triggerStayReciverList.insert({ { trigger, other }, true });
			}

			if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				trigger->OnTriggerExit(triggerObj);
				other->OnTriggerExit(otherObj);

				auto result = _triggerStayReciverList.find({ trigger, other });

				if (result != _triggerStayReciverList.end())
					_triggerStayReciverList.erase(result);
			}
		}
	}

	//void PhysicsEventDispatcher::onSleep(PxActor** acotrs, PxU32 count)
	//{
	//	std::erase_if(_triggerStayReciverList, [userData](const auto& item)
	//		{
	//			auto const& [key, value] = item;

	//	return (key.first == userData || key.second == userData);
	//		});

	//}

	void PhysicsEventDispatcher::SendTriggerStayEvent()
	{
		for (auto& iter : _triggerStayReciverList)
		{
			if (iter.second == true)
			{
				iter.second = false;

				continue;
			}

			auto* trigger = iter.first.first;
			auto* other = iter.first.second;

			auto triggerObj = std::make_shared<Trigger>();
			triggerObj->_other = other->GetUserData();

			auto otherObj = std::make_shared<Trigger>();
			otherObj->_other = trigger->GetUserData();

			trigger->OnTriggerStay(triggerObj);
			other->OnTriggerStay(otherObj);
		}
	}

	void PhysicsEventDispatcher::DeleteTriggerStayUserData(PhysicsUserData* userData)
	{
		// 액터가 딜리트될 때 스테이 중이였다면 Exit을 호출하지 않고, 그냥 없앤다. 
		std::erase_if(_triggerStayReciverList, [userData](const auto& item)
			{
				auto const& [key, value] = item;

				return (key.first == userData || key.second == userData);
			});
	}
}
