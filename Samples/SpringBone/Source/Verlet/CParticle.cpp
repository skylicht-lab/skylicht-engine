#include "pch.h"
#include "CParticle.h"

namespace Verlet
{
	CParticle::CParticle() :
		IsConstraint(false),
		Mass(0.001f)
	{

	}

	CParticle::~CParticle()
	{

	}
}