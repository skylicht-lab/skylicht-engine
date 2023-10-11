/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "GUI/CGUIContext.h"
#include "CSpaceSprite.h"
#include "AssetManager/CAssetManager.h"
#include "Editor/CEditor.h"

#include <filesystem>
#if defined(__APPLE_CC__)
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace Skylicht
{
	namespace Editor
	{
		CSpaceSprite::CSpaceSprite(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor)
		{
			GUI::CToolbar* toolbar = new GUI::CToolbar(window);

			m_label = new GUI::CLabel(toolbar);
			m_label->setPadding(GUI::SPadding(10.0f, 2.0f, 0.0f, 0.0f));
			m_label->setString(L"");
			m_label->sizeToContents();
			toolbar->addControl(m_label);

			m_inputSearch = new GUI::CTextBox(toolbar);
			m_inputSearch->setWidth(200.0f);
			m_inputSearch->showIcon();
			m_inputSearch->setStringHint(L"Search");
			toolbar->addControl(m_inputSearch, true);

			m_view = new GUI::CThumbnailView(window, 115.0f, 100.0f);
			m_view->dock(GUI::EPosition::Fill);

			m_materialID = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");
		}

		CSpaceSprite::~CSpaceSprite()
		{
		}

		void CSpaceSprite::openSprite(const std::string& path)
		{
			CSpriteFrame* sprite = CSpriteManager::getInstance()->loadSprite(path.c_str());

			std::string shortPath = CPath::getFileName(path.c_str());
			m_label->setString(CStringImp::convertUTF8ToUnicode(shortPath.c_str()));
			m_label->sizeToContents();

			std::vector<SFrame*>& allFrames = sprite->getFrames();
			for (SFrame* frame : allFrames)
			{
				GUI::CThumbnailItem* item = m_view->addItem();
				item->setLabel(CStringImp::convertUTF8ToUnicode(frame->Name.c_str()));
				item->getRenderControl()->OnRender = [&, frame](GUI::CBase* base) {
					onRenderFrame(base, frame);
				};
			}
		}

		void CSpaceSprite::update()
		{
			CSpace::update();
		}

		void CSpaceSprite::onDestroy(GUI::CBase* base)
		{
			CSpace::onDestroy(base);
		}

		void CSpaceSprite::onRenderFrame(GUI::CBase* base, SFrame* frame)
		{
			// flush the current gui
			GUI::CGUIContext::getRenderer()->flush();
			core::recti vp = getVideoDriver()->getViewPort();

			// get base rect for renderer
			GUI::SPoint position = base->localPosToCanvas();

			// scale the frame
			float sourceSize = core::max_(frame->getWidth(), frame->getHeight());
			float renderSize = core::max_(base->width(), base->height());
			float scale = sourceSize < renderSize ? 1.0f : renderSize / sourceSize;

			// align center of the render control
			float sw = frame->getWidth() * scale;
			float sh = frame->getHeight() * scale;
			float cx = (base->width() - sw) * 0.5f;
			float cy = (base->height() - sh) * 0.5f;

			// draw the frame
			core::matrix4 transform;
			transform.setTranslation(core::vector3df(position.X + cx, position.Y + cy, 0.0f));
			transform.setScale(scale);

			CGraphics2D* graphics = CGraphics2D::getInstance();
			graphics->addFrameBatch(frame, SColor(255, 255, 255, 255), transform, m_materialID);
			graphics->flush();

			// resume gui render
			getVideoDriver()->enableScissor(true);
			getVideoDriver()->setViewPort(vp);
			GUI::CGUIContext::getRenderer()->setProjection();
		}
	}
}