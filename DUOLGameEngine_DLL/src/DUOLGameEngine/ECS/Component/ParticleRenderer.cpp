#include "DUOLGameEngine/ECS/Component/ParticleRenderer.h"

#include "DUOLGameEngine/ECS/GameObject.h"
#include "DUOLGameEngine/ECS/Component/Transform.h"
#include "DUOLGameEngine/Manager/GraphicsManager.h"
#include "DUOLGraphicsEngine/ResourceManager/Resource/Mesh.h"

#include <rttr/registration>
#include "DUOLCommon/MetaDataType.h"
#include "DUOLGameEngine/ECS/Object/Material.h"
#include "DUOLGameEngine/Manager/ResourceManager.h"

using namespace rttr;

RTTR_PLUGIN_REGISTRATION
{
	//rttr::registration::class_<DUOLGraphicsEngine::RenderingData_Particle>("RenderingData_Particle")
	//.constructor()
	//(
	//rttr::policy::ctor::as_raw_ptr
	//).property("_objectID",&DUOLGraphicsEngine::RenderingData_Particle::_objectID)
	//(
	//	metadata(DUOLCommon::MetaDataType::Serializable, true)
	//	, metadata(DUOLCommon::MetaDataType::Inspectable, true)
	//	, metadata(DUOLCommon::MetaDataType::InspectType, DUOLCommon::InspectType::Int)
	//);

	rttr::registration::class_<DUOLGameEngine::ParticleRenderer>("ParticleRenderer")
	.constructor()
	(
		rttr::policy::ctor::as_raw_ptr
	)
	.constructor<DUOLGameEngine::GameObject*, const DUOLCommon::tstring&>()
	(
		rttr::policy::ctor::as_raw_ptr
	)
	.property("_particleInitData",&DUOLGameEngine::ParticleRenderer::_particleInitData)
	(
			metadata(DUOLCommon::MetaDataType::Serializable, true));

}

namespace DUOLGameEngine
{
	ParticleRenderer::ParticleRenderer(DUOLGameEngine::GameObject* owner, const DUOLCommon::tstring& name) :
		RendererBase(owner, name)
		, _particleInfo(),
		_prevMatrix(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1)
	{

		_renderObjectInfo._renderInfo = &_particleInfo;

		_isPlay = false;

		_isDelayStart = false;

		_isFirstRun = false;
	}

	ParticleRenderer::~ParticleRenderer()
	{
	}

	void ParticleRenderer::OnUpdate(float deltaTime)
	{
		if (!_isFirstRun)
		{
			if (!_renderObjectInfo._mesh)
			{
				CreateParticleBuffer();
			}

			Play();
		}

		//_particleInfo._particleData._isDelete = GetGameObject()->GetIsDelete(); // 원래는 그래픽스 엔진에서도 버퍼를 삭제할 때 확인 용도 였지만, 경민 그래픽스는 어떻게 할지 모르겠다.
		// 실행 중일 때만 정보를 업데이트한다.
		if (_isPlay)
		{


			_delayTime += deltaTime;
			if (_delayTime <= _particleInfo._particleData._commonInfo.gStartDelay[0])
				return;

			//파티클 데이터를 받아올때. 갱신 하세요 + 그 머시기 타임까지
			if (_particleInfo._particleData._commonInfo.gMaxParticles % 1024 == 0)
				_particleInfo._particleData._dim = _particleInfo._particleData._commonInfo.gMaxParticles / 1024;
			else
				_particleInfo._particleData._dim = _particleInfo._particleData._commonInfo.gMaxParticles / 1024 + 1;

			_particleInfo._particleData._dim = static_cast<int>(ceil(pow((double)_particleInfo._particleData._dim, double(1.0 / 3.0))));

			if (_particleInfo._particleData._EmissionTime >= _particleInfo._particleData._emission.gEmissiveTime)
				_particleInfo._particleData._EmissionTime = 0;

			_particleInfo._particleData._EmissionTime += deltaTime * _particleInfo._particleData._commonInfo.gSimulationSpeed;

			_particleInfo._particleData._emission.gEmissiveCount = DUOLMath::MathHelper::RandF(_emissiveCount[0], _emissiveCount[1]);

			_isDelayStart = true;

			_playTime += deltaTime;

			_particleInfo._particleData._commonInfo.gTransformMatrix = GetTransform()->GetWorldMatrix();

			_particleInfo._particleData._commonInfo.gDeltaMatrix = _prevMatrix.Invert() * GetTransform()->GetWorldMatrix();

			_particleInfo._particleData._commonInfo.gParticlePlayTime = _playTime;

			_prevMatrix = GetTransform()->GetWorldMatrix();

			if (!_isFirstRun)
				_particleInfo._particleData._EmissionTime = std::numeric_limits<float>::max();
		}
	}

	void ParticleRenderer::OnStart()
	{
	}

	void ParticleRenderer::Render()
	{

		if (_isPlay && _isDelayStart)
		{
			GraphicsManager::GetInstance()->ReserveRenderObject(&_renderObjectInfo);

			_isFirstRun = true;
		}
	}

	void ParticleRenderer::Play()
	{
		Stop();

		_isPlay = true;

		_isFirstRun = false;

		_playTime = 0;

		_delayTime = 0;

		_isDelayStart = false;

		_particleInfo._particleData._flag |= 1 << 28;

		for (auto iter : GetGameObject()->GetTransform()->GetChildGameObjects())
		{
			auto renderer = iter->GetComponent<ParticleRenderer>();
			if (renderer)
				renderer->Play();
		}
	}

	void ParticleRenderer::Stop()
	{
		_isPlay = false;
		for (auto iter : GetGameObject()->GetTransform()->GetChildGameObjects())
		{
			auto renderer = iter->GetComponent<ParticleRenderer>();
			if (renderer)
				renderer->Stop();
		}
	}

	bool ParticleRenderer::GetIsPlay() const
	{
		if (!GetIsEnabled())
			return false;

		return _isPlay;
	}

	DUOLGraphicsEngine::ConstantBuffDesc::CB_PerObject_Particle& ParticleRenderer::GetParticleData()
	{
		return _particleInfo._particleData;
	}

	void ParticleRenderer::CreateParticleBuffer()
	{
		_particleInitData = ResourceManager::GetInstance()->LoadRenderingData_Particle(_materials[0]->GetName());

		_emissiveCount[0] = _particleInitData->_emission._emissiveCount[0];
		_emissiveCount[1] = _particleInitData->_emission._emissiveCount[1];

		_particleInfo = *_particleInitData;

		_particleInfo._objectID = GetGameObject()->GetUUID();

		const auto& _graphicsEngine = DUOLGameEngine::GraphicsManager::GetInstance()->_graphicsEngine;
		auto gameObject = GetGameObject();
		auto uuid = gameObject->GetUUID();
		DUOLCommon::tstring str = _T("Particle") + DUOLCommon::StringHelper::ToTString(uuid);

		_renderObjectInfo._mesh = _graphicsEngine->CreateParticle(
			str,
			_particleInfo._particleData._commonInfo.gMaxParticles);
		_renderObjectInfo._materials = &_primitiveMaterials;

		_renderObjectInfo._renderInfo = &_particleInfo;
	}
}
