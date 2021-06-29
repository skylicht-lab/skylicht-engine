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
	class CScene : public IEventReceiver
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

		virtual CGameObject* searchObject(const wchar_t* name);

		virtual CGameObject* searchObjectInChild(const wchar_t* name);

		virtual CGameObject* searchObjectByID(const char* name);

		virtual CGameObject* searchObjectInChildByID(const char* name);

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

		void registerEvent(std::string name, IEventReceiver* pEvent);

		void unRegisterEvent(IEventReceiver* pEvent);

		virtual bool OnEvent(const SEvent& event);

		virtual void update();
	};
}