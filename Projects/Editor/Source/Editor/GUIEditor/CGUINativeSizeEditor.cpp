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
#include "CGUINativeSizeEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/CEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		GUI_EDITOR_REGISTER(CGUINativeSizeEditor, CGUIImage);
		GUI_EDITOR_REGISTER(CGUINativeSizeEditor, CGUISprite);

		CGUINativeSizeEditor::CGUINativeSizeEditor()
		{

		}

		CGUINativeSizeEditor::~CGUINativeSizeEditor()
		{
		}

		void CGUINativeSizeEditor::initGUI(CGUIElement* gui, CSpaceProperty* ui)
		{
			CGUIElementEditor::initGUI(gui, ui);

			GUI::CCollapsibleGroup* group = ui->addGroup("Native Size", this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);
			ui->addButton(layout, L"Set native size")->OnPress = [&, gui](GUI::CBase* button)
				{
					core::rectf nativeRect = gui->getNativeRect();
					gui->setRect(nativeRect);
					updateProperty();
				};

			ui->addButton(layout, L"Set center position")->OnPress = [&, gui](GUI::CBase* button)
				{
					core::rectf nativeRect = gui->getNativeRect();
					nativeRect -= core::vector2df(nativeRect.getWidth() * 0.5f, nativeRect.getHeight() * 0.5f);
					gui->setRect(nativeRect);
					updateProperty();
				};
			group->setExpand(true);
		}

		void CGUINativeSizeEditor::updateProperty()
		{
			CObjectSerializable* data = getData();
			if (data)
			{
				CVector3Property* p = dynamic_cast<CVector3Property*>(data->getProperty("position"));
				p->set(m_gui->getPosition());
				p->OnChanged();
				CVector3Property* s = dynamic_cast<CVector3Property*>(data->getProperty("scale"));
				s->set(m_gui->getScale());
				s->OnChanged();
				CVector3Property* r = dynamic_cast<CVector3Property*>(data->getProperty("rotation"));
				r->set(m_gui->getRotation());
				r->OnChanged();

				core::rectf rect = m_gui->getRect();

				CFloatProperty* rx = dynamic_cast<CFloatProperty*>(data->getProperty("rectX"));
				rx->set(rect.UpperLeftCorner.X);
				CFloatProperty* ry = dynamic_cast<CFloatProperty*>(data->getProperty("rectY"));
				ry->set(rect.UpperLeftCorner.Y);
				CFloatProperty* rw = dynamic_cast<CFloatProperty*>(data->getProperty("rectW"));
				rw->set(rect.getWidth());
				CFloatProperty* rh = dynamic_cast<CFloatProperty*>(data->getProperty("rectH"));
				rh->set(rect.getHeight());

				rx->OnChanged();
				ry->OnChanged();
				rw->OnChanged();
				rh->OnChanged();
			}

			// refresh
			CEditor::getInstance()->refresh();
		}
	}
}