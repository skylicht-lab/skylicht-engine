#include "pch.h"
#include "CStick.h"

namespace Verlet
{
	CStick::CStick(CParticle* p1, CParticle* p2) :
		P1(p1),
		P2(p2),
		Stiffness(0.2f),
		F(0.0f)
	{
		m_defaultLength = P1->Position.getDistanceFrom(P2->Position);
	}

	CStick::~CStick()
	{

	}
}