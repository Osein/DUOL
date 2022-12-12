#pragma once
#include "Light.h"

namespace Muscle
{
	class SpotLight : public Light
	{
	public:
		SpotLight(std::shared_ptr<GameObject> _GameObject);
		~SpotLight();

	private:
		MuscleGrapics::SpotLightInfo _SpotLightInfo;

	public:
		virtual void Start() override;

		virtual void LateUpdate() override;

		virtual void Update() override; // 모든 Object Udpate돌릴 때 (ObjectManager) 오브젝트가 Components를 가지고 있으면 같이 업데이트 해주기.

		virtual void Render() override; // 모든 Object 렌더 돌릴 때 (ObjectManager) 오브젝트가 Components를 가지고 있으면 같이 렌더

		void SetRange(float range);
		void SethalfAngle(float angle);
	};
}

