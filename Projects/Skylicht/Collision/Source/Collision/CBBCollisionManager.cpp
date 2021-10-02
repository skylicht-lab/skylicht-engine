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
	CBBCollisionManager::CBBCollisionManager() :
		m_root(NULL),
		m_minimalPolysPerNode(128),
		m_nodeCount(0)
	{

	}

	CBBCollisionManager::~CBBCollisionManager()
	{
		if (m_root != NULL)
			delete m_root;

		for (u32 i = 0, n = m_nodes.size(); i < n; i++)
		{
			delete m_nodes[i]->Selector;
			delete m_nodes[i];
		}
		m_nodes.clear();
	}

	void CBBCollisionManager::addBBCollision(CGameObject* object, const core::aabbox3df& bbox)
	{
		CEntity* entity = object->getEntity();
		m_nodes.push_back(new CCollisionNode(object, entity, new CTriangleBB(entity, bbox)));
		rebuildOctree();
	}

	void CBBCollisionManager::removeBBCollision(CGameObject* object)
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

		rebuildOctree();
	}

	void CBBCollisionManager::rebuildOctree()
	{
		if (m_root != NULL)
			delete m_root;

		m_root = NULL;
		m_root = new COctreeNode();

		const u32 start = os::Timer::getRealTime();
		u32 numPoly = 0;

		// step 1: update transform and triangles
		for (u32 i = 0, n = m_nodes.size(); i < n; i++)
		{
			m_nodes[i]->updateTransform();
			numPoly += m_nodes[i]->Triangles.size();
		}

		m_root->Triangles.set_used(numPoly);
		m_root->Collisions.set_used(numPoly);

		// step 2: index triangle & bbox
		u32 idx = 0;

		m_root->Box.reset(m_nodes[0]->Triangles[0].pointA);

		for (u32 i = 0, n = m_nodes.size(); i < n; i++)
		{
			CCollisionNode* node = m_nodes[i];

			u32 numTris = node->Triangles.size();
			core::triangle3df* tris = node->Triangles.pointer();

			for (u32 j = 0; j < numTris; j++)
			{
				m_root->Triangles[idx] = j;
				m_root->Collisions[idx] = node;

				m_root->Box.addInternalPoint(tris[j].pointA);
				m_root->Box.addInternalPoint(tris[j].pointB);
				m_root->Box.addInternalPoint(tris[j].pointC);

				idx++;
			}
		}

		m_root->OctreeBox = m_root->Box;
		m_root->Level = 0;

		// step 3 build octree child node
		constructOctree(m_root);

		c8 tmp[256];
		sprintf(tmp, "Needed %ums to CBBCollisionManager::rebuildOctree (%d nodes, %u polys)", os::Timer::getRealTime() - start, m_nodeCount, numPoly);
		os::Printer::log(tmp, ELL_INFORMATION);
	}

	void CBBCollisionManager::constructOctree(COctreeNode* node)
	{
		const core::vector3df& middle = node->Box.getCenter();
		core::vector3df edges[8];
		node->Box.getEdges(edges);

		core::aabbox3d<f32> box;
		core::array<int> keepTriangles;
		core::array<CCollisionNode*> keepCollisions;

		// calculate children
		if (!node->Box.isEmpty() && node->Triangles.size() > m_minimalPolysPerNode)
		{
			for (s32 ch = 0; ch < 8; ++ch)
			{
				box.reset(middle);
				box.addInternalPoint(edges[ch]);

				node->Childs[ch] = new COctreeNode();

				// step 1 new child octree
				COctreeNode* currentNode = node->Childs[ch];
				currentNode->Parent = node;
				currentNode->OctreeBox = box;
				currentNode->Level = node->Level + 1;

				s32 numTri = (s32)node->Triangles.size();

				// step 2 collect triangle in side the child
				for (s32 i = 0; i < numTri; ++i)
				{
					int triID = node->Triangles[i];
					CCollisionNode* collision = node->Collisions[i];

					core::triangle3df* triangles = collision->Triangles.pointer();
					core::triangle3df* tri = &triangles[triID];

					if (tri->isTotalOutsideBox(box) == false && tri->isTotalInsideBox(node->OctreeBox) == true)
					{
						// move triangles to child
						currentNode->Triangles.push_back(triID);
						currentNode->Collisions.push_back(collision);

						if (i == 0)
						{
							currentNode->Box.reset(tri->pointA);
							currentNode->Box.addInternalPoint(tri->pointB);
							currentNode->Box.addInternalPoint(tri->pointC);
						}
						else
						{
							currentNode->Box.addInternalPoint(tri->pointA);
							currentNode->Box.addInternalPoint(tri->pointB);
							currentNode->Box.addInternalPoint(tri->pointC);
						}
					}
					else
					{
						// keep on parent node
						keepTriangles.push_back(triID);
						keepCollisions.push_back(collision);
					}
				}

				// update current triangles
				node->Triangles.clear();
				node->Collisions.clear();

				node->Triangles.set_used(keepTriangles.size());
				node->Collisions.set_used(keepCollisions.size());

				for (int i = 0, n = (int)keepTriangles.size(); i < n; i++)
				{
					node->Triangles[i] = keepTriangles[i];
					node->Collisions[i] = keepCollisions[i];
				}

				keepTriangles.clear();
				keepCollisions.clear();

				if (currentNode->Triangles.empty())
				{
					// if no triangle
					delete node->Childs[ch];
					node->Childs[ch] = NULL;
				}
				else
				{
					// construct on childs
					constructOctree(node->Childs[ch]);
				}
			}
		}
	}
}