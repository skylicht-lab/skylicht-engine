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

#include "Graphics2D/GUI/CGUIElement.h"
#include "Graphics2D/CCanvas.h"

#include "Motion/CMotion.h"
#include "Motion/CColorMotion.h"
#include "Motion/CAlphaMotion.h"
#include "Motion/CPositionMotion.h"
#include "Motion/CScaleMotion.h"
#include "Motion/CRotationMotion.h"
#include "Motion/CFrameMotion.h"
#include "Motion/CVisibleMotion.h"

namespace Skylicht
{
	/// @brief Classes used to build the User Interface (UI).
	namespace UI
	{
		class CUIContainer;

		/**
		 * @brief Base wrapper class that connects a UI container and a GUI element.
		 *
		 * @ingroup UI
		 *
		 * CUIBase represents a UI object hosted inside a CUIContainer and backed by a CGUIElement.
		 * It manages basic UI state (enable/visible), pointer state, and motion sequences (EMotionEvent).
		 *
		 * Typical responsibilities:
		 * - Forward pointer events (hover, down, up, move) to higher-level callbacks.
		 * - Manage CMotion instances per motion event and start/stop them.
		 * - Provide utility conversion between world/local UI coordinates.
		 *
		 * Subclasses (buttons, sliders, checkboxes, etc.) override virtual event handlers
		 * to implement control-specific behavior.
		 */
		class CUIBase
		{
		protected:
			CUIContainer* m_container;
			CGUIElement* m_element;

			core::vector3df m_rectTransform[4];

			std::vector<CMotion*> m_motions[(int)EMotionEvent::NumEvent];

			bool m_multiTouch;
			bool m_enable;
			bool m_visible;

			bool m_isPointerHover;
			bool m_isPointerDown;

			bool m_skipPointerEventWhenDrag;
			bool m_continueGameEvent;

			int m_pointerId;
			float m_pointerDownX;
			float m_pointerDownY;

		public:
			std::function<void(float, float)> OnPointerHover;
			std::function<void(float, float)> OnPointerOut;
			std::function<void(float, float)> OnPointerDown;
			std::function<void(float, float)> OnPointerUp;
			std::function<void(float, float, bool)> OnPointerMove;

			std::function<void(CUIBase*)> OnPressed;
			std::function<void(CUIBase*)> OnFocus;
			std::function<void(CUIBase*)> OnLostFocus;
			std::function<void(CUIBase*, const SEvent&)> OnKeyEvent;

			std::function<void(CUIBase*, EMotionEvent)> OnMotionFinish[(int)EMotionEvent::NumEvent];

		public:
			/**
			 * @brief Constructor.
			 * @param container Parent container that owns this UI object.
			 * @param element Underlying GUI element associated with this object.
			 */
			CUIBase(CUIContainer* container, CGUIElement* element);

			/** @brief Virtual destructor. */
			virtual ~CUIBase();

			/**
			 * @brief Remove this UI object from its container and perform cleanup.
			 *
			 * After calling remove(), the object should no longer be used by the container.
			 */
			void remove();

			virtual void update();

			/**
			 * @brief Enable or disable this UI object.
			 * @param b true to enable, false to disable.
			 */
			void setEnable(bool b);

			/**
			 * @brief Set visibility of this UI object.
			 * @param b true to make visible, false to hide.
			 */
			void setVisible(bool b);

			/** @brief Returns true if the object is enabled. */
			inline bool isEnable()
			{
				return m_enable;
			}

			/** @brief Returns true if the object is visible. */
			inline bool isVisible()
			{
				return m_visible;
			}

			/** @brief Enable multitouch for this ui object */
			inline void setMultiTouch(bool b)
			{
				m_multiTouch = b;
			}

			/** @brief Returns true if multi-touch is enabled for this ui object. */
			inline bool isMultiTouch()
			{
				return m_multiTouch;
			}

			/** @brief Get the pointer ID associated with this UI object. */
			inline int getPointerId()
			{
				return m_pointerId;
			}

			/** @brief Set the pointer ID for this UI object. */
			inline void setPointerId(int id)
			{
				m_pointerId = id;
			}

			/**
			 * @brief Control whether input events continue to the game layer.
			 * @param b true to let events continue; false to stop propagation.
			 */
			inline void setContinueGameEvent(bool b)
			{
				m_continueGameEvent = b;
			}

			/** @brief Returns true when events are allowed to continue to the game layer. */
			inline bool isContinueGameEvent()
			{
				return m_continueGameEvent;
			}

			/**
			 * @brief When true pointer events will be skipped while dragging.
			 * @param b true to skip pointer events during drag.
			 */
			inline void setSkipPointerEventWhenDrag(bool b)
			{
				m_skipPointerEventWhenDrag = b;
			}

			/** @brief Returns true if pointer is hovering this object. */
			inline bool isPointerHover()
			{
				return m_isPointerHover;
			}

			/** @brief Returns true if pointer is down on this object. */
			inline bool isPointerDown()
			{
				return m_isPointerDown;
			}

			/** @brief Get the underlying CGUIElement pointer */
			inline CGUIElement* getElement()
			{
				return m_element;
			}

			/**
			 * @brief Retrieve the canvas that the underlying element belongs to.
			 */
			inline CCanvas* getCanvas()
			{
				if (m_element)
					return m_element->getCanvas();
				return NULL;
			}

			/** @brief Get parent container. */
			inline CUIContainer* getContainer()
			{
				return m_container;
			}

			/** @brief Returns a pointer to the internal rect transform (4 corners). */
			core::vector3df* getRectTransform();

			/**
			 * @brief Called when pointer enters or hovers over the element.
			 * @param pointerId ID of the pointer device.
			 * @param pointerX World X coordinate of pointer.
			 * @param pointerY World Y coordinate of pointer.
			 */
			virtual void onPointerHover(int pointerId, float pointerX, float pointerY);

			/**
			 * @brief Called when pointer leaves the element.
			 * @param pointerId ID of the pointer device.
			 * @param pointerX World X coordinate at time of out event.
			 * @param pointerY World Y coordinate at time of out event.
			 */
			virtual void onPointerOut(int pointerId, float pointerX, float pointerY);

			/**
			 * @brief Called when pointer is pressed down on the element.
			 * @param pointerId ID of the pointer device.
			 * @param pointerX World X coordinate where press occurred.
			 * @param pointerY World Y coordinate where press occurred.
			 */
			virtual void onPointerDown(int pointerId, float pointerX, float pointerY);

			/**
			 * @brief Called when pointer is released over the element.
			 * @param pointerId ID of the pointer device.
			 * @param pointerX World X coordinate where release occurred.
			 * @param pointerY World Y coordinate where release occurred.
			 */
			virtual void onPointerUp(int pointerId, float pointerX, float pointerY);

			/**
			 * @brief Called when pointer moves while over or dragging the element.
			 * @param pointerId ID of the pointer device.
			 * @param pointerX World X coordinate of pointer.
			 * @param pointerY World Y coordinate of pointer.
			 */
			virtual void onPointerMove(int pointerId, float pointerX, float pointerY);

			/**
			 * @brief Called when the element is considered "pressed" (high-level event).
			 * Subclasses may trigger OnPressed callback from here.
			 */
			virtual void onPressed();

			/** @brief Called when element receives focus. */
			virtual void onFocus();

			/** @brief Called when element loses focus. */
			virtual void onLostFocus();

			/**
			 * @brief Called when a keyboard event is delivered to this element.
			 * @param event Original SEvent from the engine input system.
			 */
			virtual void onKeyEvent(const SEvent& event);

			/** @brief Reset pointer internal state (hover/down). */
			void resetPointer();

			/**
			 * @brief Start motions associated with a specific motion event.
			 * @param event Motion event to start (EMotionEvent).
			 */
			void startMotion(EMotionEvent event);

			/**
			 * @brief Query whether any motion is currently playing for the given event.
			 * @param event Motion event to query.
			 * @return true if at least one motion is playing.
			 */
			bool isMotionPlaying(EMotionEvent event);

			/**
			 * @brief Add a CMotion instance to the list for the given event.
			 * The motion will be initialized and managed by this object.
			 * @param event Motion event to attach the motion to.
			 * @param motion Pointer to CMotion (ownership semantics: container manages lifetime).
			 * @return The same CMotion pointer.
			 */
			virtual CMotion* addMotion(EMotionEvent event, CMotion* motion);

			/**
			 * @brief Add a CMotion instance bound to a specific CGUIElement for the given event.
			 * @param event Motion event to attach the motion to.
			 * @param element Element that the motion will operate on.
			 * @param motion Motion instance.
			 * @return The same CMotion pointer.
			 */
			virtual CMotion* addMotion(EMotionEvent event, CGUIElement* element, CMotion* motion);

			/**
			 * @brief Remove a motion instance from the given event list.
			 * @param event Event list to remove from.
			 * @param motion Motion to remove.
			 * @return true if motion was found and removed.
			 */
			bool removeMotion(EMotionEvent event, CMotion* motion);

			/**
			 * @brief Remove all motions attached to the specified event.
			 * @param event Event whose motions will be cleared.
			 */
			void removeMotions(EMotionEvent event);

			/**
			 * @brief Access the motion vector for a specific event.
			 * @param event Motion event.
			 * @return Reference to vector of CMotion* for the event.
			 */
			std::vector<CMotion*>& getMotions(EMotionEvent event)
			{
				return m_motions[(int)event];
			}

			/**
			 * @brief Convert screen pointer coordinates to this UI object's local UI coordinates.
			 * @param pointerX In/out: pointer X coordinate. Converted to local coordinates.
			 * @param pointerY In/out: pointer Y coordinate. Converted to local coordinates.
			 */
			void convertToUICoordinate(float& pointerX, float& pointerY);

			/**
			 * @brief Convert world coordinates to the local coordinate space of a CGUIElement.
			 * @param element Element whose local space is the target.
			 * @param x In/out: world X -> local X.
			 * @param y In/out: world Y -> local Y.
			 */
			void convertWorldToLocal(CGUIElement* element, float& x, float& y);

			/**
			 * @brief Convert local coordinates of a CGUIElement to world coordinates.
			 * @param element Element whose local space is the source.
			 * @param x In/out: local X -> world X.
			 * @param y In/out: local Y -> world Y.
			 */
			void convertLocalToWorld(CGUIElement* element, float& x, float& y);

		};
	}
}
