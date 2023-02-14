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

		void CSerializableEditor::serializableToControl(CObjectSerializable* object, CSpaceProperty* ui, GUI::CBoxLayout* layout)
		{
			// add serializable data control
			for (u32 i = 0, n = object->getNumProperty(); i < n; i++)
			{
				CValueProperty* valueProperty = object->getPropertyID(i);
				if (valueProperty->isHidden())
					continue;

				if (valueProperty->getType() == EPropertyDataType::Bool)
				{
					CBoolProperty* value = dynamic_cast<CBoolProperty*>(valueProperty);
					CSubject<bool>* subject = new CSubject<bool>(value->get());
					subject->addObserver(new CObserver([&, value, s = subject](ISubject* subject, IObserver* from)
						{
							value->set(s->get());
							onUpdateValue(object);
						}), true);

					ui->addCheckBox(layout, ui->getPrettyName(value->Name), subject);
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
						observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
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
						ui->addSlider(layout, ui->getPrettyName(value->Name), subject, value->Min, value->Max);
					}
					else
					{
						CSubject<float>* subject = new CSubject<float>(value->get());
						CObserver* observer = new CObserver();
						observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
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
						ui->addNumberInput(layout, ui->getPrettyName(value->Name), subject, 0.01f);
						m_subjects.push_back(subject);
					}
				}
				else if (valueProperty->getType() == EPropertyDataType::Integer)
				{
					CIntProperty* value = dynamic_cast<CIntProperty*>(valueProperty);
					CSubject<int>* subject = new CSubject<int>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
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
					ui->addNumberInput(layout, ui->getPrettyName(value->Name), subject);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::UInteger)
				{
					CUIntProperty* value = dynamic_cast<CUIntProperty*>(valueProperty);
					CSubject<u32>* subject = new CSubject<u32>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
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
					ui->addNumberInput(layout, ui->getPrettyName(value->Name), subject);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::String)
				{
					CStringProperty* value = dynamic_cast<CStringProperty*>(valueProperty);
					std::wstring stringValue = CStringImp::convertUTF8ToUnicode(value->get().c_str());

					CSubject<std::wstring>* subject = new CSubject<std::wstring>(stringValue);

					CObserver* observer = new CObserver();
					observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
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
					ui->addTextBox(layout, ui->getPrettyName(value->Name), subject);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::Color)
				{
					CColorProperty* value = dynamic_cast<CColorProperty*>(valueProperty);
					CSubject<SColor>* subject = new CSubject<SColor>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
					{
						if (from != o)
						{
							const SColor& color = s->get();
							value->set(color);
							onUpdateValue(object);
						}
					};

					subject->addObserver(observer, true);
					ui->addColorPicker(layout, ui->getPrettyName(value->Name), subject);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::FilePath)
				{
					CFilePathProperty* value = dynamic_cast<CFilePathProperty*>(valueProperty);
					CSubject<std::string>* subject = new CSubject<std::string>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
					{
						if (from != o)
						{
							const std::string& path = s->get();
							value->set(path);
							onUpdateValue(object);
						}
					};

					subject->addObserver(observer, true);
					ui->addInputFile(layout, ui->getPrettyName(value->Name), subject, value->Exts);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::FolderPath)
				{
					CFolderPathProperty* value = dynamic_cast<CFolderPathProperty*>(valueProperty);
					CSubject<std::string>* subject = new CSubject<std::string>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
					{
						if (from != o)
						{
							const std::string& path = s->get();
							value->set(path);
							onUpdateValue(object);
						}
					};

					subject->addObserver(observer, true);
					ui->addInputFolder(layout, ui->getPrettyName(value->Name), subject);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::Enum)
				{
					CEnumPropertyData* enumValue = dynamic_cast<CEnumPropertyData*>(valueProperty);

					GUI::CDropdownBox* dropBox = ui->addDropBox(layout, ui->getPrettyName(valueProperty->Name), L"");
					GUI::CMenu* menu = dropBox->getMenu();

					int currentValue = enumValue->getIntValue();

					int enumCount = enumValue->getEnumCount();
					for (int i = 0; i < enumCount; i++)
					{
						const CEnumPropertyData::SEnumString& enumData = enumValue->getEnum(i);

						std::wstring enumName = CStringImp::convertUTF8ToUnicode(enumData.Name.c_str());

						GUI::CMenuItem* item = menu->addItem(enumName);
						item->tagInt(enumData.Value);

						if (enumData.Value == currentValue)
							dropBox->setLabel(enumName);

						item->OnPress = [&, item, enumValue, dropBox, ui](GUI::CBase* base)
						{
							enumValue->setIntValue(item->getTagInt());

							onUpdateValue(object);

							dropBox->setLabel(item->getLabel());
							ui->getWindow()->getCanvas()->closeMenu();
						};
					}
				}
				else if (valueProperty->getType() == EPropertyDataType::Object)
				{
					CObjectSerializable* object = (CObjectSerializable*)valueProperty;
					CSubject<CObjectSerializable*>* subject = new CSubject<CObjectSerializable*>(object);

					// header
					GUI::CCollapsibleGroup* group = ui->addSubGroup(layout);
					group->getHeader()->setLabel(ui->getPrettyName(valueProperty->Name));
					GUI::CBoxLayout* objectLayout = ui->createBoxLayout(group);

					if (object->isArray())
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
							observer->Notify = [&, arrayObject, count, o = observer, objectLayout, group, ui](ISubject* subject, IObserver* from)
							{
								if (from != o)
								{
									int numElement = count->get();
									if (arrayObject->resize(numElement))
									{
										// remove old ui
										objectLayout->getChild(0)->removeAllChildren();

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

					// show child data
					serializableToControl(object, ui, objectLayout);

					m_subjects.push_back(subject);
				}
			}
		}
	}
}