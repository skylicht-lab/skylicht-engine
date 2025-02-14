/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
#include "CContextMenuParticle.h"

#include "GUI/Input/CInput.h"
#include "CParticleController.h"

namespace Skylicht
{
	namespace Editor
	{
		CContextMenuParticle::CContextMenuParticle(GUI::CCanvas* canvas) :
			m_canvas(canvas),
			m_menuParticle(NULL),
			m_particle(NULL),
			m_contextNode(NULL)
		{
			// scene
			m_menuParticle = new GUI::CMenu(canvas);
			m_menuParticle->setHidden(true);
			m_menuParticle->OnCommand = BIND_LISTENER(&CContextMenuParticle::OnContextMenuCommand, this);
			m_menuParticle->addItem(L"Add Group", GUI::ESystemIcon::Folder);
		}

		CContextMenuParticle::~CContextMenuParticle()
		{

		}

		bool CContextMenuParticle::onContextMenu(CParticleHierachyNode* node, Particle::CParticleComponent* particle)
		{
			GUI::CInput* input = GUI::CInput::getInput();
			GUI::SPoint mousePos = input->getMousePosition();

			m_contextNode = node;
			m_particle = particle;

			if (node->getTagDataType() == CParticleHierachyNode::Particle)
			{
				m_canvas->closeMenu();
				m_menuParticle->open(mousePos);
				return true;
			}

			return false;
		}

		void CContextMenuParticle::OnContextMenuCommand(GUI::CBase* sender)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(sender);
			if (menuItem == NULL)
				return;

			const std::wstring& command = menuItem->getLabel();

			if (command == L"Add Group")
			{
				CParticleController::getInstance()->createGroup(m_particle);
			}
		}
	}
}


