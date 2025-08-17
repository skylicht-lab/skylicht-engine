/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CSpaceTextureViewer.h"
#include "GUI/Theme/ThemeConfig.h"
#include "GUI/Input/CInput.h"
#include "AssetManager/CAssetManager.h"
#include "Editor/CEditor.h"
#include "TextureManager/CTextureManager.h"

using namespace std::placeholders;

namespace Skylicht
{
	namespace Editor
	{
		CSpaceTextureViewer::CSpaceTextureViewer(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_zoom(1),
			m_scale(1.0f),
			m_viewX(0),
			m_viewY(0),
			m_leftPress(false),
			m_texture(NULL),
			m_debugColor(0),
			m_debugTransparent(0)
		{
			GUI::CToolbar* toolbar = new GUI::CToolbar(window);

			GUI::CLabel* label = new GUI::CLabel(toolbar);
			label->setPadding(GUI::SPadding(10.0f, 2.0f, 0.0f, 0.0f));
			label->setString(L"Filter: ");
			label->sizeToContents();
			toolbar->addControl(label);

			GUI::CComboBox* cmb = new GUI::CComboBox(toolbar);
			cmb->addItem(L"RGBA");
			cmb->addItem(L"Red");
			cmb->addItem(L"Green");
			cmb->addItem(L"Blue");
			cmb->addItem(L"Alpha");
			cmb->setSelectIndex(0, false);
			cmb->OnChanged = BIND_LISTENER(&CSpaceTextureViewer::onDebugColor, this);
			toolbar->addControl(cmb);
			toolbar->addSpace();

			cmb = new GUI::CComboBox(toolbar);
			cmb->addItem(L"Transparent");
			cmb->addItem(L"Soild");
			cmb->setSelectIndex(0, false);
			cmb->setWidth(100.0f);
			cmb->OnChanged = BIND_LISTENER(&CSpaceTextureViewer::onDebugTransparent, this);
			toolbar->addControl(cmb);

			label = new GUI::CLabel(toolbar);
			label->setPadding(GUI::SPadding(10.0f, 2.0f, 0.0f, 0.0f));
			label->setString(L"Scale: 1.00");
			label->sizeToContents();
			toolbar->addControl(label, true);
			m_labelZoom = label;

			m_view = new GUI::CBase(window);
			m_view->dock(GUI::EPosition::Fill);

			m_view->OnRender = BIND_LISTENER(&CSpaceTextureViewer::onRender, this);
			m_view->OnMouseMoved = std::bind(&CSpaceTextureViewer::onMouseMoved, this, _1, _2, _3, _4, _5);
			m_view->OnLeftMouseClick = std::bind(&CSpaceTextureViewer::onLeftMouseClick, this, _1, _2, _3, _4);
			m_view->OnMouseWheeled = std::bind(&CSpaceTextureViewer::onMouseWheeled, this, _1, _2);

			CShaderManager* shaderMgr = CShaderManager::getInstance();

			m_vertexColor = shaderMgr->getShaderIDByName("VertexColorAlpha");
			m_textureColor = shaderMgr->getShaderIDByName("TextureColorAlpha");

			CShader* shader = shaderMgr->loadShader("BuiltIn/Shader/Basic/TextureColorDebug.xml");
			if (shader)
				m_textureColor = shader->getMaterialRenderID();
		}

		CSpaceTextureViewer::~CSpaceTextureViewer()
		{

		}

		void CSpaceTextureViewer::onDebugColor(GUI::CBase* base)
		{
			GUI::CComboBox* cmb = (GUI::CComboBox*)base;
			m_debugColor = cmb->getSelectIndex();
		}

		void CSpaceTextureViewer::onDebugTransparent(GUI::CBase* base)
		{
			GUI::CComboBox* cmb = (GUI::CComboBox*)base;
			m_debugTransparent = cmb->getSelectIndex();
		}

		void CSpaceTextureViewer::onResize(float w, float h)
		{
			updateView();
		}

		void CSpaceTextureViewer::onMouseMoved(GUI::CBase* base, float x, float y, float deltaX, float deltaY)
		{
			m_mouseLocal = m_view->canvasPosToLocal(GUI::SPoint(x, y));

			if (m_leftPress)
			{
				if (m_texture)
				{
					m_viewX = m_viewX - (float)deltaX;
					m_viewY = m_viewY - (float)deltaY;

					updateView();
				}
			}
		}

		void CSpaceTextureViewer::updateView()
		{
			if (m_texture)
			{
				core::dimension2du s = m_texture->getSize();
				const GUI::SRect& bound = m_view->getBounds();

				s.Width = (u32)(s.Width * m_scale);
				s.Height = (u32)(s.Height * m_scale);

				float maxX = (float)s.Width - (float)bound.Width;
				float maxY = (float)s.Height - (float)bound.Height;
				maxX = core::max_(maxX, 0.0f);
				maxY = core::max_(maxY, 0.0f);

				m_viewX = core::clamp(m_viewX, 0.0f, maxX);
				m_viewY = core::clamp(m_viewY, 0.0f, maxY);

				m_world.setTranslation(core::vector3df(-m_viewX, -m_viewY, 0.0f));
				m_world.setScale(core::vector3df(m_scale, m_scale, m_scale));
			}
		}

		void CSpaceTextureViewer::onLeftMouseClick(GUI::CBase* base, float x, float y, bool down)
		{
			m_leftPress = down;

			if (down)
				GUI::CInput::getInput()->setCapture(m_view);
			else
				GUI::CInput::getInput()->setCapture(NULL);
		}

		void CSpaceTextureViewer::onMouseWheeled(GUI::CBase* base, int wheel)
		{
			GUI::SPoint mousePos = GUI::CInput::getInput()->getMousePosition();
			GUI::SPoint localPos = m_view->canvasPosToLocal(mousePos);

			float dx = localPos.X * m_scale - m_viewX;
			float dy = localPos.Y * m_scale - m_viewY;

			if (wheel < 0)
			{
				if (m_zoom > 0)
				{
					m_zoom--;
					m_scale = m_scale * 2.0f;
					m_viewX = (m_viewX + dx) * 2.0f - dx;
					m_viewY = (m_viewY + dy) * 2.0f - dy;
				}
			}
			else
			{
				if (m_zoom < 4)
				{
					m_zoom++;
					m_scale = m_scale / 2.0f;
					m_viewX = (m_viewX + dx) / 2.0f - dx;
					m_viewY = (m_viewY + dy) / 2.0f - dy;
				}
			}

			updateView();

			char text[128];
			sprintf(text, "Scale: %.02f", m_scale);
			m_labelZoom->setString(text);
			m_labelZoom->sizeToContents();
		}

		void CSpaceTextureViewer::onRender(GUI::CBase* base)
		{
			// flush 2d gui
			GUI::Context::getRenderer()->flush();

			IVideoDriver* driver = getVideoDriver();

			core::recti vp = getVideoDriver()->getViewPort();
			driver->enableScissor(false);
			driver->clearZBuffer();

			// setup space window viewport
			GUI::SPoint position = base->localPosToCanvas();
			core::recti viewport;
			viewport.UpperLeftCorner.set((int)position.X, (int)position.Y);
			viewport.LowerRightCorner.set((int)(position.X + base->width()), (int)(position.Y + base->height()));
			driver->setViewPort(viewport);

			// setup 2d projection
			core::matrix4 projection, view;
			projection.buildProjectionMatrixOrthoLH((f32)viewport.getWidth(), -(f32)viewport.getHeight(), -1.0f, 1.0f);
			view.setTranslation(core::vector3df(-(f32)viewport.getWidth() * 0.5f, -(f32)viewport.getHeight() * 0.5f, 0.0f));

			// render map
			CGraphics2D* pG = CGraphics2D::getInstance();
			pG->beginRenderGUI(projection, view);

			if (m_texture)
			{
				const core::dimension2du& s = m_texture->getSize();
				core::rectf r(0.0f, 0.0f, (float)s.Width, (float)s.Height);
				core::rectf uv(0.0f, 0.0f, 1.0f, 1.0f);

				pG->addRectangleBatch(r, uv, SColor(255, 70, 70, 70), m_world, m_vertexColor);

				// see: TextureColorDebug.xml
				CShaderManager::getInstance()->ShaderVec2[0].set((float)m_debugColor, (float)m_debugTransparent);

				pG->addImageBatch(m_texture, SColor(255, 255, 255, 255), m_world, m_textureColor);
			}

			pG->endRenderGUI();

			// resume gui render
			driver->enableScissor(true);
			driver->setViewPort(vp);
			GUI::Context::getRenderer()->setProjection();
		}

		void CSpaceTextureViewer::openTexture(const char* path)
		{
			m_texture = CTextureManager::getInstance()->getTextureFromRealPath(path);
			m_world.makeIdentity();
			m_zoom = 1;
			m_scale = 1.0f;
			m_viewX = 0;
			m_viewY = 0;
		}
	}
}