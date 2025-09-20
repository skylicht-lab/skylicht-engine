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
#include "CGUIElementEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/CEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIElement);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIRect);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIElipse);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIMask);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIText);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUILayout);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIFitSprite);

		CGUIElementEditor::CGUIElementEditor() :
			m_lastDock(EGUIDock::NoDock)
		{

		}

		CGUIElementEditor::~CGUIElementEditor()
		{
		}

		void CGUIElementEditor::closeGUI()
		{
			m_alignment = NULL;
			CGUIEditor::closeGUI();
		}

		void CGUIElementEditor::initGUI(CGUIElement* gui, CSpaceProperty* ui)
		{
			CGUIEditor::initGUI(gui, ui);

			GUI::CCollapsibleGroup* group = ui->addGroup(gui->getTypeName().c_str(), this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);
			serializableToControl(m_guiData, ui, layout);
			group->setExpand(true);

			group = ui->addGroup("Alignment", this);
			layout = ui->createBoxLayout(group);

			ui->addButton(layout, L"Set center position")->OnPress = [&, gui](GUI::CBase* button)
				{
					core::rectf nativeRect = gui->getRect();
					float w = nativeRect.getWidth();
					float h = nativeRect.getHeight();
					nativeRect = core::rectf(
						-w * 0.5f, -h * 0.5f,
						w * 0.5f, h * 0.5f
					);
					gui->setRect(nativeRect);
					updateProperty();
				};

			ui->addButton(layout, L"Set default position")->OnPress = [&, gui](GUI::CBase* button)
				{
					core::rectf nativeRect = gui->getRect();
					float w = nativeRect.getWidth();
					float h = nativeRect.getHeight();
					nativeRect = core::rectf(0.0, 0.0f, w, h);
					gui->setRect(nativeRect);
					updateProperty();
				};
			group->setExpand(true);

			m_alignment = group;
			onUpdateValue(NULL);
		}

		void CGUIElementEditor::update()
		{

		}

		void CGUIElementEditor::onUpdateValue(CObjectSerializable* object)
		{
			CGUIEditor::onUpdateValue(object);

			EGUIDock dock = m_gui->getDock();
			if (m_lastDock != dock || object == NULL)
			{
				m_lastDock = dock;
				switch (dock)
				{
				case EGUIDock::NoDock:
					showEditorTransform(true);
					showEditorRect(true);
					showEditorAlign(true);
					break;
				case EGUIDock::DockLeft:
				case EGUIDock::DockRight:
				case EGUIDock::DockTop:
				case EGUIDock::DockBottom:
					showEditorTransform(false);
					showEditorAlign(false);
					showEditorRect(true);
					break;
				case EGUIDock::DockFill:
					showEditorTransform(false);
					showEditorRect(false);
					showEditorAlign(false);
					break;
				}
				showEditorMargin(dock);

				if (m_alignment)
				{
					if (dock == EGUIDock::NoDock)
						m_alignment->setHidden(false);
					else
						m_alignment->setHidden(true);
				}
			}
		}

		void CGUIElementEditor::updateProperty()
		{
			CObjectSerializable* data = getData();
			if (data)
			{
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
			CEditor::getInstance()->refresh();
		}
	}
}