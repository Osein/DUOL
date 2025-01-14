#pragma once
#include "DUOLGraphicsLibrary/Export.h"
#include "DUOLGraphicsLibrary/ResourceFormat.h"
#include "DUOLMath/DUOLMath.h"
#include <algorithm>
#include <cmath>
#include <string>

namespace DUOLGraphicsLibrary
{
	struct DUOLGRAPHICSLIBRARY_EXPORT DUOLMath::Vector2;
	struct DUOLGRAPHICSLIBRARY_EXPORT DUOLMath::Vector3;

	enum class TextureType
	{
		TEXTURE1D,
		TEXTURE2D,
		TEXTURE3D,
		TEXTURECUBE,
		TEXTURE1DARRAY,
		TEXTURE2DARRAY,
		TEXTURECUBEARRAY,
		TEXTURE2DMS,
		TEXTURE2DMSARRAY,
	};

//todo :: tetureDesc tstring으로.. 내부 캐스팅하자
 /**
     @struct TextureDesc
     @brief  
 **/
	struct DUOLGRAPHICSLIBRARY_EXPORT TextureDesc
	{

		TextureDesc() :
			_type(TextureType::TEXTURE2D)
			, _textureExtent(1, 1, 1)
			, _sample(1)
			, _format(ResourceFormat::FORMAT_R8G8B8A8_UNORM)
			, _usage(ResourceUsage::USAGE_DEFAULT)
			, _mipLevels(1)
			, _arraySize(1)
			, _bindFlags(0)
			, _cpuAccessFlags(0)
			, _miscFlags(0)
			, _initData(nullptr)
			, _size(0)
			, _texturePath(nullptr)
		{

		}
  /**
      @brief TextureDesc object constructor
      @param textureType    - 텍스쳐 타입
      @param textureExtend  - 텍스쳐 Size
      @param sample         - 샘플 MSAA
      @param format         - 텍스쳐 포맷
      @param usage          - 리소스 용도
      @param mipLevels      - 밉사이즈
      @param arraySize      - 어레이 사이즈
      @param bindFlags      - bindFlag like SRV, RTV...
      @param cpuAccessFlags - accessFlag
      @param miscFlags      - set generate_mipmaps.......
      @param texturePath    - 리소스가 있을때.
  **/
		TextureDesc(
			TextureType			textureType
			, DUOLMath::Vector3 textureExtend
			, unsigned int		sample
			, ResourceFormat	format
			, ResourceUsage		usage
			, unsigned int		mipLevels
			, unsigned int		arraySize
			, long				bindFlags
			, long				cpuAccessFlags
			, long				miscFlags
			, const char* texturePath = nullptr) :
			_type(textureType)
			, _textureExtent(textureExtend)
			, _sample(sample)
			, _format(format)
			, _usage(usage)
			, _mipLevels(mipLevels)
			, _arraySize(arraySize)
			, _bindFlags(bindFlags)
			, _cpuAccessFlags(cpuAccessFlags)
			, _miscFlags(miscFlags)
			, _initData(nullptr)
			, _size(0)
			, _texturePath(texturePath)
		{

		}

		~TextureDesc() = default;

		TextureDesc(const TextureDesc& textureDesc) = default;

		TextureType _type;

		DUOLMath::Vector3 _textureExtent;

		unsigned int _sample;

		ResourceFormat _format;

		ResourceUsage _usage;

		unsigned int _mipLevels;

		unsigned int _arraySize;

		long _bindFlags;

		long _cpuAccessFlags;

		long _miscFlags;

		void* _initData;

		int _size;

		const char* _texturePath;
	};

	struct DUOLGRAPHICSLIBRARY_EXPORT TextureLocation
	{
		TextureLocation() :
			_offset()
			,_arrayLayer(0)
			,_mipLevel(0)
		{
		}

		DUOLMath::Vector3 _offset;

		UINT32 _arrayLayer;

		UINT32 _mipLevel;
	};

	DUOLGRAPHICSLIBRARY_EXPORT inline bool IsMultiSampleTexture(const TextureType type)
	{
		return (type >= TextureType::TEXTURE2DMS);
	}

	DUOLGRAPHICSLIBRARY_EXPORT inline bool IsMipMappedTexture(const TextureDesc& textureDesc)
	{
		//1만 아니면 밉맵이 필요한 텍스쳐임!
		return (!IsMultiSampleTexture(textureDesc._type) && (textureDesc._mipLevels == 0 || textureDesc._mipLevels > 1));
	}

	DUOLGRAPHICSLIBRARY_EXPORT inline bool IsCubeTexture(const TextureType type)
	{
		return (type == TextureType::TEXTURECUBE || type == TextureType::TEXTURECUBEARRAY);
	}


	DUOLGRAPHICSLIBRARY_EXPORT inline int NumMipLevels(const DUOLMath::Vector3& textureExtend)
	{
		const float maxSize = std::max<float>({ textureExtend.x, textureExtend.y , textureExtend.z });
		const auto log2Size = static_cast<unsigned>(std::log2(maxSize));

		return (1u + log2Size);
  	}

	DUOLGRAPHICSLIBRARY_EXPORT inline int NumMipLevels(const TextureDesc& desc,const bool& mipGenerate)
	{
		if (desc._mipLevels == 0 || mipGenerate)
			return NumMipLevels(desc._textureExtent);
		else
			return desc._mipLevels;
	}
}

