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

#pragma once

#include "Components/CComponentSystem.h"
#include "CEntity.h"

namespace Skylicht
{
	/// @brief In the Skylicht-Engine, any components with the functionality to manage multiple entities like creating or deleting entities will inherit from the CEntityHandler class.
	/// 
	/// And, classes that inherit from CEntityHandler will override the spawn() method to add the necessary data to it.
	/// 
	/// @ingroup ECS
	class SKYLICHT_API CEntityHandler : public CComponentSystem
	{
	protected:
		core::array<CEntity*> m_entities;

		bool m_shadowCasting;

	public:
		CEntityHandler();

		virtual ~CEntityHandler();

		virtual void initComponent();

		virtual void updateComponent();

		virtual void onUpdateCullingLayer(u32 mask);

		CEntity* searchEntityByID(const char* id);

		CEntity* createEntity();

		CEntity* createEntity(CEntity* parent);

		virtual CEntity* spawn()
		{
			return NULL;
		}
		
		void regenerateEntityId();
		
		void removeEntity(CEntity* entity);

		void removeAllEntities();

		core::array<CEntity*>& getEntities()
		{
			return m_entities;
		}

		inline int getEntityCount()
		{
			return (int)m_entities.size();
		}

		void getEntitiesTransforms(core::array<core::matrix4>& result);

		void setShadowCasting(bool b);

		inline bool isShadowCasting()
		{
			return m_shadowCasting;
		}

	protected:

		void setEntities(CEntity** entities, u32 count);

		void removeChilds(CEntity* entity);
	};
}
