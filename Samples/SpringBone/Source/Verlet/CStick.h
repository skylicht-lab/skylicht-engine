#pragma once

#include "CParticle.h"

namespace Verlet
{
	class CStickDistance
	{
	public:
		CParticle* P1;
		CParticle* P2;
		float F;
		float Stiffness;

		bool DebugColor;
		SColor Color;

	protected:

		float m_defaultDistance;
		float m_defaultDistance2;

	public:
		CStickDistance(CParticle* p1, CParticle* p2);

		virtual ~CStickDistance();

		inline float getDefaultDistance()
		{
			return m_defaultDistance;
		}

		inline float getDefaultDistance2()
		{
			return m_defaultDistance2;
		}
	};
}