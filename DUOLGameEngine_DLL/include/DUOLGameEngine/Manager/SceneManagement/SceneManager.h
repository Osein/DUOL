﻿/**

    @file      SceneManager.h
    @brief     Scene Management at run-time
    @details   ~
    @author    COMETLEE
    @date      19.10.2022
    @copyright © COMETLEE, 2022. All right reserved.

**/
#pragma once
#include <functional>
#include <map>
#include <vector>

#include "DUOLGameEngine/Manager/SceneManagement/Scene.h"

#include "DUOLGameEngine/Util/Defines.h"
#include "DUOLGameEngine/Util/SingletonBase.h"
#include <thread>

namespace DUOLGameEngine
{
	struct EngineSpecification;
	//// thread를 위해서 만들었는데, 나중에 다른곳에 필요할수도
	//enum class SceneState
	//{
	//	Main,
	//	Loading,
	//	InGame,
	//	End
	//};

	/**
	 * \brief 씬을 어떻게 로드할지 설정합니다. 디폴트는 Single Mode 입니다.
	 */
	enum class LoadSceneMode
	{
		/**
		 * \brief 현재 로드되어 있는 씬을 모드 종료하고 로드합니다.
		 */
		Single,

		/**
		 * \brief 현재 로드되어 있는 씬에 추가하고 로드합니다.
		 */
		Additive
	};

	/**
	 * \brief 실행 시간에 씬의 로드, 보유, 저장 등의 관리를 담당합니다.
	 */
	class DUOL_GAMEENGINE_API SceneManager : public SingletonBase<SceneManager>
	{
		DECLARE_SINGLETON(SceneManager)

		DELETE_COPY_MOVE(SceneManager)

	public:
		void Initialize();

		void UnInitialize();

		/**
		 * \brief 게임 플레이 로직을 업데이트합니다.
		 * \param deltaTime 프레임 간격
		 */
		void Update(float deltaTime);

		/**
		 * \brief Editor 의 Edit, Pause Mode에서의 로직을 업데이트합니다.
		 * \param deltaTime 프레임 간격
		 */
		void UpdateEditAndPauseMode(float deltaTime);

		/**
		 * \brief 씬을 바꿔야 한다면 바꿉니다.
		 */
		void TryChangeScene();

		void TryChangeSceneNoAwakeStart();

		bool IsSceneChanged() const;

	private:
		virtual ~SceneManager() override;

	private:
		/**
		 * \brief 해당 게임에서 빌드할 때 등록된 씬들의 집합 (TODO : Hard Code로 작성된 씬들의 모임이다.)
		 */
		std::map<DUOLCommon::tstring, std::shared_ptr<Scene>> _scenesInGame;

		/**
		 * \brief 일단 단일 씬의 루프에 대해서만 생각해보자 ..
		 */
		std::shared_ptr<DUOLGameEngine::Scene> _currentScene;

		std::shared_ptr<DUOLGameEngine::Scene> _reservedScene;

		std::thread* _thread;

	private:
		void ChangeScene();

		void ChangeSceneNoAwakeStart();

		bool _isReservedChangeScene;

		bool _isSceneChanged;

		bool _isCurrentSceneLoadedFromFile;

		DUOLCommon::tstring _nextFilePath;

		std::thread _sceneThread;
	public:
		Scene* GetCurrentScene();

		void LoadScene(const DUOLCommon::tstring& sceneName, LoadSceneMode mode = LoadSceneMode::Single);

		void AddGameScene(const std::shared_ptr<DUOLGameEngine::Scene>& scene);

		inline int GetSceneCount() const { return static_cast<int>(_scenesInGame.size()); }

		void SaveCurrentSceneTo(const DUOLCommon::tstring& filePath);

		DUOLGameEngine::Scene* LoadSceneFileFrom(const DUOLCommon::tstring& filePath);

		bool GetIsCurrentSceneLoadedFromFile() const;

		DUOLGameEngine::Scene* LoadEmptyScene();

		DUOLGameEngine::Scene* LoadUnityScene(const DUOLCommon::tstring& filePath);

		void DataLoadThread(const EngineSpecification& gameSpec);

		void LoadSceneResource(const EngineSpecification& gameSpec);

		const DUOLCommon::tstring& GetSceneName() { return _currentScene->GetName(); }

#pragma region FRIEND_CLASS
		friend class Engine;
#pragma endregion
	};
}