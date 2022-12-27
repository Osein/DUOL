#pragma once
#include <unordered_map>
#include "DUOLGraphicsEngine/ResourceManager/Resource/RenderObject.h"
#include "DUOLGraphicsLibrary/PipelineStateFlags.h"
#include "DUOLGraphicsLibrary/Renderer/Renderer.h"

namespace DUOLGraphicsLibrary
{
	class Buffer;
	class CommandBuffer;
	class RenderContext;
	class Renderer;
	class ResourceViewLayout;
}

namespace DUOLGraphicsEngine
{
	class RenderingPipeline;

	class RenderManager
	{
	public:
		RenderManager(DUOLGraphicsLibrary::Renderer* renderer, DUOLGraphicsLibrary::RenderContext* context);

	private:
		DUOLGraphicsLibrary::Renderer* _renderer;

		DUOLGraphicsLibrary::RenderContext* _context;

		DUOLGraphicsLibrary::CommandBuffer* _commandBuffer;

		//Todo:: 바깥으로 빼고싶은 목록
		DUOLGraphicsLibrary::Buffer* _streamOutBuffer;

		//particle system
		DUOLGraphicsLibrary::PipelineState* _streamOutShader;

		DUOLGraphicsLibrary::Texture* _particleRandomTextrue;
		//
		DUOLGraphicsLibrary::Buffer* _postProcessingRectVertex;

		DUOLGraphicsLibrary::Buffer* _postProcessingRectIndex;

		DUOLGraphicsLibrary::Buffer* _axisVertex;

		DUOLGraphicsLibrary::Buffer* _axisIndex;

		std::vector<RenderObject> _renderQueue;

		std::vector<RenderObject> _renderDebugQueue;

		char _buffer[100000];
		//렌더링 파이프라인 Resources slot
		DUOLGraphicsLibrary::ResourceViewLayout _currentBindSamplers;

		DUOLGraphicsLibrary::ResourceViewLayout _currentBindTextures;

		DUOLGraphicsLibrary::ResourceViewLayout _currentBindBuffer;

	public:
		//Todo 여기있으면 안되는 함수들
		void CreateAxis(DUOLGraphicsLibrary::Renderer* renderer);

		void SetStreamOutShader(DUOLGraphicsLibrary::PipelineState* pipelineState);
		//
		void CreateStreamOutBuffer(DUOLGraphicsLibrary::Renderer* renderer);

		void ReserveResourceLayout();

		void ExecuteRenderingPipeline(RenderingPipeline* renderPipeline);

		void OnResize(const DUOLMath::Vector2& resolution);

		void Render(const RenderObject& object);

		void RenderDebug(const RenderObject& object);

		void Present();

		void ExecuteDebugRenderPass(RenderingPipeline* renderPipeline);

		void ExecuteDebugRenderTargetPass(RenderingPipeline* renderPipeline);

		void SetPerFrameBuffer(DUOLGraphicsLibrary::Buffer* frameBuffer, const ConstantBufferPerFrame& buffer);
	private:
		int GetNumIndicesFromBuffer(DUOLGraphicsLibrary::Buffer* indexBuffer);

		void CreatePostProcessingRect();

		void ExecuteRenderPass(RenderingPipeline* renderPipeline);

		void ExecutePostProcessingPass(RenderingPipeline* renderPipeline);

		void RenderMesh(RenderObject& renderObject, RenderingPipeline* renderPipeline);

		void RenderParticle(RenderObject& renderObject, RenderingPipeline* renderPipeline);

	};
}
