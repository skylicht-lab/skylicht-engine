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
#include "CEntityManager.h"
#include "CRenderEntity.h"

namespace Skylicht
{
	CRenderEntity::CRenderEntity(CEntityManager *mgr) :
		m_numData(0),
		m_alive(true)
	{
		m_index = mgr->getNumEntities();
	}

	CRenderEntity::~CRenderEntity()
	{
		removeAllData();
	}	

	IEntityData* CRenderEntity::getData(int dataIndex)
	{
		if (dataIndex < 0 || dataIndex >= m_numData)
			return NULL;

		return m_data[dataIndex];
	}

	void CRenderEntity::removeData(int dataIndex)
	{
		if (dataIndex >= 0 && dataIndex < m_numData)
		{
			m_numData--;
			delete m_data[dataIndex];
			m_data.erase(dataIndex);
		}
	}

	void CRenderEntity::removeAllData()
	{
		IEntityData **data = m_data.pointer();
		for (int i = 0; i < m_numData; i++)
			delete data[i];
		m_data.clear();
	}
}