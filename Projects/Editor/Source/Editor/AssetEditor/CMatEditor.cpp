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
			closeGUI();
		}

		void CMatEditor::closeGUI()
		{
			CAssetEditor::closeGUI();

			// clean all subject
			for (ISubject* subject : m_subjects)
				delete subject;
			m_subjects.clear();
		}

		void CMatEditor::initGUI(const char* path, CSpaceProperty* ui)
		{
			CShaderManager* shaderManager = CShaderManager::getInstance();
			ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial(path, true, std::vector<std::string>());

			std::vector<GUI::CCollapsibleGroup*> groups;

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

				layout->addSpace(10.0f);

				CShader* shader = shaderManager->getShaderByPath(material->getShaderPath());
				if (shader == NULL)
					shader = shaderManager->loadShader(material->getShaderPath());

				if (shader != NULL)
				{
					// show shader UI
					showShaderGUI(ui, layout, material, shader, m_subjects);
				}

				groups.push_back(group);
			}

			if (groups.size() < 3)
			{
				for (GUI::CCollapsibleGroup* group : groups)
					group->setExpand(true);
			}
		}

		void CMatEditor::onUpdateValue(CObjectSerializable* object)
		{

		}

		void CMatEditor::showShaderGUI(CSpaceProperty* ui, GUI::CBoxLayout* layout, CMaterial* material, CShader* shader, std::vector<ISubject*> subjects)
		{
			int numUI = shader->getNumUI();
			for (int i = 0; i < numUI; i++)
			{
				CShader::SUniformUI* uniformUI = shader->getUniformUI(i);

				if (uniformUI->ControlType == CShader::UIGroup)
					addUniformUI(ui, layout, material, shader, uniformUI, 0, subjects);
				else
				{
					if (uniformUI->UniformInfo != NULL)
						addUniformUI(ui, layout, material, shader, uniformUI, 0, subjects);
				}
			}
		}

		void CMatEditor::addUniformUI(CSpaceProperty* ui, GUI::CBoxLayout* layout, CMaterial* material, CShader* shader, CShader::SUniformUI* uniformUI, int tab, std::vector<ISubject*> subjects)
		{
			wchar_t text[512];
			CStringImp::convertUTF8ToUnicode(uniformUI->Name.c_str(), text);

			if (uniformUI->ControlType == CShader::UIGroup)
			{
				ui->addLabel(layout, text);

				for (int i = 0, n = (int)uniformUI->Childs.size(); i < n; i++)
				{
					if (uniformUI->Childs[i]->UniformInfo != NULL)
						addUniformUI(ui, layout, material, shader, uniformUI->Childs[i], tab + 1, subjects);
				}
			}
			else if (uniformUI->ControlType == CShader::UITexture)
			{
				CMaterial::SUniformTexture* unifromTexture = material->getUniformTexture(uniformUI->Name.c_str());
				CSubject<std::string>* newSubject = new CSubject(unifromTexture->Path);
				subjects.push_back(newSubject);

				GUI::CRawImage* image = ui->addInputTextureFile(layout, text, newSubject);

				if (unifromTexture->Texture != NULL)
				{
					const core::dimension2du& size = unifromTexture->Texture->getSize();
					image->setImage(
						unifromTexture->Texture,
						GUI::SRect(0.0f, 0.0f, (float)size.Width, (float)size.Height)
					);
				}
			}
		}
	}
}