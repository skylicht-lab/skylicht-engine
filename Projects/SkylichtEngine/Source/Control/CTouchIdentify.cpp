#include "pch.h"
#include "CTouchIdentify.h"

namespace Skylicht
{

CTouchIdentify::CTouchIdentify()
{
	for (int i = 0; i < MAX_MULTITOUCH; i++)
	{
		m_touchIdentity[i].TouchID = -1;
		m_touchIdentity[i].Identitfy = CTouchIdentify::Nothing;
		m_touchIdentity[i].Data = NULL;
	}
}

CTouchIdentify::~CTouchIdentify()
{
}

// touchRelease
void CTouchIdentify::touchPress(int pos, int touchID)
{
	m_touchIdentity[pos].TouchID = touchID;
	m_touchIdentity[pos].Identitfy = CTouchIdentify::Nothing;
	m_touchIdentity[pos].Data = NULL;
}
	
// touchRelease
void CTouchIdentify::touchRelease(int pos, int touchID)
{
	m_touchIdentity[pos].TouchID = -1;
	m_touchIdentity[pos].Identitfy = CTouchIdentify::Nothing;
	m_touchIdentity[pos].Data = NULL;
}

// setTouchIdentify
void CTouchIdentify::setTouchIdentify(int touchID, CTouchIdentify::ETouchIdentify identify, void *data)
{
	for (int i = 0; i < MAX_MULTITOUCH; i++)
	{
		if (m_touchIdentity[i].TouchID == touchID)
		{
			m_touchIdentity[i].Identitfy = identify;
			m_touchIdentity[i].Data = data;
			return;
		}
	}
}

// getTouchIdentify
CTouchIdentify::ETouchIdentify CTouchIdentify::getTouchIdentify(int touchID)
{
	for (int i = 0; i < MAX_MULTITOUCH; i++)
	{
		if (m_touchIdentity[i].TouchID == touchID)
		{
			return m_touchIdentity[i].Identitfy;
		}
	}

	return CTouchIdentify::Nothing;
}

}