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
		for (SPrimitiveMeshInstancing* i : m_meshInstancing)
		{
			i->Mesh->drop();
			delete i;
		}

		for (auto& i : m_groups)
		{
			SPrimitiveGroup* g = i.second;

			SInstancingVertexBuffer* buffer = g->Buffer;
			if (buffer)
			{
				if (buffer->Instancing)
					buffer->Instancing->drop();
				if (buffer->Transform)
					buffer->Transform->drop();
				if (buffer->IndirectLighting)
					buffer->IndirectLighting->drop();
				delete buffer;
			}

			delete g;
		}

		m_meshInstancing.clear();
		m_groups.clear();

	}

	void CPrimitiveRendererInstancing::beginQuery(CEntityManager* entityManager)
	{
		for (auto& i : m_groups)
		{
			SPrimitiveGroup* g = i.second;
			g->Array.set_used(0);
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

			video::E_VERTEX_TYPE vtxType = p->NormalMap ? video::EVT_TANGENTS : video::EVT_STANDARD;

			if (p->Instancing && p->Material)
			{
				CShader* shader = p->Material->getShader();
				if (shader &&
					shader->getInstancing(vtxType) &&
					shader->getInstancingShader(vtxType))
				{
					IShaderInstancing* instancingShader = shader->getInstancing(vtxType);

					if (p->InstancingMesh == NULL)
					{
						for (SPrimitiveMeshInstancing* i : m_meshInstancing)
						{
							if (i->InstancingShader == instancingShader &&
								i->HaveTangent == p->NormalMap &&
								i->PrimitiveType == p->Type)
							{
								p->InstancingMesh = i;
								break;
							}
						}

						if (!p->InstancingMesh)
						{
							CMesh* mesh = initMesh(p->Type, p->NormalMap);

							SPrimitiveMeshInstancing* newInstancing = new SPrimitiveMeshInstancing();
							newInstancing->InstancingShader = instancingShader;
							newInstancing->Mesh = mesh;
							newInstancing->HaveTangent = p->NormalMap;
							newInstancing->PrimitiveType = p->Type;

							p->InstancingMesh = newInstancing;

							m_meshInstancing.push_back(newInstancing);
						}
					}

					if (p->InstancingMesh && !p->InstancingGroup)
					{
						SPrimitiveMeshInstancing* instancingMesh = p->InstancingMesh;
						CMesh* mesh = instancingMesh->Mesh;

						SPrimitiveGroup* group = getGroup(entity, shader, mesh);
						if (group->Buffer == NULL)
						{
							if (instancingShader->isSupport(mesh))
							{
								SInstancingVertexBuffer* buffer = new SInstancingVertexBuffer();

								buffer->BaseVertexType = mesh->getVertexType();
								buffer->Instancing = instancingShader->createInstancingVertexBuffer();
								buffer->Instancing->setHardwareMappingHint(EHM_STREAM);

								buffer->Transform = IShaderInstancing::createTransformVertexBuffer();
								buffer->Transform->setHardwareMappingHint(EHM_STREAM);

								buffer->IndirectLighting = IShaderInstancing::createIndirectLightingVertexBuffer();
								buffer->IndirectLighting->setHardwareMappingHint(EHM_STREAM);

								instancingShader->applyInstancing(
									mesh,
									buffer->Instancing,
									buffer->Transform
								);

								CMesh* indirectLightingMesh = mesh->clone();
								indirectLightingMesh->UseInstancing = true;
								indirectLightingMesh->removeAllMeshBuffer();

								u32 count = mesh->getMeshBufferCount();
								for (u32 i = 0; i < count; i++)
								{
									IMeshBuffer* lightingMeshBuffer = instancingShader->createLinkMeshBuffer(mesh->getMeshBuffer(i));
									indirectLightingMesh->addMeshBuffer(lightingMeshBuffer, "", NULL);
									lightingMeshBuffer->drop();
								}

								mesh->IndirectLightingMesh = indirectLightingMesh;

								instancingShader->applyInstancingForRenderLighting(
									indirectLightingMesh,
									buffer->IndirectLighting,
									buffer->Transform);

								group->Buffer = buffer;
							}
						}

						// assign to group
						p->InstancingGroup = &group->Array;
					}

					if (p->InstancingGroup)
						p->InstancingGroup->push_back(p);
				}
			}
		}
	}

	SPrimitiveGroup* CPrimitiveRendererInstancing::getGroup(CEntity* entity, CShader* shader, CMesh* mesh)
	{
		CIndirectLightingData* lighting = GET_ENTITY_DATA(entity, CIndirectLightingData);
		CPrimiviteData* p = GET_ENTITY_DATA(entity, CPrimiviteData);

		SShaderMesh shaderMesh;
		shaderMesh.Shader = shader;
		shaderMesh.Mesh = mesh;
		shaderMesh.IndirectLM = lighting->Type == CIndirectLightingData::LightmapArray ? NULL : lighting->IndirectTexture;
		shaderMesh.DirectLM = lighting->Type == CIndirectLightingData::LightmapArray ? NULL : lighting->LightTexture;

		for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
			shaderMesh.Textures[i] = p->Material->getTexture(i);

		SPrimitiveGroup* g = m_groups[shaderMesh];
		if (g == NULL)
		{
			g = new SPrimitiveGroup();
			m_groups[shaderMesh] = g;
		}

		return g;
	}

	void CPrimitiveRendererInstancing::init(CEntityManager* entityManager)
	{

	}

	void CPrimitiveRendererInstancing::update(CEntityManager* entityManager)
	{
		for (auto& it : m_groups)
		{
			SPrimitiveGroup* group = it.second;

			ArrayPrimitives& list = group->Array;
			if (list.size() == 0)
				continue;

			IShaderInstancing* instancing = it.first.Shader->getInstancing(group->Buffer->BaseVertexType);

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

			SInstancingVertexBuffer* buffer = group->Buffer;
			if (buffer)
			{
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
	}

	void CPrimitiveRendererInstancing::render(CEntityManager* entityManager)
	{
		renderPrimitive(entityManager, false);
	}

	void CPrimitiveRendererInstancing::renderTransparent(CEntityManager* entityManager)
	{
		renderPrimitive(entityManager, true);
	}

	void CPrimitiveRendererInstancing::renderPrimitive(CEntityManager* entityManager, bool isTransparent)
	{
		IVideoDriver* driver = getVideoDriver();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		for (auto& it : m_groups)
		{
			SPrimitiveGroup* group = it.second;
			const SShaderMesh& shaderMesh = it.first;

			ArrayPrimitives& list = group->Array;
			if (list.size() == 0 || group->Buffer == NULL)
				continue;

			CMesh* mesh = shaderMesh.Mesh;
			CShader* shader = shaderMesh.Shader;
			ITexture* const* textures = shaderMesh.Textures;
			video::E_VERTEX_TYPE vertexType = group->Buffer->BaseVertexType;

			if (!rp->canRenderShader(shader))
				continue;

			if (!shader->isOpaque() != isTransparent)
				continue;

			CShaderMaterial::setMaterial(NULL);

			for (u32 i = 0, n = mesh->MeshBuffers.size(); i < n; i++)
			{
				video::SMaterial& irrMat = mesh->getMeshBuffer(i)->getMaterial();

				for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
					irrMat.setTexture(i, textures[i]);

				rp->drawInstancingMeshBuffer(mesh, i, shader->getInstancingShader(vertexType), entityManager, -1, false);
			}
		}
	}
}