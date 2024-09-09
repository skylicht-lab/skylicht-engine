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
#include "CEntityGroup.h"

namespace Skylicht
{
	CEntityGroup::CEntityGroup(const u32* dataTypes, int count) :
		m_needQuery(true),
		m_needValidate(true),
		m_parentGroup(NULL)
	{
		for (int i = 0; i < count; i++)
			m_dataTypes.push_back(dataTypes[i]);
	}

	CEntityGroup::CEntityGroup(const u32* dataTypes, int count, CEntityGroup* parentGroup) :
		m_needQuery(true),
		m_parentGroup(parentGroup)
	{
		for (int i = 0; i < count; i++)
			m_dataTypes.push_back(dataTypes[i]);
	}

	CEntityGroup::~CEntityGroup()
	{

	}

	void CEntityGroup::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		u32* types = m_dataTypes.pointer();
		int count = m_dataTypes.size();

		m_entities.reset();

		if (m_parentGroup)
		{
			numEntity = m_parentGroup->getEntityCount();
			entities = m_parentGroup->getEntities();
		}

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];
			bool selectEntity = true;

			for (int j = 0; j < count; j++)
			{
				if (entity->Data[types[j]] == NULL)
				{
					selectEntity = false;
					break;
				}
			}

			if (selectEntity)
			{
				m_entities.push(entity);
			}
		}

		m_needQuery = false;
		m_needValidate = true;
	}

	bool CEntityGroup::haveDataType(u32 type)
	{
		u32* types = m_dataTypes.pointer();
		int count = m_dataTypes.size();

		for (int j = 0; j < count; j++)
		{
			if (types[j] == type)
				return true;
		}

		return false;
	}
}