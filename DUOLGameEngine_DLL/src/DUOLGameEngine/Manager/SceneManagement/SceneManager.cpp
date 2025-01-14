#include <cassert>

#include "DUOLGameEngine/Manager/SceneManagement/SceneManager.h"

#include <filesystem>
#include <future>

#include "DUOLGameEngine/ECS/GameObject.h"
#include "DUOLGameEngine/ECS/Component/Light.h"
#include "DUOLGameEngine/ECS/Component/Camera.h"

#include "DUOLGameEngine/Manager/EventManager.h"
#include "DUOLGameEngine/Manager/InputManager.h"
#include "DUOLGameEngine/Manager/NavigationManager.h"
#include "DUOLGameEngine/Manager/SceneManagement/Scene.h"
#include "DUOLGameEngine/Manager/UnityMigrator/UnityMigrator.h"

#include "DUOLGameEngine/Manager/PhysicsManager.h"
#include "DUOLGameEngine/Manager/SerializeManager.h"
#include "DUOLGameEngine/Manager/UIManager.h"
#include "DUOLGameEngine/Manager/ResourceManager.h"


namespace DUOLGameEngine
{
	SceneManager::SceneManager() :
		_scenesInGame({})
		, _currentScene(nullptr)
		, _reservedScene(nullptr)
		, _isReservedChangeScene(false)
		, _isCurrentSceneLoadedFromFile(false)
		, _nextFilePath(L"")
	{
	}

	SceneManager::~SceneManager()
	{
		UnInitialize();
	}

	void SceneManager::ChangeScene()
	{
		// Don't Destroy On Load.
		if (_currentScene != nullptr)
			_reservedScene->AddDontDestroyOnLoadGameObjects(_currentScene.get());

		_currentScene = _reservedScene;

		_reservedScene.reset();

		_isReservedChangeScene = false;

		// 게임 오브젝트 리스트 세팅
		_currentScene->SetGameObjectList();

#pragma region NAVIGATION_MESH_INIT
		// 씬이 바뀌었으니, 만약 네비게이션 메쉬를 사용하는 씬이면 불러옵니다.
		if (!_currentScene->GetNavMeshFileName().empty())
		{
			NavigationManager::GetInstance()->SetCurrentNavMesh(_currentScene->GetNavMeshFileName());

			NavigationManager::GetInstance()->InitializeCurrentGameScene(_currentScene->_rootObjectsInScene);
		}
		else
			NavigationManager::GetInstance()->SetCurrentNavMesh(TEXT(""));
#pragma endregion

#pragma region PHYSICS_SCENE_INIT
		// 씬이 바뀌었으니 물리 관련 사항도 초기화합니다.
		PhysicsManager::GetInstance()->InitializeCurrentGameScene(_currentScene->_rootObjectsInScene);
#pragma endregion

#pragma region UI_INIT
		UIManager::GetInstance()->InitializeCurrentGameScene(_currentScene->_rootObjectsInScene);
#pragma endregion

		// 씬을 일으킨다.
		_currentScene->BuildStaticGameObjectsTree();

		_currentScene->AwakeAllGameObject();

		_currentScene->StartAllGameObject();

		_currentScene->UpdateGraphicsSettings();
		// 씬 체인지 이벤트 온
		DUOLGameEngine::EventManager::GetInstance()->InvokeEvent(TEXT("SceneChanging"));
	}

	void SceneManager::ChangeSceneNoAwakeStart()
	{
		_currentScene = _reservedScene;

		_reservedScene.reset();

		_isReservedChangeScene = false;

		// 게임 오브젝트 리스트 세팅
		_currentScene->SetGameObjectList();

		// 스태틱 트리
		_currentScene->BuildStaticGameObjectsTree();

		// 렌더러 
		_currentScene->RegisterAllRenderingEvent();

		_currentScene->UpdateGraphicsSettings();

#pragma region NAVIGATION_MESH_INIT
		// 씬이 바뀌었으니, 만약 네비게이션 메쉬를 사용하는 씬이면 불러옵니다.
		if (!_currentScene->GetNavMeshFileName().empty())
		{
			NavigationManager::GetInstance()->SetCurrentNavMesh(_currentScene->GetNavMeshFileName());

			NavigationManager::GetInstance()->InitializeCurrentGameScene(_currentScene->_rootObjectsInScene);
		}
		else
			NavigationManager::GetInstance()->SetCurrentNavMesh(TEXT(""));
#pragma endregion

#pragma region PHYSICS_SCENE_INIT
		// 씬이 바뀌었으니 물리 관련 사항도 초기화합니다.
		PhysicsManager::GetInstance()->InitializeCurrentGameScene(_currentScene->_rootObjectsInScene);
#pragma endregion

#pragma region UI_INIT

		UIManager::GetInstance()->InitializeCurrentGameScene(_currentScene->_rootObjectsInScene);
#pragma endregion

		// 씬 체인지 이벤트 온
		DUOLGameEngine::EventManager::GetInstance()->InvokeEvent(TEXT("SceneChanging"));
	}

	bool SceneManager::IsSceneChanged() const
	{
		return _isSceneChanged;
	}


	void SceneManager::SaveCurrentSceneTo(const DUOLCommon::tstring& filePath)
	{
		if (DUOLGameEngine::SerializeManager::GetInstance()->SerializeScene(_currentScene.get(), filePath))
		{
			DUOL_ENGINE_INFO(DUOL_CONSOLE, "Succeeded in saving the current scene.");
		}
		else
		{
			DUOL_ENGINE_CRITICAL("Failed in saving current scene.");
		}
	}

	DUOLGameEngine::Scene* SceneManager::LoadSceneFileFrom(const DUOLCommon::tstring& filePath)
	{
		std::filesystem::path p(filePath);

		DUOLCommon::tstring sceneName = p.stem();

		std::shared_ptr<DUOLGameEngine::Scene> loadedScene = nullptr;

		if ((loadedScene = DUOLGameEngine::SerializeManager::GetInstance()->DeserializeScene(filePath)) != nullptr)
		{
			DUOL_ENGINE_INFO(DUOL_CONSOLE, "Succeeded in loading the scene.");

			auto&& iter = _scenesInGame.find(sceneName);

			// 이름 설정, 항상 파일 이름과 씬 이름이 동일하게 움직입니다.
			loadedScene->_name = sceneName;

			// 등록한 씬 리스트에 해당하는 이름의 씬이 이미 있습니다. 제거하고 로드된 것으로 다시 넣어줍니다.
			if (iter != _scenesInGame.end())
			{
				// 기존의 씬을 딜리트합니다.
				iter->second.reset();

				iter->second = loadedScene;
			}
			// 아니라면 넣어줍니다.
			else
			{
				// TODO : Hard Coding scene 사라지면 .. 없어도 되는 문장이다. (기존에 로드된, 기억하고 있는 씬을 없앤다는 뜻이니까 ..)
				if ((_currentScene != nullptr) && (_scenesInGame.find(_currentScene->GetName()) != _scenesInGame.end()))
					_scenesInGame.erase(_scenesInGame.find(_currentScene->GetName()));

				_scenesInGame.insert({ { sceneName, loadedScene} });
			}

			_reservedScene = _scenesInGame.at(sceneName);

			_isReservedChangeScene = true;

			_isCurrentSceneLoadedFromFile = true;

			loadedScene->_path = filePath;

			return loadedScene.get();
		}
		else
		{
			DUOL_ENGINE_CRITICAL("Failed in loading the scene.");

			_isCurrentSceneLoadedFromFile = false;

			return nullptr;
		}

	}

	bool SceneManager::GetIsCurrentSceneLoadedFromFile() const
	{
		return _isCurrentSceneLoadedFromFile;
	}

	DUOLGameEngine::Scene* SceneManager::LoadEmptyScene()
	{
		std::shared_ptr<DUOLGameEngine::Scene> scene = std::make_shared<DUOLGameEngine::Scene>();

		// ---------------- Main Camera
		DUOLGameEngine::GameObject* mainCam = scene->CreateEmpty();

		mainCam->SetName(TEXT("Main Camera"));

		mainCam->AddComponent<DUOLGameEngine::Camera>();

		// ---------------- Directional Light
		DUOLGameEngine::GameObject* dirLight = scene->CreateEmpty();

		dirLight->SetName(TEXT("Directional Light"));

		dirLight->AddComponent<DUOLGameEngine::Light>();

		_reservedScene = scene;

		_isReservedChangeScene = true;

		_isCurrentSceneLoadedFromFile = false;

		// TODO : Hard Coding scene 사라지면 .. 없어도 되는 문장이다. (기존에 로드된, 기억하고 있는 씬을 없앤다는 뜻이니까 ..)
		if ((_currentScene != nullptr) && (_scenesInGame.find(_currentScene->GetName()) != _scenesInGame.end()))
			_scenesInGame.erase(_scenesInGame.find(_currentScene->GetName()));

		return scene.get();
	}

	DUOLGameEngine::Scene* SceneManager::LoadUnityScene(const DUOLCommon::tstring& filePath)
	{
		std::shared_ptr<DUOLGameEngine::Scene> scene =
			DUOLGameEngine::UnityMigrator::GetInstance()->MigrateUnitySceneFile(filePath);

		_reservedScene = scene;

		_isReservedChangeScene = true;

		_isCurrentSceneLoadedFromFile = false;

		if ((_currentScene != nullptr) && (_scenesInGame.find(_currentScene->GetName()) != _scenesInGame.end()))
			_scenesInGame.erase(_scenesInGame.find(_currentScene->GetName()));

		return scene.get();
	}

	void SceneManager::DataLoadThread(const EngineSpecification& gameSpec)
	{
		_sceneThread = std::thread(&SceneManager::LoadSceneResource, this, std::ref(gameSpec));
	}

	void SceneManager::LoadSceneResource(const EngineSpecification& gameSpec)
	{
		// thread를 돌려서 리소스를 넣어줍니다
		ResourceManager::GetInstance()->LateInitialize(gameSpec);
	}

	void SceneManager::Initialize()
	{
		_isReservedChangeScene = false;

		DUOL_INFO(DUOL_FILE, "SceneManager Initialize Success !");
	}

	void SceneManager::UnInitialize()
	{
		for (auto& scene : _scenesInGame)
			scene.second.reset();

		_currentScene.reset();

		_reservedScene.reset();
	}

	void SceneManager::Update(float deltaTime)
	{
		// 유니티 생애주기와 같은 순서로 현재 게임 로직을 업데이트합니다.
		if (_currentScene != nullptr)
		{
			//if (InputManager::GetInstance()->GetKeyDown(KeyCode::Keypad5))
			//{
			//	deltaTime *= 1000;
			//	DUOL_TRACE(DUOL_CONSOLE, "key down 5 | deltatime {0}", deltaTime);
			//}

			// 생성 요청된 오브젝트들을 생성합니다.

			_currentScene->CreateGameObjects();

			_currentScene->Update(deltaTime);

			_currentScene->InvokeUpdate(deltaTime);

			_currentScene->CoroutineUpdate(deltaTime);

			_currentScene->LateUpdate(deltaTime);

			// 이번 프레임에 비활성화된 게임 오브젝트들을 비활성화합니다.
			_currentScene->InActiveGameObjects();

			// 이번 프레임에 활성화된 게임 오브젝트들을 활성화합니다.
			_currentScene->ActiveGameObjects();

			_currentScene->DestroyComponents(deltaTime);

			// 파괴 요청된 게임 오브젝트들을 파괴합니다.
			_currentScene->DestroyGameObjects();
		}
	}

	void SceneManager::UpdateEditAndPauseMode(float deltaTime)
	{
		/*if (DUOLGameEngine::ResourceManager::GetInstance()->_isThread)
		{
			if (_sceneThread.joinable())
			{
				_sceneThread.join();
				LoadSceneFileFrom(L"Asset/Scene/StartScene.dscene");
			}
		}*/

		if (_currentScene != nullptr)
		{
			_currentScene->CreateGameObjects();

			_currentScene->InActiveGameObjects();

			_currentScene->ActiveGameObjects();

			_currentScene->DestroyComponents(deltaTime);

			_currentScene->DestroyGameObjects();
		}
	}

	void SceneManager::TryChangeScene()
	{
		if (_isReservedChangeScene)
		{
			ChangeScene();
			_isSceneChanged = true;
		}
		else
		{
			_isSceneChanged = false;
		}
	}

	void SceneManager::TryChangeSceneNoAwakeStart()
	{
		if (_isReservedChangeScene)
			ChangeSceneNoAwakeStart();
	}

	Scene* SceneManager::GetCurrentScene()
	{
		return _currentScene.get();
	}

	void SceneManager::LoadScene(const DUOLCommon::tstring& sceneName, LoadSceneMode mode)
	{
		auto&& iter = _scenesInGame.find(sceneName);

		// 등록한 씬 리스트에 해당하는 이름의 씬이 없습니다 ..!
		if (iter == _scenesInGame.end())
		{
			assert(false);
		}

		_reservedScene = (*iter).second;

		_isReservedChangeScene = true;
	}

	void SceneManager::AddGameScene(const std::shared_ptr<DUOLGameEngine::Scene>& scene)
	{
		if (scene != nullptr)
		{
			_scenesInGame.insert({ scene->GetName(), scene });
		}
	}
}
