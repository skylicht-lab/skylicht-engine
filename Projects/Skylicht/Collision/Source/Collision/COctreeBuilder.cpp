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
#include "COctreeBuilder.h"

#include "Debug/CSceneDebug.h"

namespace Skylicht
{
	COctreeBuilder::COctreeBuilder() :
		m_root(NULL),
		m_minimalPolysPerNode(128)
	{

	}

	COctreeBuilder::~COctreeBuilder()
	{
		clear();
	}

	void COctreeBuilder::clear()
	{
		if (m_root != NULL)
			delete m_root;
		m_root = NULL;

		CCollisionBuilder::clear();
	}

	void COctreeBuilder::build()
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
		sprintf(tmp, "Needed %ums to COctreeBuilder::build (%u polys)", os::Timer::getRealTime() - start, numPoly);
		os::Printer::log(tmp, ELL_INFORMATION);
	}

	void COctreeBuilder::drawDebug()
	{
		std::queue<COctreeNode*> nodes;
		nodes.push(m_root);

		while (nodes.size() > 0)
		{
			COctreeNode* node = nodes.front();
			nodes.pop();

			CSceneDebug::getInstance()->addBoudingBox(node->Box, SColor(255, 255, 0, 0));

			for (u32 i = 0; i != 8; ++i)
			{
				if (node->Childs[i])
				{
					nodes.push(node->Childs[i]);
				}
			}
		}
	}

	void COctreeBuilder::constructOctree(COctreeNode* node)
	{
		const core::vector3df& middle = node->Box.getCenter();
		core::vector3df edges[8];
		node->Box.getEdges(edges);

		core::aabbox3d<f32> childOctreeBox;
		core::array<int> keepTriangles;
		core::array<CCollisionNode*> keepCollisions;

		// calculate children
		if (!node->Box.isEmpty() && node->Triangles.size() > m_minimalPolysPerNode)
		{
			for (s32 ch = 0; ch < 8; ++ch)
			{
				childOctreeBox.reset(middle);
				childOctreeBox.addInternalPoint(edges[ch]);

				node->Childs[ch] = new COctreeNode();

				// step 1 new child octree
				COctreeNode* childNode = node->Childs[ch];
				childNode->Parent = node;
				childNode->OctreeBox = childOctreeBox;
				childNode->Level = node->Level + 1;

				s32 numTri = (s32)node->Triangles.size();

				bool first = true;

				// step 2 collect triangle inside the child
				for (s32 i = 0; i < numTri; ++i)
				{
					int triID = node->Triangles[i];
					CCollisionNode* collision = node->Collisions[i];

					core::triangle3df* triangles = collision->Triangles.pointer();
					core::triangle3df* tri = &triangles[triID];

					if (tri->isTotalInsideBox(childNode->OctreeBox))
					{
						// move triangles to child
						childNode->Triangles.push_back(triID);
						childNode->Collisions.push_back(collision);

						if (first)
						{
							childNode->Box.reset(tri->pointA);
							childNode->Box.addInternalPoint(tri->pointB);
							childNode->Box.addInternalPoint(tri->pointC);
							first = false;
						}
						else
						{
							childNode->Box.addInternalPoint(tri->pointA);
							childNode->Box.addInternalPoint(tri->pointB);
							childNode->Box.addInternalPoint(tri->pointC);
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

				if (childNode->Triangles.empty())
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

	bool COctreeBuilder::getCollisionPoint(
		const core::line3d<f32>& ray,
		f32& outBestDistanceSquared,
		core::vector3df& outIntersection,
		core::triangle3df& outTriangle,
		CCollisionNode*& outNode)
	{
		outNode = NULL;

		core::vector3df mid = ray.getMiddle();
		core::vector3df	vec = ray.getVector().normalize();

		float halfLength = ray.getLength() * 0.5f;

		m_triangles.set_used(0);
		m_collisions.set_used(0);

		// step 1: get list triangle collide with ray
		getTrianglesFromOctree(m_triangles, m_collisions, m_root, mid, vec, halfLength);

		// step 2: find nearest triangle
		s32 cnt = (s32)m_triangles.size();
		core::triangle3df** listTris = m_triangles.pointer();

		core::vector3df intersection;
		f32 nearest = outBestDistanceSquared;
		const f32 raylength = ray.getLengthSQ();

		bool found = false;

		const f32 minX = core::min_(ray.start.X, ray.end.X);
		const f32 maxX = core::max_(ray.start.X, ray.end.X);
		const f32 minY = core::min_(ray.start.Y, ray.end.Y);
		const f32 maxY = core::max_(ray.start.Y, ray.end.Y);
		const f32 minZ = core::min_(ray.start.Z, ray.end.Z);
		const f32 maxZ = core::max_(ray.start.Z, ray.end.Z);

		for (s32 i = 0; i < cnt; ++i)
		{
			const core::triangle3df* triangle = listTris[i];

			if (minX > triangle->pointA.X && minX > triangle->pointB.X && minX > triangle->pointC.X)
				continue;
			if (maxX < triangle->pointA.X && maxX < triangle->pointB.X && maxX < triangle->pointC.X)
				continue;
			if (minY > triangle->pointA.Y && minY > triangle->pointB.Y && minY > triangle->pointC.Y)
				continue;
			if (maxY < triangle->pointA.Y && maxY < triangle->pointB.Y && maxY < triangle->pointC.Y)
				continue;
			if (minZ > triangle->pointA.Z && minZ > triangle->pointB.Z && minZ > triangle->pointC.Z)
				continue;
			if (maxZ < triangle->pointA.Z && maxZ < triangle->pointB.Z && maxZ < triangle->pointC.Z)
				continue;

			if (triangle->getIntersectionWithLine(ray.start, vec, intersection))
			{
				const f32 tmp = intersection.getDistanceFromSQ(ray.start);
				const f32 tmp2 = intersection.getDistanceFromSQ(ray.end);

				if (tmp < raylength && tmp2 < raylength && tmp < nearest)
				{
					nearest = tmp;
					outBestDistanceSquared = nearest;

					outTriangle = *triangle;
					outIntersection = intersection;
					outNode = m_collisions[i];
					found = true;
				}
			}
		}

		return found;
	}

	void COctreeBuilder::getTrianglesFromOctree(
		core::array<core::triangle3df*>& listTriangle,
		core::array<CCollisionNode*>& listCollisions,
		COctreeNode* node,
		const core::vector3df& midLine,
		const core::vector3df& lineVect,
		float halfLength)
	{
		u32 cnt = node->Triangles.size();
		u32 i = 0;

		int* listTriID = node->Triangles.pointer();

		// optimize function core::array::insert
		int n = listTriangle.size();
		listTriangle.set_used(n + cnt);
		listCollisions.set_used(n + cnt);

		core::triangle3df** p = listTriangle.pointer();
		CCollisionNode** c = listCollisions.pointer();

		// list triangles
		for (i = 0; i < cnt; ++i)
		{
			CCollisionNode* collision = node->Collisions[i];

			p[n + i] = &collision->Triangles[listTriID[i]];
			c[n + i] = collision;
		}

		for (i = 0; i < 8; ++i)
		{
			if (node->Childs[i] && node->Childs[i]->Box.intersectsWithLine(midLine, lineVect, halfLength) == true)
				getTrianglesFromOctree(listTriangle, listCollisions, node->Childs[i], midLine, lineVect, halfLength);
		}
	}

	void COctreeBuilder::getTriangles(const core::aabbox3df& box,
		core::array<core::triangle3df*>& result,
		core::array<CCollisionNode*>& nodes)
	{
		getTrianglesFromOctree(result, nodes, m_root, box);
	}

	void COctreeBuilder::getTrianglesFromOctree(
		core::array<core::triangle3df*>& listTriangle,
		core::array<CCollisionNode*>& listCollisions,
		COctreeNode* node,
		const core::aabbox3df& box)
	{
		u32 cnt = node->Triangles.size();
		u32 i = 0;

		int* listTriID = node->Triangles.pointer();

		// optimize function core::array::insert
		int n = listTriangle.size();
		listTriangle.set_used(n + cnt);
		listCollisions.set_used(n + cnt);
		int used = 0;

		core::triangle3df** p = listTriangle.pointer();
		CCollisionNode** c = listCollisions.pointer();

		// list triangles
		for (i = 0; i < cnt; ++i)
		{
			CCollisionNode* collision = node->Collisions[i];
			core::triangle3df& triangle = collision->Triangles[listTriID[i]];

			// if triangle collide the bbox
			if (box.isPointInside(triangle.pointA) ||
				box.isPointInside(triangle.pointB) ||
				box.isPointInside(triangle.pointC))
			{
				p[n + used] = &triangle;
				c[n + used] = collision;
				used++;
			}
		}

		// update the used
		listTriangle.set_used(n + used);
		listCollisions.set_used(n + used);

		for (i = 0; i < 8; ++i)
		{
			if (node->Childs[i] && node->Childs[i]->Box.intersectsWithBox(box))
				getTrianglesFromOctree(listTriangle, listCollisions, node->Childs[i], box);
		}
	}
}