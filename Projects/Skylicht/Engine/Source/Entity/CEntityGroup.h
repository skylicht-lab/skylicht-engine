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

	class CEntityGroup
	{
	protected:
		core::array<u32> m_dataTypes;

		CEntityGroup* m_parentGroup;

		bool m_needQuery;

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

		bool haveDataType(u32 type);

		inline CEntityGroup* getParent()
		{
			return m_parentGroup;
		}
	};
}