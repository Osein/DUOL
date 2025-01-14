 #include "D3D11Buffer.h"
#include "DUOLGraphicsLibrary_Direct3D11/Util/DXHelper.h"
#include "DUOLGraphicsLibrary_Direct3D11/Util/DXFlagMap.h"

namespace DUOLGraphicsLibrary
{
	D3D11Buffer::D3D11Buffer(const UINT64& guid, ID3D11Device* device, const BufferDesc& bufferDesc, const void* initialData) :
		Buffer(guid, bufferDesc)
		, _bufferSize(bufferDesc._size)
		, _stride(bufferDesc._stride)
	{
		_format = MapFormat(bufferDesc._format);
		CreateBuffer(device, bufferDesc, initialData);
	}

	void D3D11Buffer::CreateBuffer(ID3D11Device* device, const BufferDesc& bufferDesc, const void* initialData)
	{
		D3D11_BUFFER_DESC d3dBufferDesc;
		{
			d3dBufferDesc.ByteWidth = bufferDesc._size;
			d3dBufferDesc.Usage = MapDXUsageFlag(bufferDesc._usage);
			_usage = d3dBufferDesc.Usage;
			d3dBufferDesc.BindFlags = MapDXBindFlag(bufferDesc._bindFlags);
			d3dBufferDesc.CPUAccessFlags = MapDXCPUAccessFlag(bufferDesc._cpuAccessFlags);
			//if(_usage == D3D11_USAGE_DYNAMIC)
			//	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			//else
			//	d3dBufferDesc.CPUAccessFlags = 0;
			d3dBufferDesc.MiscFlags = MapDXMiscFlag(bufferDesc._miscFlags);
			d3dBufferDesc.StructureByteStride = bufferDesc._stride;
		}

		if (initialData != nullptr)
		{
			D3D11_SUBRESOURCE_DATA subresourceData;
			{
				subresourceData.pSysMem = initialData;
				subresourceData.SysMemPitch = 0;
				subresourceData.SysMemSlicePitch = 0;
			}

			auto hr = device->CreateBuffer(&d3dBufferDesc, &subresourceData, _buffer.ReleaseAndGetAddressOf());
			DXThrowError(hr, "D3D11Buffer CreateBuffer Failed");
		}
		else
		{
			auto hr = device->CreateBuffer(&d3dBufferDesc, nullptr, _buffer.ReleaseAndGetAddressOf());
			DXThrowError(hr, "D3D11Buffer CreateBuffer Failed");
		}
	}

	void D3D11Buffer::UpdateSubresource(ID3D11DeviceContext* context, const void* data, UINT dataSize, UINT offset)
	{
		if (_bufferSize < dataSize + offset)
		{
			DUOLGRAPHICS_ASSERT("D3D11Buffer UpdateSubresource Buffer Size Over")
		}

		if (_usage == D3D11_USAGE_DYNAMIC)
		{
			D3D11_MAPPED_SUBRESOURCE  subResource;
			auto hr = context->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
			DXThrowError(hr, "D3D11Buffer UpdateSubresource Error");

			memcpy(reinterpret_cast<char*>(subResource.pData) + offset, data, dataSize);

			context->Unmap(_buffer.Get(), 0);
		}
		else
		{
			//일단 buffer가 dynamic이 아니면 의심하라.
			if ((_bufferDesc._bindFlags & static_cast<long>(BindFlags::CONSTANTBUFFER)) != 0)
			{
				if (dataSize == _bufferSize)
					context->UpdateSubresource(_buffer.Get(), 0, nullptr, data, 0, 0);
				else
					DUOLGRAPHICS_ASSERT("D3D11Buffer UpdateSubresource Buffer Size Over");
			}
			else
			{
				const D3D11_BOX dstBox{ offset, 0, 0, offset + dataSize, 1, 1 };
				context->UpdateSubresource(_buffer.Get(), 0, &dstBox, data, 0, 0);
			}

		}
	}

	void* D3D11Buffer::Map(ID3D11DeviceContext* context, CPUAccessFlags accessFlag)
	{
		HRESULT hr;

		D3D11_MAPPED_SUBRESOURCE mapppedSubresource;

		hr = context->Map(GetNativeBuffer(), 0, MapDXCPUAccessFlagToMap(static_cast<long>(accessFlag)), 0, &mapppedSubresource);

		return (SUCCEEDED(hr) ? mapppedSubresource.pData : nullptr);
	}

	void D3D11Buffer::Unmap(ID3D11DeviceContext* context)
	{
		context->Unmap(GetNativeBuffer(), 0);
	}
}
