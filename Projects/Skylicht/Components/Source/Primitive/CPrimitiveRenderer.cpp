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

#include "Material/Shader/ShaderCallback/CShaderSH.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"

namespace Skylicht
{
	CPrimitiveRenderer::CPrimitiveRenderer() :
		m_group(NULL)
	{

	}

	CPrimitiveRenderer::~CPrimitiveRenderer()
	{

	}

	void CPrimitiveRenderer::beginQuery(CEntityManager* entityManager)
	{
		for (int i = 0; i < CPrimiviteData::Count; i++)
		{
			m_primitives[i].reset();
			m_primitivesTangent[i].reset();
		}

		if (m_group == NULL)
		{
			const u32 primitive[] = GET_LIST_ENTITY_DATA(CPrimiviteData);
			m_group = entityManager->createGroupFromVisible(primitive, 1);
		}
	}

	void CPrimitiveRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		entities = m_group->getEntities();
		numEntity = m_group->getEntityCount();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CPrimiviteData* p = GET_ENTITY_DATA(entity, CPrimiviteData);
			if (!p->Instancing)
			{
				if (p->NormalMap)
					m_primitivesTangent[p->Type].push(p);
				else
					m_primitives[p->Type].push(p);
			}
		}
	}

	void CPrimitiveRenderer::init(CEntityManager* entityManager)
	{

	}

	int cmpPrimitiveFunc(const void* a, const void* b)
	{
		CPrimiviteData* pa = *((CPrimiviteData**)a);
		CPrimiviteData* pb = *((CPrimiviteData**)b);

		ITexture* textureA = pa->Material->getTexture(0);
		ITexture* textureB = pb->Material->getTexture(0);

		// if no texture
		if (textureA == NULL || textureB == NULL)
		{
			if (pa->Material == pb->Material)
				return 0;
			return pa->Material < pb->Material ? -1 : 1;
		}

		// sort by texture 0
		if (textureA == textureB)
			return 0;
		return textureA < textureB ? -1 : 1;
	}

	void CPrimitiveRenderer::update(CEntityManager* entityManager)
	{
		for (int type = 0; type < CPrimiviteData::Count; type++)
		{
			if (m_mesh[type] == NULL)
				continue;

			CFastArray<CPrimiviteData*>& primitives = m_primitives[type];
			u32 count = primitives.count();

			// need sort by material
			qsort(primitives.pointer(), count, sizeof(CPrimiviteData*), cmpPrimitiveFunc);
		}

		for (int type = 0; type < CPrimiviteData::Count; type++)
		{
			if (m_meshTangent[type] == NULL)
				continue;

			CFastArray<CPrimiviteData*>& primitives = m_primitivesTangent[type];
			u32 count = primitives.count();

			// need sort by material
			qsort(primitives.pointer(), count, sizeof(CPrimiviteData*), cmpPrimitiveFunc);
		}
	}

	void CPrimitiveRenderer::render(CEntityManager* entityManager)
	{
		// render no-tangent
		for (int type = 0; type < CPrimiviteData::Count; type++)
		{
			if (m_mesh[type] == NULL)
				continue;

			renderPrimitive(
				entityManager,
				m_primitives[type].pointer(),
				m_mesh[type],
				m_primitives[type].count()
			);
		}

		// render tangent
		for (int type = 0; type < CPrimiviteData::Count; type++)
		{
			if (m_meshTangent[type] == NULL)
				continue;

			renderPrimitive(
				entityManager,
				m_primitivesTangent[type].pointer(),
				m_meshTangent[type],
				m_primitivesTangent[type].count()
			);
		}
	}

	CMesh* CPrimitiveRenderer::getMesh(CPrimiviteData::EPrimitive type)
	{
		return m_meshTangent[type];
	}

	void CPrimitiveRenderer::renderPrimitive(CEntityManager* entityManager,
		CPrimiviteData** primitives,
		CMesh* mesh,
		int numEntity)
	{
		IVideoDriver* driver = getVideoDriver();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		CMaterial* mat = NULL;

		for (int i = 0; i < numEntity; i++)
		{
			CPrimiviteData* data = primitives[i];

			CWorldTransformData* transform = GET_ENTITY_DATA(data->Entity, CWorldTransformData);

			if (data->Material == NULL || !rp->canRenderMaterial(data->Material))
				continue;

			if (mat != data->Material)
			{
				mat = data->Material;

				for (u32 i = 0, n = mesh->MeshBuffers.size(); i < n; i++)
				{
					mat->updateTexture(mesh->MeshBuffers[i]->getMaterial());
				}
			}

			driver->setTransform(video::ETS_WORLD, transform->World);

			CShaderMaterial::setMaterial(mat);

			CIndirectLightingData* lightingData = GET_ENTITY_DATA(data->Entity, CIndirectLightingData);
			if (lightingData != NULL)
			{
				if (lightingData->Type == CIndirectLightingData::SH9)
					CShaderSH::setSH9(lightingData->SH);
				else if (lightingData->Type == CIndirectLightingData::AmbientColor)
					CShaderLighting::setLightAmbient(lightingData->Color);
			}

			for (u32 i = 0, n = mesh->MeshBuffers.size(); i < n; i++)
			{
				mesh->Materials[i] = mat;

				rp->drawMeshBuffer(mesh, i, entityManager, data->EntityIndex, false);

				mesh->Materials[i] = NULL;
			}
		}
	}
}