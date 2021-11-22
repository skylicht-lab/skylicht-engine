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
			m_rightPress(false),
			m_downloadMapThread(NULL),
			m_lock(NULL),
			m_mapBGType(EImageMapType::GSatellite)
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

			for (int i = 0; i < NUM_HTTPREQUEST; i++)
			{
				m_imgDownloading[i] = NULL;
				m_httpStream[i] = new CHttpStream();
				m_httpRequest[i] = new CHttpRequest(m_httpStream[i]);
			}

			m_lock = IMutex::createMutex();
			m_downloadMapThread = IThread::createThread(this);
		}

		CSpaceGMap::~CSpaceGMap()
		{
			delete m_fontLarge;
			delete m_fontNormal;

			if (m_downloadMapThread != NULL)
				m_downloadMapThread->stop();

			if (m_lock != NULL)
				delete m_lock;

			for (int i = 0; i < NUM_HTTPREQUEST; i++)
			{
				m_imgDownloading[i] = NULL;
				delete m_httpStream[i];
				m_httpStream[i] = NULL;

				delete m_httpRequest[i];
				m_httpRequest[i] = NULL;
			}
		}

		void CSpaceGMap::updateThread()
		{
			m_lock->lock();

			for (int r = 0; r < NUM_HTTPREQUEST; r++)
			{
				// need create new request
				if (m_queueDownload.size() > 0 && m_imgDownloading[r] == NULL)
				{
					std::list<SImageDownload>::iterator i = m_queueDownload.begin();
					SImageDownload& img = (*i);

					SImageMapElement* pImg = &img.Image;

					m_httpRequest[r]->setURL(img.Url.c_str());
					m_httpRequest[r]->sendRequestByGet();

					// push queue to downloading
					m_downloading.push_back((*i));
					m_imgDownloading[r] = &(m_downloading.back());

					m_queueDownload.pop_front();
				}
				else
				{
					// update request download
					if (m_imgDownloading[r] && m_httpRequest[r]->updateRequest() == true)
					{
						if (m_httpRequest[r]->getResponseCode() > 0)
						{
							m_imgDownloading[r]->Image.Status = EImageMapStatus::Downloaded;

							char fileName[512] = { 0 };
							switch (m_imgDownloading[r]->Image.Type)
							{
							case EImageMapType::GSatellite:
							{
								sprintf(fileName, "gmap_%d_%d_%d.jpg",
									m_imgDownloading[r]->Image.Z,
									m_imgDownloading[r]->Image.X,
									m_imgDownloading[r]->Image.Y
								);
								break;
							}
							case EImageMapType::OSMTerrain:
								sprintf(fileName, "osm_%d_%d_%d.png",
									m_imgDownloading[r]->Image.Z,
									m_imgDownloading[r]->Image.X,
									m_imgDownloading[r]->Image.Y
								);
								break;
							default:
								break;
							}

							m_imgDownloading[r]->Image.Path = fileName;

							io::IWriteFile* file = getIrrlichtDevice()->getFileSystem()->createAndWriteFile(fileName);
							file->write(m_httpStream[r]->getData(), m_httpStream[r]->getDataSize());
							file->drop();
						}
						else
						{
							m_imgDownloading[r]->Image.Status = EImageMapStatus::Error;
						}

						std::list<SImageDownload>::iterator iDown = m_downloading.begin(), endDown = m_downloading.end();
						while (iDown != endDown)
						{
							if (iDown->Image.Type == m_imgDownloading[r]->Image.Type &&
								iDown->Image.X == m_imgDownloading[r]->Image.X &&
								iDown->Image.Y == m_imgDownloading[r]->Image.Y &&
								iDown->Image.Z == m_imgDownloading[r]->Image.Z)
							{
								// remove on downloading list
								m_downloading.erase(iDown);
								break;
							}
							++iDown;
						}

						m_imgDownloading[r] = NULL;
					}
				}
			}
			m_lock->unlock();

			if (m_queueDownload.size() == 0 && m_downloading.size() == 0)
				IThread::sleep(200);
			else
				IThread::sleep(1);
		}

		void CSpaceGMap::requestDownloadMap(long x, long y, int z)
		{
			m_lock->lock();

			SImageMapElement img;

			img.Type = m_mapBGType;
			img.X = x;
			img.Y = y;
			img.Z = z;

			std::list<SImageDownload>::iterator i = m_queueDownload.begin(), end = m_queueDownload.end();
			while (i != end)
			{
				if (i->Image.Type == img.Type &&
					i->Image.X == img.X &&
					i->Image.Y == img.Y &&
					i->Image.Z == img.Z)
				{
					m_lock->unlock();
					return;
				}
				++i;
			}

			i = m_downloading.begin(), end = m_downloading.end();
			while (i != end)
			{
				if (i->Image.Type == img.Type &&
					i->Image.X == img.X &&
					i->Image.Y == img.Y &&
					i->Image.Z == img.Z)
				{
					m_lock->unlock();
					return;
				}
				++i;
			}


			SImageDownload download;

			char lpUrl[512];

			switch (m_mapBGType)
			{
			case EImageMapType::GSatellite:
				sprintf(lpUrl,
					"https://khms0.google.com/kh/v=908?x=%ld&y=%ld&z=%d",
					img.X, img.Y, img.Z
				);
				break;
			case EImageMapType::OSMTerrain:
				sprintf(lpUrl,
					"https://api.maptiler.com/maps/outdoor/%d/%ld/%ld.png",
					img.Z, img.X, img.Y
				);
				break;
				break;
			default:
				break;
			}

			download.Image.Type = m_mapBGType;
			download.Image.X = img.X;
			download.Image.Y = img.Y;
			download.Image.Z = img.Z;
			download.Url = lpUrl;

			m_queueDownload.push_back(download);

			m_lock->unlock();
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
			if (down)
				GUI::CInput::getInput()->setCapture(m_view);
			else
				GUI::CInput::getInput()->setCapture(NULL);
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

			updateMap();
			renderMap();

			pG->endRenderGUI();

			// resume gui render
			driver->enableScissor(true);
			driver->setViewPort(vp);
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
			// clear
			CGraphics2D* pGraphics = CGraphics2D::getInstance();
			pGraphics->draw2DRectangle(core::rectf(0.0f, 0.0f, m_view->width(), m_view->height()), SColor(255, 200, 200, 200));

			// draw grid
			renderGrid();

			// draw lng, lat
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

			SColor black(0xffe1e1e1);

			// draw grid
			for (int i = 0; i < m_renderMap.CountX; i++)
			{
				if (realX == 0.0f || m_renderMap.From.X + i == m_renderMap.To.X)
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
				if (realY == 0.0f || m_renderMap.From.Y + i == m_renderMap.To.Y)
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
			SColor red(255, 255, 0, 0);

			for (long i = m_renderMap.From.Y; i < m_renderMap.From.Y + m_renderMap.CountY && i < m_renderMap.To.Y; i++)
			{
				if (i < 0)
				{
					y += m_gridSize;
					continue;
				}

				for (int j = m_renderMap.From.X; j < m_renderMap.From.X + m_renderMap.CountX && j < m_renderMap.To.X; j++)
				{
					if (j < 0)
					{
						x += m_gridSize;
						continue;
					}

					long realX = j * m_gridSize;
					long realY = i * m_gridSize;

					double lat, lng;
					getLatLngByPixel((long)realX, (long)realY, m_zoom, &lat, &lng);

					swprintf(string, 512, L"%.3lf, %.3lf", lat, lng);

					float centerX = 5.0f;
					float centerY = 5.0f;

					pGraphics->drawText(core::position2df(x + centerX, y + centerY), m_fontNormal, black, string, m_materialID);

					x += m_gridSize;
				}

				y += m_gridSize;
				x = -m_viewX % m_gridSize;
			}

#ifndef HAVE_SKYLICHT_NETWORK
			wchar_t* warningString = L"BUILD_SKYLICHT_NETWORK is Disable! Please Enable this feature on CMakeLists.txt";
			core::dimension2df size = pGraphics->measureText(m_fontNormal, warningString);
			float centerX = m_view->width() / 2 - size.Width / 2;
			float centerY = m_view->height() / 2 - size.Height / 2;
			pGraphics->drawText(core::position2df(centerX, centerY), m_fontNormal, red, warningString, m_materialID);
#endif
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