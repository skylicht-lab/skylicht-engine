/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "CSpaceGMap.h"
#include "GMapUtils.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Input/CInput.h"

using namespace std::placeholders;

namespace Skylicht
{
	namespace Editor
	{
		CSpaceGMap::CSpaceGMap(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_zoom(4),
			m_viewX(0),
			m_viewY(0),
			m_gridSize(256),
			m_rightPress(false)
		{
			m_view = new GUI::CBase(window);
			m_view->dock(GUI::EPosition::Fill);

			m_view->OnRender = BIND_LISTENER(&CSpaceGMap::onRender, this);
			m_view->OnMouseMoved = std::bind(&CSpaceGMap::onMouseMoved, this, _1, _2, _3, _4, _5);
			m_view->OnLeftMouseClick = std::bind(&CSpaceGMap::onLeftMouseClick, this, _1, _2, _3, _4);
			m_view->OnRightMouseClick = std::bind(&CSpaceGMap::onRightMouseClick, this, _1, _2, _3, _4);
			m_view->OnMiddleMouseClick = std::bind(&CSpaceGMap::onMiddleMouseClick, this, _1, _2, _3, _4);
			m_view->OnMouseWheeled = std::bind(&CSpaceGMap::onMouseWheeled, this, _1, _2);

			m_materialID = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");

			m_fontLarge = new CGlyphFont(GUI::CThemeConfig::FontName.c_str(), GUI::CThemeConfig::getFontSizePt(GUI::EFontSize::SizeLarge));
			m_fontNormal = new CGlyphFont(GUI::CThemeConfig::FontName.c_str(), GUI::CThemeConfig::getFontSizePt(GUI::EFontSize::SizeNormal));
		}

		CSpaceGMap::~CSpaceGMap()
		{
			delete m_fontLarge;
			delete m_fontNormal;
		}

		void CSpaceGMap::onResize(float w, float h)
		{

		}

		void CSpaceGMap::onMouseMoved(GUI::CBase* base, float x, float y, float deltaX, float deltaY)
		{
			if (m_rightPress)
			{
				m_viewX = m_viewX - (long)deltaX;
				m_viewY = m_viewY - (long)deltaY;
			}
		}

		void CSpaceGMap::onLeftMouseClick(GUI::CBase* base, float x, float y, bool down)
		{

		}

		void CSpaceGMap::onRightMouseClick(GUI::CBase* base, float x, float y, bool down)
		{
			m_rightPress = down;
		}

		void CSpaceGMap::onMiddleMouseClick(GUI::CBase* base, float x, float y, bool down)
		{

		}

		void CSpaceGMap::onMouseWheeled(GUI::CBase* base, int wheel)
		{
			GUI::SPoint mousePos = GUI::CInput::getInput()->getMousePosition();
			GUI::SPoint localPos = m_view->canvasPosToLocal(mousePos);

			if (wheel < 0)
			{
				zoomIn((long)localPos.X, (long)localPos.Y);
			}
			else
			{
				zoomOut((long)localPos.X, (long)localPos.Y);
			}
		}

		void CSpaceGMap::onRender(GUI::CBase* base)
		{
			// flush 2d gui
			GUI::CGUIContext::getRenderer()->flush();
			core::recti vp = getVideoDriver()->getViewPort();
			getVideoDriver()->enableScissor(false);
			getVideoDriver()->clearZBuffer();

			// setup space window viewport
			GUI::SPoint position = base->localPosToCanvas();
			core::recti viewport;
			viewport.UpperLeftCorner.set((int)position.X, (int)position.Y);
			viewport.LowerRightCorner.set((int)(position.X + base->width()), (int)(position.Y + base->height()));
			getVideoDriver()->setViewPort(viewport);

			// setup 2d projection
			core::matrix4 projection, view;
			projection.buildProjectionMatrixOrthoLH((f32)viewport.getWidth(), -(f32)viewport.getHeight(), -1.0f, 1.0f);
			view.setTranslation(core::vector3df(-(f32)viewport.getWidth() * 0.5f, -(f32)viewport.getHeight() * 0.5f, 0.0f));

			// render map
			CGraphics2D* pG = CGraphics2D::getInstance();
			pG->beginRenderGUI(projection, view);

			updateMap();
			renderMap();

			pG->endRenderGUI();

			// resume gui render
			getVideoDriver()->enableScissor(true);
			getVideoDriver()->setViewPort(vp);
			GUI::CGUIContext::getRenderer()->setProjection();
		}

		void CSpaceGMap::updateMap()
		{
			long maxElementX = 4;
			long maxElementY = 4;
			int minZoom = 2;

			for (int z = m_zoom; z > minZoom; z--)
			{
				maxElementX *= 2;
				maxElementY *= 2;
			}

			int width = (int)m_view->width();
			int height = (int)m_view->height();

			m_renderMap.CountX = width / m_gridSize + 2;
			m_renderMap.CountY = height / m_gridSize + 2;

			m_renderMap.From.X = core::min_(m_viewX / m_gridSize, maxElementX);
			m_renderMap.From.Y = core::min_(m_viewY / m_gridSize, maxElementY);
			m_renderMap.To.X = core::min_(m_renderMap.From.X + m_renderMap.CountX, maxElementX);
			m_renderMap.To.Y = core::min_(m_renderMap.To.Y + m_renderMap.CountY, maxElementY);
		}

		void CSpaceGMap::renderMap()
		{
			renderGrid();
			renderString();
		}

		void CSpaceGMap::renderGrid()
		{
			CGraphics2D* pGraphics = CGraphics2D::getInstance();

			// local tile xy
			float x = (float)(-m_viewX % m_gridSize);
			float y = (float)(-m_viewY % m_gridSize);

			// world tile xy
			float realX = m_viewX + x;
			float realY = m_viewY + y;

			float w = (float)m_view->width();
			float h = (float)m_view->height();

			SColor black(255, 10, 10, 10);

			// draw grid
			for (int i = 0; i < m_renderMap.CountX; i++)
			{
				if (realX == 0.0f)
				{
					core::rectf r(x - 2.0f, 0.0f, x + 2.0f, h);
					pGraphics->draw2DRectangle(r, black);
				}
				else
				{
					pGraphics->draw2DLine(core::position2df(x, 0.0f), core::position2df(x, h), black);
				}

				x = x + (float)m_gridSize;
				realX = realX + (float)m_gridSize;
			}

			for (int i = 0; i < m_renderMap.CountY; i++)
			{
				if (realY == 0.0f)
				{
					core::rectf r(0.0f, y - 2.0f, w, y + 2.0f);
					pGraphics->draw2DRectangle(r, black);
				}
				else
				{
					pGraphics->draw2DLine(core::position2df(0.0f, y), core::position2df(w, y), black);
				}

				y = y + (float)m_gridSize;
				realY = realY + (float)m_gridSize;
			}
		}

		void CSpaceGMap::renderString()
		{
			CGraphics2D* pGraphics = CGraphics2D::getInstance();

			long x = -m_viewX % m_gridSize;
			long y = -m_viewY % m_gridSize;

			wchar_t string[512];
			SColor black(255, 10, 10, 10);

			for (long i = m_renderMap.From.Y; i < m_renderMap.From.Y + m_renderMap.CountY; i++)
			{
				if (i < 0)
				{
					y += m_gridSize;
					continue;
				}

				for (int j = m_renderMap.From.X; j < m_renderMap.From.X + m_renderMap.CountX; j++)
				{
					if (j < 0)
					{
						x += m_gridSize;
						continue;
					}

					long realX = j * m_gridSize + m_gridSize / 2;
					long realY = i * m_gridSize + m_gridSize / 2;

					double lat, lng;
					getLatLngByPixel((long)realX, (long)realY, m_zoom, &lat, &lng);

					swprintf(string, 512, L"%.3lf, %.3lf", lat, lng);

					core::dimension2df size = pGraphics->measureText(m_fontLarge, string);
					float centerX = m_gridSize * 0.5f - size.Width * 0.5f;
					float centerY = m_gridSize * 0.5f - size.Height * 0.5f;

					pGraphics->drawText(core::position2df(x + centerX, y + centerY), m_fontLarge, black, string, m_materialID);

					x += m_gridSize;
				}

				y += m_gridSize;
				x = -m_viewX % m_gridSize;
			}
		}

		void CSpaceGMap::setZoom(int z)
		{
			if (z >= 4 && z <= 22)
			{
				// clearDownloadOverlayList();

				int ratio = z - m_zoom;
				bool zoomIn = true;

				if (ratio < 0)
				{
					ratio = -ratio;
					zoomIn = false;
				}

				while (ratio)
				{
					if (zoomIn)
					{
						m_viewX *= 2;
						m_viewY *= 2;
					}
					else
					{
						m_viewX /= 2;
						m_viewY /= 2;
					}
					ratio--;
				}

				m_zoom = z;

			}
		}

		void CSpaceGMap::zoomIn()
		{
			setZoom(m_zoom + 1);
		}

		void CSpaceGMap::zoomIn(long viewX, long viewY)
		{
			int z = m_zoom + 1;

			if (z >= 2 && z <= 22)
			{
				// clearDownloadOverlayList();

				m_viewX += viewX;
				m_viewY += viewY;

				m_viewX *= 2;
				m_viewY *= 2;

				m_viewX -= viewX;
				m_viewY -= viewY;

				m_zoom = z;
			}

		}

		void CSpaceGMap::zoomOut()
		{
			setZoom(m_zoom - 1);
		}

		void CSpaceGMap::zoomOut(long viewX, long viewY)
		{
			int z = m_zoom - 1;

			if (z >= 2 && z <= 22)
			{
				// clearDownloadOverlayList();

				m_viewX += viewX;
				m_viewY += viewY;

				m_viewX /= 2;
				m_viewY /= 2;

				m_viewX -= viewX;
				m_viewY -= viewY;

				m_zoom = z;
			}
		}
	}
}