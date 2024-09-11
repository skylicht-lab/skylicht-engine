#include "pch.h"
#include "CStick.h"

namespace Verlet
{
	CStickDistance::CStickDistance(CParticle* p1, CParticle* p2) :
		P1(p1),
		P2(p2),
		Stiffness(0.05f),
		F(0.0f),
		DebugColor(false)
	{
		m_defaultDistance = P1->Position.getDistanceFrom(P2->Position);
		m_defaultDistance2 = m_defaultDistance * m_defaultDistance;
	}

	CStickDistance::~CStickDistance()
	{

	}
}