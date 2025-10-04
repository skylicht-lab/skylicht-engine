#include "pch.h"
#include "CParticleGroupSystem.h"

#include "Entity/CEntityManager.h"

namespace Skylicht
{
	namespace Particle
	{
		CParticleGroupSystem::CParticleGroupSystem() :
			m_group(NULL)
		{

		}

		CParticleGroupSystem::~CParticleGroupSystem()
		{

		}

		void CParticleGroupSystem::beginQuery(CEntityManager* entityManager)
		{
			if (m_group == NULL)
			{
				const u32 particle[] = GET_LIST_ENTITY_DATA(CParticleBufferData);
				m_group = entityManager->findGroup(particle, 1);
				if (!m_group)
					m_group = entityManager->createGroupFromVisible(particle, 1);
			}
		}

		void CParticleGroupSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
		{
			entities = m_group->getEntities();
			numEntity = m_group->getEntityCount();

			for (int i = 0; i < numEntity; i++)
			{
				CEntity* entity = entities[i];

				CParticleBufferData* particleData = GET_ENTITY_DATA(entity, CParticleBufferData);

				// update bbox for culling
				// use last frame data
				CCullingBBoxData* box = GET_ENTITY_DATA(entity, CCullingBBoxData);

				CGroup** groups = particleData->AllGroups.pointer();
				for (u32 i = 0, n = particleData->AllGroups.size(); i < n; i++)
				{
					CGroup* g = groups[i];
					if (i == 0)
						box->BBox = g->getBBox();
					else
						box->BBox.addInternalBox(g->getBBox());
				}

				box->NeedValidate = true;
			}
		}

		void CParticleGroupSystem::init(CEntityManager* entityManager)
		{

		}

		void CParticleGroupSystem::update(CEntityManager* entityManager)
		{
			if (m_group->getEntityCount() == 0)
				return;

			CEntity** entities = m_group->getEntities();
			int numEntity = m_group->getEntityCount();

			for (int i = 0; i < numEntity; i++)
			{
				CEntity* entity = entities[i];

				CParticleBufferData* data = GET_ENTITY_DATA(entity, CParticleBufferData);
				CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);

				for (u32 j = 0, m = data->AllGroups.size(); j < m; j++)
				{
					data->AllGroups[j]->setParentWorldMatrix(transform->World);
					data->AllGroups[j]->update();
				}
			}
		}
	}
}