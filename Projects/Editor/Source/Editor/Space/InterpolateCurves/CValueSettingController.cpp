#include "pch.h"
#include "CValueSettingController.h"
#include "CInterpolateCurvesController.h"

namespace Skylicht
{
	namespace Editor
	{
		CValueSettingController::CValueSettingController(CEditor* editor, GUI::CMenu* menu) :
			m_editor(editor),
			m_menu(menu),
			m_point(NULL),
			m_controller(NULL),
			m_currentLayer(0)
		{
			GUI::CBoxLayout* boxLayout = new GUI::CBoxLayout(menu);
			boxLayout->setPadding(GUI::SPadding(5.0, 5.0, -5.0, 5.0));
			boxLayout->dock(GUI::EPosition::Top);

			GUI::CLayout* layout;
			GUI::CLabel* label;

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Value X");
			label->setTextAlignment(GUI::TextRight);

			m_valueX = new GUI::CNumberInput(layout);
			m_valueX->setStep(0.1f);
			m_valueX->OnTextChanged = BIND_LISTENER(&CValueSettingController::onChanged, this);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Value Y");
			label->setTextAlignment(GUI::TextRight);

			m_valueY = new GUI::CNumberInput(layout);
			m_valueY->setStep(0.1f);
			m_valueY->OnTextChanged = BIND_LISTENER(&CValueSettingController::onChanged, this);
			boxLayout->endVertical();

			menu->addSeparator();
			menu->addItem(L"Auto");
			menu->addItem(L"Smooth");
			menu->addItem(L"Broken");
			menu->addItem(L"Linear");

			menu->addSeparator();
			menu->addItem(L"Delete");

			menu->OnCommand = BIND_LISTENER(&CValueSettingController::onCommand, this);
			menu->OnClose = BIND_LISTENER(&CValueSettingController::onClose, this);
		}

		CValueSettingController::~CValueSettingController()
		{

		}

		void CValueSettingController::onShow(int layer, CInterpolateCurvesController* controller, SControlPoint* controlPoint)
		{
			m_controller = controller;
			m_point = controlPoint;
			m_currentLayer = layer;

			m_valueX->setValue(m_point->Position.X, false);
			m_valueY->setValue(m_point->Position.Y, false);

			enableTabGroup();

			GUI::CMenuItem* items[(int)SControlPoint::Linear + 1];
			items[SControlPoint::Auto] = m_menu->getItemByLabel(L"Auto");
			items[SControlPoint::Smooth] = m_menu->getItemByLabel(L"Smooth");
			items[SControlPoint::Broken] = m_menu->getItemByLabel(L"Broken");
			items[SControlPoint::Linear] = m_menu->getItemByLabel(L"Linear");

			for (int i = 0; i <= (SControlPoint::Linear); i++)
			{
				if (items[i] != NULL)
				{
					if (i == controlPoint->Type)
						items[i]->setIcon(GUI::ESystemIcon::Check);
					else
						items[i]->setIcon(GUI::ESystemIcon::None);
				}
			}
		}

		void CValueSettingController::enableTabGroup()
		{
			GUI::CTabableGroup& tabGroup = m_menu->getCanvas()->TabableGroup;
			tabGroup.push();
			tabGroup.clear();
			tabGroup.add(m_valueX);
			tabGroup.add(m_valueY);
		}

		void CValueSettingController::clearTabGroup()
		{
			GUI::CTabableGroup& tabGroup = m_menu->getCanvas()->TabableGroup;
			tabGroup.pop();
		}

		void CValueSettingController::onChanged(GUI::CBase* base)
		{
			m_point->Position.X = m_valueX->getValue();
			m_point->Position.Y = m_valueY->getValue();
			m_controller->updateValue();
		}

		void CValueSettingController::onClose(GUI::CBase* base)
		{
			clearTabGroup();
		}

		void CValueSettingController::onCommand(GUI::CBase* base)
		{
			GUI::CMenuItem* item = dynamic_cast<GUI::CMenuItem*>(base);
			if (item != NULL)
			{
				SControlPoint::EControlType t = m_point->Type;

				std::wstring label = item->getLabel();
				if (label == L"Auto")
					m_point->Type = SControlPoint::Auto;
				else if (label == L"Smooth")
					m_point->Type = SControlPoint::Smooth;
				else if (label == L"Broken")
					m_point->Type = SControlPoint::Broken;
				else if (label == L"Linear")
					m_point->Type = SControlPoint::Linear;
				else if (label == L"Delete")
				{
					m_controller->deletePoint(m_currentLayer, m_point);
					return;
				}

				if (m_point->Type != t)
					m_controller->onPointChangeType(m_currentLayer, m_point);
			}
		}
	}
}