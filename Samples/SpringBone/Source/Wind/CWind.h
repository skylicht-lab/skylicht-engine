#pragma once

#include "PerlinNoise.hpp"
#include "Verlet/CVerlet.h"

namespace Verlet
{
	class CWind
	{
	private:
		core::vector3df m_direction;
		float m_strong;
		float m_windTime;
		float m_scale;
		float m_timeRatio;

		siv::BasicPerlinNoise<float> m_perlin;

		bool m_enable;

	public:
		CWind();

		virtual ~CWind();

		void update(CVerlet* verlet);

		inline float getScale()
		{
			return m_scale;
		}

		inline void setScale(float scale)
		{
			m_scale = scale;
		}

		inline float getStrong()
		{
			return m_strong;
		}

		inline void setStrong(float f)
		{
			m_strong = f;
		}

		inline void setDirection(const core::vector3df& v)
		{
			m_direction = v;
		}

		inline float getTime()
		{
			return m_windTime;
		}

		inline void setTimeRatio(float t)
		{
			m_timeRatio = t;
		}

		inline float getTimeRatio()
		{
			return m_timeRatio;
		}

		inline bool isEnable()
		{
			return m_enable;
		}

		inline void setEnable(bool enable)
		{
			m_enable = enable;
		}

		float getNoiseValue(float y);
	};
}