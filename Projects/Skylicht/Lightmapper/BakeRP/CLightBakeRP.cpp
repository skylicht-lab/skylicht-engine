#include "pch.h"
#include "CLightBakeRP.h"

namespace Skylicht
{
	CLightBakeRP::CLightBakeRP() :
		m_renderMesh(NULL),
		m_normalMap(NULL),
		m_submesh(NULL),
		m_renderTarget(NULL),
		m_numTarget(0),
		m_currentTarget(0)
	{

	}

	CLightBakeRP::~CLightBakeRP()
	{

	}
}