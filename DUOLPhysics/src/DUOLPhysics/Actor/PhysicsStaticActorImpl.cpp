#include "PhysicsStaticActorImpl.h"

/* etc */
#include "../Util/PhysicsTypeConverter.h"

#include <string>

#define ERROR_THROW(errStr)				\
{										\
	std::string errTemp = errStr;		\
	errTemp += " / File : ";			\
	errTemp += __FILE__;				\
	errTemp += ", Line : ";				\
	errTemp += std::to_string(__LINE__);\
	throw errTemp;						\
}

namespace DUOLPhysics
{
	PhysicsStaticActor::Impl::Impl() :
		_actor(nullptr)
	{

	}

	PhysicsStaticActor::Impl::~Impl()
	{

	}

	PxRigidStatic* PhysicsStaticActor::Impl::Create(PxPhysics* physics, const PhysicsActorDesc& staticDesc)
	{
		if (physics == nullptr)
			ERROR_THROW("Failed to create Static Actor. (No PxPhysics.)");

		_actor = physics->createRigidStatic(ConvertTransform(staticDesc._transform));

		if (_actor == nullptr)
			ERROR_THROW("Failed to create Static Actor.");

		return _actor;
	}

	PxRigidActor* PhysicsStaticActor::Impl::GetActor()
	{
		return _actor;
	}

	PxRigidStatic* PhysicsStaticActor::Impl::GetStaticActor()
	{
		return _actor;
	}
}