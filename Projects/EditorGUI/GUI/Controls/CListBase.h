/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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

#include "CBase.h"
#include "CScrollControl.h"
#include "CButton.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CListItemBase;

			class CListBase : public CScrollControl
			{
			public:
				CListBase(CBase* parent);

				virtual ~CListBase();

				void removeAllItem();

				CButton* getItemByLabel(const std::wstring& label);

				CButton* getItemByTagValue(int value);

				CButton* getSelected();

				std::list<CButton*> getAllItems();

				void unSelectAll();

				virtual bool onKeyUp(bool down);

				virtual bool onKeyDown(bool down);

				virtual bool onKeyHome(bool down);

				virtual bool onKeyEnd(bool down);

				inline void setMultiSelected(bool b)
				{
					m_multiSelected = b;
				}

				inline bool isMultiSelected()
				{
					return m_multiSelected;
				}

				virtual CListItemBase* addItem(const std::wstring& label, ESystemIcon icon) = 0;

				virtual CListItemBase* addItem(const std::wstring& label) = 0;

			public:

				Listener OnSelected;
				Listener OnUnselected;
				Listener OnSelectChange;
				Listener OnItemContextMenu;

			protected:

				virtual void onItemDown(CBase* item);

				bool m_multiSelected;
			};
		}
	}
}