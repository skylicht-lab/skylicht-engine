/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
#include "CLightSystem.h"
#include "Culling/CVisibleData.h"
#include "Entity/CEntityManager.h"
#include "Material/Shader/CShaderManager.h"

#include "Debug/CSceneDebug.h"

namespace Skylicht
{
	CLightSystem::CLightSystem() :
		m_group(NULL),
		m_groupMesh(NULL),
		m_needUpdatePL(true),
		m_needUpdateSL(true)
	{
		m_pipelineType = IRenderPipeline::Forwarder;
	}

	CLightSystem::~CLightSystem()
	{
	}

	void CLightSystem::beginQuery(CEntityManager* entityManager)
	{
		m_pointLights.set_used(0);
		m_spotLights.set_used(0);
		m_dirLights.set_used(0);
		m_renderMeshs.set_used(0);

		if (!m_group)
		{
			const u32 type[] = GET_LIST_ENTITY_DATA(CLightCullingData);
			m_group = entityManager->createGroupFromVisible(type, 1);
		}

		if (!m_groupMesh)
		{
			const u32 type[] = GET_LIST_ENTITY_DATA(CRenderMeshData);
			m_groupMesh = (CGroupMesh*)entityManager->findGroup(type, 1);
		}
	}

	void CLightSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		entities = m_group->getEntities();
		numEntity = m_group->getEntityCount();

		if (!m_groupMesh)
			return;

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CWorldTransformData* transformData = GET_ENTITY_DATA(entity, CWorldTransformData);

			CLightCullingData* lightData = GET_ENTITY_DATA(entity, CLightCullingData);
			switch (lightData->LightType)
			{
			case 0:
				m_dirLights.push_back(lightData);
				break;
			case 1:
				m_pointLights.push_back(lightData);
				m_needUpdatePL = m_needUpdatePL || transformData->NeedValidate;
				break;
			case 2:
				m_spotLights.push_back(lightData);
				m_needUpdateSL = m_needUpdateSL || transformData->NeedValidate;
				break;
			default:
				break;
			};
		}

		entities = m_groupMesh->getStaticMeshes();
		numEntity = m_groupMesh->getNumStaticMesh();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CRenderMeshData* renderMesh = GET_ENTITY_DATA(entity, CRenderMeshData);
			if (renderMesh->isVisible() &&
				renderMesh->isSortingLights())
			{
				m_renderMeshs.push_back(renderMesh);
			}
		}

		entities = m_groupMesh->getSkinnedMeshes();
		numEntity = m_groupMesh->getNumSkinnedMesh();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CRenderMeshData* renderMesh = GET_ENTITY_DATA(entity, CRenderMeshData);
			if (renderMesh->isVisible())
				m_renderMeshs.push_back(renderMesh);
		}
	}

	void CLightSystem::init(CEntityManager* entityManager)
	{

	}

	void CLightSystem::update(CEntityManager* entityManager)
	{

	}

	void CLightSystem::render(CEntityManager* entityManager)
	{

	}

	void CLightSystem::postRender(CEntityManager* entityManager)
	{

	}
}