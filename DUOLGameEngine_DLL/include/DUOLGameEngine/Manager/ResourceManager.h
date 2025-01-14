﻿/**

	@file      ResourceManager.h
	@brief     Resource Management class.
	@details   ~
	@author    COMETLEE
	@date      1.11.2022
	@copyright © COMETLEE, 2022. All right reserved.

**/
#pragma once
#include <unordered_map>

#include "DUOLCommon/Util/UUID.h"
#include "DUOLGameEngine/Util/SingletonBase.h"
#include "DUOLGameEngine/Util/Defines.h"
#include "DUOLGameEngine/Util/EngineSpecification.h"
#include "DUOLGraphicsEngine/ResourceManager/Resource/Light.h"
#include "DUOLGraphicsLibrary/FontEngine/IFontEngine.h"
#include "DUOLMath/DUOLMath.h"

namespace DUOLGraphicsLibrary
{
	struct TextureDesc;
	class Texture;
	struct Sprite;
}

namespace DUOLPhysics
{
	class PhysicsSystem;
}

namespace DUOLGraphicsEngine
{
	struct RenderingData_Particle;
	struct MaterialDesc;
	class Model;
	class Material;
	class Mesh;
	class GraphicsEngine;
}

namespace DUOLGameEngine
{
	class AudioClip;
	class Sprite;
	class Texture;
	class ObjectBase;
	class Avatar;
	class Mesh;
	class Material;
	class PhysicsMaterial;
	class AnimationClip;
	class AnimatorController;
}

namespace DUOLGameEngine
{
	/**
	 * \brief 게임 진행 중 리소스와 관련된 사항의
	 * 로드와 언로드, 다른 모듈들과의 연동을 담당하는 매니저
	 */
	class DUOL_GAMEENGINE_API ResourceManager : public SingletonBase<ResourceManager>
	{
		DECLARE_SINGLETON(ResourceManager)

		DELETE_COPY_MOVE(ResourceManager)

	private:
#pragma region MODULES_USING_RESOURCE
		std::shared_ptr<DUOLGraphicsEngine::GraphicsEngine> _graphicsEngine;

		std::shared_ptr<DUOLPhysics::PhysicsSystem> _physicsSystem;
#pragma endregion

	private:
		virtual ~ResourceManager() override;

		/**
		 * \brief 게임 엔진 스펙에 맞게 정리된 FBX Model Table을 로드합니다.
		 * \param path 해당 테이블이 위치한 경로입니다.
		 */
		void LoadFBXTable(const DUOLCommon::tstring& path);

		// void LoadMaterial(const DUOLCommon::tstring& path);

		 /**
		  * \brief 게임 엔진 스펙에 맞게 정리된 Material table을 로드합니다.
		  * \param path 해당 테이블이 위치한 경로입니다.
		  */
		void LoadMaterialTable(const DUOLCommon::tstring& path);

		/**
		 * \brief 게임 엔진 스펙에 맞게 정리된 PhysicsMaterial table을 로드합니다.
		 * \param path 해당 테이블이 위치한 경로입니다.
		 */
		void LoadPhysicsMaterialTable(const DUOLCommon::tstring& path);

		/**
		 * \brief 게임 엔진 스펙에 맞게 정리된 AniatmionClip table을 로드합니다.
		 * \param path 해당 테이블이 위치한 경로입니다.
		 */
		void LoadAnimationClipTable(const DUOLCommon::tstring& path);

		/**
		 * \brief 게임 엔진 스펙에 맞게 정리된 AnimatorController table을 로드합니다.
		 * \param path 해당 테이블이 위치한 경로입니다.
		 */
		void LoadAnimatorControllerTable(const DUOLCommon::tstring& path);

		/**
		 * \brief 오디오 클립을 로드하자 ..
		 * \param path
		 */
		void LoadAudioClipTable(const DUOLCommon::tstring& path);

		void PlayerAudioClipTable();

		void LoadPrefabTable(const DUOLCommon::tstring& path);

		void LoadDataNameTable(const DUOLCommon::tstring& path, bool ismaterial);

		//void SetUseData(uint64 meshid, std::pair<std::vector<uint64>, std::vector<uint64>>& modeldata);

		// 하드 코딩으로 로드 중. 
		void LoadAnimatorController_CloseEnemy();
		void LoadAnimatorController_FarEnemy();
		void LoadAnimatorController_EliteEnemy();
		void LoadAnimatorController_WeakEliteEnemy();
		void LoadAnimatorController_BossEnemy();
		// 하드 코딩으로 로드 중. 
	private:
		/**
		 * \brief Graphics Model ID (이름) 과 포인터를 연결합니다.
		 * TODO : Prefab으로 바꾸고 싶습니다 .. (FBX 로딩 시 Prefab 화 하는 기능이 필요함 ..)
		 */
		std::unordered_map<DUOLCommon::tstring, DUOLGraphicsEngine::Model*> _modelIDMap;

		/**
		 * \brief Mesh의 ID (이름) 과 포인터를 연결합니다.
		 */
		std::unordered_map<DUOLCommon::tstring, std::shared_ptr<DUOLGameEngine::Mesh>> _meshIDMap;

		/**
		 * \brief Avatar의 ID (이름) 과 포인터를 연결합니다.
		 */
		std::unordered_map<DUOLCommon::tstring, std::shared_ptr<DUOLGameEngine::Avatar>> _avatarIDMap;

		/**
		 * \brief Material의 ID (이름) 과 포인터를 연결합니다.
		 */
		std::unordered_map<DUOLCommon::tstring, std::shared_ptr<DUOLGameEngine::Material>> _materialIDMap;

		/**ㅈ
		 * \brief PhysicsMaterial의 ID (이름) 과 포인터를 연결합니다.
		 */
		std::unordered_map<DUOLCommon::tstring, std::shared_ptr<DUOLGameEngine::PhysicsMaterial>> _physicsMaterialIDMap;

		/**
		 * \brief Animation Clip의 ID (이름) 과 포인터를 연결합니다.
		 */
		std::unordered_map<DUOLCommon::tstring, std::shared_ptr<DUOLGameEngine::AnimationClip>> _animationClipIDMap;

		/**
		 * \brief AnimatorController의 ID (이름) 과 포인터를 연결합니다.
		 */
		std::unordered_map<DUOLCommon::tstring, std::shared_ptr<DUOLGameEngine::AnimatorController>> _animatorControllerIDMap;

		/**
		* \brief RenderingData_Particle의 ID (이름) 과 포인터를 연결합니다.
		*/
		std::unordered_map<DUOLCommon::tstring, std::vector<std::shared_ptr<DUOLGraphicsEngine::RenderingData_Particle>>> _renderingData_ParticleIDMap;

		///**
		// * \brief Sprite의 ID (이름) 과 포인터를 연결합니다.
		// */
		std::unordered_map<DUOLCommon::tstring, std::shared_ptr<DUOLGameEngine::Sprite>> _spriteIDMap;

		/**
		 * \brief Resource Object
		 */
		std::unordered_map<DUOLCommon::UUID, DUOLGameEngine::ObjectBase*> _resourceUUIDMap;

		/**
		 * \brief 모든 Perfab의 Mesh Id Material Id Animation Id
		 */
		//std::vector<std::pair<uint64, std::pair<std::vector<uint64>, std::vector<uint64>>>> _perfabsIDList;

	public:
		/**
		 * \brief 그래픽스로 넘기기위한 변수입니다.
		 * 추후 수정할 예정입니다.
		 */
		std::vector<std::pair<uint64, DUOLCommon::tstring>> _materialNameList;

		std::vector<std::pair<uint64, DUOLCommon::tstring>> _animationNameList;
		/**
		 * \brief 모든 Perfab의 Mesh Id Material Id Animation Id
		 */

		DUOLGameEngine::Avatar* GetAvatar(const DUOLCommon::tstring& avatarID) const;

		bool _isThread;
	public:
		DUOLGameEngine::Mesh* GetMesh(const DUOLCommon::tstring& meshID) const;

		const std::unordered_map<DUOLCommon::tstring, std::shared_ptr<DUOLGameEngine::Mesh>>& GetAllMeshes() const;

		DUOLGameEngine::AnimationClip* GetAnimationClip(const DUOLCommon::tstring& animationClipID) const;

		DUOLGameEngine::Material* GetMaterial(const DUOLCommon::tstring& materialID) const;

		DUOLGameEngine::PhysicsMaterial* GetPhysicsMaterial(const DUOLCommon::tstring& physicsMaterialID) const;

		std::unordered_map<DUOLCommon::tstring, std::shared_ptr<DUOLGameEngine::Sprite>>& GetSprite() { return _spriteIDMap; }

		DUOLGameEngine::AnimatorController* GetAnimatorController(const DUOLCommon::tstring& animatorControllerID) const;

		// TODO : Prefab으로 바꾸고 싶습니다 .. 기술력이 늘어난다면 ..
		DUOLGraphicsEngine::Model* GetModel(const DUOLCommon::tstring& modelID) const;

		DUOLGameEngine::Sprite* GetSprite(const DUOLCommon::tstring& spriteID) const;

		DUOLGameEngine::AudioClip* GetAudioClip(const DUOLCommon::tstring& audioClipID) const;

		const std::unordered_map<DUOLCommon::tstring, std::unique_ptr<DUOLGameEngine::AudioClip>>& GetAllAudioClips() const;

		const std::unordered_map<DUOLCommon::tstring, std::shared_ptr<DUOLGameEngine::AnimatorController>>& GetAllAnimatorControllers() const;

		const std::unordered_map<DUOLCommon::tstring, std::shared_ptr<DUOLGameEngine::Material>>& GetAllMaterialMap() const;

		void InsertSprite(const DUOLCommon::tstring& spriteName);

		bool GetMeshInfo(const DUOLCommon::tstring& meshID, std::vector<DUOLMath::Vector3>& vertexInfo, std::vector<UINT32>& indexInfo) const;

		bool GetMeshInfo(const DUOLGameEngine::Mesh* mesh, std::vector<DUOLMath::Vector3>& vertexInfo, std::vector<UINT32>& indexInfo) const;

		DUOLGraphicsLibrary::IFont* GetFont(const DUOLCommon::tstring& fontName) const;

		DUOLGraphicsLibrary::Texture* GetTexture(const DUOLCommon::tstring& textureName) const;

		DUOLGraphicsEngine::Light* CreateLight(const uint64_t& lightId) const;

		DUOLGraphicsLibrary::IFont* CreateIFont(const DUOLCommon::tstring& fontPath) const;

		DUOLGraphicsLibrary::ICanvas* CreateCanvas(const DUOLGraphicsLibrary::CanvasRenderMode renderMode, const DUOLCommon::tstring& canvasName = _T(""), int width = 0, int height = 0) const;

		bool DeleteCanvas(const DUOLCommon::tstring& canvasname);

		DUOLGameEngine::Material* CreateMaterial(const DUOLCommon::tstring& materialID, const DUOLCommon::tstring& textureID, const DUOLCommon::tstring& normal, const DUOLCommon::tstring& metalroughhnessao, const DUOLCommon::tstring& pipelineState);

		DUOLGameEngine::Material* CreateMaterial(const DUOLCommon::tstring& materialID, DUOLGraphicsEngine::MaterialDesc& materialDesc);

		DUOLGraphicsLibrary::Texture* CreateTexture(const DUOLCommon::tstring& textureID);

		void CreateParticleMaterial(const DUOLCommon::tstring& materialID);

		DUOLGraphicsLibrary::Texture* CreateTexture(const DUOLCommon::tstring& textureID, float width, float height, int size, void* initialData);

		DUOLGameEngine::Sprite* CreateSprite(const DUOLCommon::tstring& spriteID);

		bool DeleteLight(const uint64_t& lightID) const;

		DUOLGraphicsLibrary::Texture* CreateNoiseTexture(const DUOLCommon::tstring& textureID, float frequency, int octaves, float octaveMultiplier, float sizeX, float sizeY);
		/**
		 * \brief 해당하는 경로의 애니메이터 컨트롤러를 로드합니다.
		 * \param path 경로
		 */
		DUOLGameEngine::AnimatorController* LoadAnimatorController(const DUOLCommon::tstring& path);

		DUOLGameEngine::ObjectBase* GetResourceByUUID(const DUOLCommon::UUID uuid) const;

		DUOLGameEngine::ObjectBase* GetResourceByName(const DUOLCommon::tstring& name) const;

		DUOLGameEngine::ObjectBase* GetResourceByName(const DUOLCommon::tstring& name, const DUOLCommon::tstring& type) const;

		const std::vector<std::shared_ptr<DUOLGraphicsEngine::RenderingData_Particle>>* LoadRenderingData_Particle(const DUOLCommon::tstring& path);

		std::vector<std::shared_ptr<DUOLGraphicsEngine::RenderingData_Particle>> CreateRenderingData_Particle(const DUOLCommon::tstring& path);

	public:
		void AddAnimatorController(std::shared_ptr<DUOLGameEngine::AnimatorController> animCon);

	public:
		void Initialize(const EngineSpecification& gameSpec
			, const std::shared_ptr<DUOLGraphicsEngine::GraphicsEngine>& graphicsEngine
			, const std::shared_ptr<DUOLPhysics::PhysicsSystem>& physicsSystem
		);

		void LateInitialize(const EngineSpecification& gameSpec);

		void InitializeMaterial();

		void UnInitialize();

		/**
		 * \brief Garbage Collecting 등의 수행 .. => deltaTime이 상당히 작을 때 (== 프레임 부하가 별로 없을 때)
		 * \param deltaTime 프레임 간의 시간 간격
		 */
		void Update(float deltaTime);
	};
}