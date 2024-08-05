/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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
#include "CUIEventManager.h"
#include "CUIContainer.h"

namespace Skylicht
{
	namespace UI
	{
		IMPLEMENT_SINGLETON(CUIEventManager);

		CUIEventManager::CUIEventManager()
		{
			CEventManager::getInstance()->registerProcessorEvent("CUIEventManager", this);
		}

		CUIEventManager::~CUIEventManager()
		{
			CEventManager::getInstance()->unRegisterProcessorEvent(this);
		}

		bool CUIEventManager::OnProcessEvent(const SEvent& event)
		{
			std::vector<CUIContainer*> list = m_containers;

			std::sort(list.begin(), list.end(), [](CUIContainer*& a, CUIContainer*& b)
				{
					return a->getCanvas()->getSortDepth() > b->getCanvas()->getSortDepth();
				});

			for (CUIContainer* ui : list)
			{
				if (ui->OnProcessEvent(event) == false)
					return false;
			}

			return true;
		}

		void CUIEventManager::registerUIContainer(CUIContainer* container)
		{
			std::vector<CUIContainer*>::iterator i = m_containers.begin(), end = m_containers.end();
			while (i != end)
			{
				if (*i == container)
					return;
				++i;
			}

			m_containers.push_back(container);
		}

		void CUIEventManager::unregisterUIContainer(CUIContainer* container)
		{
			std::vector<CUIContainer*>::iterator i = m_containers.begin(), end = m_containers.end();
			while (i != end)
			{
				if (*i == container)
				{
					m_containers.erase(i);
					return;
				}
				++i;
			}
		}
	}
}