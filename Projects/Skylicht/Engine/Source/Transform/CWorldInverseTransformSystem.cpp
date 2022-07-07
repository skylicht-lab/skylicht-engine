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
#include "CWorldInverseTransformSystem.h"
#include "Entity/CEntityManager.h"
#include "Transform/CTransform.h"

namespace Skylicht
{
	CWorldInverseTransformSystem::CWorldInverseTransformSystem()
	{
	}

	CWorldInverseTransformSystem::~CWorldInverseTransformSystem()
	{
	}

	void CWorldInverseTransformSystem::beginQuery(CEntityManager* entityManager)
	{
		m_world.set_used(0);
		m_worldInv.set_used(0);
	}

	void CWorldInverseTransformSystem::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CWorldInverseTransformData* worldInv = GET_ENTITY_DATA(entity, CWorldInverseTransformData);

		// worldInv->HasChanged is trigged at CWorldTransformSystem
		if (worldInv != NULL && worldInv->HasChanged == true)
		{
			CWorldTransformData* world = GET_ENTITY_DATA(entity, CWorldTransformData);

			m_world.push_back(world);
			m_worldInv.push_back(worldInv);
		}
	}

	void CWorldInverseTransformSystem::init(CEntityManager* entityManager)
	{

	}

	void CWorldInverseTransformSystem::update(CEntityManager* entityManager)
	{
		CWorldTransformData** worlds = m_world.pointer();
		CWorldInverseTransformData** worldInvs = m_worldInv.pointer();

		for (u32 i = 0, n = m_world.size(); i < n; i++)
		{
			// Get inverse matrix of world
			worlds[i]->World.getInverse(worldInvs[i]->WorldInverse);
			worldInvs[i]->HasChanged = false;
		}
	}
}