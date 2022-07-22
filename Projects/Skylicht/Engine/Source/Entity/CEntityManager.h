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

#pragma once

#include "IEntitySystem.h"
#include "IRenderSystem.h"
#include "CEntity.h"

#include "GameObject/CGameObject.h"
#include "Camera/CCamera.h"

#define MAX_ENTITY_DEPTH 256

namespace Skylicht
{
	struct SEntityDepth
	{
		core::array<CEntity*> Entities;
		int Count;
		int Alloc;

		CEntity** EntitiesPtr;

		SEntityDepth()
		{
			Count = 0;
			Alloc = 0;
			EntitiesPtr = NULL;
		}
	};

	class CEntityManager
	{
	protected:
		core::array<CEntity*> m_alives;
		core::array<CEntity*> m_entities;
		core::array<CEntity*> m_unused;

		SEntityDepth m_sortDepth[MAX_ENTITY_DEPTH];

		std::vector<IEntitySystem*> m_systems;
		std::vector<IRenderSystem*> m_renders;

		std::vector<IRenderSystem*> m_sortRender;
		bool m_systemChanged;
		bool m_needSortEntities;

		CCamera* m_camera;

		IRenderPipeline* m_renderPipeline;

	public:
		CEntityManager();

		virtual ~CEntityManager();

		void update();

		void render();

		void renderEmission();

		void cullingAndRender();

	protected:

		void sortAliveEntities();

	public:

		inline void setCamera(CCamera* camera)
		{
			m_camera = camera;
		}

		inline CCamera* getCamera()
		{
			return m_camera;
		}

		inline void setRenderPipeline(IRenderPipeline* p)
		{
			m_renderPipeline = p;
		}

		inline IRenderPipeline* getRenderPipeline()
		{
			return m_renderPipeline;
		}

		CEntity* createEntity();

		CEntity** createEntity(int num, core::array<CEntity*>& entities);

		void releaseAllEntities();

		void releaseAllSystems();

		inline int getNumEntities()
		{
			return (int)m_entities.size();
		}

		inline CEntity* getEntity(int index)
		{
			return m_entities[index];
		}

		inline CEntity** getEntities()
		{
			return m_entities.pointer();
		}

		CEntity* getEntityByID(const char* id);

		void removeEntity(int index);

		void removeEntity(CEntity* entity);

		template<class T>
		T* addSystem();

		template<class T>
		T* addRenderSystem();

		template<class T>
		T* getSystem();

		bool removeSystem(IEntitySystem* system);

		void addTransformDataToEntity(CEntity* entity, CTransform* transform);

		void updateEntityParent(CEntity* entity);

		inline void notifySortEntitiesUpdate()
		{
			m_needSortEntities = true;
		}
	protected:

		void initDefaultData(CEntity* entity);

		void updateSortRenderer();

	};

	template<class T>
	T* CEntityManager::addSystem()
	{
		T* newSystem = new T();

		IEntitySystem* system = dynamic_cast<IEntitySystem*>(newSystem);
		if (system == NULL)
		{
			char exceptionInfo[512];
			sprintf(exceptionInfo, "CEntityManager::addSystem %s must inherit IEntitySystem", typeid(T).name());
			os::Printer::log(exceptionInfo);
			delete newSystem;
			return NULL;
		}

		system->init(this);

		m_systemChanged = true;

		m_systems.push_back(system);
		return newSystem;
	}

	template<class T>
	T* CEntityManager::addRenderSystem()
	{
		T* system = getSystem<T>();
		if (system != NULL)
		{
			return system;
		}

		T* newSystem = new T();

		IRenderSystem* render = dynamic_cast<IRenderSystem*>(newSystem);
		if (render == NULL)
		{
			char exceptionInfo[512];
			sprintf(exceptionInfo, "CEntityManager::addRenderSystem %s must inherit IRenderSystem", typeid(T).name());
			os::Printer::log(exceptionInfo);
			delete newSystem;
			return NULL;
		}

		render->init(this);

		m_systems.push_back(render);
		m_renders.push_back(render);

		m_systemChanged = true;

		return newSystem;
	}

	template<class T>
	T* CEntityManager::getSystem()
	{
		for (IEntitySystem*& s : m_systems)
		{
			T* system = dynamic_cast<T*>(s);
			if (system != NULL)
			{
				return system;
			}
		}
		return NULL;
	}
}