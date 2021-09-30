/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CBBCollisionManager.h"
#include "CTriangleBB.h"

namespace Skylicht
{
	CBBCollisionManager::CBBCollisionManager()
	{

	}

	CBBCollisionManager::~CBBCollisionManager()
	{
		for (u32 i = 0, n = m_selector.size(); i < n; i++)
			delete m_selector[i];
		m_selector.clear();
	}

	void CBBCollisionManager::addBBCollision(CGameObject* object, const core::aabbox3df& bbox)
	{
		m_selector.push_back(new CTriangleBB(object, bbox));
	}

	void CBBCollisionManager::removeBBCollision(CGameObject* object)
	{
		for (u32 i = 0, n = m_selector.size(); i < n; i++)
		{
			if (m_selector[i]->getGameObject() == object)
			{
				if (n >= 2)
				{
					// swap to last and delete
					CTriangleSelector* t = m_selector[n - 1];
					m_selector[n - 1] = m_selector[i];
					m_selector[i] = t;
					delete m_selector[n - 1];
					m_selector.erase(n - 1);
					return;
				}
				else
				{
					delete m_selector[i];
					m_selector.erase(i);
					return;
				}
			}
		}
	}
}