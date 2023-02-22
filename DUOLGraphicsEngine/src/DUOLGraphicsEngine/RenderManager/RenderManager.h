#pragma once
#include <queue>

#include "DUOLGraphicsEngine/ResourceManager/Resource/RenderConstantBuffer.h"
#include "DUOLGraphicsEngine/ResourceManager/Resource/RenderObject.h"
#include "DUOLGraphicsEngine/Util/ByteBuffer.h"
#include "DUOLGraphicsLibrary/PipelineStateFlags.h"
#include "DUOLGraphicsLibrary/Renderer/Renderer.h"

namespace DUOLGraphicsLibrary
{
	class IFont;
	class Buffer;
	class CommandBuffer;
	class RenderContext;
	class Renderer;
	class ResourceViewLayout;
}

namespace DUOLGraphicsEngine
{
	class OcclusionCulling;
	class RenderingPipeline;

	class RenderManager
	{
	public:
		RenderManager(DUOLGraphicsLibrary::Renderer* renderer, DUOLGraphicsLibrary::RenderContext* context, DUOLGraphicsLibrary::Buffer* PerFrameBuffer, DUOLGraphicsLibrary::Buffer* PerCameraBuffer, DUOLGraphicsLibrary::Buffer* PerObjectBuffer);

	private:
		DUOLGraphicsLibrary::Renderer* _renderer;

		DUOLGraphicsLibrary::RenderContext* _context;

		DUOLGraphicsLibrary::CommandBuffer* _commandBuffer;

		//Todo:: 바깥으로 빼고싶은 목록
		DUOLGraphicsLibrary::Buffer* _streamOutBuffer;

		DUOLGraphicsLibrary::Buffer* _perFrameBuffer;

		DUOLGraphicsLibrary::Buffer* _perCameraBuffer;

		DUOLGraphicsLibrary::Buffer* _perObjectBuffer;

		//particle system
		DUOLGraphicsLibrary::PipelineState* _streamOutShader;

		DUOLGraphicsLibrary::PipelineState* _particleTrailShader;

		DUOLGraphicsLibrary::Texture* _particleRandomTexture;

		DUOLGraphicsLibrary::Buffer* _postProcessingRectVertex;

		DUOLGraphicsLibrary::Buffer* _postProcessingRectIndex;

		DUOLGraphicsLibrary::Buffer* _axisVertex;

		DUOLGraphicsLibrary::Buffer* _axisIndex;

		////렌더큐의 1차 정렬을 위함이다.
		//std::priority_queue<UINT32, std::vector<RenderObject*>, std::less<UINT32>> _renderQueue;

		std::unique_ptr<ByteBuffer> _buffer;
		
		//렌더링 파이프라인 Resources slot
		DUOLGraphicsLibrary::ResourceViewLayout _currentBindTextures;

		DUOLGraphicsLibrary::ResourceViewLayout _perFrameBufferBinder;

		DUOLGraphicsLibrary::ResourceViewLayout _perCameraBufferBinder;

		DUOLGraphicsLibrary::ResourceViewLayout _perObjectBufferBinder;

		int _oitDrawCount;
	public:
		//Todo 여기있으면 안되는 함수들
		void CreateAxis(DUOLGraphicsLibrary::Renderer* renderer);

		void SetStreamOutShader(DUOLGraphicsLibrary::PipelineState* streamOut, DUOLGraphicsLibrary::PipelineState* trailPipeline);
		//
		void CreateStreamOutBuffer(DUOLGraphicsLibrary::Renderer* renderer);

		void ReserveResourceLayout();

		void ExecuteRenderingPipeline(RenderingPipeline* renderPipeline, const std::vector<DecomposedRenderData>& renderObjects, void* postProcessingData = nullptr, int dataSize = 0);

		void OnResize(const DUOLMath::Vector2& resolution);

		void CopyTexture(DUOLGraphicsLibrary::Texture* destTexture, DUOLGraphicsLibrary::Texture* srcTexture);

		void Present();

		void Begin();

		void End();

		bool GetPipelineQueryInfo(DUOLGraphicsLibrary::QueryInfo& info);

		void ExecuteDebugRenderPass(RenderingPipeline* renderPipeline);

		void ExecuteDebugRenderTargetPass(RenderingPipeline* renderPipeline);

		void OcclusionCulling(OcclusionCulling* occlusionCulling, const std::vector<DecomposedRenderData>& inObjects, std::vector<DecomposedRenderData>& outObjects);

		void RenderSkyBox(RenderingPipeline* skyBox, DUOLGraphicsLibrary::Texture* skyboxCubemap, DUOLGraphicsLibrary::Buffer* vertices, DUOLGraphicsLibrary::Buffer* indices, const Camera& cameraInfo);

		void RenderCascadeShadow(DUOLGraphicsLibrary::PipelineState* shadowMesh, DUOLGraphicsLibrary::PipelineState* shadowSkinnedMesh, DUOLGraphicsLibrary::RenderTarget* shadowRenderTarget, const ConstantBufferPerFrame& perFrameInfo, const std::vector<RenderObject*>& renderObjects);

		void SetPerFrameBuffer(const ConstantBufferPerFrame& buffer);

		void BindBackBuffer(DUOLGraphicsLibrary::RenderPass* backbuffer);

		void CreateCubeMapFromPanoramaImage(DUOLGraphicsLibrary::Texture* panorama, DUOLGraphicsLibrary::RenderTarget* cubeMap[6], DUOLGraphicsLibrary::PipelineState* pipelineState, DUOLGraphicsLibrary::Buffer* perObject, DUOLGraphicsLibrary::Sampler* linearSampler);

		void CreatePreFilteredMapFromCubeImage(DUOLGraphicsLibrary::Texture* cubeMap, DUOLGraphicsLibrary::RenderTarget** RadianceMap, DUOLGraphicsLibrary::
		                                       PipelineState* pipelineState, DUOLGraphicsLibrary::RenderTarget* depth, DUOLGraphicsLibrary::Buffer* perObject, DUOLGraphicsLibrary
		                                       ::Sampler* linearSampler, UINT mipmapSize, float width, float height);

		void CreateBRDFLookUpTable(DUOLGraphicsLibrary::RenderTarget* BRDFLookUp, DUOLGraphicsLibrary::PipelineState* pipelineState, DUOLGraphicsLibrary::RenderTarget* depth, DUOLGraphicsLibrary::Buffer* perObject, UINT width, UINT height);

		bool GetRenderData(DUOLGraphicsLibrary::QueryInfo& outData);

		void SetPerCameraBuffer(ConstantBufferPerCamera& perCameraBuffer, const ConstantBufferPerFrame& perFrameBuffer);

	private:
		int GetNumIndicesFromBuffer(DUOLGraphicsLibrary::Buffer* indexBuffer);

		void CreatePostProcessingRect();

		void ExecuteRenderPass(RenderingPipeline* renderPipeline, const std::vector<DecomposedRenderData>& renderObjects);

		void ExecutePostProcessingPass(RenderingPipeline* renderPipeline, void* postProcessingData = nullptr, int dataSize = 0);

		void ExecuteTransparencyPass(RenderingPipeline* renderPipeline); // 0을 입력했을 때만 다르게 처리한다.

		void RenderMesh(DecomposedRenderData& renderObject, RenderingPipeline* renderPipeline);

		void RenderParticle(RenderObject& renderObject, RenderingPipeline* renderPipeline);
	};
}
