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
#include "CMatEditor.h"
#include "Activator/CEditorActivator.h"
#include "Editor/Space/Property/CSpaceProperty.h"

#include "MeshManager/CMeshManager.h"
#include "Material/CMaterialManager.h"
#include "Editor/CEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		ASSET_EDITOR_REGISTER(CMatEditor, mat);

		CMatEditor::CMatEditor()
		{

		}

		CMatEditor::~CMatEditor()
		{
			clear();
		}

		void CMatEditor::clear()
		{
			CAssetEditor::clear();

			// clean all subject
			for (ISubject* subject : m_subjects)
				delete subject;
			m_subjects.clear();
		}

		void CMatEditor::initGUI(const char* path, CSpaceProperty* ui)
		{
			ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial(path, true, std::vector<std::string>());

			for (CMaterial* material : materials)
			{
				GUI::CCollapsibleGroup* group = ui->addGroup(material->getName(), this);
				GUI::CBoxLayout* layout = ui->createBoxLayout(group);

				std::vector<std::string> shaderExts = { ".xml" };

				CSubject<std::string>* shaderValue = new CSubject<std::string>(material->getShaderPath());
				shaderValue->addObserver(new CObserver([](ISubject* subject, IObserver* from)
					{

					}), true);
				m_subjects.push_back(shaderValue);

				ui->addInputFile(layout, L"Shader", shaderValue, shaderExts);

				group->setExpand(true);
			}
		}

		void CMatEditor::onUpdateValue(CObjectSerializable* object)
		{

		}
	}
}