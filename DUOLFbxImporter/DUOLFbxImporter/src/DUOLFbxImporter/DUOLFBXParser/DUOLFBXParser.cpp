#include <filesystem>

#include "DUOLFBXParser/DUOLFBXParser.h"
#include "DUOLFBXParser/DUOLFBXData.h"

DUOLParser::DUOLFBXParser::DUOLFBXParser()
{
}

DUOLParser::DUOLFBXParser::~DUOLFBXParser()
{
	Destory();
}

std::shared_ptr<FBXModel> DUOLParser::DUOLFBXParser::LoadFBX(const std::string& path)
{
	_fbxModel = std::make_shared<FBXModel>();

	Initialize();

	LoadScene(path);

	return _fbxModel;
}

/**
 * \brief Create만 담당한다.
 */
void DUOLParser::DUOLFBXParser::Initialize()
{
	// fbxManager의 인스턴스를 생성한다. 
	_fbxManager = fbxsdk::FbxManager::Create();

	fbxsdk::FbxIOSettings* ios = fbxsdk::FbxIOSettings::Create(_fbxManager, IOSROOT);
	_fbxManager->SetIOSettings(ios);

	// Scene을 만들어준다. 
	_fbxScene = fbxsdk::FbxScene::Create(_fbxManager, "");

	_fbxImporter = fbxsdk::FbxImporter::Create(_fbxManager, "");
}

void DUOLParser::DUOLFBXParser::Destory()
{
	_fbxScene->Destroy();
	_fbxImporter->Destroy();
	_fbxManager->Destroy();
}

void DUOLParser::DUOLFBXParser::LoadScene(std::string path)
{
	bool success = _fbxImporter->Initialize(path.c_str(), -1, _fbxManager->GetIOSettings());

	// Scene을 가져와서 넣어준다.
	// Scene에다가 모델을 풀어놓음
	// 이제 본과 애니메이션 매쉬를 로드할 수 있음
	_fbxImporter->Import(_fbxScene);

	// 파일 크기가 크게나와서 그거 조정하는거
	// 근데 듀얼 엔진에서는 적용이 되는지 모르겠다,,??
	// 축이 없어서 그런가..
	/*fbxsdk::FbxSystemUnit lFbxFileSysteomUnit = _fbxScene->GetGlobalSettings().GetSystemUnit();
	fbxsdk::FbxSystemUnit lFbxOriginSystemUnit = _fbxScene->GetGlobalSettings().GetOriginalSystemUnit();
	double factor = lFbxFileSysteomUnit.GetScaleFactor();

	const fbxsdk::FbxSystemUnit::ConversionOptions IConversionOptions =
	{ true,true,true,true,true,true };
	lFbxFileSysteomUnit.m.ConvertScene(_fbxScene, IConversionOptions);
	lFbxOriginSystemUnit.m.ConvertScene(_fbxScene, IConversionOptions);*/

	// 좌표축을 가져온다.
	FbxAxisSystem sceneAxisSystem = _fbxScene->GetGlobalSettings().GetAxisSystem();

	// 씬 내의 좌표축을 바꾼다.
	_fbxScene->GetGlobalSettings().SetAxisSystem(fbxsdk::FbxAxisSystem::DirectX);

	// 씬 내에서 삼각형화 할 수 있는 모든 노드를 삼각형화 시킨다. 
	FbxGeometryConverter* geometryConverter = new FbxGeometryConverter(_fbxManager);

	geometryConverter->Triangulate(_fbxScene, true);

	// scene을 가져오는게 성공하면 노드를 순회해서 씬에 저장되어 있는 트리구조에서 오브젝트를 찾아옴
	if (success)
	{
		ProcessNode(_fbxScene->GetRootNode());
	}
}

/**
 * \brief 트리 구조이기 때문에 루트 노드로부터 재귀 함수를 통해 오브젝트 개수와 종류를 알아옴
 * \param parentnode
 * \param node
 */
void DUOLParser::DUOLFBXParser::ProcessNode(fbxsdk::FbxNode* node)
{
	// 모든 Animation을 저장한다.
// 애니메이션도 잘들어간다.
	LoadAnimation();

	// 모든 Bone을 저장한다.
	// bone은 잘들어간다. 
	ProcessBone(node);

	// 모든 Mesh를 저장한다. 
	ProcessMesh(node);

	ProcessMaterial(node);
}

/**
 * \brief Get Mesh Data(pos, UV, normal, tangent, binormal)
 * \param node
 */
void DUOLParser::DUOLFBXParser::ProcessMesh(FbxNode* node)
{
	fbxsdk::FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();

	// Mesh일때만 들어온다.
	if (nodeAttribute && nodeAttribute->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh)
	{
		LoadMesh(node);
	}

	int count = node->GetChildCount();

	// 재귀 구조로 모든 자식을 다 돈다. 
	for (int i = 0; i < count; i++)
	{
		ProcessMesh(node->GetChild(i));
	}
}

/**
 * \brief Material Count 만큼 돌면서 node의 material을 불러온다.
 * \param node
 */
void DUOLParser::DUOLFBXParser::ProcessMaterial(FbxNode* node)
{
	// Material Count로 갯수를 불러온다. 
	const int materialcount = node->GetMaterialCount();

	for (int i = 0; i < materialcount; i++)
	{
		fbxsdk::FbxSurfaceMaterial* surfaceMaterial = node->GetMaterial(i);

		LoadMaterial(surfaceMaterial, node->GetName());
	}

	int count = node->GetChildCount();

	// 재귀 구조로 모든 자식을 다 돈다. 
	for (int i = 0; i < count; i++)
	{
		ProcessMaterial(node->GetChild(i));
	}
}

void DUOLParser::DUOLFBXParser::ProcessBone(FbxNode* node)
{
	for (size_t childindex = 0; childindex < node->GetChildCount(); ++childindex)
	{
		fbxsdk::FbxNode* currNode = node->GetChild(childindex);
		// parentindex를 -1이라고 하는데 이건 rootnode라는 의미이다.
		LoadSkeleton(currNode, childindex, -1);
	}
}

void DUOLParser::DUOLFBXParser::ProcessAnimation(FbxNode* node)
{
	// 이미 애니메이션이 생성되고 난후다.
	// 그러므로 애니메이션 크기를 불러온다.
	const size_t animationCount = _fbxModel->animationClipList.size();
	std::vector<std::shared_ptr<DuolData::KeyFrame>> keyframeList;

	// 애니메이션 갯수만큼 돌려준다.
	// 한 애니메이션에는 여러 키 프레임을 가질수 있다. 
	for (size_t count = 0; count < animationCount; ++count)
	{
		// 시간이 어떤 타입인지 받아온다
		fbxsdk::FbxTime::EMode timeMode = _fbxScene->GetGlobalSettings().GetTimeMode();

		// 키프레임만큼 돌리고 키프레임을 생성해준다. 
		for (fbxsdk::FbxLongLong frame = 0; frame < _fbxModel->animationClipList[count]->totalKeyFrame; ++frame)
		{
			std::shared_ptr<DuolData::KeyFrame> keyframeInfo = std::make_shared<DuolData::KeyFrame>();

			fbxsdk::FbxTime fbxTime = 0;
			// Time
			fbxTime.SetFrame(frame, timeMode);

			// Local Transform = 부모 bone의 global transform의 역행렬 * 자신 bone의 global transform;
			// 그 프레임의 시간에 local을 들고온다.
			// EvaluateGlobalTransform : 변환행렬
			fbxsdk::FbxAMatrix localTransform = node->EvaluateGlobalTransform(fbxTime);

			DUOLMath::Matrix localTM;

			// 부모가 있는지 체크한다.
			FbxNode* parent = node->GetParent();

			if (parent)
			{
				// 부모도 스켈레톤 애니메이션인지 확인한다.
				FbxNodeAttribute* parentAttribute = parent->GetNodeAttribute();

				if (parentAttribute && parentAttribute->GetAttributeType() == fbxsdk::FbxNodeAttribute::eSkeleton)
				{
					fbxsdk::FbxAMatrix ParentTransform = parent->EvaluateGlobalTransform(fbxTime);

					localTransform = ParentTransform.Inverse() * localTransform;

					localTM = ConvertMatrix(localTransform);
				}
				// 부모가 스켈레톤이 아니면 자기 자신 그대로 사용
				else
				{
					localTM = ConvertMatrix(localTransform);

					// 돌려줬기때문에 돌려준다.
					const auto roll = -90.0f * DirectX::XM_PI / 180.0f;

					DUOLMath::Quaternion q = DirectX::XMQuaternionRotationRollPitchYaw(roll, 0.0f, 0.0f);

					localTM *= XMMatrixRotationQuaternion(q);
				}
			}

			DirectX::XMVECTOR localScale;
			DirectX::XMVECTOR localRot;
			DirectX::XMVECTOR localPosition;
			DirectX::XMMatrixDecompose(&localScale, &localRot, &localPosition, localTM);

			keyframeInfo->time = fbxTime.GetSecondDouble();

			keyframeInfo->localTransform = DUOLMath::Vector3(localPosition);
			keyframeInfo->localRotation = DUOLMath::Vector4(localRot);
			keyframeInfo->localScale = DUOLMath::Vector3(localScale);

			keyframeList.emplace_back(keyframeInfo);
		}
		_fbxModel->animationClipList[count]->keyframeList.emplace_back(keyframeList);
	}
}

void DUOLParser::DUOLFBXParser::LoadAnimation()
{
	// 일단 정보가 다 들어감을 확인함 _ tick per frame 이 이상한거같기도하고?
	// 애니메이션 이름들을 모두 담는 배열
	fbxsdk::FbxArray<FbxString*> animationNameArray;
	fbxsdk::FbxDocument* document = dynamic_cast<fbxsdk::FbxDocument*>(_fbxScene);
	if (document != nullptr)
	{
		document->FillAnimStackNameArray(animationNameArray);
	}

	size_t nowAnimationCount = _fbxImporter->GetAnimStackCount();

	// 애니메이션이 없으면 리턴한다. 
	if (nowAnimationCount <= 0)
		return;

	// 애니메이션 만큼 돌면서 애니메이션을 생성해주고 넣어준다.
	for (size_t count = 0; count < nowAnimationCount; ++count)
	{
		// Get animation information
		fbxsdk::FbxAnimStack* currAnimStack = _fbxScene->GetSrcObject<fbxsdk::FbxAnimStack>(0);

		if (currAnimStack == nullptr)
			continue;

		std::shared_ptr<DuolData::AnimationClip> animationClip = std::make_shared< DuolData::AnimationClip>();

		// 애니메이션 이름 
		fbxsdk::FbxString animStackName = currAnimStack->GetName();
		animationClip->animationName = animStackName.Buffer();

		fbxsdk::FbxTakeInfo* takeInfo = _fbxScene->GetTakeInfo(animStackName);

		// 시작시간, 종료시간, 초당 프레임에 대한 정보
		// 일단 SDK로 받아왔는데 이걸 내가 바꿔줘야하나 고민_ 확인해보기
		// frame을 들고오는것이다. 시간을 들고오려면 .GetSecondDouble() 해야함
		fbxsdk::FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart();
		fbxsdk::FbxTime endTime = takeInfo->mLocalTimeSpan.GetStop();

		// GetFrameCount : 시간의 프레임 수를 가져온다.
		// 그냥 프레임수를 가져올때는 제대로 가져오지 못하는듯하다.
		// 그냥 그대로 불러와 보자 : 제대로 불러온다 GetFrameCount 함수를 제대로 이해하고 못쓰는듯
		animationClip->frameRate = (float)fbxsdk::FbxTime::GetFrameRate(_fbxScene->GetGlobalSettings().GetTimeMode());

		if (startTime.GetFrameCount(fbxsdk::FbxTime::eFrames24) < endTime.GetFrameCount(fbxsdk::FbxTime::eFrames24))
		{
			animationClip->startKeyFrame = startTime.GetFrameCount(fbxsdk::FbxTime::eFrames24);
			animationClip->endKeyFrame = endTime.GetFrameCount(fbxsdk::FbxTime::eFrames24);
			animationClip->totalKeyFrame = endTime.GetFrameCount(fbxsdk::FbxTime::eFrames24) - startTime.GetFrameCount(fbxsdk::FbxTime::eFrames24) + 1;
			animationClip->tickPerFrame = (endTime.GetSecondDouble() - startTime.GetSecondDouble()) / (animationClip->totalKeyFrame);
		}

		_fbxModel->animationClipList.emplace_back(animationClip);
	}
}

void DUOLParser::DUOLFBXParser::LoadMesh(FbxNode* node)
{
	// Mesh를 뽑는다.
	fbxsdk::FbxMesh* currentMesh = node->GetMesh();
	fbxsdk::FbxGeometry* geometry = node->GetGeometry();

	// Mesh를 생성해준다. 
	std::shared_ptr<DuolData::Mesh> meshinfo = std::make_shared<DuolData::Mesh>();

	// model에 Mesh를 넣어준다.
	_fbxModel->fbxMeshList.emplace_back(meshinfo);

	meshinfo->nodeName = node->GetName();

	// NodeTM 넣기
	fbxsdk::FbxAMatrix nodetransform = _fbxScene->GetAnimationEvaluator()->GetNodeGlobalTransform(node);

	DUOLMath::Matrix nodematrix = ConvertMatrix(nodetransform);

	const auto roll = -90.0f * DirectX::XM_PI / 180.0f;

	DUOLMath::Quaternion q = DirectX::XMQuaternionRotationRollPitchYaw(roll, 0.0f, 0.0f);

	nodematrix *= XMMatrixRotationQuaternion(q);

	meshinfo->nodeTM = nodematrix;

	// node의 Parent 찾기
	// 근데 이게 Mesh의 부모인지 아니면 그 트리구조의 부모인지 모르겠다.
	// 만약 트리구조의 부모라면 FindMesh로 Mesh에서만 찾아야함.
	fbxsdk::FbxMesh* parentMesh = node->GetParent()->GetMesh();

	// Mesh의 부모 노드가 존재하는지 여부 판단
	if (parentMesh == nullptr)
	{
		meshinfo->isparent = false;
	}
	// 만약 부모가 있다면 이름을 가져와서 넣어준다.
	else
	{
		// 부모가 있다. 
		meshinfo->isparent = true;
		// 부모 이름을 가져와서 넣어준다.
		const char* parentName = node->GetParent()->GetName();
		meshinfo->parentName = parentName;
		// 부모 Mesh를 넣어준다. 
		std::shared_ptr<DuolData::Mesh> nodeparentmesh = FindMesh(parentName);
		meshinfo->parentMesh = nodeparentmesh;
		// 부모에 자식으로 넣어준다.
		nodeparentmesh->childList.emplace_back(meshinfo);
	}

	// Node TRS 설정
	//SetTransform(node);

	// Mesh Vertex
	// fbxsdk에서는 vertex를 controlpoint라고 한다. 
	const int vertexcount = currentMesh->GetControlPointsCount();

	// Mesh에서 삼각형 개수를 가져온다.
	// fbxsdk 에서는 face를 polygon이라고 한다.
	const int facecount = currentMesh->GetPolygonCount();

	// vertex 가 없으면 처리 X
	if (vertexcount < 1)
		return;

	meshinfo->vertexList.resize(vertexcount);

	// position 정보를 가져온다. (축을 바꿔야함)
	fbxsdk::FbxVector4* controlpoints = currentMesh->GetControlPoints();

	// Vertex는 잘 들어감을 확인했다.
	for (int i = 0; i < vertexcount; i++)
	{
		meshinfo->vertexList[i].position.x = static_cast<float>(controlpoints[i].mData[0]);
		meshinfo->vertexList[i].position.y = static_cast<float>(controlpoints[i].mData[2]);
		meshinfo->vertexList[i].position.z = static_cast<float>(controlpoints[i].mData[1]);

		meshinfo->vertexList[i].position = DUOLMath::XMVector3TransformCoord(meshinfo->vertexList[i].position, nodematrix);
	}

	// 가중치랑 넣어줘야한다.
	const int deformerCount = currentMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);

	for (int i = 0; i < deformerCount; ++i)
	{
		// 스키닝 메쉬 count가 있으므로 스키닝 메쉬이다.
		meshinfo->isSkinned = true;

		_fbxModel->isSkinnedAnimation = true;

		fbxsdk::FbxSkin* fbxSkin = static_cast<fbxsdk::FbxSkin*>(currentMesh->GetDeformer(i, fbxsdk::FbxDeformer::eSkin));

		if (fbxSkin)
		{
			// 어떤 보간인지 이런거인듯?
			// 메쉬가 변형되는 방식
			fbxsdk::FbxSkin::EType type = fbxSkin->GetSkinningType();

			if (fbxsdk::FbxSkin::eRigid == type)
			{
				// Cluster는 skinning정보가 있는 bone의 갯수만 준다.
				const int clusterCount = fbxSkin->GetClusterCount();

				for (int j = 0; j < clusterCount; ++j)
				{
					fbxsdk::FbxCluster* cluster = fbxSkin->GetCluster(j);
					if (cluster->GetLink() == nullptr)
						continue;

					// 이제 본이 무엇인지 찾아야한다.
					std::string BoneName = cluster->GetLink()->GetName();
					int boneIndex = GetBoneIndex(BoneName);

					// 해당 본에 영향을 받는 정점 인덱스 갯수가 나옴
					const int indicesCount = cluster->GetControlPointIndicesCount();

					for (int k = 0; k < indicesCount; ++k)
					{
						// 해당 정점의 가중치
						double weight = cluster->GetControlPointWeights()[k];

						// 컨트롤 포인트의 인덱스(버텍스의 인덱스)
						int controlpointIndex = cluster->GetControlPointIndices()[k];

						// 가중치는 최대 8개(더들어갈수도있나?)
						// 돌면서 빈곳에 넣고 break한다
						for (int weightindex = 0; weightindex < 8; ++weightindex)
						{
							if (meshinfo->vertexList[controlpointIndex].boneIndices[weightindex] == -1)
							{
								meshinfo->vertexList[controlpointIndex].boneWeight[weightindex] = weight;
								meshinfo->vertexList[controlpointIndex].boneIndices[weightindex] = boneIndex;

								break;
							}
						}
					}

					// 이제 계층구조를 가진 BoneOffSetMatrix를 구해야한다.
					// FbxSdk에서는 대놓고 구할수없다.
					// GetTransformLinkMaterix로 해당 본의 월드 Matrix를 얻고
					// GetTransformMatrix로는 부모까지 오게된 Matrix를 얻는다
					// 해당 부모 행렬의 역행렬을 곱하면 부모 행렬기준 자기 자신의 정보에 대한
					// 행렬값만 남게 되서 계층구조적 행렬값을 갖을 수 있게된다는데..
					fbxsdk::FbxAMatrix transform;
					fbxsdk::FbxAMatrix linkTransform;

					cluster->GetTransformMatrix(transform);
					cluster->GetTransformLinkMatrix(linkTransform);

					DUOLMath::Matrix boneTransform = ConvertMatrix(transform);
					DUOLMath::Matrix boneLinkTransform = ConvertMatrix(linkTransform);

					// BindPose 행렬을 구하자
					fbxsdk::FbxAMatrix geometryTransform = GetGeometryTransformation(currentMesh->GetNode());
					DUOLMath::Matrix geometryMatrix = ConvertMatrix(geometryTransform);
					
					DUOLMath::Matrix offsetMatrix = boneTransform * boneLinkTransform.Invert() * geometryMatrix;

					_fbxModel->fbxBoneList[boneIndex]->offsetMatrix = offsetMatrix;
				}
			}
		}
	}

	// vertex 갯수
	int vertexcounter = 0;

	// 버텍스 쪼개기를 하기위한 체크 요소이다.
	std::vector<bool> isvertex(vertexcount, false);

	// face 갯수만큼 size를 지정해줌(삼각형의 개수)
	// face랑 mesh를 어떤식으로 연결할꺼냐 생각해봐야함
	meshinfo->meshFace.resize(facecount);

	for (unsigned int i = 0; i < facecount; i++)
	{
		meshinfo->meshFace[i] = std::make_shared <DuolData::Face>();

		std::shared_ptr<DuolData::Face> nowface = meshinfo->meshFace[i];

		// 삼각형은 세개의 정점으로 구성되어 있음
		// 여기서 버텍스 쪼개기를 해줘야 한다. 
		for (unsigned int j = 0; j < 3; j++)
		{
			// i번째 Face에 j번째 버텍스 인덱스를 가져온다.
			int controlpointIndex = currentMesh->GetPolygonVertex(i, j);
			int uvindex = currentMesh->GetTextureUVIndex(i, j);

			// 버텍스 쪼개기
			if (isvertex[controlpointIndex] == true)
			{
				// new vertex만들기
				DuolData::Vertex vertex;

				vertex.position = meshinfo->vertexList[controlpointIndex].position;

				for (int weightidx = 0; weightidx < 8; ++weightidx)
				{
					vertex.boneWeight[weightidx] = meshinfo->vertexList[controlpointIndex].boneWeight[weightidx];
					vertex.boneIndices[weightidx] = meshinfo->vertexList[controlpointIndex].boneIndices[weightidx];
				}
				meshinfo->vertexList.push_back(vertex);

				controlpointIndex = meshinfo->vertexList.size() - 1;

				isvertex.push_back(true);
			}

			// 노말을 불러온다.
			GetNormal(currentMesh, meshinfo, controlpointIndex, vertexcounter);
			GetUV(currentMesh, meshinfo, controlpointIndex, vertexcounter);

			// Index를 넣어준다.
			nowface->vertexIndex[j] = controlpointIndex;

			vertexcounter++;
			// 이 버텍스는 이미 접근했어요 체크
			isvertex[controlpointIndex] = true;
		}

		meshinfo->indices.emplace_back(nowface->vertexIndex[0]);
		meshinfo->indices.emplace_back(nowface->vertexIndex[2]);
		meshinfo->indices.emplace_back(nowface->vertexIndex[1]);
	}
}

void DUOLParser::DUOLFBXParser::LoadMaterial(const fbxsdk::FbxSurfaceMaterial* surfacematerial,std::string nodename)
{
	std::shared_ptr<DuolData::Material> material = std::make_shared<DuolData::Material>();

	material->materialName = surfacematerial->GetName();

	// 퐁 모델에 대한걸 가져온다.
	if (surfacematerial->GetClassId().Is(fbxsdk::FbxSurfacePhong::ClassId))
	{
		// Ambient Data
		material->material_Ambient.x = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->Ambient.Get()[0]) * 10.0f;
		material->material_Ambient.y = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->Ambient.Get()[1]) * 10.0f;
		material->material_Ambient.z = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->Ambient.Get()[2]) * 10.0f;
		material->material_Ambient.w = 1.0f;

		// Diffuse Data
		material->material_Diffuse.x = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->Diffuse.Get()[0]);
		material->material_Diffuse.y = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->Diffuse.Get()[1]);
		material->material_Diffuse.z = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->Diffuse.Get()[2]);
		material->material_Diffuse.w = 1.0f;

		// Specular Data
		material->material_Specular.x = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->Specular.Get()[0]);
		material->material_Specular.y = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->Specular.Get()[1]);
		material->material_Specular.z = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->Specular.Get()[2]);
		material->material_Specular.w = 1.0f;

		// Emissive Data
		material->material_Emissive.x = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->Emissive.Get()[0]);
		material->material_Emissive.y = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->Emissive.Get()[1]);
		material->material_Emissive.z = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->Emissive.Get()[2]);
		material->material_Emissive.w = 1.0f;

		// Transparecy Data (투명도)
		material->material_Transparency = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->TransparencyFactor.Get());

		// Shininess Data
		material->roughness = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->Shininess.Get());

		// Reflectivity Data
		material->material_Reflectivity = static_cast<float>(((fbxsdk::FbxSurfacePhong*)surfacematerial)->ReflectionFactor.Get());
	}

	material->albedoMap = GetTextureName(surfacematerial, fbxsdk::FbxSurfaceMaterial::sDiffuse);
	material->normalMap = GetTextureName(surfacematerial, fbxsdk::FbxSurfaceMaterial::sNormalMap);
	material->roughnessMap = GetTextureName(surfacematerial, fbxsdk::FbxSurfaceMaterial::sShininess);
	material->emissiveMap = GetTextureName(surfacematerial, fbxsdk::FbxSurfaceMaterial::sEmissive);

	if (material->albedoMap != L"")
		material->isAlbedo = true;
	if (material->normalMap != L"")
		material->isNormal = true;
	if (material->roughnessMap != L"")
		material->isRoughness = true;
	if (material->emissiveMap != L"")
		material->isEmissive = true;


	// Mesh에는 material Name만 넣어준다.
	std::shared_ptr<DuolData::Mesh> meshinfo = FindMesh(nodename);
	meshinfo->materialName = surfacematerial->GetName();

	// Material은 fbxmodel이 가지고 있는다. 
	_fbxModel->fbxmaterialList.emplace_back(material);
}

void DUOLParser::DUOLFBXParser::LoadSkeleton(fbxsdk::FbxNode* node, int nowindex, int parentindex)
{
	fbxsdk::FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute && attribute->GetAttributeType() == fbxsdk::FbxNodeAttribute::eSkeleton)
	{
		std::shared_ptr<DuolData::Bone> boneInfo = std::make_shared<DuolData::Bone>();
		boneInfo->boneName = node->GetName();
		boneInfo->parentIndex = parentindex;

		// Mesh Node를 돌렸기 때문에 본들도 돌려줘야한다.
		// 둘의 차이점을 모르겠다 돌려보면서 확인해보기
		fbxsdk::FbxAMatrix nodeTransform = _fbxScene->GetAnimationEvaluator()->GetNodeGlobalTransform(node);
		//FbxAMatrix nodeTransform = node->EvaluateGlobalTransform(fbxsdk::FbxTime(0));

		DUOLMath::Matrix nodeMatrix = ConvertMatrix(nodeTransform);

		const auto roll = -90.0f * DirectX::XM_PI / 180.0f;

		DUOLMath::Quaternion q = DirectX::XMQuaternionRotationRollPitchYaw(roll, 0.0f, 0.0f);

		nodeMatrix *= XMMatrixRotationQuaternion(q);

		boneInfo->nodeMatrix = nodeMatrix;

		_fbxModel->fbxBoneList.emplace_back(boneInfo);

		// Skeleton이면 애니메이션을 돌려준다.
		ProcessAnimation(node);
	}

	for (size_t childcount = 0; childcount < node->GetChildCount(); ++childcount)
	{
		LoadSkeleton(node->GetChild(childcount), _fbxModel->fbxBoneList.size(), nowindex);
	}
}

std::wstring DUOLParser::DUOLFBXParser::GetTextureName(const fbxsdk::FbxSurfaceMaterial* surfacematerial, const char* materialproperty)
{
	std::string name;
	// 텍스쳐를 로드하려면 먼저 유효한 property를 검색한다
	fbxsdk::FbxProperty textureproperty = surfacematerial->FindProperty(materialproperty);

	if (textureproperty.IsValid())
	{
		// 연결되어있는 맵의 개수인듯(?)
		int count = textureproperty.GetSrcObjectCount();

		if (0 < count)
		{
			// 연결되어 있는 지정된 인덱스의 개체를 반환함
			// 기본 인덱스는 0 인듯
			fbxsdk::FbxFileTexture* texture = textureproperty.GetSrcObject<fbxsdk::FbxFileTexture>(0);
			// name을 이상하게 받아와서 경로가 이상해진다.
			if (texture)
				name = texture->GetRelativeFileName();
		}
	}

	std::wstring wname = L"";

	wname.assign(name.begin(), name.end());

	// 일단 예제는 Texture\\2048\\texturename 이렇게 나오므로 texturename이 나오게 수정이 필요하다.
	// 근데 path.filename을 쓰면 파일 이름만 뽑아준다.
	std::wstring filename = std::filesystem::path(wname).filename();

	return filename;
}

/**
 * \brief 메시의 최적화를 해준다.
 *		노말값, 텍스쳐 좌표에 따라 곂치지 않으면 늘리고, 중첩되면 지운다.
 * \return
 */
bool DUOLParser::DUOLFBXParser::ConvertOptimize(std::shared_ptr<DuolData::Mesh>)
{
	return true;
}

std::shared_ptr<DuolData::Mesh> DUOLParser::DUOLFBXParser::FindMesh(const std::string nodename)
{
	for (auto mesh : _fbxModel->fbxMeshList)
	{
		if (mesh->nodeName == nodename)
			return mesh;
	}
	return nullptr;
}

void DUOLParser::DUOLFBXParser::GetNormal(fbxsdk::FbxMesh* mesh, std::shared_ptr<DuolData::Mesh>  meshinfo, int controlpointindex, int vertexindex)
{
	// 여기서 예외처리 해주기
	if (mesh->GetElementNormalCount() < 1)
		return;

	// 노말 획득
	const fbxsdk::FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);
	int index;

#pragma region MappingMode
	// 매핑 모드
	switch (vertexNormal->GetMappingMode())
	{
		// control point mapping
	case fbxsdk::FbxGeometryElement::eByControlPoint:
	{
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			index = controlpointindex;
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			// 우리가 필요한 인덱스를 뽑아준다.
			index = vertexNormal->GetIndexArray().GetAt(controlpointindex);
		}
		break;
		default:
			break;
		}
	}
	break;
	case fbxsdk::FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			index = vertexindex;
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			// 우리가 필요한 인덱스를 뽑아준다.
			index = vertexNormal->GetIndexArray().GetAt(vertexindex);
		}
		break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}
#pragma endregion

	meshinfo->vertexList[controlpointindex].normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);;
	meshinfo->vertexList[controlpointindex].normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);;
	meshinfo->vertexList[controlpointindex].normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);;
}

void DUOLParser::DUOLFBXParser::GetUV(fbxsdk::FbxMesh* mesh, std::shared_ptr<DuolData::Mesh>  meshinfo, int controlpointindex, int vertexindex)
{
	if (mesh->GetElementUVCount() < 1)
		return;

	int index;

#pragma region MapponMode
	// Layer에 있는 UV를 가져온다.
	fbxsdk::FbxGeometryElementUV* vertexuv = mesh->GetLayer(0)->GetUVs();

	// Mapping mode -> index / reference mode -> data
	switch (vertexuv->GetMappingMode())
	{
		// Surface Control Point를 위해 하나의 mapping cordinate만 존재
	case FbxGeometryElement::eByControlPoint:
	{
		// Key값의 차이이다.
		switch (vertexuv->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			// control point index를 구한다음(vertexindex) 이를 key값으로해서 direct array에 직접 uv data접근
			index = controlpointindex;
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			// index array에서 실제 index를 획득한 다음에 data를 획득한다.
			index = vertexuv->GetIndexArray().GetAt(controlpointindex);
		}
		break;
		default:
			break;
		}
	}
	break;
	// vertex가 구성하는 polygon만큼의 mapping coordinate를 가지게 된다.
	// 정확히 무슨말인지는 잘 모르겠다. 
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertexuv->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			index = vertexindex;
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			index = vertexuv->GetIndexArray().GetAt(vertexindex);
		}
		break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}
#pragma endregion
	meshinfo->vertexList[controlpointindex].uv.x = static_cast<float>(vertexuv->GetDirectArray().GetAt(index).mData[0]);
	meshinfo->vertexList[controlpointindex].uv.y = 1.0f - static_cast<float>(vertexuv->GetDirectArray().GetAt(index).mData[1]);
}

int DUOLParser::DUOLFBXParser::GetBoneIndex(std::string bonename)
{
	for (size_t count = 0; count < _fbxModel->fbxBoneList.size(); ++count)
	{
		if (_fbxModel->fbxBoneList[count]->boneName == bonename)
		{
			return count;
		}
	}
	return -1;
}

/**
 * \brief node의 TM을 얻기위한것 (단위행렬이다)
 * \param node
 * \return
 */
fbxsdk::FbxAMatrix  DUOLParser::DUOLFBXParser::GetGeometryTransformation(fbxsdk::FbxNode* node)
{
	if (!node)
		throw std::exception("Null for mesh geometry");

	const fbxsdk::FbxVector4 translation = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const fbxsdk::FbxVector4 rotation = node->GetGeometricRotation(FbxNode::eSourcePivot);
	const fbxsdk::FbxVector4 scale = node->GetGeometricScaling(FbxNode::eSourcePivot);

	return fbxsdk::FbxAMatrix(translation, rotation, scale);
}

//void DUOLParser::DUOLFBXParser::DecomposeMatrix(DUOLMath::Matrix nodetm)
//{
//	
//}
