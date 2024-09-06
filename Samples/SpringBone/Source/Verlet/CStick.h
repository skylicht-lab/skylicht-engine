#pragma once

#include "CParticle.h"

namespace Verlet
{
	class CStick
	{
	public:
		CParticle* P1;
		CParticle* P2;
		float F;
		float Stiffness;

	protected:

		float m_defaultLength;

	public:
		CStick(CParticle* p1, CParticle* p2);

		virtual ~CStick();

		inline float getDefaultLength()
		{
			return m_defaultLength;
		}
	};
}