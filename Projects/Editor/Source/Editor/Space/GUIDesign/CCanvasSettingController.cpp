/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CCanvasSettingController.h"
#include "Editor/SpaceController/CGUIDesignController.h"
#include "Editor/SpaceController/CAssetPropertyController.h"
#include "GUI/Utils/CTabableGroup.h"
#include "Localize/CLocalize.h"

namespace Skylicht
{
	namespace Editor
	{
		CCanvasSettingController::CCanvasSettingController(CEditor* editor, GUI::CMenu* menu) :
			m_menu(menu),
			m_editor(editor)
		{
			GUI::CBoxLayout* boxLayout = new GUI::CBoxLayout(menu);
			boxLayout->setPadding(GUI::SPadding(5.0, 5.0, -5.0, 5.0));
			boxLayout->dock(GUI::EPosition::Top);

			GUI::CLayout* layout;
			GUI::CLabel* label;

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setString(L"Canvas setting");
			label->setTextAlignment(GUI::TextLeft);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Width");
			label->setTextAlignment(GUI::TextRight);

			m_inputWidth = new GUI::CNumberInput(layout);
			m_inputWidth->setNumberType(GUI::UInteger);
			m_inputWidth->setValue(1920.0f, false);
			m_inputWidth->OnTextChanged = BIND_LISTENER(&CCanvasSettingController::onChanged, this);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Height");
			label->setTextAlignment(GUI::TextRight);

			m_inputHeight = new GUI::CNumberInput(layout);
			m_inputHeight->setNumberType(GUI::UInteger);
			m_inputHeight->setValue(1080.0f, false);
			m_inputHeight->OnTextChanged = BIND_LISTENER(&CCanvasSettingController::onChanged, this);
			boxLayout->endVertical();

			boxLayout->addSpace(5.0f);

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Draw GUI outline");
			label->setTextAlignment(GUI::TextRight);

			GUI::CBase* checkBoxContainer = new GUI::CBase(layout);
			m_checkboxOutline = new GUI::CCheckBox(checkBoxContainer);
			m_checkboxOutline->OnChanged = BIND_LISTENER(&CCanvasSettingController::onCheckBoxOutline, this);
			boxLayout->endVertical();

			boxLayout->addSpace(5.0f);

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Text localization");
			label->setTextAlignment(GUI::TextRight);

			CGUIDesignController* controller = CGUIDesignController::getInstance();

			GUI::CInputResourceBox* inputFileLocalize = new GUI::CInputResourceBox(layout);
			inputFileLocalize->showIcon(GUI::ESystemIcon::File);
			inputFileLocalize->showBrowseButton(true);
			inputFileLocalize->getBrowseButton()->OnPress = [&, canvas = editor->getRootCanvas(), controller, inputFileLocalize](GUI::CBase* btn)
				{
					std::string p = controller->getLocalizePath();
					std::string assetFolder = p.empty() ? CAssetManager::getInstance()->getAssetFolder() : CPath::getFolderPath(p);

					GUI::COpenSaveDialog* dialog = new GUI::COpenSaveDialog(canvas, GUI::COpenSaveDialog::Open, assetFolder.c_str(), assetFolder.c_str(), "xml;csv", true);
					dialog->OnOpen = [&, inputFileLocalize](std::string path)
						{
							path = CAssetManager::getInstance()->getShortPath(path.c_str());
							onChangeLocalize(path);

							std::string fileName = CPath::getFileName(path.c_str());
							if (fileName.size() > 0)
								inputFileLocalize->setString(CStringImp::convertUTF8ToUnicode(fileName.c_str()));
						};
				};

			std::string fileName = CPath::getFileName(controller->getLocalizePath().c_str());
			if (fileName.size() > 0)
				inputFileLocalize->setString(CStringImp::convertUTF8ToUnicode(fileName.c_str()));
			else
			{
				std::vector<std::string> exts;
				exts.push_back("xml");
				exts.push_back("csv");

				std::string extString = "None (";
				for (int i = 0, n = (int)exts.size(); i < n; i++)
				{
					extString += ".";
					extString += exts[i];
					if (i < n - 1)
						extString += ", ";
				}
				extString += ")";
				inputFileLocalize->setString(CStringImp::convertUTF8ToUnicode(extString.c_str()));
			}

			inputFileLocalize->OnPressed = [controller](GUI::CBase* base)
				{
					std::string path = controller->getLocalizePath();
					if (!path.empty())
						CAssetPropertyController::getInstance()->browseAsset(path.c_str());
				};
			boxLayout->endVertical();

			m_languageLayout = boxLayout->beginVertical();
			label = new GUI::CLabel(m_languageLayout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Language");
			label->setTextAlignment(GUI::TextRight);

			m_languageBox = new GUI::CComboBox(m_languageLayout);
			m_languageBox->OnChanged = [&, controller](GUI::CBase* base) {
				u32 index = m_languageBox->getSelectIndex();
				controller->setLanguage(index);
				};
			boxLayout->endVertical();
			m_languageLayout->setHidden(true);

			boxLayout->addSpace(20.0f);

			layout = boxLayout->beginVertical();
			GUI::CButton* defaultBtn = new GUI::CButton(layout);
			defaultBtn->setLabel(L"Cancel");
			defaultBtn->setTextAlignment(GUI::TextCenter);
			defaultBtn->OnPress = BIND_LISTENER(&CCanvasSettingController::onCancel, this);

			GUI::CButton* okBtn = new GUI::CButton(layout);
			okBtn->setLabel(L"OK");
			okBtn->setTextAlignment(GUI::TextCenter);
			okBtn->OnPress = BIND_LISTENER(&CCanvasSettingController::onOK, this);
			boxLayout->endVertical();

			menu->OnClose = BIND_LISTENER(&CCanvasSettingController::onMenuClose, this);
		}

		CCanvasSettingController::~CCanvasSettingController()
		{

		}

		void CCanvasSettingController::update()
		{

		}

		void CCanvasSettingController::onShow()
		{
			CScene* scene = CGUIDesignController::getInstance()->getScene();
			CGameObject* canvasObj = scene->searchObjectInChild(L"GUICanvas");
			if (canvasObj)
			{
				CCanvas* canvas = canvasObj->getComponent<CCanvas>();
				if (canvas)
				{
					CGUIElement* root = canvas->getRootElement();
					float w = root->getWidth();
					float h = root->getHeight();

					m_inputWidth->setValue(w, false);
					m_inputHeight->setValue(h, false);
					m_checkboxOutline->setToggle(canvas->DrawOutline);

					showLanguage();
				}
			}
			enableTabGroup();
		}

		void CCanvasSettingController::showLanguage()
		{
			CLocalize* localize = CLocalize::getInstance();
			if (localize->getLanguages().size() > 0)
			{
				std::vector<std::wstring> list;
				for (const std::string& lang : localize->getLanguages())
					list.push_back(CStringImp::convertUTF8ToUnicode(lang.c_str()));
				list.erase(list.begin());

				if (list.size() > 0)
				{
					m_languageBox->setListValue(list);
					m_languageBox->setSelectIndex(localize->getLanguage(), false);
					m_languageLayout->setHidden(false);
				}
			}
			else
			{
				m_languageLayout->setHidden(true);
			}
		}

		void CCanvasSettingController::enableTabGroup()
		{
			GUI::CTabableGroup& tabGroup = m_menu->getCanvas()->TabableGroup;
			tabGroup.push();
			tabGroup.clear();
			tabGroup.add(m_inputWidth);
			tabGroup.add(m_inputHeight);
		}

		void CCanvasSettingController::clearTabGroup()
		{
			GUI::CTabableGroup& tabGroup = m_menu->getCanvas()->TabableGroup;
			tabGroup.pop();
		}

		void CCanvasSettingController::onCancel(GUI::CBase* base)
		{
			m_menu->close();
		}

		void CCanvasSettingController::onMenuClose(GUI::CBase* base)
		{
			clearTabGroup();
		}

		void CCanvasSettingController::onOK(GUI::CBase* base)
		{
			float w = m_inputWidth->getValue();
			float h = m_inputHeight->getValue();

			CScene* scene = CGUIDesignController::getInstance()->getScene();
			CGameObject* canvasObj = scene->searchObjectInChild(L"GUICanvas");
			if (canvasObj)
			{
				CCanvas* canvas = canvasObj->getComponent<CCanvas>();
				CGUIElement* root = canvas->getRootElement();

				root->setRect(core::rectf(0.0f, 0.0f, w, h));
			}

			m_menu->close();
			clearTabGroup();
		}

		void CCanvasSettingController::onChanged(GUI::CBase* base)
		{

		}

		void CCanvasSettingController::onCheckBoxOutline(GUI::CBase* base)
		{
			CScene* scene = CGUIDesignController::getInstance()->getScene();
			CGameObject* canvasObj = scene->searchObjectInChild(L"GUICanvas");
			if (canvasObj)
			{
				CCanvas* canvas = canvasObj->getComponent<CCanvas>();
				if (canvas)
				{
					canvas->DrawOutline = m_checkboxOutline->getToggle();
				}
			}
		}

		void CCanvasSettingController::onChangeLocalize(std::string src)
		{
			CGUIDesignController* controller = CGUIDesignController::getInstance();
			controller->initLocalizePath(src.c_str());

			showLanguage();
		}
	}
}
