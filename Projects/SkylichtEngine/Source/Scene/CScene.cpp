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
#include "CScene.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	CScene::CScene()
	{
	}

	CScene::~CScene()
	{
		releaseScene();
	}

	CGameObject* CScene::searchObject(const char* name)
	{
		wchar_t buffer[1024] = { 0 };
		CStringImp::convertUTF8ToUnicode(name, buffer);

		ArrayZoneIter iZone = m_zones.begin(), iEnd = m_zones.end();
		while (iZone != iEnd)
		{
			CZone *pZone = (*iZone);

			if (CStringImp::comp<const wchar_t>(pZone->getName(), buffer) == 0)
				return pZone;

			iZone++;
		}

		return NULL;
	}

	CGameObject* CScene::searchObjectInChild(const char* name)
	{
		CGameObject *obj = searchObject(name);
		if (obj == NULL)
		{
			wchar_t buffer[1024] = { 0 };
			CStringImp::convertUTF8ToUnicode(name, buffer);

			ArrayZoneIter iZone = m_zones.begin(), iEnd = m_zones.end();
			while (iZone != iEnd)
			{
				CZone *pZone = (*iZone);
				obj = pZone->searchObjectInChild(buffer);
				if (obj != NULL)
					return obj;

				++iZone;
			}
		}
		return obj;
	}

	void CScene::releaseScene()
	{
		ArrayZoneIter iZone = m_zones.begin(), iEnd = m_zones.end();
		while (iZone != iEnd)
		{
			CZone *p = (CZone*)(*iZone);
			p->updateAddRemoveObject(true);
			delete p;
			iZone++;
		}
		m_zones.clear();
	}

	/**
	* It will force add/remove object to scene tree
	* Note: This function is called on CScene::update beforce update all CGameObject
	*       You don't call this function inside CGameObject::update or Component::update
	*       You can call this function when construct scene
	*/
	void CScene::updateAddRemoveObject()
	{
		ArrayZoneIter iZone = m_zones.begin(), iEnd = m_zones.end();
		while (iZone != iEnd)
		{
			CZone *p = (CZone*)(*iZone);
			p->updateAddRemoveObject(true);
			++iZone;
		}
	}

	void CScene::updateIndexSearchObject()
	{
		ArrayZoneIter iZone = m_zones.begin(), iEnd = m_zones.end();
		while (iZone != iEnd)
		{
			CZone *p = (CZone*)(*iZone);
			p->updateIndexSearchObject();
			++iZone;
		}
	}

	CZone* CScene::createZone()
	{
		CZone *zone = new CZone();

		char name[512];
		sprintf(name, "Zone_%d", (int)CGameObject::s_objectID);

		zone->setName(name);
		zone->setID(CGameObject::s_objectID++);

		m_zones.push_back(zone);

		return zone;
	}

	void CScene::removeZone(CGameObject* zone)
	{
		ArrayZoneIter iZone = m_zones.begin(), iEnd = m_zones.end();
		while (iZone != iEnd)
		{
			CZone *p = (CZone*)(*iZone);
			if (p == zone)
			{
				p->updateAddRemoveObject();
				delete p;

				m_zones.erase(iZone);
				return;
			}
			++iZone;
		}
	}

	void CScene::update()
	{
		// pass Update
		ArrayZoneIter iZone = m_zones.begin(), iEnd = m_zones.end();
		while (iZone != iEnd)
		{
			(*iZone)->updateObject();
			++iZone;
		}

		// pass PostUpdate
		iZone = m_zones.begin(), iEnd = m_zones.end();
		while (iZone != iEnd)
		{
			(*iZone)->postUpdateObject();
			++iZone;
		}

		// pass EndUpdate
		iZone = m_zones.begin(), iEnd = m_zones.end();
		while (iZone != iEnd)
		{
			(*iZone)->endUpdate();
			++iZone;
		}
	}
}