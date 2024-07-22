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

#pragma once

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CBase;

			class CTabableGroup
			{
			public:
				CBase* CurrentTab;

			protected:
				bool m_enable;

				struct SElement
				{
					bool Enable;
					CBase* Control;

					SElement(CBase* control, bool enable)
					{
						Control = control;
						Enable = enable;
					}
				};

				std::list<SElement> m_list;
				
			public:
				CTabableGroup();

				virtual ~CTabableGroup();

				void disableAllControl();
				
				void setEnableControl(CBase* control);
				
				void add(CBase* control);

				void remove(CBase* control);

				void next();

				void clear();

				inline bool isEnable()
				{
					return m_enable;
				}

				inline void setEnable(bool b)
				{
					m_enable = b;
				}
			};
		}
	}
}
