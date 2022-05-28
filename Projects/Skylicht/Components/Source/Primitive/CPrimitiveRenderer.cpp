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
		{
			m_mesh[i] = NULL;
			m_materials[i] = NULL;
		}

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

			if (m_materials[i])
				m_materials[i]->drop();
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
			video::S3DVertex(1, 0, 0, -1, 0, 0, clr, 0, 1),
			video::S3DVertex(1, 0, 1, -1, 0, 0, clr, 1, 1),
			video::S3DVertex(1, 1, 1, -1, 0, 0, clr, 1, 0),
			video::S3DVertex(1, 1, 0, -1, 0, 0, clr, 1, 0),

			// right
			video::S3DVertex(0, 0, 0, 1, 0, 0, clr, 0, 1),
			video::S3DVertex(0, 0, 1, 1, 0, 0, clr, 1, 1),
			video::S3DVertex(0, 1, 1, 1, 0, 0, clr, 1, 0),
			video::S3DVertex(0, 1, 0, 1, 0, 0, clr, 1, 0),
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
		CMaterial* mat = new CMaterial("Primitive", "BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");

		mesh->addMeshBuffer(meshBuffer, "", mat);

		mat->setUniform4("uColor", SColor(255, 180, 180, 180));
		mat->addAffectMesh(meshBuffer);
		mat->applyMaterial();

		meshBuffer->recalculateBoundingBox();
		mesh->recalculateBoundingBox();

		m_mesh[CPrimiviteData::Cube] = mesh;
		m_materials[CPrimiviteData::Cube] = mat;

		meshBuffer->drop();
	}

	void CPrimitiveRenderer::initMesh(IMesh* primitiveMesh, CPrimiviteData::EPrimitive primitive)
	{
		CMesh* mesh = new CMesh();
		CMaterial* mat = new CMaterial("Primitive", "BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");

		IMeshBuffer* mb = primitiveMesh->getMeshBuffer(0);
		mesh->addMeshBuffer(mb, "", mat);

		mat->setUniform4("uColor", SColor(255, 180, 180, 180));
		mat->addAffectMesh(mb);
		mat->applyMaterial();

		mb->recalculateBoundingBox();
		mesh->recalculateBoundingBox();

		m_mesh[primitive] = mesh;
		m_materials[primitive] = mat;
	}

	void CPrimitiveRenderer::beginQuery(CEntityManager* entityManager)
	{
		for (int i = 0; i < CPrimiviteData::Count; i++)
		{
			m_transforms[i].set_used(0);
			m_primitives[i].set_used(0);
		}
	}

	void CPrimitiveRenderer::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CPrimiviteData* p = (CPrimiviteData*)entity->getDataByIndex(CPrimiviteData::DataTypeIndex);
		if (p != NULL)
		{
			CVisibleData* visible = (CVisibleData*)entity->getDataByIndex(CVisibleData::DataTypeIndex);
			CWorldTransformData* transformData = (CWorldTransformData*)entity->getDataByIndex(CWorldTransformData::DataTypeIndex);

			if (visible->Visible)
			{
				m_transforms[p->Type].push_back(transformData->World);
				m_primitives[p->Type].push_back(p);
			}
		}
	}

	void CPrimitiveRenderer::init(CEntityManager* entityManager)
	{

	}

	void CPrimitiveRenderer::update(CEntityManager* entityManager)
	{

	}

	void CPrimitiveRenderer::render(CEntityManager* entityManager)
	{
		for (int type = 0; type < CPrimiviteData::Count; type++)
		{
			if (m_mesh[type] == NULL || m_materials[type] == NULL)
				continue;

			renderPrimitive(
				entityManager,
				m_primitives[type],
				m_transforms[type],
				m_mesh[type],
				m_materials[type]
			);
		}
	}

	void CPrimitiveRenderer::renderPrimitive(
		CEntityManager* entityManager,
		core::array<CPrimiviteData*>& primitives,
		core::array<core::matrix4>& transforms,
		CMesh* mesh,
		CMaterial* material)
	{
		IVideoDriver* driver = getVideoDriver();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		if (!rp->canRenderMaterial(material))
			return;

		u32 count = transforms.size();
		for (u32 i = 0; i < count; i++)
		{
			core::matrix4& world = transforms[i];
			CPrimiviteData* data = primitives[i];
			driver->setTransform(video::ETS_WORLD, world);

			for (u32 i = 0, n = mesh->MeshBuffers.size(); i < n; i++)
				rp->drawMeshBuffer(mesh, i, entityManager, data->EntityIndex);
		}
	}
}