#pragma once

namespace Muscle
{
	class ParticleRenderer : public IComponents
	{
	public:
		ParticleRenderer(std::shared_ptr<GameObject> _GameObject);

		~ParticleRenderer();

	private:
		std::shared_ptr<Muscle::Transform> _transform;

		std::shared_ptr<RenderingData_Particle> _particleData;

		bool _isFirstUpdate;

		bool _isPlay;


	public:
		// �ʻ�� Getter
		std::shared_ptr<RenderingData_Particle> GetParticleData() { return _particleData; }

		// ��ƼŬ ����Ʈ ����
		void Play();

		// ��ƼŬ ����Ʈ ����
		void Stop();

	public:
		virtual void Start() override;

		virtual void Update() override;

		virtual void Render() override;

	};
}