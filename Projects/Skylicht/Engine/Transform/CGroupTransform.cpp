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
#include "CGroupTransform.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	CGroupTransform::CGroupTransform(CEntityGroup* parent) :
		CEntityGroup(NULL, 0)
	{
		m_parentGroup = parent;
		m_dataTypes.push_back(DATA_TYPE_INDEX(CWorldTransformData));
	}

	CGroupTransform::~CGroupTransform()
	{

	}

	void CGroupTransform::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		CEntity** allEntities = entityManager->getEntities();

		if (m_parentGroup)
		{
			numEntity = m_parentGroup->getEntityCount();
			entities = m_parentGroup->getEntities();
		}

		m_entities.reset();
		m_roots.reset();
		m_childs.reset();
		m_lateUpdate.reset();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];
			CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);

			// set disable flag
			transform->NeedValidate = false;
			transform->NeedValidateForLate = false;

			int parentID = transform->AttachParentIndex >= 0 ?
				transform->AttachParentIndex :
				transform->ParentIndex;

			if (parentID != -1)
			{
				transform->Parent = GET_ENTITY_DATA(allEntities[parentID], CWorldTransformData);
				transform->Depth = transform->Parent->Depth + 1;

				// this transform changed because parent is changed
				if (transform->Parent->NeedValidate)
					transform->HasChanged = true;

				if (transform->Parent->NeedValidateForLate)
					transform->HasLateChanged = true;
			}
			else
			{
				transform->Parent = NULL;
				transform->Depth = 0;
			}

			// tag to update list
			if (transform->HasChanged)
			{
				// set enable flag for another system
				transform->NeedValidate = true;

				m_entities.push(entity);

				if (transform->Depth == 0 || transform->IsWorldTransform)
					m_roots.push(transform);
				else
					m_childs.push(transform);

				transform->HasChanged = false;
			}

			if (transform->HasLateChanged)
			{
				transform->NeedValidateForLate = true;

				// see CWorldTransformSystem for late update
				m_lateUpdate.push(transform);

				transform->HasLateChanged = false;
			}
		}

		// notify alway update this group
		m_needQuery = true;
		m_needValidate = true;
	}
}