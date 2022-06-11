/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CPrimitiveRenderer.h"
#include "Entity/CEntityManager.h"
#include "Culling/CVisibleData.h"
#include "Transform/CWorldTransformData.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"

namespace Skylicht
{
	CPrimitiveRenderer::CPrimitiveRenderer()
	{
		m_pipelineType = IRenderPipeline::Mix;

		for (int i = 0; i < (int)CPrimiviteData::Count; i++)
			m_mesh[i] = NULL;

		const IGeometryCreator* geometry = getIrrlichtDevice()->getSceneManager()->getGeometryCreator();

		// cube mesh
		{
			initCube();
		}

		// sphere mesh
		{
			IMesh* m = geometry->createSphereMesh(0.5f);
			initMesh(m, CPrimiviteData::Sphere);
			m->drop();
		}
	}

	CPrimitiveRenderer::~CPrimitiveRenderer()
	{
		for (int i = 0; i < (int)CPrimiviteData::Count; i++)
		{
			if (m_mesh[i])
				m_mesh[i]->drop();
		}
	}

	void CPrimitiveRenderer::initCube()
	{
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
			video::S3DVertex(0, 1, 0, 0, 0, -1, clr, 1, 0),

			// front
			video::S3DVertex(0, 0, 1, 0, 0, 1, clr, 0, 1),
			video::S3DVertex(1, 0, 1, 0, 0, 1, clr, 1, 1),
			video::S3DVertex(1, 1, 1, 0, 0, 1, clr, 1, 0),
			video::S3DVertex(0, 1, 1, 0, 0, 1, clr, 1, 0),

			// bottom
			video::S3DVertex(0, 0, 0, 0, -1, 0, clr, 0, 1),
			video::S3DVertex(0, 0, 1, 0, -1, 0, clr, 1, 1),
			video::S3DVertex(1, 0, 1, 0, -1, 0, clr, 1, 0),
			video::S3DVertex(1, 0, 0, 0, -1, 0, clr, 1, 0),

			// top
			video::S3DVertex(0, 1, 0, 0, 1, 0, clr, 0, 1),
			video::S3DVertex(0, 1, 1, 0, 1, 0, clr, 1, 1),
			video::S3DVertex(1, 1, 1, 0, 1, 0, clr, 1, 0),
			video::S3DVertex(1, 1, 0, 0, 1, 0, clr, 1, 0),

			// left
			video::S3DVertex(1, 0, 0, 1, 0, 0, clr, 0, 1),
			video::S3DVertex(1, 0, 1, 1, 0, 0, clr, 1, 1),
			video::S3DVertex(1, 1, 1, 1, 0, 0, clr, 1, 0),
			video::S3DVertex(1, 1, 0, 1, 0, 0, clr, 1, 0),

			// right
			video::S3DVertex(0, 0, 0, -1, 0, 0, clr, 0, 1),
			video::S3DVertex(0, 0, 1, -1, 0, 0, clr, 1, 1),
			video::S3DVertex(0, 1, 1, -1, 0, 0, clr, 1, 0),
			video::S3DVertex(0, 1, 0, -1, 0, 0, clr, 1, 0),
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

		CMesh* mesh = new CMesh();
		mesh->addMeshBuffer(meshBuffer);

		meshBuffer->recalculateBoundingBox();
		mesh->recalculateBoundingBox();
		mesh->setHardwareMappingHint(EHM_STATIC);

		m_mesh[CPrimiviteData::Cube] = mesh;

		meshBuffer->drop();
	}

	void CPrimitiveRenderer::initMesh(IMesh* primitiveMesh, CPrimiviteData::EPrimitive primitive)
	{
		CMesh* mesh = new CMesh();

		IMeshBuffer* mb = primitiveMesh->getMeshBuffer(0);
		mesh->addMeshBuffer(mb);

		mb->recalculateBoundingBox();
		mesh->recalculateBoundingBox();
		mesh->setHardwareMappingHint(EHM_STATIC);

		m_mesh[primitive] = mesh;
	}

	void CPrimitiveRenderer::beginQuery(CEntityManager* entityManager)
	{
		for (int i = 0; i < CPrimiviteData::Count; i++)
		{
			m_primitives[i].set_used(0);
			m_transforms[i].set_used(0);
		}
	}

	void CPrimitiveRenderer::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CPrimiviteData* p = (CPrimiviteData*)entity->getDataByIndex(CPrimiviteData::DataTypeIndex);
		if (p != NULL)
		{
			CVisibleData* visible = (CVisibleData*)entity->getDataByIndex(CVisibleData::DataTypeIndex);
			if (visible->Visible)
			{
				m_primitives[p->Type].push_back(p);
			}
		}
	}

	void CPrimitiveRenderer::init(CEntityManager* entityManager)
	{

	}

	int cmpfunc(const void* a, const void* b)
	{
		CPrimiviteData* pa = *((CPrimiviteData**)a);
		CPrimiviteData* pb = *((CPrimiviteData**)b);

		return pa->Material > pb->Material;
	}

	void CPrimitiveRenderer::update(CEntityManager* entityManager)
	{
		for (int type = 0; type < CPrimiviteData::Count; type++)
		{
			if (m_mesh[type] == NULL)
				continue;

			core::array<CPrimiviteData*>& primitives = m_primitives[type];
			u32 count = primitives.size();

			// need sort by material
			qsort(primitives.pointer(), count, sizeof(CPrimiviteData*), cmpfunc);

			// get world transform			
			for (u32 i = 0; i < count; i++)
			{
				CEntity* entity = entityManager->getEntity(primitives[i]->EntityIndex);

				CWorldTransformData* transform = (CWorldTransformData*)entity->getDataByIndex(CWorldTransformData::DataTypeIndex);
				m_transforms[type].push_back(transform->World);
			}
		}
	}

	void CPrimitiveRenderer::render(CEntityManager* entityManager)
	{
		for (int type = 0; type < CPrimiviteData::Count; type++)
		{
			if (m_mesh[type] == NULL)
				continue;

			renderPrimitive(
				entityManager,
				m_primitives[type],
				m_transforms[type],
				m_mesh[type]
			);
		}
	}

	void CPrimitiveRenderer::renderPrimitive(
		CEntityManager* entityManager,
		core::array<CPrimiviteData*>& primitives,
		core::array<core::matrix4>& transforms,
		CMesh* mesh)
	{
		IVideoDriver* driver = getVideoDriver();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		CMaterial* mat = NULL;

		u32 count = transforms.size();
		for (u32 i = 0; i < count; i++)
		{
			CPrimiviteData* data = primitives[i];

			if (data->Material == NULL || !rp->canRenderMaterial(data->Material))
				continue;

			if (mat != data->Material)
			{
				mat = data->Material;
				CShaderMaterial::setMaterial(data->Material);

				for (u32 i = 0, n = mesh->MeshBuffers.size(); i < n; i++)
				{
					IMeshBuffer* mb = mesh->MeshBuffers[i];
					mat->applyMaterial(mb->getMaterial());
				}
			}

			core::matrix4& world = transforms[i];
			driver->setTransform(video::ETS_WORLD, world);

			for (u32 i = 0, n = mesh->MeshBuffers.size(); i < n; i++)
				rp->drawMeshBuffer(mesh, i, entityManager, data->EntityIndex, false);
		}
	}
}