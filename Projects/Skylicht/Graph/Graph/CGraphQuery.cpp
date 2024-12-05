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

		CDistancePriorityQueue::CDistancePriorityQueue()
		{

		}

		CDistancePriorityQueue::~CDistancePriorityQueue()
		{

		}

		void CDistancePriorityQueue::push(const SDistanceTile& d)
		{
			SDistanceTile* a = m_queue.pointer();

			// find position
			int pos = -1;
			for (u32 i = 0, n = m_queue.size(); i < n; i++)
			{
				if (m_queue[i].Distance < d.Distance)
				{
					pos = i;
					break;
				}
			}

			if (pos == -1)
				pos = m_queue.size();

			// insert empty at last
			m_queue.push_back(SDistanceTile());

			// swap
			for (int i = m_queue.size() - 1; i > pos; i--)
			{
				SDistanceTile t = m_queue[i];
				m_queue[i] = m_queue[i - 1];
				m_queue[i - 1] = t;
			}

			m_queue[pos] = d;
		}

		void CDistancePriorityQueue::pop()
		{
			m_queue.set_used(m_queue.size() - 1);
		}

		const SDistanceTile& CDistancePriorityQueue::top()
		{
			return m_queue.getLast();
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

			m_root->Obstacle.addSegments(obstacle->getSegments());

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
			core::array<core::line3df> keepSegments;

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
					for (u32 i = 0, n = keepTriangles.size(); i < n; i++)
					{
						node->Triangles[i] = keepTriangles[i];
					}

					keepTriangles.clear();

					// step 3: collect obstacle segment
					core::array<core::line3df>& segs = node->Obstacle.getSegments();
					for (u32 i = 0, n = segs.size(); i < n; i++)
					{
						core::line3df& s = segs[i];

						if (childNode->OctreeBox.isPointInside(s.start) && childNode->OctreeBox.isPointInside(s.end))
						{
							childNode->Obstacle.addSegment(s.start, s.end);

							childNode->Box.addInternalPoint(s.start);
							childNode->Box.addInternalPoint(s.end);
						}
						else
						{
							keepSegments.push_back(s);
						}
					}

					// update current obstacle
					node->Obstacle.clear();
					node->Obstacle.addSegments(keepSegments);
					keepSegments.clear();

					if (childNode->Triangles.empty() && childNode->Obstacle.empty())
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

			core::array<core::triangle3df*> triangles;

			// step 1: get list triangle collide with ray
			getTrianglesFromOctree(triangles, m_root, mid, vec, halfLength);

			// step 2: find nearest triangle
			s32 cnt = (s32)triangles.size();
			core::triangle3df** listTris = triangles.pointer();

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

		void CGraphQuery::getTriangles(const core::aabbox3df& box, core::array<core::triangle3df*>& result)
		{
			if (m_root == NULL)
				return;

			getTrianglesFromOctree(result, m_root, box);
		}

		void CGraphQuery::getObstacles(const core::aabbox3df& box, CObstacleAvoidance& obstacle)
		{
			if (m_root == NULL)
				return;

			getObstacleFromOctree(obstacle, m_root, box);
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

		void CGraphQuery::getTrianglesFromOctree(
			core::array<core::triangle3df*>& listTriangle,
			COctreeNode* node,
			const core::aabbox3df& box)
		{
			u32 cnt = node->Triangles.size();
			u32 i = 0;

			core::triangle3df* listTriID = node->Triangles.pointer();

			// optimize function core::array::insert
			int n = listTriangle.size();
			listTriangle.set_used(n + cnt);
			int used = 0;

			core::triangle3df** p = listTriangle.pointer();

			// list triangles
			for (i = 0; i < cnt; ++i)
			{
				core::triangle3df* triangle = &listTriID[i];

				// if triangle collide the bbox
				if (!triangle->isTotalOutsideBox(box))
				{
					p[n + used] = triangle;
					used++;
				}
			}

			// update the used
			listTriangle.set_used(n + used);

			for (i = 0; i < 8; ++i)
			{
				if (node->Childs[i] && node->Childs[i]->Box.intersectsWithBox(box))
					getTrianglesFromOctree(listTriangle, node->Childs[i], box);
			}
		}

		void CGraphQuery::getObstacleFromOctree(
			CObstacleAvoidance& obstacle,
			COctreeNode* node,
			const core::aabbox3df& box)
		{
			core::array<core::line3df>& segs = node->Obstacle.getSegments();
			for (u32 i = 0, n = segs.size(); i < n; i++)
			{
				core::line3df& s = segs[i];
				if (box.intersectsWithLine(s))
				{
					obstacle.addSegment(s.start, s.end);
				}
			}

			for (u32 i = 0; i < 8; ++i)
			{
				if (node->Childs[i] && node->Childs[i]->Box.intersectsWithBox(box))
					getObstacleFromOctree(obstacle, node->Childs[i], box);
			}
		}

		bool CGraphQuery::findPath(CWalkingTileMap* map, STile* from, STile* to, core::array<STile*>& result)
		{
			result.clear();
			u32 numTile = map->getNumTile();

			CDistancePriorityQueue queue;

			std::vector<STile*> prev(numTile, NULL);
			std::vector<float> dist(numTile, -1.0);

			float length = (to->Position - from->Position).getLengthSQ();
			queue.push({ length, from });

			dist[from->Id] = 0.0f;

			map->resetVisit();

			while (!queue.empty())
			{
				const SDistanceTile& t = queue.top();

				STile* tile = t.Tile;
				queue.pop();

				if (tile->Visit)
					continue;

				tile->Visit = true;
				if (tile == to)
					break;

				float walkDistance = dist[tile->Id];

				for (u32 i = 0, n = tile->Neighbours.size(); i < n; i++)
				{
					STile* nei = tile->Neighbours[i];

					float neiDist = (nei->Position - tile->Position).getLength();
					float currentDist = walkDistance + neiDist;

					if (dist[nei->Id] < 0 || dist[nei->Id] > currentDist)
					{
						// if not yet calc dist of nei
						// or have another link shorter
						dist[nei->Id] = currentDist;

						float length = currentDist + (to->Position - nei->Position).getLength();
						queue.push({ length, nei });

						prev[nei->Id] = tile;
					}
				}
			}

			map->resetVisit();

			if (prev[to->Id] == NULL)
				return false;

			STile* u = to;
			result.push_back(u);
			while (u != from) {
				result.push_back(prev[u->Id]);
				u = prev[u->Id];
			}

			// reverse result
			u32 count = result.size();
			for (u32 i = 0, n = count / 2; i < n; i++)
			{
				STile* temp = result[count - i - 1];
				result[count - i - 1] = result[i];
				result[i] = temp;
			}

			return true;
		}
	}
}