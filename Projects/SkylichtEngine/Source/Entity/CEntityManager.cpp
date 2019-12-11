/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "CEntityManager.h"
#include "CWorldTransform.h"

namespace Skylicht
{
	CEntityManager::CEntityManager()
	{

	}

	CEntityManager::~CEntityManager()
	{
		releaseAllEntities();
	}

	void CEntityManager::releaseAllEntities()
	{
		CRenderEntity** entities = m_entities.pointer();
		for (int i = 0, n = (int)m_entities.size(); i < n; i++)
		{
			delete entities[i];
			entities[i] = NULL;
		}

		m_entities.set_used(0);
		m_unused.set_used(0);
	}

	CRenderEntity* CEntityManager::createEntity(CTransform *transform)
	{
		if (m_unused.size() > 0)
		{
			int last = (int)m_unused.size() - 1;

			CRenderEntity *entity = m_unused[last];
			entity->setAlive(true);
			
			if (transform != NULL)
				addEntityTransformData(entity, transform);			

			m_unused.erase(last);
			return entity;
		}

		CRenderEntity *entity = new CRenderEntity(this);
		
		if (transform != NULL)
			addEntityTransformData(entity, transform);

		m_entities.push_back(entity);
		return entity;
	}

	void CEntityManager::addEntityTransformData(CRenderEntity *entity, CTransform *transform)
	{
		CWorldTransform *transformData = entity->addData<CWorldTransform>();
		transformData->Transform = transform;
	}

	void CEntityManager::removeEntity(int index)
	{
		CRenderEntity* entity = m_entities[index];
		entity->setAlive(false);
		entity->removeAllData();
		m_unused.push_back(entity);
	}

	void CEntityManager::update()
	{
		for (IEntitySystem* &s : m_systems)
		{
			s->update();
		}
	}	

	bool CEntityManager::removeSystem(IEntitySystem *system)
	{
		std::vector<IEntitySystem*>::iterator i = m_systems.begin(), end = m_systems.end();
		while (i != end)
		{
			if ((*i) == system)
			{
				delete system;
				m_systems.erase(i);
				return true;
			}
			++i;
		}
		return false;
	}

	void CEntityManager::removeAllSystem()
	{
		for (IEntitySystem* &s : m_systems)
		{
			delete s;
		}
		m_systems.clear();
	}
}