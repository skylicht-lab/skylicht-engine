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

#include "Transform/CTransformComponentSystem.h"
#include "Transform/CWorldTransformSystem.h"
#include "Transform/CWorldInvTransformSystem.h"
#include "RenderMesh/CJointTransformSystem.h"
#include "RenderMesh/CMeshRenderer.h"
#include "RenderMesh/CSkinMeshRenderer.h"
#include "RenderMesh/CJointAnimationSystem.h"
#include "RenderMesh/CSkinnedMeshSystem.h"
#include "RenderMesh/CSoftwareSkinningSystem.h"
#include "Culling/CCullingSystem.h"

namespace Skylicht
{
	CEntityManager::CEntityManager() :
		m_camera(NULL)
	{
		addSystem<CComponentTransformSystem>();
		addSystem<CJointTransformSystem>();
		addSystem<CWorldTransformSystem>();
		addSystem<CWorldInvTransformSystem>();
		addSystem<CJointAnimationSystem>();
		addSystem<CSkinnedMeshSystem>();
		addRenderSystem<CCullingSystem>();
		addSystem<CSoftwareSkinningSystem>();
		addRenderSystem<CMeshRenderer>();
		addRenderSystem<CSkinMeshRenderer>();
	}

	CEntityManager::~CEntityManager()
	{
		releaseAllEntities();
		releaseAllSystems();
	}

	void CEntityManager::releaseAllEntities()
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

	void CEntityManager::releaseAllSystems()
	{
		for (IEntitySystem* &s : m_systems)
		{
			delete s;
		}

		m_systems.clear();
		m_renders.clear();
	}

	CEntity* CEntityManager::createEntity()
	{
		if (m_unused.size() > 0)
		{
			int last = (int)m_unused.size() - 1;

			CEntity *entity = m_unused[last];
			entity->setAlive(true);

			m_unused.erase(last);
			return entity;
		}

		CEntity *entity = new CEntity(this);
		m_entities.push_back(entity);
		return entity;
	}

	CEntity** CEntityManager::createEntity(int num, core::array<CEntity*>& entities)
	{
		entities.reallocate(num);
		entities.set_used(0);

		for (int i = 0; i < num; i++)
		{
			CEntity *entity = new CEntity(this);
			m_entities.push_back(entity);

			entities.push_back(entity);
		}

		return entities.pointer();
	}

	void CEntityManager::addTransformDataToEntity(CEntity *entity, CTransform *transform)
	{
		CWorldTransformData *transformData = entity->addData<CWorldTransformData>();
		CTransformComponentData *componentData = entity->addData<CTransformComponentData>();

		// add component to transform
		componentData->TransformComponent = transform;
		componentData->TransformComponent->setChanged(true);

		// add parent relative
		CEntity *parent = componentData->TransformComponent->getParentEntity();
		if (parent != NULL)
		{
			transformData->Name = transform->getName();
			transformData->ParentIndex = parent->getIndex();
			transformData->Depth = parent->getData<CWorldTransformData>()->Depth + 1;
		}
	}

	void CEntityManager::removeEntity(int index)
	{
		CEntity* entity = m_entities[index];
		entity->setAlive(false);
		entity->removeAllData();
		m_unused.push_back(entity);
	}

	void CEntityManager::removeEntity(CEntity *entity)
	{
		entity->setAlive(false);
		entity->removeAllData();
		m_unused.push_back(entity);
	}

	void CEntityManager::update()
	{
		for (IEntitySystem* &s : m_systems)
		{
			s->beginQuery();
		}

		CEntity** entity = m_entities.pointer();
		int numEntity = m_entities.size();

		for (IEntitySystem* &s : m_systems)
		{
			for (int i = 0; i < numEntity; i++)
			{
				if (entity[i]->isAlive())
				{
					s->onQuery(this, entity[i]);
				}
			}

			s->update(this);
		}
	}

	void CEntityManager::render()
	{
		for (IRenderSystem* &s : m_renders)
		{
			s->render(this);
		}

		for (IRenderSystem* &s : m_renders)
		{
			s->postRender(this);
		}
	}

	bool CEntityManager::removeSystem(IEntitySystem *system)
	{
		bool release = false;

		// remove on list system
		{
			std::vector<IEntitySystem*>::iterator i = m_systems.begin(), end = m_systems.end();
			while (i != end)
			{
				if ((*i) == system)
				{
					release = true;
					m_systems.erase(i);
					break;
				}
				++i;
			}
		}

		// remove on list render
		{
			std::vector<IRenderSystem*>::iterator i = m_renders.begin(), end = m_renders.end();
			while (i != end)
			{
				if ((*i) == system)
				{
					release = true;
					m_renders.erase(i);
					break;
				}
				++i;
			}
		}

		if (release == true)
		{
			delete system;
			return true;
		}

		return false;
	}
}