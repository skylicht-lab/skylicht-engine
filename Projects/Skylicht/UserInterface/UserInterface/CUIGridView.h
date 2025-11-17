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
#include "CUIListView.h"

namespace Skylicht
{
	namespace UI
	{
		/**
		* @brief Grid view control (list view with multi-column layout).
		*
		* @ingroup UI
		* 
		* `CUIGridView` extends `CUIListView` to arrange items in a two-dimensional
		* grid instead of a single row/column. Items are positioned using horizontal
		* and vertical spacing values; scrolling and input behavior are inherited
		* from `CUIListView`.
		*
		* The class overrides the limit and layout update hooks:
		* - `updateLimitOffset()` recalculates maximum scroll offset for a grid.
		* - `updateItemPosition()` places items into rows and columns using the
		*   configured spacing.
		*
		* @note `CUIGridView` reuses the `m_baseItem` template from `CUIListView`
		*       to create each grid cell.
		*
		* @see CUIListView
		*/
		class CUIGridView : public CUIListView
		{
		protected:
			float m_itemSpacingX;
			float m_itemSpacingY;

		public:
			/**
			* @brief Create a grid view.
			* @param container Parent `CUIContainer` that owns this control.
			* @param element Root `CGUIElement` node for the grid view.
			* @param baseItem Template `CGUIElement` used to instantiate grid cells.
			* @param listElement The list display items (if null, it will be element).
			*
			* The `baseItem` is duplicated/cloned to create each grid entry (same
			* behaviour as `CUIListView`).
			*/
			CUIGridView(CUIContainer* container, CGUIElement* element, CGUIElement* baseItem, CGUIElement* listElement = NULL);

			virtual ~CUIGridView();

			virtual void updateLimitOffset();

			virtual void updateItemPosition();

			/**
			* @brief Set spacing between items on X and Y axes.
			* @param x Horizontal spacing (pixels / world units).
			* @param y Vertical spacing (pixels / world units).
			*
			* This affects how `updateItemPosition()` arranges items in the grid.
			*/
			inline void setItemSpacing(float x, float y)
			{
				m_itemSpacingX = x;
				m_itemSpacingY = y;
			}
		};
	}
}