#pragma once
#include <unordered_map>
#include "Core/Pass/PassBase.h"

#include "util/TypeDefine.h"
#include "Export/RenderingData.h"

/// <summary>
/// 미리 이펙트와 메쉬를 생성하고 땡겨쓸듯?
/// </summary>
///

namespace MuscleGrapics
{
	class ResourceBase;

	class Factory;

	class TextureRenderPass;

	class ResourceManager
	{
	public:
		ResourceManager();

		~ResourceManager();
	private:
		template<class T>
		TypeUID GetTypeKey();

		std::string D3DMacroToString(std::vector<D3D_SHADER_MACRO>& macro);

		std::string TupleToString(std::tuple<tstring, std::string, std::string>& key);

	public:
		void init();

		std::pair<unsigned int, ID3D11ClassInstance*>& GetShaderClassInstance(std::string key);

		void* InsertTexture(tstring path);

		ID3D11ShaderResourceView* GetTexture(tstring name);

		ID3D11ShaderResourceView* GetNoiseMap(std::tuple<float, int, float> key);

		PassBase<RenderingData_3D>* Get3DShader(tstring name);

		PassBase<RenderingData_Particle>* GetParticleShader(tstring name);

		PassBase<std::vector<std::pair<ID3D11ShaderResourceView*, int>>>* GetTextureRenderShader(tstring name);

		void CreateParticleMesh(std::string name);

		void CompileVertexShader(PipeLineDesc& pipeLineDesc, const WCHAR* fileName, const CHAR* entryName, std::vector<D3D_SHADER_MACRO> macro = std::vector<D3D_SHADER_MACRO>());

		void CompilePixelShader(PipeLineDesc& pipeLineDesc, const WCHAR* fileName, const CHAR* entryName, std::vector<D3D_SHADER_MACRO> macro = std::vector<D3D_SHADER_MACRO>());

		void CompileGeometryShader(PipeLineDesc& pipeLineDesc, const WCHAR* fileName, const CHAR* entryName, bool useStreamOut, std::vector<D3D_SHADER_MACRO> macro = std::vector<D3D_SHADER_MACRO>());

		void CompileComputeShader(PipeLineDesc& pipeLineDesc, const WCHAR* fileName, const CHAR* entryName, std::vector<D3D_SHADER_MACRO> macro = std::vector<D3D_SHADER_MACRO>());

		void InsertShaderClassInstance(std::string key, std::pair<unsigned int, ID3D11ClassInstance*> instance);

	private:
		// Frequency 값과 Octaves, OctaaveMutiplier 값을 키값으로 가진다.
		std::map<std::tuple<float, int, float>, std::shared_ptr<ID3D11ShaderResourceView>> _perlineNoiseMaps; // 어째서 노이즈 맵만 shared_ptr을 사용하는가! refcount가 필요하다!! 하지만 직접 만드는 것은 낭비같으니 이 친구만 sharedptr을 사용하겠다!
		// 왜냐하면! 레퍼런스 카운트를 참조해 해제를 해줄 필요가 있기 때문이다!


		std::unordered_map<tstring, unsigned int> _mesh_VBIB_ID_Maps; // 탐색의 속도가 string 과 int의 차이가 있기 때문에 속도 향상을 위해 매핑을 해서 사용한다.
		std::unordered_map<tstring, ID3D11ShaderResourceView*> _textureMapIDs;
		std::unordered_map<tstring, PassBase<RenderingData_3D>*> _3DShaderIDs; // 쉐이더는 종류가 mesh 만큼 많을 것 같지 않으니 매핑을 할 필요는 없을듯 하다.. 스트링으로 저장하자..
		std::unordered_map<tstring, PassBase<RenderingData_Particle>*> _particleShaderIDs;
		std::unordered_map<tstring, PassBase<std::vector<std::pair<ID3D11ShaderResourceView*, int>>>*> _textureRenderShaderIDs;


		std::unordered_map<std::string, std::pair<unsigned int, ID3D11ClassInstance*>> _shaderClassInstanceStorage; // 모든 셰이더를 통틀어 같은 이름의 클래스가 없다고 가정한다.


		std::unordered_map<TypeUID, std::unordered_map<std::string, ResourceBase*>> _resourceStorage;
	public:
		template<class resourceType>
		void AddResource(const std::string& resourceName, resourceType* resource);

		template<class resourceType>
		resourceType* GetResource(const std::string& resourceName);

		template<class resourceType>
		void SubResource(const std::string& resourceName);
	private:

		Factory* _factory;

		unsigned int _textureId;

		unsigned int _meshId;

	};

	template <class T>
	TypeUID ResourceManager::GetTypeKey()
	{
		static T* TypeKeyObject = nullptr;

		return (TypeUID)(&TypeKeyObject);

	}

	template <class resourceType>
	void ResourceManager::AddResource(const std::string& resourceName, resourceType* resource)
	{
		auto& targetStructure = _resourceStorage[GetTypeKey<resourceType>()];

		auto result = targetStructure.find(resourceName);

		if (result != targetStructure.end())
			assert(false);

		static_assert(std::is_base_of_v<ResourceBase, resourceType>);

		targetStructure.insert({ resourceName, resource });
	}

	template <class resourceType>
	resourceType* ResourceManager::GetResource(const std::string& resourceName)
	{
		auto& targetStructure = _resourceStorage[GetTypeKey<resourceType>()];

		auto result = targetStructure.find(resourceName);

		if (result == targetStructure.end())
			return nullptr;

		return reinterpret_cast<resourceType*>(result->second); //Dynamic Cast를 사용하지 않는 이유
		// 이미 키 값을 통해 필터링을 수행했기 때문.
	}

	template <class resourceType>
	void ResourceManager::SubResource(const std::string& resourceName)
	{
		auto& targetStructure = _resourceStorage[GetTypeKey<resourceType>()];

		auto result = targetStructure.find(resourceName);

		if (result != targetStructure.end())
		{
			delete result->second;
			targetStructure.erase(result);
		}
	}
}
