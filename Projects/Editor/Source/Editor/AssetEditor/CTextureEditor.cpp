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

#include "Editor/CEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		CTextureEditor::CTextureEditor() :
			m_settings(NULL)
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

			GUI::CCollapsibleGroup* group = ui->addGroup("Texture setting", this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);
			serializableToControl(m_settings, ui, layout);
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
		}

		ASSET_EDITOR_REGISTER(CTextureEditor, png);
		ASSET_EDITOR_REGISTER(CTextureEditor, tga);
		ASSET_EDITOR_REGISTER(CTextureEditor, jpg);
		ASSET_EDITOR_REGISTER(CTextureEditor, jpeg);
		ASSET_EDITOR_REGISTER(CTextureEditor, bmp);
	}
}