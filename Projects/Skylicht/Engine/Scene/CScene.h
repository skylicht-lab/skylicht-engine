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

#include "GameObject/CZone.h"
#include "Entity/CEntityManager.h"
#include "EventManager/CEventManager.h"

#include "RenderPipeline/CForwardRP.h"
#include "RenderPipeline/CDeferredRP.h"

namespace Skylicht
{
	/// @brief This object class manages all other objects, it represents the data of a scene.
	/// @ingroup GameObject
	/// 
	/// A scene can be constructed programmatically, or you can use an Skylicht-Editor to create and arrange objects.
	/// @image html GameObject/scene_editor.jpg "The image shows a scene built with Skylicht-Editor."
	/// 
	/// When a scene is created in the editor and saved as a .scene file, you can use CSceneImporter to load all the data back into CScene.
	/// 
	/// Additionally, in some cases, you can manually create a scene using code, as shown below:
	/// @code
	/// CScene *scene = new CScene();
	/// CZone *zone = scene->createZone();
	/// 
	/// // Example add canvas
	/// CGameObject* guiObj = zone->createEmptyObject();
	/// CCanvas* canvas = guiObj->addComponent<CCanvas>();
	/// 
	/// // Example add camera
	/// CGameObject* camObj = zone->createEmptyObject();
	/// camObj->addComponent<CCamera>();
	/// camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);
	/// camObj->addComponent<CFpsMoveCamera>()->setMoveSpeed(1.0f);
	/// 
	/// // call update method for all objects
	/// scene->update();
	/// @endcode
	/// 
	/// @see CGameObject, CContainerObject, CZone, CSceneImporter
	/// 
	/// 
	/// To render the scene to screen, you first need to set up the Render Pipeline, and then call render method from the Pipeline.
	/// @see CShadowMapRP, CDeferredRP, CForwardRP, CPostProcessorRP
	class SKYLICHT_API CScene : public IEventReceiver
	{
	protected:

		std::wstring m_name;
		std::string m_namec;

		ArrayZone m_zones;

		CEntityManager* m_entityManager;

		typedef std::pair<std::string, IEventReceiver*> eventType;
		std::vector<eventType> m_eventReceivers;

	public:
		CScene();
		virtual ~CScene();

		void releaseScene();

		inline void setName(const wchar_t* lpName)
		{
			m_name = lpName;
		}

		void setName(const char* lpName);

		const char* getNameA();

		inline const wchar_t* getName()
		{
			return m_name.c_str();
		}

		void setVisibleAllZone(bool b);

		void updateAddRemoveObject();

		void updateIndexSearchObject();

		virtual CGameObject* searchObjectInChild(const wchar_t* name);

		virtual CGameObject* searchObjectInChildByID(const char* id);

		virtual CEntity* searchEntityInChildByID(const char* id);

		u32 searchObjectByCullingLayer(ArrayGameObject& result, u32 mask);

		virtual CZone* createZone();

		virtual void removeZone(CGameObject* zone);

		inline CEntityManager* getEntityManager()
		{
			return m_entityManager;
		}

		inline int getZoneCount()
		{
			return (int)m_zones.size();
		}

		inline CZone* getZone(int i)
		{
			return m_zones[i];
		}

		inline ArrayZone* getAllZone()
		{
			return &m_zones;
		}

		void bringToNext(CZone* object, CZone* target, bool behind);

		CZone* getZoneBefore(CZone* object);

		void registerEvent(std::string name, IEventReceiver* pEvent);

		void unRegisterEvent(IEventReceiver* pEvent);

		virtual bool OnEvent(const SEvent& event);

		virtual void update();

		CObjectSerializable* createSerializable();

		void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CScene)
	};
}