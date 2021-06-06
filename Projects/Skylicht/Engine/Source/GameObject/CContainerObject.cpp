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

#include "CContainerObject.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{

	CContainerObject::CContainerObject(CGameObject* parent, CZone* zone) :
		CGameObject(parent, zone),
		m_updateRemoveAdd(true)
	{
	}

	CContainerObject::~CContainerObject()
	{
		removeAllObject(true);

		char logText[512];
		sprintf(logText, "Release obj %s", getNameA());
		os::Printer::log(logText);
	}

	void CContainerObject::removeAllObject(bool force)
	{
		updateAddRemoveObject(force);

		ArrayGameObjectIter it = m_childs.begin(), end = m_childs.end();

		int i = 0;

		while (it != end)
		{
			CGameObject* pObj = (*it);

			++i;

			char logText[512];
			sprintf(logText, "Release obj pos: %d - %s", i, pObj->getNameA());
			os::Printer::log(logText);

			delete pObj;
			++it;
		}

		m_childs.clear();
		m_objectByName.clear();
	}

	core::array<CGameObject*>& CContainerObject::getArrayChilds(bool addThis)
	{
		m_arrayChildObjects.set_used(0);
		std::queue<CGameObject*> queueObjs;

		if (addThis == true)
			queueObjs.push(this);
		else
		{
			for (CGameObject*& obj : m_childs)
				queueObjs.push(obj);
		}

		while (queueObjs.size() != 0)
		{
			CGameObject* obj = queueObjs.front();
			queueObjs.pop();

			m_arrayChildObjects.push_back(obj);

			CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
			if (container != NULL)
			{
				for (CGameObject*& child : container->m_childs)
					queueObjs.push(child);
			}
		}

		return m_arrayChildObjects;
	}

	template<typename T>
	void CContainerObject::getListObjectType(ArrayGameObject& listObjs, T type)
	{
		listObjs.clear();

		for (CGameObject*& obj : m_childs)
		{
			if (typeid(*obj) == typeid(type))
			{
				listObjs.push_back(obj);
			}
		}
	}

	CGameObject* CContainerObject::createEmptyObject()
	{
		CGameObject* p = new CGameObject(this, m_zone);
		if (p == NULL)
			return NULL;

		std::string name = generateObjectName("GameObject");

		p->setID(generateRandomID().c_str());
		p->setName(name.c_str());

		addChild(p);

		p->createEntity();
		p->addComponent<CTransformEuler>();
		return p;
	}

	CContainerObject* CContainerObject::createContainerObject()
	{
		CContainerObject* container = new CContainerObject(this, m_zone);

		std::string name = generateObjectName("Container");

		container->setID(generateRandomID().c_str());
		container->setParent(this);

		container->setName(name.c_str());
		addChild(container);

		container->createEntity();
		container->addComponent<CTransformEuler>();
		return container;
	}

	void CContainerObject::updateIndexSearchObject()
	{
		m_objectByName.clear();

		for (CGameObject*& obj : m_childs)
		{
			registerObjectName(obj);

			CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
			if (container != NULL)
			{
				container->updateIndexSearchObject();
			}
		}
	}

	int CContainerObject::getNumberObjects()
	{
		return (int)m_childs.size();
	}

	CGameObject* CContainerObject::searchObject(const wchar_t* objectName)
	{
		core::map<std::wstring, CGameObject*>::Node* node = m_objectByName.find(std::wstring(objectName));
		if (node == NULL)
		{
			// try search in list add
			for (CGameObject* addObject : m_add)
			{
				if (std::wstring(addObject->getName()) == objectName)
				{
					return addObject;
				}
			}

			return NULL;
		}

		return node->getValue();
	}

	CGameObject* CContainerObject::searchObjectInChild(const wchar_t* objectName)
	{
		CGameObject* result = searchObject(objectName);
		if (result == NULL)
		{
			for (CGameObject*& obj : m_childs)
			{
				CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
				if (container != NULL)
				{
					result = container->searchObjectInChild(objectName);
					if (result != NULL)
						return result;
				}
			}
		}
		return result;
	}

	std::string CContainerObject::generateObjectName(const char* objTemplate)
	{
		wchar_t name[1024];
		char lpName[1024];

		int objectID = 0;
		do
		{
			objectID++;
			sprintf(lpName, "%s_%d", objTemplate, objectID);
			CStringImp::convertUTF8ToUnicode(lpName, name);
		} while (searchObject(name) != NULL);

		return std::string(lpName);
	}

	std::string CContainerObject::generateRandomID()
	{
		std::string randomString;
		static const char alphanum[] = "0123456789";

		srand((unsigned int)os::Timer::getRealTime());

		int len = 16;
		randomString.reserve(len);

		for (int i = 0; i < len; ++i)
			randomString += alphanum[rand() % (sizeof(alphanum) - 1)];

		return randomString;
	}

	void CContainerObject::registerObjectName(CGameObject* obj)
	{
		m_objectByName[obj->getName()] = obj;
	}

	void CContainerObject::updateAddRemoveObject(bool force)
	{
		if (m_updateRemoveAdd == true || force == true)
		{
			m_updateRemoveAdd = false;

			if (m_add.size() > 0)
			{
				for (CGameObject*& obj : m_add)
					m_childs.push_back(obj);

				m_add.clear();
			}

			if (m_remove.size() > 0)
			{
				ArrayGameObject arrayRemove = m_remove;
				m_remove.clear();

				for (CGameObject*& obj : arrayRemove)
				{
					ArrayGameObjectIter iObj = m_childs.begin(), iObjEnd = m_childs.end();
					while (iObj != iObjEnd)
					{
						if (obj == (*iObj))
						{
							m_objectByName.remove(obj->getDefaultName());
							m_childs.erase(iObj);

							delete obj;
							break;
						}
						++iObj;
					}
				}
			}
		}

		// update in children
		for (CGameObject*& obj : m_childs)
		{
			CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
			if (container != NULL)
			{
				container->updateAddRemoveObject(force);
			}
		}
	}

	void CContainerObject::addChild(CGameObject* p)
	{
		p->setParent(this);
		m_add.push_back(p);
		m_updateRemoveAdd = true;
	}

	void CContainerObject::removeObject(CGameObject* pObj)
	{
		m_remove.push_back(pObj);
		m_updateRemoveAdd = true;
	}
}