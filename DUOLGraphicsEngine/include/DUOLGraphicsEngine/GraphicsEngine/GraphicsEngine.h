#pragma once
#include "DUOLGraphicsEngine/Export.h"
#include "DUOLGraphicsEngine/GraphicsEngineFlags.h"
#include "DUOLGraphicsEngine/ResourceManager/Resource/RenderConstantBuffer.h"

#include "DUOLCommon/StringHelper.h"
#include <memory>

#include "DUOLGraphicsEngine/ResourceManager/Resource/Material.h"


namespace DUOLGraphicsLibrary
{
	class RenderContext;
	class Buffer;
	class Renderer;
	class RenderPass;
}

namespace DUOLGraphicsEngine
{
	class AnimationClip;
	class Model;
	class Material;
	class RenderingPipeline;
	class MeshBase;
	struct RenderObject;

	class ResourceManager;
	class RenderManager;

	/**
		@class   GraphicsEngine
		@brief   그래픽스 엔진
		@details ~
		@author  KyungMin Oh

	**/
	class DUOLGRAPHICSENGINE_EXPORT GraphicsEngine
	{
	public:
		GraphicsEngine(const GraphicsEngineDesc& engineDesc);

		~GraphicsEngine();

	private:
		DUOLGraphicsLibrary::Renderer* _renderer;

		DUOLGraphicsLibrary::RenderContext* _context;

		std::unique_ptr<ResourceManager> _resourceManager;

		std::unique_ptr<RenderManager> _renderManager;

		//for IMGUI
		std::unique_ptr<DUOLGraphicsLibrary::RenderPass> _backbufferRenderPass;

		//SkyBox... 어디론가 없애버려야한다
		DUOLGraphicsLibrary::Buffer* _skyboxVertex;

		DUOLGraphicsLibrary::Buffer* _skyboxIndex;

		DUOLGraphicsLibrary::Texture* _skyboxTexture;

		DUOLGraphicsLibrary::Texture* _skyboxPreFilteredTexture;

		DUOLGraphicsLibrary::Texture* _skyboxIrradianceTexture;

		DUOLGraphicsLibrary::Texture* _skyboxBRDFLookUpTexture;

	private:
		void LoadRenderingPipelineTables(const DUOLMath::Vector2& screenSize);

		void ReadFromStaticMesh(MeshBase* const mesh, std::vector<DUOLMath::Vector3>& vertexInfo, std::vector<UINT32>& indexInfo);

		void ReadFromSkinnedMesh(MeshBase* const mesh, std::vector<DUOLMath::Vector3>& vertexInfo, std::vector<UINT32>& indexInfo);

		void Initialize();

		//SkyBox
		void CreateSkyBox();

		DUOLGraphicsLibrary::Texture* CreateCubeMapFromPanoramaImage(DUOLGraphicsLibrary::Texture* panorama);

		DUOLGraphicsLibrary::Texture* BakeIBLIrradianceMap(DUOLGraphicsLibrary::Texture* cubeMap);

		DUOLGraphicsLibrary::Texture* BakeIBLPreFilteredMap(DUOLGraphicsLibrary::Texture* cubeMap, int mipSize, float width, float height);

		DUOLGraphicsLibrary::Texture* BakeBRDFLookUpTable(float width, float height);

	public:
		DUOLGraphicsEngine::ModuleInfo GetModuleInfo();

		void RenderObject(const DUOLGraphicsEngine::RenderObject* object);

		void RenderDebugObject(const DUOLGraphicsEngine::RenderObject* object);

		void Execute(const ConstantBufferPerFrame& perFrameInfo);

		void PrePresent();

		void Present();

		void OnReszie(const DUOLMath::Vector2& resolution);

		//Resource
		void UpdateMesh(MeshBase* mesh, void* vertices, UINT vertexSize, void* indices, UINT indexSize);

		//생성
		Model* CreateModelFromFBX(const DUOLCommon::tstring& objectID, const DUOLCommon::tstring& path);

		MeshBase* CreateMesh(const DUOLCommon::tstring& objectID, void* vertices, UINT vertexSize, UINT vertexStructureSize, void* indices, UINT indexSize);

		Material* CreateMaterial(const DUOLCommon::tstring& objectID, DUOLGraphicsEngine::MaterialDesc& material);
		
		MeshBase* CreateParticle(const DUOLCommon::tstring&, int maxParticle, int emitterSize);

		//Read & Write
		Model* LoadModel(const DUOLCommon::tstring& objectID);

		bool ReadMeshInfo(const DUOLCommon::tstring& objectID, std::vector<DUOLMath::Vector3>& vertexInfo, std::vector<UINT32>& indexInfo);

		bool ReadMeshInfo(MeshBase* const mesh, std::vector<DUOLMath::Vector3>& vertexInfo, std::vector<UINT32>& indexInfo);

		MeshBase* LoadMesh(const DUOLCommon::tstring& objectID);

		Material* LoadMaterial(const DUOLCommon::tstring& objectID);

		AnimationClip* LoadAnimationClip(const DUOLCommon::tstring& objectID);

		void LoadMeshTable(const DUOLCommon::tstring& path);
	};
}
