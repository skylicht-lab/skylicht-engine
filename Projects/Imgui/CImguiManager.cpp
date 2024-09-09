/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "CImguiManager.h"
#include "Graphics2D/CGraphics2D.h"
#include "EventManager/CEventManager.h"
#include "Control/CTouchManager.h"
#include "imgui_impl_skylicht.h"

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CImguiManager);

	CImguiManager::CImguiManager()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();

#if defined(IOS) || defined(ANDROID)
		// scale in the big screen size
		style.ScaleAllSizes(2.0f);
#endif

		ImGui_Impl_Skylicht_Init();

		CEventManager::getInstance()->registerProcessorEvent("ImguiManager", this);
	}

	CImguiManager::~CImguiManager()
	{
		ImGui_Impl_Skylicht_Shutdown();

		ImGui::DestroyContext();

		CEventManager::getInstance()->unRegisterProcessorEvent(this);
	}

	void CImguiManager::onNewFrame()
	{
		core::dimension2du vp = CGraphics2D::getInstance()->getScreenSize();
		if (m_viewport != vp)
		{
			m_viewport = vp;
			ImGui_Impl_Skylicht_ResizeFunc(vp.Width, vp.Height);
		}

		ImGui_Impl_Skylicht_NewFrame();
	}

	void CImguiManager::onRender()
	{
		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO();

		ImGui_Impl_Skylicht_RenderDrawData(ImGui::GetDrawData());
	}

	bool CImguiManager::OnProcessEvent(const SEvent& event)
	{
		bool skipAllEvent = false;

		if (event.EventType == EET_MOUSE_INPUT_EVENT)
		{
			ImGuiIO& io = ImGui::GetIO();

			int x = event.MouseInput.X;
			int y = event.MouseInput.Y;

			if (event.MouseInput.Event == EMIE_MOUSE_MOVED)
				ImGui_Impl_Skylicht_MouseMoveFunc(x, y);
			else if (event.MouseInput.Event == EMIE_MOUSE_WHEEL)
				ImGui_Impl_Skylicht_MouseWheelFunc((int)-event.MouseInput.Wheel, x, y);
			else
			{
				int button[EMIE_COUNT] = { 0 };
				int state[EMIE_COUNT] = { 0 };

				button[EMIE_LMOUSE_PRESSED_DOWN] = 0;
				state[EMIE_LMOUSE_PRESSED_DOWN] = 1;

				button[EMIE_LMOUSE_LEFT_UP] = 0;
				state[EMIE_LMOUSE_LEFT_UP] = 0;

				button[EMIE_MMOUSE_PRESSED_DOWN] = 2;
				state[EMIE_MMOUSE_PRESSED_DOWN] = 1;

				button[EMIE_MMOUSE_LEFT_UP] = 2;
				state[EMIE_MMOUSE_LEFT_UP] = 0;

				button[EMIE_RMOUSE_PRESSED_DOWN] = 1;
				state[EMIE_RMOUSE_PRESSED_DOWN] = 1;

				button[EMIE_RMOUSE_LEFT_UP] = 1;
				state[EMIE_RMOUSE_LEFT_UP] = 0;

				int eventID = (int)event.MouseInput.Event;
				ImGui_Impl_Skylicht_MouseButtonFunc(button[eventID], state[eventID], x, y);
			}

			if (io.WantCaptureMouse == true)
			{
				skipAllEvent = true;
				CTouchManager::getInstance()->setTouchIdentify(event.MouseInput.ID, CTouchIdentify::TouchOnUI);
			}
		}
		else if (event.EventType == irr::EET_KEY_INPUT_EVENT)
		{
			ImGuiIO& io = ImGui::GetIO();

			unsigned int keyChar = (unsigned int)event.KeyInput.Char;
			bool control = event.KeyInput.Control;
			bool shift = event.KeyInput.Shift;

			if (keyChar != 0)
				ImGui_Impl_Skylicht_CharFunc(keyChar);

			if (event.KeyInput.PressedDown == true)
				ImGui_Impl_Skylicht_KeyPressedFunc((int)event.KeyInput.Key, control, shift, false);
			else
				ImGui_Impl_Skylicht_KeyReleasedFunc((int)event.KeyInput.Key, control, shift, false);

			if (io.WantCaptureKeyboard == true)
				skipAllEvent = true;
		}

		return !skipAllEvent;
	}
}