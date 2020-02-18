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

#pragma once

#include "imgui.h"

IMGUI_IMPL_API bool     ImGui_Impl_Skylicht_Init();
IMGUI_IMPL_API void     ImGui_Impl_Skylicht_Shutdown();
IMGUI_IMPL_API void     ImGui_Impl_Skylicht_NewFrame();
IMGUI_IMPL_API void     ImGui_Impl_Skylicht_SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height);
IMGUI_IMPL_API void     ImGui_Impl_Skylicht_RenderDrawData(ImDrawData* draw_data);

IMGUI_IMPL_API void     ImGui_Impl_Skylicht_ResizeFunc(int w, int h);
IMGUI_IMPL_API void     ImGui_Impl_Skylicht_MouseMoveFunc(int x, int y);
IMGUI_IMPL_API void     ImGui_Impl_Skylicht_MouseButtonFunc(int button, int state, int x, int y);
IMGUI_IMPL_API void     ImGui_Impl_Skylicht_MouseWheelFunc(int button, int dir, int x, int y);
IMGUI_IMPL_API void     ImGui_Impl_Skylicht_CharPressedFunc(unsigned char c, int x, int y);
IMGUI_IMPL_API void     ImGui_Impl_Skylicht_CharReleasedFunc(unsigned char c, int x, int y);
IMGUI_IMPL_API void     ImGui_Impl_Skylicht_KeyPressedFunc(int key, int x, int y);
IMGUI_IMPL_API void     ImGui_Impl_Skylicht_KeyReleasedFunc(int key, int x, int y);