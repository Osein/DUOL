#include "DUOLGameEngine/ECS/Component/Canvas.h"
#include "DUOLGameEngine/Manager/GraphicsManager.h"
#include "DUOLGameEngine/Manager/ResourceManager.h"
#include "DUOLGameEngine/Manager/UIManager.h"
#include "DUOLCommon/MetaDataType.h"
#include "DUOLGameEngine/ECS/GameObject.h"


#include <rttr/registration>

#include "DUOLGameEngine/Manager/SceneManagement/Scene.h"

using namespace rttr;

RTTR_PLUGIN_REGISTRATION
{
	rttr::registration::class_<DUOLGameEngine::Canvas>("Canvas")
	.constructor()
	(
		rttr::policy::ctor::as_raw_ptr
	)
	.constructor<DUOLGameEngine::GameObject*, const DUOLCommon::tstring&>()
	(
		rttr::policy::ctor::as_raw_ptr
	)
	.property("Cavnas Name",&DUOLGameEngine::Canvas::GetCanvasName,&DUOLGameEngine::Canvas::SetCanvasName)
	(
		metadata(DUOLCommon::MetaDataType::Inspectable, true)
		, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::String)
	);
}

namespace DUOLGameEngine
{
	Canvas::Canvas() :
		RendererBase(nullptr, TEXT("Canvas"))
		, _renderMode()
		, _canvas(nullptr)
		, _canvasName(L"Canvas")
	{
	}

	Canvas::Canvas(DUOLGameEngine::GameObject* owner, const DUOLCommon::tstring& name) :
		RendererBase(owner, name)
		, _renderMode()
		, _canvas(nullptr)
		, _canvasName(L"Canvas")
	{
	}

	Canvas::~Canvas()
	{
		DUOLGameEngine::UIManager::GetInstance()->RemoveCanvas(this->GetGameObject());
	}

	void Canvas::Render()
	{
		GraphicsManager::GetInstance()->ReserveCanvas(_canvas);
	}

	void Canvas::SceneViewRender()
	{
	}

	void Canvas::OnResize(int width, int height)
	{
		ResourceManager::GetInstance()->DeleteCanvas(_canvasName);

		CreateCanvas(DUOLGraphicsLibrary::CanvasRenderMode::Texture, _canvasName, width, height);
	}

	DUOLGraphicsLibrary::ICanvas* Canvas::GetCanvas()
	{
		if (!_canvas)
		{
			auto screensize = DUOLGameEngine::GraphicsManager::GetInstance()->GetScreenSize();

			_canvas = ResourceManager::GetInstance()->CreateCanvas(DUOLGraphicsLibrary::CanvasRenderMode::Texture, L"Canvas", screensize.x, screensize.y);
		}

		return _canvas;
	}

	void Canvas::CreateCanvas(DUOLGraphicsLibrary::CanvasRenderMode canvas, const DUOLCommon::tstring& canvasName, int width, int height)
	{
		_canvasName = canvasName;

		_canvas = ResourceManager::GetInstance()->CreateCanvas(canvas, canvasName, width, height);
	}
}
