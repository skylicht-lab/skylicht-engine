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
		/**
		 * @brief Scrollable list view control.
		 *
		 * @ingroup UI
		 *
		 * `CUIListView` displays a list of item elements and provides basic scrolling
		 * with inertia, spring-back when over-scrolled, and optional masking.
		 *
		 * The control can operate in vertical or horizontal mode. Items are instances of
		 * `CGUIElement` created from a `m_baseItem` template. The list manages item positions,
		 * spacing and movement; it tracks pointer input to support drag scrolling and click selection.
		 *
		 * The class exposes protected hooks (`updateLimitOffset`, `updateItemPosition`, etc.)
		 * to customize behaviour in subclasses.
		 */
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
			/**
			 * @brief Create a list view.
			 * @param container Parent `CUIContainer` that owns this control.
			 * @param element Root `CGUIElement` node for the list view.
			 * @param baseItem Template `CGUIElement` used to instantiate new items.
			 *
			 * The `baseItem` is duplicated/cloned (implementation detail) to create list entries.
			 */
			CUIListView(CUIContainer* container, CGUIElement* element, CGUIElement* baseItem);

			virtual ~CUIListView();

			/**
			 * @brief Create a new item using the `m_baseItem` template and append it to the list.
			 * @return Pointer to the newly created `CGUIElement` item.
			 *
			 * Caller can further configure the returned element (set texts, attach data, etc.).
			 */
			CGUIElement* addItem();

			/**
			 * @brief Create a new item and attach `data` to it (using serialization template).
			 * @param data Serializable object to initialize the item.
			 * @return Pointer to the newly created item.
			 */
			CGUIElement* addItem(CObjectSerializable* data);

			/**
			 * @brief Enable or disable visual masking for the list viewport.
			 * @param b true to enable the mask (clip children), false to disable.
			 */
			void enableMask(bool b);

			/**
			 * @brief Remove a specific item from the list, The function also remove and destroy all child UI elements created from the list's base item
			 * @param item Pointer to item to remove.
			 * @return true if the item was found and removed; false otherwise.
			 */
			bool removeItem(CGUIElement* item);

			/**
			 * @brief Remove and destroy all child UI elements created from the list's base item.
			 *
			 * This function removes every UI element that was added to the list via `addItem()`
			 * (i.e. elements instantiated from `m_baseItem` or otherwise created by the list).
			 * Each removed item is detached from the list's element, any internal references are
			 * cleared and the internal `m_items` container is emptied.
			 *
			 * Important notes:
			 * - The `m_baseItem` template itself is preserved and is not removed.
			 * - External pointers to removed items become invalid after this call.
			 * - Associated serializable data (if used via `m_itemSerializable`) will no longer be
			 *   referenced by the list after clearing.
			 * - The method will typically reset scattering state such as offsets/speed so the list
			 *   is left in a clean state for future `addItem()` calls.
			 */
			void clear();

			/** @brief Return number of items currently in the list. */
			inline int getItemCount()
			{
				return (int)m_items.size();
			}

			/**
			 * @brief Get item by index.
			 * @param id Zero-based index of the item.
			 * @return Pointer to `CGUIElement` or nullptr if index is out of range.
			 */
			CGUIElement* getItem(int id);

			virtual void update();

			virtual void onPointerDown(int pointerId, float pointerX, float pointerY);

			virtual void onPointerUp(int pointerId, float pointerX, float pointerY);

			virtual void onPointerMove(int pointerId, float pointerX, float pointerY);
			
			/** @brief Set vertical layout mode. */
			inline void setVertical(bool b)
			{
				m_vertical = b;
			}

			/** @brief Returns true when layout is vertical. */
			inline bool isVertical()
			{
				return m_vertical;
			}

			/** @brief Set spacing between items. */
			inline void setItemSpacing(float b)
			{
				m_itemSpacing = b;
			}

			/** @brief Get spacing between items. */
			inline float getItemSpacing()
			{
				return m_itemSpacing;
			}

			/** @brief Get current scroll offset. */
			inline float getOffset()
			{
				return m_offset;
			}

			/**
			 * @brief Set content offset programmatically.
			 * @param offset New offset value. Resets inertia speed to zero.
			 */
			inline void setOffset(float offset)
			{
				m_speed = 0.0f;
				m_offset = offset;
			}

			/** @brief Returns maximum computed offset (content size - viewport size). */
			inline float getMaxOffset()
			{
				return m_maxOffset;
			}

			virtual bool acceptDragFocus()
			{
				return true;
			}
			
		protected:

			/**
			 * @brief Update computed limits for allowable offset (`m_maxOffset` and related).
			 *
			 * Called when items change or layout needs recalculation.
			 * Subclasses may override to implement different bounds logic.
			 */
			virtual void updateLimitOffset();

			/**
			 * @brief Recompute and apply item world/local positions based on `m_offset`.
			 *
			 * Positions should account for `m_itemSpacing` and `m_vertical` mode.
			 */
			virtual void updateItemPosition();

			/**
			 * @brief Update item movement logic (applies `m_speed`, clamps and spring-back).
			 *
			 * This method centralizes how `m_offset` and `m_speed` are adjusted each frame.
			 */
			virtual void updateItemMovement();

			/** @brief Compute and apply inertia when the pointer is released and `m_speed` != 0. */
			virtual void updateInertia();

			/** @brief Damp and stop the scrolling speed smoothly when near rest. */
			virtual void updateStopSpeed();
		};
	}
}
