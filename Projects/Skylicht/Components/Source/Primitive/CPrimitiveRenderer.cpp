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

	}

	CPrimitiveRenderer::~CPrimitiveRenderer()
	{

	}

	void CPrimitiveRenderer::beginQuery(CEntityManager* entityManager)
	{
		for (int i = 0; i < CPrimiviteData::Count; i++)
		{
			m_primitives[i].set_used(0);
			m_transforms[i].set_used(0);
		}
	}

	void CPrimitiveRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CPrimiviteData* p = GET_ENTITY_DATA(entity, CPrimiviteData);
			if (p != NULL)
			{
				CVisibleData* visible = GET_ENTITY_DATA(entity, CVisibleData);
				if (visible->Visible && !p->Instancing)
				{
					m_primitives[p->Type].push_back(p);
				}
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

			core::array<CPrimiviteData*>& primitives = m_primitives[type];
			u32 count = primitives.size();

			// need sort by material
			qsort(primitives.pointer(), count, sizeof(CPrimiviteData*), cmpPrimitiveFunc);

			// get world transform			
			for (u32 i = 0; i < count; i++)
			{
				CEntity* entity = entityManager->getEntity(primitives[i]->EntityIndex);

				CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
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