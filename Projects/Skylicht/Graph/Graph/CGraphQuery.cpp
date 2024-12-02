#include "pch.h"
#include "CGraphQuery.h"

namespace Skylicht
{
	namespace Graph
	{
		COctreeNode::COctreeNode() :
			Parent(NULL),
			Level(0)
		{
			for (u32 i = 0; i < 8; i++)
				Childs[i] = NULL;
		}

		COctreeNode::~COctreeNode()
		{
			for (u32 i = 0; i != 8; ++i)
			{
				if (Childs[i])
					delete Childs[i];
			}
		}


		CGraphQuery::CGraphQuery() :
			m_root(NULL),
			m_minimalPolysPerNode(4)
		{

		}

		CGraphQuery::~CGraphQuery()
		{
			release();
		}

		void CGraphQuery::release()
		{
			if (m_root)
			{
				delete m_root;
				m_root = NULL;
			}
		}

		void CGraphQuery::buildIndexNavMesh(CMesh* navMesh, CObstacleAvoidance* obstacle)
		{
			release();

			m_root = new COctreeNode();

			// check used tile
			IMeshBuffer* mb = navMesh->getMeshBuffer(0);
			IVertexBuffer* vb = mb->getVertexBuffer();
			IIndexBuffer* ib = mb->getIndexBuffer();

			for (u32 i = 0, n = ib->getIndexCount(); i < n; i += 3)
			{
				u32 ixA = ib->getIndex(i);
				u32 ixB = ib->getIndex(i + 1);
				u32 ixC = ib->getIndex(i + 2);

				S3DVertex* a = (S3DVertex*)vb->getVertex(ixA);
				S3DVertex* b = (S3DVertex*)vb->getVertex(ixB);
				S3DVertex* c = (S3DVertex*)vb->getVertex(ixC);

				if (i == 0)
				{
					m_root->Box.reset(a->Pos);
					m_root->Box.addInternalPoint(b->Pos);
					m_root->Box.addInternalPoint(c->Pos);
				}
				else
				{
					m_root->Box.addInternalPoint(a->Pos);
					m_root->Box.addInternalPoint(b->Pos);
					m_root->Box.addInternalPoint(c->Pos);
				}

				core::triangle3df tri(a->Pos, b->Pos, c->Pos);
				m_root->Triangles.push_back(tri);
			}

			m_root->OctreeBox = m_root->Box;

			constructOctree(m_root);
		}

		void CGraphQuery::constructOctree(COctreeNode* node)
		{
			const core::vector3df& middle = node->Box.getCenter();
			core::vector3df edges[8];
			node->Box.getEdges(edges);

			core::aabbox3d<f32> childOctreeBox;
			core::array<core::triangle3df> keepTriangles;

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
						core::triangle3df& tri = node->Triangles[i];

						if (tri.isTotalInsideBox(childNode->OctreeBox))
						{
							// move triangles to child
							childNode->Triangles.push_back(tri);

							if (first)
							{
								childNode->Box.reset(tri.pointA);
								childNode->Box.addInternalPoint(tri.pointB);
								childNode->Box.addInternalPoint(tri.pointC);
								first = false;
							}
							else
							{
								childNode->Box.addInternalPoint(tri.pointA);
								childNode->Box.addInternalPoint(tri.pointB);
								childNode->Box.addInternalPoint(tri.pointC);
							}
						}
						else
						{
							// keep on parent node
							keepTriangles.push_back(tri);
						}
					}

					// update current triangles
					node->Triangles.clear();
					node->Triangles.set_used(keepTriangles.size());

					for (int i = 0, n = (int)keepTriangles.size(); i < n; i++)
					{
						node->Triangles[i] = keepTriangles[i];
					}

					keepTriangles.clear();

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

		bool CGraphQuery::getCollisionPoint(
			const core::line3d<f32>& ray,
			f32& outBestDistanceSquared,
			core::vector3df& outIntersection,
			core::triangle3df& outTriangle)
		{
			if (m_root == NULL)
				return false;

			core::vector3df mid = ray.getMiddle();
			core::vector3df	vec = ray.getVector().normalize();

			float halfLength = ray.getLength() * 0.5f;

			m_triangles.set_used(0);

			// step 1: get list triangle collide with ray
			getTrianglesFromOctree(m_triangles, m_root, mid, vec, halfLength);

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

						found = true;
					}
				}
			}

			return found;
		}

		void CGraphQuery::getTrianglesFromOctree(
			core::array<core::triangle3df*>& listTriangle,
			COctreeNode* node,
			const core::vector3df& midLine,
			const core::vector3df& lineVect,
			float halfLength)
		{
			u32 cnt = node->Triangles.size();
			u32 i = 0;

			core::triangle3df* listTriID = node->Triangles.pointer();

			// optimize function core::array::insert
			int n = listTriangle.size();
			listTriangle.set_used(n + cnt);

			core::triangle3df** p = listTriangle.pointer();

			// list triangles
			for (i = 0; i < cnt; ++i)
			{
				p[n + i] = &listTriID[i];
			}

			for (i = 0; i < 8; ++i)
			{
				if (node->Childs[i] && node->Childs[i]->Box.intersectsWithLine(midLine, lineVect, halfLength) == true)
					getTrianglesFromOctree(listTriangle, node->Childs[i], midLine, lineVect, halfLength);
			}
		}
	}
}