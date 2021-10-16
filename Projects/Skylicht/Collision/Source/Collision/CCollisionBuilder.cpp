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
#include "CCollisionBuilder.h"

#include "GameObject/CGameObject.h"

namespace Skylicht
{
	CCollisionBuilder::CCollisionBuilder()
	{

	}

	CCollisionBuilder::~CCollisionBuilder()
	{

	}

	void CCollisionBuilder::removeCollision(CGameObject* object)
	{
		for (u32 i = 0, n = m_nodes.size(); i < n; i++)
		{
			if (m_nodes[i]->GameObject == object)
			{
				if (n >= 2)
				{
					// swap to last and delete
					CCollisionNode* t = m_nodes[n - 1];
					m_nodes[n - 1] = m_nodes[i];
					m_nodes[i] = t;
					delete m_nodes[n - 1];
					m_nodes.erase(n - 1);
					return;
				}
				else
				{
					delete m_nodes[i];
					m_nodes.erase(i);
					return;
				}
			}
		}

		build();
	}

	void CCollisionBuilder::removeCollision(CCollisionNode** nodes, int count)
	{
		for (u32 i = 0, n = m_nodes.size(); i < n; i++)
		{
			int id = findNode(m_nodes[i], nodes, count);
			if (id >= 0)
			{
				nodes[id] = NULL;

				if (n >= 2)
				{
					// swap to last and delete
					CCollisionNode* t = m_nodes[n - 1];
					m_nodes[n - 1] = m_nodes[i];
					m_nodes[i] = t;
					delete m_nodes[n - 1];
					m_nodes.erase(n - 1);
					return;
				}
				else
				{
					delete m_nodes[i];
					m_nodes.erase(i);
					return;
				}
			}
		}
	}

	void CCollisionBuilder::clear()
	{
		for (u32 i = 0, n = m_nodes.size(); i < n; i++)
		{
			delete m_nodes[i]->Selector;
			delete m_nodes[i];
		}
		m_nodes.clear();
	}

	int CCollisionBuilder::findNode(CCollisionNode* node, CCollisionNode** nodes, int count)
	{
		for (int i = 0; i < count; i++)
		{
			if (nodes[i] == node)
			{
				return i;
			}
		}

		return -1;
	}

	CGameObject* CCollisionBuilder::getObjectWithRay(
		const core::line3d<f32>& ray,
		f32& outBestDistanceSquared,
		core::vector3df& outIntersection,
		core::triangle3df& outTriangle,
		CCollisionNode*& outNode)
	{
		if (getCollisionPoint(ray, outBestDistanceSquared, outIntersection, outTriangle, outNode))
		{
			return outNode->GameObject;
		}
		return NULL;
	}

	bool CCollisionBuilder::getCollisionPoint(
		const core::vector3df& target,
		const core::vector3df& pos,
		core::vector3df& outIntersection,
		core::triangle3df& outTriangle,
		CCollisionNode*& outNode)
	{
		const core::line3df ray(pos, target);
		float outBestDistanceSquared = ray.getLengthSQ();
		return getCollisionPoint(ray, outBestDistanceSquared, outIntersection, outTriangle, outNode);
	}
}