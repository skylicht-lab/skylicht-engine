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
#include "Material/Shader/CShaderManager.h"

#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed (stb stuff)
#endif

using namespace Skylicht;
using namespace irr;

ITexture *g_fontTexture = NULL;
int g_vertexColorShader = -1;
int g_textureColorShader = -1;

IMeshBuffer *g_meshBuffer = NULL;

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

	IVideoDriver *driver = getVideoDriver();
	core::dimension2du size((u32)width, (u32)height);
	IImage *img = driver->createImageFromData(video::ECF_A8R8G8B8, size, pixels, true, false);
	g_fontTexture = driver->addTexture("imgui_font", img);
	io.Fonts->SetTexID((ImTextureID*)g_fontTexture);
	img->drop();

	g_meshBuffer = new CMeshBuffer<S3DVertex>(driver->getVertexDescriptor(EVT_STANDARD), video::EIT_32BIT);
	g_meshBuffer->setHardwareMappingHint(EHM_STREAM);
	return true;
}

void ImGui_Impl_Skylicht_Shutdown()
{
	if (g_fontTexture != NULL)
	{
		getVideoDriver()->removeTexture(g_fontTexture);
		g_fontTexture = NULL;
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->SetTexID(NULL);
	}

	if (g_meshBuffer != NULL)
	{
		g_meshBuffer->drop();
		g_meshBuffer = NULL;
	}
}

void ImGui_Impl_Skylicht_NewFrame()
{
	// Setup time step
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = Skylicht::getTimeStep() / 1000.0f;

	// Start the frame
	ImGui::NewFrame();
}

void ImGui_Impl_Skylicht_SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height)
{
	if (g_textureColorShader == -1)
	{
		CShaderManager *shaderManager = CShaderManager::getInstance();
		g_vertexColorShader = shaderManager->getShaderIDByName("VertexColorAlpha");
		g_textureColorShader = shaderManager->getShaderIDByName("TextureColorAlpha");
	}

	// set viewport
	IVideoDriver *driver = getVideoDriver();
	driver->setViewPort(core::recti(0, 0, fb_width, fb_height));

	core::matrix4 orthoMatrix;
	orthoMatrix.buildProjectionMatrixOrthoLH(
		draw_data->DisplaySize.x,
		-draw_data->DisplaySize.y,
		-1.0f,
		1.0f);
	orthoMatrix.setTranslation(core::vector3df(-1, 1, 0));

	driver->setTransform(video::ETS_PROJECTION, orthoMatrix);
	driver->setTransform(video::ETS_VIEW, core::IdentityMatrix);
	driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);
}

void ImGui_Impl_Skylicht_RenderDrawData(ImDrawData* draw_data)
{
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	if (fb_width == 0 || fb_height == 0)
		return;

	IVideoDriver *driver = getVideoDriver();

	ImGui_Impl_Skylicht_SetupRenderState(draw_data, fb_width, fb_height);

	// Will project scissor/clipping rectangles into framebuffer space
	ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
	ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

	// Render command lists
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
		const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

		SVertexBuffer *vtx = (SVertexBuffer*)g_meshBuffer->getVertexBuffer(0);
		int vtxSize = cmd_list->VtxBuffer.Size;
		vtx->set_used(vtxSize);

		for (int i = 0, n = vtxSize; i < n; i++)
		{
			S3DVertex &v = vtx->getVertex(i);
			const ImDrawVert* imguiVertex = vtx_buffer + i;

			v.Pos.X = imguiVertex->pos.x;
			v.Pos.Y = imguiVertex->pos.y;
			v.Pos.Z = 0;
			v.TCoords.X = imguiVertex->uv.x;
			v.TCoords.Y = imguiVertex->uv.y;
			v.Color.set(imguiVertex->col);
		}

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				// User callback, registered via ImDrawList::AddCallback()
				// (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
				if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
					ImGui_Impl_Skylicht_SetupRenderState(draw_data, fb_width, fb_height);
				else
					pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				// Project scissor/clipping rectangles into framebuffer space
				ImVec4 clip_rect;
				clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
				clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
				clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
				clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

				if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
				{
					// Apply scissor/clipping rectangle
					// glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));

					int n = pcmd->ElemCount;
					IIndexBuffer *idxBuffer = g_meshBuffer->getIndexBuffer();
					idxBuffer->set_used(n);

					u32* idx = (u32*)idxBuffer->getIndices();
					for (int i = 0; i < n; i++)
						idx[i] = (u32)idx_buffer[i];

					g_meshBuffer->setPrimitiveType(scene::EPT_TRIANGLES);
					g_meshBuffer->setDirty();

					SMaterial material;
					material.ZBuffer = video::ECFN_ALWAYS;
					material.ZWriteEnable = false;
					material.BackfaceCulling = false;
					material.FrontfaceCulling = false;
					material.UseMipMaps = false;

					if (pcmd->TextureId == NULL)
						material.MaterialType = g_vertexColorShader;
					else
					{
						material.MaterialType = g_textureColorShader;
						material.setTexture(0, (ITexture*)pcmd->TextureId);
					}

					driver->setMaterial(material);
					driver->drawMeshBuffer(g_meshBuffer);
				}
			}
			idx_buffer += pcmd->ElemCount;
		}
	}
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
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
	if (state == 1)
		io.MouseDown[button] = true;
	else
		io.MouseDown[button] = false;
}

void ImGui_Impl_Skylicht_MouseWheelFunc(int dir, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
	io.MouseWheel += dir;
}

void ImGui_Impl_Skylicht_CharFunc(unsigned int c)
{
	ImGuiIO& io = ImGui::GetIO();
	if (c >= 32)
		io.AddInputCharacter(c);
}

void ImGui_Impl_Skylicht_KeyPressedFunc(int key, bool ctrl, bool shift, bool alt)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = true;
	io.KeyCtrl = ctrl;
	io.KeyShift = shift;
	io.KeyAlt = alt;
}

void ImGui_Impl_Skylicht_KeyReleasedFunc(int key, bool ctrl, bool shift, bool alt)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = false;
	io.KeyCtrl = ctrl;
	io.KeyShift = shift;
	io.KeyAlt = alt;
}