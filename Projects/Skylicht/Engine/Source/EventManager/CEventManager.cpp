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
#include "CEventManager.h"

namespace Skylicht
{
	CEventManager::CEventManager()
	{

	}

	CEventManager::~CEventManager()
	{
		m_eventReceivers.clear();
	}

	void CEventManager::registerEvent(std::string name, IEventReceiver *pEvent)
	{
		std::vector<eventType>::iterator i = m_eventReceivers.begin(), end = m_eventReceivers.end();
		while (i != end)
		{
			if ((*i).second == pEvent)
				return;
			++i;
		}

		m_eventReceivers.push_back(eventType(name, pEvent));
	}

	void CEventManager::unRegisterEvent(IEventReceiver *pEvent)
	{
		std::vector<eventType>::iterator i = m_eventReceivers.begin(), end = m_eventReceivers.end();
		while (i != end)
		{
			if ((*i).second == pEvent)
			{
				m_eventReceivers.erase(i);
				return;
			}
			++i;
		}
	}

	bool CEventManager::OnEvent(const SEvent& event)
	{
		// need copy to another array.
		// because the application will crash if registerEvent or unregister on process list Event
		std::vector<eventType> eventWillProcess = m_eventReceivers;

		std::vector<eventType>::iterator i = eventWillProcess.begin(), end = eventWillProcess.end();
		while (i != end)
		{
			bool sendEvent = false;

			// we need check
			std::vector<eventType>::iterator j = m_eventReceivers.begin(), jend = m_eventReceivers.end();
			while (j != jend)
			{
				if ((*i).second == (*j).second)
				{
					sendEvent = true;
					break;
				}
				++j;
			}

			// ok send event
			if (sendEvent)
			{
				(*i).second->OnEvent(event);
			}

			++i;
		}

		return false;
	}
}