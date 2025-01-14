#include "D3D11Shader.h"
#include "DUOLGraphicsLibrary_Direct3D11/Util/DXHelper.h"
#include <string>
#include <vector>
#include "d3d11shader.h"
#include "d3dcompiler.h"
#include "DUOLGraphicsLibrary_Direct3D11/Util/ReadData.h"
#include "DUOLCommon/StringHelper.h"

namespace DUOLGraphicsLibrary
{
	D3D11Shader::D3D11Shader(const UINT64& guid, ID3D11Device* device, const ShaderDesc& shaderDesc) :
		Shader(guid, shaderDesc._type)
	{
		if (CreateShader(device, shaderDesc))
		{
			DUOLGRAPHICS_ASSERT("Shader Create Error");
		}
	}

	bool D3D11Shader::CreateShader(ID3D11Device* device, const ShaderDesc& shaderDesc)
	{

		if (IsHLSL(shaderDesc._source))
		{
			CompileShader(device, shaderDesc);
			CreateNativeShaderFromBlob(device, shaderDesc);
		}
		else
		{
			CreateNativeShaderFromCompiledFile(device, shaderDesc);
		}

		return false;
	}

	bool D3D11Shader::CreateNativeShaderFromBlob(ID3D11Device* device, const ShaderDesc& shaderDesc)
	{
		if (_shaderBlob == nullptr)
		{
			return false;
		}

		HRESULT hr = S_OK;
		ComPtr<ID3D11ShaderReflection> ShaderReflector = nullptr;
		D3DReflect(_shaderBlob->GetBufferPointer(), _shaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)ShaderReflector.GetAddressOf());
		BuildConstantBufferInfoDesc(device, ShaderReflector);

		switch (shaderDesc._type)
		{
		case ShaderType::UNKNOWN:
		{
			DUOLGRAPHICS_ASSERT("Shader Type Can't Be Unkown");
			break;
		}
		case ShaderType::VERTEX:
		{
			hr = device->CreateVertexShader(
				_shaderBlob->GetBufferPointer()
				, _shaderBlob->GetBufferSize(),
				nullptr,
				_nativeShader._vertexShader.ReleaseAndGetAddressOf());

			if (shaderDesc._useShaderReflection)
			{
				BuildInputLayout(device, ShaderReflector);
			}
			else
			{
				BuildInputLayout(device, shaderDesc);
			}
			break;
		}
		case ShaderType::HULL:
		{
			break;
		}
		case ShaderType::DOMAINS:
		{
			break;
		}
		case ShaderType::GEOMETRY:
		{
			std::vector<D3D11_SO_DECLARATION_ENTRY> streamOutput;
			BuildGeometryShaderAttribute(device, ShaderReflector, streamOutput);

			UINT rasterizerStream = D3D11_SO_NO_RASTERIZED_STREAM;

			if (shaderDesc._useStreamOut)
			{
				rasterizerStream = 0;

				hr = device->CreateGeometryShaderWithStreamOutput(
					_shaderBlob->GetBufferPointer()
					, _shaderBlob->GetBufferSize()
					, streamOutput.data()
					, static_cast<UINT>(streamOutput.size())
					, nullptr
					, 0
					, rasterizerStream
					, nullptr
					, _nativeShader._geometryShader.ReleaseAndGetAddressOf());
			}
			else
			{
				hr = device->CreateGeometryShader(
					_shaderBlob->GetBufferPointer()
					, _shaderBlob->GetBufferSize()
					, nullptr
					, _nativeShader._geometryShader.ReleaseAndGetAddressOf());
			}

			break;
		}
		case ShaderType::PIXEL:
		{
			hr = device->CreatePixelShader(
				_shaderBlob->GetBufferPointer()
				, _shaderBlob->GetBufferSize(),
				nullptr,
				_nativeShader._pixelShader.ReleaseAndGetAddressOf());
			break;
		}
		case ShaderType::COMPUTE:
		{
			hr = device->CreateComputeShader(
				_shaderBlob->GetBufferPointer()
				, _shaderBlob->GetBufferSize(),
				nullptr,
				_nativeShader._computeShader.ReleaseAndGetAddressOf());
			break;
		}
		default:
		{
			break;
		}
		}

		DXThrowError(hr, "D3D11Shader CreateNativeShader Failed");

		return true;
	}

	bool D3D11Shader::CompileShader(ID3D11Device* device, const ShaderDesc& shaderDesc)
	{
		ComPtr<ID3DBlob> errorBlob = nullptr;

		size_t macroSize = shaderDesc._shaderMacro.size();

		const char* entry = shaderDesc._entryPoint;
		const char* profile = shaderDesc._profile;

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
		flags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		flags |= shaderDesc._flags;

		std::string strfilepath(shaderDesc._source);
		std::wstring filePath(strfilepath.begin(), strfilepath.end());

		HRESULT hr;

		if (macroSize > 0)
		{
			D3D_SHADER_MACRO* shaderMacro = new D3D_SHADER_MACRO[macroSize + 1];
			shaderMacro[macroSize].Name = NULL;
			shaderMacro[macroSize].Definition = NULL;

			for (int macroIndex = 0; macroIndex < macroSize; macroIndex++)
			{
				auto& inputMacro = shaderDesc._shaderMacro[macroIndex];
				shaderMacro[macroIndex].Name = inputMacro.Name;
				shaderMacro[macroIndex].Definition = inputMacro.Definition;
			}

			hr = D3DCompileFromFile(
				filePath.c_str()
				, shaderMacro
				, D3D_COMPILE_STANDARD_FILE_INCLUDE
				, entry
				, profile
				, flags
				, 0
				, _shaderBlob.ReleaseAndGetAddressOf()
				, errorBlob.ReleaseAndGetAddressOf());

			delete[] shaderMacro;
			DXThrowError(hr, "D3D11Shader CreateShader Error");
		}
		else
		{
			hr = D3DCompileFromFile(
				filePath.c_str()
				, nullptr
				, D3D_COMPILE_STANDARD_FILE_INCLUDE
				, entry
				, profile
				, flags
				, 0
				, _shaderBlob.ReleaseAndGetAddressOf()
				, errorBlob.ReleaseAndGetAddressOf());

			DXThrowError(hr, "D3D11Shader CreateShader Error");
		}

		if (hr != S_OK)
		{
			if (errorBlob.Get())
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}
		}

		return true;
	}

	bool D3D11Shader::BuildConstantBufferInfoDesc(ID3D11Device* device, ComPtr<ID3D11ShaderReflection> ShaderReflector)
	{
		D3D11_SHADER_DESC shaderDesc;
		ShaderReflector->GetDesc(&shaderDesc);

		_constantBufferInfos.reserve(shaderDesc.ConstantBuffers);

		for (unsigned constantBufferIdx = 0; constantBufferIdx < shaderDesc.ConstantBuffers; ++constantBufferIdx)
		{
			ConstantBufferInfoDesc bufferinfodesc;
			auto d3dShaderBufferInfo = ShaderReflector->GetConstantBufferByIndex(constantBufferIdx);

			D3D11_SHADER_BUFFER_DESC d3dBufferDesc;

			d3dShaderBufferInfo->GetDesc(&d3dBufferDesc);

			bufferinfodesc._name = d3dBufferDesc.Name;
			bufferinfodesc._size = d3dBufferDesc.Size;
			//bufferinfodesc._variables = d3dBufferDesc.Variables;

			bufferinfodesc._variables.reserve(d3dBufferDesc.Variables);

			for (unsigned variableIndex = 0; variableIndex < d3dBufferDesc.Variables; ++variableIndex)
			{
				auto d3dVariableInfo = d3dShaderBufferInfo->GetVariableByIndex(variableIndex);

				D3D11_SHADER_VARIABLE_DESC d3dVariableDesc;

				d3dVariableInfo->GetDesc(&d3dVariableDesc);

				ConstantBufferInfoDesc::ConstantBufferVariable variable;

				variable._name = d3dVariableDesc.Name;
				variable._size = d3dVariableDesc.Size;
				variable._startOffset = d3dVariableDesc.StartOffset;

				bufferinfodesc._variables.emplace_back(variable);
			}

			_constantBufferInfos.emplace_back(bufferinfodesc);
		}

		return true;
	}

	bool D3D11Shader::BuildInputLayout(ID3D11Device* device, ComPtr<ID3D11ShaderReflection> ShaderReflector)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutDesc;

		D3D11_SHADER_DESC ShaderDesc;
		ShaderReflector->GetDesc(&ShaderDesc);

		for (unsigned InputIndex = 0; InputIndex < ShaderDesc.InputParameters; InputIndex++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			ShaderReflector->GetInputParameterDesc(InputIndex, &paramDesc);

			D3D11_INPUT_ELEMENT_DESC elementDesc;

			elementDesc.SemanticName = paramDesc.SemanticName;
			elementDesc.SemanticIndex = paramDesc.SemanticIndex; //동일한 시멘틱스를 사용하는 매개변수 구분
			elementDesc.InputSlot = 0; //d3d11에 정의된 0~15값 input assembler
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; //편의를 위해 이걸로, 
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			//Mask which indicates which components of a register are used.

			if (paramDesc.Mask == 1)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (paramDesc.Mask <= 3)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (paramDesc.Mask <= 7)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (paramDesc.Mask <= 15)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			InputLayoutDesc.push_back(elementDesc);
		}

		//std::vector<D3D11_SIGNATURE_PARAMETER_DESC> temp;

		//for (unsigned InputIndex = 0; InputIndex < ShaderDesc.OutputParameters; InputIndex++)
		//{
		//	D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		//	ShaderReflector->GetOutputParameterDesc(InputIndex, &paramDesc);
		//	temp.emplace_back(paramDesc);
		//}

		//_shaderblob을 통해 유효성 검사를 한다
		auto hr = device->CreateInputLayout(&InputLayoutDesc.front(), static_cast<UINT>(InputLayoutDesc.size()), _shaderBlob->GetBufferPointer(), _shaderBlob->GetBufferSize(), _inputLayout.GetAddressOf());

		DXThrowError(hr, "D3D11Shader CreateInputLayout Error");

		return true;
	}

	bool D3D11Shader::BuildInputLayout(ID3D11Device* device, const ShaderDesc& shaderDesc)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutDesc;

		for (unsigned InputIndex = 0; InputIndex < shaderDesc._inputLayout.size(); InputIndex++)
		{
			D3D11_INPUT_ELEMENT_DESC elementDesc;

			elementDesc.SemanticName = shaderDesc._inputLayout[InputIndex]._semanticName.c_str();
			elementDesc.SemanticIndex = shaderDesc._inputLayout[InputIndex]._semanticIndex; //동일한 시멘틱스를 사용하는 매개변수 구분
			elementDesc.InputSlot = shaderDesc._inputLayout[InputIndex]._inputSlot; //d3d11에 정의된 0~15값 input assembler
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; //편의를 위해 이걸로, 
			elementDesc.InputSlotClass = (shaderDesc._inputLayout[InputIndex]._inputSlotType == INPUT_CLASSIFICATION::VertexData)? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
			elementDesc.InstanceDataStepRate = shaderDesc._inputLayout[InputIndex]._instanceStepRate;

			InputLayoutDesc.push_back(elementDesc);
		}

		//_shaderblob을 통해 유효성 검사를 한다
		auto hr = device->CreateInputLayout(&InputLayoutDesc.front(), static_cast<UINT>(InputLayoutDesc.size()), _shaderBlob->GetBufferPointer(), _shaderBlob->GetBufferSize(), _inputLayout.GetAddressOf());

		DXThrowError(hr, "D3D11Shader CreateInputLayout Error");

		return true;
	}

	bool D3D11Shader::BuildGeometryShaderAttribute(ID3D11Device* device, ComPtr<ID3D11ShaderReflection> ShaderReflector, std::vector<D3D11_SO_DECLARATION_ENTRY>& outputElements)
	{
		D3D11_SHADER_DESC shaderDesc;
		ShaderReflector->GetDesc(&shaderDesc);

		outputElements.resize(shaderDesc.OutputParameters);

		//레지스터의 총사이즈를 어떻게 알 수 있을까?
		int registerInfo[256] = { 0, };

		//이전 bit + 1
		for (unsigned InputIndex = 0; InputIndex < shaderDesc.OutputParameters; InputIndex++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			ShaderReflector->GetOutputParameterDesc(InputIndex, &paramDesc);

			D3D11_SO_DECLARATION_ENTRY& elementDesc = outputElements[InputIndex];

			elementDesc.Stream = 0;
			elementDesc.SemanticName = paramDesc.SemanticName;
			elementDesc.SemanticIndex = paramDesc.SemanticIndex;

			elementDesc.StartComponent = 0;

			if (paramDesc.Mask & (1 << 0))
			{
				elementDesc.ComponentCount++;
			}
			if (paramDesc.Mask & (1 << 1))
			{
				elementDesc.ComponentCount++;
			}
			if (paramDesc.Mask & (1 << 2))
			{
				elementDesc.ComponentCount++;
			}
			if (paramDesc.Mask & (1 << 3))
			{
				elementDesc.ComponentCount++;
			}

			registerInfo[paramDesc.Register] += elementDesc.ComponentCount;

			elementDesc.OutputSlot = 0;
		}

		return false;
	}

	bool D3D11Shader::CreateNativeShaderFromCompiledFile(ID3D11Device* device, const ShaderDesc& shaderDesc)
	{

		HRESULT hr = S_OK;

		std::wstring str = DUOLCommon::StringHelper::ToWString(shaderDesc._source);

		switch (shaderDesc._type)
		{
		case ShaderType::UNKNOWN:
		{
			DUOLGRAPHICS_ASSERT("Shader Type Can't Be Unkown");
			DXThrowError(S_FALSE, "D3D11Shader CreateNativeShader Failed. \n Type is UNKOWN");
			break;
		}
		case ShaderType::VERTEX:
		{
			DUOLGRAPHICS_ASSERT("VertexShader Must be a hlsl file for Shader Reflection ");
			DXThrowError(S_FALSE, "D3D11Shader CreateNativeShader Failed \n VertexShader Must be a hlsl file for Shader Reflection ");
			break;
		}
		case ShaderType::HULL:
		{
			break;
		}
		case ShaderType::DOMAINS:
		{
			break;
		}
		case ShaderType::GEOMETRY:
		{

			if (shaderDesc._useStreamOut)
			{
				DUOLGRAPHICS_ASSERT("Geometry Shader with StreamOut Must be a hlsl file for Shader Reflection ");
				DXThrowError(S_FALSE, "D3D11Shader CreateNativeShader Failed \n Geometry Shader with StreamOut Must be a hlsl file for Shader Reflection");
				break;
			}
			else
			{
				auto ShaderByteCode = DX::ReadData(str.data());

				hr = device->CreateGeometryShader(
					ShaderByteCode.data()
					, ShaderByteCode.size()
					, nullptr
					, _nativeShader._geometryShader.ReleaseAndGetAddressOf());
			}

			break;
		}
		case ShaderType::PIXEL:
		{
			auto ShaderByteCode = DX::ReadData(str.data());

			hr = device->CreatePixelShader(
				ShaderByteCode.data()
				, ShaderByteCode.size()
				, nullptr
				, _nativeShader._pixelShader.ReleaseAndGetAddressOf());
			break;
		}
		case ShaderType::COMPUTE:
		{
			auto ShaderByteCode = DX::ReadData(str.data());

			hr = device->CreateComputeShader(
				ShaderByteCode.data()
				, ShaderByteCode.size()
				, nullptr
				, _nativeShader._computeShader.ReleaseAndGetAddressOf());
			break;
		}
		default:
		{
			break;
		}
		}

		DXThrowError(hr, "D3D11Shader CreateNativeShader Failed");
		return true;
	}

	bool D3D11Shader::IsHLSL(const char* path)
	{
		std::string pathstr(path);

		if (pathstr.length() > 1)
		{
			auto extensionStartPoint = pathstr.find_last_of('.') + 1;

			std::string fileFormat = pathstr.substr(extensionStartPoint);

			if (fileFormat == "hlsl" || fileFormat == "HLSL")
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		return false;
	}
}
