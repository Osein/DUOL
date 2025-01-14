#include "DUOLGameEngine/ECS/Component/Transform.h"

#include "DUOLGameEngine/ECS/GameObject.h"
#include "DUOLGameEngine/Manager/SceneManagement/Scene.h"

#include <rttr/registration>
#include "DUOLCommon/MetaDataType.h"
#include "DUOLCommon/Log/LogHelper.h"

using namespace rttr;

RTTR_PLUGIN_REGISTRATION
{
	rttr::registration::class_<DUOLGameEngine::Transform>("Transform")
	.constructor()
	(
		rttr::policy::ctor::as_raw_ptr
	)
	.constructor<DUOLGameEngine::GameObject*, const DUOLCommon::tstring&>()
	(
		rttr::policy::ctor::as_raw_ptr
	)
	.property("Position", &DUOLGameEngine::Transform::GetLocalPositionWithoutCheck, & DUOLGameEngine::Transform::SetLocalPosition)
	(
		metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Float3)
	)
	.property("Rotation", &DUOLGameEngine::Transform::GetLocalEulerAngleWithoutCheck, &DUOLGameEngine::Transform::SetLocalEulerAngle)
	(
		metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Float3)
	)
	.property("Scale", &DUOLGameEngine::Transform::GetLocalScaleWithoutCheck, &DUOLGameEngine::Transform::SetLocalScale)
	(
		metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Float3)
	)
	.property("_localPosition", &DUOLGameEngine::Transform::GetLocalPositionWithoutCheck, &DUOLGameEngine::Transform::SetLocalPosition)
	(
		metadata(DUOLCommon::MetaDataType::Serializable, true)
	)
	.property("_localRotation", &DUOLGameEngine::Transform::GetLocalRotationWithoutCheck, &DUOLGameEngine::Transform::SetLocalRotation)
	(
		metadata(DUOLCommon::MetaDataType::Serializable, true)
	)
	.property("_localScale", &DUOLGameEngine::Transform::GetLocalScaleWithoutCheck, &DUOLGameEngine::Transform::SetLocalScale)
	(
		metadata(DUOLCommon::MetaDataType::Serializable, true)
	)
	.property("_children", &DUOLGameEngine::Transform::_children)
	(
		metadata(DUOLCommon::MetaDataType::Serializable, true)
		, metadata(DUOLCommon::MetaDataType::SerializeByUUID, true)
		, metadata(DUOLCommon::MetaDataType::MappingType, DUOLCommon::MappingType::FileUUID)
	)
	.property("_childrenGameObjects", &DUOLGameEngine::Transform::_childrenGameObjects)
	(
		metadata(DUOLCommon::MetaDataType::Serializable, true)
		, metadata(DUOLCommon::MetaDataType::SerializeByUUID, true)
		, metadata(DUOLCommon::MetaDataType::MappingType, DUOLCommon::MappingType::FileUUID)
	)
	.property("_parent", &DUOLGameEngine::Transform::_parent)
	(
		metadata(DUOLCommon::MetaDataType::Serializable, true)
		, metadata(DUOLCommon::MetaDataType::SerializeByUUID, true)
		, metadata(DUOLCommon::MetaDataType::MappingType, DUOLCommon::MappingType::FileUUID)
	);
}

namespace DUOLGameEngine
{
	Transform::Transform() :
		ComponentBase(nullptr, TEXT("Transform"))
		, _parent(nullptr)
		, _children(std::vector<DUOLGameEngine::Transform*>())
		, _localPosition(Vector3::Zero)
		, _localScale(Vector3::One)
		, _localRotation(Quaternion::Identity)
		, _localEulerAngle(Vector3::Zero)
		, _worldPosition(Vector3::Zero)
		, _worldScale(Vector3::One)
		, _worldRotation(Quaternion::Identity)
		, _worldEulerAngle(Vector3::Zero)
		, _localMatrix(Matrix::Identity)
		, _worldMatrix(Matrix::Identity)
		, _look(DUOLMath::Vector3::Forward)
		, _right(DUOLMath::Vector3::Right)
		, _up(DUOLMath::Vector3::Up)
		, _isDirtTransform(false)
		, _scaledEvent()
		, _setParentEvent()
	{
	}

	Transform::Transform(DUOLGameEngine::GameObject* owner, const DUOLCommon::tstring& name) :
		ComponentBase(owner, name)
		, _parent(nullptr)
		, _children(std::vector<DUOLGameEngine::Transform*>())
		, _localPosition(Vector3::Zero)
		, _localScale(Vector3::One)
		, _localRotation(Quaternion::Identity)
		, _localEulerAngle(Vector3::Zero)
		, _worldPosition(Vector3::Zero)
		, _worldScale(Vector3::One)
		, _worldRotation(Quaternion::Identity)
		, _worldEulerAngle(Vector3::Zero)
		, _localMatrix(Matrix::Identity)
		, _worldMatrix(Matrix::Identity)
		, _look(DUOLMath::Vector3::Forward)
		, _right(DUOLMath::Vector3::Right)
		, _up(DUOLMath::Vector3::Up)
		, _isDirtTransform(false)
		, _scaledEvent()
		, _setParentEvent()
	{
		
	}

	Transform::~Transform()
	{
		// 자식 오브젝트들에게 나 삭제된다구 .. 알려줘야지
		for (auto child : _children)
			child->_parent = nullptr;
	}

	void Transform::UpdateLocalTM()
	{
		_localMatrix = Matrix::CreateScale(_localScale) * Matrix::CreateFromQuaternion(_localRotation) *
			Matrix::CreateTranslation(_localPosition);
	}

	void Transform::UpdateWorldTM()
	{
		if (_parent != nullptr)
		{
			const Matrix& parentWorldTM = _parent->GetWorldMatrix();

			_worldMatrix = _localMatrix * parentWorldTM;
		}
		else
		{
			_worldMatrix = _localMatrix;
		}
	}

	void Transform::UpdateTMAndAllProperties()
	{
		UpdateLocalTM();

		UpdateWorldTM();

		UpdateLookRightUp();

		DecomposeLocalTM();

		DecomposeWorldTM();
	}

	void Transform::CheckIsDirtAndUpdate()
	{
		if (_isDirtTransform)
		{
			UpdateTMAndAllProperties();

			_isDirtTransform = false;
		}
	}

	void Transform::SetDirtTransform()
	{
		_isDirtTransform = true;

		for (auto& child : _children)
		{
			if (child != nullptr)
				child->SetDirtTransform();
		}
	}

	Vector3 Transform::GetLocalEulerAngleWithoutCheck()
	{
		return Vector3(DUOLMath::MathHelper::RadianToDegree(_localEulerAngle.x), 
			DUOLMath::MathHelper::RadianToDegree(_localEulerAngle.y), DUOLMath::MathHelper::RadianToDegree(_localEulerAngle.z));
	}

	const Vector3& Transform::GetLocalEulerAngle()
	{
		CheckIsDirtAndUpdate();

		return Vector3(DUOLMath::MathHelper::RadianToDegree(_localEulerAngle.x), DUOLMath::MathHelper::RadianToDegree(_localEulerAngle.y), DUOLMath::MathHelper::RadianToDegree(_localEulerAngle.z));
	}

	const Matrix& Transform::GetLocalMatrix()
	{
		CheckIsDirtAndUpdate();

		return _localMatrix;
	} 

	const Matrix& Transform::GetWorldMatrix()
	{
		CheckIsDirtAndUpdate();

		return _worldMatrix;
	}

	void Transform::LookAt(Transform* target, const Vector3& worldUp)
	{
		LookAt(target->GetWorldPosition(), worldUp);
	}

	void Transform::LookAt(const Vector3& worldPosition, const Vector3& worldUp)
	{
		// 에러 방지
		if (worldPosition == _worldPosition)
			return;

		// DUOLMath::Matrix::CreateLookAt => XMMatrixLookAtLH(...)
		// : Builds a view matrix for a left-handed coordinate system using a camera position, an up direction, and a focal point.
		// 따라서, 이 행렬의 역행렬을 하면 해당 카메라의 월드 행렬이 나온다. 여기서 회전값을 빼서 적용시키면 해당 worldPosition을 바라보는
		// 회전 상태로 되돌릴 수 있습니다.
		DUOLMath::Matrix lookAtMatrix = DUOLMath::Matrix::CreateLookAt(_worldPosition, worldPosition, worldUp);

		// 회전만 시켜주기 위해서 회전 성분 빼낸다.
		Quaternion lookAtQuat = Quaternion::CreateFromRotationMatrix(lookAtMatrix);

		lookAtQuat.Inverse(lookAtQuat);

		_worldRotation = Quaternion::Identity;

		// 회전 성분 업데이트
		UpdateRotation(lookAtQuat, Space::World);
	}

	void Transform::Rotate(const Vector3& eulers, Space relativeTo)
	{
		CheckIsDirtAndUpdate();

		const Vector3 radianEulers = Vector3(MathHelper::DegreeToRadian(eulers.x), MathHelper::DegreeToRadian(eulers.y),
			MathHelper::DegreeToRadian(eulers.z));

		// 각각 좌표 시스템에 대해서 회전한다는 것을 의미하고 싶다 ..
		Quaternion deltaRot = Quaternion::Identity;

		if (relativeTo == Space::World)
			deltaRot = Quaternion::CreateFromYawPitchRoll(radianEulers.y, radianEulers.x, radianEulers.z);
		else if (relativeTo == Space::Self)
			deltaRot = Quaternion::CreateFromAxisAngle(_look, radianEulers.z) *
				Quaternion::CreateFromAxisAngle(_right, radianEulers.x) * Quaternion::CreateFromAxisAngle(_up, radianEulers.y);

		UpdateRotation(deltaRot, relativeTo);
	}

	void Transform::Rotate(const Vector3& axis, float angle, Space relativeTo)
	{
		CheckIsDirtAndUpdate();

		const float radian = MathHelper::DegreeToRadian(angle);

		Quaternion deltaRot = Quaternion::Identity;

		if (relativeTo == Space::World)
		{
			deltaRot = Quaternion::CreateFromAxisAngle(axis, radian);
		}
		else if (relativeTo == Space::Self)
		{
			// Local Coordinate에 맞게 조절. 현재 회전에 대해서 axis를 돌려준다.
			const Vector3 localAxis = 
				Vector3::TransformNormal(axis, Matrix::CreateFromLookRightUp(_look, _right, _up));

			deltaRot = Quaternion::CreateFromAxisAngle(localAxis, radian);
		}

		UpdateRotation(deltaRot, relativeTo);
	}

	void Transform::Rotate(const Quaternion& quat, Space relativeTo)
	{
		CheckIsDirtAndUpdate();

		if (relativeTo == Space::Self)
		{
			
		}

		UpdateRotation(quat, relativeTo);
	}

	void Transform::RotateAround(const Vector3& point, const Vector3& axis, float angle)
	{
		CheckIsDirtAndUpdate();

		// Point를 부모로 취하고 포지션을 변환시킨다.
		const Vector3 diff = (_worldPosition - point);

		// degree to radian.
		const float radian = MathHelper::DegreeToRadian(angle);

		const Matrix rotateAroundMat = Matrix::CreateTranslation(diff) * Matrix::CreateFromAxisAngle(axis, radian)
			* Matrix::CreateTranslation(Vector3::Transform(-diff, Matrix::CreateFromAxisAngle(axis, radian)));

		const Matrix newWorldTM = _worldMatrix * rotateAroundMat;

		_worldMatrix = newWorldTM;

		if (_parent != nullptr)
			_localMatrix = _worldMatrix * (_parent->GetWorldMatrix().Invert());
		else
			_localMatrix = _worldMatrix;

		SetDirtTransform();
	}

	void Transform::Translate(const Vector3& translation, Space relativeTo)
	{
		CheckIsDirtAndUpdate();

		if (relativeTo == Space::Self)
		{
			const Matrix& localRot = Matrix::CreateFromQuaternion(_localRotation);

			const Vector3 movement = Vector3::Transform(translation, localRot);

			_worldPosition = _worldPosition + movement;
		}
		else
		{
			// DUOLGameEngine에서는 Scene의 World Coordinate를 바꾸는 기능이 없으니 다음과 같이 바로 계산합니다.
			_worldPosition = _worldPosition + translation;
		}

		if (_parent != nullptr)
		{
			const Quaternion& parentRot = _parent->GetWorldRotation();

			const Vector3& parentPos = _parent->GetWorldPosition();

			const Matrix parentWorldTM = Matrix::CreateFromQuaternion(parentRot) * Matrix::CreateTranslation(parentPos);

			Matrix parentWorldTMInv;

			parentWorldTM.Invert(parentWorldTMInv);

			// 로컬 위치는 월드 위치에서 부모 변환의 역변환을 취하면 된다. (스케일 제외)
			_localPosition = Vector3::Transform(_worldPosition, parentWorldTMInv);
		}
		else
		{
			_localPosition = _worldPosition;
		}

		SetDirtTransform();
	}

	void Transform::Translate(const Vector3& translation, Transform* relativeTo)
	{
		CheckIsDirtAndUpdate();

		if (relativeTo == nullptr)
		{
			Translate(translation, Space::World);

			return;
		}

		const Matrix& localRot = Matrix::CreateFromQuaternion(relativeTo->GetLocalRotation());

		const Vector3 movement = Vector3::Transform(translation, localRot);

		Translate(movement, Space::World);
	}

	void Transform::SetLocalScale(const Vector3& localScale)
	{
		_localScale = localScale;

#pragma region SCALED_EVENT
		_scaledEvent.Invoke(localScale);
#pragma endregion

		SetDirtTransform();

		// 스케일은 좀 특수하기 때문에 .. 프로퍼티들을 전부 바꿔주자.
		UpdateTMAndAllProperties();

		SetDirtTransform();
	}

	void Transform::SetPosition(const Vector3& position, Space relativeTo)
	{
		if (relativeTo == Space::World)
		{
			_worldPosition = position;

			Vector3 parentPosition = Vector3::Zero;

			DUOLMath::Quaternion rot = DUOLMath::Quaternion::Identity;

			if (_parent != nullptr)
			{
				parentPosition = _parent->GetTransform()->GetWorldPosition();

				rot = _parent->GetTransform()->GetWorldRotation();

				rot.Inverse(rot);
			}

			// 부모 기준, 부모의 로테이션이 0인 경우의 위치를 Local Position 이라고 하자.
			_localPosition = DUOLMath::Vector3::Transform(position - parentPosition, rot);
		}
		else
		{
			_localPosition = position;

			Vector3 parentPosition = Vector3::Zero;

			if (_parent != nullptr)
				parentPosition = _parent->GetTransform()->GetWorldPosition();

			_worldPosition = parentPosition + position;
		}

		SetDirtTransform();
	}

	void Transform::SetRotation(const Quaternion& rotation, Space relativeTo)
	{
		if (relativeTo == Space::World)
		{
			_worldRotation = rotation;

			Quaternion parentRotationInv = Quaternion::Identity;

			if (_parent != nullptr)
				_parent->GetWorldRotation().Inverse(parentRotationInv);

			const Quaternion newLocalRotation = _worldRotation * parentRotationInv;

			_localRotation = newLocalRotation;
		}
		else
		{
			_localRotation = rotation;

			Quaternion parentRotation = Quaternion::Identity;

			if (_parent != nullptr)
				parentRotation = _parent->GetWorldRotation();

			const Quaternion newWorldRotation = _localRotation * parentRotation;

			_worldRotation = newWorldRotation;
		}

		_localEulerAngle = Quaternion::ConvertQuaternionToEuler(_localRotation);

		_worldEulerAngle = Quaternion::ConvertQuaternionToEuler(_worldRotation);

		SetDirtTransform();
	}

	void Transform::SetWorldTM(const Matrix& worldMatrix)
	{
		// World Property
		_worldMatrix = worldMatrix;

		DecomposeWorldTM();

		if (_parent != nullptr)
			_localMatrix = _worldMatrix * _parent->GetWorldMatrix().Invert();
		else
			_localMatrix = _worldMatrix;

		DecomposeLocalTM();

		SetDirtTransform();
	}

	void Transform::SetLocalTM(const Matrix& localMatrix)
	{
		// Local Property
		_localMatrix = localMatrix;

		DecomposeLocalTM();

		// World Property
		if (_parent != nullptr)
			_worldMatrix = _localMatrix * _parent->GetWorldMatrix();
		else
			_worldMatrix = _localMatrix;

		DecomposeWorldTM();

		SetDirtTransform();
	}

	void Transform::SetLocalTMWithoutDirt(const Matrix& localMatrix)
	{
		_localMatrix = localMatrix;

		DecomposeLocalTM();

		// World Property
		if (_parent != nullptr)
			_worldMatrix = _localMatrix * _parent->GetWorldMatrix();
		else
			_worldMatrix = _localMatrix;

		DecomposeWorldTM();
	}

	void Transform::SetLocalPosition(const DUOLMath::Vector3& position)
	{
		SetPosition(position, Space::Self);
	}

	void Transform::SetLocalEulerAngle(Vector3 eulers)
	{
		const Vector3 radianEulers = Vector3(MathHelper::DegreeToRadian(eulers.x), MathHelper::DegreeToRadian(eulers.y),
			MathHelper::DegreeToRadian(eulers.z));

		Quaternion rot = Quaternion::Identity;

		rot = Quaternion::CreateFromYawPitchRoll(radianEulers.y, radianEulers.x, radianEulers.z);

		SetRotation(rot, Space::Self);
	}

	void Transform::SetLocalRotation(const Quaternion& quaternion)
	{
		SetRotation(quaternion, Space::Self);
	}

	void Transform::UpdateRotation(const Quaternion& rotation, Space relativeTo)
	{
		if (relativeTo == Space::Self)
		{
			_localRotation = _localRotation * rotation;

			Quaternion parentRotation = Quaternion::Identity;

			if (_parent != nullptr)
				parentRotation = _parent->GetWorldRotation();

			const Quaternion newWorldRotation =  _localRotation * parentRotation;

			_worldRotation = newWorldRotation;
		}
		else
		{
			_worldRotation = _worldRotation * rotation;

			Quaternion parentRotationInv = Quaternion::Identity;

			if (_parent != nullptr)
				_parent->GetWorldRotation().Inverse(parentRotationInv);

			const Quaternion newLocalRotation = _worldRotation * parentRotationInv;

			_localRotation = newLocalRotation;
		}

		_localEulerAngle = Quaternion::ConvertQuaternionToEuler(_localRotation);

		_worldEulerAngle = Quaternion::ConvertQuaternionToEuler(_worldRotation);

		SetDirtTransform();
	}

	void Transform::UpdateLookRightUp()
	{
		_look = Vector3::TransformNormal(Vector3::UnitZ, _worldMatrix);

		_right = Vector3::TransformNormal(Vector3::UnitX, _worldMatrix);

		_up = Vector3::TransformNormal(Vector3::UnitY, _worldMatrix);
	}

	void Transform::DecomposeLocalTM()
	{
		_localMatrix.Decompose(_localScale, _localRotation, _localPosition);

		_localEulerAngle = Quaternion::ConvertQuaternionToEuler(_localRotation);
	}

	void Transform::DecomposeWorldTM()
	{
		_worldMatrix.Decompose(_worldScale, _worldRotation, _worldPosition);

		_worldEulerAngle = Quaternion::ConvertQuaternionToEuler(_worldRotation);
	}

	Transform* Transform::GetRootTransform()
	{
		Transform* rootTransform = this;

		// 더 이상의 부모가 없을 때까지 위로 올라갑니다.
		while (rootTransform->_parent != nullptr)
		{
			rootTransform = rootTransform->_parent;
		}

		return rootTransform;
	}

	void Transform::SetParent(Transform* parent, bool worldPositionStays, bool setRootObject)
	{
		Matrix parentWorldTM = Matrix::Identity;

		// 기존의 부모가 존재했다면 부모를 정리합니다.
		if (_parent != nullptr)
		{
			_parent->ResetChild(this);
		}
		// 기존의 부모가 존재하지 않았습니다. => Root Object였으므로 해당 오브젝트가 속한 씬의 Root Object list에서 지워줍니다.
		else
		{
			DUOLGameEngine::GameObject* gameObject = GetGameObject();

			DUOLGameEngine::Scene* scene = gameObject->GetScene();

			if (scene != nullptr)
				scene->RemoveInRootObjectsList(gameObject);
		}

		// 매개변수 parent를 널포인터로 받은 경우 => 이제 Root Object가 되겠습니다.
		if ((parent == nullptr) && (setRootObject))
		{
			_parent = nullptr;

			DUOLGameEngine::GameObject* gameObject = GetGameObject();

			DUOLGameEngine::Scene* scene = gameObject->GetScene();

			if (scene != nullptr)
				scene->AddInRootObjectsList(gameObject);
		}

		if (parent != nullptr)
		{
			_parent = parent;

			_parent->_children.push_back(this);

			_parent->_childrenGameObjects.push_back(this->GetGameObject());

			parentWorldTM = _parent->GetWorldMatrix();
		}

		// 이전과 같은 월드 포지션, 스케일, 로테이션을 가진다.
		if (worldPositionStays)
		{
			// 월드를 유지하기 위해서 새로 바뀐 부모에 비례하여 로컬이 바뀐다.
			_localMatrix = _worldMatrix * parentWorldTM.Invert();

			DecomposeLocalTM();
		}
		// 이전과 같은 로컬 포지션, 스케일, 로테이션을 가진다.
		else
		{
			// 로컬을 유지하기 위해서 새로 바뀐 부모에 비례하여 월드가 바뀐다.
			_worldMatrix = _localMatrix * parentWorldTM;

			DecomposeWorldTM();

			SetDirtTransform();
		}

		// SetParent Event ..
		_setParentEvent.Invoke(parent);
	}

	Transform* Transform::FindChild(const DUOLCommon::tstring& name) const
	{
		for (auto& child : _children)
		{
			if ((child != nullptr) && (child->GetGameObject()->GetName() == name))
				return child;
		}

		return nullptr;
	}

	Transform* Transform::Find(const DUOLCommon::tstring& name) const
	{
		if (name.find('/') == false)
			return FindChild(name);

		Transform* ret = nullptr;

		size_t prevIndex = 0;

		size_t index = name.find('/');

		while (index != DUOLCommon::tstring::npos)
		{
			DUOLCommon::tstring childName = DUOLCommon::tstring(name.begin() + prevIndex, name.begin() + index);

			ret = FindChild(childName);

			prevIndex = index;

			index = name.find('/', prevIndex);
		}

		return ret;
	}

	const std::vector<Transform*>& Transform::GetChildren() const
	{
		return _children;
	}

	const std::vector<DUOLGameEngine::GameObject*>& Transform::GetChildGameObjects() const
	{
		return _childrenGameObjects;
	}

	void Transform::GetChildGameObjectsRecursively(std::vector<DUOLGameEngine::GameObject*>& addOutput)
	{
		for (const auto& child : _children)
		{
			if (child != nullptr)
			{
				addOutput.push_back(child->GetGameObject());

				child->GetChildGameObjectsRecursively(addOutput);
			}
		}
	}

	void Transform::OnAwake()
	{
		ComponentBase::OnAwake();
	}

	std::vector<DUOLGameEngine::GameObject*> Transform::GetAllChildGameObjects() const
	{
		std::vector<GameObject*> ret{};

		for (const auto& child : _children)
		{
			if (child != nullptr)
			{
				ret.push_back(child->GetGameObject());

				child->GetChildGameObjectsRecursively(ret);
			}
		}

		return ret;
	}

	void Transform::DetachChildren()
	{
		for (auto& child : _children)
		{
			if (child != nullptr)
			{
				child->SetParent(nullptr, true);
			}
		}

		_children.clear();
	}

	bool Transform::IsChildOf(Transform* parent) const
	{
		const std::vector<Transform*>& parentChildren = parent->_children;

		for (const auto& child : parentChildren)
		{
			if ((child != nullptr) && (child == this))
			{
				// 있음.
				return true;
			}
		}

		// 없음.
		return false;
	}

	bool Transform::IsRootObject() const
	{
		return (_parent == nullptr);
	}

	void Transform::ResetChild(Transform* child)
	{
		// 지우기만 하면 됩니다. 자식이 없어진다고 해서 뭐 부모가 따로 조작될 것은 없음 ..!
		std::erase_if(_children, [&child](DUOLGameEngine::Transform* item)
			{
				// UUID로 비교해야하나 ..?
				return (child == item);
			});

		DUOLGameEngine::GameObject* childGameObject = child->GetGameObject();;

		// 칠드런 게임 오브젝트 리스트에서도 삭제 ..!
		std::erase_if(_childrenGameObjects, [childGameObject](DUOLGameEngine::GameObject* item)
			{
				// UUID로 비교해야하나 ..?
				return (childGameObject == item);
			});
	}
}