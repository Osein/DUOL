/**

	@file    PhysicsDynamicActorImpl.h
	@brief	 Physics Dynamic Actor의 Implementation 클래스
	@details -
	@author  JKim
	@date    3.11.2022

**/
#pragma once
#ifndef _SILENCE_CXX20_CISO646_REMOVED_WARNING
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#endif

#include "PxPhysicsAPI.h"
#include "PhysicsActorBaseImpl.h"
#include "DUOLPhysics/Actor/PhysicsDynamicActor.h"

namespace DUOLPhysics
{
	using namespace physx;

	/**

		@class   Impl
		@brief	 Physics Dynamic Actor의 Implementation 클래스
		@details -

	**/
	class PhysicsDynamicActor::Impl : public PhysicsActorBase::Impl
	{
	public:
		/**
			@brief   Impl 클래스 생성자
			@details -
		**/
		Impl();

		/**
			@brief   Impl 클래스 default 소멸자
			@details -
		**/
		~Impl() = default;

	private:
		PxRigidDynamic* _actor;

	public:
		/**
			@brief	 비어있는 Dynamic Actor 생성
			@details -
			@param   physics     - Physics 객체를 통해 생성
			@param   dynamicDesc - Dynamic Actor 생성에 필요한 값
			@retval  생성된 DynamicActor 객체
		**/
		PxRigidDynamic* Create(PxPhysics* physics, const PhysicsActorDesc& dynamicDesc);

		/**
			@brief	 상속된 Actor 반환 함수
			@details -
			@retval  PxRigidActor*
		**/
		PxRigidActor* GetActor() override;

		/**
			@brief	 Actor 객체 Getter
			@details -
			@retval  _actor
		**/
		PxRigidDynamic* GetDynamicActor();

		/**
			@brief	 Velocity만큼 힘을 가함
			@details -
			@param   velocity - 가할 힘의 값
		**/
		void SetLinearVelocity(DUOLMath::Vector3 velocity);

		/**
			@brief	 Actor 객체의 중심, 밀도 등을 변경
			@details -
			@param   density               - 밀도
			@param   massLocalPose         - 질량의 중심
			@param   isIncludedNonSimShape - Simulation flag가 아닌 도형도 포함할 것인가에 대한 값
		**/
		void SetMassAndInertia(float density, DUOLMath::Vector3 massLocalPose, bool isIncludedNonSimShape);

		/**
			@brief	 이동에 대한 저항력(선형 제동) 변경
			@details 음수는 0으로 처리됨.
			@param   linDamp - 선형 저항 값
		**/
		void SetLinearDamping(float linDamp);

		/**
			@brief	 회전에 대한 저항력(각형 제동) 변경
			@details 음수는 0으로 처리됨.
			@param   angDamp - 각형 저항 값
		**/
		void SetAngularDamping(float angDamp);
	};
}