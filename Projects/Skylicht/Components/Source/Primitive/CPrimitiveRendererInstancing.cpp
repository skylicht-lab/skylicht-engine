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
#include "CPrimitiveRendererInstancing.h"
#include "Entity/CEntityManager.h"
#include "Culling/CVisibleData.h"
#include "Transform/CWorldTransformData.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"

namespace Skylicht
{
	CPrimitiveRendererInstancing::CPrimitiveRendererInstancing()
	{
	}

	CPrimitiveRendererInstancing::~CPrimitiveRendererInstancing()
	{
		for (auto& i : m_buffers)
		{
			if (i.second)
				i.second->drop();
		}
	}

	void CPrimitiveRendererInstancing::beginQuery(CEntityManager* entityManager)
	{
		for (auto& i : m_groups)
		{
			i.second.set_used(0);
		}
	}

	void CPrimitiveRendererInstancing::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CPrimiviteData* p = GET_ENTITY_DATA(entity, CPrimiviteData);
			if (p != NULL)
			{
				CVisibleData* visible = GET_ENTITY_DATA(entity, CVisibleData);
				if (visible->Visible &&
					p->Instancing &&
					p->Material &&
					p->Material->getShader() &&
					p->Material->getShader()->getInstancing() &&
					p->Material->getShader()->getInstancingShader())
				{
					CShader* shader = p->Material->getShader();
					IShaderInstancing* instancing = shader->getInstancing();

					CMesh* mesh = m_mesh[p->Type];

					if (instancing->isSupport(mesh))
					{
						CIndirectLightingData* lighting = GET_ENTITY_DATA(entity, CIndirectLightingData);

						SShaderMesh shaderMesh;
						shaderMesh.Shader = shader;
						shaderMesh.Mesh = mesh;
						shaderMesh.Lightmap = lighting->Type == CIndirectLightingData::LightmapArray ? NULL : lighting->LightmapTexture;

						for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
							shaderMesh.Textures[i] = p->Material->getTexture(i);

						ArrayPrimitives& list = m_groups[shaderMesh];
						list.push_back(p);

						if (!mesh->UseInstancing)
						{
							mesh->UseInstancing = true;

							IVertexBuffer* buffer = instancing->createInstancingMeshBuffer();
							buffer->setHardwareMappingHint(EHM_STREAM);

							instancing->applyInstancing(mesh, buffer);

							m_buffers[shaderMesh] = buffer;
						}
					}
				}
			}
		}
	}

	void CPrimitiveRendererInstancing::init(CEntityManager* entityManager)
	{

	}

	void CPrimitiveRendererInstancing::update(CEntityManager* entityManager)
	{
		for (auto& it : m_groups)
		{
			ArrayPrimitives& list = it.second;
			if (list.size() == 0)
				continue;

			IShaderInstancing* instancing = it.first.Shader->getInstancing();
			IVertexBuffer* buffer = m_buffers[it.first];

			m_materials.set_used(0);
			m_transforms.set_used(0);
			m_indirectLightings.set_used(0);

			CPrimiviteData** primitives = list.pointer();

			for (u32 i = 0, n = list.size(); i < n; i++)
			{
				CPrimiviteData* primitive = primitives[i];

				// add materials
				m_materials.push_back(primitive->Material);

				// add transform
				CEntity* entity = entityManager->getEntity(primitive->EntityIndex);
				CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				CIndirectLightingData* lighting = GET_ENTITY_DATA(entity, CIndirectLightingData);

				m_transforms.push_back(transform);
				m_indirectLightings.push_back(lighting);
			}

			// batching transform & material data to buffer
			instancing->batchIntancing(buffer, m_materials, m_transforms, m_indirectLightings);
		}
	}

	void CPrimitiveRendererInstancing::render(CEntityManager* entityManager)
	{
		IVideoDriver* driver = getVideoDriver();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		for (auto& it : m_groups)
		{
			ArrayPrimitives& list = it.second;
			if (list.size() == 0)
				continue;

			CMesh* mesh = it.first.Mesh;
			CShader* shader = it.first.Shader;
			int materialType = shader->getInstancingShader()->getMaterialRenderID();

			if (!rp->canRenderShader(shader))
				continue;

			CShaderMaterial::setMaterial(NULL);

			for (u32 i = 0, n = mesh->MeshBuffers.size(); i < n; i++)
			{
				rp->drawInstancingMeshBuffer(mesh, i, materialType, entityManager, false);
			}
		}
	}
}