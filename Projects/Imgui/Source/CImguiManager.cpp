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
#include "imgui_impl_skylicht.h"

namespace Skylicht
{
	CImguiManager::CImguiManager()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGui_Impl_Skylicht_Init();
	}

	CImguiManager::~CImguiManager()
	{
		ImGui_Impl_Skylicht_Shutdown();

		ImGui::DestroyContext();
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

		// Dear ImGui Apps (accessible from the "Tools" menu)
		static bool open = true;
		if (!ImGui::Begin("About Dear ImGui", &open, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::End();
			return;
		}
		ImGui::Text("Dear ImGui %s", ImGui::GetVersion());
		ImGui::Separator();
		ImGui::Text("By Omar Cornut and all Dear ImGui contributors.");
		ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
		ImGui::End();
	}

	void CImguiManager::onRender()
	{
		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO();

		ImGui_Impl_Skylicht_RenderDrawData(ImGui::GetDrawData());
	}
}