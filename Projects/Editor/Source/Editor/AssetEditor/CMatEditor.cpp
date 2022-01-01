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
#include "Editor/SpaceController/CAssetPropertyController.h"

#include "MeshManager/CMeshManager.h"
#include "Editor/CEditor.h"
#include "GUI/Input/CInput.h"

namespace Skylicht
{
	namespace Editor
	{
		ASSET_EDITOR_REGISTER(CMatEditor, mat);

		GUI::CMenu* CMatEditor::s_pickTextureMenu = NULL;

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

			CEditor* editor = CEditor::getInstance();
			if (s_pickTextureMenu == NULL)
			{
				s_pickTextureMenu = new GUI::CMenu(editor->getRootCanvas());
				s_pickTextureMenu->addItem(L"Clear Texture", GUI::ESystemIcon::Close);
			}

			m_path = path;
			m_materials = CMaterialManager::getInstance()->loadMaterial(path, true, std::vector<std::string>());

			std::vector<GUI::CCollapsibleGroup*> groups;

			for (CMaterial* material : m_materials)
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
					showShaderGUI(ui, layout, material, shader, m_subjects, m_materials, m_path);
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

		void CMatEditor::showShaderGUI(CSpaceProperty* ui, GUI::CBoxLayout* layout, CMaterial* material, CShader* shader, std::vector<ISubject*> subjects, const ArrayMaterial& materials, const std::string& path)
		{
			int numUI = shader->getNumUI();
			for (int i = 0; i < numUI; i++)
			{
				CShader::SUniformUI* uniformUI = shader->getUniformUI(i);

				if (uniformUI->ControlType == CShader::UIGroup)
					addUniformUI(ui, layout, material, shader, uniformUI, 0, subjects, materials, path);
				else
				{
					if (uniformUI->UniformInfo != NULL)
						addUniformUI(ui, layout, material, shader, uniformUI, 0, subjects, materials, path);
				}
			}
		}

		void CMatEditor::addUniformUI(CSpaceProperty* ui, GUI::CBoxLayout* layout, CMaterial* material, CShader* shader, CShader::SUniformUI* uniformUI, int tab, std::vector<ISubject*> subjects, const ArrayMaterial& materials, const std::string& path)
		{
			wchar_t text[512];
			CStringImp::convertUTF8ToUnicode(uniformUI->Name.c_str(), text);

			if (uniformUI->ControlType == CShader::UIGroup)
			{
				ui->addLabel(layout, text);

				for (int i = 0, n = (int)uniformUI->Childs.size(); i < n; i++)
				{
					if (uniformUI->Childs[i]->UniformInfo != NULL)
						addUniformUI(ui, layout, material, shader, uniformUI->Childs[i], tab + 1, subjects, materials, path);
				}
			}
			else if (uniformUI->ControlType == CShader::UIColor)
			{

			}
			else if (uniformUI->ControlType == CShader::UISlider)
			{

			}
			else if (uniformUI->ControlType == CShader::UIFloat)
			{

			}
			else if (uniformUI->ControlType == CShader::UIFloat2)
			{

			}
			else if (uniformUI->ControlType == CShader::UIFloat3)
			{

			}
			else if (uniformUI->ControlType == CShader::UIFloat4)
			{

			}
			else if (uniformUI->ControlType == CShader::UITexture)
			{
				CMaterial::SUniformTexture* unifromTexture = material->getUniformTexture(uniformUI->Name.c_str());

				CSubject<std::string>* newSubject = new CSubject<std::string>(unifromTexture->Path);
				subjects.push_back(newSubject);

				GUI::CImageButton* imageButton = ui->addInputTextureFile(layout, text, newSubject);

				if (unifromTexture->Texture != NULL)
				{
					const core::dimension2du& size = unifromTexture->Texture->getSize();
					imageButton->getImage()->setImage(
						unifromTexture->Texture,
						GUI::SRect(0.0f, 0.0f, (float)size.Width, (float)size.Height)
					);
					imageButton->getImage()->setColor(GUI::SGUIColor(255, 255, 255, 255));
				}

				imageButton->OnAcceptDragDrop = [](GUI::SDragDropPackage* data)
				{
					if (data->Name == "ListFSItem")
					{
						GUI::CListRowItem* item = (GUI::CListRowItem*)data->Control;

						std::string fullPath = item->getTagString();
						bool isFolder = item->getTagBool();

						if (!isFolder)
						{
							std::string ext = CPath::getFileNameExt(fullPath);
							ext = CStringImp::toLower(ext);
							if (ext == "png" || ext == "tga" || ext == "bmp" || ext == "jpg" || ext == "jpeg")
							{
								return true;
							}
						}
					}
					return false;
				};

				imageButton->OnDrop = [imgBtn = imageButton, uniformName = uniformUI->Name, mat = material, listMaterial = materials, p = path](GUI::SDragDropPackage* data, float mouseX, float mouseY)
				{
					if (data->Name == "ListFSItem")
					{
						GUI::CListRowItem* item = (GUI::CListRowItem*)data->Control;

						std::string fullPath = item->getTagString();
						std::string shortPath = CAssetManager::getInstance()->getShortPath(fullPath.c_str());
						std::string shortMaterialPath = CAssetManager::getInstance()->getShortPath(p.c_str());
						std::string name = CPath::getFileName(shortPath);

						ITexture* texture = CTextureManager::getInstance()->getTexture(shortPath.c_str());
						if (texture != NULL)
						{
							// set texture
							CMaterial::SUniformTexture* uniform = mat->getUniformTexture(uniformName.c_str());
							uniform->Path = shortPath;
							uniform->Texture = texture;
							mat->autoDetectLoadTexture();
							mat->applyMaterial();

							// save
							CMaterialManager::getInstance()->saveMaterial(listMaterial, shortMaterialPath.c_str());

							// update gui
							const core::dimension2du& size = texture->getSize();
							imgBtn->getImage()->setImage(
								texture,
								GUI::SRect(0.0f, 0.0f, (float)size.Width, (float)size.Height)
							);
							imgBtn->getImage()->setColor(GUI::SGUIColor(255, 255, 255, 255));
						}
						else
						{
							CEditor* editor = CEditor::getInstance();

							GUI::CMessageBox* msgBox = new GUI::CMessageBox(editor->getRootCanvas(), GUI::CMessageBox::OK);
							msgBox->setMessage("Can't load texture!", name.c_str());
							msgBox->getMessageIcon()->setIcon(GUI::ESystemIcon::Alert);
						}
					}
				};

				imageButton->OnPress = [uniformName = uniformUI->Name, mat = material](GUI::CBase* button)
				{
					CMaterial::SUniformTexture* uniform = mat->getUniformTexture(uniformName.c_str());
					if (uniform->Texture)
					{
						std::string assetPath = uniform->Texture->getName().getPath().c_str();
						assetPath = CAssetManager::getInstance()->getShortPath(assetPath.c_str());
						CAssetPropertyController::getInstance()->browseAsset(assetPath.c_str());
					}
				};

				imageButton->OnRightPress = [uniformName = uniformUI->Name,
					mat = material,
					mats = materials,
					imgBtn = imageButton,
					matPath = path](GUI::CBase* button)
				{
					GUI::SPoint mousePos = GUI::CInput::getInput()->getMousePosition();
					s_pickTextureMenu->open(mousePos);
					s_pickTextureMenu->OnCommand = [name = uniformName,
						m = mat,
						ms = mats,
						btn = imgBtn,
						p = matPath](GUI::CBase* item)
					{
						// clear texture
						CMaterial::SUniformTexture* uniform = m->getUniformTexture(name.c_str());
						uniform->Path = "";
						uniform->Texture = NULL;

						// clear gui
						btn->getImage()->setImage(NULL, GUI::SRect(0.0f, 0.0f, 0.0f, 0.0f));
						btn->getImage()->setColor(GUI::SGUIColor(255, 0, 0, 0));

						// save
						std::string shortMaterialPath = CAssetManager::getInstance()->getShortPath(p.c_str());
						CMaterialManager::getInstance()->saveMaterial(ms, shortMaterialPath.c_str());
					};
				};
			}
		}
	}
}