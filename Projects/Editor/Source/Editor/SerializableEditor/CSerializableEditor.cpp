/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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
#include "CSerializableEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/SpaceController/CAssetPropertyController.h"
#include "Editor/CEditor.h"
#include "GUI/Input/CInput.h"
#include "AssetManager/CAssetManager.h"
#include "Graphics2D/SpriteFrame/CSpriteManager.h"
#include "ResourceSettings/CTextureSettings.h"
#include "Serializable/CSerializableLoader.h"

namespace Skylicht
{
	namespace Editor
	{
		CSerializableEditor::CSerializableEditor()
		{

		}

		CSerializableEditor::~CSerializableEditor()
		{

		}

		void CSerializableEditor::closeGUI()
		{
			for (size_t i = 0, n = m_subjects.size(); i < n; i++)
				delete m_subjects[i];
			m_subjects.clear();
		}

		GUI::CMenu* s_pickTextureMenu = NULL;
		GUI::CMenu* s_pickFileMenu = NULL;

		void initPickTextureMenu()
		{
			CEditor* editor = CEditor::getInstance();
			if (s_pickTextureMenu == NULL)
			{
				s_pickTextureMenu = new GUI::CMenu(editor->getRootCanvas());
				s_pickTextureMenu->addItem(L"Clear Texture", GUI::ESystemIcon::Close);
			}

			if (s_pickFileMenu == NULL)
			{
				s_pickFileMenu = new GUI::CMenu(editor->getRootCanvas());
				s_pickFileMenu->addItem(L"Clear Resource", GUI::ESystemIcon::Close);
			}
		}

		void CSerializableEditor::serializableToControl(CObjectSerializable* object, CSpaceProperty* ui, GUI::CBoxLayout* layout)
		{
			initPickTextureMenu();

			// add serializable data control
			for (u32 i = 0, n = object->getNumProperty(); i < n; i++)
			{
				CValueProperty* valueProperty = object->getPropertyID(i);
				if (valueProperty->isHidden())
					continue;

				// add ui space
				if (valueProperty->getUISpace() > 0)
				{
					float space = valueProperty->getUISpace();
					if (space > 100.0f)
						space = 100.0f;
					layout->addSpace(space);
				}

				// add ui header
				if (valueProperty->getUIHeader().size() > 0)
				{
					std::wstring label = CStringImp::convertUTF8ToUnicode(valueProperty->getUIHeader().c_str());
					ui->addLabel(layout, label.c_str(), GUI::TextLeft);
				}

				if (valueProperty->getType() == EPropertyDataType::Vector3)
				{
					CVector3Property* value = dynamic_cast<CVector3Property*>(valueProperty);
					const core::vector3df& v = value->get();

					CSubject<float>* X = new CSubject<float>(v.X);
					CSubject<float>* Y = new CSubject<float>(v.Y);
					CSubject<float>* Z = new CSubject<float>(v.Z);

					CObserver* observer = new CObserver();
					observer->Notify = [&, object, value, X, Y, Z, observer](ISubject* subject, IObserver* from)
						{
							if (from != observer)
							{
								core::vector3df v = value->get();
								v.X = X->get();
								v.Y = Y->get();
								v.Z = Z->get();
								value->set(v);
								onUpdateValue(object);
							}
						};

					X->addObserver(observer);
					Y->addObserver(observer);
					Z->addObserver(observer, true);

					valueProperty->OnSetHidden = [X, Y, Z, o = observer](bool hide)
						{
							X->setEnable(!hide);
							X->notify(o);

							Y->setEnable(!hide);
							Y->notify(o);

							Z->setEnable(!hide);
							Z->notify(o);
						};

					valueProperty->OnChanged = [value, X, Y, Z, observer]()
						{
							X->set(value->get().X);
							X->notify(observer);

							Y->set(value->get().Y);
							Y->notify(observer);

							Z->set(value->get().Z);
							Z->notify(observer);
						};

					std::wstring name = ui->getPrettyName(value->Name);
					name += L" X";

					ui->addNumberInput(layout, name.c_str(), X, 0.1f);
					ui->addNumberInput(layout, L"Y", Y, 0.1f);
					ui->addNumberInput(layout, L"Z", Z, 0.1f);
					layout->addSpace(5.0f);

					m_subjects.push_back(X);
					m_subjects.push_back(Y);
					m_subjects.push_back(Z);
				}
				else if (valueProperty->getType() == EPropertyDataType::Bool)
				{
					CBoolProperty* value = dynamic_cast<CBoolProperty*>(valueProperty);
					CSubject<bool>* subject = new CSubject<bool>(value->get());
					CObserver* observer = new CObserver([&, object, value, s = subject](ISubject* subject, IObserver* from)
						{
							value->set(s->get());
							onUpdateValue(object);
						});
					subject->addObserver(observer, true);

					valueProperty->OnSetHidden = [s = subject, o = observer](bool hide)
						{
							s->setEnable(!hide);
							s->notify(o);
						};

					ui->addCheckBox(layout, ui->getPrettyName(value->Name), subject);

					valueProperty->OnChanged = [value, subject, observer]()
						{
							subject->set(value->get());
							subject->notify(observer);
						};

					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::Float)
				{
					CFloatProperty* value = dynamic_cast<CFloatProperty*>(valueProperty);
					if (value->ClampMin && value->ClampMax)
					{
						// add slider control on Limit Float
						CSubject<float>* subject = new CSubject<float>(value->get());
						CObserver* observer = new CObserver();
						observer->Notify = [&, object, value, s = subject, o = observer](ISubject* subject, IObserver* from)
							{
								if (from != o)
								{
									float v = s->get();
									value->set(v);
									s->set(v);
									onUpdateValue(object);
								}
							};
						subject->addObserver(observer, true);

						valueProperty->OnSetHidden = [s = subject, o = observer](bool hide)
							{
								s->setEnable(!hide);
								s->notify(o);
							};

						valueProperty->OnChanged = [value, subject, observer]()
							{
								subject->set(value->get());
								subject->notify(observer);
							};

						ui->addSlider(layout, ui->getPrettyName(value->Name), subject, value->Min, value->Max);
						m_subjects.push_back(subject);
					}
					else
					{
						CSubject<float>* subject = new CSubject<float>(value->get());
						CObserver* observer = new CObserver();
						observer->Notify = [&, object, value, s = subject, o = observer](ISubject* subject, IObserver* from)
							{
								if (from != o)
								{
									float v = s->get();
									bool notifyUI = false;

									if (value->ClampMin && v < value->Min)
									{
										v = value->Min;
										notifyUI = true;
									}
									if (value->ClampMax && v > value->Max)
									{
										v = value->Max;
										notifyUI = true;
									}

									value->set(v);

									if (notifyUI)
									{
										s->set(v);
										s->notify(o);
									}

									onUpdateValue(object);
								}
							};
						subject->addObserver(observer, true);

						valueProperty->OnSetHidden = [s = subject, o = observer](bool hide)
							{
								s->setEnable(!hide);
								s->notify(o);
							};

						valueProperty->OnChanged = [value, subject, observer]()
							{
								subject->set(value->get());
								subject->notify(observer);
							};

						ui->addNumberInput(layout, ui->getPrettyName(value->Name), subject, 0.01f);
						m_subjects.push_back(subject);
					}
				}
				else if (valueProperty->getType() == EPropertyDataType::Integer)
				{
					CIntProperty* value = dynamic_cast<CIntProperty*>(valueProperty);
					CSubject<int>* subject = new CSubject<int>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, object, value, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								int v = s->get();
								bool notifyUI = false;

								if (value->ClampMin && v < value->Min)
								{
									v = value->Min;
									notifyUI = true;
								}
								if (value->ClampMax && v > value->Max)
								{
									v = value->Max;
									notifyUI = true;
								}

								value->set(v);

								if (notifyUI)
								{
									s->set(v);
									s->notify(o);
								}

								onUpdateValue(object);
							}
						};
					subject->addObserver(observer, true);

					valueProperty->OnSetHidden = [s = subject, o = observer](bool hide)
						{
							s->setEnable(!hide);
							s->notify(o);
						};

					valueProperty->OnChanged = [value, subject, observer]()
						{
							subject->set(value->get());
							subject->notify(observer);
						};

					ui->addNumberInput(layout, ui->getPrettyName(value->Name), subject);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::UInteger)
				{
					CUIntProperty* value = dynamic_cast<CUIntProperty*>(valueProperty);
					CSubject<u32>* subject = new CSubject<u32>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, object, value, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								u32 v = s->get();
								bool notifyUI = false;

								if (value->ClampMax && v > value->Max)
								{
									v = value->Max;
									notifyUI = true;
								}

								value->set(v);

								if (notifyUI)
								{
									s->set(v);
									s->notify(o);
								}

								onUpdateValue(object);
							}
						};
					subject->addObserver(observer, true);

					valueProperty->OnSetHidden = [s = subject, o = observer](bool hide)
						{
							s->setEnable(!hide);
							s->notify(o);
						};

					valueProperty->OnChanged = [value, subject, observer]()
						{
							subject->set(value->get());
							subject->notify(observer);
						};

					ui->addNumberInput(layout, ui->getPrettyName(value->Name), subject);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::String)
				{
					CStringProperty* value = dynamic_cast<CStringProperty*>(valueProperty);
					std::wstring stringValue = CStringImp::convertUTF8ToUnicode(value->get().c_str());

					CSubject<std::wstring>* subject = new CSubject<std::wstring>(stringValue);

					CObserver* observer = new CObserver();
					observer->Notify = [&, object, value, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								const std::wstring& stringValue = s->get();
								std::string stringValueA = CStringImp::convertUnicodeToUTF8(stringValue.c_str());
								value->set(stringValueA);
								onUpdateValue(object);
							}
						};
					subject->addObserver(observer, true);

					valueProperty->OnSetHidden = [s = subject, o = observer](bool hide)
						{
							s->setEnable(!hide);
							s->notify(o);
						};

					ui->addTextBox(layout, ui->getPrettyName(value->Name), subject);

					valueProperty->OnChanged = [value, subject, observer]()
						{
							std::wstring stringValue = CStringImp::convertUTF8ToUnicode(value->get().c_str());
							subject->set(stringValue);
							subject->notify(observer);
						};

					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::Color)
				{
					CColorProperty* value = dynamic_cast<CColorProperty*>(valueProperty);
					CSubject<SColor>* subject = new CSubject<SColor>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, object, value, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								const SColor& color = s->get();
								value->set(color);
								onUpdateValue(object);
							}
						};
					subject->addObserver(observer, true);

					valueProperty->OnSetHidden = [s = subject, o = observer](bool hide)
						{
							s->setEnable(!hide);
							s->notify(o);
						};

					ui->addColorPicker(layout, ui->getPrettyName(value->Name), subject);

					valueProperty->OnChanged = [value, subject, observer]()
						{
							subject->set(value->get());
							subject->notify(observer);
						};

					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::FilePath)
				{
					CFilePathProperty* value = dynamic_cast<CFilePathProperty*>(valueProperty);
					CSubject<std::string>* subject = new CSubject<std::string>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, object, value, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								const std::string& path = s->get();
								value->set(path);
								onUpdateValue(object);
							}
						};

					subject->addObserver(observer, true);

					valueProperty->OnSetHidden = [s = subject, o = observer](bool hide)
						{
							s->setEnable(!hide);
							s->notify(o);
						};

					GUI::CInputResourceBox* input = ui->addInputFile(layout, ui->getPrettyName(value->Name), subject, value->Exts);
					input->OnRightMouseClick = [&, s = subject, o = observer](GUI::CBase* button, float x, float y, bool b)
						{
							GUI::SPoint mousePos = GUI::CInput::getInput()->getMousePosition();
							s_pickFileMenu->open(mousePos);
							s_pickFileMenu->OnCommand = [&, s, o](GUI::CBase* item)
								{
									s->set("");
									s->notify(NULL);
								};
						};

					valueProperty->OnChanged = [value, subject, observer]()
						{
							subject->set(value->get());
							subject->notify(observer);
						};

					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::FolderPath)
				{
					CFolderPathProperty* value = dynamic_cast<CFolderPathProperty*>(valueProperty);
					CSubject<std::string>* subject = new CSubject<std::string>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, object, value, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								const std::string& path = s->get();
								value->set(path);
								onUpdateValue(object);
							}
						};

					subject->addObserver(observer, true);

					valueProperty->OnSetHidden = [s = subject, o = observer](bool hide)
						{
							s->setEnable(!hide);
							s->notify(o);
						};

					GUI::CInputResourceBox* input = ui->addInputFolder(layout, ui->getPrettyName(value->Name), subject);

					input->OnRightMouseClick = [&, s = subject, o = observer](GUI::CBase* button, float x, float y, bool b)
						{
							GUI::SPoint mousePos = GUI::CInput::getInput()->getMousePosition();
							s_pickFileMenu->open(mousePos);
							s_pickFileMenu->OnCommand = [&, s, o](GUI::CBase* item)
								{
									s->set("");
									s->notify(NULL);
								};
						};

					valueProperty->OnChanged = [value, subject, observer]()
						{
							subject->set(value->get());
							subject->notify(observer);
						};

					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::Enum)
				{
					CEnumPropertyData* enumValue = dynamic_cast<CEnumPropertyData*>(valueProperty);

					GUI::CDropdownBox* dropBox = ui->addDropBox(layout, ui->getPrettyName(valueProperty->Name), L"");
					GUI::CMenu* menu = dropBox->getMenu();

					int currentValue = enumValue->getIntValue();
					int enumCount = enumValue->getEnumCount();

					valueProperty->OnSetHidden = [dropBox](bool hide)
						{
							dropBox->getParent()->setHidden(hide);
						};

					for (int i = 0; i < enumCount; i++)
					{
						const CEnumPropertyData::SEnumString& enumData = enumValue->getEnum(i);

						std::wstring enumName = CStringImp::convertUTF8ToUnicode(enumData.Name.c_str());

						GUI::CMenuItem* item = menu->addItem(enumName);
						item->tagInt(enumData.Value);

						if (enumData.Value == currentValue)
							dropBox->setLabel(enumName);

						item->OnPress = [&, object, item, enumValue, dropBox, ui](GUI::CBase* base)
							{
								enumValue->setIntValue(item->getTagInt());

								onUpdateValue(object);

								dropBox->setLabel(item->getLabel());
								ui->getWindow()->getCanvas()->closeMenu();
								ui->onEndEditValue(item);
								ui->focus();
							};
					}

					valueProperty->OnChanged = [dropBox, enumValue]()
						{
							int currentValue = enumValue->getIntValue();
							CEnumPropertyData::SEnumString* enumData = enumValue->getEnumByValue(currentValue);
							if (enumData)
							{
								std::wstring enumName = CStringImp::convertUTF8ToUnicode(enumData->Name.c_str());
								dropBox->setLabel(enumName);
							}
						};
				}
				else if (valueProperty->getType() == EPropertyDataType::Object)
				{
					CObjectSerializable* object = (CObjectSerializable*)valueProperty;
					CSubject<CObjectSerializable*>* subject = new CSubject<CObjectSerializable*>(object);

					bool haveChildGroup = true;

					if (object->getObjectType() == ObjectInterpolate)
						haveChildGroup = false;

					GUI::CCollapsibleGroup* group = NULL;
					GUI::CBoxLayout* objectLayout = NULL;

					if (haveChildGroup)
					{
						// header
						group = ui->addSubGroup(layout);
						group->getHeader()->setLabel(ui->getPrettyName(valueProperty->Name));
						objectLayout = ui->createBoxLayout(group);

						valueProperty->OnSetHidden = [group](bool hide)
							{
								group->setHidden(hide);
							};
					}

					if (object->getObjectType() == ObjectArray ||
						object->getObjectType() == FileArray ||
						object->getObjectType() == TextureArray)
					{
						CArraySerializable* arrayObject = (CArraySerializable*)object;

						if (arrayObject->haveCreateElementFunction())
						{
							// add input to add elements
							CSubject<int>* count = new CSubject<int>(arrayObject->getElementCount());
							m_subjects.push_back(count);

							ui->addNumberTextBox(objectLayout, L"Count", count);

							objectLayout = ui->createBoxLayout(group);

							CObserver* observer = new CObserver();
							observer->Notify = [&, object, arrayObject, count, o = observer, objectLayout, group, ui](ISubject* subject, IObserver* from)
								{
									if (from != o)
									{
										int numElement = count->get();
										if (arrayObject->resize(numElement))
										{
											// remove old ui
											objectLayout->getChild(0)->releaseChildren();

											// re-init ui
											serializableToControl(arrayObject, ui, objectLayout);

											// update object data
											onUpdateValue(object);
										}
									}
								};
							count->addObserver(observer, true);
						}
					}
					else if (object->getObjectType() == ObjectInterpolate)
					{
						CInterpolateFloatSerializable* value = (CInterpolateFloatSerializable*)valueProperty;

						CInterpolator interpolator;
						value->getInterpolator(&interpolator);

						CSubject<CInterpolator>* subject = new CSubject<CInterpolator>(interpolator);
						CObserver* observer = new CObserver();
						observer->Notify = [&, object, value, s = subject, o = observer](ISubject* subject, IObserver* from)
							{
								if (from != o)
								{
									value->setInterpolator(&s->get());
									onUpdateValue(object);
								}
							};
						subject->addObserver(observer, true);

						valueProperty->OnSetHidden = [s = subject, o = observer](bool hide)
							{
								s->setEnable(!hide);
								s->notify(o);
							};

						ui->addInterpolateCurves(layout, ui->getPrettyName(value->Name), subject);

						valueProperty->OnChanged = [value, subject, observer]()
							{
								CInterpolator v;
								value->getInterpolator(&v);
								subject->set(v);
								subject->notify(observer);
							};

						m_subjects.push_back(subject);
					}

					// show child data
					if (haveChildGroup)
						serializableToControl(object, ui, objectLayout);

					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::ImageSource)
				{
					CImageSourceProperty* value = dynamic_cast<CImageSourceProperty*>(valueProperty);
					CSubject<std::string>* subject = new CSubject<std::string>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, object, value, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								const std::string& path = s->get();

								// read guid
								std::string guid = CAssetManager::getInstance()->getGenerateMetaGUID(path.c_str());

								value->set(path);
								value->setGUID(guid.c_str());

								// apply image texture
								onUpdateValue(object);
							}
						};

					subject->addObserver(observer, true);

					valueProperty->OnSetHidden = [s = subject, o = observer](bool hide)
						{
							s->setEnable(!hide);
							s->notify(o);
						};

					GUI::CImageButton* imageButton = ui->addInputTextureFile(layout, ui->getPrettyName(value->Name), subject);

					// load texture and show on GUI
					ITexture* texture = CTextureManager::getInstance()->getTexture(value->get().c_str());
					if (texture != NULL)
					{
						const core::dimension2du& size = texture->getSize();
						imageButton->getImage()->setImage(
							texture,
							GUI::SRect(0.0f, 0.0f, (float)size.Width, (float)size.Height)
						);
						imageButton->getImage()->setColor(GUI::SGUIColor(255, 255, 255, 255));
					}

					// apply event drag/drop...
					valueProperty->OnChanged = [value, subject, observer]()
						{
							subject->set(value->get());
							subject->notify(observer);
						};

					m_subjects.push_back(subject);

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
									if (CTextureManager::isTextureExt(ext.c_str()))
									{
										return true;
									}
								}
							}
							return false;
						};

					imageButton->OnDrop = [&, imageButton, value, object](GUI::SDragDropPackage* data, float mouseX, float mouseY)
						{
							if (data->Name == "ListFSItem")
							{
								GUI::CListRowItem* item = (GUI::CListRowItem*)data->Control;

								std::string fullPath = item->getTagString();
								std::string shortPath = CAssetManager::getInstance()->getShortPath(fullPath.c_str());
								std::string name = CPath::getFileName(shortPath);

								// read guid
								std::string guid = CAssetManager::getInstance()->getGenerateMetaGUID(shortPath.c_str());

								ITexture* texture = CTextureManager::getInstance()->getTexture(shortPath.c_str());
								if (texture != NULL)
								{
									// update gui
									const core::dimension2du& size = texture->getSize();
									imageButton->getImage()->setImage(
										texture,
										GUI::SRect(0.0f, 0.0f, (float)size.Width, (float)size.Height)
									);
									imageButton->getImage()->setColor(GUI::SGUIColor(255, 255, 255, 255));

									// update value
									value->set(shortPath.c_str());
									value->setGUID(guid.c_str());

									onUpdateValue(object);
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

					imageButton->OnPress = [value](GUI::CBase* button)
						{
							std::string assetPath = value->get();
							assetPath = CAssetManager::getInstance()->getShortPath(assetPath.c_str());
							CAssetPropertyController::getInstance()->browseAsset(assetPath.c_str());
						};

					imageButton->OnRightPress = [&, imageButton, value, object](GUI::CBase* button)
						{
							GUI::SPoint mousePos = GUI::CInput::getInput()->getMousePosition();
							s_pickTextureMenu->open(mousePos);
							s_pickTextureMenu->OnCommand = [&, imageButton, value, object](GUI::CBase* item)
								{
									// clear
									imageButton->getImage()->setImage(NULL, GUI::SRect(0, 0, 0, 0));
									imageButton->getImage()->setColor(GUI::SGUIColor(255, 50, 50, 50));

									// update value
									value->set("");
									onUpdateValue(object);
								};
						};
				}
				else if (valueProperty->getType() == EPropertyDataType::FrameSource)
				{
					CFrameSourceProperty* value = dynamic_cast<CFrameSourceProperty*>(valueProperty);
					CSubject<std::string>* subject = new CSubject<std::string>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, object, value, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								const std::string& path = s->get();
								std::string meta = path + ".meta";

								CTextureSettings textureSetting;
								if (CSerializableLoader::loadSerializable(meta.c_str(), &textureSetting))
								{
									// update value
									value->setGUID(textureSetting.getGUID());
									value->setSprite(textureSetting.SpritePath.getString());

									// apply image texture
									onUpdateValue(object);
								}
							}
						};

					subject->addObserver(observer, true);

					valueProperty->OnSetHidden = [s = subject, o = observer](bool hide)
						{
							s->setEnable(!hide);
							s->notify(o);
						};

					GUI::CImageButton* imageButton = ui->addInputTextureFile(layout, ui->getPrettyName(value->Name), subject);

					// load texture and show on GUI
					std::string sprite = value->getSprite();
					std::string frameId = value->getGUID();

					CSpriteFrame* spriteFrame = CSpriteManager::getInstance()->loadSprite(sprite.c_str());
					if (spriteFrame)
					{
						SFrame* frame = spriteFrame->getFrameById(frameId.c_str());
						if (frame)
						{
							ITexture* texture = frame->Image->Texture;
							if (texture != NULL && frame->ModuleOffset.size() > 0)
							{
								SModuleRect* moduleRect = frame->ModuleOffset[0].Module;

								imageButton->getImage()->setImage(
									texture,
									GUI::SRect(moduleRect->X, moduleRect->Y, moduleRect->W, moduleRect->H)
								);
								imageButton->getImage()->setColor(GUI::SGUIColor(255, 255, 255, 255));
							}
						}
					}

					// apply event drag/drop...
					valueProperty->OnChanged = [value, subject, observer]()
						{
							subject->set(value->get());
							subject->notify(observer);
						};

					m_subjects.push_back(subject);

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
									if (CTextureManager::isTextureExt(ext.c_str()))
									{
										return true;
									}
								}
							}
							return false;
						};

					imageButton->OnDrop = [&, imageButton, value, object](GUI::SDragDropPackage* data, float mouseX, float mouseY)
						{
							if (data->Name == "ListFSItem")
							{
								GUI::CListRowItem* item = (GUI::CListRowItem*)data->Control;

								std::string fullPath = item->getTagString();
								std::string shortPath = CAssetManager::getInstance()->getShortPath(fullPath.c_str());
								std::string name = CPath::getFileName(shortPath);
								std::string meta = shortPath + ".meta";

								bool errorSprite = true;

								CTextureSettings textureSetting;
								if (CSerializableLoader::loadSerializable(meta.c_str(), &textureSetting))
								{
									// show on property
									CSpriteFrame* spriteFrame = CSpriteManager::getInstance()->loadSprite(textureSetting.SpritePath.getString());
									if (spriteFrame)
									{
										errorSprite = false;

										// update value
										value->set(shortPath.c_str());
										value->setGUID(textureSetting.getGUID());
										value->setSprite(textureSetting.SpritePath.getString());
										value->setSpriteId(textureSetting.SpriteId.getString());

										SFrame* frame = spriteFrame->getFrameById(textureSetting.getGUID());
										if (frame)
										{
											ITexture* texture = frame->Image->Texture;
											if (texture != NULL && frame->ModuleOffset.size() > 0)
											{
												SModuleRect* moduleRect = frame->ModuleOffset[0].Module;

												imageButton->getImage()->setImage(
													texture,
													GUI::SRect(moduleRect->X, moduleRect->Y, moduleRect->W, moduleRect->H)
												);
												imageButton->getImage()->setColor(GUI::SGUIColor(255, 255, 255, 255));
											}
										}
									}

									// show on canvas
									onUpdateValue(object);
								}

								if (errorSprite)
								{
									CEditor* editor = CEditor::getInstance();

									GUI::CMessageBox* msgBox = new GUI::CMessageBox(editor->getRootCanvas(), GUI::CMessageBox::OK);
									msgBox->setMessage("Image resource should build in a sprite atlas", name.c_str());
									msgBox->getMessageIcon()->setIcon(GUI::ESystemIcon::Alert);
								}
							}
						};

					imageButton->OnPress = [value](GUI::CBase* button)
						{
							std::string assetPath = value->get();
							std::string spritePath = value->getSprite();

							CAssetManager* assetManager = CAssetManager::getInstance();
							assetPath = assetManager->getShortPath(assetPath.c_str());
							if (assetManager->isExist(assetPath.c_str()))
							{
								CAssetPropertyController::getInstance()->browseAsset(assetPath.c_str());
							}
							else
							{
								CAssetPropertyController::getInstance()->browseAsset(spritePath.c_str());
							}
						};

					imageButton->OnRightPress = [&, imageButton, value, object](GUI::CBase* button)
						{
							GUI::SPoint mousePos = GUI::CInput::getInput()->getMousePosition();
							s_pickTextureMenu->open(mousePos);
							s_pickTextureMenu->OnCommand = [&, imageButton, value, object](GUI::CBase* item)
								{
									// clear
									imageButton->getImage()->setImage(NULL, GUI::SRect(0, 0, 0, 0));
									imageButton->getImage()->setColor(GUI::SGUIColor(255, 50, 50, 50));

									// update value
									value->set("");
									value->setGUID("");
									value->setSprite("");
									onUpdateValue(object);
								};
						};
				}

				initCustomDataGUI(object, valueProperty, layout, ui);
			}
		}
	}
}