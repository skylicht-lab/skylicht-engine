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
#include "CSpaceParticle.h"

#include "Selection/CSelection.h"

#include "Editor/SpaceController/CParticleController.h"
#include "Editor/SpaceController/CSceneController.h"


namespace Skylicht
{
	namespace Editor
	{
		CSpaceParticle::CSpaceParticle(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_particle(NULL),
			m_canvas(NULL),
			m_tree(NULL),
			m_hierarchyController(NULL),
			m_hierarchyContextMenu(NULL)
		{
			m_canvas = window->getCanvas();

			GUI::CToolbar* toolbar = new GUI::CToolbar(window);
			GUI::CButton* btn;

			btn = toolbar->addButton(L"Save", GUI::ESystemIcon::Save);
			btn->OnPress = BIND_LISTENER(&CSpaceParticle::onSave, this);

			toolbar->addSpace();

			btn = toolbar->addButton(L"Play", GUI::ESystemIcon::PlayerPlay);
			btn->OnPress = BIND_LISTENER(&CSpaceParticle::onPlay, this);

			btn = toolbar->addButton(L"Stop", GUI::ESystemIcon::PlayerPause);
			btn->OnPress = BIND_LISTENER(&CSpaceParticle::onStop, this);

			GUI::CBase* treeContainer = new GUI::CBase(window);
			treeContainer->dock(GUI::EPosition::Fill);

			m_tree = new GUI::CTreeControl(treeContainer);
			m_tree->dock(GUI::EPosition::Fill);

			m_hierarchyController = new CParticleHierarchyController(window->getCanvas(), m_tree, m_editor);
			m_hierarchyContextMenu = new CParticleHierachyContextMenu(m_tree);

			CParticleController::getInstance()->setSpaceParticle(this);

			CSelectObject* selectObject = CSelection::getInstance()->getLastSelected();
			if (selectObject)
			{
				CScene* scene = CSceneController::getInstance()->getScene();
				CGameObject* obj = scene->searchObjectInChildByID(selectObject->getID().c_str());
				if (obj != NULL)
				{
					Particle::CParticleComponent* particle = obj->getComponent<Particle::CParticleComponent>();
					if (particle)
						setParticle(particle);
					else
						setNull();
				}
			}
		}

		CSpaceParticle::~CSpaceParticle()
		{
			delete m_hierarchyController;
			delete m_hierarchyContextMenu;

			CParticleController::getInstance()->closeSpaceParticle(this);
		}

		void CSpaceParticle::setParticle(Particle::CParticleComponent* particle)
		{
			m_particle = particle;
			if (m_particle)
			{
				CParticleHierachyNode* node = CParticleController::getInstance()->buildNode(particle);
				if (node)
					m_hierarchyController->setTreeNode(node);
			}
		}

		void CSpaceParticle::setNull()
		{
			m_particle = NULL;
			m_hierarchyController->setTreeNode(NULL);
		}

		void CSpaceParticle::onSave(GUI::CBase* base)
		{
			if (m_particle)
			{
				std::string file = m_particle->getSourcePath();
				if (file.empty())
				{
					std::string assetFolder = CAssetManager::getInstance()->getAssetFolder();
					GUI::COpenSaveDialog* dialog = new GUI::COpenSaveDialog(m_canvas, GUI::COpenSaveDialog::Save, assetFolder.c_str(), assetFolder.c_str(), "particle;*");
					dialog->OnSave = [&, particle = m_particle](std::string path)
						{
							std::string shortPath = CAssetManager::getInstance()->getShortPath(path.c_str());
							particle->setSourcePath(shortPath.c_str());
							particle->save();
						};
				}
				else
				{
					m_particle->save();
				}
			}
		}

		void CSpaceParticle::onPlay(GUI::CBase* base)
		{
			if (m_particle)
				m_particle->Play();
		}

		void CSpaceParticle::onStop(GUI::CBase* base)
		{
			if (m_particle)
				m_particle->Stop();
		}

		void CSpaceParticle::addToTreeNode(CParticleHierachyNode* node)
		{
			m_hierarchyController->addToTreeNode(node);
		}

		void CSpaceParticle::updateTreeNode(CParticleHierachyNode* node)
		{
			m_hierarchyController->updateTreeNode(node);
		}
	}
}