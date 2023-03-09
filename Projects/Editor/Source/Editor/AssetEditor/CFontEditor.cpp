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

			if (m_fontSource)
			{
				delete m_fontSource;
				m_fontSource = NULL;
			}

			// close
			CAssetEditor::closeGUI();
		}

		void CFontEditor::initGUI(const char* path, CSpaceProperty* ui)
		{
			CEditor* editor = CEditor::getInstance();

			m_fontSource = new CFontSource();
			CSerializableLoader::loadSerializable(path, m_fontSource);

			m_path = path;
		}

		void CFontEditor::onUpdateValue(CObjectSerializable* object)
		{

		}
	}
}