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

#include "EventManager/CEventManager.h"

namespace Skylicht
{
	namespace UI
	{
		class CUIBase;

		class CUIContainer;

		/**
		* @class CUIEventManager
		* @brief Central UI event router and input processor.
		*
		* @ingroup UI
		* 
		* CUIEventManager implements IEventProcessor and acts as the global bridge
		* between low-level engine input events (SEvent) and the UI system
		* (CUIContainer / CUIBase). It manages a list of active UI containers,
		* routes pointer/keyboard events to the correct container or UI element,
		* and maintains focus/capture state for higher-level UI interactions.
		*
		* Responsibilities:
		* - Register/unregister UI containers that should receive events.
		* - Maintain capture and focus semantics for UI elements.
		* - Track the current pointer position and pointer id for multi-pointer systems.
		* - Convert and forward SEvent instances to the appropriate container(s).
		*
		* Threading: Event processing is expected to be invoked from the main/game thread.
		*
		* Lifetime: CUIEventManager is declared as a singleton via DECLARE_SINGLETON.
		*/
		class CUIEventManager : IEventProcessor
		{
		public:
			DECLARE_SINGLETON(CUIEventManager)

		protected:
			std::vector<CUIContainer*> m_containers;

			CUIBase* m_capture;
			CUIBase* m_focus;

			int m_pointerId;
			int m_pointerX;
			int m_pointerY;

		public:
			/** @brief Constructor. Initializes internal state. */
			CUIEventManager();

			/** @brief Destructor. Cleans up any state if necessary. */
			virtual ~CUIEventManager();

			/**
			* @brief Implementation of IEventProcessor::OnProcessEvent.
			*
			* Receives an SEvent from the engine and routes it to registered
			* CUIContainer instances. Responsible for updating captured/focused
			* elements and for translating low-level input into UI pointer/key events.
			*
			* @param event The input event provided by the engine.
			* @return true if the event was handled by the UI and should not be
			*         further processed; false to allow propagation.
			*/
			virtual bool OnProcessEvent(const SEvent& event);

			/**
			* @brief Register a UI container to receive routed events.
			*
			* The container will be added to the internal list and will participate
			* when routing input events. Duplicate registration is ignored.
			*
			* @param container Container to register.
			*/
			void registerUIContainer(CUIContainer* container);

			/**
			* @brief Unregister a UI container so it no longer receives events.
			*
			* Removes the container from internal routing lists. If the container
			* is not registered this is a no-op.
			*
			* @param container Container to unregister.
			*/
			void unregisterUIContainer(CUIContainer* container);

			/**
			* @brief Set the element that currently captures pointer input.
			*
			* When a CUIBase has capture it will receive pointer move/up events
			* even if the pointer moves outside the original hit area.
			*
			* @param base UI element to capture pointer input, or nullptr to release capture.
			*/
			inline void setCapture(CUIBase* base)
			{
				m_capture = base;
			}

			/**
			* @brief Set keyboard/focus to a given UI element.
			*
			* Focused element receives keyboard events and focus callbacks.
			*
			* @param focus Element to receive focus (nullptr to clear focus).
			*/
			void setFocus(CUIBase* focus);

			/** @brief Get the element that currently has keyboard/focus. */
			inline CUIBase* getFocus()
			{
				return m_focus;
			}

			/** @brief Get the last known pointer world X coordinate. */
			inline int getPointerX()
			{
				return m_pointerX;
			}

			/** @brief Get the last known pointer world Y coordinate. */
			inline int getPointerY()
			{
				return m_pointerY;
			}
		};
	}
}