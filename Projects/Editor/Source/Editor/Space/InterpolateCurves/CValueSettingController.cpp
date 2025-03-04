#include "pch.h"
#include "CValueSettingController.h"

namespace Skylicht
{
	namespace Editor
	{
		CValueSettingController::CValueSettingController(CEditor* editor, GUI::CMenu* menu) :
			m_editor(editor),
			m_menu(menu),
			m_point(NULL)
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
		}

		CValueSettingController::~CValueSettingController()
		{

		}

		void CValueSettingController::onShow(SControlPoint* controlPoint)
		{
			m_point = controlPoint;
			m_valueX->setValue(m_point->Position.X, false);
			m_valueY->setValue(m_point->Position.Y, false);
		}

		void CValueSettingController::onChanged(GUI::CBase* base)
		{
			m_point->Position.X = m_valueX->getValue();
			m_point->Position.Y = m_valueY->getValue();
		}
	}
}