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

#include "CGameObject.h"

namespace Skylicht
{
	class CContainerObject : public CGameObject
	{
	protected:
		ArrayGameObject m_childs;
		ArrayGameObject m_add;
		ArrayGameObject m_remove;

		core::array<CGameObject*> m_arrayChildObjects;

		core::array<CGameObject*> m_objectsAffectLight;
		core::array<CContainerObject*> m_childContainer;

		core::map<std::wstring, CGameObject*> m_objectByName;
		core::map<std::string, CGameObject*> m_objectByID;

		bool m_updateRemoveAdd;
	public:
		CContainerObject(CGameObject* parent, CZone* zone);

		virtual ~CContainerObject();

		CGameObject* createEmptyObject();

		CContainerObject* createContainerObject();

		void updateAddRemoveObject(bool force = false);

		int getNumberObjects();

		void updateIndexSearchObject();

		virtual CTransform* getTransform();

		virtual CTransformEuler* getTransformEuler();

		virtual CGameObject* searchObject(const wchar_t* objectName);

		virtual CGameObject* searchObjectInChild(const wchar_t* objectName);

		virtual CGameObject* searchObjectByID(const char* id);

		virtual CGameObject* searchObjectInChildByID(const char* id);

		virtual CGameObject* searchObjectInScene(const wchar_t* objectName);

		virtual bool testConflictName(const wchar_t* objectName);

		std::string generateObjectName(const char* objTemplate);

		std::string generateRandomID();

		void registerObjectInSearchList(CGameObject* obj);

		void removeObject(CGameObject* pObj);

		void addChild(CGameObject* p);

		inline ArrayGameObject* getChilds()
		{
			return &m_childs;
		}

		void removeAllObject(bool force = false);

		template<typename T>
		void getListObjectType(ArrayGameObject& listObjs, T type);

		template<typename T>
		std::vector<T*> getComponentsInChild(bool addThis);

		core::array<CGameObject*>& getArrayChilds(bool addThis);
	};

	template<typename T>
	std::vector<T*> CContainerObject::getComponentsInChild(bool addThis)
	{
		std::vector<T*> result;
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

			T* comp = obj->getComponent<T>();
			if (comp != NULL)
				result.push_back(comp);

			CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
			if (container != NULL)
			{
				for (CGameObject*& child : container->m_childs)
					queueObjs.push(child);
			}
		}

		return result;
	}
}