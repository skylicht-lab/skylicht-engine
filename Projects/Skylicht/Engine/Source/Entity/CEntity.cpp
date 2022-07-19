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
#include "CEntity.h"
#include "CEntityManager.h"
#include "CEntityPrefab.h"

#include "Utils/CActivator.h"

namespace Skylicht
{
	CEntity::CEntity(CEntityManager* mgr) :
		m_alive(true),
		m_visible(true),
		Depth(0)
	{
		m_index = mgr->getNumEntities();

		for (int i = 0; i < MAX_ENTITY_DATA; i++)
			Data[i] = NULL;
	}

	CEntity::CEntity(CEntityPrefab* mgr) :
		m_alive(true),
		m_visible(true),
		Depth(0)
	{
		m_index = mgr->getNumEntities();

		for (int i = 0; i < MAX_ENTITY_DATA; i++)
			Data[i] = NULL;
	}

	CEntity::~CEntity()
	{
		removeAllData();
	}

	bool CEntity::removeData(u32 index)
	{
		if (Data[index])
		{
			delete Data[index];
			Data[index] = NULL;
			return true;
		}

		return false;
	}

	IEntityData* CEntity::addDataByActivator(const char* dataType)
	{
		IActivatorObject* obj = CActivator::getInstance()->createInstance(dataType);

		IEntityData* data = dynamic_cast<IEntityData*>(obj);
		if (data == NULL)
		{
			char exceptionInfo[512];
			sprintf(exceptionInfo, "CEntity::addData %s must inherit IEntityData", dataType);
			os::Printer::log(exceptionInfo);

			if (obj != NULL)
				delete obj;

			return NULL;
		}

		// also save this entity index
		data->EntityIndex = m_index;

		int index = CEntityDataTypeManager::getDataIndex(typeid(*data));

		if (Data[index])
			delete Data[index];

		// save at index
		Data[index] = data;

		return data;
	}

	void CEntity::removeAllData()
	{
		for (u32 i = 0; i < MAX_ENTITY_DATA; i++)
		{
			if (Data[i])
			{
				delete Data[i];
				Data[i] = NULL;
			}
		}
	}
}