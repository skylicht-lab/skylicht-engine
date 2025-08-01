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
#include "Utils/CRandomID.h"

#include "Scene/CScene.h"
#include "Entity/CEntityHandler.h"

namespace Skylicht
{
	CContainerObject::CContainerObject(CGameObject* parent, CZone* zone) :
		CGameObject(parent, zone),
		m_updateRemoveAdd(true),
		m_updateListChild(true),
		m_lastGenerateID(-1)
	{
		m_isContainer = true;
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
		m_objectByID.clear();
	}

	core::array<CGameObject*>& CContainerObject::getArrayChilds(bool addThis)
	{
		if (m_updateListChild)
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

			m_updateListChild = false;
		}
		return m_arrayChildObjects;
	}

	bool CContainerObject::haveChild(CGameObject* gameObject)
	{
		std::queue<CGameObject*> queueObjs;

		for (CGameObject*& obj : m_childs)
			queueObjs.push(obj);

		while (queueObjs.size() != 0)
		{
			CGameObject* obj = queueObjs.front();
			queueObjs.pop();

			if (gameObject == obj)
				return true;

			CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
			if (container != NULL)
			{
				for (CGameObject*& child : container->m_childs)
					queueObjs.push(child);
			}
		}

		return false;
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

	CGameObject* CContainerObject::createObject(CObjectSerializable* data, bool generateNewID)
	{
		CGameObject* ret = NULL;

		std::string saveId;
		CStringProperty* id = (CStringProperty*)data->getProperty("id");

		if (generateNewID)
		{
			saveId = id->get();
			id->set(CRandomID::generate());
		}

		if (data->Name == "CZone" || data->Name == "CContainerObject")
		{
			CContainerObject* container = createContainerObject();
			container->loadSerializable(data);
			container->startComponent();

			CObjectSerializable* childs = data->getProperty<CObjectSerializable>("Childs");
			if (childs != NULL)
			{
				for (int i = 0, n = childs->getNumProperty(); i < n; i++)
				{
					CObjectSerializable* childData = (CObjectSerializable*)childs->getPropertyID(i);
					container->createObject(childData, generateNewID);
				}
				container->updateAddRemoveObject();
			}
			ret = container;
		}
		else if (data->Name == "CGameObject")
		{
			ret = createEmptyObject();
			ret->loadSerializable(data);
			ret->startComponent();
		}

		if (ret != NULL)
		{
			if (generateNewID)
			{
				id->set(saveId);

				std::string tid = ret->getTemplateObjectID();
				if (!m_templateObjectId.empty() && tid.empty())
					ret->setTemplateObjectID(ret->getID().c_str());

				for (CComponentSystem* component : ret->getListComponent())
				{
					CEntityHandler* handler = dynamic_cast<CEntityHandler*>(component);
					if (handler)
						handler->regenerateEntityId();
				}
			}

			updateIndexSearchObject();
		}

		return ret;
	}

	CGameObject* CContainerObject::createEmptyObject()
	{
		CGameObject* p = new CGameObject(this, m_zone);
		if (p == NULL)
			return NULL;

		std::string name = generateObjectName("GameObject");

		p->setID(CRandomID::generate().c_str());
		p->setName(name.c_str());

		if (!m_templateObjectId.empty())
			p->setTemplateObjectID(p->getID().c_str());

		addChild(p);

		p->createEntity();
		p->setupEulerTransform();

		return p;
	}

	CContainerObject* CContainerObject::createContainerObject()
	{
		CContainerObject* container = new CContainerObject(this, m_zone);

		std::string name = generateObjectName("Container");

		container->setID(CRandomID::generate().c_str());
		container->setParent(this);

		if (!m_templateObjectId.empty())
			container->setTemplateObjectID(container->getID().c_str());

		container->setName(name.c_str());
		addChild(container);

		container->createEntity();
		container->setupEulerTransform();

		return container;
	}

	void CContainerObject::updateIndexSearchObject()
	{
		m_objectByName.clear();
		m_objectByID.clear();

		for (CGameObject*& obj : m_childs)
		{
			registerObjectInSearchList(obj);

			CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
			if (container != NULL)
			{
				container->updateIndexSearchObject();
			}
		}

		for (CGameObject*& obj : m_add)
		{
			registerObjectInSearchList(obj);

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

	CGameObject* CContainerObject::getChildObjectBefore(CGameObject* object)
	{
		ArrayGameObject childs;
		for (CGameObject* obj : m_childs)
		{
			if (!obj->isEditorObject())
				childs.push_back(obj);
		}

		ArrayGameObject::iterator i = std::find(childs.begin(), childs.end(), object);
		if (i == childs.begin() || i == childs.end())
			return NULL;
		--i;
		return (*i);
	}

	void CContainerObject::bringToNext(CGameObject* object, CGameObject* target, bool behind)
	{
		if (target->getParent() != this)
			return;

		core::matrix4 world = object->getTransform()->calcWorldTransform();

		if (object->getParent() == this)
		{
			// remove old position
			ArrayGameObject::iterator i = std::find(m_childs.begin(), m_childs.end(), object);
			m_childs.erase(i);
		}
		else
		{
			// remove old parent
			CContainerObject* oldParent = (CContainerObject*)object->getParent();
			ArrayGameObject::iterator i = std::find(oldParent->m_childs.begin(), oldParent->m_childs.end(), object);
			if (i != oldParent->m_childs.end())
			{
				oldParent->m_childs.erase(i);
				oldParent->updateIndexSearchObject();
			}

			// set new parent
			object->setParent(this);
			object->updateEntityParent();
		}

		// insert new position
		ArrayGameObject::iterator pos = std::find(m_childs.begin(), m_childs.end(), target);
		if (behind)
			++pos;

		m_childs.insert(pos, object);
		updateIndexSearchObject();

		object->getTransform()->setWorldMatrix(world);
		m_lastGenerateID = -1;
	}

	void CContainerObject::bringToChild(CGameObject* object)
	{
		core::matrix4 world = object->getTransform()->calcWorldTransform();

		if (object->getParent() == this)
		{
			// remove old position
			ArrayGameObject::iterator i = std::find(m_childs.begin(), m_childs.end(), object);
			m_childs.erase(i);
		}
		else
		{
			// remove old parent
			CContainerObject* oldParent = (CContainerObject*)object->getParent();
			ArrayGameObject::iterator i = std::find(oldParent->m_childs.begin(), oldParent->m_childs.end(), object);
			if (i != oldParent->m_childs.end())
			{
				oldParent->m_childs.erase(i);
				oldParent->updateIndexSearchObject();
			}

			// set new parent
			object->setParent(this);
			object->updateEntityParent();
		}

		// insert new position
		m_childs.push_back(object);
		updateIndexSearchObject();

		object->getTransform()->setWorldMatrix(world);
		m_lastGenerateID = -1;
	}

	void CContainerObject::sortChildsByTemplateOrder(std::vector<std::string>& order)
	{
		auto getOrder = [](const char* id, std::vector<std::string>& order)
			{
				int i = 0;
				for (auto value : order)
				{
					if (value == id)
						return i;
					i++;
				}
				return 0;
			};

		int numChild = (int)m_childs.size();
		for (int i = 0; i < numChild - 1; i++)
		{
			for (int j = i + 1; j < numChild; j++)
			{
				CGameObject* a = m_childs[i];
				CGameObject* b = m_childs[j];

				std::string ta = a->getTemplateObjectID();
				std::string tb = b->getTemplateObjectID();

				if (!ta.empty() && tb.empty())
				{
					if (getOrder(ta.c_str(), order) > getOrder(tb.c_str(), order))
					{
						m_childs[i] = b;
						m_childs[j] = a;
					}
				}
			}
		}
	}

	void CContainerObject::setTemplateChanged(bool b)
	{
		CGameObject::setTemplateChanged(b);

		if (b == false)
		{
			for (u32 i = 0, n = m_arrayChildObjects.size(); i < n; i++)
			{
				if (m_templateId == m_arrayChildObjects[i]->getTemplateID())
					m_arrayChildObjects[i]->setTemplateChanged(b);
			}
		}
	}

	CGameObject* CContainerObject::searchObject(const wchar_t* objectName)
	{
		core::map<std::wstring, CGameObject*>::Node* node = m_objectByName.find(std::wstring(objectName));
		if (node == NULL)
		{
			// try search in list add
			for (CGameObject* addObject : m_add)
			{
				if (CStringImp::comp(addObject->getName(), objectName) == 0)
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

	CGameObject* CContainerObject::searchObjectByID(const char* id)
	{
		core::map<std::string, CGameObject*>::Node* node = m_objectByID.find(std::string(id));
		if (node == NULL)
		{
			// try search in list add
			for (CGameObject* addObject : m_add)
			{
				if (addObject->getID() == id)
				{
					return addObject;
				}
			}

			return NULL;
		}

		return node->getValue();
	}

	CGameObject* CContainerObject::searchObjectInChildByID(const char* id)
	{
		CGameObject* result = searchObjectByID(id);
		if (result == NULL)
		{
			for (CGameObject*& obj : m_childs)
			{
				CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
				if (container != NULL)
				{
					result = container->searchObjectInChildByID(id);
					if (result != NULL)
						return result;
				}
			}
		}
		return result;
	}

	CEntity* CContainerObject::searchEntityByID(const char* id)
	{
		for (CGameObject*& obj : m_childs)
		{
			CEntityHandler* entityHandler = obj->getComponent<CEntityHandler>();
			if (entityHandler != NULL)
			{
				CEntity* result = entityHandler->searchEntityByID(id);
				if (result != NULL)
					return result;
			}
		}
		return NULL;
	}

	CEntity* CContainerObject::searchEntityInChildByID(const char* id)
	{
		for (CGameObject*& obj : m_childs)
		{
			CEntityHandler* entityHandler = obj->getComponent<CEntityHandler>();
			if (entityHandler != NULL)
			{
				CEntity* result = entityHandler->searchEntityByID(id);
				if (result != NULL)
					return result;
			}

			CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
			if (container != NULL)
			{
				CEntity* result = container->searchEntityByID(id);
				if (result != NULL)
					return result;
			}
		}

		return NULL;
	}

	CGameObject* CContainerObject::searchObjectInChildByTemplateObjId(const char* id)
	{
		for (CGameObject*& obj : m_childs)
		{
			if (CStringImp::comp(obj->getTemplateObjectID(), id) == 0)
			{
				return obj;
			}

			CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
			if (container != NULL)
			{
				CGameObject* ret = container->searchObjectInChildByTemplateObjId(id);
				if (ret)
					return ret;
			}
		}

		return NULL;
	}

	u32 CContainerObject::searchObjectByCullingLayer(ArrayGameObject& result, u32 mask)
	{
		for (CGameObject*& obj : m_childs)
		{
			if (obj->getCullingLayer() & mask)
				result.push_back(obj);

			CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
			if (container != NULL)
				container->searchObjectByCullingLayer(result, mask);
		}

		return (u32)result.size();
	}

	void CContainerObject::setCullingLayerForChild(u32 mask)
	{
		for (CGameObject*& obj : m_childs)
		{
			obj->setCullingLayer(mask);
			CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
			if (container != NULL)
				container->setCullingLayerForChild(mask);
		}
	}

	void CContainerObject::setCullingOnOffForChild(u32 value, bool on)
	{
		for (CGameObject*& obj : m_childs)
		{
			obj->setCullingLayerOnOff(value, on);
			CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
			if (container != NULL)
				container->setCullingOnOffForChild(value, on);
		}
	}

	bool CContainerObject::testConflictName(const wchar_t* objectName)
	{
		return searchObject(objectName) != NULL;
	}

	std::string CContainerObject::generateObjectName(const char* objTemplate)
	{
		wchar_t name[1024];
		char lpName[1024];

		int objectID = 0;
		if (m_lastGenerateID >= 0)
			objectID = m_lastGenerateID;

		do
		{
			objectID++;
			sprintf(lpName, "%s_%d", objTemplate, objectID);
			CStringImp::convertUTF8ToUnicode(lpName, name);
		} while (testConflictName(name));

		m_lastGenerateID = objectID;

		return std::string(lpName);
	}

	void CContainerObject::registerObjectInSearchList(CGameObject* obj)
	{
		m_objectByName[obj->getName()] = obj;
		m_objectByID[obj->getID()] = obj;
	}

	void CContainerObject::updateAddRemoveObject(bool force)
	{
		if (m_updateRemoveAdd == true || force == true)
		{
			m_updateRemoveAdd = false;
			m_lastGenerateID = -1;

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
							m_objectByID.remove(obj->getID());

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
			if (obj->isContainer())
			{
				CContainerObject* container = (CContainerObject*)obj;
				if (container != NULL)
				{
					container->updateAddRemoveObject(force);
				}
			}
		}
	}

	void CContainerObject::addChild(CGameObject* p)
	{
		p->setParent(this);
		m_add.push_back(p);
		m_updateRemoveAdd = true;
		m_updateListChild = true;
		getZone()->notifyUpdateListChild();
	}

	void CContainerObject::removeObject(CGameObject* pObj)
	{
		m_remove.push_back(pObj);
		m_updateRemoveAdd = true;
		m_updateListChild = true;
		getZone()->notifyUpdateListChild();
	}
}
