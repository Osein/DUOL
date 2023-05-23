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
#include "DUOLGameEngine/ECS/Component/Button.h"
#include "DUOLGameEngine/ECS/Component/Scrollbar.h"
#include "DUOLGameEngine/Manager/InputManager.h"
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
	.property("Color", &DUOLGameEngine::Image::GetColor,  &DUOLGameEngine::Image::SetColor)
	(
		metadata(DUOLCommon::MetaDataType::Serializable, true)
		,metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Color4)
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
		if (_isEnabled == false)
			return;

		if (!_rectTransform)
			_rectTransform = this->GetGameObject()->GetComponent<RectTransform>();

		if (!_sprite)
			return;

		if (!_canvas)
		{
			DUOL_TRACE(DUOL_CONSOLE, "Canvas Null.");

			auto object = this->GetGameObject()->GetTransform()->GetParent();
			while (object->GetParent() != nullptr)
			{
				object = object->GetParent();
			}
			SetCanvas(object->GetGameObject()->GetComponent<Canvas>()->GetCanvas());
		}

		if (_sprite->GetSprite()->_texture == nullptr)
			LoadTexture(_spriteName);

		if (this->GetGameObject() != nullptr && this->GetGameObject()->GetIsActive() == false)
			return;

		ImageRender();

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

	DUOLMath::Vector4 Image::GetColor()
	{
		auto sprite = _sprite->GetSprite();

		if (sprite != nullptr)
		{
			return sprite->_color;
		}

		return DUOLMath::Vector4::Zero;
	}

	void Image::SetColor(DUOLMath::Vector4 vector)
	{
		auto sprite = _sprite->GetSprite();

		if(sprite != nullptr)
		{
			sprite->_color = vector;
		}
	}

	void Image::SetCanvas(DUOLGraphicsLibrary::ICanvas* canvas)
	{
		_canvas = canvas;
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

	// 텍스쳐가 끊긴다. 연결해줘야함.
	void Image::OnResize()
	{
		GameObject* object = DUOLGameEngine::UIManager::GetInstance()->GetCanvas();

		if (object == nullptr)
			return;

		auto canvas = object->GetComponent<Canvas>();

		SetCanvas(object->GetComponent<Canvas>()->GetCanvas());

		_canvasRectTransform = object->GetComponent<RectTransform>();

		auto transform = this->GetGameObject()->GetComponent<RectTransform>();

		float x = transform->GetPosX() * canvas->GetScreenRatio().x;
		float y = transform->GetPosY() * canvas->GetScreenRatio().y;

		float width = transform->GetWidth() * canvas->GetScreenRatio().x;
		float height = transform->GetHeight() * canvas->GetScreenRatio().y;

		// 이것도 곱해줘야하나?
		_rectTransform->SetRectX(x);
		_rectTransform->SetRectY(y);

		// 이건 맞다..?
		_rectTransform->SetRectZ(width);
		_rectTransform->SetRectW(height);

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
		// 여기서 게임 오브젝트가 바뀐다.
		GameObject* object = DUOLGameEngine::UIManager::GetInstance()->GetCanvas();

		if (object == nullptr)
			return;

		SetCanvas(object->GetComponent<Canvas>()->GetCanvas());

		_rectTransform = this->GetGameObject()->GetComponent<RectTransform>();

		_canvasRectTransform = object->GetComponent<RectTransform>();
	}

	void Image::ImageRender()
	{
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

		if (_sprite->GetSprite()->_scale.x == 0 || _sprite->GetSprite()->_scale.y == 0)
			DUOL_TRACE(DUOL_CONSOLE, "scale Null.");

		if (_sprite->GetSprite() == nullptr)
			DUOL_TRACE(DUOL_CONSOLE, "sprite Null.");

		auto parent = this->GetGameObject()->GetComponent<Transform>()->GetParent();
		DUOLGraphicsLibrary::Rect rect;


		if (parent)
		{
			// 부모가 스크롤 바를 가지고 있다.
			auto scrollbar = parent->GetGameObject()->GetComponent<DUOLGameEngine::Scrollbar>();
			// 내가 버튼을 가지고 있냐
			auto button = this->GetGameObject()->GetComponent<DUOLGameEngine::Button>();

			rect = _rectTransform->CalculateRect(GraphicsManager::GetInstance()->GetScreenSize());

			if (scrollbar && !button)
				rect = scrollbar->CalculateRect(parent->GetGameObject()->GetComponent<RectTransform>()->GetCalculateRect());
			else if (scrollbar && button)
				rect = scrollbar->CalculateButtonRect(parent->GetGameObject()->GetComponent<RectTransform>()->GetCalculateRect(), rect);
		}
		else
			rect = _rectTransform->CalculateRect(GraphicsManager::GetInstance()->GetScreenSize());

		auto mousepos = DUOLGameEngine::InputManager::GetInstance()->GetMousePosition();

		//DUOL_INFO(DUOL_CONSOLE, "left : {} / top: {} / mouseposX : {} /mouseposY : {}", rect.left, rect.top, mousepos.x, mousepos.y);

		_rectTransform->SetCalculateRect(rect);

		_sprite->GetSprite()->_rect = rect;
		_sprite->GetSprite()->_pivot = _rectTransform->GetPivot();

		//std::string path = DUOLCommon::StringHelper::ToString(_sprite->GetSprite()->_texture->GetTextureName());

		//std::string path= std::to_string(reinterpret_cast<std::uintptr_t>(_canvas));
		//DUOL_INFO(DUOL_CONSOLE, "Image Canvas Pointer {}", path);


		_canvas->DrawSprite(_sprite->GetSprite(), _orderInLayer);
	}
}