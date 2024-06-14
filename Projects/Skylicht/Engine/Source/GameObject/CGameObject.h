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

#include "pch.h"
#include "Components/CComponentSystem.h"
#include "Components/CNullComponent.h"
#include "Components/CDependentComponent.h"
#include "Transform/CTransformEuler.h"
#include "Transform/CTransformMatrix.h"
#include "Serializable/CObjectSerializable.h"

namespace Skylicht
{
	class CScene;
	class CZone;
	class CEntity;
	class CEntityManager;

	class SKYLICHT_API CGameObject
	{
		friend class CDependentComponent;

	protected:
		std::string m_objectID;

		std::wstring m_name;
		std::wstring m_defaultName;
		std::string m_namec;

		bool m_static;
		bool m_enable;
		bool m_visible;
		bool m_isContainer;

		bool m_editorObject;
		bool m_enableEditorChange;
		bool m_enableEditorSelect;
		bool m_enableEndUpdate;

		u32 m_cullingLayer;

		CEntity* m_entity;
		CGameObject* m_parent;
		CZone* m_zone;

		void* m_tagData;
		int m_tagDataInt;
		std::string m_tagDataString;

		std::vector<CComponentSystem*> m_components;

		CTransform* m_transform;
		CTransformEuler* m_transformEuler;
		CTransformMatrix* m_transformMatrix;

	public:
		CGameObject(CGameObject* parent, CZone* zone);

		virtual ~CGameObject();

	protected:
		void initNull();

	public:
		CEntity* createEntity();

		void destroyEntity();

		void updateEntityParent();

		virtual void setID(const char* id)
		{
			m_objectID = id;
		}

		inline std::string& getID()
		{
			return m_objectID;
		}

		inline const wchar_t* getName()
		{
			return m_name.c_str();
		}

		const char* getNameA();

		inline const wchar_t* getDefaultName()
		{
			return m_defaultName.c_str();
		}

		inline void setName(const wchar_t* lpName)
		{
			m_name = lpName;

			if (m_defaultName == L"")
				m_defaultName = lpName;
		}

		void setName(const char* lpName);

		inline CGameObject* getParent()
		{
			return m_parent;
		}

		inline CZone* getZone()
		{
			return m_zone;
		}

		CScene* getScene();

		inline void setParent(CGameObject* p)
		{
			m_parent = p;
		}

		inline CEntity* getEntity()
		{
			return m_entity;
		}

		CEntityManager* getEntityManager();

		inline CTransform* getTransform()
		{
			return m_transform;
		}

		inline CTransformEuler* getTransformEuler()
		{
			return m_transformEuler;
		}

		inline CTransformMatrix* getTransformMatrix()
		{
			return m_transformMatrix;
		}

		void setupMatrixTransform();

		void setupEulerTransform();

		core::vector3df getPosition();

		core::quaternion getRotation();

		core::vector3df getUp();

		core::vector3df getFront();

		inline const core::matrix4& getWorldTransform();

		core::matrix4 calcWorldTransform();

		inline bool isEnable()
		{
			return m_enable;
		}

		inline bool isEnableEndUpdate()
		{
			return m_enableEndUpdate;
		}

		inline bool isVisible()
		{
			return m_visible;
		}

		virtual void setEnable(bool b)
		{
			m_enable = b;
		}

		virtual void setEnableEndUpdate(bool b)
		{
			m_enableEndUpdate = b;
		}

		virtual void setStatic(bool b)
		{
			m_static = b;
		}

		virtual bool isStatic()
		{
			return m_static;
		}

		virtual void setVisible(bool b);

		inline bool isContainer()
		{
			return m_isContainer;
		}

		void setCullingLayer(u32 layer);

		inline u32 getCullingLayer()
		{
			return m_cullingLayer;
		}

		inline void setEditorObject(bool b)
		{
			m_editorObject = b;
		}

		inline bool isEditorObject()
		{
			return m_editorObject;
		}

		inline void tagData(void* data)
		{
			m_tagData = data;
		}

		inline void tagData(int data)
		{
			m_tagDataInt = data;
		}

		inline void tagData(std::string& s)
		{
			m_tagDataString = s;
		}

		inline void* getTagData()
		{
			return m_tagData;
		}

		inline int getTagDataInt()
		{
			return m_tagDataInt;
		}

		const char* getTagDataString()
		{
			return m_tagDataString.c_str();
		}

		virtual void updateObject();

		virtual void endUpdate();

		virtual void remove();

		template<class T>
		T* addComponent();

		template<class T>
		T* getComponent();

		template<class T>
		bool removeComponent();

		bool removeComponent(CComponentSystem* comp);

		void releaseAllComponent();

		CComponentSystem* addComponentByTypeName(const char* name);

		CComponentSystem* getComponentByTypeName(const char* name);

		inline std::vector<CComponentSystem*>& getListComponent()
		{
			return m_components;
		}

		int getComponentPosition(CComponentSystem* comp);

		inline int getComponentCount()
		{
			return (int)m_components.size();
		}

		void moveComponentUp(CComponentSystem* comp);

		void moveComponentDown(CComponentSystem* comp);

		int getSerializableComponentCount();

		void moveSerializableComponentUp(CComponentSystem* comp);

		void moveSerializableComponentDown(CComponentSystem* comp);

		inline void enableEditorChange(bool b)
		{
			m_enableEditorChange = b;
		}

		inline void enableEditorSelect(bool b)
		{
			m_enableEditorSelect = b;
		}

		inline bool isEnableEditorChange()
		{
			return m_enableEditorChange;
		}

		inline bool isEnableEditorSelect()
		{
			return m_enableEditorSelect;
		}

		void startComponent();

		CObjectSerializable* createSerializable();

		void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CGameObject)
	};

	// typedef for array object
	typedef std::vector<CGameObject*> ArrayGameObject;
	typedef std::vector<CGameObject*>::iterator ArrayGameObjectIter;

	// typedef for array component system
	typedef std::vector<CComponentSystem*> ArrayComponent;
	typedef std::vector<CComponentSystem*>::iterator ArrayComponentIter;

	template<class T>
	T* CGameObject::addComponent()
	{
		T* newComp = new T();
		CComponentSystem* compSystem = dynamic_cast<CComponentSystem*>(newComp);
		if (compSystem == NULL)
		{
			char exceptionInfo[512];
			sprintf(exceptionInfo, "CGameObject::addComponent %s must inherit CComponentSystem", typeid(T).name());
			os::Printer::log(exceptionInfo);
			delete newComp;
			return NULL;
		}

		m_components.push_back(compSystem);

		compSystem->setOwner(this);
		compSystem->initComponent();

		CDependentComponent::createGetInstance()->createDependentComponent(compSystem);
		return newComp;
	}

	template<class T>
	T* CGameObject::getComponent()
	{
		ArrayComponentIter i = m_components.begin(), end = m_components.end();
		while (i != end)
		{
			T* c = dynamic_cast<T*>(*i);
			if (c != NULL)
				return c;
			++i;
		}
		return NULL;
	}

	template<class T>
	bool CGameObject::removeComponent()
	{
		ArrayComponentIter i = m_components.begin(), end = m_components.end();
		while (i != end)
		{
			T* c = dynamic_cast<T*>(*i);
			if (c != NULL)
			{
				m_components.erase(i);
				delete c;
				return true;
			}
			++i;
		}

		return false;
	}
}