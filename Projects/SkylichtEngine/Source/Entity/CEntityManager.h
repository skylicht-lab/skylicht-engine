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

#ifndef _ENTITY_MANAGER_H_
#define _ENTITY_MANAGER_H_

#include "IEntitySystem.h"
#include "CRenderEntity.h"

#include "GameObject/CGameObject.h"

namespace Skylicht
{
	class CEntityManager
	{
	protected:
		core::array<CRenderEntity*> m_entities;
		core::array<CRenderEntity*> m_unused;
		
		std::vector<IEntitySystem*> m_systems;
	public:
		CEntityManager();

		virtual ~CEntityManager();

		void update();

	public:
		CRenderEntity* createEntity(CTransform *transform = NULL);

		void releaseAllEntities();

		inline int getNumEntities()
		{
			return (int)m_entities.size();
		}

		inline CRenderEntity* getEntity(int index)
		{
			return m_entities[index];
		}

		void removeEntity(int index);

		template<class T>
		T* addSystem();		

		template<class T>
		T* getSystem();

		bool removeSystem(IEntitySystem *system);

		void removeAllSystem();

	protected:

		void addEntityTransformData(CRenderEntity *entity, CTransform *transform);

	};

	template<class T>
	T* CEntityManager::addSystem()
	{
		T* newSystem = new T();

		IEntitySystem *system = dynamic_cast<IEntitySystem*>(newSystem);
		if (system == NULL)
		{
			char exceptionInfo[512];
			sprintf(exceptionInfo, "CEntityManager::addSystem %s must inherit IEntitySystem", typeid(T).name());
			os::Printer::log(exceptionInfo);
			return NULL;
		}

		system->init();

		m_systems.push_back(system);
		return newSystem;
	}

	template<class T>
	T* CEntityManager::getSystem()
	{
		for (IEntitySystem* &s : m_systems)
		{
			if (dynamic_cast<T*>(s) != NULL)
			{
				return s;
			}
		}
		return NULL;
	}
}

#endif