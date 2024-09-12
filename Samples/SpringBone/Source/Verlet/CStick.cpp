#include "pch.h"
#include "CStick.h"

#include "Utils/CVector.h"

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

	CStickAngle::CStickAngle(CParticle* p1, CParticle* p2, CParticle* p3) :
		P1(p1),
		P2(p2),
		P3(p3),
		Stiffness(0.005f)
	{
		Direction1 = P2->Position - P1->Position;
		Direction2 = P3->Position - P2->Position;
	}

	CStickAngle::~CStickAngle()
	{

	}
}