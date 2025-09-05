#include "pch.h"
#include "CShadowMapBaseRP.h"

namespace Skylicht
{
	CShadowMapBaseRP::CShadowMapBaseRP() :
		m_currentLight(NULL),
		m_bakeInUV0(false),
		m_bakeDetailNormal(false)
	{

	}

	CShadowMapBaseRP::~CShadowMapBaseRP()
	{

	}
}