#pragma once

#include <vector>
#include <boost/serialization/assume_abstract.hpp>

#include "DUOLCommon/StringHelper.h"
#include "DUOLMath/DUOLMath.h"

#include <boost/serialization/vector.hpp>

namespace DUOLGraphicsEngine
{
	enum class ParticleFlags
	{
		None = 0,
		ParticleSystemCommonInfo = 1 << 0, // ParticleSystemCommonInfo Module
		Emission = 1 << 1, // Emission Module
		Shape = 1 << 2, // Shape Module
		Velocity_Over_Lifetime = 1 << 3, // Velocity_Over_Lifetime Module
		Force_over_Lifetime = 1 << 4, // Force_over_Lifetime Module
		Color_over_Lifetime = 1 << 5, // Color_over_Lifetime Module
		Size_over_Lifetime = 1 << 6, // Size_over_Lifetime Module
		Rotation_over_Lifetime = 1 << 7, // Rotation_over_Lifetime Module
		Noise = 1 << 8, // Noise Module
		Collision = 1 << 9, // Collision Module
		Texture_Sheet_Animation = 1 << 10, // Texture_Sheet_Animation Module
		Trails = 1 << 11, // Trails Module  
		Renderer = 1 << 12, // Renderer Module

	};

	struct PixelData
	{
		PixelData() :Color(0xffffffff), Depth(0xffffffff), BlendType(0xffffffff)
		{}
		unsigned int Color;
		float Depth;
		unsigned BlendType;
	};

	struct PixelNode
	{
		PixelNode() :Data(), Next(0xffffffff)
		{}
		PixelData Data;
		unsigned int Next;
	};

	//texture Data
	struct Particle
	{
		Particle() :InitialPos(0, 0, 0),
			InitialVel(0, 0, 0, 1.0f),
			Size_StartSize(1, 1, 1, 1),
			Age_LifeTime_Rotation_Gravity(0, 0, 0, 0),
			Type(0),
			StartColor(),
			Color(),
			TexIndex(),
			TrailWidth(1.0f),
			TrailRecordTime(0.0f)
		{
		}
		unsigned int Type;
		DUOLMath::Vector3 InitialPos;

		DUOLMath::Vector4 InitialVel;

		DUOLMath::Vector4 Size_StartSize;

		DUOLMath::Vector4 Age_LifeTime_Rotation_Gravity;

		DUOLMath::Vector4 StartColor;
		DUOLMath::Vector4 Color;

		DUOLMath::Vector2 TexIndex[4];

		DUOLMath::Vector4 InitEmitterPos;

		DUOLMath::Vector4 PrevPos[30];

		DUOLMath::Vector4 LastestPrevPos;

		float TrailWidth;
		float TrailRecordTime;
		DUOLMath::Vector2 TrailScrollSpeed;
	};


#pragma region Particle Rendering
	/**
		@enum  RenderingData_Particle
		@brief ver_0.3 이제부터 버전을 기록하자. 모듈 단위로 기능을 관리할 계획이다. 이유는 셰이더 코드에서 불필요한 연산을 줄이기 위함.
	**/
	enum class Space
	{
		Local,
		World
	};
	/**
		@struct Particle_CommonInfo
		@brief  커먼인포는 모든 파티클이 사용하는 가장 기본적인 모듈, 무조건 실행시킨다.
	**/
	struct Particle_CommonInfo
	{
		enum class Option_Particle
		{
			Constant,
			Curve, // 커브는 후순위로 미루자..!
			RandomBetweenTwoConstant,
			RandomBetweenTwoCurve // 커브는 후순위로 미루자..!
		};
		Particle_CommonInfo() :
			_useModule(true),
			_firstRun(true),
			_duration(5.0f),
			_looping(true),
			_startDelayOption(Option_Particle::Constant),
			_startDelay{ 0,0 },
			_startLifeTimeOption(Option_Particle::Constant),
			_startLifeTime{ 5.0f,5.0f },
			_startSpeedOption(Option_Particle::Constant),
			_startSpeed{ 4.0f,4.0f },
			_startSizeOption(Option_Particle::Constant),
			_startSize{ DUOLMath::Vector2(1.0f, 1.0f),DUOLMath::Vector2(1.0f, 1.0f) },
			_startRotationOption(Option_Particle::Constant),
			_startRotation{ 0,0 },
			_startColorOption(Option_Particle::Constant),
			_startColor{ DUOLMath::Vector4(1.0f, 1.0f, 1.0f,1.0f),DUOLMath::Vector4(1.0f, 1.0f, 1.0f,1.0f) },
			_gravirtModifierOption(Option_Particle::Constant),
			_gravityModifier{ 0.0f,0.0f },
			_maxParticles(1000),
			_transformMatrix(DUOLMath::Matrix::Identity),
			_simulationSpeed(1.0f),
			_space(Space::World),
			_deltaMatrix(DUOLMath::Matrix::Identity)
		{}
		bool operator==(const Particle_CommonInfo& other) const
		{
			if (
				_useModule != other._useModule ||
				_firstRun != other._firstRun ||
				_duration != other._duration ||
				_looping != other._looping ||
				_startDelayOption != other._startDelayOption ||
				_startDelay[0] != other._startDelay[0] ||
				_startDelay[1] != other._startDelay[1] ||
				_startLifeTimeOption != other._startLifeTimeOption ||
				_startLifeTime[0] != other._startLifeTime[0] ||
				_startLifeTime[1] != other._startLifeTime[1] ||
				_startSpeedOption != other._startSpeedOption ||
				_startSpeed[0] != other._startSpeed[0] ||
				_startSpeed[1] != other._startSpeed[1] ||
				_startSizeOption != other._startSizeOption ||
				_startSize[0] != other._startSize[0] ||
				_startSize[1] != other._startSize[1] ||
				_startRotationOption != other._startRotationOption ||
				_startRotation[0] != other._startRotation[0] ||
				_startRotation[1] != other._startRotation[1] ||
				_startColorOption != other._startColorOption ||
				_startColor[0] != other._startColor[0] ||
				_startColor[1] != other._startColor[1] ||
				_gravirtModifierOption != other._gravirtModifierOption ||
				_gravityModifier[0] != other._gravityModifier[0] ||
				_gravityModifier[1] != other._gravityModifier[1] ||
				_maxParticles != other._maxParticles ||
				_transformMatrix != other._transformMatrix ||
				_simulationSpeed != other._simulationSpeed ||
				_space != other._space ||
				_deltaMatrix != other._deltaMatrix

				)
				return false;
			else
				return true;
		}
		bool _useModule;
		//																					ShaderCode					   ImGui						
		bool _firstRun;					// 시작인가요 ..?										O							O

		float _duration;				// 몇 초 동안 파티클 객체가 재생될 지.						X							O

		bool _looping;					// 반복여부.												X							O

		Option_Particle _startDelayOption;	//													X							X

		float _startDelay[2];				// 몇 초 뒤에 파티클이 재생될 지.						X							O

		Option_Particle _startLifeTimeOption;	//												X							X

		float _startLifeTime[2];				// 한 파티클의 생존 시간.							O							O

		Option_Particle _startSpeedOption;//													X							X

		float _startSpeed[2];				// 파티클 생성시 시작 속도.							O							O

		Option_Particle _startSizeOption;//														X							X

		DUOLMath::Vector2 _startSize[2];				// 파티클의 시작 크기.								O							O

		Option_Particle _startRotationOption;//													X							X

		float _startRotation[2];			// 파티클의 시작 회전.								X							O

		Option_Particle _startColorOption;//													X							X

		DUOLMath::Vector4 _startColor[2];			// 파티클의 시작 색상										X							O

		Option_Particle _gravirtModifierOption;//												X							X

		float _gravityModifier[2];			// 파티클에 가해지는 중력.							O							O

		int _maxParticles;				// 파티클 최대 출력 사이즈.								X							O
		// ContantBuffer 16정렬 후 등록 완료..

		DUOLMath::Matrix _transformMatrix;		// 파티클의 생성 위치 및 각을 조정							X							X

		float _playTime;				// play 시작후 흐른 시간.

		float _simulationSpeed;

		Space _space; // 생성된 파티클이 Mitter의 값에 영향을 받을지 안받을지. 월드면 영향을 안받고 로컬이면 영향을 받는다.

		DUOLMath::Matrix _deltaMatrix;		// 파티클의 생성 위치 및 각을 조정	
	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _useModule;

			ar& _firstRun;

			ar& _duration;

			ar& _looping;

			ar& _startDelayOption;

			ar& _startDelay[0];
			ar& _startDelay[1];

			ar& _startLifeTimeOption;

			ar& _startLifeTime[0];
			ar& _startLifeTime[1];

			ar& _startSpeedOption;

			ar& _startSpeed[0];
			ar& _startSpeed[1];

			ar& _startSizeOption;

			ar& _startSize[0];
			ar& _startSize[1];

			ar& _startRotationOption;

			ar& _startRotation[0];
			ar& _startRotation[1];

			ar& _startColorOption;

			ar& _startColor[0];
			ar& _startColor[1];

			ar& _gravirtModifierOption;

			ar& _gravityModifier[0];
			ar& _gravityModifier[1];

			ar& _maxParticles;

			ar& _transformMatrix;

			ar& _playTime;

			ar& _simulationSpeed;

			ar& _space;

			ar& _deltaMatrix;
		}

	};
	struct Particle_Emission
	{
		Particle_Emission() : _useModule(true),
			_emissiveCount{ 1,1 }, _emissiveTime(0.1f), _emissiveTimer(0),
			_isRateOverDistance(false),
			_emissionOption(Particle_CommonInfo::Option_Particle::Constant)
		{
		}
		bool operator==(const Particle_Emission& other) const
		{
			if (memcmp(this, &other, sizeof(Particle_Emission)) == 0)
				return true;
			else
				return false;
		}
		bool _useModule;

		int _emissiveCount[2];			// 한번에 몇개를 방출 시킬지.

		float _emissiveTime;			// 다음 방출까지 걸리는 시간.

		bool _isRateOverDistance;			// 움직일 때 파티클 방출.

		float _emissiveTimer;			// 방출 타이머.

		Particle_CommonInfo::Option_Particle _emissionOption;

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _useModule;
			ar& _emissiveCount[0];
			ar& _emissiveCount[1];
			ar& _emissiveTime;
			ar& _isRateOverDistance;
			ar& _emissiveTimer;
			ar& _emissionOption;
		}
	};
	struct Particle_Shape
	{
		Particle_Shape() :
			_useModule(true),
			_shape(Shape::Cone),
			_angle(3.141592f / 6.0f), _radius(1.0f), _donutRadius(0), _arc(3.141592f * 2.0f),
			_position(0, 0, 0),
			_rotation(0, 0, 0),
			_scale(1, 1, 1),
			_radiusThickness(1.0f),
			_edgeMode_flags(0),
			_edgeMode(EdgeMode::Random),
			_speed(1.0f),
			_spread(0.0f),
			pad3()
		{
		}
		bool operator==(const Particle_Shape& other) const
		{
			if (memcmp(this, &other, sizeof(Particle_Shape)) == 0)
				return true;
			else
				return false;
		}
		enum class Shape
		{
			Sphere,
			Hemisphere,
			Cone,
			Donut,
			Box,
			Circle,
			Rectangle,
			Edge

		};
		enum class EdgeMode
		{
			Random,
			Loop,
			PingPong,
			BurstSpread
		};
		bool _useModule;

		Shape _shape;

		EdgeMode _edgeMode;

		float _angle;
		float _radius;
		float _donutRadius;
		float _arc;

		DUOLMath::Vector3 _position;
		float _radiusThickness;

		DUOLMath::Vector3 _rotation;
		float pad1;

		DUOLMath::Vector3 _scale;
		float pad2;

		unsigned int _edgeMode_flags;
		float _speed;
		float _spread;
		float pad3;


	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _useModule;

			ar& _shape;

			ar& _edgeMode;

			ar& _angle;
			ar& _radius;
			ar& _donutRadius;
			ar& _arc;

			ar& _position;
			ar& _radiusThickness;

			ar& _rotation;
			ar& pad1;

			ar& _scale;
			ar& pad2;

			ar& _edgeMode_flags;
			ar& _speed;
			ar& _spread;
			ar& pad3;
		}

	};
	struct Particle_Velocity_over_Lifetime
	{
		Particle_Velocity_over_Lifetime() : _useModule(false),
			_linearVelocity(0, 0, 0), _space(Space::Local),
			_orbital(0, 0, 0), _offset(0, 0, 0), _isGravity(1)
		{
		}
		bool operator==(const Particle_Velocity_over_Lifetime& other) const
		{
			if (memcmp(this, &other, sizeof(Particle_Velocity_over_Lifetime)) == 0)
				return true;
			else
				return false;
		}
		bool _useModule;

		DUOLMath::Vector3 _linearVelocity;
		Space _space;

		DUOLMath::Vector3 _orbital;
		unsigned int _isGravity;

		DUOLMath::Vector3 _offset;
		float pad2;


	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _useModule;

			ar& _linearVelocity;
			ar& _space;

			ar& _orbital;
			ar& _isGravity;

			ar& _offset;
			ar& pad2;
		}
	};
	struct Particle_Limit_Velocity_Over_Lifetime
	{
		Particle_Limit_Velocity_Over_Lifetime() : _useModule(false),
			_pointA(0, 0),
			_pointB(0.3333f, 0.3333f),
			_pointC(0.6666f, 0.6666f),
			_pointD(1.0f, 1.0f),
			_speed(1.0f),
			_dampen(1.0f)
		{
		}
		bool operator==(const Particle_Limit_Velocity_Over_Lifetime& other) const
		{
			if (memcmp(this, &other, sizeof(Particle_Limit_Velocity_Over_Lifetime)) == 0)
				return true;
			else
				return false;
		}
		bool _useModule;

		DUOLMath::Vector2 _pointA;

		DUOLMath::Vector2 _pointB;

		DUOLMath::Vector2 _pointC;

		DUOLMath::Vector2 _pointD;

		float _speed;
		float _dampen;
		DUOLMath::Vector2 pad;
	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _useModule;

			ar& _pointA;

			ar& _pointB;

			ar& _pointC;

			ar& _pointD;

			ar& _speed;

			ar& _dampen;

			ar& pad;
		}
	};
	struct Particle_Force_over_LifeTime
	{
		Particle_Force_over_LifeTime() :_useModule(false), _force(0, 0, 0),
			_space(Space::Local)
		{
		}
		bool operator==(const Particle_Force_over_LifeTime& other) const
		{
			if (memcmp(this, &other, sizeof(Particle_Velocity_over_Lifetime)) == 0)
				return true;
			else
				return false;
		}
		bool _useModule;

		DUOLMath::Vector3 _force;

		Space _space;

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _useModule;

			ar& _force;

			ar& _space;
		}
	};
	struct Particle_Color_over_Lifetime
	{
		/**
			@brief Cpu에서 보간 값을 미리 정해서 던지지 않는 이유는 파티클마다 라이프타임이 다르기 때문이다.
			gpu 안에서 계산할 수 있는 방법을 생각해보자.
		**/
		Particle_Color_over_Lifetime() : _useModule(false)
		{
			for (int i = 0; i < 8; i++)
			{
				_alpha_Ratio[i] = DUOLMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
				_color_Ratio[i] = DUOLMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			}


		}
		bool operator==(const Particle_Color_over_Lifetime& other) const
		{
			for (int i = 0; i < 8; i++)
			{
				if (_alpha_Ratio[i] != other._alpha_Ratio[i])
					return false;

				if (_alpha_Ratio[i] != other._alpha_Ratio[i])
					return false;

			}
			if (_useModule != other._useModule)
				return false;
			return true;
		}
		bool _useModule;

		DUOLMath::Vector4 _alpha_Ratio[8];

		DUOLMath::Vector4 _color_Ratio[8];

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _useModule;

			ar& _alpha_Ratio[0];
			ar& _alpha_Ratio[1];
			ar& _alpha_Ratio[2];
			ar& _alpha_Ratio[3];
			ar& _alpha_Ratio[4];
			ar& _alpha_Ratio[5];
			ar& _alpha_Ratio[6];
			ar& _alpha_Ratio[7];

			ar& _color_Ratio[0];
			ar& _color_Ratio[1];
			ar& _color_Ratio[2];
			ar& _color_Ratio[3];
			ar& _color_Ratio[4];
			ar& _color_Ratio[5];
			ar& _color_Ratio[6];
			ar& _color_Ratio[7];
		}
	};
	struct Particle_Size_Over_Lifetime
	{
		Particle_Size_Over_Lifetime() :
			_useModule(false),
			_pointA(0, 0),
			_pointB(0.333f, 0.333f),
			_pointC(0.666f, 0.666f),
			_pointD(1.0f, 1.0f)
		{
		}
		bool operator==(const Particle_Size_Over_Lifetime& other) const
		{
			if (memcmp(this, &other, sizeof(Particle_Size_Over_Lifetime)) == 0)
				return true;
			else
				return false;
		}
		bool _useModule;

		DUOLMath::Vector2 _pointA;
		DUOLMath::Vector2 _pointB;
		DUOLMath::Vector2 _pointC;
		DUOLMath::Vector2 _pointD;

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _useModule;

			ar& _pointA;

			ar& _pointB;

			ar& _pointC;

			ar& _pointD;
		}
	};
	struct Particle_Rotation_Over_Lifetime
	{
		Particle_Rotation_Over_Lifetime() :
			_useModule(false),
			_AngularVelocity(0)
		{
		}
		bool operator==(const Particle_Rotation_Over_Lifetime& other) const
		{
			if (memcmp(this, &other, sizeof(Particle_Rotation_Over_Lifetime)) == 0)
				return true;
			else
				return false;
		}
		bool _useModule;

		float _AngularVelocity;

		DUOLMath::Vector3 pad0;

		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _useModule;

			ar& _AngularVelocity;

			ar& pad0;
		}
	};
	struct Particle_Texture_Sheet_Animation
	{
		Particle_Texture_Sheet_Animation() :
			_useModule(false),
			_grid_XY{ 1, 1 },
			_timeMode(0)
		{
		}
		bool operator==(const Particle_Texture_Sheet_Animation& other) const
		{
			if (memcmp(this, &other, sizeof(Particle_Texture_Sheet_Animation)) == 0)
				return true;
			else
				return false;
		}
		bool _useModule;

		int _grid_XY[2];

		int _timeMode;

		float pad0;

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _useModule;

			ar& _grid_XY[0];

			ar& _grid_XY[1];

			ar& _timeMode;

			ar& pad0;
		}
	};
	struct Particle_Noise
	{
		enum class Quality
		{
			Low,
			Middle,
			High
		};
		Particle_Noise() : _useModule(false),
			_strength(1.0f), _frequency(0.5f), _scrollSpeed(0), _damping(true), _octaves(1),
			_octaveMultiplier(0.5), _octaveScale(2), _quality(Quality::High), _positionAmount(1), _rotationAmount(0),
			_sizeAmount(0)
		{}
		bool operator==(const Particle_Noise& other) const
		{
			if (memcmp(this, &other, sizeof(Particle_Noise)) == 0)
				return true;
			else
				return false;
		}
		bool _useModule;

		float _strength;
		float _frequency;
		float _scrollSpeed;
		bool _damping;
		int _octaves;
		float _octaveMultiplier;
		float _octaveScale;
		Quality _quality;

		float _positionAmount;
		float _rotationAmount;
		float _sizeAmount;

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _useModule;
			ar& _strength;
			ar& _frequency;
			ar& _scrollSpeed;
			ar& _damping;
			ar& _octaves;
			ar& _octaveMultiplier;
			ar& _octaveScale;
			ar& _quality;

			ar& _positionAmount;
			ar& _rotationAmount;
			ar& _sizeAmount;
		}
	};
	struct Particle_Collision
	{
		Particle_Collision() : _useModule(false),
			_planeCount(0), _boundce(1), _lifeTimeLoss(0)
		{

			for (int i = 0; i < 8; i++)
			{
				_planePos[i] = DUOLMath::Vector4();
				_planeNormalVec[i] = DUOLMath::Vector4(0, 1, 0, 0);
			}
		}
		bool operator==(const Particle_Collision& other) const
		{
			if (memcmp(this, &other, sizeof(Particle_Collision)) == 0)
				return true;
			else
				return false;
		}

		bool _useModule;

		int _planeCount; // 최대 갯수를 정해놓자. 8개? 이유는 팔면체를 생각했기 때문.

		float _boundce;

		float _lifeTimeLoss;

		float _pad;

		DUOLMath::Vector4 _planePos[8]; // 어째서 Vector4인가! 16바이트 정렬을 편하게 하기 위함.

		DUOLMath::Vector4 _planeNormalVec[8];


	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _useModule;

			ar& _planeCount;

			ar& _boundce;

			ar& _lifeTimeLoss;

			ar& _pad;

			ar& _planePos[0];
			ar& _planePos[1];
			ar& _planePos[2];
			ar& _planePos[3];
			ar& _planePos[4];
			ar& _planePos[5];
			ar& _planePos[6];
			ar& _planePos[7];

			ar& _planeNormalVec[0];
			ar& _planeNormalVec[1];
			ar& _planeNormalVec[2];
			ar& _planeNormalVec[3];
			ar& _planeNormalVec[4];
			ar& _planeNormalVec[5];
			ar& _planeNormalVec[6];
			ar& _planeNormalVec[7];
		}
	};
	struct Particle_Trails
	{
		Particle_Trails() :_useModule(false), _ratio(1.0f), _lifeTime(1.0f), _minimumVertexDistance(0.1f),
			_worldSpace(false), _dieWithParticle(false), _textureMode(TextureMode::Stretch),
			_sizeAffectsWidth(false), _sizeAffectsLifeTime(false),
			_inheritParticleColor(true), _widthOverTrail{ 1.0f,1.0f },
			_generateLightingData(false),
			_shadowBias(0), _trailVertexCount(15),
			_widthModifierOption(Particle_CommonInfo::Option_Particle::Constant),
			_scrollXSpeed{ 0,0 }, _scrollYSpeed{ 0,0 },
			_condition(Conditional::And), _recordTime(0),
			_scrollModifierOption(Particle_CommonInfo::Option_Particle::Constant)
		{
			for (int i = 0; i < 8; i++)
			{
				_alpha_Ratio_Lifetime[i] = DUOLMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
				_color_Ratio_Lifetime[i] = DUOLMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

				_alpha_Ratio_Trail[i] = DUOLMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
				_color_Ratio_Trail[i] = DUOLMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			}

		}
		bool operator==(const Particle_Trails& other) const
		{
			if (memcmp(this, &other, sizeof(Particle_Trails)) == 0)
				return true;
			else
				return false;
		}
		enum class TextureMode
		{
			Stretch,
			Tile,
			DistributePerSegment,
			RepeatPerSegment

		};
		enum class Conditional
		{
			And,
			Or
		};
		bool _useModule;

		float _ratio; // 0 ~ 1 값
		float _lifeTime; // 0 ~ 1 값
		float _minimumVertexDistance;
		bool _worldSpace;

		bool _dieWithParticle;
		TextureMode _textureMode;
		bool _sizeAffectsWidth;
		bool _sizeAffectsLifeTime;

		bool _inheritParticleColor;
		DUOLMath::Vector4 _alpha_Ratio_Lifetime[8];
		DUOLMath::Vector4 _color_Ratio_Lifetime[8];

		Particle_CommonInfo::Option_Particle _widthModifierOption;
		float _widthOverTrail[2];

		DUOLMath::Vector4 _alpha_Ratio_Trail[8];
		DUOLMath::Vector4 _color_Ratio_Trail[8];

		bool _generateLightingData;
		float _shadowBias;

		int _trailVertexCount;

		Particle_CommonInfo::Option_Particle _scrollModifierOption;
		float _scrollXSpeed[2];
		float _scrollYSpeed[2];

		Conditional _condition;
		float _recordTime;

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _useModule;

			ar& _ratio;
			ar& _lifeTime;
			ar& _minimumVertexDistance;
			ar& _worldSpace;

			ar& _dieWithParticle;
			ar& _textureMode;
			ar& _sizeAffectsWidth;
			ar& _sizeAffectsLifeTime;

			ar& _inheritParticleColor;

			ar& _alpha_Ratio_Lifetime[0];
			ar& _alpha_Ratio_Lifetime[1];
			ar& _alpha_Ratio_Lifetime[2];
			ar& _alpha_Ratio_Lifetime[3];
			ar& _alpha_Ratio_Lifetime[4];
			ar& _alpha_Ratio_Lifetime[5];
			ar& _alpha_Ratio_Lifetime[6];
			ar& _alpha_Ratio_Lifetime[7];

			ar& _color_Ratio_Lifetime[0];
			ar& _color_Ratio_Lifetime[1];
			ar& _color_Ratio_Lifetime[2];
			ar& _color_Ratio_Lifetime[3];
			ar& _color_Ratio_Lifetime[4];
			ar& _color_Ratio_Lifetime[5];
			ar& _color_Ratio_Lifetime[6];
			ar& _color_Ratio_Lifetime[7];

			ar& _widthModifierOption;

			ar& _widthOverTrail[0];
			ar& _widthOverTrail[1];

			ar& _alpha_Ratio_Trail[0];
			ar& _alpha_Ratio_Trail[1];
			ar& _alpha_Ratio_Trail[2];
			ar& _alpha_Ratio_Trail[3];
			ar& _alpha_Ratio_Trail[4];
			ar& _alpha_Ratio_Trail[5];
			ar& _alpha_Ratio_Trail[6];
			ar& _alpha_Ratio_Trail[7];

			ar& _color_Ratio_Trail[0];
			ar& _color_Ratio_Trail[1];
			ar& _color_Ratio_Trail[2];
			ar& _color_Ratio_Trail[3];
			ar& _color_Ratio_Trail[4];
			ar& _color_Ratio_Trail[5];
			ar& _color_Ratio_Trail[6];
			ar& _color_Ratio_Trail[7];

			ar& _generateLightingData;
			ar& _shadowBias;

			ar& _trailVertexCount;

			ar& _scrollModifierOption;
			ar& _scrollXSpeed[0];
			ar& _scrollXSpeed[1];

			ar& _scrollYSpeed[0];
			ar& _scrollYSpeed[1];
			ar& _condition;
			ar& _recordTime;
		}

	};
	struct Particle_Renderer
	{
		Particle_Renderer() :_useModule(true),
			_renderMode(RenderMode::Billboard), _blendState(BlendState::OIT_Default),
			_meshName(_T("")), _texturePath(_T("")), _traillTexturePath(_T("")),
			_renderAlignment(RenderAlignment::View), _speedScale(0), _lengthScale(2)
		{
		}
		bool operator==(const Particle_Renderer& other) const
		{
			if (
				_useModule != other._useModule ||
				_renderMode != other._renderMode ||
				_blendState != other._blendState ||
				_meshName != other._meshName ||
				_texturePath != other._texturePath ||
				_traillTexturePath != other._traillTexturePath ||
				_renderAlignment != other._renderAlignment ||
				_speedScale != other._speedScale ||
				_lengthScale != other._lengthScale ||
				_masking != other._masking
				)
				return false;
			return true;
		}
		enum class RenderMode
		{
			Billboard,
			StretchedBillboard,
			HorizontalBillboard,
			VerticalBillboard,
			Mesh,
			None
		};
		enum class BlendState
		{
			OIT_Default,
			OIT_Additive,
			Foward
		};
		enum class RenderAlignment
		{
			View,
			World,
			Local,
			Velocity
		};
		enum class Masking
		{
			NoMasking,
			VisibleInsideMask,
			OutsideMask
		};
		bool _useModule;

		RenderMode _renderMode;

		BlendState _blendState;

		DUOLCommon::tstring _meshName;

		DUOLCommon::tstring _texturePath;			// 파티클 이펙트가 텍스쳐의 주소.

		DUOLCommon::tstring _traillTexturePath;			// 파티클 이펙트가 텍스쳐의 주소.

		RenderAlignment _renderAlignment;

		float _speedScale;

		float _lengthScale;

		Masking _masking;

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _useModule;

			ar& _renderMode;

			ar& _blendState;

			ar& _meshName;

			ar& _texturePath;

			ar& _traillTexturePath;

			ar& _renderAlignment;

			ar& _speedScale;

			ar& _lengthScale;

			ar& _masking;
		}
	};
	// 파티클 시스템을 사용하기 위한 인터페이스
	// 한 개의 입자 시스템을 묘사한다.
	enum class RASTERIZER_STATE
	{
		SOLID = 0,
		WIREFRAME = 1,
	};

	struct RenderingData_Particle
	{
		RenderingData_Particle() :
			_commonInfo(),
			_emission(),
			_color_Over_Lifetime(),
			_velocity_Over_Lifetime(),
			_size_Over_Lifetime(),
			_rotation_Over_Lifetime(),
			_texture_Sheet_Animaition(),
			_objectID(0),
			_isDelete(false),
			_childrenIDs()
			, shaderName(),
			_rasterizerState(RASTERIZER_STATE::SOLID),
			_particleCount(0)
		{}
		bool operator==(DUOLGraphicsEngine::RenderingData_Particle& other)
		{
			if (this->_commonInfo != other._commonInfo ||
				this->_emission != other._emission ||
				this->_shape != other._shape ||
				this->_velocity_Over_Lifetime != other._velocity_Over_Lifetime ||
				this->_force_Over_Lifetime != other._force_Over_Lifetime ||
				this->_color_Over_Lifetime != other._color_Over_Lifetime ||
				this->_size_Over_Lifetime != other._size_Over_Lifetime ||
				this->_rotation_Over_Lifetime != other._rotation_Over_Lifetime ||
				this->_texture_Sheet_Animaition != other._texture_Sheet_Animaition ||
				this->_noise != other._noise ||
				this->_collision != other._collision ||
				this->_trails != other._trails ||
				this->_renderer != other._renderer
				)
				return false;
			else
				return true;
		}
		//------------------------------------------------ 모듈 -------------------------------------------
		Particle_CommonInfo _commonInfo;

		Particle_Emission _emission;

		Particle_Shape _shape;

		Particle_Velocity_over_Lifetime _velocity_Over_Lifetime;

		Particle_Limit_Velocity_Over_Lifetime _limit_Velocity_Over_Lifetime;

		Particle_Force_over_LifeTime _force_Over_Lifetime;

		Particle_Color_over_Lifetime _color_Over_Lifetime;

		Particle_Size_Over_Lifetime _size_Over_Lifetime;

		Particle_Rotation_Over_Lifetime _rotation_Over_Lifetime;

		Particle_Noise _noise;

		Particle_Collision _collision;

		Particle_Texture_Sheet_Animation _texture_Sheet_Animaition;

		Particle_Trails _trails;

		Particle_Renderer _renderer;
		//------------------------------------------------ 모듈 -------------------------------------------

		unsigned int _objectID; // 파티클 ID 리소스 매니저에 맵핑한 아이디, 오브젝트 ID로 사용하자.

		std::vector<std::string> shaderName; // 어떤 쉐이더를 사용하는지.

		std::vector<unsigned int> _childrenIDs;

		bool _isDelete; // 파티클을 다 사용했으면 할당 해제 하기 위함. 파티클을 내부에서 오브젝트 풀 등으로 관리 안하는 이유는 파티클마다 버퍼의 크기가 다르기 때문이다.

		RASTERIZER_STATE _rasterizerState;

		unsigned int _particleCount; // 현재 방출된 파티클의 숫자.
		/**
		* \brief 여기 값을 지표로
		* \brief RenderingData, Common.hlsli 를 수정하도록 하자..!
		*/
		unsigned int GetFlag()
		{
			unsigned int flag = 0;

			if (_commonInfo._useModule) flag |= 1 << 0;
			if (_emission._useModule) flag |= 1 << 1;
			if (_shape._useModule) flag |= 1 << 2;
			if (_velocity_Over_Lifetime._useModule) flag |= 1 << 3;
			if (_force_Over_Lifetime._useModule) flag |= 1 << 4;
			if (_color_Over_Lifetime._useModule) flag |= 1 << 5;
			if (_size_Over_Lifetime._useModule) flag |= 1 << 6;
			if (_rotation_Over_Lifetime._useModule) flag |= 1 << 7;
			if (_noise._useModule) flag |= 1 << 8;
			if (_collision._useModule) flag |= 1 << 9;
			if (_texture_Sheet_Animaition._useModule) flag |= 1 << 10;
			if (_trails._useModule) flag |= 1 << 11;
			if (_renderer._useModule) flag |= 1 << 12;

			if (_shape._shape == Particle_Shape::Shape::Sphere) flag |= 1 << 13;
			if (_shape._shape == Particle_Shape::Shape::Hemisphere) flag |= 1 << 14;
			if (_shape._shape == Particle_Shape::Shape::Cone) flag |= 1 << 15;
			if (_shape._shape == Particle_Shape::Shape::Donut) flag |= 1 << 16;
			if (_shape._shape == Particle_Shape::Shape::Box) flag |= 1 << 17;
			if (_shape._shape == Particle_Shape::Shape::Circle) flag |= 1 << 18;
			if (_shape._shape == Particle_Shape::Shape::Rectangle) flag |= 1 << 19;

			if (_renderer._renderMode == Particle_Renderer::RenderMode::Billboard) flag |= 1 << 20;
			if (_renderer._renderMode == Particle_Renderer::RenderMode::StretchedBillboard) flag |= 1 << 21;
			if (_renderer._renderMode == Particle_Renderer::RenderMode::HorizontalBillboard) flag |= 1 << 22;
			if (_renderer._renderMode == Particle_Renderer::RenderMode::VerticalBillboard) flag |= 1 << 23;
			if (_renderer._renderMode == Particle_Renderer::RenderMode::Mesh) flag |= 1 << 24;

			if (_commonInfo._space == Space::World) flag |= 1 << 25;

			if (_limit_Velocity_Over_Lifetime._useModule) flag |= 1 << 26;

			if (_renderer._renderMode == Particle_Renderer::RenderMode::None) flag |= 1 << 27;

			// 28 번 FirstRun으로 사용 중.
			return flag;
		}

	protected:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& _commonInfo;

			ar& _emission;

			ar& _shape;

			ar& _velocity_Over_Lifetime;

			ar& _limit_Velocity_Over_Lifetime;

			ar& _force_Over_Lifetime;

			ar& _color_Over_Lifetime;

			ar& _size_Over_Lifetime;

			ar& _rotation_Over_Lifetime;

			ar& _noise;

			ar& _collision;

			ar& _texture_Sheet_Animaition;

			ar& _trails;

			ar& _renderer;

			ar& _objectID;

			ar& shaderName;

			ar& _childrenIDs;

			ar& _isDelete;

			ar& _rasterizerState;

			ar& _particleCount;
		}
	};

	BOOST_SERIALIZATION_ASSUME_ABSTRACT(Particle_CommonInfo);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(Particle_Emission);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(Particle_Shape);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(Particle_Velocity_over_Lifetime);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(Particle_Limit_Velocity_Over_Lifetime);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(Particle_Force_over_LifeTime);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(Particle_Color_over_Lifetime);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(Particle_Size_Over_Lifetime);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(Particle_Rotation_Over_Lifetime);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(Particle_Noise);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(Particle_Collision);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(Particle_Texture_Sheet_Animation);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(Particle_Trails);
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(Particle_Renderer);

#pragma endregion
	namespace ConstantBuffDesc
	{
		__declspec(align(16)) struct CB_DynamicBuffer
		{
			CB_DynamicBuffer() : g_EmiitionTime(0), g_dim(0), pad1(0), pad2(0)
			{

			}
			float g_EmiitionTime;
			int g_dim;
			float pad1;
			float pad2;
		};
		__declspec(align(16)) struct CommonInfo // 0~8 36
		{
			CommonInfo() = default;
			CommonInfo(Particle_CommonInfo& renderingData)
			{
				memcpy(&gDeltaMatrix, &renderingData._deltaMatrix, sizeof(DUOLMath::Matrix));

				memcpy(gStartDelay, renderingData._startDelay, sizeof(gStartDelay));
				memcpy(gStartLifeTime, renderingData._startLifeTime, sizeof(gStartLifeTime));

				memcpy(gStartSpeed, renderingData._startSpeed, sizeof(gStartSpeed));
				memcpy(gStartRotation, renderingData._startRotation, sizeof(gStartRotation));

				memcpy(gStartSize, renderingData._startSize, sizeof(gStartSize));

				memcpy(gStartColor, renderingData._startColor, sizeof(gStartColor));

				memcpy(gGravityModifier, renderingData._gravityModifier, sizeof(gGravityModifier));
				gParticlePlayTime = renderingData._playTime; // 게임 시간

				gisLooping = static_cast<int>(renderingData._looping);
				gSimulationSpeed = renderingData._simulationSpeed;
				gDuration = renderingData._duration;
				gMaxParticles = renderingData._maxParticles;

			}
			DUOLMath::Matrix gTransformMatrix;
			DUOLMath::Matrix gDeltaMatrix;

			float	gStartDelay[2];				// 몇 초 뒤에 파티클이 재생될 지.					
			float	gStartLifeTime[2];				// 한 파티클의 생존 시간.						

			float	gStartSpeed[2];				// 파티클 생성시 시작 속도.
			float	gStartRotation[2];			// 파티클의 시작 회전.							

			float gStartSize[4];				// 파티클의 시작 크기.							

			DUOLMath::Vector4 gStartColor[2];			// 파티클의 시작 색상									

			float	gGravityModifier[2];			// 파티클에 가해지는 중력.						
			float gParticlePlayTime;
			float	gObjectID;			// 파티클이 가지고 있는 오브젝트 ID.						

			int	gisLooping;					// 반복여부.
			float gSimulationSpeed;
			float	gDuration;				// 몇 초 동안 파티클 객체가 재생될 지.					
			int		gMaxParticles;				// 파티클 최대 출력 사이즈.							

		};
		__declspec(align(16)) struct Emission // 9
		{
			Emission() = default;
			Emission(Particle_Emission& _renderingData)
			{
				gEmissiveCount = rand() % (abs(_renderingData._emissiveCount[1] - _renderingData._emissiveCount[0]) + 1) + _renderingData._emissiveCount[0];

				gEmissiveTime = _renderingData._emissiveTime;

				if (_renderingData._isRateOverDistance)
					gIsRateOverDistance = 1;
				else
					gIsRateOverDistance = 0;
			}
			int	gEmissiveCount;			// 한번에 몇개를 방출 시킬지.
			float	gEmissiveTime;			// 다음 방출까지 걸리는 시간.
			int	gIsRateOverDistance;			// 다음 방출까지 걸리는 시간.
			float pad;
		};
		__declspec(align(16)) struct Shape // 10 ~ 13
		{
			Shape() = default;
			Shape(Particle_Shape& _renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&_renderingData) + 3, sizeof(Particle_Shape) - sizeof(int) * 3);

				_edgeMode |= 1 << static_cast<unsigned int>(_renderingData._edgeMode);

				if (gRadius == 0)
					gRadius = 0.000001f;
			}

			float gAngle;
			float gRadius;
			float gDonutRadius;
			float gArc;

			DUOLMath::Vector3 gPosition;
			float _radiusThickness;

			DUOLMath::Vector3 gRotation;
			float pad1;

			DUOLMath::Vector3 gScale;
			float pad2;

			int _edgeMode;
			float _speed;
			float _spread;
			float pad3;
		};
		__declspec(align(16)) struct Velocity_over_Lifetime // 14 // 4
		{
			Velocity_over_Lifetime() = default;
			Velocity_over_Lifetime(Particle_Velocity_over_Lifetime& _renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&_renderingData) + 1, sizeof(Particle_Velocity_over_Lifetime) - sizeof(int));
			}
			DUOLMath::Vector3 gVelocity;
			float pad;
			DUOLMath::Vector3 gOrbital;
			unsigned int gIsGravity;
			DUOLMath::Vector3 gOffset;
			float pad3;
		};

		__declspec(align(16)) struct Limit_Velocity_Over_Lifetime // 14 // 4
		{
			Limit_Velocity_Over_Lifetime() = default;
			Limit_Velocity_Over_Lifetime(Particle_Limit_Velocity_Over_Lifetime& _renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&_renderingData) + 1, sizeof(Particle_Limit_Velocity_Over_Lifetime) - sizeof(int));
			}
			DUOLMath::Vector2 gPointA;

			DUOLMath::Vector2 gPointB;

			DUOLMath::Vector2 gPointC;

			DUOLMath::Vector2 gPointD;

			float gSpeed;

			float gDampen;

			DUOLMath::Vector2 pad;
		};

		__declspec(align(16)) struct Force_over_LifeTime // 15 // 4
		{
			Force_over_LifeTime() = default;
			Force_over_LifeTime(Particle_Force_over_LifeTime& _renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&_renderingData) + 1, sizeof(Particle_Force_over_LifeTime) - sizeof(int));
			}
			DUOLMath::Vector3 gForce;
			float pad;
		};
		__declspec(align(16)) struct Color_over_Lifetime //
		{
			Color_over_Lifetime() = default;
			Color_over_Lifetime(Particle_Color_over_Lifetime& renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&renderingData) + 1, sizeof(Particle_Color_over_Lifetime) - sizeof(int));
			}

			DUOLMath::Vector4 gAlpha_Ratio[8];

			DUOLMath::Vector4 gColor_Ratio[8];
		};
		__declspec(align(16)) struct Size_Over_Lifetime
		{
			Size_Over_Lifetime() = default;
			Size_Over_Lifetime(Particle_Size_Over_Lifetime& _renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&_renderingData) + 1, sizeof(Particle_Size_Over_Lifetime) - sizeof(int));
			}
			DUOLMath::Vector2 gPointA;
			DUOLMath::Vector2 gPointB;
			DUOLMath::Vector2 gPointC;
			DUOLMath::Vector2 gPointD;
		};
		__declspec(align(16)) struct Rotation_Over_Lifetime
		{
			Rotation_Over_Lifetime() = default;
			Rotation_Over_Lifetime(Particle_Rotation_Over_Lifetime& _renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&_renderingData) + 1, sizeof(Particle_Rotation_Over_Lifetime) - sizeof(int));
			}

			float gAngularVelocity;

			DUOLMath::Vector3 pad;
		};
		__declspec(align(16)) struct Texture_Sheet_Animation // 30
		{
			Texture_Sheet_Animation() = default;
			Texture_Sheet_Animation(Particle_Texture_Sheet_Animation& _renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&_renderingData) + 1, sizeof(Particle_Texture_Sheet_Animation) - sizeof(int));
			}
			int gGrid_XY[2];
			int gTimeMode;
			float pad;
		};
		__declspec(align(16)) struct Noise
		{
			Noise() = default;
			Noise(Particle_Noise& _renderingData)
			{
				gStregth = _renderingData._strength;
				gScrollSpeed = _renderingData._scrollSpeed;
				gPositionAmount = _renderingData._positionAmount;
				gRotationAmount = _renderingData._rotationAmount;

				gSizeAmount = _renderingData._sizeAmount;
			}
			float gStregth;
			float gScrollSpeed;
			float gPositionAmount;
			float gRotationAmount;

			float gSizeAmount;
			DUOLMath::Vector3 pad;
		};
		__declspec(align(16)) struct Collision
		{
			Collision() = default;
			Collision(Particle_Collision& _renderingData)
			{
				memcpy(this, reinterpret_cast<int*>(&_renderingData) + 1, sizeof(Particle_Collision) - sizeof(int));

				if (_renderingData._useModule)
				{
					for (int i = 0; i < gPlaneCount; i++)
					{
						gPlanNormalVec[i].Normalize();
					}
				}

			}
			int gPlaneCount;
			float gBoundce;
			float gLifeTimeLoss;
			float pad;

			DUOLMath::Vector4 gPlanePosition[8];
			DUOLMath::Vector4 gPlanNormalVec[8];
		};
		__declspec(align(16)) struct Trails
		{
			Trails() = default;
			Trails(Particle_Trails& _renderingData)
			{
				gRatio = _renderingData._ratio;
				gLifeTime = _renderingData._lifeTime;
				gMinimumVertexDistance = _renderingData._minimumVertexDistance;
				gWidthOverTrail[0] = _renderingData._widthOverTrail[0];
				gWidthOverTrail[1] = _renderingData._widthOverTrail[1];
				gTrailVertexCount = _renderingData._trailVertexCount;

				gTrailsFlag = 0;
				if (_renderingData._worldSpace)
					gTrailsFlag |= 1 << 0;
				if (_renderingData._dieWithParticle)
					gTrailsFlag |= 1 << 1;
				if (_renderingData._sizeAffectsWidth) // o
					gTrailsFlag |= 1 << 2;
				if (_renderingData._sizeAffectsLifeTime) // o
					gTrailsFlag |= 1 << 3;
				if (_renderingData._inheritParticleColor) // o
					gTrailsFlag |= 1 << 4;
				if (_renderingData._generateLightingData)
					gTrailsFlag |= 1 << 5;

				if (_renderingData._textureMode == Particle_Trails::TextureMode::Stretch)
					gTrailsFlag |= 1 << 6;
				if (_renderingData._textureMode == Particle_Trails::TextureMode::Tile)
					gTrailsFlag |= 1 << 7;
				if (_renderingData._textureMode == Particle_Trails::TextureMode::DistributePerSegment)
					gTrailsFlag |= 1 << 8;
				if (_renderingData._textureMode == Particle_Trails::TextureMode::RepeatPerSegment)
					gTrailsFlag |= 1 << 9;

				for (int i = 0; i < 8; i++)
				{
					gAlpha_Ratio_Lifetime[i] = _renderingData._alpha_Ratio_Lifetime[i];
					gColor_Ratio_Lifetime[i] = _renderingData._color_Ratio_Lifetime[i];
					gAlpha_Ratio_Trail[i] = _renderingData._alpha_Ratio_Trail[i];
					gColor_Ratio_Trail[i] = _renderingData._color_Ratio_Trail[i];
				}

				gScrollXSpeed.x = _renderingData._scrollXSpeed[0];
				gScrollXSpeed.y = _renderingData._scrollXSpeed[1];
				gScrollYSpeed.x = _renderingData._scrollYSpeed[0];
				gScrollYSpeed.y = _renderingData._scrollYSpeed[1];

				gCondition = 0;
				gCondition |= 1 << static_cast<int>(_renderingData._condition);
				gRecordTime = _renderingData._recordTime;
			}
			float gRatio; // o
			float gLifeTime; // o
			float gMinimumVertexDistance; // o
			int pad;

			int gTrailsFlag;
			int gTrailVertexCount;
			float gWidthOverTrail[2]; // o

			DUOLMath::Vector4 gAlpha_Ratio_Lifetime[8]; // o
			DUOLMath::Vector4 gColor_Ratio_Lifetime[8]; // o
			DUOLMath::Vector4 gAlpha_Ratio_Trail[8]; // o
			DUOLMath::Vector4 gColor_Ratio_Trail[8]; // o

			DUOLMath::Vector2 gScrollXSpeed;
			DUOLMath::Vector2 gScrollYSpeed;

			unsigned int gCondition;
			float gRecordTime;
			DUOLMath::Vector2 pad123;
		};
		__declspec(align(16)) struct paticle_Renderer
		{
			paticle_Renderer() = default;
			paticle_Renderer(Particle_Renderer& _renderingData)
			{
				gSpeedScale = _renderingData._speedScale;

				gLengthScale = _renderingData._lengthScale;

				gBlendType = static_cast<unsigned int>(_renderingData._blendState);

				gRenderAlignment = 0;

				gRenderAlignment |= 1 << static_cast<unsigned int>(_renderingData._renderAlignment);
			}
			float gSpeedScale;
			float gLengthScale;
			unsigned int gBlendType;
			unsigned int gRenderAlignment;
		};
		/**
		 * \brief 오브젝트마다 공통되는 contant 버퍼 구조체, 수정할 때 항상 쉐이더 코드도 같이 수정하자. 16 바이트 정렬 잊지말자.
		 */
		__declspec(align(16)) struct CB_PerObject
		{
			DUOLMath::Matrix worldViewProj;

			DUOLMath::Matrix gWorld;

			DUOLMath::Matrix gWorldInvTranspose;

			DUOLMath::Vector4 gObjectID;

			DUOLMath::Vector4 gColor;

			DUOLMath::Vector4 gMetalicRoughnessAoSpecular;

		};

		__declspec(align(16)) struct CB_PerObject_Particle
		{
			CB_PerObject_Particle()
			{

			}

			CB_PerObject_Particle(RenderingData_Particle& renderingData);

			CommonInfo _commonInfo;

			Emission _emission;

			Shape _shape;

			Velocity_over_Lifetime _velocityoverLifetime;

			Limit_Velocity_Over_Lifetime _limitVelocityOverLifetime;

			Force_over_LifeTime _forceoverLifetime;

			Color_over_Lifetime _coloroverLifetime;

			Size_Over_Lifetime _sizeoverLifetime;

			Rotation_Over_Lifetime _rotationoverLifetime;

			Noise _noise;

			Collision _collision;

			Texture_Sheet_Animation _textureSheetAnimation;

			Trails _trails;

			paticle_Renderer _renderer;

			unsigned int _flag;
			float _EmissionTime;
			float _dim;
			float Pad;
		};

		inline CB_PerObject_Particle::CB_PerObject_Particle(RenderingData_Particle& renderingData) :
			_commonInfo(renderingData._commonInfo),
			_emission(renderingData._emission),
			_shape(renderingData._shape),
			_velocityoverLifetime(renderingData._velocity_Over_Lifetime),
			_limitVelocityOverLifetime(renderingData._limit_Velocity_Over_Lifetime),
			_forceoverLifetime(renderingData._force_Over_Lifetime),
			_coloroverLifetime(renderingData._color_Over_Lifetime),
			_sizeoverLifetime(renderingData._size_Over_Lifetime),
			_rotationoverLifetime(renderingData._rotation_Over_Lifetime),
			_textureSheetAnimation(renderingData._texture_Sheet_Animaition),
			_trails(renderingData._trails),
			_renderer(renderingData._renderer),
			_noise(renderingData._noise),
			_collision(renderingData._collision),
			_EmissionTime(0),
			_dim(0)
			//_renderer()
		{

			_flag = renderingData.GetFlag();

			memcpy(&_commonInfo.gObjectID, &renderingData._objectID, sizeof(UINT));
			{
				DUOLMath::Matrix world = renderingData._commonInfo._transformMatrix; // 월트 메트릭스

				// --------------------------------- Set_QuatAndScale ----------------------------------------------
				// ------------------------- 회전 혹은 스케일에 영향을 받는 옵션들. -----------------------------
				DUOLMath::Vector3 s;
				DUOLMath::Quaternion r;
				DUOLMath::Vector3 t;

				world.Decompose(s, r, t);

				world.m[3][0] = 0; world.m[3][1] = 0; world.m[3][2] = 0;

				_commonInfo.gStartSize[0] = _commonInfo.gStartSize[0] * s.x;
				_commonInfo.gStartSize[1] = _commonInfo.gStartSize[1] * s.y;
				_commonInfo.gStartSize[2] = _commonInfo.gStartSize[2] * s.x;
				_commonInfo.gStartSize[3] = _commonInfo.gStartSize[3] * s.y;

				if (renderingData._velocity_Over_Lifetime._space == Space::Local)
					_velocityoverLifetime.gVelocity = DUOLMath::Vector3::Transform(_velocityoverLifetime.gVelocity, world);

				if (renderingData._force_Over_Lifetime._space == Space::Local)
					_forceoverLifetime.gForce = DUOLMath::Vector3::Transform(_forceoverLifetime.gForce, world);

				DUOLMath::Matrix shapeTM = DUOLMath::Matrix::CreateScale(_shape.gScale) * DUOLMath::Matrix::CreateFromYawPitchRoll(_shape.gRotation.z, _shape.gRotation.x, _shape.gRotation.y) * DUOLMath::Matrix::CreateTranslation(_shape.gPosition);

				world = shapeTM * renderingData._commonInfo._transformMatrix;
				memcpy(&_commonInfo.gTransformMatrix, &world, sizeof(DUOLMath::Matrix));

			}
		}

	}

};