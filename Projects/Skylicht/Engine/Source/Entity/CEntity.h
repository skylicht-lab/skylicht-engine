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

#include "IEntityData.h"
#include "CEntityDataTypeManager.h"

namespace Skylicht
{
	class CEntityManager;
	class CEntityPrefab;

#define GET_ENTITY_DATA(entity, DataType) ((DataType*)(entity->Data[DataType##_DataTypeIndex]))

#define GET_LIST_ENTITY_DATA(DataType) { DataType##_DataTypeIndex }

#define GET_LIST_ENTITY_DATA2(DataType1, DataType2) { DataType1##_DataTypeIndex, DataType2##_DataTypeIndex }

#define MAX_ENTITY_DATA 64

	class SKYLICHT_API CEntity
	{
		friend class CEntityManager;
		friend class CEntityPrefab;

	protected:
		bool m_visible;
		bool m_alive;
		int m_index;
		std::string m_id;

		CEntityManager* m_mgr;
	public:

		IEntityData* Data[MAX_ENTITY_DATA];

	public:
		CEntity(CEntityManager* mgr);
		CEntity(CEntityPrefab* mgr);

		virtual ~CEntity();

		template<class T>
		T* addData();

		template<class T>
		T* addData(int index);

		IEntityData* addDataByActivator(const char* dataType);

		inline int getDataCount()
		{
			return MAX_ENTITY_DATA;
		}

		inline IEntityData* getDataByIndex(u32 dataIndex)
		{
			return Data[dataIndex];
		}

		inline void setID(const char* id)
		{
			m_id = id;
		}

		inline std::string& getID()
		{
			return m_id;
		}

		template<class T>
		T* getData(); // "Replaced by getDataByIndex, which has an improved performance"

		template<class T>
		bool removeData();

		bool removeData(u32 index);

		void removeAllData();

		inline int getIndex()
		{
			return m_index;
		}

		inline bool isAlive()
		{
			return m_alive;
		}

		inline bool isVisible()
		{
			return m_visible;
		}

		void setVisible(bool b);

		void notifyUpdateGroup(int type);

	protected:

		inline void setAlive(bool b)
		{
			m_alive = b;
		}

	};

	template<class T>
	T* CEntity::addData()
	{
		T* newData = new T();
		IEntityData* data = dynamic_cast<IEntityData*>(newData);
		if (data == NULL)
		{
			char exceptionInfo[512];
			sprintf(exceptionInfo, "CEntity::addData %s must inherit IEntityData", typeid(T).name());
			os::Printer::log(exceptionInfo);

			delete newData;
			return NULL;
		}

		// get index of type
		u32 index = CEntityDataTypeManager::getDataIndex(typeid(T));

		// also save this entity index
		data->EntityIndex = m_index;
		data->Entity = this;

		if (Data[index])
			delete Data[index];

		// save at index
		Data[index] = newData;

		notifyUpdateGroup(index);

		return newData;
	}

	template<class T>
	T* CEntity::addData(int index)
	{
		T* newData = new T();
		IEntityData* data = dynamic_cast<IEntityData*>(newData);
		if (data == NULL)
		{
			char exceptionInfo[512];
			sprintf(exceptionInfo, "CEntity::addData %s must inherit IEntityData", typeid(T).name());
			os::Printer::log(exceptionInfo);

			delete newData;
			return NULL;
		}

		// also save this entity index
		data->EntityIndex = m_index;
		data->Entity = this;

		if (Data[index])
			delete Data[index];

		// save at index
		Data[index] = newData;

		notifyUpdateGroup(index);

		return newData;
	}

	// "Replaced by getDataByIndex, which has an improved performance"
	template<class T>
	T* CEntity::getData()
	{
		for (u32 i = 0; i < MAX_ENTITY_DATA; i++)
		{
			if (Data[i])
			{
				T* t = dynamic_cast<T*>(Data[i]);
				if (t)
				{
					return t;
				}
			}
		}

		return NULL;
	}

	template<class T>
	bool CEntity::removeData()
	{
		u32 index = CEntityDataTypeManager::getDataIndex(typeid(T));

		if (Data[index])
		{
			delete Data[index];
			Data[index] = NULL;

			notifyUpdateGroup(index);

			return true;
		}

		return false;
	}
}