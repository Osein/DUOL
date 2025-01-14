#include "PhysicsShapeBaseImpl.h"

/* System */
#include "../System/PhysicsSystemImpl.h"

/* Scene */
#include "../Scene/PhysicsSceneImpl.h"

/* Material */
#include "../Material/PhysicsMaterialImpl.h"

/* etc */
#include "../Util/PhysicsTypeConverter.h"
#include "DUOLPhysics/Util/ObjectLayer.h"
#include "DUOLPhysics/Util/PhysicsDefines.h"

namespace DUOLPhysics
{
	PhysicsShapeBase::Impl::Impl() :
		_shape(nullptr)
	{

	}

	PhysicsShapeBase::Impl::~Impl()
	{
		Release();
	}

	void PhysicsShapeBase::Impl::Create(PhysicsSystem* system, const PxGeometry& geometry, const PhysicsShapeDesc& shapeDesc)
	{
		Create(system->_impl->GetPhysics(), geometry, shapeDesc);
	}

	void PhysicsShapeBase::Impl::Create(PhysicsScene* scene, const PxGeometry& geometry, const PhysicsShapeDesc& shapeDesc)
	{
		Create(scene->_impl->GetPhysics(), geometry, shapeDesc);
	}

	void PhysicsShapeBase::Impl::Create(PxPhysics* physics, const PxGeometry& geometry, const PhysicsShapeDesc& shapeDesc)
	{
		if (physics == nullptr)
			ERROR_THROW("Failed to create PxShape. (No PxPhysics.)");

		PxMaterial* material = shapeDesc._material.lock()->_impl->GetMaterial();

		if (material == nullptr)
			ERROR_THROW("Failed to create PxShape. (No PxMaterial.)");

		auto bitMask = shapeDesc._flag.GetBitMask();

		PxShapeFlags flag(bitMask);

		flag |= PxShapeFlag::Enum::eVISUALIZATION;

		_shape = physics->createShape(geometry, *material, shapeDesc._isExclusive, flag);

		if (_shape == nullptr)
			ERROR_THROW("Failed to create PxShape.");

		_shape->userData = nullptr;

		SetSimulationLayer(TEXT("Default"));
	}

	PxShape* PhysicsShapeBase::Impl::GetShape() const
	{
		return _shape;
	}

	void PhysicsShapeBase::Impl::SetMaterial(const std::weak_ptr<PhysicsMaterial>& material)
	{
		if (_shape == nullptr)
			ERROR_THROW("Failed to set Material. (No PxShape.)");

		PxMaterial* pxMaterial = material.lock()->_impl->GetMaterial();

		if (pxMaterial == nullptr)
			ERROR_THROW("Failed to set Material. (No PxMaterial.)");

		_shape->setMaterials(&pxMaterial, 1);
	}

	void PhysicsShapeBase::Impl::SetSimulationLayer(const DUOLCommon::tstring& layer)
	{
		if (_shape == nullptr)
			ERROR_THROW("Failed to set Material. (No PxShape.)");

		auto layerState = DUOLPhysics::ObjectLayerControl::GetLayerState(layer);

		PxFilterData filterData;

		filterData.word0 = layerState._layer;
		filterData.word1 = layerState._layerMask;

		_shape->setSimulationFilterData(filterData);
		_shape->setQueryFilterData(filterData);
	}

	PxPhysics* PhysicsShapeBase::Impl::GetPhysics(PhysicsSystem* system) const
	{
		return system->_impl->GetPhysics();
	}

	PxPhysics* PhysicsShapeBase::Impl::GetPhysics(PhysicsScene* scene) const
	{
		return scene->_impl->GetPhysics();
	}

	PxCooking* PhysicsShapeBase::Impl::GetCooking(PhysicsSystem* system) const
	{
		return system->_impl->GetCooking();
	}

	PxCooking* PhysicsShapeBase::Impl::GetCooking(PhysicsScene* scene) const
	{
		return scene->_impl->GetCooking();
	}

	bool PhysicsShapeBase::Impl::HasActor() const
	{
		if (_shape != nullptr)
			return _shape->getActor() != nullptr;

		return false;
	}

	void PhysicsShapeBase::Impl::Release()
	{
		if (_shape != nullptr)
		{
			_shape->release();
			_shape = nullptr;
		}
	}
}