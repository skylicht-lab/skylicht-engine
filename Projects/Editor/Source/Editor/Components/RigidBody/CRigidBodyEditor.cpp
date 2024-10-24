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

#ifdef BUILD_SKYLICHT_PHYSIC

#include "CRigidBodyEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/SpaceController/CSceneController.h"

#include "RigidBody/CRigidbody.h"
using namespace Skylicht::Physics;

namespace Skylicht
{
	namespace Editor
	{
		EDITOR_REGISTER(CRigidBodyEditor, CRigidbody);

		CRigidBodyEditor::CRigidBodyEditor()
		{

		}

		CRigidBodyEditor::~CRigidBodyEditor()
		{

		}

		void CRigidBodyEditor::initGUI(CComponentSystem* target, CSpaceProperty* ui)
		{
			CDefaultEditor::initGUI(target, ui);
		}

		void CRigidBodyEditor::initCustomGUI(GUI::CBoxLayout* layout, CSpaceProperty* ui)
		{
			layout->addSpace(5.0f);

			ui->addButton(layout, L"Refresh Rigidbody")->OnPress = [&](GUI::CBase* button)
				{
					CRigidbody* rigidBody = (CRigidbody*)m_component;
					rigidBody->initRigidbody();
				};
		}

		void CRigidBodyEditor::update()
		{

		}
	}
}

#endif