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
#include "imgui_impl_skylicht.h"

#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed (stb stuff)
#endif

bool ImGui_Impl_Skylicht_Init()
{
	ImGuiIO& io = ImGui::GetIO();

	io.BackendPlatformName = "imgui_impl_skylicht";

	io.KeyMap[ImGuiKey_Tab] = irr::KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = irr::KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = irr::KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = irr::KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = irr::KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = irr::KEY_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = irr::KEY_NEXT;
	io.KeyMap[ImGuiKey_Home] = irr::KEY_HOME;
	io.KeyMap[ImGuiKey_End] = irr::KEY_END;
	io.KeyMap[ImGuiKey_Insert] = irr::KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = irr::KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = irr::KEY_BACK;
	io.KeyMap[ImGuiKey_Space] = irr::KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = irr::KEY_RETURN;
	io.KeyMap[ImGuiKey_Escape] = irr::KEY_ESCAPE;
	io.KeyMap[ImGuiKey_KeyPadEnter] = irr::KEY_RETURN;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	unsigned char* pixels = NULL;
	int width = 0, height = 0;
	io.Fonts->AddFontDefault();
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	return true;
}

void ImGui_Impl_Skylicht_Shutdown()
{

}

void ImGui_Impl_Skylicht_NewFrame()
{
	// Setup time step
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = Skylicht::getTimeStep() / 1000.0f;

	// Start the frame
	ImGui::NewFrame();
}

void ImGui_Impl_Skylicht_ResizeFunc(int w, int h)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)w, (float)h);
}

void ImGui_Impl_Skylicht_MouseMoveFunc(int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
}

void ImGui_Impl_Skylicht_MouseButtonFunc(int button, int state, int x, int y)
{

}

void ImGui_Impl_Skylicht_MouseWheelFunc(int button, int dir, int x, int y)
{

}

void ImGui_Impl_Skylicht_CharPressedFunc(unsigned char c, int x, int y)
{

}

void ImGui_Impl_Skylicht_CharReleasedFunc(unsigned char c, int x, int y)
{

}

void ImGui_Impl_Skylicht_KeyPressedFunc(int key, int x, int y)
{

}

void ImGui_Impl_Skylicht_KeyReleasedFunc(int key, int x, int y)
{

}