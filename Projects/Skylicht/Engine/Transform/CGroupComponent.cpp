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
#include "CGroupComponent.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	CGroupComponent::CGroupComponent(CEntityGroup* parent) :
		CEntityGroup(NULL, 0)
	{
		m_parentGroup = parent;
		m_dataTypes.push_back(DATA_TYPE_INDEX(CTransformComponentData));
	}

	CGroupComponent::~CGroupComponent()
	{

	}

	void CGroupComponent::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		CEntity** allEntities = entityManager->getEntities();

		if (m_parentGroup)
		{
			numEntity = m_parentGroup->getEntityCount();
			entities = m_parentGroup->getEntities();
		}

		m_entities.reset();

		CEntity* entity;
		CWorldTransformData* transform;
		CTransformComponentData* component;
		CTransform* transformComponent;

		for (int i = 0; i < numEntity; i++)
		{
			entity = entities[i];

			component = GET_ENTITY_DATA(entity, CTransformComponentData);
			if (!component)
				continue;

			transform = GET_ENTITY_DATA(entity, CWorldTransformData);

			transformComponent = component->TransformComponent;
			if (transformComponent != NULL && transformComponent->hasChanged())
			{
				// copy transform to relative matrix
				transformComponent->getRelativeTransform(transform->Relative);
				transformComponent->setChanged(false);

				// notify changed for CWorldTransformSystem, sync in TransformComponent->hasChanged()
				transform->HasChanged = true;

				// sync is world transform
				transform->IsWorldTransform = transformComponent->isWorldTransform();
			}
		}

		// notify alway update this group
		m_needQuery = true;
		m_needValidate = true;
	}
}