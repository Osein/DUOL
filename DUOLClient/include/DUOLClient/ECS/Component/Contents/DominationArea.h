#pragma once
#include "DUOLClient/Export_Client.h"
#include "DUOLGameEngine/ECS/Component/MonoBehaviourBase.h"

namespace DUOLGameEngine
{
	class BoxCollider;
}

/**
 * \brief 인게임 내의 A 구역 컨텐츠 점령존을 위한 클래스입니다.
 *
 *	기믹 -
 *		
 *
 */
namespace DUOLClient
{
	
class DUOL_CLIENT_API DominationArea :public DUOLGameEngine::MonoBehaviourBase
{
public:
	DominationArea(DUOLGameEngine::GameObject* owner = nullptr, const DUOLCommon::tstring& name = DUOLCommon::StringHelper::ToTString("DominationArea"));

	virtual ~DominationArea() override;

///**
// * \brief 에어리어 지역 크기
// */
//	DUOLMath::Vector3 _areaExtents;

	/**
	 * \brief 몬스터 수에따른 파괴율 증가수치
	 */
	float _destroyIncreasePerMonster;

	/**
	* \brief 구역의 맥스 정화도
	*/
	float _maxClearPercent;

	/**
	 * \brief 구역의 현재 정화도
	 */
	float _currentClearPercent;

	/**
	 * \brief 플레이어 점령 시간
	 */
	float _dominationTime;

	DUOLGameEngine::BoxCollider* _areaCollider;

	int _currentEnemyCountInZone;

	bool _playerInZone;

	/**
	 * \brief 점령여부
	 */
	bool _isCleared;


	bool IsIsCleared() const;

	/**
 * \brief Behaviour가 사용 가능해졌을 때 호출됩니다.
 */
	virtual void OnEnable() override;

	/**
	 * \brief Behaviour가 사용 불가능해졌을 때 호출됩니다.
	 */
	virtual void OnDisable() override;

	/**
 * \brief 씬이 시작할 때 OnStart 보다 이전에 호출됩니다.
 * 씬 시작 시 우선 순위의 게임 로직을 적용할 수 있습니다.
 */
	virtual void OnAwake() override;

	/**
	 * \brief 씬이 시작할 때 OnAwake 이후에 호출됩니다.
	 * 씬 시작 시 우선 순위의 게임 로직을 적용할 수 있습니다.
	 */
	virtual void OnStart() override;

	/**
	 * \brief 컴포넌트가 파괴될 때 호출됩니다.
	 */
	virtual void OnDestroy() override;

	/**
	 * \brief 매 프레임에 호출됩니다.
	 * \param deltaTime 프레임 간 시간 간격입니다.
	 */
	virtual void OnUpdate(float deltaTime) override;

	virtual void OnFixedUpdate(float deltaTime) override;


	void OnTriggerStay(const std::shared_ptr<DUOLPhysics::Trigger>& trigger) override;


	RTTR_ENABLE(DUOLGameEngine::MonoBehaviourBase)

	RTTR_REGISTRATION_FRIEND
};

}
