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
#include "CGGraph.h"

#include "Utils/CActivator.h"
#include "Components/CDependentComponent.h"

#include "GameObject/CGameObject.h"
#include "Selection/CSelection.h"

#include "Editor/CEditor.h"
#include "Handles/CHandles.h"

#ifdef BUILD_SKYLICHT_GRAPH

using namespace Graph;

namespace Skylicht
{
	namespace Editor
	{
		ACTIVATOR_REGISTER(CGGraph);

		DEPENDENT_COMPONENT(CGraphComponent, CGGraph);

		CGGraph::CGGraph() :
			m_graph(NULL)
		{
		}

		CGGraph::~CGGraph()
		{
		}

		void CGGraph::initComponent()
		{
			m_graph = m_gameObject->getComponent<CGraphComponent>();
		}

		void CGGraph::updateComponent()
		{
			CSelectObject* selectObject = CSelection::getInstance()->getLastSelected();
			if (selectObject && selectObject->getID() == m_gameObject->getID())
			{
				CMesh* mesh = m_graph->getNavMesh();

				SColor white(255, 255, 255, 255);
				CHandles* handles = CHandles::getInstance();

				for (u32 i = 0, n = mesh->getMeshBufferCount(); i < n; i++)
				{
					IMeshBuffer* mb = mesh->getMeshBuffer(i);
					IVertexBuffer* vb = mb->getVertexBuffer();
					IIndexBuffer* ib = mb->getIndexBuffer();

					for (u32 i = 0, n = ib->getIndexCount(); i < n; i += 3)
					{
						u32 a = ib->getIndex(i);
						u32 b = ib->getIndex(i + 1);
						u32 c = ib->getIndex(i + 2);

						S3DVertex* p1 = (S3DVertex*)vb->getVertex(a);
						S3DVertex* p2 = (S3DVertex*)vb->getVertex(b);
						S3DVertex* p3 = (S3DVertex*)vb->getVertex(c);

						handles->drawLine(p1->Pos, p2->Pos, white);
						handles->drawLine(p2->Pos, p3->Pos, white);
						handles->drawLine(p3->Pos, p1->Pos, white);
					}
				}
			}
		}
	}
}

#endif