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
	CPrimitiveRendererInstancing::CPrimitiveRendererInstancing() :
		m_group(NULL)
	{
	}

	CPrimitiveRendererInstancing::~CPrimitiveRendererInstancing()
	{
		for (auto& i : m_buffers)
		{
			SInstancingVertexBuffer* b = i.second;
			if (b)
			{
				if (b->Instancing)
					b->Instancing->drop();
				if (b->Transform)
					b->Transform->drop();
				if (b->IndirectLighting)
					b->IndirectLighting->drop();
				delete b;
			}
		}

		m_buffers.clear();
	}

	void CPrimitiveRendererInstancing::beginQuery(CEntityManager* entityManager)
	{
		for (auto& i : m_groups)
		{
			i.second.set_used(0);
		}

		if (m_group == NULL)
		{
			const u32 primitive[] = GET_LIST_ENTITY_DATA(CPrimiviteData);
			m_group = entityManager->createGroupFromVisible(primitive, 1);
		}
	}

	void CPrimitiveRendererInstancing::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		entities = m_group->getEntities();
		numEntity = m_group->getEntityCount();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CPrimiviteData* p = GET_ENTITY_DATA(entity, CPrimiviteData);

			if (p->Instancing &&
				p->Material &&
				p->Material->getShader() &&
				p->Material->getShader()->getInstancing() &&
				p->Material->getShader()->getInstancingShader())
			{
				CShader* shader = p->Material->getShader();
				IShaderInstancing* instancing = shader->getInstancing();

				CMesh* mesh = p->NormalMap ? m_meshTangent[p->Type] : m_mesh[p->Type];

				if (instancing->isSupport(mesh))
				{
					CIndirectLightingData* lighting = GET_ENTITY_DATA(entity, CIndirectLightingData);

					SShaderMesh shaderMesh;
					shaderMesh.Shader = shader;
					shaderMesh.Mesh = mesh;
					shaderMesh.IndirectLM = lighting->Type == CIndirectLightingData::LightmapArray ? NULL : lighting->IndirectTexture;
					shaderMesh.DirectLM = lighting->Type == CIndirectLightingData::LightmapArray ? NULL : lighting->LightTexture;

					for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
						shaderMesh.Textures[i] = p->Material->getTexture(i);

					ArrayPrimitives& list = m_groups[shaderMesh];
					list.push_back(p);

					// need create instancing mesh
					if (!mesh->UseInstancing)
					{
						mesh->UseInstancing = true;

						SInstancingVertexBuffer* buffer = new SInstancingVertexBuffer();

						buffer->Instancing = instancing->createInstancingVertexBuffer();
						buffer->Instancing->setHardwareMappingHint(EHM_STREAM);

						buffer->Transform = IShaderInstancing::createTransformVertexBuffer();
						buffer->Transform->setHardwareMappingHint(EHM_STREAM);

						buffer->IndirectLighting = IShaderInstancing::createIndirectLightingVertexBuffer();
						buffer->IndirectLighting->setHardwareMappingHint(EHM_STREAM);

						instancing->applyInstancing(
							mesh,
							buffer->Instancing,
							buffer->Transform
						);

						m_buffers[shaderMesh] = buffer;


						CMesh* indirectLightingMesh = mesh->clone();
						indirectLightingMesh->UseInstancing = true;
						indirectLightingMesh->removeAllMeshBuffer();

						u32 count = mesh->getMeshBufferCount();
						for (u32 i = 0; i < count; i++)
						{
							IMeshBuffer* lightingMeshBuffer = instancing->createLinkMeshBuffer(mesh->getMeshBuffer(i));
							indirectLightingMesh->addMeshBuffer(lightingMeshBuffer, "", NULL);
						}

						mesh->IndirectLightingMesh = indirectLightingMesh;

						instancing->applyInstancingForRenderLighting(
							indirectLightingMesh,
							buffer->IndirectLighting,
							buffer->Transform);
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

			m_materials.reset();
			m_entities.reset();

			CPrimiviteData** primitives = list.pointer();

			for (u32 i = 0, n = list.size(); i < n; i++)
			{
				CPrimiviteData* primitive = primitives[i];

				// add materials
				m_materials.push(primitive->Material);

				// add transform
				m_entities.push(primitive->Entity);
			}

			SInstancingVertexBuffer* buffer = m_buffers[it.first];

			// batching transform & material data to buffer
			instancing->batchIntancing(
				buffer->Instancing,
				m_materials.pointer(),
				m_entities.pointer(),
				m_entities.count());

			IShaderInstancing::batchTransformAndLighting(
				buffer->Transform,
				buffer->IndirectLighting,
				m_entities.pointer(),
				m_entities.count());
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
			ITexture* const* textures = it.first.Textures;

			if (!rp->canRenderShader(shader))
				continue;

			CShaderMaterial::setMaterial(NULL);

			for (u32 i = 0, n = mesh->MeshBuffers.size(); i < n; i++)
			{
				video::SMaterial& irrMat = mesh->getMeshBuffer(i)->getMaterial();

				for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
					irrMat.setTexture(i, textures[i]);

				rp->drawInstancingMeshBuffer(mesh, i, shader->getInstancingShader(), entityManager, -1, false);
			}
		}
	}
}