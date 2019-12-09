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
#include "CGameObject.h"
#include "CContainerObject.h"

#include "Utils/CStringImp.h"

namespace Skylicht
{

	long CGameObject::s_objectID = 1;
	std::map<long, long> CGameObject::s_mapObjIDOnFileSaved;
	bool CGameObject::s_repairIDMode = false;

	core::vector3df CGameObject::s_ox = core::vector3df(1.0f, 0.0f, 0.0f);
	core::vector3df CGameObject::s_oy = core::vector3df(0.0f, 1.0f, 0.0f);
	core::vector3df CGameObject::s_oz = core::vector3df(0.0f, 0.0f, 1.0f);

	CGameObject::CGameObject()
	{
		initNull();
	}

	CGameObject::CGameObject(CGameObject *parent)
	{
		initNull();

		m_parent = parent;
	}

	void CGameObject::initNull()
	{
		m_objectID = -1;
		m_name = L"NoNameObj";

		m_lighting = true;
		m_enable = true;
		m_visible = true;
		m_static = false;

		m_parent = NULL;

		m_tagData = NULL;
		m_tagDataInt = 0;
	}

	CGameObject::~CGameObject()
	{
		releaseAllComponent();
	}

	void CGameObject::remove()
	{
		if (m_parent != NULL)
		{
			CContainerObject *parent = dynamic_cast<CContainerObject*>(m_parent);
			if (parent)
				parent->removeObject(this);
		}
	}

	void CGameObject::setName(const char* lpName)
	{
		wchar_t name[1024];
		CStringImp::convertUTF8ToUnicode(lpName, name);
		m_name = name;
	}

	const char * CGameObject::getNameA()
	{
		char name[1024];
		CStringImp::convertUnicodeToUTF8(m_name.c_str(), name);
		m_namec = name;
		return m_namec.c_str();
	}

	void CGameObject::releaseAllComponent()
	{
		ArrayComponentIter iComp = m_components.begin(), iEnd = m_components.end();
		while (iComp != iEnd)
		{
			delete (*iComp);
			++iComp;
		}
		m_components.clear();
	}	

	void CGameObject::initComponent()
	{
		ArrayComponentIter i = m_components.begin(), end = m_components.end();
		while (i != end)
		{
			(*i)->initComponent();
			++i;
		}
	}

	void CGameObject::updateObject()
	{
		int numComponents = m_components.size();
		CComponentSystem** components = m_components.data();

		for (int i = 0; i < numComponents; i++)
			components[i]->updateComponent();
	}

	void CGameObject::postUpdateObject()
	{
		int numComponents = m_components.size();
		CComponentSystem** components = m_components.data();

		for (int i = 0; i < numComponents; i++)
			components[i]->postUpdateComponent();
	}

	void CGameObject::endUpdate()
	{
		int numComponents = m_components.size();
		CComponentSystem** components = m_components.data();

		for (int i = 0; i < numComponents; i++)
			components[i]->endUpdate();
	}
}
