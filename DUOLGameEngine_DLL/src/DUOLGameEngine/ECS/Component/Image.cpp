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
		.property("LeftTop", &DUOLGameEngine::Image::GetSourceLeftTop, &DUOLGameEngine::Image::SetSourceLeftTop)
	(
		metadata(DUOLCommon::MetaDataType::Serializable, true)
		, metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Float2)
	)
		.property("RightBottom", &DUOLGameEngine::Image::GetSourceRightBottom, &DUOLGameEngine::Image::SetSourceRightBottom)
	(
		metadata(DUOLCommon::MetaDataType::Serializable, true)
		, metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Float2)
	)
	.property("Fade Image", &DUOLGameEngine::Image::_isFadeImage)
	(
		metadata(DUOLCommon::MetaDataType::Serializable, true)
		, metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Bool)
	)
	.property("Fade Time", &DUOLGameEngine::Image::_fadeTime)
	(
		metadata(DUOLCommon::MetaDataType::Serializable, true)
		, metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Float)
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
	Image::Image(DUOLGameEngine::GameObject* owner, const DUOLCommon::tstring& name) :
		BehaviourBase(owner, name)
		, _canvas(nullptr)
		, _orderInLayer(0)
		, _sprite(new Sprite())
		, _rectTransform(nullptr)
		, _raycastTarget(true)
		, _spriteName(L"None(Sprite)")
		, _isFadeImage(false)
		, _fadeTime(0.0f)
		, _isFadeInit(true)
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
		if (_canvas == nullptr)
			return;

		if (_isEnabled == false)
			return;

		if (_isFadeInit && _isFadeImage)
		{
			GetGameObject()->SetIsActiveSelf(true);
			_isFadeInit = false;
		}

		if (_isFadeImage)
		{
			auto sprite = _sprite->GetSprite();

			float currentAlpha = sprite->_color.w;

			SetColor(sprite->_color + DUOLMath::Vector4(0.f, 0.f, 0.f, -(1.f / _fadeTime) * deltaTime));

			if (sprite->_color.w <= 0.0f)
			{
				GetGameObject()->SetIsActiveSelf(false);

				_isFadeImage = false;
			}
		}

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
				OnUpdate(0.01);
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

		if (sprite != nullptr)
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

	void Image::SetSourceLeftTop(DUOLMath::Vector2 pivot)
	{
		pivot.Clamp({ 0.f, 0.f }, { 1.f, 1.f });
		_sprite->GetSprite()->_imagePivotLeftTop = pivot;
	}

	void Image::SetSourceRightBottom(DUOLMath::Vector2 pivot)
	{
		pivot.Clamp({ 0.f, 0.f }, { 1.f, 1.f });
		_sprite->GetSprite()->_imagePivotRightBottom = pivot;
	}

	DUOLMath::Vector2 Image::GetSourceLeftTop()
	{
		return _sprite->GetSprite()->_imagePivotLeftTop;
	}

	DUOLMath::Vector2 Image::GetSourceRightBottom()
	{
		return _sprite->GetSprite()->_imagePivotRightBottom;
	}

	// 텍스쳐가 끊긴다. 연결해줘야함.
	void Image::OnResize()
	{
		GameObject* object = DUOLGameEngine::UIManager::GetInstance()->GetCanvas();

		if (object == nullptr)
			return;

		SetCanvas(object->GetComponent<Canvas>()->GetCanvas());

		_canvasRectTransform = object->GetComponent<RectTransform>();

		if (_rectTransform == nullptr)
			_rectTransform = GetGameObject()->GetComponent<RectTransform>();

		auto ratio = GraphicsManager::GetInstance()->GetScreenRatio();

		auto screensize = GraphicsManager::GetInstance()->GetScreenSize();

		auto rect = _rectTransform->GetRect();

		float x = rect.x * ratio.x;
		float y = rect.y * ratio.y;

		float width = rect.z * ratio.x;
		float height = rect.w * ratio.y;

		_rectTransform->SetRect(DUOLMath::Vector4(x, y, width, height));
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

		auto ratio = GraphicsManager::GetInstance()->GetScreenRatio();

		auto rect = _rectTransform->GetRect();

		float x = rect.x * ratio.x;
		float y = rect.y * ratio.y;

		float width = rect.z * ratio.x;
		float height = rect.w * ratio.y;

		_rectTransform->SetRect(DUOLMath::Vector4(x, y, width, height));
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


		_rectTransform->SetCalculateRect(rect);

		_sprite->GetSprite()->_rect = rect;
		_sprite->GetSprite()->_pivot = _rectTransform->GetPivot();

		_canvas->DrawSprite(_sprite->GetSprite(), _orderInLayer);
	}
}