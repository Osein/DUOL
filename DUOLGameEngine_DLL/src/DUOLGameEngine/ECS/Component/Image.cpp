#include "DUOLGameEngine/ECS/Component/Image.h"
#include <rttr/registration>

#include "DUOLGameEngine/ECS/GameObject.h"
#include "DUOLGameEngine/ECS/Component/RectTransform.h"
#include "DUOLGameEngine/Manager/GraphicsManager.h"
#include "DUOLCommon/MetaDataType.h"
#include "DUOLGameEngine/ECS/Component/Camera.h"
#include "DUOLGameEngine/Manager/ResourceManager.h"
#include "DUOLGameEngine/Manager/UIManager.h"
#include "DUOLGraphicsLibrary/Renderer/Texture.h"
#include "DUOLGameEngine/ECS/Component/Canvas.h"
#include "DUOLGameEngine/ECS/Component/Camera.h"
#include "DUOLGameEngine/ECS/Object/Sprite.h"
#include "DUOLMath/DUOLMath.h"

using namespace rttr;

RTTR_PLUGIN_REGISTRATION
{
	rttr::registration::class_<DUOLGameEngine::Image>("Image")
	.constructor()
	(
		rttr::policy::ctor::as_raw_ptr
	)
	.constructor<DUOLGameEngine::GameObject*, const DUOLCommon::tstring&>()
	(
		rttr::policy::ctor::as_raw_ptr
	)
	.property("Source Image", &DUOLGameEngine::Image::GetSpritePathName,&DUOLGameEngine::Image::SetSpritePathName)
	(
		metadata(DUOLCommon::MetaDataType::Serializable, true)
		, metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::UIFileName)
	)
	.property("Raycast Target", &DUOLGameEngine::Image::GetRaycastTarget,&DUOLGameEngine::Image::SetRaycastTarget)
	(
		metadata(DUOLCommon::MetaDataType::Serializable, true)
		,metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Bool)
	)
	.property("Order In Layer", &DUOLGameEngine::Image::GetLayer,&DUOLGameEngine::Image::SetLayer)
	(
		metadata(DUOLCommon::MetaDataType::Serializable, true)
		,metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Int)
	);
}

namespace DUOLGameEngine
{
	Image::Image() :
		BehaviourBase(nullptr, TEXT("Image"))
		, _canvas(nullptr)
		, _orderInLayer(0)
		, _sprite(new Sprite())
		, _rectTransform(nullptr)
		, _raycastTarget(true)
		, _spriteName(L"None(Sprite)")
	{
		Initialize(nullptr);
	}

	Image::Image(DUOLGameEngine::GameObject* owner, const DUOLCommon::tstring& name) :
		BehaviourBase(owner, name)
		, _canvas(nullptr)
		, _orderInLayer(0)
		, _sprite(new Sprite())
		, _rectTransform(nullptr)
		, _raycastTarget(true)
		, _spriteName(L"None(Sprite)")
	{
		Initialize(owner);
	}

	Image::~Image()
	{
		// 이벤트가 돌아가면 종료도 해줘야 한다.
		DUOLGameEngine::EventManager::GetInstance()->RemoveEventFunction<void>(TEXT("SceneEditModeUpdating"), _updateID);
		DUOLGameEngine::UIManager::GetInstance()->RemoveImage(this);
	}

	void Image::OnUpdate(float deltaTime)
	{
		if (!_rectTransform)
			return;

		if (!_sprite)
			return;

		if (_sprite->GetSprite()->_texture == nullptr)
			LoadTexture(_spriteName);

		if (this->GetGameObject() != nullptr && this->GetGameObject()->GetIsActive() == false)
			return;

		if (_rectTransform->_dirtyFlagRotate)
		{
			_sprite->GetSprite()->_angle = _rectTransform->GetRotation().z;
			_rectTransform->_dirtyFlagRotate = false;
		}

		if (_rectTransform->_dirtyFlagScale)
		{
			_sprite->GetSprite()->_scale = DUOLMath::Vector2(_rectTransform->GetScale().x, _rectTransform->GetScale().y);
			_rectTransform->_dirtyFlagScale = false;
		}

		_sprite->GetSprite()->_rect = _rectTransform->CalculateRect(GraphicsManager::GetInstance()->GetScreenSize());
		_sprite->GetSprite()->_pivot = _rectTransform->GetPivot();

		_canvas->DrawSprite(_sprite->GetSprite(), _orderInLayer);
	}

	void Image::Initialize(DUOLGameEngine::GameObject* owner)
	{
		GameObject* object = DUOLGameEngine::UIManager::GetInstance()->GetCanvas();

		DUOLGameEngine::UIManager::GetInstance()->CreateImage(this);

		_updateID = DUOLGameEngine::EventManager::GetInstance()->AddEventFunction(TEXT("SceneEditModeUpdating"), [this]()
			{
				OnUpdate(1.0f);
			});

		if (object == nullptr)
			return;

		SetCanvas(object->GetComponent<Canvas>()->GetCanvas());

		if (owner == nullptr)
			return;

		_rectTransform = this->GetGameObject()->GetComponent<RectTransform>();

		_canvasRectTransform = object->GetComponent<RectTransform>();
	}

	void Image::SetSpriteName(std::string path)
	{
		auto _path = path;
	}

	void Image::SetRaycastTarget(bool israycast)
	{
		_raycastTarget = israycast;
	}

	void Image::SetLayer(int layer)
	{
		if (layer < 0)
			layer = 0;

		_orderInLayer = layer;
	}

	void Image::OnResize()
	{
		GameObject* object = DUOLGameEngine::UIManager::GetInstance()->GetCanvas();

		if (object == nullptr)
			return;

		SetCanvas(object->GetComponent<Canvas>()->GetCanvas());

		_canvasRectTransform = object->GetComponent<RectTransform>();
	}

	void Image::LoadTexture(const DUOLCommon::tstring& textureID)
	{
		std::string path = "UI/" + DUOLCommon::StringHelper::ToString(textureID);
		auto tstringPath = DUOLCommon::StringHelper::ToTString(path);

		if (_sprite == nullptr)
			ResourceManager::GetInstance()->InsertSprite(tstringPath);

		_sprite->GetSprite()->_texture = ResourceManager::GetInstance()->GetTexture(tstringPath);

		_spriteName = textureID;

		_sprite->SetSpriteName(tstringPath);
	}

	void Image::LoadScene()
	{
		GameObject* object = DUOLGameEngine::UIManager::GetInstance()->GetCanvas();

		if (object == nullptr)
			return;

		SetCanvas(object->GetComponent<Canvas>()->GetCanvas());

		_rectTransform = this->GetGameObject()->GetComponent<RectTransform>();

		_canvasRectTransform = object->GetComponent<RectTransform>();
	}
}
