/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CEntityHandler.h"
#include "CEntityHandleData.h"
#include "CEntityManager.h"
#include "GameObject/CGameObject.h"
#include "GameObject/CZone.h"
#include "Transform/CWorldTransformData.h"
#include "Culling/CVisibleData.h"
#include "Utils/CRandomID.h"

namespace Skylicht
{
	CEntityHandler::CEntityHandler()
	{

	}

	CEntityHandler::~CEntityHandler()
	{
		removeAllEntities();
	}

	void CEntityHandler::initComponent()
	{

	}

	void CEntityHandler::updateComponent()
	{

	}

	CEntity* CEntityHandler::createEntity()
	{
		CEntityManager* entityManager = m_gameObject->getEntityManager();

		CEntity* entity = entityManager->createEntity();
		CWorldTransformData* transformData = entity->addData<CWorldTransformData>();

		// name
		char name[512];
		sprintf(name, "#%d", entity->getIndex());

		// id
		std::string id = CRandomID::generate();
		entity->setID(id.c_str());

		// add parent relative
		CEntity* parent = m_gameObject->getEntity();
		if (parent != NULL)
		{
			transformData->Name = name;
			transformData->ParentIndex = parent->getIndex();
		}

		// add handle data
		CEntityHandleData* handleData = entity->addData<CEntityHandleData>();
		handleData->Handler = this;

		m_entities.push_back(entity);
		return entity;
	}

	CEntity* CEntityHandler::createEntity(CEntity* parent)
	{
		CEntityManager* entityManager = m_gameObject->getEntityManager();

		CEntity* entity = entityManager->createEntity();
		CWorldTransformData* transformData = entity->addData<CWorldTransformData>();

		char name[512];
		sprintf(name, "#%d", entity->getIndex());

		// id
		std::string id = CRandomID::generate();
		entity->setID(id.c_str());

		// add parent relative
		if (parent != NULL)
		{
			transformData->Name = name;
			transformData->ParentIndex = parent->getIndex();
		}

		m_entities.push_back(entity);
		return entity;
	}

	void CEntityHandler::removeEntity(CEntity* entity)
	{
		CEntityManager* entityManager = m_gameObject->getEntityManager();

		for (int i = (int)m_entities.size() - 1; i >= 0; i--)
		{
			if (m_entities[i] == entity)
			{
				m_entities.erase(i);
				entityManager->removeEntity(entity);
			}
		}
	}

	void CEntityHandler::removeAllEntities()
	{
		if (m_gameObject == NULL)
			return;

		CEntityManager* entityManager = m_gameObject->getEntityManager();
		if (entityManager == NULL)
			return;

		for (int i = (int)m_entities.size() - 1; i >= 0; i--)
			entityManager->removeEntity(m_entities[i]);
		m_entities.clear();
	}

	void CEntityHandler::setEntities(CEntity** entities, u32 count)
	{
		m_entities.clear();
		for (u32 i = 0; i < count; i++)
		{
			CEntity* entity = entities[i];
			CWorldTransformData* transformData = GET_ENTITY_DATA(entity, CWorldTransformData);

			// assign name
			if (transformData->Name.empty())
			{
				char name[512];
				sprintf(name, "#%d", entity->getIndex());
				transformData->Name = name;
			}

			// generate id for entity
			std::string id = CRandomID::generate();
			entity->setID(id.c_str());

			// add to handler
			m_entities.push_back(entity);
		}
	}
}