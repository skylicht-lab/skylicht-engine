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
#include "CBBoxPatchBuilder.h"

namespace Skylicht
{
	CBBoxPatchBuilder::CBBoxPatchBuilder() :
		m_patchSize(100.0f)
	{

	}

	CBBoxPatchBuilder::~CBBoxPatchBuilder()
	{
		clear();
	}

	void CBBoxPatchBuilder::clear()
	{
		for (int i = 0, n = m_patchs.size(); i < n; i++)
			delete m_patchs[i];

		m_patchs.set_used(0);
		m_collisionPatchs.set_used(0);

		CCollisionBuilder::clear();
	}

	void CBBoxPatchBuilder::build()
	{
		// delete old patchs
		for (int i = 0, n = m_patchs.size(); i < n; i++)
			delete m_patchs[i];

		m_patchs.set_used(0);
		m_collisionPatchs.set_used(0);

		const u32 start = os::Timer::getRealTime();
		u32 numPoly = 0;

		// step 1: caculator the box size
		for (u32 i = 0, n = m_nodes.size(); i < n; i++)
		{
			if (i == 0)
				m_size = m_nodes[i]->getTransformBBox();
			else
				m_size.addInternalBox(m_nodes[i]->getTransformBBox());
		}

		// step 2: caculator num patch
		core::vector3df s = m_size.getExtent();

		m_numX = core::ceil32(s.X / m_patchSize);
		m_numY = core::ceil32(s.Y / m_patchSize);
		m_numZ = core::ceil32(s.Z / m_patchSize);

		for (int x = 0; x < m_numX; x++)
		{
			for (int z = 0; z < m_numZ; z++)
			{
				for (int y = 0; y < m_numY; y++)
				{
					m_patchs.push_back(new SPatch());

					SPatch& p = *(m_patchs.getLast());
					p.X = x;
					p.Y = y;
					p.Z = z;

					p.BBox.MinEdge = m_size.MinEdge + core::vector3df(x * m_patchSize, y * m_patchSize, z * m_patchSize);
					p.BBox.MaxEdge = p.BBox.MinEdge + core::vector3df(m_patchSize, m_patchSize, m_patchSize);
				}
			}
		}

		// step 3: index nodes to patch
		for (u32 i = 0, n = m_nodes.size(); i < n; i++)
		{
			addNodeToPatch(m_nodes[i]);
		}

		// step4: collect the patch have collisions
		for (u32 i = 0, n = m_patchs.size(); i < n; i++)
		{
			if (m_patchs[i]->Collisions.size() > 0)
			{
				m_collisionPatchs.push_back(m_patchs[i]);
			}
		}


		c8 tmp[256];
		sprintf(tmp, "Needed %ums to CBBoxPatchBuilder::build", os::Timer::getRealTime() - start);
		os::Printer::log(tmp, ELL_INFORMATION);
	}

	CBBoxPatchBuilder::SPatch* CBBoxPatchBuilder::getPatch(int x, int y, int z)
	{
		int id = x * m_numZ * m_numY + z * m_numY + y;
		if (id < 0 || id >= (int)m_patchs.size())
			return NULL;
		return m_patchs[id];
	}

	void CBBoxPatchBuilder::addNodeToPatch(CCollisionNode* collision)
	{
		core::aabbox3df bbox = collision->getTransformBBox();
		int x1 = core::floor32((bbox.MinEdge.X - m_size.MinEdge.X) / m_patchSize);
		int y1 = core::floor32((bbox.MinEdge.Y - m_size.MinEdge.Y) / m_patchSize);
		int z1 = core::floor32((bbox.MinEdge.Z - m_size.MinEdge.Z) / m_patchSize);

		int x2 = core::floor32((bbox.MaxEdge.X - m_size.MinEdge.X) / m_patchSize);
		int y2 = core::floor32((bbox.MaxEdge.Y - m_size.MinEdge.Y) / m_patchSize);
		int z2 = core::floor32((bbox.MaxEdge.Z - m_size.MinEdge.Z) / m_patchSize);

		for (int x = x1; x <= x2; x++)
		{
			for (int z = z1; z <= z2; z++)
			{
				for (int y = y1; y <= y2; y++)
				{
					SPatch* patch = getPatch(x, y, z);
					if (patch)
					{
						if (patch->BBox.intersectsWithBox(bbox))
						{
							patch->Collisions.push_back(collision);
						}
					}
				}
			}
		}
	}

	void CBBoxPatchBuilder::removeNode(CCollisionNode* collision)
	{
		core::aabbox3df bbox = collision->getTransformBBox();
		int x1 = core::floor32((bbox.MinEdge.X - m_size.MinEdge.X) / m_patchSize);
		int y1 = core::floor32((bbox.MinEdge.Y - m_size.MinEdge.Y) / m_patchSize);
		int z1 = core::floor32((bbox.MinEdge.Z - m_size.MinEdge.Z) / m_patchSize);

		int x2 = core::floor32((bbox.MaxEdge.X - m_size.MinEdge.X) / m_patchSize);
		int y2 = core::floor32((bbox.MaxEdge.Y - m_size.MinEdge.Y) / m_patchSize);
		int z2 = core::floor32((bbox.MaxEdge.Z - m_size.MinEdge.Z) / m_patchSize);

		for (int x = x1; x <= x2; x++)
		{
			for (int z = z1; z <= z2; z++)
			{
				for (int y = y1; y <= y2; y++)
				{
					SPatch* patch = getPatch(x, y, z);
					if (patch)
					{
						if (patch->BBox.intersectsWithBox(bbox))
						{
							removeNode(collision, patch);
						}
					}
				}
			}
		}
	}

	void CBBoxPatchBuilder::removeNode(CCollisionNode* collision, SPatch* patch)
	{
		for (int i = 0, n = patch->Collisions.size(); i < n; i++)
		{
			if (patch->Collisions[i] == collision)
			{
				patch->Collisions.erase(i);
				return;
			}
		}
	}
}