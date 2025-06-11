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
#include "Editor/CEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "GUI/GUI.h"
#include "CFileArrayEditor.h"
#include "Activator/CEditorActivator.h"
#include "AssetManager/CAssetImporter.h"
#include "Utils/CPath.h"

#include "Serializable/CTextureArraySerializable.h"

namespace Skylicht
{
	namespace Editor
	{
		CFileArrayEditor::CFileArrayEditor() :
			m_fileArray(NULL),
			m_obj(NULL)
		{

		}

		CFileArrayEditor::~CFileArrayEditor()
		{
			closeGUI();
		}

		void CFileArrayEditor::closeGUI()
		{
			CAssetEditor::closeGUI();
			if (m_fileArray)
			{
				delete m_fileArray;
				m_fileArray = NULL;
			}

			if (m_obj)
			{
				delete m_obj;
				m_obj = NULL;
			}
		}

		void CFileArrayEditor::initGUI(const char* path, CSpaceProperty* ui)
		{
			m_obj = createGetFileArray(path);

			GUI::CCollapsibleGroup* group = ui->addGroup("Array", this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);
			serializableToControl(m_obj, ui, layout);
			group->setExpand(true);
		}

		CObjectSerializable* CFileArrayEditor::createGetFileArray(const char* path)
		{
			CObjectSerializable* obj = new CObjectSerializable("Files");

			std::string ext = CPath::getFileNameExt(path);

			if (ext == "texarray")
				m_fileArray = new CTextureArraySerializable(obj);
			else
				m_fileArray = new CFileArraySerializable(obj);

			if (!m_fileArray->load(path))
				m_fileArray->save(path);

			return obj;
		}

		void CFileArrayEditor::onUpdateValue(CObjectSerializable* object)
		{
			m_fileArray->saveToFile();
		}

		ASSET_EDITOR_REGISTER(CFileArrayEditor, array);
		ASSET_EDITOR_REGISTER(CFileArrayEditor, texarray);
	}
}