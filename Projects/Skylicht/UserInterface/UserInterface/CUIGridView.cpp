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

#include "pch.h"
#include "CUIContainer.h"
#include "CUIGridView.h"
#include "Graphics2D/CGUIImporter.h"
#include "Graphics2D/CGUIExporter.h"

#include "CUIEventManager.h"

namespace Skylicht
{
	namespace UI
	{
		CUIGridView::CUIGridView(CUIContainer* container, CGUIElement* element, CGUIElement* baseItem) :
			CUIListView(container, element, baseItem),
			m_itemSpacingX(0.0f),
			m_itemSpacingY(0.0f)
		{

		}

		CUIGridView::~CUIGridView()
		{

		}

		void CUIGridView::updateLimitOffset()
		{
			m_springOffset = 0.0f;
			m_maxOffset = 0.0f;

			if (m_items.size() > 0)
			{
				float height = m_element->getHeight();
				float width = m_element->getWidth();

				float x = 0.0f;
				float y = 0.0f;

				if (m_vertical)
				{
					m_springOffset = height * 0.2f;

					float lastItemHeight = 0.0f;
					float itemHeight = 0.0f;

					for (CGUIElement* item : m_items)
					{
						if (item->getHeight() > itemHeight)
						{
							itemHeight = item->getHeight();
							lastItemHeight = itemHeight;
						}

						x = x + item->getWidth() + m_itemSpacingX;
						if (x >= width - item->getWidth())
						{
							x = 0.0f;
							y = y + itemHeight + m_itemSpacingY;
							itemHeight = 0.0f;
						}
					}

					m_maxOffset = y + lastItemHeight;
					m_maxOffset = m_maxOffset - height;
					if (m_maxOffset < 0.0f)
						m_maxOffset = 0.0f;
				}
				else
				{
					m_springOffset = width * 0.2f;

					float lastItemWidth = 0.0f;
					float itemWidth = 0.0f;

					for (CGUIElement* item : m_items)
					{
						if (item->getWidth() > itemWidth)
						{
							itemWidth = item->getWidth();
							lastItemWidth = itemWidth;
						}

						y = y + item->getHeight() + m_itemSpacingY;
						if (y >= height - item->getHeight())
						{
							y = 0.0f;
							x = x + itemWidth + m_itemSpacingX;
							itemWidth = 0.0f;
						}
					}

					m_maxOffset = x + lastItemWidth;
					m_maxOffset = m_maxOffset - width;
					if (m_maxOffset < 0.0f)
						m_maxOffset = 0.0f;
				}
			}
		}

		void CUIGridView::updateItemPosition()
		{
			float x = 0.0f, y = 0.0f;

			float width = m_element->getWidth();
			float height = m_element->getHeight();

			if (m_vertical)
			{
				x = 0.0f;
				y = m_offset;
			}
			else
			{
				x = m_offset;
				y = 0.0f;
			}

			float itemWidth = 0.0f;
			float itemHeight = 0.0f;

			for (CGUIElement* item : m_items)
			{
				if (m_vertical)
				{
					if (item->getHeight() > itemHeight)
						itemHeight = item->getHeight();

					item->setPosition(core::vector3df(x, y, 0.0f));
					x = x + item->getWidth() + m_itemSpacingX;

					if (x >= width - item->getWidth())
					{
						x = 0.0f;
						y = y + itemHeight + m_itemSpacingY;
						itemHeight = 0.0f;
					}
				}
				else
				{
					if (item->getWidth() > itemWidth)
						itemWidth = item->getWidth();

					item->setPosition(core::vector3df(x, y, 0.0f));
					y = y + item->getHeight() + m_itemSpacingY;

					if (y >= height - item->getHeight())
					{
						y = 0.0f;
						x = x + itemWidth + m_itemSpacingX;
						itemWidth = 0.0f;
					}
				}
			}
		}
	}
}