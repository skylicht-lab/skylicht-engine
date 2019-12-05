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
	CContainerObject::CContainerObject()
	{
		m_updateRemoveAdd = true;
	}

	CContainerObject::CContainerObject(CGameObject *parent)
		:CGameObject(parent)
	{
		m_updateRemoveAdd = true;
	}

	CContainerObject::~CContainerObject()
	{
		removeAllObject(true);
	}

	void CContainerObject::removeAllObject(bool force)
	{
		updateAddRemoveObject(force);

		ArrayGameObjectIter it = m_childs.begin(), end = m_childs.end();

		int i = 0;

		while (it != end)
		{
			CGameObject *pObj = (*it);

			++i;

			char logText[512];
			sprintf(logText, "Release obj %d - %s", i, pObj->getNameA());
			os::Printer::log(logText);

			delete pObj;
			++it;
		}

		m_childs.clear();
		m_objectByName.clear();
	}

	void CContainerObject::updateObject()
	{
		CGameObject::updateObject();

		updateAddRemoveObject();

		int numChild = (int)m_childs.size();
		CGameObject **childs = m_childs.data();

		for (int i = 0; i < numChild; i++)
		{
			CGameObject *pObject = childs[i];
			if (pObject->isEnable())
				pObject->updateObject();
		}
	}

	void CContainerObject::postUpdateObject()
	{
		CGameObject::postUpdateObject();

		ArrayGameObjectIter it = m_childs.begin(), end = m_childs.end();
		while (it != end)
		{
			CGameObject *pObject = (CGameObject*)(*it);
			if (pObject->isEnable())
			{
				pObject->postUpdateObject();
			}
			++it;
		}
	}

	void CContainerObject::endUpdate()
	{
		CGameObject::endUpdate();

		ArrayGameObjectIter it = m_childs.begin(), end = m_childs.end();
		while (it != end)
		{
			CGameObject *pObject = (CGameObject*)(*it);
			if (pObject->isEnable())
			{
				pObject->endUpdate();
			}
			++it;
		}
	}

	template<typename T>
	void CContainerObject::getListObjectType(ArrayGameObject& listObjs, T type)
	{
		listObjs.clear();

		ArrayGameObjectIter it = m_childs.begin(), end = m_childs.end();
		while (it != end)
		{
			CGameObject *pObject = (CGameObject*)(*it);

			if (typeid(*pObject) == typeid(type))
			{
				listObjs.push_back(pObject);
			}
			++it;
		}
	}

	CGameObject* CContainerObject::createEmptyObject()
	{
		CGameObject *p = new CGameObject(this);
		if (p == NULL)
			return NULL;

		char lpName[1024];
		sprintf(lpName, "RunTimeEmptyObject_%d", (int)CGameObject::s_objectID);

		p->setID(CGameObject::s_objectID++);
		p->setName(lpName);

		addChild(p);

		return p;
	}

	CContainerObject* CContainerObject::createContainerObject()
	{
		CContainerObject *container = new CContainerObject(this);

		std::string name = generateObjectName("Container");

		container->setID(CGameObject::s_objectID++);
		container->setParent(this);

		container->setName(name.c_str());
		addChild(container);

		return container;
	}

	void CContainerObject::updateIndexSearchObject()
	{
		m_objectByName.clear();

		for (int i = 0, n = (int)m_childs.size(); i < n; i++)
		{
			registerObjectName(m_childs[i]);

			CContainerObject *child = dynamic_cast<CContainerObject*>(m_childs[i]);
			if (child != NULL)
			{
				child->updateIndexSearchObject();
			}
		}
	}

	int CContainerObject::getNumberObjects()
	{
		int numObj = 0;
		ArrayGameObject::iterator i = m_childs.begin(), end = m_childs.end();
		while (i != end)
		{
			++numObj;
			++i;
		}
		return numObj;
	}

	CGameObject* CContainerObject::searchObject(const wchar_t *objectName)
	{
		core::map<std::wstring, CGameObject*>::Node *node = m_objectByName.find(std::wstring(objectName));
		if (node == NULL)
		{
			for (int i = 0, n = (int)m_childs.size(); i < n; i++)
			{
				CContainerObject *child = dynamic_cast<CContainerObject*>(m_childs[i]);
				if (child != NULL)
				{
					CGameObject* obj = child->searchObject(objectName);
					if (obj != NULL)
						return obj;
				}
			}
			return NULL;
		}

		return node->getValue();
	}

	CGameObject* CContainerObject::searchObjectInChild(const wchar_t *objectName)
	{
		CGameObject *obj = searchObject(objectName);
		if (obj == NULL)
		{
			ArrayGameObject::iterator i = m_childs.begin(), end = m_childs.end();
			while (i != end)
			{
				CContainerObject *container = dynamic_cast<CContainerObject*>(*i);
				if (container != NULL)
				{
					obj = container->searchObjectInChild(objectName);
					if (obj != NULL)
						return obj;
				}
				++i;
			}
		}
		return obj;
	}

	std::string CContainerObject::generateObjectName(const char* objTemplate)
	{
		wchar_t name[1024];
		char	lpName[1024];

		CGameObject::s_objectID--;

		do
		{
			CGameObject::s_objectID++;
			sprintf(lpName, "%s_%d", objTemplate, (int)CGameObject::s_objectID);
			CStringImp::convertUTF8ToUnicode(lpName, name);
		} while (searchObject(name) != NULL);

		return std::string(lpName);
	}

	void CContainerObject::registerObjectName(CGameObject* obj)
	{
		m_objectByName[obj->getName()] = obj;
	}

	void CContainerObject::updateAddRemoveObject(bool force)
	{
		if (m_updateRemoveAdd == true || force == true)
		{
			ArrayGameObjectIter it, end;

			if (m_add.size() > 0)
			{
				it = m_add.begin(), end = m_add.end();
				while (it != end)
				{
					m_childs.push_back((*it));
					++it;
				}

				m_add.clear();
			}

			if (m_remove.size() > 0)
			{
				m_updateRemoveAdd = false;

				ArrayGameObject arrayRemove = m_remove;
				m_remove.clear();

				it = arrayRemove.begin(), end = arrayRemove.end();
				while (it != end)
				{
					CGameObject *pObj = (*it);

					ArrayGameObjectIter iObj = m_childs.begin(), iObjEnd = m_childs.end();
					while (iObj != iObjEnd)
					{
						if (pObj == (*iObj))
						{
							m_objectByName.remove(pObj->getDefaultName());

							m_childs.erase(iObj);

							delete pObj;
							break;
						}

						++iObj;
					}

					++it;
				}
			}
		}

		m_add.clear();

		// update in children
		ArrayGameObjectIter it, end;
		it = m_childs.begin(), end = m_childs.end();
		while (it != end)
		{
			CContainerObject *container = dynamic_cast<CContainerObject*>(*it);
			if (container != NULL)
				container->updateAddRemoveObject(force);
			++it;
		}
	}

	void CContainerObject::addChild(CGameObject *p)
	{
		p->setParent(this);
		m_add.push_back(p);
		m_updateRemoveAdd = true;
	}

	void CContainerObject::removeObject(CGameObject *pObj)
	{
		m_remove.push_back(pObj);
		m_updateRemoveAdd = true;
	}
}