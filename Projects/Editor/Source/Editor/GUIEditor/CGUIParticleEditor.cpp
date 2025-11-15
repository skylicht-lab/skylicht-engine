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
#include "CGUIParticleEditor.h"
#include "ParticleSystem/CGUIParticle.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/CEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		GUI_EDITOR_REGISTER(CGUIParticleEditor, CGUIParticle);

		CGUIParticleEditor::CGUIParticleEditor()
		{

		}

		CGUIParticleEditor::~CGUIParticleEditor()
		{
		}

		void CGUIParticleEditor::initGUI(CGUIElement* gui, CSpaceProperty* ui)
		{
			CGUIElementEditor::initGUI(gui, ui);

			GUI::CCollapsibleGroup* group = ui->addGroup("Particles", this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);
			ui->addButton(layout, L"Play")->OnPress = [&, gui](GUI::CBase* button)
				{
					Particle::CGUIParticle* ps = dynamic_cast<Particle::CGUIParticle*>(gui);
					ps->playParticle();
				};

			ui->addButton(layout, L"Stop")->OnPress = [&, gui](GUI::CBase* button)
				{
					Particle::CGUIParticle* ps = dynamic_cast<Particle::CGUIParticle*>(gui);
					ps->stopParticle();
				};

			layout->addSpace(20.0f);

			ui->addButton(layout, L"Reload")->OnPress = [&, gui](GUI::CBase* button)
				{
					Particle::CGUIParticle* ps = dynamic_cast<Particle::CGUIParticle*>(gui);
					ps->reload();
				};

			group->setExpand(true);
		}
	}
}