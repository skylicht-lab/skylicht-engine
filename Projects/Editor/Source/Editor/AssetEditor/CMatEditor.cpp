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
		GUI::CMenu* CMatEditor::s_settingMaterialMenu = NULL;

		CMatEditor::CMatEditor() :
			m_renameContext(NULL),
			m_materialContext(NULL),
			m_groupContext(NULL)
		{

		}

		CMatEditor::~CMatEditor()
		{
			closeGUI();
		}

		void CMatEditor::closeGUI()
		{
			// save material
			if (!m_path.empty())
			{
				CMaterialManager::getInstance()->saveMaterial(m_materials, m_path.c_str());
			}

			// close
			CAssetEditor::closeGUI();
		}

		void CMatEditor::OnSettingCommand(GUI::CBase* menu)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(menu);
			if (menuItem == NULL || m_materialContext == NULL)
				return;

			const std::wstring& command = menuItem->getLabel();

			if (command == L"Rename")
			{
				if (m_renameContext != NULL)
				{
					m_renameContext->beginEdit([&](GUI::CBase* control)
						{
							GUI::CTextBox* textbox = dynamic_cast<GUI::CTextBox*>(control);
							std::wstring newNameW = textbox->getString();
							if (!newNameW.empty())
							{
								// rename
								std::string newName = CStringImp::convertUnicodeToUTF8(newNameW.c_str());

								char* trim = new char[newName.size()];
								CStringImp::copy(trim, newName.c_str());
								CStringImp::trim(trim);
								m_materialContext->rename(trim);

								// save material								
								CMaterialManager::getInstance()->saveMaterial(m_materials, m_path.c_str());
							}
						},
						[](GUI::CBase*)
						{
							// on cancel

						});
				}
			}
			else if (command == L"Delete")
			{
				GUI::CMessageBox* msgBox = new GUI::CMessageBox(GUI::CGUIContext::getRoot(), GUI::CMessageBox::YesNo);
				msgBox->setMessage("Do you want to delete the material", m_materialContext->getName());
				msgBox->OnYes = [&](GUI::CBase*)
				{
					// hide this group
					m_groupContext->setHidden(true);

					// delete and save material
					CMaterialManager::getInstance()->deleteMaterial(m_materials, m_materialContext);

					// save material					
					CMaterialManager::getInstance()->saveMaterial(m_materials, m_path.c_str());
				};
			}
		}

		void CMatEditor::initGUI(const char* path, CSpaceProperty* ui)
		{
			CEditor* editor = CEditor::getInstance();
			if (s_pickTextureMenu == NULL)
			{
				s_pickTextureMenu = new GUI::CMenu(editor->getRootCanvas());
				s_pickTextureMenu->addItem(L"Clear Texture", GUI::ESystemIcon::Close);
			}

			if (s_settingMaterialMenu == NULL)
			{
				s_settingMaterialMenu = new GUI::CMenu(editor->getRootCanvas());
				s_settingMaterialMenu->OnCommand = BIND_LISTENER(&CMatEditor::OnSettingCommand, this);
				s_settingMaterialMenu->addItem(L"Rename");
				s_settingMaterialMenu->addSeparator();
				s_settingMaterialMenu->addItem(L"Delete", GUI::ESystemIcon::Trash);
			}

			m_path = path;
			m_materials = CMaterialManager::getInstance()->loadMaterial(path, true, std::vector<std::string>());

			std::vector<GUI::CCollapsibleGroup*> groups;

			for (CMaterial* material : m_materials)
			{
				GUI::CCollapsibleGroup* group = ui->addGroup(material->getName(), this);

				GUI::CCollapsibleButton* header = group->getHeader();
				GUI::CTextEditHelper* textEditHelper = new GUI::CTextEditHelper(
					header, new GUI::CTextBox(header), header->getTextContainer()
				);

				// add setting on material name
				GUI::CButton* btn = new GUI::CButton(group->getHeader());
				btn->setWidth(20.0f);
				btn->setPadding(GUI::SPadding(0.0f, 0.0f, 0.0f, 0.0f));
				btn->dock(GUI::EPosition::Right);
				btn->setIcon(GUI::ESystemIcon::Setting);
				btn->showIcon(true);
				btn->showLabel(false);
				btn->enableDrawBackground(false);
				btn->tagData(editor);
				btn->OnPress = [&, editor, textEditHelper, btn, material, group](GUI::CBase* button)
				{
					m_groupContext = group;
					m_materialContext = material;
					m_renameContext = textEditHelper;

					s_settingMaterialMenu->open(btn);
				};


				GUI::CBoxLayout* layout = ui->createBoxLayout(group);

				std::vector<std::string> shaderExts = { "xml" };

				// On change shader
				CSubject<std::string>* shaderValue = new CSubject<std::string>(material->getShaderPath());
				shaderValue->addObserver(new CObserver([&, material, group](ISubject* subject, IObserver* from)
					{
						CSubject<std::string>* value = (CSubject<std::string>*)subject;

						// remove old layout
						GUI::CBoxLayout* layout = m_materialUI[material];
						layout->remove();

						// create new layout
						layout = ui->createBoxLayout(group);
						m_materialUI[material] = layout;

						// change the shader
						material->changeShader(value->get().c_str());
						showMaterialGUI(ui, layout, material);

						// save material						
						CMaterialManager::getInstance()->saveMaterial(m_materials, m_path.c_str());
					}), true);
				m_subjects.push_back(shaderValue);

				ui->addInputFile(layout, L"Shader", shaderValue, shaderExts);

				layout->addSpace(10.0f);

				layout = ui->createBoxLayout(group);

				showMaterialGUI(ui, layout, material);
				m_materialUI[material] = layout;

				groups.push_back(group);
			}

			GUI::CCollapsibleGroup* group = ui->addGroup("Functions", this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);
			ui->addButton(layout, L"Add new material")->OnPress = [&](GUI::CBase* button)
			{
				// Add new material
				CMaterialManager::getInstance()->createMaterial(m_materials);

				// save material
				CMaterialManager::getInstance()->saveMaterial(m_materials, m_path.c_str());

				// update the gui
				CAssetPropertyController::getInstance()->onSelectAsset(m_path.c_str(), false);
				CEditor::getInstance()->refresh();
			};
			group->setExpand(true);

			if (groups.size() < 3)
			{
				for (GUI::CCollapsibleGroup* group : groups)
					group->setExpand(true);
			}
		}

		void CMatEditor::onUpdateValue(CObjectSerializable* object)
		{

		}

		void CMatEditor::showMaterialGUI(CSpaceProperty* ui, GUI::CBoxLayout* layout, CMaterial* material)
		{
			CShaderManager* shaderManager = CShaderManager::getInstance();

			// show shader UI
			showShaderGUI(ui, layout, material, m_subjects,
				[&]() {
					// on change					
					CMaterialManager::getInstance()->saveMaterial(m_materials, m_path.c_str());
				});
		}

		void CMatEditor::showShaderGUI(CSpaceProperty* ui, GUI::CBoxLayout* layout, CMaterial* material, std::vector<ISubject*> subjects, std::function<void()> onChange)
		{
			CShader* shader = material->getShader();
			if (shader == NULL)
			{
				wchar_t text[512];
				sprintf("Missing shader: %s\n", CPath::getFileName(material->getShaderPath()).c_str());
				ui->addLabel(layout, text);
				return;
			}

			int numUI = shader->getNumUI();
			for (int i = 0; i < numUI; i++)
			{
				CShader::SUniformUI* uniformUI = shader->getUniformUI(i);

				if (uniformUI->ControlType == CShader::UIGroup)
					addUniformUI(ui, layout, material, shader, uniformUI, 0, subjects, onChange);
				else
				{
					if (uniformUI->UniformInfo != NULL)
						addUniformUI(ui, layout, material, shader, uniformUI, 0, subjects, onChange);
				}
			}
		}

		void CMatEditor::addUniformUI(CSpaceProperty* ui, GUI::CBoxLayout* layout, CMaterial* material, CShader* shader, CShader::SUniformUI* uniformUI, int tab, std::vector<ISubject*> subjects, std::function<void()> onChange)
		{
			wchar_t text[512];
			CStringImp::convertUTF8ToUnicode(uniformUI->Name.c_str(), text);

			if (uniformUI->ControlType == CShader::UIGroup)
			{
				ui->addLabel(layout, text);

				for (int i = 0, n = (int)uniformUI->Childs.size(); i < n; i++)
				{
					if (uniformUI->Childs[i]->UniformInfo != NULL)
						addUniformUI(ui, layout, material, shader, uniformUI->Childs[i], tab + 1, subjects, onChange);
				}
			}
			else if (uniformUI->ControlType == CShader::UIColor)
			{
				CMaterial::SUniformValue* uniform = material->getUniform(uniformUI->Name.c_str());
				std::wstring name = CStringImp::convertUTF8ToUnicode(uniformUI->Name.c_str());

				// default color
				SColorf c(
					uniform->FloatValue[0],
					uniform->FloatValue[1],
					uniform->FloatValue[2],
					uniform->FloatValue[3]
				);

				CSubject<SColor>* colorSubject = new CSubject<SColor>(c.toSColor());
				subjects.push_back(colorSubject);

				ui->addColorPicker(layout, name.c_str(), colorSubject);

				CObserver* observer = new CObserver();
				observer->Notify = [&, colorSubject, material, uniformUI](ISubject* subject, IObserver* from)
				{
					// on change color
					material->setUniform4(uniformUI->Name.c_str(), colorSubject->get());
					material->applyMaterial();
				};

				colorSubject->addObserver(observer);
			}
			else if (uniformUI->ControlType == CShader::UIFloat2)
			{
				CMaterial::SUniformValue* uniform = material->getUniform(uniformUI->Name.c_str());
				std::wstring name = CStringImp::convertUTF8ToUnicode(uniformUI->Name.c_str());

				CSubject<float>* x = new CSubject<float>(uniform->FloatValue[0]);
				subjects.push_back(x);

				CSubject<float>* y = new CSubject<float>(uniform->FloatValue[1]);
				subjects.push_back(y);

				if (uniformUI->ElementName.size() > 0)
				{
					// write element name #0
					name = CStringImp::convertUTF8ToUnicode(uniformUI->ElementName[0].c_str());
					if (uniformUI->Min > FLT_MIN || uniformUI->Max < FLT_MAX)
						ui->addSlider(layout, name.c_str(), x, uniformUI->Min, uniformUI->Max);
					else
						ui->addNumberInput(layout, name.c_str(), x, 0.1f);

					// write element name #1
					if (uniformUI->ElementName.size() >= 2)
					{
						name = CStringImp::convertUTF8ToUnicode(uniformUI->ElementName[1].c_str());

						if (uniformUI->Min > FLT_MIN || uniformUI->Max < FLT_MAX)
							ui->addSlider(layout, name.c_str(), y, uniformUI->Min, uniformUI->Max);
						else
							ui->addNumberInput(layout, name.c_str(), y, 0.1f);
					}
				}
				else
				{
					// write default X,Y
					ui->addLabel(layout, name.c_str());

					if (uniformUI->Min > FLT_MIN || uniformUI->Max < FLT_MAX)
					{
						ui->addSlider(layout, L"X", x, uniformUI->Min, uniformUI->Max);
						ui->addSlider(layout, L"Y", y, uniformUI->Min, uniformUI->Max);
					}
					else
					{
						ui->addNumberInput(layout, L"X", x, 0.1f);
						ui->addNumberInput(layout, L"Y", y, 0.1f);
					}
				}

				// event change value
				CObserver* observer = new CObserver();
				observer->Notify = [&, x, y, material, uniformUI](ISubject* subject, IObserver* from)
				{
					float value[2];
					value[0] = x->get();
					value[1] = y->get();

					// on change color
					material->setUniform2(uniformUI->Name.c_str(), value);
					material->applyMaterial();
				};

				x->addObserver(observer);
				y->addObserver(observer);
			}
			else if (uniformUI->ControlType == CShader::UIFloat4)
			{
				CMaterial::SUniformValue* uniform = material->getUniform(uniformUI->Name.c_str());
				std::wstring name = CStringImp::convertUTF8ToUnicode(uniformUI->Name.c_str());

				CSubject<float>* x = new CSubject<float>(uniform->FloatValue[0]);
				subjects.push_back(x);

				CSubject<float>* y = new CSubject<float>(uniform->FloatValue[1]);
				subjects.push_back(y);

				CSubject<float>* z = new CSubject<float>(uniform->FloatValue[2]);
				subjects.push_back(z);

				CSubject<float>* w = new CSubject<float>(uniform->FloatValue[3]);
				subjects.push_back(w);

				if (uniformUI->ElementName.size() > 0)
				{
					// write element name #0
					name = CStringImp::convertUTF8ToUnicode(uniformUI->ElementName[0].c_str());
					if (uniformUI->Min > FLT_MIN || uniformUI->Max < FLT_MAX)
						ui->addSlider(layout, name.c_str(), x, uniformUI->Min, uniformUI->Max);
					else
						ui->addNumberInput(layout, name.c_str(), x, 0.1f);

					// write element name #1
					if (uniformUI->ElementName.size() >= 2)
					{
						name = CStringImp::convertUTF8ToUnicode(uniformUI->ElementName[1].c_str());

						if (uniformUI->Min > FLT_MIN || uniformUI->Max < FLT_MAX)
							ui->addSlider(layout, name.c_str(), y, uniformUI->Min, uniformUI->Max);
						else
							ui->addNumberInput(layout, name.c_str(), y, 0.1f);
					}

					// write element name #2
					if (uniformUI->ElementName.size() >= 3)
					{
						name = CStringImp::convertUTF8ToUnicode(uniformUI->ElementName[2].c_str());

						if (uniformUI->Min > FLT_MIN || uniformUI->Max < FLT_MAX)
							ui->addSlider(layout, name.c_str(), z, uniformUI->Min, uniformUI->Max);
						else
							ui->addNumberInput(layout, name.c_str(), z, 0.1f);
					}

					// write element name #3
					if (uniformUI->ElementName.size() >= 4)
					{
						name = CStringImp::convertUTF8ToUnicode(uniformUI->ElementName[3].c_str());

						if (uniformUI->Min > FLT_MIN || uniformUI->Max < FLT_MAX)
							ui->addSlider(layout, name.c_str(), w, uniformUI->Min, uniformUI->Max);
						else
							ui->addNumberInput(layout, name.c_str(), w, 0.1f);
					}
				}
				else
				{
					// write default X,Y
					ui->addLabel(layout, name.c_str());

					if (uniformUI->Min > FLT_MIN || uniformUI->Max < FLT_MAX)
					{
						ui->addSlider(layout, L"X", x, uniformUI->Min, uniformUI->Max);
						ui->addSlider(layout, L"Y", y, uniformUI->Min, uniformUI->Max);
						ui->addSlider(layout, L"Z", z, uniformUI->Min, uniformUI->Max);
						ui->addSlider(layout, L"W", w, uniformUI->Min, uniformUI->Max);
					}
					else
					{
						ui->addNumberInput(layout, L"X", x, 0.1f);
						ui->addNumberInput(layout, L"Y", y, 0.1f);
						ui->addNumberInput(layout, L"Z", z, 0.1f);
						ui->addNumberInput(layout, L"W", w, 0.1f);
					}
				}

				// event change value
				CObserver* observer = new CObserver();
				observer->Notify = [&, x, y, z, w, material, uniformUI](ISubject* subject, IObserver* from)
				{
					float value[4];
					value[0] = x->get();
					value[1] = y->get();
					value[2] = z->get();
					value[3] = w->get();

					// on change color
					material->setUniform2(uniformUI->Name.c_str(), value);
					material->applyMaterial();
				};

				x->addObserver(observer);
				y->addObserver(observer);
				z->addObserver(observer);
				w->addObserver(observer);
			}
			else if (uniformUI->ControlType == CShader::UITexture)
			{
				CMaterial::SUniformTexture* uniformTexture = material->getUniformTexture(uniformUI->Name.c_str());

				CSubject<std::string>* newSubject = new CSubject<std::string>(uniformTexture->Path);
				subjects.push_back(newSubject);

				GUI::CImageButton* imageButton = ui->addInputTextureFile(layout, text, newSubject);

				if (uniformTexture->Texture != NULL)
				{
					const core::dimension2du& size = uniformTexture->Texture->getSize();
					imageButton->getImage()->setImage(
						uniformTexture->Texture,
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

				imageButton->OnDrop = [imageButton, uniformName = uniformUI->Name, material, onChange](GUI::SDragDropPackage* data, float mouseX, float mouseY)
				{
					if (data->Name == "ListFSItem")
					{
						GUI::CListRowItem* item = (GUI::CListRowItem*)data->Control;

						std::string fullPath = item->getTagString();
						std::string shortPath = CAssetManager::getInstance()->getShortPath(fullPath.c_str());
						std::string name = CPath::getFileName(shortPath);

						ITexture* texture = CTextureManager::getInstance()->getTexture(shortPath.c_str());
						if (texture != NULL)
						{
							// set texture
							CMaterial::SUniformTexture* uniform = material->getUniformTexture(uniformName.c_str());
							uniform->Path = shortPath;
							uniform->Texture = texture;
							material->autoDetectLoadTexture();
							material->applyMaterial();

							// update gui
							const core::dimension2du& size = texture->getSize();
							imageButton->getImage()->setImage(
								texture,
								GUI::SRect(0.0f, 0.0f, (float)size.Width, (float)size.Height)
							);
							imageButton->getImage()->setColor(GUI::SGUIColor(255, 255, 255, 255));

							// save
							onChange();
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

				imageButton->OnPress = [uniformName = uniformUI->Name, material](GUI::CBase* button)
				{
					CMaterial::SUniformTexture* uniform = material->getUniformTexture(uniformName.c_str());
					if (uniform->Texture)
					{
						std::string assetPath = uniform->Texture->getName().getPath().c_str();
						assetPath = CAssetManager::getInstance()->getShortPath(assetPath.c_str());
						CAssetPropertyController::getInstance()->browseAsset(assetPath.c_str());
					}
				};

				imageButton->OnRightPress = [uniformName = uniformUI->Name, material, imageButton, onChange](GUI::CBase* button)
				{
					GUI::SPoint mousePos = GUI::CInput::getInput()->getMousePosition();
					s_pickTextureMenu->open(mousePos);
					s_pickTextureMenu->OnCommand = [name = uniformName, material, imageButton, onChange](GUI::CBase* item)
					{
						// clear texture
						CMaterial::SUniformTexture* uniform = material->getUniformTexture(name.c_str());
						uniform->Path = "";
						uniform->Texture = NULL;

						// clear gui
						imageButton->getImage()->setImage(NULL, GUI::SRect(0.0f, 0.0f, 0.0f, 0.0f));
						imageButton->getImage()->setColor(GUI::SGUIColor(255, 0, 0, 0));

						// save
						onChange();
					};
				};
			}
		}
	}
}