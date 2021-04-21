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
#include "CZone.h"
#include "Scene/CScene.h"

#include "Utils/CStringImp.h"
#include "Culling/CVisibleData.h"

namespace Skylicht
{

	CGameObject::CGameObject(CGameObject* parent, CZone* zone)
	{
		initNull();

		m_parent = parent;
		m_zone = zone;
	}

	void CGameObject::initNull()
	{
		m_objectID = "";
		m_name = L"NoNameObj";

		m_lighting = true;
		m_enable = true;
		m_visible = true;
		m_static = false;

		m_editorObject = false;

		m_parent = NULL;
		m_zone = NULL;
		m_entity = NULL;

		m_tagData = NULL;
		m_tagDataInt = 0;
		m_cullingLayer = 1;
	}

	CGameObject::~CGameObject()
	{
		releaseAllComponent();
		destroyEntity();
	}

	void CGameObject::remove()
	{
		if (m_parent != NULL)
		{
			CContainerObject* parent = dynamic_cast<CContainerObject*>(m_parent);
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

	const char* CGameObject::getNameA()
	{
		char name[1024];
		CStringImp::convertUnicodeToUTF8(m_name.c_str(), name);
		m_namec = name;
		return m_namec.c_str();
	}

	CScene* CGameObject::getScene()
	{
		return m_zone->getScene();
	}

	CEntity* CGameObject::createEntity()
	{
		if (m_entity != NULL)
			return m_entity;

		CEntityManager* entityManager = getEntityManager();
		if (entityManager == NULL)
			return NULL;

		// create entity
		m_entity = entityManager->createEntity();

		return m_entity;
	}

	void CGameObject::destroyEntity()
	{
		if (m_entity != NULL && getEntityManager() != NULL)
		{
			getEntityManager()->removeEntity(m_entity);
			m_entity = NULL;
		}
	}

	void CGameObject::setVisible(bool b)
	{
		m_visible = b;
		if (m_entity != NULL)
			m_entity->setVisible(b);
	}

	void CGameObject::setCullingLayer(u32 layer)
	{
		m_cullingLayer = layer;
		if (m_entity != NULL)
		{
			CVisibleData* visible = m_entity->getData<CVisibleData>();
			visible->CullingLayer = layer;
		}
	}

	CEntityManager* CGameObject::getEntityManager()
	{
		return m_zone->getEntityManager();
	}

	CTransform* CGameObject::getTransform()
	{
		return getComponent<CTransform>();
	}

	CTransformEuler* CGameObject::getTransformEuler()
	{
		return getComponent<CTransformEuler>();
	}

	core::vector3df CGameObject::getPosition()
	{
		return getTransform()->getMatrixTransform().getTranslation();
	}

	core::quaternion CGameObject::getRotation()
	{
		const core::matrix4& matrix = getTransform()->getMatrixTransform();
		return core::quaternion(matrix);
	}

	void CGameObject::releaseAllComponent()
	{
		for (CComponentSystem*& comp : m_components)
			delete comp;
		m_components.clear();
	}

	void CGameObject::updateObject()
	{
		int numComponents = (int)m_components.size();
		CComponentSystem** components = m_components.data();

		for (int i = 0; i < numComponents; i++)
		{
			if (components[i]->isEnable())
				components[i]->updateComponent();
		}
	}

	void CGameObject::postUpdateObject()
	{
		int numComponents = (int)m_components.size();
		CComponentSystem** components = m_components.data();

		for (int i = 0; i < numComponents; i++)
		{
			if (components[i]->isEnable())
				components[i]->postUpdateComponent();
		}
	}

	void CGameObject::endUpdate()
	{
		int numComponents = (int)m_components.size();
		CComponentSystem** components = m_components.data();

		for (int i = 0; i < numComponents; i++)
		{
			if (components[i]->isEnable())
				components[i]->endUpdate();
		}
	}
}
