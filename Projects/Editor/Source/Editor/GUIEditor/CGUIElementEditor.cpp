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

namespace Skylicht
{
	namespace Editor
	{
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIElement);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIRect);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIMask);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIText);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUILayout);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIFitSprite);

		CGUIElementEditor::CGUIElementEditor()
		{

		}

		CGUIElementEditor::~CGUIElementEditor()
		{
		}

		void CGUIElementEditor::closeGUI()
		{
			CGUIEditor::closeGUI();
		}

		void CGUIElementEditor::initGUI(CGUIElement* gui, CSpaceProperty* ui)
		{
			CGUIEditor::initGUI(gui, ui);

			GUI::CCollapsibleGroup* group = ui->addGroup(gui->getTypeName().c_str(), this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);
			serializableToControl(m_guiData, ui, layout);
			group->setExpand(true);

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
			}
		}
	}
}