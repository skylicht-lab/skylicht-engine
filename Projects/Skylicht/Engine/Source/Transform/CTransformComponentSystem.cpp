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
#include "CTransform.h"
#include "CWorldTransformData.h"
#include "CTransformComponentData.h"
#include "CTransformComponentSystem.h"

namespace Skylicht
{
	CComponentTransformSystem::CComponentTransformSystem()
	{

	}

	CComponentTransformSystem::~CComponentTransformSystem()
	{

	}

	void CComponentTransformSystem::beginQuery(CEntityManager* entityManager)
	{
		m_transforms.set_used(0);
		m_components.set_used(0);
	}

	void CComponentTransformSystem::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CTransformComponentData* component = (CTransformComponentData*)entity->getDataByIndex(CTransformComponentData::DataTypeIndex);
		if (component != NULL
			&& component->TransformComponent != NULL
			&& component->TransformComponent->hasChanged())
		{
			CWorldTransformData* transform = (CWorldTransformData*)entity->getDataByIndex(CWorldTransformData::DataTypeIndex);

			// notify changed for CWorldTransformSystem, sync in TransformComponent->hasChanged()
			transform->HasChanged = true;

			m_transforms.push_back(transform);
			m_components.push_back(component);
		}
	}

	void CComponentTransformSystem::init(CEntityManager* entityManager)
	{

	}

	void CComponentTransformSystem::update(CEntityManager* entityManager)
	{
		CWorldTransformData** transforms = m_transforms.pointer();
		CTransformComponentData** components = m_components.pointer();
		u32 numEntity = m_components.size();

		for (u32 i = 0; i < numEntity; i++)
		{
			// copy transform to relative matrix
			components[i]->TransformComponent->getRelativeTransform(transforms[i]->Relative);
			components[i]->TransformComponent->setChanged(false);
		}
	}
}