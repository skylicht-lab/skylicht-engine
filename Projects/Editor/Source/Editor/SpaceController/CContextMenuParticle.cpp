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
#include "CPropertyController.h"

namespace Skylicht
{
	namespace Editor
	{
		CContextMenuParticle::CContextMenuParticle(GUI::CCanvas* canvas) :
			m_canvas(canvas),
			m_menuParticle(NULL),
			m_particle(NULL),
			m_group(NULL),
			m_contextNode(NULL)
		{
			m_menuParticle = new GUI::CMenu(canvas);
			m_menuParticle->setHidden(true);
			m_menuParticle->OnCommand = BIND_LISTENER(&CContextMenuParticle::OnContextMenuCommand, this);
			m_menuParticle->addItem(L"Add Group", GUI::ESystemIcon::Folder);

			m_menuGroup = new GUI::CMenu(canvas);
			m_menuGroup->setHidden(true);
			m_menuGroup->OnCommand = BIND_LISTENER(&CContextMenuParticle::OnContextMenuGroupCommand, this);
			m_menuGroup->addItemByPath(L"Add/Emitter/Normal");
			m_menuGroup->addItemByPath(L"Add/Emitter/Random");
			m_menuGroup->addItemByPath(L"Add/Emitter/Spheric");
			m_menuGroup->addItemByPath(L"Add/Emitter/Straight");

			GUI::CMenuItem* subMenu = m_menuGroup->getItemByPath(L"Add/Emitter");
			if (subMenu)
				subMenu->getMenu()->OnCommand = BIND_LISTENER(&CContextMenuParticle::OnContextMenuGroupCommand, this);

			m_menuGroup->addItemByPath(L"Add/Model/Scale");
			m_menuGroup->addItemByPath(L"Add/Model/Scale X");
			m_menuGroup->addItemByPath(L"Add/Model/Scale Y");
			m_menuGroup->addItemByPath(L"Add/Model/Scale Z");
			m_menuGroup->addItemByPath(L"Add/Model/Rotate X");
			m_menuGroup->addItemByPath(L"Add/Model/Rotate Y");
			m_menuGroup->addItemByPath(L"Add/Model/Rotate Z");
			m_menuGroup->addItemByPath(L"Add/Model/Red");
			m_menuGroup->addItemByPath(L"Add/Model/Green");
			m_menuGroup->addItemByPath(L"Add/Model/Blue");
			m_menuGroup->addItemByPath(L"Add/Model/Alpha");
			m_menuGroup->addItemByPath(L"Add/Model/Mass");
			m_menuGroup->addItemByPath(L"Add/Model/Frame Index");
			m_menuGroup->addItemByPath(L"Add/Model/Rotate speed X");
			m_menuGroup->addItemByPath(L"Add/Model/Rotate speed Y");
			m_menuGroup->addItemByPath(L"Add/Model/Rotate speed Z");

			subMenu = m_menuGroup->getItemByPath(L"Add/Model");
			if (subMenu)
				subMenu->getMenu()->OnCommand = BIND_LISTENER(&CContextMenuParticle::OnContextMenuGroupCommand, this);

			m_menuGroup->addItemByPath(L"Renderer/Quad (Instancing)");
			m_menuGroup->addItemByPath(L"Renderer/CPU Billboard");

			subMenu = m_menuGroup->getItemByPath(L"Renderer");
			if (subMenu)
				subMenu->getMenu()->OnCommand = BIND_LISTENER(&CContextMenuParticle::OnContextMenuGroupCommand, this);

			m_menuGroup->addSeparator();
			m_menuGroup->addItem(L"Delete", GUI::ESystemIcon::Trash);
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
			m_group = NULL;

			if (node->getTagDataType() == CParticleHierachyNode::Particle)
			{
				m_canvas->closeMenu();
				m_menuParticle->open(mousePos);
				return true;
			}
			else if (node->getTagDataType() == CParticleHierachyNode::Group)
			{
				m_group = (Particle::CGroup*)node->getTagData();
				m_canvas->closeMenu();

				Particle::IRenderer* renderer = m_group->getRenderer();
				Particle::CQuadRenderer* quadRenderer = dynamic_cast<Particle::CQuadRenderer*>(renderer);

				GUI::CMenuItem* r1 = m_menuGroup->getItemByPath(L"Renderer/Quad (Instancing)");
				GUI::CMenuItem* r2 = m_menuGroup->getItemByPath(L"Renderer/CPU Billboard");

				if (quadRenderer)
				{
					r1->setIcon(GUI::ESystemIcon::Check);
					r2->setIcon(GUI::ESystemIcon::None);
				}
				else
				{
					r1->setIcon(GUI::ESystemIcon::None);
					r2->setIcon(GUI::ESystemIcon::Check);
				}

				m_menuGroup->open(mousePos);
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

		void CContextMenuParticle::OnContextMenuGroupCommand(GUI::CBase* sender)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(sender);
			if (menuItem == NULL)
				return;

			const std::wstring& command = menuItem->getLabel();

			Particle::CFactory* factory = m_particle->getParticleFactory();
			CParticleController* particleController = CParticleController::getInstance();
			CPropertyController* propretyController = CPropertyController::getInstance();

			if (command == L"Delete")
			{
				particleController->removeGroup(m_group);
				propretyController->setProperty(NULL);
				m_particle->getData()->removeGroup(m_group);
				m_group = NULL;
				return;
			}

			if (command == L"Quad (Instancing)")
			{
				Particle::IRenderer* renderer = m_group->getRenderer();
				Particle::CQuadRenderer* quadRenderer = dynamic_cast<Particle::CQuadRenderer*>(renderer);
				if (quadRenderer)
				{
					return;
				}
				else
				{
					factory->deleteRenderer(renderer);
					m_group->setRenderer(factory->createQuadRenderer());
				}
			}
			else if (command == L"CPU Billboard")
			{
				Particle::IRenderer* renderer = m_group->getRenderer();
				Particle::CBillboardAdditiveRenderer* billboardRenderer = dynamic_cast<Particle::CBillboardAdditiveRenderer*>(renderer);
				if (billboardRenderer)
				{
					return;
				}
				else
				{
					factory->deleteRenderer(renderer);
					m_group->setRenderer(factory->createBillboardAdditiveRenderer());
				}
			}

			particleController->updateGroupHierachy(m_group);
		}
	}
}


