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
#include "CDefaultEditor.h"

#include "Editor/Space/Property/CSpaceProperty.h"
#include "GameObject/CGameObject.h"

namespace Skylicht
{
	namespace Editor
	{
		CDefaultEditor::CDefaultEditor() :
			m_data(NULL)
		{

		}

		CDefaultEditor::~CDefaultEditor()
		{
			for (size_t i = 0, n = m_subjects.size(); i < n; i++)
				delete m_subjects[i];
			m_subjects.clear();

			if (m_data != NULL)
				delete m_data;
		}

		void CDefaultEditor::initGUI(CComponentSystem* target, CSpaceProperty* ui)
		{
			m_component = target;
			m_gameObject = target->getGameObject();
			m_data = target->createSerializable();

			if (m_gameObject->isEnableEditorChange() && m_component != NULL)
			{
				// setup gui
				std::string name = m_component->getTypeName().c_str();
				if (!m_name.empty())
					name = m_name;

				GUI::CCollapsibleGroup* group = ui->addGroup(name.c_str(), this);

				GUI::CBoxLayout* layout = ui->createBoxLayout(group);

				// add serializable data control
				for (u32 i = 0, n = m_data->getNumProperty(); i < n; i++)
				{
					CValueProperty* valueProperty = m_data->getPropertyID(i);

					if (valueProperty->getType() == EPropertyDataType::Bool)
					{
						CBoolProperty* value = dynamic_cast<CBoolProperty*>(valueProperty);
						CSubject<bool>* subject = new CSubject<bool>(value->get());
						subject->addObserver(new CObserver<CDefaultEditor>(this, [&, value, s = subject](ISubject* subject, IObserver* from, CDefaultEditor* target)
							{
								value->set(s->get());
								m_component->loadSerializable(m_data);
							}), true);

						ui->addCheckBox(layout, getPrettyName(value->Name), subject);
						m_subjects.push_back(subject);
					}
					else if (valueProperty->getType() == EPropertyDataType::Float)
					{
						CFloatProperty* value = dynamic_cast<CFloatProperty*>(valueProperty);
						CSubject<float>* subject = new CSubject<float>(value->get());
						CObserver<CDefaultEditor>* observer = new CObserver<CDefaultEditor>(this);
						observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from, CDefaultEditor* target)
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
								s->set(v);

								if (notifyUI)
									s->notify(o);

								m_component->loadSerializable(m_data);
							}
						};
						subject->addObserver(observer, true);
						ui->addNumberInput(layout, getPrettyName(value->Name), subject, 0.01f);
						m_subjects.push_back(subject);
					}
					else if (valueProperty->getType() == EPropertyDataType::Integer)
					{
						CIntProperty* value = dynamic_cast<CIntProperty*>(valueProperty);
						CSubject<int>* subject = new CSubject<int>(value->get());
						CObserver<CDefaultEditor>* observer = new CObserver<CDefaultEditor>(this);
						observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from, CDefaultEditor* target)
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
								s->set(v);

								if (notifyUI)
									s->notify(o);

								m_component->loadSerializable(m_data);
							}
						};
						subject->addObserver(observer, true);
						ui->addNumberInput(layout, getPrettyName(value->Name), subject);
						m_subjects.push_back(subject);
					}
					else if (valueProperty->getType() == EPropertyDataType::UInteger)
					{
						CUIntProperty* value = dynamic_cast<CUIntProperty*>(valueProperty);
						CSubject<u32>* subject = new CSubject<u32>(value->get());
						CObserver<CDefaultEditor>* observer = new CObserver<CDefaultEditor>(this);
						observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from, CDefaultEditor* target)
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
								s->set(v);

								if (notifyUI)
									s->notify(o);

								m_component->loadSerializable(m_data);
							}
						};
						subject->addObserver(observer, true);
						ui->addNumberInput(layout, getPrettyName(value->Name), subject);
						m_subjects.push_back(subject);
					}
					else if (valueProperty->getType() == EPropertyDataType::String)
					{

					}
					else if (valueProperty->getType() == EPropertyDataType::StringW)
					{

					}
				}

				group->setExpand(true);
			}
		}

		void CDefaultEditor::update()
		{

		}

		const wchar_t* CDefaultEditor::getPrettyName(const std::string& name)
		{
			char prettyName[512] = { 0 };
			int j = 0;
			bool lastCharIsUpper = false;

			for (size_t i = 0, j = 0, n = name.length(); i < n; i++, j++)
			{
				char c = name[i];

				if (i == 0)
				{
					prettyName[j] = toupper(c);
				}
				else if (isupper(c) && !lastCharIsUpper)
				{
					prettyName[j++] = ' ';
					prettyName[j] = c;
				}
				else
				{
					prettyName[j] = c;
				}

				lastCharIsUpper = isupper(c);
			}

			m_tempName = CStringImp::convertUTF8ToUnicode(prettyName);
			return m_tempName.c_str();
		}
	}
}