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

				// add serializable data control

			}
		}

		void CDefaultEditor::update()
		{

		}
	}
}