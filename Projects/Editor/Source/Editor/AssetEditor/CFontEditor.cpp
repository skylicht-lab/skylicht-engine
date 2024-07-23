/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "GUI/GUI.h"
#include "CFontEditor.h"
#include "Activator/CEditorActivator.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/SpaceController/CAssetPropertyController.h"

#include "Editor/CEditor.h"
#include "GUI/Input/CInput.h"
#include "Serializable/CSerializableLoader.h"

#include "Graphics2D/SpriteFrame/CSpriteFrame.h"

namespace Skylicht
{
	namespace Editor
	{
		ASSET_EDITOR_REGISTER(CFontEditor, font);

		CFontEditor::CFontEditor() :
			m_fontSource(NULL)
		{

		}

		CFontEditor::~CFontEditor()
		{
			closeGUI();
		}

		void CFontEditor::closeGUI()
		{
			// save font
			if (!m_path.empty())
			{
				if (m_fontSource)
					m_fontSource->save(m_path.c_str());
			}

			// close
			CAssetEditor::closeGUI();
		}

		void CFontEditor::initGUI(const char* path, CSpaceProperty* ui)
		{
			CEditor* editor = CEditor::getInstance();

			m_fontSource = CFontManager::getInstance()->loadFontSource(path);
			CSerializableLoader::loadSerializable(path, m_fontSource);
			m_fontSource->initFont();

			m_path = path;

			GUI::CCollapsibleGroup* group = ui->addGroup("Font Source", this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);
			serializableToControl(m_fontSource, ui, layout);
			group->setExpand(true);

			group = ui->addGroup("Preview", this);
			GUI::CBoxLayout* boxLayout = ui->createBoxLayout(group);

			GUI::CLayout* row = boxLayout->beginVertical();

			GUI::CBase* fontRenderer = new GUI::CBase(row);
			fontRenderer->setHeight(500.0f);
			fontRenderer->enableRenderFillRect(true);
			fontRenderer->setFillRectColor(GUI::SGUIColor(255, 0, 0, 0));
			fontRenderer->OnRender = BIND_LISTENER(&CFontEditor::onRenderPreview, this);

			boxLayout->endVertical();

			group->setExpand(true);
		}

		void CFontEditor::onUpdateValue(CObjectSerializable* object)
		{
			// save font
			if (!m_path.empty())
			{
				if (m_fontSource)
					m_fontSource->save(m_path.c_str());
			}

			if (m_fontSource)
				m_fontSource->initFont();
		}

		void CFontEditor::onRenderPreview(GUI::CBase* base)
		{
			if (!m_fontSource || m_fontSource->getFont() == NULL)
				return;
			
			// flush 2d gui
			GUI::CGUIContext::getRenderer()->flush();
			IVideoDriver* driver = getVideoDriver();

			core::recti vp = driver->getViewPort();
			driver->enableScissor(false);
			driver->clearZBuffer();

			{
				// setup scene viewport
				GUI::SPoint position = base->localPosToCanvas();
				core::recti viewport;
				viewport.UpperLeftCorner.set((int)position.X, (int)position.Y);
				viewport.LowerRightCorner.set((int)(position.X + base->width()), (int)(position.Y + base->height()));
				driver->setViewPort(viewport);

				// draw text here
				IFont* font = m_fontSource->getFont();

				// setup 2d projection
				core::matrix4 projection, view;
				projection.buildProjectionMatrixOrthoLH((f32)viewport.getWidth(), -(f32)viewport.getHeight(), -1.0f, 1.0f);
				view.setTranslation(core::vector3df(-(f32)viewport.getWidth() * 0.5f, -(f32)viewport.getHeight() * 0.5f, 0.0f));

				// render preview text
				CGraphics2D* pG = CGraphics2D::getInstance();
				pG->beginRenderGUI(projection, view);

				const wchar_t* text = L"The quick brown fox jumps over the lazy dog";
				const wchar_t* textUppercase = L"THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG'S BACK";
				const wchar_t* textNumber = L"1234567890 `~!@#$%^&*();:,.|";

				float x = 10.0f;
				float y = 10.0f;
				float w = viewport.getWidth() - 2.0f * x;

				SModuleOffset* moduleCharA = font->getCharacterModule((int)'A');
				if (moduleCharA)
				{
					float charHeight = moduleCharA->OffsetY + moduleCharA->Module->H;

					// render string
					renderString(text, x, y, 10.0, w);

					// next line
					x = 10.0f;
					y = y + 2.0f * charHeight;
					renderString(textUppercase, x, y, 10.0, w);

					// next line
					x = 10.0f;
					y = y + 2.0f * charHeight;
					renderString(textNumber, x, y, 10.0, w);
				}

				pG->endRenderGUI();
			}

			// resume gui render
			driver->enableScissor(true);
			driver->setViewPort(vp);
			GUI::CGUIContext::getRenderer()->setProjection();
		}

		void CFontEditor::renderString(const wchar_t* text, float& px, float& py, float x, float w)
		{
			IFont* font = m_fontSource->getFont();
			
			SModuleOffset* moduleCharA = font->getCharacterModule((int)'A');
			float charHeight = moduleCharA->OffsetY + moduleCharA->Module->H;

			CGraphics2D* g = CGraphics2D::getInstance();
			SColor white(255, 255, 255, 255);
			int shaderID = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");

			std::vector<SModuleOffset*> modules;

			int i = 0;
			while (text[i] != 0)
			{
				SModuleOffset* c = font->getCharacterModule((int)text[i]);
				if (c != NULL)
					modules.push_back(c);

				i++;
			}

			float spacePadding = 0.0f;
			float charPadding = 0.0f;

			font->updateFontTexture();

			for (int i = 0, n = (int)modules.size(); i < n; i++)
			{
				SModuleOffset* moduleOffset = modules[i];

				// found space and need test for new line
				if (moduleOffset->Character == ' ')
				{
					float wordWidth = 0.0f;
					for (int j = i; j < n; j++)
					{
						SModuleOffset* c = modules[j];
						if (j > i && c->Character == ' ')
						{
							// end this word
							break;
						}
						wordWidth = wordWidth + c->XAdvance + charPadding;
					}

					// if size is over rect width
					if (px + wordWidth >= x + w)
					{
						px = x;
						py = py + charHeight;

						// new line and no need render space
						continue;
					}
				}

				// render the character
				g->addModuleBatch(moduleOffset, white, core::IdentityMatrix, px, py, shaderID);

				if (moduleOffset->Character == ' ')
					px += (moduleOffset->XAdvance + spacePadding);
				else
					px += (moduleOffset->XAdvance + charPadding);
			}
		}
	}
}
