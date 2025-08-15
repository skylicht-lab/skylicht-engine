#include "pch.h"
#include "CLineRenderer.h"
#include "Entity/CEntityManager.h"
#include "Culling/CCullingData.h"

namespace Skylicht
{
	CLineRenderer::CLineRenderer() :
		m_group(NULL)
	{

	}

	CLineRenderer::~CLineRenderer()
	{

	}

	void CLineRenderer::beginQuery(CEntityManager* entityManager)
	{
		m_transforms.reset();
		m_lines.reset();

		if (m_group == NULL)
		{
			const u32 line[] = GET_LIST_ENTITY_DATA(CLineRenderData);
			m_group = entityManager->createGroupFromVisible(line, 1);
		}
	}

	void CLineRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		entities = m_group->getEntities();
		numEntity = m_group->getEntityCount();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CCullingData* cullingData = GET_ENTITY_DATA(entity, CCullingData);
			if (cullingData->Visible)
			{
				CLineRenderData* l = GET_ENTITY_DATA(entity, CLineRenderData);
				if (l->Points.size() >= 2)
				{
					m_lines.push(l);

					CWorldTransformData* t = GET_ENTITY_DATA(entity, CWorldTransformData);
					m_transforms.push(t);
				}
			}
		}
	}

	void CLineRenderer::init(CEntityManager* entityManager)
	{

	}

	void CLineRenderer::update(CEntityManager* entityManager)
	{

	}

	void CLineRenderer::render(CEntityManager* entityManager)
	{

	}
}