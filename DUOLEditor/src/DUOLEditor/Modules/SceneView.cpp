#include "DUOLEditor/Modules/SceneView.h"

#include "DUOLEditor/Modules/EditorEventManager.h"
#include "DUOLEditor/UI/Widgets/Display/Image.h"

#include "DUOLGameEngine/ECS/Component/Camera.h"
#include "DUOLGameEngine/ECS/Component/Light.h"

#include "DUOLGameEngine/Manager/GraphicsManager.h"
#include "DUOLGameEngine/Manager/EventManager.h"
#include "DUOLGameEngine/Manager/InputManager.h"
#include "DUOLGameEngine/Manager/ResourceManager.h"
#include "DUOLGameEngine/Manager/SceneManagement/SceneManager.h"

#include "DUOLGameEngine/ECS/GameObject.h"
#include "DUOLGameEngine/ECS/Object/Material.h"
#include "DUOLGameEngine/ECS/Object/Mesh.h"

namespace DUOLEditor
{
	SceneView::SceneView(const DUOLCommon::tstring& title, bool isOpened, const PanelWindowSetting& windowSetting) :
		ControllableViewBase(title, isOpened, windowSetting)
		, _currentTransformGizmoState(TransformGizmoState::Translate)
		, _isTransformOperationGlobal(true)
		, _selectedAxis(TransformGizmoSelectedAxis::None)
	{
		_cameraGizmoMaterials.push_back(DUOLGameEngine::ResourceManager::GetInstance()->GetMaterial(TEXT("CamMat"))->GetPrimitiveMaterial());

		for (int i = 0; i < MAX_CAMERA_GIZMO; i++)
		{
			_cameraGizmoMeshInfos[i].SetTransformPointer(&_cameraGizmoTransform[i]);

			_cameraGizmos[i]._renderInfo = &_cameraGizmoMeshInfos[i];

			_cameraGizmos[i]._mesh = DUOLGameEngine::ResourceManager::GetInstance()->GetMesh(TEXT("Camera"))->GetPrimitiveMesh();

			_cameraGizmos[i]._materials = &_cameraGizmoMaterials;
		}

		_transformRedMaterials.push_back(DUOLGameEngine::ResourceManager::GetInstance()->GetMaterial(TEXT("TransformRed"))->GetPrimitiveMaterial());

		// 제가 원하는 파이프라인 상태로 바꿔줍니다.
		DUOLGameEngine::ResourceManager::GetInstance()->GetMaterial(TEXT("TransformRed"))->GetPrimitiveMaterial()->
			SetPipelineState(DUOLGameEngine::GraphicsManager::GetInstance()->GetPipelineState(TEXT("Gizmo")));

		_transformGreenMaterials.push_back(DUOLGameEngine::ResourceManager::GetInstance()->GetMaterial(TEXT("TransformGreen"))->GetPrimitiveMaterial());

		DUOLGameEngine::ResourceManager::GetInstance()->GetMaterial(TEXT("TransformGreen"))->GetPrimitiveMaterial()->
			SetPipelineState(DUOLGameEngine::GraphicsManager::GetInstance()->GetPipelineState(TEXT("Gizmo")));

		_transformBlueMaterials.push_back(DUOLGameEngine::ResourceManager::GetInstance()->GetMaterial(TEXT("TransformBlue"))->GetPrimitiveMaterial());

		DUOLGameEngine::ResourceManager::GetInstance()->GetMaterial(TEXT("TransformBlue"))->GetPrimitiveMaterial()->
			SetPipelineState(DUOLGameEngine::GraphicsManager::GetInstance()->GetPipelineState(TEXT("Gizmo")));

		_transformCenterMaterials.push_back(DUOLGameEngine::ResourceManager::GetInstance()->GetMaterial(TEXT("TransformWhite"))->GetPrimitiveMaterial());

		DUOLGameEngine::ResourceManager::GetInstance()->GetMaterial(TEXT("TransformWhite"))->GetPrimitiveMaterial()->
			SetPipelineState(DUOLGameEngine::GraphicsManager::GetInstance()->GetPipelineState(TEXT("Gizmo")));

		for (int i = 0 ; i < 9 ; i++)
		{
			_transformGizmoMeshInfos[i].SetTransformPointer(&_transformGizmoTransform[i]);

			_transformGizmos[i]._renderInfo = &_transformGizmoMeshInfos[i];

			if (i < 3)
				_transformGizmos[i]._mesh = DUOLGameEngine::ResourceManager::GetInstance()->GetMesh(TEXT("Arrow_Scale"))->GetPrimitiveMesh();
			else if (i < 6)
				_transformGizmos[i]._mesh = DUOLGameEngine::ResourceManager::GetInstance()->GetMesh(TEXT("Arrow_Rotate"))->GetPrimitiveMesh();
			else
				_transformGizmos[i]._mesh = DUOLGameEngine::ResourceManager::GetInstance()->GetMesh(TEXT("Arrow_Translate"))->GetPrimitiveMesh();

			// Look Right Up
			if (i % 3 == 0)
				_transformGizmos[i]._materials = &_transformBlueMaterials;
			else if (i % 3 == 1)
				_transformGizmos[i]._materials = &_transformRedMaterials;
			else if (i % 3 == 2)
				_transformGizmos[i]._materials = &_transformGreenMaterials;
		}

		_transformCenterGizmoMeshInfo.SetTransformPointer(&_transformCenterGizmoTransform);

		_transformCenterGizmo._renderInfo = &_transformCenterGizmoMeshInfo;

		_transformCenterGizmo._mesh = DUOLGameEngine::ResourceManager::GetInstance()->GetMesh(TEXT("Transform_Center"))->GetPrimitiveMesh();

		_transformCenterGizmo._materials = &_transformCenterMaterials;

		// Select GameObject Event 등록
		DUOLEditor::EditorEventManager::GetInstance()->GetGameObjectSelectedEvent() +=
			std::bind([this](DUOLGameEngine::GameObject* gameObject) { this->_selectedGameObject = gameObject; },
				std::placeholders::_1);

		// UnSelect GameObject Event 등록
		DUOLEditor::EditorEventManager::GetInstance()->GetGameObjectUnselectedEvent() +=
			std::bind([this]() { this->_selectedGameObject = nullptr; });
	}

	SceneView::~SceneView()
	{

	}

	void SceneView::RenderOutline()
	{
		uint64_t selectedID = _selectedGameObject->GetUUID();

		DUOLGameEngine::GraphicsManager::GetInstance()->_renderingPipelineLayouts.at(TEXT("IDOutline"))->_dataSize = sizeof(uint64_t);
		DUOLGameEngine::GraphicsManager::GetInstance()->_renderingPipelineLayouts.at(TEXT("IDOutline"))->_perObjectBufferData = &selectedID;

		DUOLGameEngine::GraphicsManager::GetInstance()->Execute(TEXT("IDOutline"), true, false);
	}

	void SceneView::RenderAllPerspectiveGizmo()
	{
		DUOLGameEngine::Scene* scene = DUOLGameEngine::SceneManager::GetInstance()->GetCurrentScene();

		auto&& allGameObjects = scene->GetAllGameObjects();

		int cameraCount = 0;

		for (int i = 0; i < allGameObjects.size(); i++)
		{
			DUOLGameEngine::GameObject* gameObject = allGameObjects[i];

			DUOLGameEngine::Transform* transform = gameObject->GetTransform();

			DUOLGameEngine::GameObject* rootGameObject = transform->GetRootTransform()->GetGameObject();

			DUOLGameEngine::Camera* camera = gameObject->GetComponent<DUOLGameEngine::Camera>();

			DUOLGameEngine::Light* light = gameObject->GetComponent<DUOLGameEngine::Light>();

			if (camera != nullptr)
			{
				const DUOLMath::Matrix& worldTM = transform->GetWorldMatrix();

				_cameraGizmoTransform[cameraCount]._world = worldTM;

				_cameraGizmoTransform[cameraCount]._worldInvTranspose = worldTM.Invert().Transpose();

				_cameraGizmoMeshInfos[cameraCount].SetObjectID(rootGameObject->GetUUID());

				DUOLGameEngine::GraphicsManager::GetInstance()->ReserveRenderObject(&_cameraGizmos[cameraCount++]);
			}
		}
	}

	void SceneView::RenderTransformGizmo()
	{
#pragma region TRANSFORM_GIZMO
		if (_selectedGameObject != nullptr)
		{
			if (_isTransformOperationGlobal)
			{
				const DUOLMath::Vector3& selectedTranslation = _selectedGameObject->GetComponent<DUOLGameEngine::Transform>()->GetWorldPosition();

				// Perspective Transform을 실시해서 현재 그려지는 화면에서 어디에 위치하는지 확인한다.
				DUOLMath::Vector4 calcSelc = DUOLMath::Vector4{ selectedTranslation.x, selectedTranslation.y ,selectedTranslation.z , 1.f };

				calcSelc = DUOLMath::Vector4::Transform(calcSelc, _perspectiveCamera->_viewMatrix * _perspectiveCamera->_projectionMatrix);

				calcSelc.x = (calcSelc.x / calcSelc.w) * DUOLGameEngine::GraphicsManager::GetInstance()->GetScreenSize().x / 2;
				calcSelc.y = (calcSelc.y / calcSelc.w) * DUOLGameEngine::GraphicsManager::GetInstance()->GetScreenSize().y / 2;
				calcSelc.z = (calcSelc.z / calcSelc.w);

				// NDC 좌표 + 현재 스크린 해상도에서 어느 곳에 위치하는지 확인합니다.
				DUOLMath::Vector3 calcSel = DUOLMath::Vector3{calcSelc.x, calcSelc.y, 100.f};

				DUOLMath::Vector3 calcSelSphe = DUOLMath::Vector3{calcSelc.x, calcSelc.y, 50.f};

				DUOLMath::Matrix camWorldRot = DUOLMath::Matrix::CreateFromQuaternion(_orthographicCamera->GetTransform()->GetWorldRotation()).Invert();

				DUOLMath::Matrix lookMat = DUOLMath::Matrix::CreateScale(DUOLMath::Vector3{ 100.f, 100.f ,100.f }) * camWorldRot * DUOLMath::Matrix::CreateTranslation(calcSel)
					* _orthographicCamera->GetTransform()->GetWorldMatrix();
					
				DUOLMath::Matrix rightMat = DUOLMath::Matrix::CreateScale(DUOLMath::Vector3{ 100.f, 100.f ,100.f }) * DUOLMath::Matrix::CreateRotationY(DUOLMath::PI / 2) * camWorldRot * DUOLMath::Matrix::CreateTranslation(calcSel)
					*_orthographicCamera->GetTransform()->GetWorldMatrix();
					
				DUOLMath::Matrix upMat = DUOLMath::Matrix::CreateScale(DUOLMath::Vector3{ 100.f, 100.f ,100.f }) * DUOLMath::Matrix::CreateRotationX(-DUOLMath::PI / 2) * camWorldRot * DUOLMath::Matrix::CreateTranslation(calcSel)
					* _orthographicCamera->GetTransform()->GetWorldMatrix();

				DUOLMath::Matrix centerMat = DUOLMath::Matrix::CreateScale(DUOLMath::Vector3{ 10.f, 10.f,10.f}) * DUOLMath::Matrix::CreateTranslation(calcSelSphe)
				* _orthographicCamera->GetTransform()->GetWorldMatrix();

				_transformGizmoTransform[static_cast<int>(_currentTransformGizmoState) + 0]._world = lookMat;

				_transformGizmoTransform[static_cast<int>(_currentTransformGizmoState) + 0]._worldInvTranspose = lookMat.Invert().Transpose();

				_transformGizmoMeshInfos[static_cast<int>(_currentTransformGizmoState) + 0].SetObjectID(_selectedGameObject->GetUUID() + 1);
				
				_transformGizmoTransform[static_cast<int>(_currentTransformGizmoState) + 1]._world = rightMat;

				_transformGizmoTransform[static_cast<int>(_currentTransformGizmoState) + 1]._worldInvTranspose = rightMat.Invert().Transpose();

				_transformGizmoMeshInfos[static_cast<int>(_currentTransformGizmoState) + 1].SetObjectID(_selectedGameObject->GetUUID() + 2);

				_transformGizmoTransform[static_cast<int>(_currentTransformGizmoState) + 2]._world = upMat;

				_transformGizmoTransform[static_cast<int>(_currentTransformGizmoState) + 2]._worldInvTranspose = upMat.Invert().Transpose();

				_transformGizmoMeshInfos[static_cast<int>(_currentTransformGizmoState) + 2].SetObjectID(_selectedGameObject->GetUUID() + 3);

				_transformCenterGizmoTransform._world = centerMat;

				_transformCenterGizmoTransform._worldInvTranspose = centerMat.Invert().Transpose();

				_transformCenterGizmoMeshInfo.SetObjectID(_selectedGameObject->GetUUID());

				DUOLGameEngine::GraphicsManager::GetInstance()->ReserveRenderObject(&_transformGizmos[static_cast<int>(_currentTransformGizmoState) + 0]);

				DUOLGameEngine::GraphicsManager::GetInstance()->ReserveRenderObject(&_transformGizmos[static_cast<int>(_currentTransformGizmoState) + 1]);

				DUOLGameEngine::GraphicsManager::GetInstance()->ReserveRenderObject(&_transformGizmos[static_cast<int>(_currentTransformGizmoState) + 2]);

				DUOLGameEngine::GraphicsManager::GetInstance()->ReserveRenderObject(&_transformCenterGizmo);
			}
		}
#pragma endregion
	}

	void SceneView::TransformGizmoUpdate()
	{
		const DUOLMath::Vector2& prevMousePos = DUOLGameEngine::InputManager::GetInstance()->GetPrevMousePositionInScreen();

		const DUOLMath::Vector2& currMousePos = DUOLGameEngine::InputManager::GetInstance()->GetMousePositionInScreen();

		DUOLMath::Vector2 mouseDiff = currMousePos - prevMousePos;

		switch (_selectedAxis)
		{
			case DUOLEditor::TransformGizmoSelectedAxis::Look:
			{
				// 어떻게하면 사영할 수 있을까

				break;
			}

			case DUOLEditor::TransformGizmoSelectedAxis::Right:
			{

				break;
			}

			case DUOLEditor::TransformGizmoSelectedAxis::Up:
			{

				break;
			}

			case DUOLEditor::TransformGizmoSelectedAxis::None:
			{

				break;
			}
		}
	}

	void SceneView::ObjectPicking_SceneView(const DUOLMath::Vector2& currentTextureSize,
	                                        const DUOLMath::Vector2& mousePosition)
	{
		auto&& pickedID = ControllableViewBase::ObjectPicking(_image->_size, mousePosition);

#pragma region UI_GIZMO_PICKING
		// 선택된 게임 오브젝트가 이미 있고
		if (_selectedGameObject != nullptr)
		{
			// 현재 선택된 게임 오브젝트의 아이디
			uint64_t&& selectedID = _selectedGameObject->GetUUID();

			// 눌린 곳의 아이디가 Transform Gizmo의 ID라면 축 선택 상태로 전환합니다 ..?
			if ((pickedID == selectedID + 1) || (pickedID == selectedID + 2) || (pickedID == selectedID + 3))
			{
				_selectedAxis = static_cast<DUOLEditor::TransformGizmoSelectedAxis>(pickedID - selectedID);

				return;
			}
		}
#pragma endregion

#pragma region GAMEOBJECT_PICKING
		DUOLGameEngine::Scene* scene = DUOLGameEngine::SceneManager::GetInstance()->GetCurrentScene();

		if (scene != nullptr)
		{
			// Root Object의 선택만 가능합니다.
			auto&& rootObjects = scene->GetRootObjects();

			for (auto& rootObject : rootObjects)
			{
				// 선택이 되었습니다.
				if (pickedID == rootObject->GetUUID())
				{
					DUOLEditor::EditorEventManager::GetInstance()->SelectGameObject(rootObject);

					return;
				}
			}
		}

		// 만약 선택된 게임 오브젝트도 없고 Gizmo도 없다면 Unselect + Selected Axis 도 없도록 지정합니다.
		DUOLEditor::EditorEventManager::GetInstance()->UnselectGameObject();

		_selectedAxis = TransformGizmoSelectedAxis::None;
#pragma endregion
	}

	void SceneView::Update(float deltaTime)
	{
		ControllableViewBase::Update(deltaTime);

		// 0. 이전에 Game View (== Game) 가 그려졌다. 복사해놓자
		DUOLGameEngine::GraphicsManager::GetInstance()->CopyTexture(TEXT("CopyGameView"), TEXT("GameView"));

		// 1. SceneView의 이미지가 호출해야할 텍스처를 업데이트합니다.
		_image->_size = GetSafeSize();

#pragma region GameView를 그릴 때 Context를 초기화하지 않았기 때문에 Rendering, Lighting 등의 이벤트를 한 번 더 일으킬 필요가 없습니다 ..!
		// 1. SceneRender Event Invoke
		// DUOLGameEngine::EventManager::GetInstance()->InvokeEvent(TEXT("SceneRendering"));

		// 2. SceneLighting Event Invoke
		// DUOLGameEngine::EventManager::GetInstance()->InvokeEvent(TEXT("SceneLighting"));
#pragma endregion

		DUOLGameEngine::GraphicsManager::GetInstance()->UpdateRenderScreenSize(_image->_size);

		// 1-1. Perspective Gizmo Render Event
		RenderAllPerspectiveGizmo();

		// 3. Perspective Camera Info
		if (_perspectiveCamera != nullptr)
		{
			// Game View 화면 사이즈에 맞게 카메라 세팅 변경
			_perspectiveCamera->OnResize(&_image->_size);

			DUOLGameEngine::GraphicsManager::GetInstance()->UpdateCameraInfo(&_perspectiveCamera->GetCameraInfo());
		}

		// 4. Execute - Perspective
		DUOLGameEngine::GraphicsManager::GetInstance()->Execute(TEXT("Scene"), true);

		// 5 - 0. Orthographic Camera Info
		if (_orthographicCamera != nullptr)
		{
			DUOLMath::Vector2 screenSize = DUOLGameEngine::GraphicsManager::GetInstance()->GetScreenSize();

			// Game View 화면 사이즈에 맞게 카메라 세팅 변경
			_orthographicCamera->OnResize(&screenSize);

			DUOLGameEngine::GraphicsManager::GetInstance()->UpdateCameraInfo(&_orthographicCamera->GetCameraInfo());
		}

		if (_selectedGameObject != nullptr)
		{
			RenderOutline();

			RenderTransformGizmo();
		}

		// Orthographic Gizmo 를 그립니다.
		DUOLGameEngine::GraphicsManager::GetInstance()->Execute(TEXT("SceneView_Gizmo"), true, false);

		if (GetIsHovered())
		{
			// 마우스 위치도 스크린 좌표로 합시다.
			DUOLMath::Vector2 mousePosition = DUOLGameEngine::InputManager::GetInstance()->GetMousePositionInScreen();

			// 현재 View 기준으로 마우스의 위치를 옮겨 어느 위치의 픽셀에 마우스가 올라갔는지 검사합니다.
			mousePosition.x -= _position.x;

			mousePosition.y = mousePosition.y - _position.y - ImGui::GetFrameHeight();

			// 왼쪽 버튼이 다운되었습니다. Object ID Picking
			if (DUOLGameEngine::InputManager::GetInstance()->GetMouseButtonDown(DUOLGameEngine::MouseCode::Left))
			{
				ObjectPicking_SceneView(_image->_size, mousePosition);
			}
			// 왼쪽 버튼이 눌리고 있습니다.
			else if (DUOLGameEngine::InputManager::GetInstance()->GetMouseButtonPressed(DUOLGameEngine::MouseCode::Left))
			{
				// 현재 계속 눌리고 있는 축이 있다면 트랜스폼 업데이트 ?
				if (_selectedAxis != TransformGizmoSelectedAxis::None)
				{
					TransformGizmoUpdate();
				}
			}

		}

		// 윈도우가 Focus가 된 상태에서 ..
		if (GetIsFocused())
		{
			// Gizmo 변경 기능
			if (DUOLGameEngine::InputManager::GetInstance()->GetKeyDown(DUOLGameEngine::KeyCode::Alpha1))
			{
				_currentTransformGizmoState = TransformGizmoState::Scale;
			}
			else if (DUOLGameEngine::InputManager::GetInstance()->GetKeyDown(DUOLGameEngine::KeyCode::Alpha2))
			{
				_currentTransformGizmoState = TransformGizmoState::Rotate;
			}
			else if (DUOLGameEngine::InputManager::GetInstance()->GetKeyDown(DUOLGameEngine::KeyCode::Alpha3))
			{
				_currentTransformGizmoState = TransformGizmoState::Translate;
			}
		}

		// SceneView panel은 최종적인 Object를 그립니다.
		_image->_textureID = DUOLGameEngine::GraphicsManager::GetInstance()->GetShaderResourceAddress(TEXT("SceneView"));
	}
}