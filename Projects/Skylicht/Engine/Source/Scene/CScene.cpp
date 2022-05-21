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
#include "EventManager/CEventManager.h"

namespace Skylicht
{
	CScene::CScene()
	{
		m_entityManager = new CEntityManager();
		CEventManager::getInstance()->registerEvent("Scene", this);

		setName(L"Scene");
	}

	CScene::~CScene()
	{
		releaseScene();
		CEventManager::getInstance()->unRegisterEvent(this);
	}

	void CScene::setName(const char* lpName)
	{
		wchar_t name[1024];
		CStringImp::convertUTF8ToUnicode(lpName, name);
		m_name = name;
	}

	const char* CScene::getNameA()
	{
		m_namec = CStringImp::convertUnicodeToUTF8(m_name.c_str());
		return m_namec.c_str();
	}

	CGameObject* CScene::searchObject(const wchar_t* name)
	{
		for (CZone*& zone : m_zones)
		{
			if (CStringImp::comp<const wchar_t>(zone->getName(), name) == 0)
				return zone;
		}
		return NULL;
	}

	CGameObject* CScene::searchObjectInChild(const wchar_t* name)
	{
		CGameObject* obj = searchObject(name);
		if (obj == NULL)
		{
			for (CZone*& zone : m_zones)
			{
				obj = zone->searchObjectInChild(name);
				if (obj != NULL)
					return obj;
			}
		}
		return obj;
	}

	CGameObject* CScene::searchObjectByID(const char* id)
	{
		for (CZone*& zone : m_zones)
		{
			if (zone->getID() == id)
				return zone;
		}
		return NULL;
	}

	CGameObject* CScene::searchObjectInChildByID(const char* id)
	{
		CGameObject* obj = searchObjectByID(id);
		if (obj == NULL)
		{
			for (CZone*& zone : m_zones)
			{
				obj = zone->searchObjectInChildByID(id);
				if (obj != NULL)
					return obj;
			}
		}
		return obj;
	}

	void CScene::releaseScene()
	{
		for (CZone*& zone : m_zones)
		{
			zone->updateAddRemoveObject();
			delete zone;
		}
		m_zones.clear();

		delete m_entityManager;
		m_entityManager = NULL;
	}

	void CScene::setVisibleAllZone(bool b)
	{
		for (CZone*& zone : m_zones)
		{
			zone->setVisible(b);
		}
	}

	/**
	* It will force add/remove object to scene tree
	* Note: This function is called on CScene::update beforce update all CGameObject
	*       You don't call this function inside CGameObject::update or Component::update
	*       You can call this function when construct scene
	*/
	void CScene::updateAddRemoveObject()
	{
		for (CZone*& zone : m_zones)
			zone->updateAddRemoveObject(true);
	}

	void CScene::updateIndexSearchObject()
	{
		for (CZone*& zone : m_zones)
			zone->updateIndexSearchObject();
	}

	CZone* CScene::createZone()
	{
		CZone* zone = new CZone(this);

		std::string name = zone->generateObjectName("Zone");

		zone->setName(name.c_str());
		zone->setID(zone->generateRandomID().c_str());
		zone->createEntity();
		zone->addComponent<CTransformEuler>();

		m_zones.push_back(zone);
		return zone;
	}

	void CScene::removeZone(CGameObject* zone)
	{
		ArrayZoneIter iZone = m_zones.begin(), iEnd = m_zones.end();
		while (iZone != iEnd)
		{
			CZone* p = (CZone*)(*iZone);
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

	void CScene::bringToNext(CZone* object, CZone* target, bool behind)
	{
		ArrayZone::iterator i = std::find(m_zones.begin(), m_zones.end(), object);
		if (i != m_zones.end())
			m_zones.erase(i);

		// insert new position
		ArrayZone::iterator pos = std::find(m_zones.begin(), m_zones.end(), target);
		if (behind)
			++pos;

		m_zones.insert(pos, object);
	}

	void CScene::registerEvent(std::string name, IEventReceiver* pEvent)
	{
		std::vector<eventType>::iterator i = m_eventReceivers.begin(), end = m_eventReceivers.end();
		while (i != end)
		{
			if ((*i).second == pEvent)
				return;
			++i;
		}

		m_eventReceivers.push_back(eventType(name, pEvent));
	}

	void CScene::unRegisterEvent(IEventReceiver* pEvent)
	{
		std::vector<eventType>::iterator i = m_eventReceivers.begin(), end = m_eventReceivers.end();
		while (i != end)
		{
			if ((*i).second == pEvent)
			{
				m_eventReceivers.erase(i);
				return;
			}
			++i;
		}
	}

	bool CScene::OnEvent(const SEvent& event)
	{
		// need copy to another array.
		// because the application will crash if registerEvent or unregister on process list Event
		std::vector<eventType> eventWillProcess = m_eventReceivers;

		std::vector<eventType>::iterator i = eventWillProcess.begin(), end = eventWillProcess.end();
		while (i != end)
		{
			bool sendEvent = false;

			// we need check
			std::vector<eventType>::iterator j = m_eventReceivers.begin(), jend = m_eventReceivers.end();
			while (j != jend)
			{
				if ((*i).second == (*j).second)
				{
					sendEvent = true;
					break;
				}
				++j;
			}

			// ok send event
			if (sendEvent)
			{
				(*i).second->OnEvent(event);
			}

			++i;
		}

		return false;
	}

	void CScene::update()
	{
		for (CZone*& zone : m_zones)
		{
			// Update add/remove childs object
			zone->updateAddRemoveObject();

			if (!zone->isEnable())
				continue;

			// we add all child to array to optimize performance
			core::array<CGameObject*>& listChilds = zone->getArrayChilds(false);
			CGameObject** objs = listChilds.pointer();

			for (u32 i = 0, n = listChilds.size(); i < n; i++)
			{
				CGameObject* obj = objs[i];
				if (obj->isEnable() == true)
					obj->updateObject();
			}

			for (u32 i = 0, n = listChilds.size(); i < n; i++)
			{
				CGameObject* obj = objs[i];
				if (obj->isEnable() == true)
					obj->postUpdateObject();
			}

			for (u32 i = 0, n = listChilds.size(); i < n; i++)
			{
				CGameObject* obj = objs[i];
				if (obj->isEnable() == true)
					obj->endUpdate();
			}
		}
	}

	CObjectSerializable* CScene::createSerializable()
	{
		CObjectSerializable* object = new CObjectSerializable(getTypeName().c_str());
		object->autoRelease(new CStringProperty(object, "name", getNameA()));
		return object;
	}

	void CScene::loadSerializable(CObjectSerializable* object)
	{
		setName(object->get<std::wstring>("name", std::wstring(L"Scene")).c_str());
	}
}