#pragma once
#include "DUOLGraphicsLibrary/Renderer/Resource.h"
#include "DUOLGraphicsLibrary/TextureFlags.h"
#include <string>

namespace DUOLGraphicsLibrary
{
 /**

     @class   Texture
     @brief   파이프라인에 바인딩될 리소스 중 텍스쳐
     @details ~
     @author  KyungMin Oh

 **/
	class DUOLGRAPHICSLIBRARY_EXPORT Texture : public Resource
	{
		DUOLGRAPHICSLIBRARY_DECLARE_ENTITY(EntityID::Texture);

	protected:
		Texture(const UINT64& guid, const TextureDesc& _textureDesc):
			Resource(guid)
			, _textureDesc(_textureDesc)
		{

		}

		virtual ~Texture();

	protected:
		TextureDesc _textureDesc;

	public:
		virtual ResourceType GetResourceType() override { return ResourceType::TEXTURE; };

		const TextureDesc& GetTextureDesc() { return _textureDesc; }

		virtual void* GetShaderResourceAddress() abstract;

		const std::string GetTextureName() { return _textureDesc._texturePath; }

		void SetTextureDesc(std::string path);
	};

}
