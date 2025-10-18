/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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

#include "CUIBase.h"

namespace Skylicht
{
	namespace UI
	{
		class CUIListView : public CUIBase
		{
		protected:
			bool m_vertical;

			CGUIMask* m_mask;

			std::vector<CGUIElement*> m_items;

			CGUIElement* m_baseItem;
			CObjectSerializable* m_itemSerializable;

			float m_offset;
			float m_speed;

			float m_lastPointerX;
			float m_lastPointerY;
			float m_pointerX;
			float m_pointerY;

			float m_maxOffset;
			float m_springOffset;
			float m_itemSpacing;

		public:
			CUIListView(CUIContainer* container, CGUIElement* element, CGUIElement* baseItem);

			virtual ~CUIListView();

			CGUIElement* addItem();

			CGUIElement* addItem(CObjectSerializable* data);

			void enableMask(bool b);

			bool removeItem(CGUIElement* item);

			void clear();

			inline int getItemCount()
			{
				return (int)m_items.size();
			}

			CGUIElement* getItem(int id);

			virtual void update();

			virtual void onPointerDown(float pointerX, float pointerY);

			virtual void onPointerUp(float pointerX, float pointerY);

			virtual void onPointerMove(float pointerX, float pointerY);

			inline void setVertical(bool b)
			{
				m_vertical = b;
			}

			inline bool isVertical()
			{
				return m_vertical;
			}

			inline void setItemSpacing(float b)
			{
				m_itemSpacing = b;
			}

			inline float getItemSpacing()
			{
				return m_itemSpacing;
			}

			inline float getOffset()
			{
				return m_offset;
			}

			inline void setOffset(float offset)
			{
				m_speed = 0.0f;
				m_offset = offset;
			}

			inline float getMaxOffset()
			{
				return m_maxOffset;
			}

		protected:

			virtual void updateLimitOffset();

			virtual void updateItemPosition();

			virtual void updateItemMovement();

			virtual void updateInertia();

			virtual void updateStopSpeed();
		};
	}
}