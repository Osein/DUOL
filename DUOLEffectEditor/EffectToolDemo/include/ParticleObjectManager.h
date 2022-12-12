#pragma once
/**

	@class   ParticleObjectManager
	@brief   파티클 오브젝트만을 위한 오브젝트 매니저
	@details ~ 오브젝트 뷰어에 보여줄 오브젝트와 세이브 로드를 할 때 등 파티클 오브젝트에 접근할 때 사용할 오브젝트 매니저이다.

**/
#include <vector>
#include <memory>

namespace Muscle
{
	class GameObject;
}
namespace MuscleGrapics
{
	struct RenderingData_Particle;
}
class ParticleObjectManager
{
public:
	ParticleObjectManager(const ParticleObjectManager& other) = delete;

	ParticleObjectManager& operator =(const ParticleObjectManager& other) = delete;

	ParticleObjectManager(ParticleObjectManager&& other) = delete;

protected:
	ParticleObjectManager() = default;

	~ParticleObjectManager() = default;

public:
	static ParticleObjectManager& Get();

private:
	static ParticleObjectManager _instance;

	std::vector<std::shared_ptr<Muscle::GameObject>> _particleObjects;

public:
	const std::vector<std::shared_ptr<Muscle::GameObject>>& GetParticleObjects();

	std::shared_ptr<Muscle::GameObject>& CreateParticleObject();

	std::shared_ptr<Muscle::GameObject>& CreateParticleObjectFromParticleData(MuscleGrapics::RenderingData_Particle& data, std::shared_ptr<Muscle::GameObject> parent = nullptr);

	void DeleteParticleObject(int index);

	void DeleteAllParticleObject();
};

