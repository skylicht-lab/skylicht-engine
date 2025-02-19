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
			m_emitter(NULL),
			m_zone(NULL),
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
				subMenu->getMenu()->OnCommand = BIND_LISTENER(&CContextMenuParticle::OnContextMenuEmitterCommand, this);

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


			m_menuRenderer = new GUI::CMenu(canvas);
			m_menuRenderer->setHidden(true);
			m_menuRenderer->OnCommand = BIND_LISTENER(&CContextMenuParticle::OnContextMenuGroupCommand, this);
			m_menuRenderer->addItem(L"Quad (Instancing)");
			m_menuRenderer->addItem(L"CPU Billboard");

			m_menuEmitters = new GUI::CMenu(canvas);
			m_menuEmitters->setHidden(true);
			m_menuEmitters->addItemByPath(L"Add/Normal");
			m_menuEmitters->addItemByPath(L"Add/Random");
			m_menuEmitters->addItemByPath(L"Add/Spheric");
			m_menuEmitters->addItemByPath(L"Add/Straight");
			subMenu = m_menuEmitters->getItemByLabel(L"Add");
			if (subMenu)
				subMenu->getMenu()->OnCommand = BIND_LISTENER(&CContextMenuParticle::OnContextMenuEmitterCommand, this);

			m_menuEmitter = new GUI::CMenu(canvas);
			m_menuEmitter->setHidden(true);
			m_menuEmitter->addItemByPath(L"Zone/AABox");
			m_menuEmitter->addItemByPath(L"Zone/Cylinder");
			m_menuEmitter->addItemByPath(L"Zone/Sphere");
			m_menuEmitter->addItemByPath(L"Zone/Point");
			m_menuEmitter->addItemByPath(L"Zone/Line");
			m_menuEmitter->addItemByPath(L"Zone/PolyLine");
			m_menuEmitter->addItemByPath(L"Zone/Ring");
			subMenu = m_menuEmitter->getItemByLabel(L"Zone");
			if (subMenu)
				subMenu->getMenu()->OnCommand = BIND_LISTENER(&CContextMenuParticle::OnContextMenuZoneCommand, this);
			m_menuEmitter->addSeparator();
			m_menuEmitter->addItem(L"Delete");
			m_menuEmitter->OnCommand = BIND_LISTENER(&CContextMenuParticle::OnContextMenuEmitterCommand, this);

			m_menuZone = new GUI::CMenu(canvas);
			m_menuZone->setHidden(true);
			m_menuZone->addItemByPath(L"AABox");
			m_menuZone->addItemByPath(L"Cylinder");
			m_menuZone->addItemByPath(L"Sphere");
			m_menuZone->addItemByPath(L"Point");
			m_menuZone->addItemByPath(L"Line");
			m_menuZone->addItemByPath(L"PolyLine");
			m_menuZone->addItemByPath(L"Ring");
			m_menuZone->OnCommand = BIND_LISTENER(&CContextMenuParticle::OnContextMenuZoneCommand, this);
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
			m_emitter = NULL;
			m_zone = NULL;

			CParticleHierachyNode::EDataType tagData = node->getTagDataType();

			switch (tagData)
			{
			case CParticleHierachyNode::Particle:
				m_canvas->closeMenu();
				m_menuParticle->open(mousePos);
				break;
			case CParticleHierachyNode::Group:
				m_group = (Particle::CGroup*)node->getTagData();
				m_canvas->closeMenu();
				checkMenuRenderer(m_menuGroup);
				m_menuGroup->open(mousePos);
				break;
			case CParticleHierachyNode::Renderer:
				m_group = (Particle::CGroup*)node->getParentData();
				m_canvas->closeMenu();
				checkMenuRenderer(m_menuRenderer);
				m_menuRenderer->open(mousePos);
				break;
			case CParticleHierachyNode::Emitters:
				m_group = (Particle::CGroup*)node->getParentData();
				m_canvas->closeMenu();
				m_menuEmitters->open(mousePos);
				break;
			case CParticleHierachyNode::Emitter:
				m_group = (Particle::CGroup*)node->getParentData();
				m_emitter = (Particle::CEmitter*)node->getTagData();
				m_zone = m_emitter->getZone();
				m_canvas->closeMenu();
				checkMenuZone(m_menuEmitter);
				m_menuEmitter->open(mousePos);
				break;
			case CParticleHierachyNode::Zone:
				m_canvas->closeMenu();
				m_group = (Particle::CGroup*)node->getParentData();
				m_zone = (Particle::CZone*)node->getTagData();

				for (Particle::CEmitter* emitter : m_group->getEmitters())
				{
					if (emitter->getZone() == m_zone)
					{
						m_emitter = emitter;
						break;
					}
				}

				checkMenuZone(m_menuZone);
				m_menuZone->open(mousePos);
				break;
			default:
				break;
			}

			return true;
		}

		void CContextMenuParticle::checkMenuRenderer(GUI::CMenu* menu)
		{
			Particle::IRenderer* renderer = m_group->getRenderer();
			Particle::CQuadRenderer* quadRenderer = dynamic_cast<Particle::CQuadRenderer*>(renderer);

			GUI::CMenuItem* r1 = menu->searchItemByLabel(L"Quad (Instancing)");
			GUI::CMenuItem* r2 = menu->searchItemByLabel(L"CPU Billboard");

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
		}

		void CContextMenuParticle::checkMenuZone(GUI::CMenu* menu)
		{
			GUI::CMenuItem* items[Particle::EZone::NumOfZone];

			items[Particle::EZone::AABox] = menu->searchItemByLabel(L"AABox");
			items[Particle::EZone::Cylinder] = menu->searchItemByLabel(L"Cylinder");
			items[Particle::EZone::Sphere] = menu->searchItemByLabel(L"Sphere");
			items[Particle::EZone::Point] = menu->searchItemByLabel(L"Point");
			items[Particle::EZone::Line] = menu->searchItemByLabel(L"Line");
			items[Particle::EZone::PolyLine] = menu->searchItemByLabel(L"PolyLine");
			items[Particle::EZone::Ring] = menu->searchItemByLabel(L"Ring");

			for (int i = 0; i < (int)Particle::EZone::NumOfZone; i++)
			{
				if (items[i])
				{
					if (i == (int)m_zone->getType())
						items[i]->setIcon(GUI::ESystemIcon::Check);
					else
						items[i]->setIcon(GUI::ESystemIcon::None);
				}
			}
		}

		void CContextMenuParticle::OnContextMenuCommand(GUI::CBase* sender)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(sender);
			if (menuItem == NULL)
				return;

			const std::wstring& command = menuItem->getLabel();

			if (command == L"Add Group")
				CParticleController::getInstance()->createGroup(m_particle);
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

		void CContextMenuParticle::OnContextMenuEmitterCommand(GUI::CBase* sender)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(sender);
			if (menuItem == NULL)
				return;

			const std::wstring& command = menuItem->getLabel();

			Particle::CFactory* factory = m_particle->getParticleFactory();
			CParticleController* particleController = CParticleController::getInstance();
			CPropertyController* propretyController = CPropertyController::getInstance();

			Particle::CEmitter* emitter = NULL;

			if (command == L"Normal")
				emitter = factory->createNormalEmitter(false);
			else if (command == L"Random")
				emitter = factory->createRandomEmitter();
			else if (command == L"Spheric")
				emitter = factory->createSphericEmitter(Transform::Oy, 0.0f, 45.0f * core::DEGTORAD);
			else if (command == L"Straight")
				emitter = factory->createStraightEmitter(Transform::Oy);
			else if (command == L"Delete")
			{
				m_group->removeEmitter(m_emitter);
				factory->deleteZone(m_emitter->getZone());
				factory->deleteEmitter(m_emitter);

				CPropertyController::getInstance()->setProperty(NULL);
			}

			if (emitter)
			{
				Particle::CAABox* zone = factory->createAABoxZone(core::vector3df(), core::vector3df(0.5f, 0.5f, 0.5f));
				emitter->setZone(zone);

				m_group->addEmitter(emitter);
			}

			particleController->updateGroupHierachy(m_group);
		}

		void CContextMenuParticle::OnContextMenuZoneCommand(GUI::CBase* sender)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(sender);
			if (menuItem == NULL)
				return;

			const std::wstring& command = menuItem->getLabel();

			Particle::CFactory* factory = m_particle->getParticleFactory();
			CParticleController* particleController = CParticleController::getInstance();
			CPropertyController* propretyController = CPropertyController::getInstance();

			Particle::EZone currentZone = m_emitter->getZone()->getType();
			Particle::EZone createZone = currentZone;

			if (command == L"AABox")
				createZone = Particle::EZone::AABox;
			else if (command == L"Cylinder")
				createZone = Particle::EZone::Cylinder;
			else if (command == L"Sphere")
				createZone = Particle::EZone::Sphere;
			else if (command == L"Point")
				createZone = Particle::EZone::Point;
			else if (command == L"Line")
				createZone = Particle::EZone::Line;
			else if (command == L"PolyLine")
				createZone = Particle::EZone::PolyLine;
			else if (command == L"Ring")
				createZone = Particle::EZone::Ring;

			if (currentZone != createZone)
			{
				Particle::CZone* zone = factory->createZone(createZone);
				if (zone)
				{
					factory->deleteZone(m_emitter->getZone());
					m_emitter->setZone(zone);
				}

				particleController->updateGroupHierachy(m_group);
			}
		}
	}
}


