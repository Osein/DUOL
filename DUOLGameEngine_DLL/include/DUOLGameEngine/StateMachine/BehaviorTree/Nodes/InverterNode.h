/**

	@file    InverterNode.h
	@brief	 Child Node의 반환 값을 뒤집어서 Return하는 노드
	@details -
	@author  JKim
	@date    14.10.2022

**/
#pragma once
#include "DecoratorNode.h"

namespace DUOLGameEngine
{
	/**

		@class   InverterNode
		@brief	 Child Node의 반환 값을 뒤집어서 Return하는 노드
		@details -

	**/
	class InverterNode : public DecoratorNode
	{
	public:
		/**
			@brief   InverterNode 클래스 생성자
			@details -
			@param   name - Node의 Name
		**/
		InverterNode(const DUOLCommon::tstring& name);

		/**
			@brief   InverterNode 클래스 default 소멸자
			@details -
		**/
		virtual ~InverterNode() override = default;

	protected:
		/**
			@brief   매 프레임 호출되는 함수
			@details 상속받은 Child Class에서 Update하고 싶은 동작들을 구현한다.
		**/
		NodeState Tick() override final;

		/**
			@brief	 Node의 상태를 Idle로 변경하는 함수
			@details Child Node의 모든 동작을 정리하고 Idle 상태로 변경
		**/
		void Stop() override;
	};
}