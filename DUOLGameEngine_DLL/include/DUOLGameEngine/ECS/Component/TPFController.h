﻿/**

    @file      TPFCameraController.h
    @brief     기본 제공 3인칭 자유 시점 컨트롤러
    @details   ~
    @author    COMETLEE
    @date      11.11.2022
    @copyright © COMETLEE, 2022. All right reserved.

**/
#pragma once
#include "DUOLGameEngine/ECS/Component/BehaviourBase.h"
#include "DUOLMath/DUOLMath.h"

namespace DUOLGameEngine
{
	/**
	 * \brief 기본 제공 3인칭 자유 시점 컨트롤러
	 */
	class DUOL_GAMEENGINE_API TPFController final : public DUOLGameEngine::BehaviourBase
	{
	public:
        TPFController();

        TPFController(DUOLGameEngine::GameObject* owner,
            const DUOLCommon::tstring& name = TEXT("TPFController"));

        virtual ~TPFController() override;

	private:
        float _moveSpeed;

	public:
        virtual void OnLateUpdate(float deltaTime) override;

        RTTR_ENABLE(BehaviourBase)

		RTTR_REGISTRATION_FRIEND
	};
}
