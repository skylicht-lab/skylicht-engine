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
#include "CSpaceInterpolateCurves.h"

#include "GUI/Input/CInput.h"

using namespace std::placeholders;

namespace Skylicht
{
	namespace Editor
	{
		CSpaceInterpolateCurves::CSpaceInterpolateCurves(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_owner(NULL),
			m_view(NULL),
			m_guiScale(512.0f, 256.0f),
			m_gridSize(256.0f, 256.0f),
			m_viewX(0.0f),
			m_viewY(0.0f),
			m_oldViewX(0.0f),
			m_oldViewY(0.0f),
			m_pressX(0.0f),
			m_pressY(0.0f),
			m_middleDrag(false),
			m_mouseX(0.0f),
			m_mouseY(0.0f),
			m_mouseGUIX(0.0f),
			m_mouseGUIY(0.0f),
			m_leftMouseDown(false),
			m_rightMouseDown(false),
			m_hoverPoint(-1),
			m_hoverState(0),
			m_hoverLine(-1),
			m_insertId(-1)
		{
			m_controller = new CInterpolateCurvesController(this);

			GUI::CButton* btn = NULL;
			m_toolBar = new GUI::CToolbar(window);
			btn = m_toolBar->addButton(L"Clear", GUI::ESystemIcon::CurveClear);
			btn->OnPress = BIND_LISTENER(&CSpaceInterpolateCurves::onClear, this);

			btn = m_toolBar->addButton(L"Line", GUI::ESystemIcon::CurveLine);
			btn->OnPress = BIND_LISTENER(&CSpaceInterpolateCurves::onLinear, this);

			btn = m_toolBar->addButton(L"InOutCubic", GUI::ESystemIcon::CurveInOutCubic);
			btn->OnPress = BIND_LISTENER(&CSpaceInterpolateCurves::onInOutCubic, this);

			m_toolBar->addSpace();

			btn = m_toolBar->addButton(L"Auto zoom", GUI::ESystemIcon::CurveAutoZoom);
			btn->OnPress = BIND_LISTENER(&CSpaceInterpolateCurves::onAutoZoom, this);

			m_textMousePos = new GUI::CLabel(m_toolBar);
			m_toolBar->addControl(m_textMousePos, true);
			m_textMousePos->setPadding(GUI::SPadding(0.0f, 3.0f, 0.0f, 0.0f));
			m_textMousePos->setString(L"(0.0, 0.0)");
			m_textMousePos->setTextAlignment(GUI::TextCenter);
			m_textMousePos->setWidth(90.0f);

			m_view = new GUI::CBase(window);
			m_view->dock(GUI::EPosition::Fill);
			m_view->enableRenderFillRect(true);
			m_view->setFillRectColor(GUI::ThemeConfig::WindowBackgroundColor);
			m_view->setKeyboardInputEnabled(true);

			m_view->OnMouseMoved = std::bind(&CSpaceInterpolateCurves::onMouseMoved, this, _1, _2, _3, _4, _5);
			m_view->OnMiddleMouseClick = std::bind(&CSpaceInterpolateCurves::onMiddleMouseClick, this, _1, _2, _3, _4);
			m_view->OnMouseWheeled = std::bind(&CSpaceInterpolateCurves::onMouseWheel, this, _1, _2);
			m_view->OnLeftMouseClick = std::bind(&CSpaceInterpolateCurves::onLeftMouseClick, this, _1, _2, _3, _4);
			m_view->OnRightMouseClick = std::bind(&CSpaceInterpolateCurves::onRightMouseClick, this, _1, _2, _3, _4);

			/*
			m_view->OnKeyPress = std::bind(&CSpaceGUIDesign::onKeyPressed, this, _1, _2, _3);

			m_view->addAccelerator("Ctrl + C", [&](GUI::CBase* base) {this->onHotkey(base, "Ctrl + C"); });
			m_view->addAccelerator("Ctrl + V", [&](GUI::CBase* base) {this->onHotkey(base, "Ctrl + V"); });
			m_view->addAccelerator("Ctrl + D", [&](GUI::CBase* base) {this->onHotkey(base, "Ctrl + D"); });
			m_view->addAccelerator("Ctrl + Z", [&](GUI::CBase* base) {this->onHotkey(base, "Ctrl + Z"); });
			m_view->addAccelerator("Ctrl + Y", [&](GUI::CBase* base) {this->onHotkey(base, "Ctrl + Y"); });
			*/

			m_view->OnRender = BIND_LISTENER(&CSpaceInterpolateCurves::onRender, this);

			m_valueSettingMenu = new GUI::CMenu(window->getCanvas());
			m_valueSettingController = new CValueSettingController(editor, m_valueSettingMenu);

			m_insertPointMenu = new GUI::CMenu(window->getCanvas());
			m_insertPointMenu->addItem(L"Insert point");
			m_insertPointMenu->OnCommand = BIND_LISTENER(&CSpaceInterpolateCurves::onInsertCommand, this);
		}

		CSpaceInterpolateCurves::~CSpaceInterpolateCurves()
		{
			if (m_controller->OnClose != nullptr)
				m_controller->OnClose();

			delete m_controller;
		}

		void CSpaceInterpolateCurves::onResize(float w, float h)
		{

		}

		void CSpaceInterpolateCurves::onClear(GUI::CBase* base)
		{
			m_controller->clearGraph();
		}

		void CSpaceInterpolateCurves::onLinear(GUI::CBase* base)
		{
			m_controller->setDefaultLinear();
		}

		void CSpaceInterpolateCurves::onInOutCubic(GUI::CBase* base)
		{
			m_controller->setDefaultInOutCubic();
		}

		void CSpaceInterpolateCurves::onAutoZoom(GUI::CBase* base)
		{
			core::vector2df min, max;
			m_controller->getRangleMinMax(min, max);

			if (min == core::vector2df() && max == core::vector2df())
				return;

			m_guiScale.set(4096.0f, 2048.0f);
			m_gridSize.set(2048.0f, 2048.0f);

			float width = m_view->width();
			float height = m_view->height();

			m_viewX = 0.0f;
			m_viewY = 0.0f;

			while (m_guiScale.X > 2.0f && m_guiScale.Y > 2.0f)
			{
				core::vector2df pMin = min;
				core::vector2df pMax = max;

				ptToView(pMin.X, pMin.Y);
				ptToView(pMax.X, pMax.Y);

				float dx = pMax.X - pMin.X;
				float dy = pMax.Y - pMin.Y;

				m_viewX = min.X * m_guiScale.X;
				m_viewY = min.Y * m_guiScale.Y;

				if (dx < width && dy < height)
				{
					// align center graph
					m_viewX = m_viewX - (width - dx) * 0.5f;
					m_viewY = m_viewY - (height - dy) * 0.5f;
					break;
				}

				doZoomOut(0.0f, 0.0f);
			}
		}

		void CSpaceInterpolateCurves::onRender(GUI::CBase* base)
		{
			// flush 2d gui
			GUI::Context::getRenderer()->flush();
			core::recti vp = getVideoDriver()->getViewPort();
			getVideoDriver()->enableScissor(false);
			getVideoDriver()->clearZBuffer();

			{
				// setup scene viewport
				GUI::SPoint position = m_view->localPosToCanvas();
				core::recti viewport;
				viewport.UpperLeftCorner.set((int)position.X, (int)position.Y);
				viewport.LowerRightCorner.set((int)(position.X + m_view->width()), (int)(position.Y + base->height()));

				// setup new viewport
				getVideoDriver()->setViewPort(viewport);

				f32 w = m_view->width();
				f32 h = m_view->height();

				core::matrix4 project, view;
				project.buildProjectionMatrixOrthoLH(w, h, -1.0f, 1.0f);
				project.setTranslation(core::vector3df(-1.0f, -1.0f, 0.0f)); // origin is bottom right

				CGraphics2D::getInstance()->beginRenderGUI(project, view);

				renderGrid();
				renderGraph();
			}

			// resume gui render
			getVideoDriver()->enableScissor(true);
			getVideoDriver()->setViewPort(vp);
			GUI::Context::getRenderer()->setProjection();
		}

		void CSpaceInterpolateCurves::renderGrid()
		{
			CGraphics2D* g = CGraphics2D::getInstance();

			float gx = core::max_(m_gridSize.X, 16.0f);
			float gy = core::max_(m_gridSize.Y, 16.0f);

			float x = fmodf(-m_viewX, gx);
			float y = fmodf(-m_viewY, gy);

			float w = (float)m_view->width();
			float h = (float)m_view->height();

			int countX = core::ceil32(w / gx) + 1;
			int countY = core::ceil32(h / gy) + 1;

			SColor redlight(255, 80, 60, 60);
			SColor grey(255, 60, 60, 60);
			SColor dark(255, 20, 20, 20);

			// draw grid
			for (int i = 0; i < countX; i++)
			{
				float realX = (x + m_viewX) / m_guiScale.X;
				if (realX == 0.0f)
				{
					core::rectf r(x - 2.0f, 0.0f, x + 2.0f, h);
					g->draw2DRectangle(r, redlight);
				}
				else
				{
					bool d = fmodf(realX, 1.0f) == 0.0f;
					g->draw2DLine(core::position2df(x, 0.0f), core::position2df(x, h), d ? grey : dark);
				}

				x = x + gx;
			}

			for (int i = 0; i < countY; i++)
			{
				float realY = (y + m_viewY) / m_guiScale.Y;
				if (realY == 0.0f)
				{
					core::rectf r(0.0f, y - 2.0f, w, y + 2.0f);
					g->draw2DRectangle(r, redlight);
				}
				else
				{
					bool d = fmodf(realY, 1.0f) == 0.0f;
					g->draw2DLine(core::position2df(0.0f, y), core::position2df(w, y), d ? grey : dark);
				}

				y = y + gy;
			}
		}

		void CSpaceInterpolateCurves::renderGraph()
		{
			CGraphics2D* g = CGraphics2D::getInstance();

			std::vector<SControlPoint>& points = m_controller->getInterpolator().getControlPoints();
			int n = (int)points.size();
			if (n < 2)
				return;

			// compute all lines
			m_controller->getInterpolator().computeLine(m_lines, 20);

			// convert to view
			for (u32 i = 0, n = (u32)m_lines.size(); i < n; i++)
			{
				ArrayPoint2df& line = m_lines[i];
				for (u32 j = 0, m = line.size(); j < m; j++)
				{
					core::vector2df& a = line[j];
					ptToView(a.X, a.Y);
				}
			}

			// draw line
			SColor green(0xFF00AA00);
			SColor hover(0xFF1080FF);

			for (u32 i = 0, n = (u32)m_lines.size(); i < n; i++)
			{
				ArrayPoint2df& line = m_lines[i];
				for (u32 j = 1, m = line.size(); j < m; j++)
				{
					core::vector2df& a = line[j - 1];
					core::vector2df& b = line[j];

					g->draw2DLine(a, b, i == m_hoverLine ? hover : green);
				}
			}

			// draw control point
			core::vector2df pointSize(6.0f, 6.0f);
			core::vector2df controlSize(4.0f, 4.0f);

			SColor white(255, 200, 200, 200);
			SColor grey(255, 100, 100, 100);

			core::rectf r;
			core::vector2df pos, pos1, pos2;

			for (int i = 0; i < n; i++)
			{
				SControlPoint& p = points[i];

				pos = p.Position;
				ptToView(pos.X, pos.Y);

				if (p.Type == SControlPoint::Auto ||
					p.Type == SControlPoint::Smooth ||
					p.Type == SControlPoint::Broken)
				{
					if (i == 0)
					{
						pos2 = p.Position + p.Right;
						ptToView(pos2.X, pos2.Y);

						g->draw2DLine(pos, pos2, grey);

						r = core::rectf(pos2 - controlSize * 0.5f, pos2 + controlSize);
						g->draw2DRectangle(r, (i == m_hoverPoint && m_hoverState == 2) ? hover : white);
					}
					else if (i == n - 1)
					{
						pos1 = p.Position + p.Left;
						ptToView(pos1.X, pos1.Y);

						g->draw2DLine(pos, pos1, grey);

						r = core::rectf(pos1 - controlSize * 0.5f, pos1 + controlSize);
						g->draw2DRectangle(r, (i == m_hoverPoint && m_hoverState == 1) ? hover : white);
					}
					else
					{
						pos1 = p.Position + p.Right;
						ptToView(pos1.X, pos1.Y);

						pos2 = p.Position + p.Left;
						ptToView(pos2.X, pos2.Y);

						g->draw2DLine(pos, pos1, grey);
						g->draw2DLine(pos, pos2, grey);

						r = core::rectf(pos1 - controlSize * 0.5f, pos1 + controlSize);
						g->draw2DRectangle(r, (i == m_hoverPoint && m_hoverState == 2) ? hover : white);

						r = core::rectf(pos2 - controlSize * 0.5f, pos2 + controlSize);
						g->draw2DRectangle(r, (i == m_hoverPoint && m_hoverState == 1) ? hover : white);
					}
				}

				core::rectf r(pos - pointSize * 0.5f, pos + pointSize);
				g->draw2DRectangle(r, (i == m_hoverPoint && m_hoverState == 0) ? hover : white);
			}
		}

		float getPointToLineDistance(float x1, float y1, float x2, float y2, float px, float py)
		{
			float A = px - x1;
			float B = py - y1;
			float C = x2 - x1;
			float D = y2 - y1;

			float dot = A * C + B * D;
			float len_sq = C * C + D * D;
			float param = (len_sq != 0) ? dot / len_sq : -1;

			float xx, yy;

			if (param < 0)
			{
				xx = x1;
				yy = y1;
			}
			else if (param > 0.0f && param < 1.0f)
			{
				xx = x1 + param * C;
				yy = y1 + param * D;
			}
			else
			{
				xx = x2;
				yy = y2;
			}

			float dx = px - xx;
			float dy = py - yy;
			return sqrtf(dx * dx + dy * dy);
		}

		void CSpaceInterpolateCurves::setOwner(GUI::CBase* owner)
		{
			if (m_owner)
				onOwnerClosed();
			m_owner = owner;
		}

		void CSpaceInterpolateCurves::onOwnerClosed()
		{
			m_controller->onClosed();
			m_owner = NULL;
		}

		void CSpaceInterpolateCurves::onMiddleMouseClick(GUI::CBase* view, float x, float y, bool down)
		{
			m_middleDrag = down;

			if (down)
			{
				GUI::CInput::getInput()->setCapture(view);
				m_pressX = x;
				m_pressY = y;
				m_oldViewX = m_viewX;
				m_oldViewY = m_viewY;
			}
			else
			{
				GUI::CInput::getInput()->setCapture(NULL);

				float dx = x - m_pressX;
				float dy = y - m_pressY;

				m_viewX = m_oldViewX - dx;
				m_viewY = m_oldViewY + dy;
			}
		}

		void CSpaceInterpolateCurves::onMouseMoved(GUI::CBase* view, float x, float y, float deltaX, float deltaY)
		{
			if (m_middleDrag)
			{
				float dx = x - m_pressX;
				float dy = y - m_pressY;

				m_viewX = m_oldViewX - dx;
				m_viewY = m_oldViewY + dy;
			}

			// check the mouse position
			GUI::SPoint localPos = view->canvasPosToLocal(GUI::SPoint(x, y));

			m_mouseX = localPos.X;
			m_mouseY = view->height() - localPos.Y;

			m_mouseGUIX = m_mouseX;
			m_mouseGUIY = m_mouseY;
			viewToPt(m_mouseGUIX, m_mouseGUIY);

			wchar_t mouseText[512];
			swprintf(
				mouseText, 512,
				L"(%.02f, %.02f)",
				m_mouseGUIX,
				m_mouseGUIY
			);
			m_textMousePos->setString(std::wstring(mouseText));

			if (!m_leftMouseDown)
				checkHoverPoint();

			if (m_leftMouseDown && m_hoverPoint != -1)
				doDragPoint();
		}

		void CSpaceInterpolateCurves::checkHoverPoint()
		{
			m_hoverLine = -1;
			m_hoverPoint = -1;
			m_hoverState = 0;

			core::vector2df pos;
			int i = 0;

			std::vector<SControlPoint>& points = m_controller->getInterpolator().getControlPoints();
			for (SControlPoint& p : points)
			{
				pos = p.Position;
				ptToView(pos.X, pos.Y);

				if (core::abs_(m_mouseX - pos.X) < 8 && core::abs_(m_mouseY - pos.Y) < 8)
				{
					m_hoverPoint = i;
					m_hoverState = 0;
					break;
				}

				pos = p.Position + p.Left;
				ptToView(pos.X, pos.Y);

				if (core::abs_(m_mouseX - pos.X) < 8 && core::abs_(m_mouseY - pos.Y) < 8)
				{
					m_hoverPoint = i;
					m_hoverState = 1;
					break;
				}

				pos = p.Position + p.Right;
				ptToView(pos.X, pos.Y);

				if (core::abs_(m_mouseX - pos.X) < 8 && core::abs_(m_mouseY - pos.Y) < 8)
				{
					m_hoverPoint = i;
					m_hoverState = 2;
					break;
				}

				i++;
			}

			if (m_hoverPoint != -1)
				m_view->setCursor(GUI::ECursorType::SizeAll);
			else
				m_view->setCursor(GUI::ECursorType::Normal);

			if (m_hoverPoint == -1)
			{
				for (u32 i = 0, n = (u32)m_lines.size(); i < n; i++)
				{
					ArrayPoint2df& line = m_lines[i];
					for (u32 j = 1, m = line.size(); j < m; j++)
					{
						float d = getPointToLineDistance(
							line[j - 1].X,
							line[j - 1].Y,
							line[j].X,
							line[j].Y,
							(float)m_mouseX,
							(float)m_mouseY
						);

						if (d < 8)
						{
							m_hoverLine = i;
							break;
						}
					}
				}
			}
		}

		void CSpaceInterpolateCurves::doDragPoint()
		{
			std::vector<SControlPoint>& points = m_controller->getInterpolator().getControlPoints();

			SControlPoint& dragPoint = points[m_hoverPoint];

			if (m_hoverState == 0)
			{
				dragPoint.Position.set(m_mouseGUIX, m_mouseGUIY);
				m_controller->updateValue();
			}
			else if (m_hoverState == 1)
			{
				core::vector2df p(m_mouseGUIX, m_mouseGUIY);
				dragPoint.Left = p - dragPoint.Position;

				if (dragPoint.Type == SControlPoint::Auto)
				{
					dragPoint.Right = -dragPoint.Left;
				}
				else if (dragPoint.Type == SControlPoint::Smooth)
				{
					core::vector2df n = -dragPoint.Left;
					n.normalize();
					dragPoint.Right = n * dragPoint.Right.getLength();
				}
				m_controller->updateValue();
			}
			else if (m_hoverState == 2)
			{
				core::vector2df p(m_mouseGUIX, m_mouseGUIY);
				dragPoint.Right = p - dragPoint.Position;

				if (dragPoint.Type == SControlPoint::Auto)
				{
					dragPoint.Left = -dragPoint.Right;
				}
				else if (dragPoint.Type == SControlPoint::Smooth)
				{
					core::vector2df n = -dragPoint.Right;
					n.normalize();
					dragPoint.Left = n * dragPoint.Left.getLength();
				}
				m_controller->updateValue();
			}
		}

		void CSpaceInterpolateCurves::onLeftMouseClick(GUI::CBase* view, float x, float y, bool down)
		{
			m_leftMouseDown = down;

			if (down)
				GUI::CInput::getInput()->setCapture(view);
			else
				GUI::CInput::getInput()->setCapture(NULL);
		}

		void CSpaceInterpolateCurves::onRightMouseClick(GUI::CBase* view, float x, float y, bool down)
		{
			m_rightMouseDown = down;

			if (!down)
			{
				if (m_hoverPoint != -1)
				{
					std::vector<SControlPoint>& points = m_controller->getInterpolator().getControlPoints();

					m_valueSettingMenu->open(GUI::SPoint(x, y));
					m_valueSettingController->onShow(m_controller, &points[m_hoverPoint]);
				}
				else if (m_hoverLine != -1)
				{
					m_insertId = m_hoverLine;
					m_insertPosition.set(m_mouseGUIX, m_mouseGUIY);
					m_insertPointMenu->open(GUI::SPoint(x, y));
				}
			}
		}

		void CSpaceInterpolateCurves::onMouseWheel(GUI::CBase* view, int delta)
		{
			GUI::SPoint mousePos = GUI::CInput::getInput()->getMousePosition();
			GUI::SPoint localPos = m_view->canvasPosToLocal(mousePos);

			float dx = localPos.X;
			float dy = m_view->height() - localPos.Y;

			if (delta < 0)
			{
				// zoom in
				if (m_guiScale.X < 4096.0f && m_guiScale.Y < 4096.0f)
				{
					doZoomIn(dx, dy);
				}
			}
			else
			{
				// zoom out
				if (m_guiScale.X > 2.0f && m_guiScale.Y > 2.0f)
				{
					doZoomOut(dx, dy);
				}
			}
		}

		void CSpaceInterpolateCurves::onInsertCommand(GUI::CBase* base)
		{
			std::vector<SControlPoint>& points = m_controller->getInterpolator().getControlPoints();
			m_controller->insertPoint(&points[m_insertId], m_insertPosition);
		}

		void CSpaceInterpolateCurves::doZoomIn(float dx, float dy)
		{
			if (m_guiScale.X < 4096.0f && m_guiScale.Y < 4096.0f)
			{
				m_guiScale.X = m_guiScale.X * 2.0f;
				m_guiScale.Y = m_guiScale.Y * 2.0f;

				m_gridSize.X = m_gridSize.X * 2.0f;
				m_gridSize.Y = m_gridSize.Y * 2.0f;

				m_viewX = m_viewX + dx;
				m_viewY = m_viewY + dy;

				m_viewX = m_viewX * 2.0f;
				m_viewY = m_viewY * 2.0f;

				m_viewX = m_viewX - dx;
				m_viewY = m_viewY - dy;
			}
		}

		void CSpaceInterpolateCurves::doZoomOut(float dx, float dy)
		{
			if (m_guiScale.X > 2.0f && m_guiScale.Y > 2.0f)
			{
				m_guiScale.X = m_guiScale.X / 2.0f;
				m_guiScale.Y = m_guiScale.Y / 2.0f;

				m_gridSize.X = m_gridSize.X / 2.0f;
				m_gridSize.Y = m_gridSize.Y / 2.0f;

				m_viewX = m_viewX + dx;
				m_viewY = m_viewY + dy;

				m_viewX = m_viewX / 2.0f;
				m_viewY = m_viewY / 2.0f;

				m_viewX = m_viewX - dx;
				m_viewY = m_viewY - dy;
			}
		}

		void CSpaceInterpolateCurves::ptToView(float& x, float& y)
		{
			x = x * m_guiScale.X - m_viewX;
			y = y * m_guiScale.Y - m_viewY;
		}

		void CSpaceInterpolateCurves::viewToPt(float& x, float& y)
		{
			x = (x + m_viewX) / m_guiScale.X;
			y = (y + m_viewY) / m_guiScale.Y;
		}
	}
}