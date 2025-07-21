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
#include "GUI/GUI.h"
#include "CTextureEditor.h"
#include "Activator/CEditorActivator.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "AssetManager/CAssetManager.h"

#include "Editor/CEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		CTextureEditor::CTextureEditor() :
			m_settings(NULL),
			m_thumbnails(NULL),
			m_width(0),
			m_height(0),
			m_txtScaleResult(NULL)
		{

		}

		CTextureEditor::~CTextureEditor()
		{
			closeGUI();
		}

		void CTextureEditor::closeGUI()
		{
			CAssetEditor::closeGUI();
			if (m_settings)
			{
				m_thumbnails->drop();
				m_thumbnails = NULL;
				m_settings->saveToFile();
				delete m_settings;
				m_settings = NULL;
			}
		}

		void CTextureEditor::initGUI(const char* path, CSpaceProperty* ui)
		{
			std::string meta = path;
			meta += ".meta";
			m_settings = createTextureSetting(meta.c_str());

			IImage* img = getVideoDriver()->createImageFromFile(path);
			m_width = 0;
			m_height = 0;

			std::string s = "\n";

			if (img)
			{
				m_width = img->getDimension().Width;
				m_height = img->getDimension().Height;
				img->getColorFormat();

				s += "Width: ";
				s += std::to_string(m_width);
				s += "\n";
				s += "Height: ";
				s += std::to_string(m_height);
				img->drop();
			}

			m_thumbnails = CAssetManager::getInstance()->getThumbnail()->getThumbnail(path);

			GUI::CCollapsibleGroup* group = ui->addGroup("Texture setting", this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);

			// Texture info
			GUI::CLayout* vLayout = layout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(vLayout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setTextAlignment(GUI::TextRight);
			label->setHeight(60.0f);
			label->setWrapMultiline(true);
			label->setString(s);

			GUI::CBase* container = new GUI::CBase(vLayout);
			container->setSize(64.0f, 64.0f);

			GUI::CImageButton* image = new GUI::CImageButton(container);
			image->setSize(64, 64);
			image->dock(GUI::EPosition::Left);
			image->getImage()->setColor(GUI::SGUIColor(255, 50, 50, 50));
			if (m_thumbnails)
			{
				float w = (float)m_thumbnails->getSize().Width;
				float h = (float)m_thumbnails->getSize().Height;
				GUI::CRawImage* rawImage = image->getImage();
				rawImage->setImage(m_thumbnails, GUI::SRect(0.0f, 0.0f, w, h));
				rawImage->setColor(GUI::SGUIColor(255, 255, 255, 255));
				m_thumbnails->grab();
			}

			layout->endVertical();
			layout->addSpace(10.0f);

			// Texture custom data control
			serializableToControl(m_settings, ui, layout);

			// Custom size data
			vLayout = layout->beginVertical();
			label = new GUI::CLabel(vLayout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setTextAlignment(GUI::TextRight);
			label->setHeight(60.0f);
			label->setWrapMultiline(true);
			m_txtScaleResult = label;
			showTargetSize();
			layout->endVertical();

			layout->addSpace(10.0f);

			group->setExpand(true);
		}

		CTextureSettings* CTextureEditor::createTextureSetting(const char* path)
		{
			CTextureSettings* setting = new CTextureSettings();
			if (!setting->load(path))
				setting->save(path);
			return setting;
		}

		void CTextureEditor::onUpdateValue(CObjectSerializable* object)
		{
			m_settings->saveToFile();
			showTargetSize();
		}

		void CTextureEditor::showTargetSize()
		{
			if (m_settings->AutoScale.get() == false)
			{
				float scale = m_settings->CustomScale.get();
				float targetW = m_width * scale;
				float targetH = m_height * scale;
				int w = 4;
				int h = 4;
				while (w < targetW && h < targetH)
				{
					w = w * 2;
					h = h * 2;
				}
				std::string s = "";
				s += "Target width: ";
				s += std::to_string(w);
				s += "\n";
				s += "Target height: ";
				s += std::to_string(h);
				m_txtScaleResult->setString(s);
			}
			else
			{
				m_txtScaleResult->setString("");
			}
		}

		ASSET_EDITOR_REGISTER(CTextureEditor, png);
		ASSET_EDITOR_REGISTER(CTextureEditor, tga);
		ASSET_EDITOR_REGISTER(CTextureEditor, jpg);
		ASSET_EDITOR_REGISTER(CTextureEditor, jpeg);
		ASSET_EDITOR_REGISTER(CTextureEditor, bmp);
	}
}