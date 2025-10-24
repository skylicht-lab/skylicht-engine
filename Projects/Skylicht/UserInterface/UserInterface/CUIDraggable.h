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
		* @class CUIDraggable
		* @brief A UI control that enables dragging of its underlying GUI element.
		*
		* @ingroup UI
		*
		* `CUIDraggable` extends `CUIBase` to provide simple pointer-driven dragging
		* behavior for a `CGUIElement`. It converts pointer events into local
		* movements of the element and exposes lifecycle callbacks for begin,
		* ongoing, and end of drag operations.
		*
		* Features:
		* - Optionally lock movement on the X or Y axis via `lock()`.
		* - Optionally constrain dragging to a rectangular region via
		*   `limitDragToRect()` and `setBounds()`.
		* - Callbacks for `OnBeginDrag`, `OnDrag` and `OnEndDrag` provide
		*   integration points for application logic.
		*
		* Notes:
		* - Coordinates passed to callbacks are world (canvas) coordinates.
		* - Dragging behavior uses pointer events delivered by `CUIBase`
		*   (`onPointerDown`, `onPointerMove`, `onPointerUp`) and implements
		*   an internal offset to preserve the initial grab position.
		*/
		class CUIDraggable : public CUIBase
		{
		protected:
			core::vector2df m_offset;
			core::vector3df m_oldPosition;

			bool m_lockX;
			bool m_lockY;

			bool m_limitDragToRect;
			core::rectf m_bounds;

		public:
			/**
			* @brief Callback invoked when a drag operation begins.
			* @param this Pointer to the draggable instance.
			* @param posX World X coordinate where the drag began.
			* @param posY World Y coordinate where the drag began.
			*
			* Typical use: start visual feedback or record initial state.
			*/
			std::function<void(CUIDraggable*, float posX, float posY)> OnBeginDrag;

			/**
			* @brief Callback invoked while dragging (on pointer move).
			* @param this Pointer to the draggable instance.
			* @param posX Current world X coordinate.
			* @param posY Current world Y coordinate.
			*
			* Typical use: update linked data, synchronize other UI, etc.
			*/
			std::function<void(CUIDraggable*, float posX, float posY)> OnDrag;

			/**
			* @brief Callback invoked when the drag operation ends.
			* @param this Pointer to the draggable instance.
			* @param posX Final world X coordinate.
			* @param posY Final world Y coordinate.
			*
			* Typical use: finalize state, persist position, stop feedback.
			*/
			std::function<void(CUIDraggable*, float posX, float posY)> OnEndDrag;

		public:
			/**
			* @brief Construct a draggable wrapper for a GUI element.
			* @param container Parent `CUIContainer` that owns this UI object.
			* @param element Underlying `CGUIElement` to be moved by dragging.
			*
			* The constructor registers this object with the container and prepares
			* internal state. It does not automatically lock axes or bounds.
			*/
			CUIDraggable(CUIContainer* container, CGUIElement* element);

			virtual ~CUIDraggable();

			/**
			* @brief Lock/unlock movement on each axis.
			* @param x true to lock X axis, false to allow X movement.
			* @param y true to lock Y axis, false to allow Y movement.
			*/
			inline void lock(bool x, bool y)
			{
				m_lockX = x;
				m_lockY = y;
			}

			/**
			* @brief Enable or disable rectangular bounds for dragging.
			* @param b true to constrain dragging to `m_bounds`, false to allow free dragging.
			*/
			inline void limitDragToRect(bool b)
			{
				m_limitDragToRect = b;
			}

			/**
			* @brief Set the rectangular bounds used when `limitDragToRect(true)` is active.
			* @param r Rectangle in the draggable's local coordinate space.
			*/
			inline void setBounds(const core::rectf& r)
			{
				m_bounds = r;
			}

		protected:

			virtual void onPointerDown(int pointerId, float pointerX, float pointerY);

			virtual void onPointerUp(int pointerId, float pointerX, float pointerY);

			virtual void onPointerMove(int pointerId, float pointerX, float pointerY);

			/**
			* @brief Apply pointer movement to the underlying element.
			*
			* Computes the new target position using `m_offset`, optional axis locks
			* (`m_lockX`, `m_lockY`) and optional bounds (`m_limitDragToRect`, `m_bounds`).
			* After moving the element it updates `m_oldPosition` and invokes `OnDrag`.
			*/
			virtual void updateDrag();

			/**
			* @brief Internal hook invoked when a drag begins.
			*
			* Subclasses can override to provide additional state setup. Default
			* implementation should invoke `OnBeginDrag` if set.
			*/
			virtual void onBeginDrag();

			/**
			* @brief Internal hook invoked when a drag ends.
			*
			* Subclasses can override to provide teardown behavior. Default
			* implementation should invoke `OnEndDrag` if set.
			*/
			virtual void onEndDrag();
		};
	}
}