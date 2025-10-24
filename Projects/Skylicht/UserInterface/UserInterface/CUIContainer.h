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

#include "CUIEventManager.h"
#include "Components/CComponentSystem.h"

#include "CUIBase.h"

namespace Skylicht
{
	namespace UI
	{
		/**
		* @brief Top-level UI container and event router.
		*
		* @ingroup UI
		*
		* `CUIContainer` manages a collection of `CUIBase` objects and acts as the
		* bridge between low-level input events (irrlicht `SEvent`) and high-level
		* UI interactions. Responsibilities include:
		* - owning and updating child UI objects,
		* - performing hit/raycast ordering through `m_raycastUIObjects`,
		* - forwarding pointer and keyboard events to the appropriate child,
		* - managing focus/capture semantics via `CUIEventManager`,
		* - controlling simple motion sequences (in/out) and invoking finish callbacks.
		*
		* The container is itself a `CComponentSystem` so it integrates with the
		* engine update loop through `initComponent()` and `updateComponent()`.
		*
		* @note Most UI controls are created as `CUIBase` subclasses and attached
		*       to a container with `addChild()`. Event processing is done through
		*       `OnProcessEvent()` which returns the `CUIBase*` that handled the event
		*       (or NULL if none).
		*/
		class CUIContainer : public CComponentSystem
		{
		protected:
			bool m_enable;

			std::vector<CUIBase*> m_arrayUIObjects;
			std::vector<CUIBase*> m_raycastUIObjects;

			CUIBase* m_skip;
			CUIBase* m_hover;
			CCanvas* m_canvas;

			bool m_inMotion;
			bool m_outMotion;

		public:
			/**
			* @brief Callback invoked when in-motion sequence finishes.
			*
			* Used by UI transitions that require a post-animation action.
			*/
			std::function<void()> OnMotionInFinish;

			/**
			* @brief Callback invoked when out-motion sequence finishes.
			*
			* Used by UI transitions that require teardown or state change after hiding.
			*/
			std::function<void()> OnMotionOutFinish;

		public:
			CUIContainer();

			virtual ~CUIContainer();

			virtual void initComponent();

			virtual void updateComponent();

			/**
			* @brief Get the associated canvas.
			* @return Pointer to the `CCanvas` used by this container.
			*/
			CCanvas* getCanvas();

			/**
			* @brief Add a child UI object to the container.
			* @param base `CUIBase*` to add. The container will own and update it.
			*/
			void addChild(CUIBase* base);

			/**
			* @brief Remove a child UI object from the container.
			* @param base `CUIBase*` to remove.
			* @return true if the child was found and removed; false otherwise.
			*/
			bool removeChild(CUIBase* base);

			/**
			* @brief Remove all children that are backed by the specified GUI element.
			* @param element `CGUIElement*` whose associated UI objects will be removed.
			*/
			void removeChildsByGUI(CGUIElement* element);

			/**
			* @brief Find a child by its underlying GUI element.
			* @param element GUI element to search for.
			* @return Pointer to the `CUIBase` that wraps the element or NULL if not found.
			*/
			CUIBase* getChildByGUI(CGUIElement* element);

			/**
			* @brief Process an input event and forward it to the appropriate child.
			* @param event Low-level input `SEvent`.
			* @return The `CUIBase*` that handled the event, or NULL if none handled it.
			*
			* This overload resolves events using the container's internal capture/focus state.
			*/
			virtual CUIBase* OnProcessEvent(const SEvent& event);

			/**
			* @brief Process an input event with an explicit capture hint.
			* @param event   Low-level input `SEvent`.
			* @param capture `CUIBase*` currently capturing input (may be NULL).
			* @return The `CUIBase*` that handled the event, or NULL if none handled it.
			*
			* Use this overload when event routing should respect a specific capture target.
			*/
			virtual CUIBase* OnProcessEvent(const SEvent& event, CUIBase* capture);

			/**
			* @brief Notify container that pointer left the given position.
			* @param pointerId ID of the pointer that moved out.
			* @param x World X coordinate of pointer out.
			* @param y World Y coordinate of pointer out.
			*
			* This will clear hover state and forward onPointerOut to the previously
			* hovered child if any.
			*/
			virtual void onPointerOut(int pointerId, float x, float y);

			/**
			* @brief Cancel an active pointer down on a child.
			* @param base Pointer to the child where pointer down should be cancelled.
			* @param pointerId ID of the pointer to cancel.
			* @param pointerX Last known pointer X (world).
			* @param pointerY Last known pointer Y (world).
			*
			* This is used to abort press interactions when another system needs to
			* interrupt the ongoing pointer down state.
			*/
			virtual void cancelPointerDown(CUIBase* base, int pointerId, float pointerX, float pointerY);

			/** @brief Start the "in" motion sequence and set internal state. */
			void startInMotion();

			/** @brief Start the "out" motion sequence and set internal state. */
			void startOutMotion();

			/** @brief Enable or disable the container processing. */
			inline void setEnable(bool b)
			{
				m_enable = b;
			}

			/**
			* @brief Enable or disable the container processing.
			*
			* When set to false the container will ignore pointer/touch events and
			* stop routing input to its child UI objects. `OnProcessEvent()` returns
			* NULL while disabled, so pointer and mouse events are effectively ignored.
			*/
			inline bool isEnable()
			{
				return m_enable;
			}
		};
	}
}