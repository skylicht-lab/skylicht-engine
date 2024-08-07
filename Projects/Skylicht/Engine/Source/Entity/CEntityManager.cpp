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
#include "RenderMesh/CMeshRendererInstancing.h"
#include "RenderMesh/CSkinnedMeshRenderer.h"
#include "RenderMesh/CJointAnimationSystem.h"
#include "RenderMesh/CSkinnedMeshSystem.h"
#include "RenderMesh/CSoftwareBlendShapeSystem.h"
#include "RenderMesh/CSoftwareSkinningSystem.h"
#include "OcclusionQuery/COcclusionQueryRenderer.h"
#include "Culling/CVisibleSystem.h"
#include "Culling/CCullingSystem.h"
#include "LOD/CLODSystem.h"
#include "Lighting/CLightCullingSystem.h"
#include "ReflectionProbe/CReflectionProbeSystem.h"
#include "IndirectLighting/CIndirectLightingSystem.h"
#include "Debug/CDebugRenderer.h"

namespace Skylicht
{
	CEntityManager::CEntityManager() :
		m_camera(NULL),
		m_renderPipeline(NULL),
		m_systemChanged(true),
		m_needSortEntities(true)
	{
		// core engine systems
		addSystem<CVisibleSystem>();
		addSystem<CComponentTransformSystem>();
		addSystem<CWorldTransformSystem>();
		addSystem<CWorldInverseTransformSystem>();
		addSystem<CJointAnimationSystem>();
		addSystem<CSkinnedMeshSystem>();
		addSystem<CSoftwareBlendShapeSystem>();
		addSystem<CSoftwareSkinningSystem>();
		addSystem<CReflectionProbeSystem>();
		addSystem<CIndirectLightingSystem>();

		// culling system
		addRenderSystem<CLODSystem>();
		addRenderSystem<CCullingSystem>();
		addRenderSystem<CLightCullingSystem>();

		// systems run after culling
		addRenderSystem<CMeshRenderer>();
		addRenderSystem<CSkinnedMeshRenderer>();
		addRenderSystem<CMeshRendererInstancing>();
		addRenderSystem<COcclusionQueryRenderer>();
		addRenderSystem<CDebugRenderer>();
	}

	CEntityManager::~CEntityManager()
	{
		releaseAllEntities();
		releaseAllSystems();
		releaseAllGroups();
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

		notifyUpdateSortEntities();
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

	void CEntityManager::releaseAllGroups()
	{
		for (u32 i = 0, n = m_groups.size(); i < n; i++)
			delete m_groups[i];
		m_groups.clear();
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

			notifyUpdateSortEntities();
			return entity;
		}

		CEntity* entity = new CEntity(this);
		initDefaultData(entity);
		m_entities.push_back(entity);

		notifyUpdateSortEntities();
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

		notifyUpdateSortEntities();
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
		}
	}

	void CEntityManager::updateEntityParent(CEntity* entity)
	{
		CWorldTransformData* transformData = GET_ENTITY_DATA(entity, CWorldTransformData);
		CTransformComponentData* componentData = GET_ENTITY_DATA(entity, CTransformComponentData);

		// no component
		if (componentData->TransformComponent == NULL)
			return;

		// notify update
		componentData->TransformComponent->setChanged(true);

		// update parent
		CEntity* parent = componentData->TransformComponent->getParentEntity();
		if (parent != NULL)
			transformData->ParentIndex = parent->getIndex();
		else
			transformData->ParentIndex = -1;

		notifyUpdateSortEntities();
	}

	CEntity* CEntityManager::getEntityByID(const char* id)
	{
		CEntity** e = m_entities.pointer();
		u32 count = m_entities.size();

		for (u32 i = 0; i < count; i++, e++)
		{
			CEntity* entity = (*e);
			if (entity->isAlive() &&
				!entity->getID().empty() &&
				entity->getID() == id)
			{
				return entity;
			}
		}

		return NULL;
	}

	void CEntityManager::removeEntity(int index)
	{
		CEntity* entity = m_entities[index];
		entity->setAlive(false);
		entity->removeAllData();
		m_unused.push_back(entity);

		notifyUpdateSortEntities();
	}

	void CEntityManager::removeEntity(CEntity* entity)
	{
		entity->setAlive(false);
		entity->removeAllData();
		m_unused.push_back(entity);

		notifyUpdateSortEntities();
	}

	void CEntityManager::sortAliveEntities()
	{
		CEntity** entities = m_entities.pointer();
		u32 numEntity = m_entities.size();

		int maxDepth = 0;

		for (u32 i = 0; i < MAX_ENTITY_DEPTH; i++)
		{
			m_sortDepth[i].reset();
		}

		for (u32 i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			if (entity->isAlive())
			{
				CWorldTransformData* world = GET_ENTITY_DATA(entity, CWorldTransformData);

				if (world->AttachParentIndex >= 0)
				{
					CEntity* parent = m_entities[world->AttachParentIndex];
					CWorldTransformData* parentWorld = GET_ENTITY_DATA(parent, CWorldTransformData);
					world->Depth = parentWorld->Depth + 1;
				}
				else
				{
					if (world->Parent)
						world->Depth = world->Parent->Depth + 1;
					else
						world->Depth = 0;
				}

				m_sortDepth[world->Depth].push(entity);

				if (maxDepth < world->Depth)
					maxDepth = world->Depth;
			}
		}

		m_alives.set_used(numEntity);
		CEntity** alives = m_alives.pointer();

		// copy and sort the alives by depth
		int count = 0;
		for (int i = 0; i <= maxDepth; i++)
		{
			CEntity** entitiesPtr = m_sortDepth[i].pointer();

			for (int j = 0, n = m_sortDepth[i].count(); j < n; j++)
				alives[count++] = entitiesPtr[j];
		}
		m_alives.set_used(count);

		m_needSortEntities = false;
	}

	void CEntityManager::sortSystem()
	{
		m_sortUpdate = m_systems;

		struct {
			bool operator()(IEntitySystem* a, IEntitySystem* b) const
			{
				int priorityA = (int)a->getSystemOrder();
				int priorityB = (int)b->getSystemOrder();

				if (priorityA == priorityB)
					return a->getSystemOrder() < b->getSystemOrder();

				return priorityA < priorityB;
			}
		} customLess;

		std::sort(m_sortUpdate.begin(), m_sortUpdate.end(), customLess);
	}

	void CEntityManager::update()
	{
		if (m_systemChanged == true)
		{
			sortSystem();
		}

		for (IEntitySystem*& s : m_sortUpdate)
		{
			s->beginQuery(this);
		}

		if (m_needSortEntities)
			sortAliveEntities();

		CEntity** entities = m_alives.pointer();
		int numEntity = (int)m_alives.size();

		for (u32 i = 0, n = m_groups.size(); i < n; i++)
		{
			CEntityGroup* g = m_groups[i];
			g->finishValidate();

			if (g->needQuery())
				g->onQuery(this, entities, numEntity);
		}

		for (IEntitySystem*& s : m_sortUpdate)
		{
			// note: Render system will be updated in cullingAndRender function
			if (!s->isRenderSystem())
			{
				s->onQuery(this, entities, numEntity);
				s->update(this);
			}
		}
	}

	void CEntityManager::sortRenderer()
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
	}

	void CEntityManager::render()
	{
		if (m_systemChanged == true)
		{
			sortRenderer();
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
			s->beginQuery(this);
		}

		if (m_systemChanged == true)
		{
			sortRenderer();
			m_systemChanged = false;
		}

		CEntity** entities = m_alives.pointer();
		int numEntity = m_alives.size();

		for (IRenderSystem*& s : m_renders)
		{
			s->onQuery(this, entities, numEntity);
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

	CEntityGroup* CEntityManager::addCustomGroup(CEntityGroup* group)
	{
		m_groups.push_back(group);
		return group;
	}

	CEntityGroup* CEntityManager::createGroupFromVisible(const u32* types, int count)
	{
		const u32 visibleGroupType[] = GET_LIST_ENTITY_DATA(CVisibleData);
		CEntityGroup* visibleGroup = findGroup(visibleGroupType, 1);

		return createGroup(types, count, visibleGroup);
	}

	CEntityGroup* CEntityManager::createGroup(const u32* types, int count)
	{
		CEntityGroup* group = new CEntityGroup(types, count);
		m_groups.push_back(group);
		return group;
	}

	CEntityGroup* CEntityManager::createGroup(const u32* types, int count, CEntityGroup* parent)
	{
		CEntityGroup* group = new CEntityGroup(types, count, parent);
		m_groups.push_back(group);
		return group;
	}

	CEntityGroup* CEntityManager::findGroup(const u32* types, int count)
	{
		u32 groupCount = m_groups.size();

		CEntityGroup* result = NULL;

		for (u32 i = 0; i < groupCount; i++)
		{
			result = m_groups[i];

			for (int j = 0; j < count; j++)
			{
				if (!result->haveDataType(types[j]))
				{
					result = NULL;
					break;
				}
			}

			if (result)
				break;
		}

		return result;
	}

	void CEntityManager::removeGroup(CEntityGroup* group)
	{
		u32 count = m_groups.size();
		for (u32 i = 0; i < count; i++)
		{
			if (m_groups[i] == group)
			{
				delete group;
				m_groups.erase(i);
			}
		}
	}

	void CEntityManager::removeAllGroup()
	{
		u32 count = m_groups.size();
		for (u32 i = 0; i < count; i++)
		{
			delete m_groups[i];
		}
		m_groups.clear();
	}

	void CEntityManager::notifyUpdateSortEntities()
	{
		m_needSortEntities = true;

		u32 count = m_groups.size();
		for (u32 i = 0; i < count; i++)
		{
			m_groups[i]->notifyNeedQuery();
		}
	}

	void CEntityManager::notifyUpdateGroup(u32 dataType)
	{
		u32 count = m_groups.size();
		for (u32 i = 0; i < count; i++)
		{
			CEntityGroup* g = m_groups[i];

			if (g->haveDataType(dataType))
				g->notifyNeedQuery();

			if (g->getParent() && g->getParent()->needQuery())
				g->notifyNeedQuery();
		}
	}
}