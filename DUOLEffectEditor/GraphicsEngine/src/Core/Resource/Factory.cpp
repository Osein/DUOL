#include "Core/Resource/Factory.h"
#include "Core/Resource/Resource/VBIBMesh.h"
#include "util/VertexDesc.h"
#include "util/TypeDefine.h"
#include "Core/DirectX11/DXEngine.h"
#include "Core/Resource/Resource/ParticleMesh.h"
#include "Core/DirectX11/geometrygenerator.h"
#include "Core/Resource/PerlinNoise.hpp"
#include "Core/Resource/ResourceManager.h"

// 2022. 06. 16 그래픽 엔진 구조 변경 중 
// 1. 일단 Mesh의 생성자에서 하던일을 전부 팩토리로 옮길 것. 오버로딩으로 하는게 좋을듯?
// 2. 매쉬에서 분리할 수 있는것들 다른 클래스로 분리 할 것.
// 3. 파서의 파싱 결과에 따라 적절한 컴포넌트를 부착할 수 있도록 구조를 변경할 것.
// 4. MeshComponents의 내용을 채워 넣을 것.
// 5. 리소스 매니저에 팩토리를 사용해서 리소스를 저장할 것.
namespace MuscleGrapics
{
	Factory::Factory()
	{
	}
	ID3D11ShaderResourceView* Factory::LoadTexture(std::string path)
	{
		return nullptr;
	}

	std::pair<ID3D11Buffer*, ID3D11Buffer*> Factory::LoadASE(std::string path)
	{
		return std::pair<ID3D11Buffer*, ID3D11Buffer*>();
	}

	VBIBMesh* Factory::CreateGridMesh()
	{
		ID3D11Device* _d3dDevice = DXEngine::GetInstance()->GetD3dDevice();

		const int _Size = 120;

		std::vector<Vertex::Basic> vertices;

		vertices.resize(_Size);

		for (int i = 0; i < _Size / 4; i++)
			vertices[i].Pos = DirectX::XMFLOAT3(5.0f * i, 0, 0);

		for (int i = 0; i < _Size / 4; i++)
			vertices[i + _Size / 4].Pos = DirectX::XMFLOAT3(5.0f * i, 0, 5.0f * (_Size / 4 - 1));

		for (int i = 0; i < _Size / 4; i++)
			vertices[i + _Size / 2].Pos = DirectX::XMFLOAT3(0, 0, 5.0f * i);

		for (int i = 0; i < _Size / 4; i++)
			vertices[i + _Size / 4 * 3].Pos = DirectX::XMFLOAT3(5.0f * (_Size / 4 - 1), 0, 5.0f * i);

		for (int i = 0; i < _Size; i++)
		{
			vertices[i].Pos.x -= 5.0f * (_Size / 8 - 1);

			vertices[i].Pos.z -= 5.0f * (_Size / 8 - 1);
		}

		UINT indices[_Size];

		for (int i = 0; i < _Size / 2; i++, i++)
		{
			indices[i] = i / 2;

			indices[i + 1] = i / 2 + _Size / 4;
		}

		for (int i = 0; i < _Size / 2; i++, i++)
		{
			indices[i + _Size / 2] = i / 2 + _Size / 2;

			indices[i + _Size / 2 + 1] = i / 2 + _Size / 4 * 3;
		}

		std::vector<index3> _Temp;

		_Temp.resize(_Size / 3);

		for (unsigned int i = 0; i < _Temp.size(); i++)
		{
			_Temp[i].m_Index[0] = indices[i * 3];

			_Temp[i].m_Index[1] = indices[i * 3 + 1];

			_Temp[i].m_Index[2] = indices[i * 3 + 2];
		}

		auto mesh = new VBIBMesh();

		mesh->Init(vertices, _Temp);

		return mesh;
	}

	VBIBMesh* Factory::CreateSphereMesh()
	{
		GeometryGenerator::MeshData sphere;

		GeometryGenerator geoGen;

		geoGen.CreateSphere(0.5f, 30, 30, sphere);

		std::vector<Vertex::BasicLight> vertices(sphere.Vertices.size());

		std::vector<index3> indices(sphere.Indices.size() / 3);

		ChangeGeometry(&sphere, vertices, indices);

		VBIBMesh* _Temp = new VBIBMesh();

		_Temp->Init(vertices, indices);

		return _Temp;
	}

	VBIBMesh* Factory::CreateBoxMesh()
	{
		GeometryGenerator::MeshData Box;

		GeometryGenerator geoGen;

		geoGen.CreateBox(1.0f, 1.0f, 1.0f, Box);

		std::vector<Vertex::BasicLight> vertices(Box.Vertices.size());

		std::vector<index3> indices(Box.Indices.size() / 3);

		ChangeGeometry(&Box, vertices, indices);

		VBIBMesh* _Temp = new VBIBMesh();

		_Temp->Init<Vertex::BasicLight>(vertices, indices);

		return _Temp;
	}

	/**
	 * \brief 텍스처 이미지를 만드는 것이 아닌 화면에 출력하기 위한 버텍스 6개를 정의 하는 것.
	 */
	VBIBMesh* Factory::CreateTextureMesh()
	{
		std::vector<Vertex::Basic> vertices(6);

		std::vector<index3> indices(2);

		indices[0].m_Index[0] = 0;
		indices[0].m_Index[1] = 1;
		indices[0].m_Index[2] = 2;
		indices[1].m_Index[0] = 3;
		indices[1].m_Index[1] = 4;
		indices[1].m_Index[2] = 5;

		VBIBMesh* _Temp = new VBIBMesh();

		_Temp->Init<Vertex::Basic>(vertices, indices, D3D11_USAGE_DYNAMIC, D3D11_USAGE_IMMUTABLE, D3D11_CPU_ACCESS_WRITE);

		return _Temp;
	}

	ID3D11ShaderResourceView* Factory::CreateRandomTexture1DSRV()
	{
		auto device = DXEngine::GetInstance()->GetD3dDevice();

		XMFLOAT4 randomValues[1024];
		int count = 0;
		for (int i = 0; i < 1024; ++i)
		{


			randomValues[i].x = MathHelper::RandF(-1.0f, 1.0f);
			randomValues[i].y = MathHelper::RandF(-1.0f, 1.0f);
			randomValues[i].z = MathHelper::RandF(-1.0f, 1.0f);
			randomValues[i].w = MathHelper::RandF(-1.0f, 1.0f);

			if (0.9f <= randomValues[i].x && randomValues[i].x <= 1.0f)
				count++;
		}

		int a = 0;




		D3D11_SUBRESOURCE_DATA initData;

		initData.pSysMem = randomValues;
		initData.SysMemPitch = 1024 * sizeof(XMFLOAT4);
		initData.SysMemSlicePitch = 0;

		// Create the texture.
		D3D11_TEXTURE1D_DESC texDesc;

		texDesc.Width = 1024;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.ArraySize = 1;

		ID3D11Texture1D* randomTex = 0;

		HR(device->CreateTexture1D(&texDesc, &initData, &randomTex));

		// Create the resource view.
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

		viewDesc.Format = texDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
		viewDesc.Texture1D.MostDetailedMip = 0;

		ID3D11ShaderResourceView* randomTexSRV;

		HR(device->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV));

		ReleaseCOM(randomTex);

		return randomTexSRV;
	}

	ID3D11ShaderResourceView* Factory::CreatePerlinNoiseTexture(float frequency/*주파수*/, int octaves/*레이어 수*/, float octaveMutiplier, std::uint32_t seed/*randSeed*/, float width, float height)
	{
		auto device = DXEngine::GetInstance()->GetD3dDevice();

		const siv::PerlinNoise perlin0{ seed };
		const siv::PerlinNoise perlin1{ seed + 1 };
		const siv::PerlinNoise perlin2{ seed + 2 };
		const double fx = (frequency / width);
		const double fy = (frequency / height);

		std::vector<XMFLOAT4> colors(width * height);

		for (std::int32_t y = 0; y < height; ++y)
		{
			for (std::int32_t x = 0; x < width; ++x)
			{
				int index = width * y + x;

				colors[index].x = perlin0.octave2D_01((x * fx), (y * fy), octaves, octaveMutiplier);
				colors[index].y = perlin1.octave2D_01((x * fx), (y * fy), octaves, octaveMutiplier);
				colors[index].z = perlin2.octave2D_01((x * fx), (y * fy), octaves, octaveMutiplier);
				colors[index].w = 1.0f;
			}
		}



		D3D11_SUBRESOURCE_DATA initData;

		initData.pSysMem = colors.data();
		initData.SysMemPitch = width * sizeof(XMFLOAT4);
		initData.SysMemSlicePitch = 0;

		// Create the texture.
		D3D11_TEXTURE2D_DESC texDesc;

		ZeroMemory(&texDesc, sizeof(texDesc));

		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Usage = D3D11_USAGE_DEFAULT;

		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		texDesc.SampleDesc.Count = 1;

		ID3D11Texture2D* Tex = 0;

		HR(device->CreateTexture2D(&texDesc, &initData, &Tex));

		// Create the resource view.
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

		viewDesc.Format = texDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MostDetailedMip = 0;
		viewDesc.Texture2D.MipLevels = texDesc.MipLevels;

		ID3D11ShaderResourceView* TexSRV;

		HR(device->CreateShaderResourceView(Tex, &viewDesc, &TexSRV));

		ReleaseCOM(Tex);

		return TexSRV;
	}

	ParticleMesh* Factory::CreateParticleMesh()
	{
		return new ParticleMesh();
	}

	void Factory::CompileVertexShader(PipeLineDesc& pipeLineDesc, const WCHAR* fileName, const CHAR* entryName,
		std::vector<D3D_SHADER_MACRO> macro)
	{
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* vertexShaderBuffer = nullptr;
		ID3D11VertexShader* vertexShader = nullptr;

		auto device = DXEngine::GetInstance()->GetD3dDevice();

		ID3D11InputLayout* inputLayOut = nullptr;


		if (FAILED(::D3DCompileFromFile(fileName, macro.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryName, "vs_5_0", compileFlag, 0, &vertexShaderBuffer, &errorMessage)))
		{
			if (errorMessage)
				OutputShaderErrorMessage(errorMessage, nullptr, fileName);
			else
				::MessageBoxA(nullptr, "VS Shader Compile Failed ! PassBase..", nullptr, MB_OK);
		}

		if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader)))
		{
			if (errorMessage)
				OutputShaderErrorMessage(errorMessage, nullptr, fileName);
			else
				::MessageBoxA(nullptr, "VS Shader Create Failed ! PassBase..", nullptr, MB_OK);
		}


		if (!BuildInputLayout(vertexShaderBuffer, &inputLayOut))
			assert(false);

		vertexShaderBuffer->Release();

		if (errorMessage)
			errorMessage->Release();

		pipeLineDesc._vs = vertexShader;
		pipeLineDesc._il = inputLayOut;
	}

	void Factory::CompilePixelShader(PipeLineDesc& pipeLineDesc, const WCHAR* fileName, const CHAR* entryName,
		std::vector<D3D_SHADER_MACRO> macro)
	{
		ID3DBlob* pixelShaderBuffer = nullptr;
		ID3DBlob* errorMessage = nullptr;

		auto device = DXEngine::GetInstance()->GetD3dDevice();

		ID3D11PixelShader* pixelShader;

		if (FAILED(::D3DCompileFromFile(fileName, macro.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE
			, entryName, "ps_5_0", compileFlag, 0, &pixelShaderBuffer, &errorMessage)))
		{
			if (errorMessage)
				OutputShaderErrorMessage(errorMessage, nullptr, fileName);
			else
				::MessageBoxA(nullptr, "PS Shader Compile Failed ! Shader..", nullptr, MB_OK);
		}

		if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader)))
		{
			if (errorMessage)
				OutputShaderErrorMessage(errorMessage, nullptr, fileName);
			else
				::MessageBoxA(nullptr, "PS Shader Create Failed ! Shader..", nullptr, MB_OK);
		}


		pixelShaderBuffer->Release();
		if (errorMessage)
			errorMessage->Release();

		pipeLineDesc._ps = pixelShader;
	}

	void Factory::CompileGeometryShader(PipeLineDesc& pipeLineDesc, const WCHAR* fileName, const CHAR* entryName,
		bool useStreamOut, std::vector<D3D_SHADER_MACRO> macro)
	{
		ID3DBlob* geometryShaderBuffer = nullptr;
		ID3DBlob* errorMessage = nullptr;

		auto device = DXEngine::GetInstance()->GetD3dDevice();

		ID3D11GeometryShader* geometryShader = nullptr;

		if (FAILED(::D3DCompileFromFile(fileName, macro.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE
			, entryName, "gs_5_0", compileFlag, 0, &geometryShaderBuffer, &errorMessage)))
		{
			if (errorMessage)
				OutputShaderErrorMessage(errorMessage, nullptr, fileName);
			else
				::MessageBoxA(nullptr, "Geometry Compile Failed !", nullptr, MB_OK);
		}

		if (useStreamOut)
		{


			UINT strides[1] = { sizeof(Vertex::Particle) };


			// ------------------------------------------------------------ TEST -------------------------------------------------------
			// 1. 동적 링킹 배열을 만들어서 클래스 링킹 오브젝트를 만든다.
			ID3D11ClassLinkage* _pPsClassLinkage = nullptr;
			DXEngine::GetInstance()->GetD3dDevice()->CreateClassLinkage(&_pPsClassLinkage);

			std::vector<D3D11_SO_DECLARATION_ENTRY> decl;

			std::vector<std::string> sementicNames;

			BuildGeometryShaderAttribute(geometryShaderBuffer, decl, sementicNames);

			for (int i = 0; i < decl.size(); i++)
			{
				decl[i].SemanticName = sementicNames[i].c_str();
			}

			// 2. 동적 클래스 링킹을 사용할 쉐이더를 만들 때, 클래스 링킹 오브젝트를 쉐이더 생성 함수의 인자로 넘긴다.
			if (FAILED(device->CreateGeometryShaderWithStreamOutput(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), decl.data(),
				decl.size(), strides, 1, D3D11_SO_NO_RASTERIZED_STREAM, _pPsClassLinkage, &geometryShader)))
			{
				if (errorMessage)
					OutputShaderErrorMessage(errorMessage, nullptr, fileName);
				else
					::MessageBoxA(nullptr, "Geometry Create Failed !", nullptr, MB_OK);
			}


			// 3. D3DReflect 함수를 사용하여 ID3D11ShaderReflection 오브젝트를 만들어라.
			ID3D11ShaderReflection* pReflector = nullptr;
			D3DReflect(geometryShaderBuffer->GetBufferPointer(),
				geometryShaderBuffer->GetBufferSize(),
				IID_ID3D11ShaderReflection,
				(void**)&pReflector);

			// 4. 셰이더 내의 인터페이스 인스턴스의 숫자를 얻어라.
			pipeLineDesc._numGsInstance = pReflector->GetNumInterfaceSlots();

			// 5. 셰이더의 인터페이스 인스턴스 수를 저장할 수 있을 만큼 충분한 배열을 만든다.
			pipeLineDesc._gsDynamicLinkageArray = (ID3D11ClassInstance**)malloc(sizeof(ID3D11ClassInstance*) * pipeLineDesc._numGsInstance);

			// 6. 각 인터페이스 인스턴스에 대응하는 배열 내의 인덱스를 확인한다. // 인덱스는 하드코딩으로 하자...! 따로 저장하기 귀찮다..!
			// 인덱스는 hlsl 작성 순서가 아니였다...! offset을 저장해야한다...
			//ID3D11ShaderReflectionVariable* tempClass = pReflector->GetVariableByName("g_shapeInstance");
			//auto slotIndex = tempClass->GetInterfaceSlot(0);

			//tempClass = pReflector->GetVariableByName("g_velocityOverLifeTimeInstance");
			//slotIndex = tempClass->GetInterfaceSlot(0);

			//tempClass = pReflector->GetVariableByName("g_forceOverLifeTimeInstance");
			//slotIndex = tempClass->GetInterfaceSlot(0);

			//tempClass = pReflector->GetVariableByName("g_colorOverLifeTimeInstance");
			//slotIndex = tempClass->GetInterfaceSlot(0);

			//tempClass = pReflector->GetVariableByName("g_sizeOverLifeTimeInstance");
			//slotIndex = tempClass->GetInterfaceSlot(0);

			// Pass 에서 처리 중.

			// 7. 셰이더 내의 인터페이스로부터 상속된 각 클래스 오브젝트를 위한 클래스 인스턴스를 얻는다.


			auto func = [&](LPCSTR str, unsigned int index)
			{
				ID3D11ClassInstance* _classInstanceTemp;

				_pPsClassLinkage->CreateClassInstance(str, 0, 0, 0, 0, &_classInstanceTemp);
				DXEngine::GetInstance()->GetResourceManager()->InsertShaderClassInstance(str, { index,_classInstanceTemp });
			};
			//auto test1 = g_pPsClassLinkage->GetClassInstance("g_TestClass", 0, &g_pTestClass); // 초기화를 해 둔 인스턴스를 가져오는 함수이다. 초기화를 시키고 버퍼를 업데이트를 시켜야 사용할 수 있다. 


			ID3D11ShaderReflectionVariable* tempClass = pReflector->GetVariableByName("g_shapeInstance");

			func("CShape", tempClass->GetInterfaceSlot(0));
			func("CNullShape", tempClass->GetInterfaceSlot(0));

			tempClass = pReflector->GetVariableByName("g_velocityOverLifeTimeInstance");

			func("CVelocityOverLifeTime", tempClass->GetInterfaceSlot(0));
			func("CNullVelocityOverLifeTime", tempClass->GetInterfaceSlot(0));

			tempClass = pReflector->GetVariableByName("g_forceOverLifeTimeInstance");

			func("CForceOverLifeTime", tempClass->GetInterfaceSlot(0));
			func("CNullForceOverLifeTime", tempClass->GetInterfaceSlot(0));

			tempClass = pReflector->GetVariableByName("g_colorOverLifeTimeInstance");

			func("CColorOverLifeTime", tempClass->GetInterfaceSlot(0));
			func("CNullColorOverLifeTime", tempClass->GetInterfaceSlot(0));

			tempClass = pReflector->GetVariableByName("g_sizeOverLifeTimeInstance");

			func("CSizeOverLifeTime", tempClass->GetInterfaceSlot(0));
			func("CNullSizeOverLifeTime", tempClass->GetInterfaceSlot(0));

			tempClass = pReflector->GetVariableByName("g_roationOverLifeTimeInstance");

			func("CRoationOverLifeTime", tempClass->GetInterfaceSlot(0));
			func("CNullRoationOverLifeTime", tempClass->GetInterfaceSlot(0));

			tempClass = pReflector->GetVariableByName("g_noiseInstance");

			func("CNoise", tempClass->GetInterfaceSlot(0));
			func("CNullNoise", tempClass->GetInterfaceSlot(0));

			tempClass = pReflector->GetVariableByName("g_collisionInstance");

			func("CCollision", tempClass->GetInterfaceSlot(0));
			func("CNullCollision", tempClass->GetInterfaceSlot(0));

			tempClass = pReflector->GetVariableByName("g_textureSheetAnimationInstance");

			func("CTextureSheetAnimation", tempClass->GetInterfaceSlot(0));
			func("CNullTextureSheetAnimation", tempClass->GetInterfaceSlot(0));

			tempClass = pReflector->GetVariableByName("g_trails");

			func("CTrails", tempClass->GetInterfaceSlot(0));
			func("CNullTrails", tempClass->GetInterfaceSlot(0));

			// 8. 인터페이스 인스턴스를 클래스 인스턴스로 설정하려면 동적 연결 배열에서 해당 항목을 설정합니다.
			// pipeLineDesc._gsDynamicLinkageArray[testOffset] = _classInstanceTemp;
			// Pass 에서 처리 중.

			// 9. 동적 역결 배열을 SetShader 호출에 매개 변수로 전달 합니다.
			//DXEngine::GetInstance()->Getd3dImmediateContext()->GSSetShader(geometryShaderTemp, _dynmicLinkageArray, numGsInstance);
			// Pass 에서 처리 중.
			// ------------------------------------------------------------ TEST -------------------------------------------------------
			ReleaseCOM(pReflector);
			ReleaseCOM(_pPsClassLinkage);
		}
		else
		{
			if (FAILED(device->CreateGeometryShader(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), NULL, &geometryShader)))
			{
				if (errorMessage)
					OutputShaderErrorMessage(errorMessage, nullptr, fileName);
				else
					::MessageBoxA(nullptr, "Geometry Create Failed !", nullptr, MB_OK);
			}
		}

		pipeLineDesc._gs = geometryShader;

	}

	void Factory::ChangeGeometry(GeometryGenerator::MeshData* _MeshData, std::vector<Vertex::BasicLight>& _vertices,
		std::vector<index3>& _Indices)
	{
		for (int i = 0; i < _MeshData->Vertices.size(); i++)
		{
			_vertices[i].Pos = _MeshData->Vertices[i].Position;
			_vertices[i].Nomal = _MeshData->Vertices[i].Normal;
			_vertices[i].Tangent = _MeshData->Vertices[i].TangentU;
			_vertices[i].Texture = _MeshData->Vertices[i].TexC;
		}

		for (int i = 0; i < _Indices.size(); i++)
		{
			_Indices[i].m_Index[0] = _MeshData->Indices[i * 3];
			_Indices[i].m_Index[1] = _MeshData->Indices[i * 3 + 1];
			_Indices[i].m_Index[2] = _MeshData->Indices[i * 3 + 2];
		}

	}

	void Factory::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd, const WCHAR* shaderFileName)
	{
		char* compileErrors;
		unsigned long long bufferSize, i;
		std::ofstream fout;

		// Get a pointer to the error message text buffer.
		compileErrors = (char*)(errorMessage->GetBufferPointer());

		// Get the length of the message.
		bufferSize = errorMessage->GetBufferSize();

		// Open a file to write the error message to.
		fout.open("shader-error.txt");

		// Write out the error message.
		for (i = 0; i < bufferSize; i++)
		{
			fout << compileErrors[i];
		}

		// Close the file.
		fout.close();

		// Release the error message.
		errorMessage->Release();

		errorMessage = 0;

		// Pop a message up on the screen to notify the user to check the text file for compile errors.
		MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.",
			shaderFileName, MB_OK);

		return;

	}

	bool Factory::BuildInputLayout(ID3DBlob* shaderBlob,
		ID3D11InputLayout** ppinputLayOut)
	{

		ID3D11ShaderReflection* pReflector = nullptr;

		D3DReflect(shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize(),
			IID_ID3D11ShaderReflection,
			(void**)&pReflector);


		std::vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutDesc;

		D3D11_SHADER_DESC ShaderDesc;
		pReflector->GetDesc(&ShaderDesc);

		int byteOffset = 0;


		for (unsigned InputIndex = 0; InputIndex < ShaderDesc.InputParameters; InputIndex++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			pReflector->GetInputParameterDesc(InputIndex, &paramDesc);

			D3D11_INPUT_ELEMENT_DESC elementDesc;

			elementDesc.SemanticName = paramDesc.SemanticName;
			elementDesc.SemanticIndex = paramDesc.SemanticIndex; //동일한 시멘틱스를 사용하는 매개변수 구분
			elementDesc.InputSlot = 0; //d3d11에 정의된 0~15값 input assembler
			elementDesc.AlignedByteOffset = byteOffset; //편의를 위해 이걸로, 
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			//Mask which indicates which components of a register are used.

			if (paramDesc.Mask == 1)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;

				byteOffset += 4;
			}
			else if (paramDesc.Mask <= 3)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;

				byteOffset += 8;
			}
			else if (paramDesc.Mask <= 7)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

				byteOffset += 12;
			}
			else if (paramDesc.Mask <= 15)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

				byteOffset += 16;
			}

			InputLayoutDesc.push_back(elementDesc);

#ifdef DEBUG
			std::string str = elementDesc.SemanticName;
			str += "\n";
			ATLTRACE(str.c_str());
#endif
		}

		std::vector<D3D11_SIGNATURE_PARAMETER_DESC> temp;

		for (unsigned InputIndex = 0; InputIndex < ShaderDesc.OutputParameters; InputIndex++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			pReflector->GetOutputParameterDesc(InputIndex, &paramDesc);
			temp.emplace_back(paramDesc);
		}



		auto device = DXEngine::GetInstance()->GetD3dDevice();
		//_shaderblob을 통해 유효성 검사를 한다
		HR(device->CreateInputLayout(&InputLayoutDesc.front(), static_cast<UINT>(InputLayoutDesc.size()), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), ppinputLayOut));

		ReleaseCOM(pReflector);

		return true;
	}

	bool Factory::BuildGeometryShaderAttribute(ID3DBlob* shaderBlob,
		std::vector<D3D11_SO_DECLARATION_ENTRY>& outputElements, std::vector<std::string>& sementicNames)
	{
		ID3D11ShaderReflection* pReflector = nullptr;

		D3DReflect(shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize(),
			IID_ID3D11ShaderReflection,
			(void**)&pReflector);

		D3D11_SHADER_DESC shaderDesc;
		pReflector->GetDesc(&shaderDesc);

		outputElements.resize(shaderDesc.OutputParameters);
		sementicNames.resize(shaderDesc.OutputParameters);

		//레지스터의 총사이즈를 어떻게 알 수 있을까?
		int registerInfo[256] = { 0, };

		//이전 bit + 1
		for (unsigned InputIndex = 0; InputIndex < shaderDesc.OutputParameters; InputIndex++)
		{

			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			pReflector->GetOutputParameterDesc(InputIndex, &paramDesc);

			D3D11_SO_DECLARATION_ENTRY& elementDesc = outputElements[InputIndex];

			elementDesc.Stream = 0;

			sementicNames[InputIndex] = paramDesc.SemanticName;

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

#ifdef DEBUG
			std::string str = paramDesc.SemanticName;
			str += "\n";
			ATLTRACE(str.c_str());
#endif


		}
		ReleaseCOM(pReflector);

		return false;

	}
}
