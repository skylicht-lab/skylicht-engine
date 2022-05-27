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
			IMesh* m = geometry->createCubeMesh(core::vector3df(1.0f, 1.0f, 1.0f));
			initMesh(m, CPrimiviteData::Cube);
			m->drop();
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