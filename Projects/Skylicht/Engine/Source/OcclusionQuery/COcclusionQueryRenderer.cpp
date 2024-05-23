/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "COcclusionQueryRenderer.h"
#include "RenderMesh/CMesh.h"
#include "Entity/CEntityManager.h"
#include "Material/Shader/CShaderManager.h"

namespace Skylicht
{
	COcclusionQueryRenderer::COcclusionQueryRenderer() :
		m_group(NULL),
		m_queryDelay(0.0f),
		m_timeDelay(0.0f)
	{
		m_renderPass = IRenderSystem::OcclusionQuery;

		IVideoDriver* driver = getVideoDriver();
		IMeshBuffer* meshBuffer = new CMeshBuffer<S3DVertex>(driver->getVertexDescriptor(EVT_STANDARD), EIT_16BIT);
		IIndexBuffer* ib = meshBuffer->getIndexBuffer();
		IVertexBuffer* vb = meshBuffer->getVertexBuffer();

		// Create vertices
		video::SColor clr(255, 255, 255, 255);

		vb->reallocate(12);

		video::S3DVertex Vertices[] = {
			// back
			video::S3DVertex(0, 0, 0, 0, 0, -1, clr, 0, 1),
			video::S3DVertex(1, 0, 0, 0, 0, -1, clr, 1, 1),
			video::S3DVertex(1, 1, 0, 0, 0, -1, clr, 1, 0),
			video::S3DVertex(0, 1, 0, 0, 0, -1, clr, 0, 0),

			// front
			video::S3DVertex(0, 0, 1, 0, 0, 1, clr, 0, 1),
			video::S3DVertex(1, 0, 1, 0, 0, 1, clr, 1, 1),
			video::S3DVertex(1, 1, 1, 0, 0, 1, clr, 1, 0),
			video::S3DVertex(0, 1, 1, 0, 0, 1, clr, 0, 0),

			// bottom
			video::S3DVertex(0, 0, 0, 0, -1, 0, clr, 0, 1),
			video::S3DVertex(0, 0, 1, 0, -1, 0, clr, 1, 1),
			video::S3DVertex(1, 0, 1, 0, -1, 0, clr, 1, 0),
			video::S3DVertex(1, 0, 0, 0, -1, 0, clr, 0, 0),

			// top
			video::S3DVertex(0, 1, 0, 0, 1, 0, clr, 0, 1),
			video::S3DVertex(0, 1, 1, 0, 1, 0, clr, 1, 1),
			video::S3DVertex(1, 1, 1, 0, 1, 0, clr, 1, 0),
			video::S3DVertex(1, 1, 0, 0, 1, 0, clr, 0, 0),

			// left
			video::S3DVertex(1, 0, 0, 1, 0, 0, clr, 0, 1),
			video::S3DVertex(1, 0, 1, 1, 0, 0, clr, 1, 1),
			video::S3DVertex(1, 1, 1, 1, 0, 0, clr, 1, 0),
			video::S3DVertex(1, 1, 0, 1, 0, 0, clr, 0, 0),

			// right
			video::S3DVertex(0, 0, 0, -1, 0, 0, clr, 0, 1),
			video::S3DVertex(0, 0, 1, -1, 0, 0, clr, 1, 1),
			video::S3DVertex(0, 1, 1, -1, 0, 0, clr, 1, 0),
			video::S3DVertex(0, 1, 0, -1, 0, 0, clr, 0, 0),
		};

		for (u32 i = 0; i < 24; ++i)
		{
			Vertices[i].Pos -= core::vector3df(0.5f, 0.5f, 0.5f);
			vb->addVertex(&Vertices[i]);
		}

		// cube mesh
		// Create indices
		const u16 u[36] =
		{
			// back
			0,2,1,
			0,3,2,

			// front
			4,5,6,
			4,6,7,

			// bottom
			8,10,9,
			8,11,10,

			// top
			12,13,14,
			12,14,15,

			// left
			16,18,17,
			16,19,18,

			// right
			20,21,22,
			20,22,23
		};

		ib->set_used(36);

		for (u32 i = 0; i < 36; ++i)
			ib->setIndex(i, u[i]);

		meshBuffer->recalculateBoundingBox();
		meshBuffer->getMaterial().MaterialType = CShaderManager::getInstance()->getShaderIDByName("VertexColor");

		m_cubeMesh = new CMesh();
		m_cubeMesh->addMeshBuffer(meshBuffer);
		m_cubeMesh->recalculateBoundingBox();
		m_cubeMesh->setHardwareMappingHint(EHM_STATIC);

		meshBuffer->drop();
	}

	COcclusionQueryRenderer::~COcclusionQueryRenderer()
	{
		m_cubeMesh->drop();
	}

	void COcclusionQueryRenderer::beginQuery(CEntityManager* entityManager)
	{
		if (m_group == NULL)
		{
			const u32 type[] = GET_LIST_ENTITY_DATA(COcclusionQueryData);
			m_group = entityManager->createGroupFromVisible(type, 1);
		}

		m_query.reset();
		m_transforms.reset();
	}

	void COcclusionQueryRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		entities = m_group->getEntities();
		numEntity = m_group->getEntityCount();

		for (int i = 0; i < numEntity; i++)
		{
			m_query.push(GET_ENTITY_DATA(entities[i], COcclusionQueryData));
			m_transforms.push(GET_ENTITY_DATA(entities[i], CWorldTransformData));
		}
	}

	void COcclusionQueryRenderer::init(CEntityManager* entityManager)
	{

	}

	void COcclusionQueryRenderer::update(CEntityManager* entityManager)
	{
		COcclusionQueryData** queryData = m_query.pointer();
		CWorldTransformData** transforms = m_transforms.pointer();

		int numEntity = m_query.count();

		for (int i = 0; i < numEntity; i++)
		{
			COcclusionQueryData* data = queryData[i];
			CWorldTransformData* transform = transforms[i];

			if (transform->NeedValidate || data->NeedValidate)
			{
				data->getSceneNode()->updateTransform(transform->World);
				data->NeedValidate = false;
			}

			if (!data->isRegisterQuery())
				data->registerQuery(m_cubeMesh);
		}
	}

	void COcclusionQueryRenderer::render(CEntityManager* entityManager)
	{
		IVideoDriver* driver = getVideoDriver();

		COcclusionQueryData** queryData = m_query.pointer();
		CWorldTransformData** transforms = m_transforms.pointer();
		int numEntity = m_query.count();

		m_timeDelay = m_timeDelay - getTimeStep();

		if (numEntity > 0 && m_timeDelay <= 0)
		{
			m_timeDelay = m_queryDelay;

			driver->runAllOcclusionQueries(false);
			driver->updateAllOcclusionQueries();

			for (int i = 0; i < numEntity; i++)
			{
				COcclusionQueryData* data = queryData[i];
				data->QueryResult = driver->getOcclusionQueryResult(data->getSceneNode());
				data->QueryVisible = data->QueryResult > 0;
			}
		}
	}
}