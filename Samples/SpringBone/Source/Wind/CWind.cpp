#include "pch.h"
#include "CWind.h"

namespace Verlet
{
	CWind::CWind() :
		m_direction(0.0f, 0.0f, 1.0f),
		m_strong(0.01f),
		m_windTime(0.0f),
		m_scale(0.5f),
		m_timeRatio(1.0f),
		m_enable(true)
	{
		m_perlin.reseed(getIrrlichtDevice()->getRandomizer()->rand());
	}

	CWind::~CWind()
	{

	}

	void CWind::update(CVerlet* verlet)
	{
		if (!m_enable)
			return;

		CParticle** particles = verlet->getParticles();

		float noise = 0.0f;
		core::vector3df wind;

		float step = getTimeStep() * 0.001f;

		m_windTime = m_windTime + step * m_timeRatio;

		for (int i = 0, n = verlet->getNumParticle(); i < n; i++)
		{
			CParticle* particle = particles[i];

			noise = m_perlin.noise2D_01(particle->Position.Y * m_scale, m_windTime);
			wind = m_direction * m_strong * step * noise;
			particle->AffectVelocity += wind;
		}
	}

	float CWind::getNoiseValue(float y)
	{
		return m_perlin.noise2D_01(y * m_scale, m_windTime);;
	}
}