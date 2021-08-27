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
#include "Transform/CWorldInverseTransformSystem.h"
#include "RenderMesh/CMeshRenderer.h"
#include "RenderMesh/CSkinnedMeshRenderer.h"
#include "RenderMesh/CJointAnimationSystem.h"
#include "RenderMesh/CSkinnedMeshSystem.h"
#include "RenderMesh/CSoftwareSkinningSystem.h"
#include "Culling/CVisibleSystem.h"
#include "Culling/CCullingSystem.h"
#include "Lighting/CLightCullingSystem.h"
#include "ReflectionProbe/CReflectionProbeSystem.h"

namespace Skylicht
{
	CEntityManager::CEntityManager() :
		m_camera(NULL),
		m_renderPipeline(NULL),
		m_systemChanged(true)
	{
		// core engine systems
		addSystem<CComponentTransformSystem>();
		addSystem<CWorldTransformSystem>();
		addSystem<CWorldInverseTransformSystem>();
		addSystem<CJointAnimationSystem>();
		addSystem<CSkinnedMeshSystem>();
		addSystem<CSoftwareSkinningSystem>();
		addSystem<CReflectionProbeSystem>();

		// culling system
		addRenderSystem<CVisibleSystem>();
		addRenderSystem<CCullingSystem>();
		addRenderSystem<CLightCullingSystem>();

		// systems run after culling
		addRenderSystem<CMeshRenderer>();
		addRenderSystem<CSkinnedMeshRenderer>();
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
		for (IEntitySystem*& s : m_systems)
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

			CEntity* entity = m_unused[last];
			initDefaultData(entity);
			entity->setAlive(true);

			m_unused.erase(last);
			return entity;
		}

		CEntity* entity = new CEntity(this);
		initDefaultData(entity);
		m_entities.push_back(entity);
		return entity;
	}

	CEntity** CEntityManager::createEntity(int num, core::array<CEntity*>& entities)
	{
		entities.reallocate(num);
		entities.set_used(0);

		for (int i = 0; i < num; i++)
		{
			CEntity* entity = new CEntity(this);
			initDefaultData(entity);

			m_entities.push_back(entity);
			entities.push_back(entity);
		}

		return entities.pointer();
	}

	void CEntityManager::initDefaultData(CEntity* entity)
	{
		entity->addData<CVisibleData>();
	}

	void CEntityManager::addTransformDataToEntity(CEntity* entity, CTransform* transform)
	{
		CWorldTransformData* transformData = entity->addData<CWorldTransformData>();
		CTransformComponentData* componentData = entity->addData<CTransformComponentData>();

		// add component to transform
		componentData->TransformComponent = transform;
		componentData->TransformComponent->setChanged(true);

		// add parent relative
		CEntity* parent = componentData->TransformComponent->getParentEntity();
		if (parent != NULL)
		{
			transformData->Name = transform->getName();
			transformData->ParentIndex = parent->getIndex();
			transformData->Depth = parent->getData<CWorldTransformData>()->Depth + 1;
		}
	}

	void CEntityManager::updateEntityParent(CEntity* entity)
	{
		CWorldTransformData* transformData = entity->getData<CWorldTransformData>();
		CTransformComponentData* componentData = entity->getData<CTransformComponentData>();

		// no component
		if (componentData->TransformComponent == NULL)
			return;

		// notify update
		componentData->TransformComponent->setChanged(true);

		// update parent
		CEntity* parent = componentData->TransformComponent->getParentEntity();
		if (parent != NULL)
		{
			transformData->ParentIndex = parent->getIndex();
			transformData->Depth = parent->getData<CWorldTransformData>()->Depth + 1;
		}
		else
		{
			transformData->ParentIndex = -1;
			transformData->Depth = 0;
		}
	}

	void CEntityManager::removeEntity(int index)
	{
		CEntity* entity = m_entities[index];
		entity->setAlive(false);
		entity->removeAllData();
		m_unused.push_back(entity);
	}

	void CEntityManager::removeEntity(CEntity* entity)
	{
		entity->setAlive(false);
		entity->removeAllData();
		m_unused.push_back(entity);
	}

	void CEntityManager::update()
	{
		for (IEntitySystem*& s : m_systems)
		{
			s->beginQuery();
		}

		CEntity** entity = m_entities.pointer();
		int numEntity = m_entities.size();

		for (IEntitySystem*& s : m_systems)
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
		if (m_systemChanged == true)
		{
			m_sortRender = m_renders;

			struct {
				bool operator()(IRenderSystem* a, IRenderSystem* b) const
				{
					int priorityA = (int)a->getRenderPass();
					int priorityB = (int)b->getRenderPass();

					if (priorityA == priorityB)
						return a->getSortingPriority() < b->getSortingPriority();

					return priorityA < priorityB;
				}
			} customLess;

			std::sort(m_sortRender.begin(), m_sortRender.end(), customLess);

			m_systemChanged = false;
		}

		for (IRenderSystem*& s : m_sortRender)
		{
			IRenderPipeline::ERenderPipelineType t = s->getPipelineType();
			if (t == IRenderPipeline::Mix || t == m_renderPipeline->getType())
			{
				s->render(this);
			}
		}

		// transparent pass
		for (IRenderSystem*& s : m_sortRender)
		{
			IRenderPipeline::ERenderPipelineType t = s->getPipelineType();
			if (t == IRenderPipeline::Mix || t == m_renderPipeline->getType())
			{
				s->renderTransparent(this);
			}
		}

		// post render
		for (IRenderSystem*& s : m_sortRender)
		{
			IRenderPipeline::ERenderPipelineType t = s->getPipelineType();
			if (t == IRenderPipeline::Mix || t == m_renderPipeline->getType())
			{
				s->postRender(this);
			}
		}
	}

	void CEntityManager::cullingAndRender()
	{
		for (IRenderSystem*& s : m_renders)
		{
			s->beginQuery();
		}

		CEntity** entity = m_entities.pointer();
		int numEntity = m_entities.size();

		for (IRenderSystem*& s : m_renders)
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

		for (IRenderSystem*& s : m_sortRender)
		{
			IRenderPipeline::ERenderPipelineType t = s->getPipelineType();
			if (t == IRenderPipeline::Mix || t == m_renderPipeline->getType())
			{
				s->render(this);
			}
		}

		for (IRenderSystem*& s : m_sortRender)
		{
			IRenderPipeline::ERenderPipelineType t = s->getPipelineType();
			if (t == IRenderPipeline::Mix || t == m_renderPipeline->getType())
			{
				s->renderTransparent(this);
			}
		}

		for (IRenderSystem*& s : m_sortRender)
		{
			IRenderPipeline::ERenderPipelineType t = s->getPipelineType();
			if (t == IRenderPipeline::Mix || t == m_renderPipeline->getType())
			{
				s->postRender(this);
			}
		}
	}

	void CEntityManager::renderEmission()
	{
		for (IRenderSystem*& s : m_sortRender)
		{
			IRenderPipeline::ERenderPipelineType t = s->getPipelineType();
			if (t == IRenderPipeline::Mix || t == m_renderPipeline->getType())
			{
				s->renderEmission(this);
			}
		}
	}

	bool CEntityManager::removeSystem(IEntitySystem* system)
	{
		bool release = false;

		// remove on list system
		{
			std::vector<IEntitySystem*>::iterator i = std::find(m_systems.begin(), m_systems.end(), system);
			if (i != m_systems.end())
			{
				release = true;
				m_systems.erase(i);
			}
		}

		// remove on list render
		{
			std::vector<IRenderSystem*>::iterator i = std::find(m_renders.begin(), m_renders.end(), system);
			if (i != m_renders.end())
			{
				release = true;
				m_renders.erase(i);
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