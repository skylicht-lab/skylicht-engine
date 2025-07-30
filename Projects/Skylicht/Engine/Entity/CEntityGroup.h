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

#include "CEntity.h"
#include "CArrayUtils.h"

namespace Skylicht
{
	class CEntityManager;

	/// @brief The object class is designed to optimize entity queries.
	/// @ingroup ECS
	/// 
	/// CGroupEntity will store query results; however, in some cases, it will re-query, such as when an entity is created or destroyed by CEntityManager, or when the notifyNeedQuery method is called.
	/// 
	/// Example of creating a group, that only queries entities containing CPrimiviteData.
	/// @code
	/// const u32 primitive[] = GET_LIST_ENTITY_DATA(CPrimiviteData);
	/// CEntityGroup* group = entityManager->createGroup(primitive, 1);
	/// @endcode
	/// 
	/// 
	/// This method also creates a group that queries entities with CPrimiviteData, but only if those entities are visible (visible = true)
	/// @code
	/// const u32 primitive[] = GET_LIST_ENTITY_DATA(CPrimiviteData);
	/// CEntityGroup* group = entityManager->createGroupFromVisible(primitive, 1);
	/// @endcode
	class SKYLICHT_API CEntityGroup
	{
	protected:
		core::array<u32> m_dataTypes;

		CEntityGroup* m_parentGroup;

		// needQuery tell this group will query again at next frame
		bool m_needQuery;

		// needValidate tell children system update the query
		bool m_needValidate;

		CFastArray<CEntity*> m_entities;

	public:
		CEntityGroup(const u32* dataTypes, int count);

		CEntityGroup(const u32* dataTypes, int count, CEntityGroup* parentGroup);

		virtual ~CEntityGroup();

		virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity);

		inline CEntity** getEntities()
		{
			return m_entities.pointer();
		}

		inline int getEntityCount()
		{
			return m_entities.count();
		}

		inline void notifyNeedQuery()
		{
			m_needQuery = true;
		}

		inline bool needQuery()
		{
			return m_needQuery;
		}

		inline bool needValidate()
		{
			return m_needValidate;
		}

		inline void finishValidate()
		{
			m_needValidate = false;
		}

		bool haveDataType(u32 type);

		inline CEntityGroup* getParent()
		{
			return m_parentGroup;
		}
	};
}