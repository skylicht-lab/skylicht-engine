#include "pch.h"
#include "CEntityPrefab.h"
#include "Transform/CWorldTransformData.h"

namespace Skylicht
{
	CEntityPrefab::CEntityPrefab()
	{

	}

	CEntityPrefab::~CEntityPrefab()
	{
		releaseAllEntities();
	}

	CEntity* CEntityPrefab::createEntity()
	{
		if (m_unused.size() > 0)
		{
			int last = (int)m_unused.size() - 1;

			CEntity* entity = m_unused[last];
			entity->setAlive(true);
			m_unused.erase(last);

			return entity;
		}

		CEntity* entity = new CEntity(this);
		m_entities.push_back(entity);
		return entity;
	}

	CEntity** CEntityPrefab::createEntity(int num, core::array<CEntity*>& entities)
	{
		entities.reallocate(num);
		entities.set_used(0);

		for (int i = 0; i < num; i++)
		{
			CEntity* entity = new CEntity(this);

			m_entities.push_back(entity);
			entities.push_back(entity);
		}

		return entities.pointer();
	}

	void CEntityPrefab::releaseAllEntities()
	{
		CEntity** entities = m_entities.pointer();
		for (u32 i = 0, n = m_entities.size(); i < n; i++)
		{
			delete entities[i];
			entities[i] = NULL;
		}

		m_entities.set_used(0);
		m_unused.set_used(0);
	}

	void CEntityPrefab::removeEntity(u32 index)
	{
		CEntity* entity = m_entities[index];
		entity->setAlive(false);
		entity->removeAllData();
		m_unused.push_back(entity);
	}

	void CEntityPrefab::removeEntity(CEntity* entity)
	{
		entity->setAlive(false);
		entity->removeAllData();
		m_unused.push_back(entity);
	}

	void CEntityPrefab::addTransformData(CEntity* entity, CEntity* parent, const core::matrix4& transform, const char* name)
	{
		CWorldTransformData* transformData = entity->addData<CWorldTransformData>();
		transformData->Relative = transform;
		transformData->Name = name;

		// add parent relative
		if (parent != NULL)
		{
			transformData->ParentIndex = parent->getIndex();
			transformData->Depth = GET_ENTITY_DATA(parent, CWorldTransformData)->Depth + 1;
		}
	}

	void CEntityPrefab::changeParent(CEntity* entity, CEntity* parent)
	{
		CWorldTransformData* transformData = GET_ENTITY_DATA(entity, CWorldTransformData);
		if (!transformData)
			return;

		if (parent != NULL)
		{
			transformData->ParentIndex = parent->getIndex();
			transformData->Depth = GET_ENTITY_DATA(parent, CWorldTransformData)->Depth + 1;
		}
		else
		{
			transformData->ParentIndex = -1;
			transformData->Depth = 0;
		}
	}
}