#include "DUOLEditor/TestScenes/CometTestScene.h"

#include "DUOLGameEngine/Manager/ResourceManager.h"

#include "DUOLGameEngine/ECS/GameObject.h"
#include "DUOLGameEngine/ECS/Component/Camera.h"
#include "DUOLGameEngine/ECS/Component/TPFController.h"
#include "DUOLGameEngine/ECS/Component/MeshFilter.h"
#include "DUOLGameEngine/ECS/Component/MeshRenderer.h"
#include "DUOLGameEngine/ECS/Component/SkinnedMeshRenderer.h"
#include "DUOLGameEngine/ECS/Component/Animator.h"
#include "DUOLGameEngine/ECS/Component/BoxCollider.h"
#include "DUOLGameEngine/ECS/Component/CapsuleCollider.h"
#include "DUOLGameEngine/ECS/Component/Light.h"
#include "DUOLGameEngine/ECS/Component/Rigidbody.h"
#include "DUOLGameEngine/ECS/Component/ParticleRenderer.h"

//#include "DUOLClient/ECS/Component/Enemy/AI_EnemyBase.h"


#include "DUOLGameEngine/ECS/Component/MeshCollider.h"

namespace DUOLEditor
{
	CometTestScene::CometTestScene() :
		Scene(TEXT("CometTestScene"))
	{

	}

	CometTestScene::~CometTestScene()
	{

	}

	void CometTestScene::Awake()
	{
		// ----------- Main Camera -----------
		DUOLGameEngine::GameObject* mainCamObject = CreateEmpty();

		mainCamObject->SetName(TEXT("MainCamera"));

		mainCamObject->GetComponent<DUOLGameEngine::Transform>()->Translate(DUOLMath::Vector3(0.f, 6.f, -10.f));

		mainCamObject->GetComponent<DUOLGameEngine::Transform>()->Rotate(DUOLMath::Vector3{ 30.f, 0.f, 0.f });

		mainCamObject->AddComponent<DUOLGameEngine::Camera>();

		// ----------- Directional Light -----------
		DUOLGameEngine::GameObject* dirLight = CreateEmpty();

		dirLight->SetName(TEXT("DirectionalLight"));

		DUOLGameEngine::Light* dirLightCom = dirLight->AddComponent<DUOLGameEngine::Light>();

		dirLightCom->SetLightType(DUOLGameEngine::LightType::Directional);

		dirLightCom->SetIntensity(10.f);

		dirLightCom->SetColor(DUOLMath::Vector3{ 1.f, 1.f, 1.f });

		dirLightCom->GetTransform()->Rotate(DUOLMath::Vector3(45.f, 45.f, 0.f));

		// ----------- Point Light -----------
		DUOLGameEngine::GameObject* pointLight = CreateEmpty();

		pointLight->SetName(TEXT("PointLight"));

		pointLight->GetComponent<DUOLGameEngine::Transform>()->SetPosition(DUOLMath::Vector3{ 5.f, 3.f, 0.f });

		DUOLGameEngine::Light* pointLightCom = pointLight->AddComponent<DUOLGameEngine::Light>();

		pointLightCom->SetLightType(DUOLGameEngine::LightType::Point);

		pointLightCom->SetIntensity(10.f);

		pointLightCom->SetColor(DUOLMath::Vector3{ 0.2f, 1.f, 0.2f });

		pointLightCom->SetAngle(15.f);

		//pointLight->AddComponent<DUOLClient::AI_Enemy>();
		// ----------- TEST MODEL -----------
	/*	{
				DUOLGameEngine::GameObject* player = CreateFromFBXModel(TEXT("duolExport"));

				player->GetComponent<DUOLGameEngine::Animator>()->
					SetAnimatorController(DUOLGameEngine::ResourceManager::GetInstance()->GetAnimatorController(TEXT("TestAnimCon")));
		}*/

		// ----------- UNREAL ENGINE LEVEL IMPORT -----------
		{
			// DUOLGameEngine::GameObject* testFBX = CreateFromFBXModel(TEXT("FirstPersonMap"));
		}

		//DUOLGameEngine::GameObject* monsterObject = CreateFromFBXModel(TEXT("duol"));

		////monsterObject->GetComponent<DUOLGameEngine::Transform>()->SetPosition(DUOLMath::Vector3(-50.f, 50.f, 0.f));
		////monsterObject->GetComponent<DUOLGameEngine::Transform>()->SetLocalScale(DUOLMath::Vector3(100.f, 100.f, 100.f));

		//monsterObject->GetComponent<DUOLGameEngine::Animator>()
		//	->SetAnimatorController(DUOLGameEngine::ResourceManager::GetInstance()->GetAnimatorController(TEXT("ProtoEnemyAnimCon")));

		//// monsterObject->AddComponent<DUOLGame::MoveController>();

		__super::Awake();
	}

	void CometTestScene::Update(float deltaTime)
	{
		__super::Update(deltaTime);
	}
}
