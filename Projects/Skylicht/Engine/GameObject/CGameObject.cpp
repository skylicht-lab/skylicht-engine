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

#include "Utils/CActivator.h"

#include "Transform/CTransformComponentData.h"

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

		m_enable = true;
		m_visible = true;
		m_static = false;
		m_isContainer = false;

		m_editorObject = false;
		m_enableEditorChange = true;
		m_enableEditorSelect = true;

		m_enableEndUpdate = false;

		m_parent = NULL;
		m_zone = NULL;
		m_entity = NULL;

		m_transform = NULL;
		m_transformEuler = NULL;
		m_transformMatrix = NULL;

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
		m_namec = CStringImp::convertUnicodeToUTF8(m_name.c_str());
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

	void CGameObject::updateEntityParent()
	{
		if (m_entity != NULL && getEntityManager() != NULL)
		{
			getEntityManager()->updateEntityParent(m_entity);
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
			CVisibleData* visible = GET_ENTITY_DATA(m_entity, CVisibleData);
			visible->CullingLayer = layer;
		}
	}

	CEntityManager* CGameObject::getEntityManager()
	{
		return m_zone->getEntityManager();
	}

	void CGameObject::setupMatrixTransform()
	{
		if (getComponent<CTransformMatrix>() == NULL)
		{
			core::matrix4 relative;
			if (getTransform())
				relative = getTransform()->getRelativeTransform();

			removeComponent<CTransformEuler>();
			CTransformMatrix* t = addComponent<CTransformMatrix>();
			t->setRelativeTransform(relative);

			CTransformComponentData* componentData = GET_ENTITY_DATA(m_entity, CTransformComponentData);
			componentData->TransformComponent = t;
			componentData->TransformComponent->setChanged(true);

			m_transformEuler = NULL;
			m_transformMatrix = t;
			m_transform = t;
		}
	}

	void CGameObject::setupEulerTransform()
	{
		if (getComponent<CTransformEuler>() == NULL)
		{
			core::matrix4 relative;
			if (getTransform())
				relative = getTransform()->getRelativeTransform();

			removeComponent<CTransformMatrix>();
			CTransformEuler* t = addComponent<CTransformEuler>();
			t->setRelativeTransform(relative);

			CTransformComponentData* componentData = GET_ENTITY_DATA(m_entity, CTransformComponentData);
			componentData->TransformComponent = t;
			componentData->TransformComponent->setChanged(true);

			m_transformMatrix = NULL;
			m_transformEuler = t;
			m_transform = t;
		}
	}

	core::vector3df CGameObject::getPosition()
	{
		return getTransform()->getRelativeTransform().getTranslation();
	}

	core::quaternion CGameObject::getRotation()
	{
		const core::matrix4& matrix = getTransform()->getRelativeTransform();
		return core::quaternion(matrix);
	}

	core::matrix4 CGameObject::calcWorldTransform()
	{
		return getTransform()->calcWorldTransform();
	}

	const core::matrix4& CGameObject::getWorldTransform()
	{
		return GET_ENTITY_DATA(m_entity, CWorldTransformData)->World;
	}

	core::vector3df CGameObject::getUp()
	{
		core::vector3df up = Transform::Oy;
		getWorldTransform().rotateVect(up);
		return up;
	}

	core::vector3df CGameObject::getFront()
	{
		core::vector3df front = Transform::Oz;
		getWorldTransform().rotateVect(front);
		return front;
	}

	void CGameObject::releaseAllComponent()
	{
		for (CComponentSystem*& comp : m_components)
			comp->removeAllLink();

		while (m_components.size() > 0)
		{
			delete m_components[0];
			m_components.erase(m_components.begin());
		}
	}

	void CGameObject::updateObject()
	{
		size_t numComponents = m_components.size();
		CComponentSystem** components = m_components.data();

		for (int i = 0; i < numComponents; i++)
		{
			if (components[i]->isEnable())
				components[i]->updateComponent();
		}
	}

	void CGameObject::endUpdate()
	{
		size_t numComponents = m_components.size();
		CComponentSystem** components = m_components.data();

		for (int i = 0; i < numComponents; i++)
		{
			if (components[i]->isEnable())
				components[i]->endUpdate();
		}
	}

	CComponentSystem* CGameObject::addComponentByTypeName(const char* name)
	{
		IActivatorObject* activator = CActivator::getInstance()->createInstance(name);
		if (activator == NULL)
			return NULL;

		CComponentSystem* compSystem = dynamic_cast<CComponentSystem*>(activator);
		if (compSystem == NULL)
		{
			delete activator;
			return NULL;
		}

		m_components.push_back(compSystem);

		compSystem->setOwner(this);
		compSystem->initComponent();

		CDependentComponent::getInstance()->createDependentComponent(compSystem);
		return compSystem;
	}

	CComponentSystem* CGameObject::getComponentByTypeName(const char* name)
	{
		size_t numComponents = m_components.size();
		CComponentSystem** components = m_components.data();

		for (int i = 0; i < numComponents; i++)
		{
			if (components[i]->getTypeName() == name)
				return components[i];
			else if (m_components[i]->getTypeName() == "CNullComponent")
			{
				// unsupport component
				CNullComponent* nullComp = (CNullComponent*)m_components[i];
				if (nullComp->getName() == name)
					return nullComp;
			}
		}

		return NULL;
	}

	bool CGameObject::removeComponent(CComponentSystem* comp)
	{
		ArrayComponentIter i = m_components.begin(), end = m_components.end();
		while (i != end)
		{
			if (*i == comp)
			{
				m_components.erase(i);
				delete comp;
				return true;
			}
			++i;
		}

		return false;
	}

	int CGameObject::getSerializableComponentCount()
	{
		int count = 0;
		ArrayComponentIter i = m_components.begin(), end = m_components.end();
		while (i != end)
		{
			if ((*i)->isSerializable())
			{
				count++;
			}
			++i;
		}

		return count;
	}

	void CGameObject::moveSerializableComponentUp(CComponentSystem* comp)
	{
		int pos = getComponentPosition(comp);
		if (pos > 0)
		{
			for (int i = pos - 1; i >= 0; i--)
			{
				if (m_components[i]->isSerializable())
				{
					CComponentSystem* temp = m_components[i];
					m_components[i] = comp;
					m_components[pos] = temp;
					return;
				}
			}
		}
	}

	void CGameObject::moveSerializableComponentDown(CComponentSystem* comp)
	{
		int pos = getComponentPosition(comp);
		if (pos < (int)m_components.size() - 1)
		{
			for (int i = pos + 1, n = (int)m_components.size(); i < n; i++)
			{
				if (m_components[i]->isSerializable())
				{
					CComponentSystem* temp = m_components[i];
					m_components[i] = comp;
					m_components[pos] = temp;
					return;
				}
			}
		}
	}

	int CGameObject::getComponentPosition(CComponentSystem* comp)
	{
		int pos = 0;
		ArrayComponentIter i = m_components.begin(), end = m_components.end();
		while (i != end)
		{
			if (*i == comp)
				break;
			++pos;
			++i;
		}

		return pos;
	}

	void CGameObject::moveComponentUp(CComponentSystem* comp)
	{
		int pos = getComponentPosition(comp);
		if (pos > 0)
		{
			CComponentSystem* temp = m_components[pos - 1];
			m_components[pos - 1] = comp;
			m_components[pos] = temp;
		}
	}

	void CGameObject::moveComponentDown(CComponentSystem* comp)
	{
		int pos = getComponentPosition(comp);
		if (pos < (int)m_components.size() - 1)
		{
			CComponentSystem* temp = m_components[pos + 1];
			m_components[pos + 1] = comp;
			m_components[pos] = temp;
		}
	}

	void CGameObject::startComponent()
	{
		// run start Component after import scene (seriable is Serializable)
		size_t numComponents = m_components.size();
		CComponentSystem** components = m_components.data();

		for (int i = 0; i < numComponents; i++)
		{
			components[i]->startComponent();
		}
	}

	CObjectSerializable* CGameObject::createSerializable()
	{
		CObjectSerializable* object = new CObjectSerializable(getTypeName().c_str());
		object->autoRelease(new CStringProperty(object, "id", getID().c_str()));
		object->autoRelease(new CStringProperty(object, "name", getNameA()));
		object->autoRelease(new CBoolProperty(object, "enable", isEnable()));
		object->autoRelease(new CBoolProperty(object, "visible", isVisible()));
		object->autoRelease(new CBoolProperty(object, "static", isStatic()));
		object->autoRelease(new CUIntProperty(object, "culling", getCullingLayer()));

		CObjectSerializable* coms = new CObjectSerializable("Components");
		object->addProperty(coms);
		object->autoRelease(coms);

		size_t numComponents = m_components.size();
		CComponentSystem** components = m_components.data();

		for (int i = 0; i < numComponents; i++)
		{
			if (!components[i]->isSerializable())
				continue;

			CObjectSerializable* data = components[i]->createSerializable();
			if (data != NULL)
			{
				if (data->Name != "CComponentSystem")
				{
					coms->addProperty(data);
					coms->autoRelease(data);
				}
				else
				{
					delete data;
				}
			}
		}

		return object;
	}

	void CGameObject::loadSerializable(CObjectSerializable* object)
	{
		setID(object->get("id", std::string("")).c_str());
		setName(object->get("name", std::string("")).c_str());
		setEnable(object->get("enable", true));
		setVisible(object->get("visible", true));
		setStatic(object->get("static", true));
		setCullingLayer(object->get<u32>("culling", 1));

		CObjectSerializable* coms = object->getProperty<CObjectSerializable>("Components");
		for (int i = 0, n = coms->getNumProperty(); i < n; i++)
		{
			CObjectSerializable* componentData = dynamic_cast<CObjectSerializable*>(coms->getPropertyID(i));
			if (componentData != NULL)
			{
				CComponentSystem* comSystem = getComponentByTypeName(componentData->Name.c_str());
				if (comSystem == NULL)
				{
					comSystem = addComponentByTypeName(componentData->Name.c_str());
					if (comSystem == NULL)
						continue;
				}
				comSystem->loadSerializable(componentData);
			}
		}
	}
}
