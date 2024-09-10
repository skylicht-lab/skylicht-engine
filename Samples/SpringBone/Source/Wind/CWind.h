#pragma once

#include "PerlinNoise.hpp"
#include "Verlet/CVerlet.h"

namespace Verlet
{
	class CWind
	{
	private:
		core::vector3df m_vector;
		float m_strong;
		float m_windTime;
		float m_scale;

		siv::BasicPerlinNoise<float> m_perlin;

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

		inline void setVector(const core::vector3df& v)
		{
			m_vector = v;
		}

		inline float getTime()
		{
			return m_windTime;
		}

		float getNoiseValue(float y);
	};
}