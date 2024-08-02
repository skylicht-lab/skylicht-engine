/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

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

	void CTouchIdentify::touchPress(int pos, int touchID)
	{
		m_touchIdentity[pos].TouchID = touchID;
		m_touchIdentity[pos].Identitfy = CTouchIdentify::Nothing;
		m_touchIdentity[pos].Data = NULL;
	}

	void CTouchIdentify::touchRelease(int pos, int touchID)
	{
		m_touchIdentity[pos].TouchID = -1;
		m_touchIdentity[pos].Identitfy = CTouchIdentify::Nothing;
		m_touchIdentity[pos].Data = NULL;
	}

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