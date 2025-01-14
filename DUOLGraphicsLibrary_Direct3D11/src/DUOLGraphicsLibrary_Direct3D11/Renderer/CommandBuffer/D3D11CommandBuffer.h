#pragma once
#include "DUOLGraphicsLibrary/Renderer/CommandBuffer.h"
#include "DUOLGraphicsLibrary/Renderer/Sampler.h"
#include "DUOLGraphicsLibrary/Renderer/Texture.h"
#include "DUOLGraphicsLibrary/Renderer/RenderTarget.h"
#include "DUOLGraphicsLibrary_Direct3D11/ComPtr.h"
#include "DUOLGraphicsLibrary_Direct3D11/Direct3D11.h"
#include "DUOLGraphicsLibrary_Direct3D11/Renderer/StateManager/D3D11StateManager.h"

namespace DUOLGraphicsLibrary
{
	class D3D11RenderContext;

	class D3D11CommandBuffer : public CommandBuffer
	{

	public:
		D3D11CommandBuffer(
			const UINT64& guid
			, const ComPtr<ID3D11Device>& device
			, const ComPtr<ID3D11DeviceContext>& context
			, const CommandBufferDesc& renderContextDesc);

	private:
		ComPtr<ID3D11Device> _device;

		ComPtr<ID3D11DeviceContext> _d3dContext;

		ComPtr<ID3D11Query> _query;

		bool _queryCheck;

	private:
		void SetBuffer(Buffer* buffer, unsigned slot, long bindFlags, long stageFlags, unsigned initCount);

		void SetTexture(Texture* texture, unsigned slot, long bindFlags, long stageFlags);

		void SetTextureSubResource(Texture* texture, unsigned slot, long bindFlags, long stageFlags, int startMip, int mipSize, int startArray, int arraySize);

		void SetSampler(Sampler* sampler, unsigned slot, long stageFlags);

		D3D11StateManager _stateManager;
		
	public:

		virtual void Begin() override;

		virtual void Flush() override;

		virtual void ClearState() override;

		virtual void End() override;

		/*dest = destination*/
		virtual void UpdateBuffer(Buffer* destBuffer, int destBufferOffset, const void* data, int dataSize) override;

		/*dest = destination*/
		virtual void CopyBuffer(Buffer* dstBuffer, int dstOffset, Buffer* srcBuffer, int srcOffset, int size) override;

		virtual void CopyTexture(Texture* destTexture, const TextureLocation& destOption, Texture* srcTexture, const TextureLocation& srcOption) override;

		/*---- Viewport ----*/

		virtual void SetViewport(const Viewport& viewport) override;

		virtual void SetScissorRect() override;

		/*---- Input Assembler ----*/

		virtual void SetVertexBuffer(Buffer* buffer) override;

		virtual void SetVertexBufferArray(Buffer* bufferArray, int bufferStartPoint, int bufferCount, int stride, int offSet) override;

		virtual void SetIndexBuffer(Buffer* buffer) override;

		/*---- Resource ----*/
		virtual void SetResource(Resource* resource, unsigned int slot, long bindFlags, long stageFlag, unsigned initCount = -1) override;

		virtual void SetResource(const ResourceViewDesc& resourceView) override;

		virtual void SetResources(const ResourceViewLayout& resourceViewLayout) override;

		/*---- Shader ----*/
		virtual void SetShader(Shader* shader) override;

		/*---- Pipeline State  ----*/

		virtual void SetPipelineState(PipelineState* pipelineState) override;

		virtual void SetBlendFactor(const float* color) override;

		/*---- RenderTarget ----*/
		virtual void SetRenderTarget(RenderTarget* renderTarget, RenderTarget* depth, unsigned int slot) override;

		virtual void SetRenderPass(RenderPass* renderPass) override;

		/*---- Draw ----*/

		virtual void Draw(int numVertices, int startVertexLocation) override;

		virtual void DrawIndexed(int numIndices, int startIndexLoaction, int baseVertexLocation) override;

		virtual void DrawIndexedInstanced(int numIndices, int numInstance, int startIndexLocation, int startVertexLocation, int startInstanceLocation) override;

		virtual void DrawInstanced(int numVertices, int startVertexLocation) override;

		virtual void DrawAuto() override;

		/*---- Geometry ----*/
		virtual void BeginStreamOutput(int numBuffers, Buffer* const* buffers) override;

		virtual void EndStreamOutput() override;

		/*---- Compute ----*/
		virtual void Dispatch(int WorkGroupsX, int WorkGroupsY, int WorkGroupsZ) override;

		/*---- Debug ----*/
		/**
		@brief Begin과 End 사이의 데이터를 출력합니다.
		@param  outData - output Data 입니다
		@retval 성공유무에 따라 true와 false를 반환합니다.
		**/
		virtual bool GetData(QueryInfo& outData) override;
	};
}
