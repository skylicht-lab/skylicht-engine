/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "ISubject.h"

namespace Skylicht
{
	namespace Editor
	{
		ISubject::ISubject()
		{

		}

		ISubject::~ISubject()
		{
			removeAllObserver();
		}

		IObserver* ISubject::addObserver(IObserver* observer, bool autoRelease)
		{
			if (std::find(m_observers.begin(), m_observers.end(), observer) != m_observers.end())
				return observer;

			m_observers.push_back(observer);
			m_autoRelease.push_back(autoRelease);
			return observer;
		}

		void ISubject::removeObserver(IObserver* observer)
		{
			int i = 0;
			for (IObserver* o : m_observers)
			{
				if (o == observer)
				{
					if (m_autoRelease[i])
					{
						delete observer;
					}

					m_observers.erase(m_observers.begin() + i);
					m_autoRelease.erase(m_autoRelease.begin() + i);
					return;
				}
				++i;
			}
		}

		void ISubject::notify(IObserver* from)
		{
			for (IObserver* o : m_observers)
			{
				o->onNotify(this, from);
			}
		}

		void ISubject::removeAllObserver()
		{
			int i = 0;
			for (IObserver* observer : m_observers)
			{
				if (m_autoRelease[i])
				{
					delete observer;
				}
				++i;
			}

			m_autoRelease.clear();
			m_observers.clear();
		}
	}
}