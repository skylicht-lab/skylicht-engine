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

#ifndef _CONTAINEROBJ_
#define _CONTAINEROBJ_

#include "CGameObject.h"

namespace Skylicht
{
	class CContainerObject : public CGameObject
	{
	protected:
		ArrayGameObject						m_childs;
		ArrayGameObject						m_add;
		ArrayGameObject						m_remove;

		core::array<CGameObject*>			m_objectsAffectLight;
		core::array<CContainerObject*>		m_childContainer;

		core::map<std::wstring, CGameObject*>	m_objectByName;

		bool								m_updateRemoveAdd;
	public:
		CContainerObject();
		CContainerObject(CGameObject *parent);

		virtual ~CContainerObject();

		virtual void updateObject();

		virtual void postUpdateObject();

		virtual void endUpdate();

		CGameObject* createEmptyObject();

		CContainerObject* createContainerObject();
		
		/**
	    * It will force add/remove object to scene tree
		* Note: This function is called on CScene::update beforce update all CGameObject
		*       You don't call this function inside CGameObject::update or Component::update
		*       You can call this function when construct scene
	    */
		void updateAddRemoveObject(bool force = false);

		int getNumberObjects();

		void updateIndexSearchObject();

		CGameObject* searchObject(const wchar_t *objectName);

		CGameObject* searchObjectInChild(const wchar_t *objectName);

		std::string generateObjectName(const char* objTemplate);

		void registerObjectName(CGameObject* obj);

		void removeObject(CGameObject *pObj);

		void addChild(CGameObject *p);

		inline ArrayGameObject* getChilds()
		{
			return &m_childs;
		}

		void removeAllObject(bool force = false);

		template<typename T>
		void getListObjectType(ArrayGameObject& listObjs, T type);
	};
}

#endif