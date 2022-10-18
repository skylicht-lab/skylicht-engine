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
#include "CSpaceGUIDesign.h"

namespace Skylicht
{
	namespace Editor
	{
		const float LeftOffset = 40.0f;

		CSpaceGUIDesign::CSpaceGUIDesign(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_guiWidth(1920.0f),
			m_guiHeight(1080.0f),
			m_guiScale(1.0f)
		{
			GUI::CButton* btn = NULL;
			m_toolBar = new GUI::CToolbar(window);
			m_toolBar->addButton(L"New", GUI::ESystemIcon::NewFile);
			m_toolBar->addButton(L"Save", GUI::ESystemIcon::Save);
			m_toolBar->addSpace();

			btn = m_toolBar->addButton(L"Zoom in", GUI::ESystemIcon::ZoomIn);;
			btn->OnPress = BIND_LISTENER(&CSpaceGUIDesign::onZoomIn, this);

			btn = m_toolBar->addButton(L"Zoom out", GUI::ESystemIcon::ZoomOut);
			btn->OnPress = BIND_LISTENER(&CSpaceGUIDesign::onZoomOut, this);

			m_toolBar->addSpace();
			m_toolBar->addButton(GUI::ESystemIcon::Copy);
			m_toolBar->addButton(GUI::ESystemIcon::Paste);

			m_textZoom = new GUI::CLabel(m_toolBar);
			m_toolBar->addControl(m_textZoom, true);
			m_textZoom->setPadding(GUI::SPadding(0.0f, 3.0f, 0.0f, 0.0f));
			m_textZoom->setString(L"Zoom: 100%");
			m_textZoom->sizeToContents();

			m_leftRuler = new GUI::CRulerBar(window, false);
			m_leftRuler->setHeight(100.0f);
			m_leftRuler->setBeginOffset(LeftOffset);
			m_leftRuler->setPixelPerUnit(10.0f);

			m_topRuler = new GUI::CRulerBar(window, true);
			m_topRuler->setWidth(200.0f);
			m_topRuler->setPixelPerUnit(10.0f);

			m_leftRuler->setUnitScale(10.0f);
			m_topRuler->setUnitScale(10.0f);

			m_leftRuler->setPosition(0.0f);
			m_topRuler->setPosition(0.0f);

			m_scrollPanel = new GUI::CScrollControl(window);
			m_scrollPanel->dock(GUI::EPosition::Fill);
			m_scrollPanel->enableRenderFillRect(true);
			m_scrollPanel->enableScroll(true, true);
			m_scrollPanel->showScrollBar(true, true);
			m_scrollPanel->OnLayout = BIND_LISTENER(&CSpaceGUIDesign::onScrollLayout, this);

			m_guiRender = new GUI::CBase(m_scrollPanel);
			m_guiRender->setWidth(m_guiWidth * m_guiScale);
			m_guiRender->setHeight(m_guiHeight * m_guiScale);
			m_guiRender->enableRenderFillRect(true);
			m_guiRender->setFillRectColor(GUI::CThemeConfig::Black);
		}

		CSpaceGUIDesign::~CSpaceGUIDesign()
		{

		}

		void CSpaceGUIDesign::openGUI(const char* path)
		{

		}

		void CSpaceGUIDesign::onScrollLayout(GUI::CBase* scroll)
		{
			float x = m_scrollPanel->getScrollHorizontal();
			float y = m_scrollPanel->getScrollVertical();

			m_topRuler->setBeginOffset(x);
			m_leftRuler->setBeginOffset(LeftOffset + y);
		}

		void CSpaceGUIDesign::onZoomIn(GUI::CBase* base)
		{
			if (m_guiScale < 4.0f)
			{
				m_guiScale = m_guiScale * 2.0f;

				int z = (int)(m_guiScale * 100);
				wchar_t text[128];
				swprintf(text, 128, L"Zoom: %2d%%", z);
				m_textZoom->setString(text);

				m_guiRender->setWidth(m_guiWidth * m_guiScale);
				m_guiRender->setHeight(m_guiHeight * m_guiScale);

				m_leftRuler->setPixelPerUnit(10.0f * m_guiScale);
				m_topRuler->setPixelPerUnit(10.0f * m_guiScale);

				m_leftRuler->setTextPerUnit((int)(10.0f / m_guiScale));
				m_topRuler->setTextPerUnit((int)(10.0f / m_guiScale));
			}
		}

		void CSpaceGUIDesign::onZoomOut(GUI::CBase* base)
		{
			if (m_guiScale > 0.25f)
			{
				m_guiScale = m_guiScale * 0.5f;

				int z = (int)(m_guiScale * 100);
				wchar_t text[128];
				swprintf(text, 128, L"Zoom: %2d%%", z);
				m_textZoom->setString(text);

				m_guiRender->setWidth(m_guiWidth * m_guiScale);
				m_guiRender->setHeight(m_guiHeight * m_guiScale);

				m_leftRuler->setPixelPerUnit(10.0f * m_guiScale);
				m_topRuler->setPixelPerUnit(10.0f * m_guiScale);

				m_leftRuler->setTextPerUnit((int)(10.0f / m_guiScale));
				m_topRuler->setTextPerUnit((int)(10.0f / m_guiScale));
			}
		}
	}
}