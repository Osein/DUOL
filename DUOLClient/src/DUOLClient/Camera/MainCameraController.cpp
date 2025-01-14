#include "DUOLClient/Camera/MainCameraController.h"

#include "DUOLGameEngine/ECS/GameObject.h"
#include "DUOLGameEngine/ECS/Component/Transform.h"
#include "DUOLGameEngine/Manager/InputManager.h"
#include "DUOLGameEngine/Manager/PhysicsManager.h"
#include "DUOLGameEngine/Manager/SceneManagement/SceneManager.h"
#include "DUOLPhysics/Util/PhysicsDataStructure.h"

#include <rttr/registration>

#include "DUOLClient/Player/Player.h"
#include "DUOLCommon/MetaDataType.h"
#include "DUOLGameEngine/ECS/Component/Camera.h"
#include "DUOLGameEngine/Manager/TimeManager.h"

using namespace rttr;

RTTR_REGISTRATION
{
	rttr::registration::class_<DUOLClient::MainCameraController>("MainCameraController")
	.constructor()
	(
		rttr::policy::ctor::as_raw_ptr
	)
	.constructor<DUOLGameEngine::GameObject*, const DUOLCommon::tstring&>()
	(
		rttr::policy::ctor::as_raw_ptr
	)
	.property("_followSpeed", &DUOLClient::MainCameraController::_followSpeed)
	(
		metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Float)
		, metadata(DUOLCommon::MetaDataType::Serializable, true)
	)
	.property("_sensitivity", &DUOLClient::MainCameraController::_sensitivity)
	(
		metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Float)
		, metadata(DUOLCommon::MetaDataType::Serializable, true)
	)
	.property("_clampAngle", &DUOLClient::MainCameraController::_clampAngle)
	(
		metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Float)
		, metadata(DUOLCommon::MetaDataType::Serializable, true)
	)
	.property("_minDistance", &DUOLClient::MainCameraController::_minDistance)
	(
		metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Float)
		, metadata(DUOLCommon::MetaDataType::Serializable, true)
	)
	.property("_maxDistance", &DUOLClient::MainCameraController::_maxDistance)
	(
		metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Float)
		, metadata(DUOLCommon::MetaDataType::Serializable, true)
	)
	.property("_smoothness", &DUOLClient::MainCameraController::_smoothness)
	(
		metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Float)
		, metadata(DUOLCommon::MetaDataType::Serializable, true)
	);
}

namespace DUOLClient
{
	MainCameraController::MainCameraController(DUOLGameEngine::GameObject* owner, const DUOLCommon::tstring& name) :
		MonoBehaviourBase(owner, name)
		, _followSpeed(8.f)
		, _sensitivity(20.f)
		, _clampAngle(30.f)
		, _minDistance(1.f)
		, _maxDistance(8.f)
		, _smoothness(10.f)
		, _isLockRotationByMouse(false)
		, _followTransform(nullptr)
		, _viewTransform(nullptr)
		, _cameraShakeTime(0.f)
		, _shakePower(DUOLMath::Vector2(0.f, 0.f))
		, _maxShakeDistance(30.0f)

	{
		_obstacleLayer = DUOLGameEngine::PhysicsManager::GetInstance()->GetLayerNumber(TEXT("Obstacle"));
	}

	MainCameraController::~MainCameraController()
	{

	}

	void MainCameraController::UpdateCameraShake(float deltaTime)
	{
		// Camera Shake time check.
		if (_cameraShakeTime <= 0.f)
			return;

		// 내리는 힘 좀 더 추가
		// TODO : 찍는 것은 좋지 않다고 생각해서 뺀다.
		//{
		//	// _cameraTransform->Translate(DUOLMath::Vector3(0.f, -_shakePower.y, 0.f) * _cameraShakeTime * deltaTime * 10.f);

		//	_cameraTransform->Rotate(-_cameraTransform->GetRight(), -30.f * _shakePower.y * _cameraShakeTime * deltaTime * 3.f, DUOLGameEngine::Space::World);
		//}
		// 시간에 따라 감소하도록..!
		_shakePower = DUOLMath::Vector2::Lerp(DUOLMath::Vector2::Zero, _maxShakePower, _cameraShakeTime / _maxShakeTime);
		// Shake !
		float xShake = DUOLMath::MathHelper::RandF(-_shakePower.x * deltaTime, _shakePower.x * deltaTime);

		float yShake = DUOLMath::MathHelper::RandF(-_shakePower.y * deltaTime, _shakePower.y * deltaTime);

		// float zShake = DUOLMath::MathHelper::RandF(-_shakePower.y * deltaTime, _shakePower.y * deltaTime);

		_cameraTransform->Translate(DUOLMath::Vector3(xShake, yShake, 0.f), DUOLGameEngine::Space::Self);

		_cameraShakeTime -= deltaTime;
	}

	void MainCameraController::UpdateRotationValue(float deltaTime)
	{
		if (_isLockRotationByMouse || _mainCameraState == MainCameraState::CAMERA_SEQUENCE)
			return;

		if (_preMainCameraState == MainCameraState::CAMERA_SEQUENCE && _mainCameraState == MainCameraState::FOLLOW_PLAYER)
		{
			//	const DUOLMath::Quaternion& prevCameraRotation = _playerTransform->GetLocalRotation();

			//	//_cameraTransform->SetRotation(rot, DUOLGameEngine::Space::World);
			//	_cameraTransform->SetRotation(prevCameraRotation, DUOLGameEngine::Space::World);

			//	_rotY = _cameraTransform->GetTransform()->GetLocalEulerAngle().y;

			_preMainCameraState = _mainCameraState;


		}
		/*else
		{*/
		const DUOLMath::Quaternion& prevCameraRotation = _cameraTransform->GetWorldRotation();

		const DUOLMath::Vector2& prevMousePosition = DUOLGameEngine::InputManager::GetInstance()->GetPrevMousePosition();

		const DUOLMath::Vector2& currMousePosition = DUOLGameEngine::InputManager::GetInstance()->GetMousePosition();

		const DUOLMath::Vector2 deltaMouseMove = 0.15f * (prevMousePosition - currMousePosition);

		_rotX += -deltaMouseMove.y * _sensitivity * deltaTime;

		_rotY += -deltaMouseMove.x * _sensitivity * deltaTime;

		// Y 방향의 회전은 최소, 최대 제한을 둡니다.
		_rotX = std::clamp(_rotX, -_clampAngle, _clampAngle);

		// todo : x 축을 돌리는게 맞는 것 같은데 어째서 z 축을 돌릴 때 정상적으로 작동하는가...! 
		//DUOLMath::Quaternion rot = DUOLMath::Quaternion::CreateFromEulerAngle(0, DUOLMath::MathHelper::DegreeToRadian(_rotY), -DUOLMath::MathHelper::DegreeToRadian(_rotX));
		DUOLMath::Quaternion rot = DUOLMath::Quaternion::CreateFromEulerAngle(DUOLMath::MathHelper::DegreeToRadian(_rotX), DUOLMath::MathHelper::DegreeToRadian(_rotY), 0);

		//_cameraTransform->SetRotation(rot, DUOLGameEngine::Space::World);
		_cameraTransform->SetRotation(DUOLMath::Quaternion::Slerp(prevCameraRotation, rot, deltaTime * _smoothness), DUOLGameEngine::Space::World);

	}

	void MainCameraController::OnFollowPlayerState(float deltaTime)
	{
		const DUOLMath::Vector3& camPosition = _cameraTransform->GetWorldPosition();

		if (_followTransform == nullptr)
			return;

		// _followTransform 을 따라가자.
		DUOLMath::Vector3 dirToFollow = (_followTransform->GetWorldPosition() - camPosition);
		if (dirToFollow == DUOLMath::Vector3::Zero)
			dirToFollow = DUOLMath::Vector3::Backward;

		float lengthToFollow = (dirToFollow).Length();

		_cameraTransform->SetPosition(camPosition + dirToFollow.Normalized() * std::min(lengthToFollow * _followSpeed * deltaTime, lengthToFollow));

		// View Transform 이 지정되어 있으면 바라보는 것은 이쪽 방향이다 ..!
		if (_viewTransform != nullptr)
		{
			// _cameraTransform->LookAt(_viewTransform);

			const DUOLMath::Quaternion& currentRot = _cameraTransform->GetWorldRotation();

			// 회전만 시켜주기 위해서 회전 성분 빼낸다.
			DUOLMath::Quaternion lookAtQuat =
				DUOLMath::Quaternion::CreateFromRotationMatrix(DUOLMath::Matrix::CreateLookAt(_cameraTransform->GetWorldPosition(), _viewTransform->GetWorldPosition(), DUOLMath::Vector3::Up));

			lookAtQuat.Inverse(lookAtQuat);

			_cameraTransform->SetRotation(DUOLMath::Quaternion::Slerp(currentRot, lookAtQuat, deltaTime * _smoothness), DUOLGameEngine::Space::World);
		}

		const DUOLMath::Matrix& worldMat = _cameraTransform->GetWorldMatrix();

		// 최종 최대 방향 및 거리를 로컬에서 월드로 변경해줍니다.
		_finalDir = DUOLMath::Vector3::TransformNormal(_dirNormalized, worldMat);

		_finalDir *= _maxDistance;

		DUOLPhysics::RaycastHit hit;

		// 장애물이 막고 있으면 그 앞으로 땡겨줍니다.
		if (DUOLGameEngine::PhysicsManager::GetInstance()->Raycast(_realCameraTransform->GetWorldPosition(), _finalDir.Normalized(), _finalDir.Length(), _obstacleLayer, hit))
		{
			DUOLGameEngine::GameObject* hittedObject = reinterpret_cast<DUOLGameEngine::GameObject*>(hit._userData);

			if ((hittedObject != nullptr) /*&& (hittedObject->GetTag() == TEXT("Obstacle"))*/)
				_finalDistance = std::clamp(hit._hitDistance, _minDistance, _maxDistance);
		}
		else
		{
			_finalDistance = _maxDistance;
		}

		_realCameraTransform->SetLocalPosition(DUOLMath::Vector3::Lerp(_realCameraTransform->GetLocalPosition(), _dirNormalized * _finalDistance, std::clamp(_smoothness * deltaTime, 0.f, 1.f)));


	}

	void MainCameraController::SetLockRotationByMouse(bool value)
	{
		_isLockRotationByMouse = value;
	}

	void MainCameraController::SetCameraShake(float shakeTime, const DUOLMath::Vector2& shakePower, DUOLGameEngine::Transform* startTr)
	{
		_cameraShakeTime = shakeTime;

		_maxShakeTime = _cameraShakeTime;

		float distance = 0;

		if (startTr)
			distance = DUOLMath::Vector3::Distance(startTr->GetWorldPosition(), _cameraTransform->GetWorldPosition());

		float distanceOffset = 1 - std::min(1.0f, distance / _maxShakeDistance);

		_shakePower = shakePower * distanceOffset;

		_maxShakePower = _shakePower;
	}

	void MainCameraController::SetFollowTransform(DUOLGameEngine::Transform* followTransform)
	{
		_followTransform = followTransform;
	}

	void MainCameraController::SetViewTransform(DUOLGameEngine::Transform* viewTransform)
	{
		_viewTransform = viewTransform;

		// 뷰 트랜스폼이 있으면 마우스 회전이 아닌 공식을 이용해 카메라의 회전을 통제한다.
		_viewTransform != nullptr ? SetLockRotationByMouse(true) : SetLockRotationByMouse(false);
	}

	void MainCameraController::SetCameraState(MainCameraState state)
	{
		_preMainCameraState = _mainCameraState;
		_mainCameraState = state;

		if(_mainCameraState == MainCameraState::CAMERA_SEQUENCE)
			_realCameraTransform->SetLocalPosition({ 0.f, 0.f, 0.f });
		else
		{
			//플레이어가 있다고 가정한다.
			if (_player)
			{
				auto look =_player->GetTransform()->GetLook();
				look.Normalize();
				//todo 카메라의 로테이션과 변경해줘야하는데..

				_realCameraTransform->SetLocalPosition(_dirNormalized * _maxDistance);
			}
		}

		if (_player)
		{
			if (_mainCameraState == MainCameraState::CAMERA_SEQUENCE)
				_player->SetCanInteraction(false);
			else if (_mainCameraState == MainCameraState::FOLLOW_PLAYER)
				_player->SetCanInteraction(true);
		}
	}

	void MainCameraController::SetCameraInitPos()
	{
		auto object = this->GetGameObject();
		if (object == nullptr)
			return;
		for (auto childs : object->GetComponent<DUOLGameEngine::Transform>()->GetAllChildGameObjects())
		{
			if (childs->GetTag() == L"Camera")
			{
				childs->GetComponent<DUOLGameEngine::Transform>()->SetLocalPosition(DUOLMath::Vector3(0, 0, -10));
				childs->GetComponent<DUOLGameEngine::Transform>()->SetLocalEulerAngle(DUOLMath::Vector3(0, 0, 0));
			}
		}
	}


	void MainCameraController::OnStart()
	{
		MonoBehaviourBase::OnStart();


		if (DUOLGameEngine::SceneManager::GetInstance()->GetCurrentScene()->GetName() == L"TotalScene")
			_mainCameraState = MainCameraState::CAMERA_SEQUENCE;

		else
			_mainCameraState = MainCameraState::FOLLOW_PLAYER;

		// Follow (== Character)
		auto& allGOs = DUOLGameEngine::SceneManager::GetInstance()->GetCurrentScene()->GetAllGameObjects();

		for (auto gameObject : allGOs)
		{
			if (gameObject->GetTag() == TEXT("PlayerFollowRoot"))
			{
				_followTransform = gameObject->GetTransform();
			}
			if (gameObject->GetTag() == TEXT("Player"))
			{
				_playerTransform = gameObject->GetTransform();
				_player = gameObject->GetComponent<Player>();
			}
		}

		// Third Person
		_rotX = GetTransform()->GetLocalEulerAngle().x;
		_rotY = GetTransform()->GetLocalEulerAngle().y;

		// Transform Information
		for (auto gameObject : allGOs)
		{
			if (gameObject->GetTag() == TEXT("MainCamera"))
				_realCameraTransform = gameObject->GetTransform();

			if (_realCameraTransform == nullptr)
			{
				auto childs = GetTransform()->GetChildren();

				for (auto& child : childs)
				{
					if (child->GetTag() == TEXT("MainCamera"))
					{
						_realCameraTransform = child->GetTransform();
					}
				}
			}

			auto cam = _realCameraTransform->GetGameObject()->GetComponent<DUOLGameEngine::Camera>();

			//Camera Settings
			cam->SetFar(400);
			cam->SetFOV(65.f);

			_maxDistance = 6.f;
		}

		_cameraTransform = GetTransform();
		_dirNormalized = _realCameraTransform->GetLocalPosition().Normalized();
		_finalDistance = _realCameraTransform->GetLocalPosition().Length();
	}

	void MainCameraController::OnLateUpdate(float deltaTime)
	{
		// 마우스 입력에 따른 카메라 회전도
		UpdateRotationValue(deltaTime);

		switch (_mainCameraState)
		{
		case MainCameraState::FOLLOW_PLAYER:
		{
			OnFollowPlayerState(deltaTime);
			break;
		}

		case MainCameraState::UI_SELECT:
		{

			break;
		}

		case MainCameraState::CAMERA_SEQUENCE:
		{

			break;
		}
		}

		// 카메라 쉐이크는 가장 마지막에 업데이트하자.
		UpdateCameraShake(deltaTime);
	}

	void MainCameraController::OnFixedUpdate(float deltaTime)
	{
		//// 마우스 입력에 따른 카메라 회전도
		//UpdateRotationValue(deltaTime);

		//switch (_mainCameraState)
		//{
		//	case MainCameraState::FOLLOW_PLAYER:
		//	{
		//		OnFollowPlayerState(deltaTime);

		//		break;
		//	}

		//	case MainCameraState::UI_SELECT:
		//	{

		//		break;
		//	}

		//	case MainCameraState::CAMERA_SEQUENCE:
		//	{

		//		break;
		//	}
		//}

		//// 카메라 쉐이크는 가장 마지막에 업데이트하자.
		//UpdateCameraShake(deltaTime);
	}
}