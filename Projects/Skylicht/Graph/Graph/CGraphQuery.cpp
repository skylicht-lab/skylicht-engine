#include "pch.h"
#include "CGraphQuery.h"

namespace Skylicht
{
	namespace Graph
	{
		COctreeNode::COctreeNode() :
			Parent(NULL)
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
			m_root(NULL)
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

		void CGraphQuery::buildIndexNavMesh(CMesh* navMesh, CObstacleAvoidance* obstacle, float minCellSize)
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
					m_root->Box.reset(a->Pos);
				else
				{
					m_root->Box.addInternalPoint(a->Pos);
					m_root->Box.addInternalPoint(b->Pos);
					m_root->Box.addInternalPoint(c->Pos);
				}

				core::triangle3df tri(a->Pos, b->Pos, c->Pos);
				m_root->Triangles.push_back(tri);
			}
		}

		void CGraphQuery::constructOctree(COctreeNode* node)
		{

		}
	}
}