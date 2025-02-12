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
#include "CParticleController.h"

#include "Editor/Space/Particle/CSpaceParticle.h"

#include "ParticleSystem/CParticleComponent.h"

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CParticleController);

		CParticleController::CParticleController() :
			m_hierachyNode(NULL),
			m_contextNode(NULL),
			m_canvas(NULL)
		{

		}

		CParticleController::~CParticleController()
		{

		}

		void CParticleController::initContextMenu(GUI::CCanvas* canvas)
		{
			m_canvas = canvas;
		}

		void CParticleController::setGameObject(CGameObject* obj)
		{
			CSpaceParticle* spaceParticle = (CSpaceParticle*)CEditor::getInstance()->getWorkspaceByName(L"Particle");
			if (spaceParticle == NULL)
				return;

			Particle::CParticleComponent* particle = obj->getComponent<Particle::CParticleComponent>();
			if (particle)
				spaceParticle->setParticle(particle);
			else
				spaceParticle->setNull();
		}

		CParticleHierachyNode* CParticleController::buildNode(Particle::CParticleComponent* particle)
		{
			if (m_hierachyNode)
				delete m_hierachyNode;

			m_hierachyNode = new CParticleHierachyNode(NULL);
			m_hierachyNode->setName(L"Particle System");
			m_hierachyNode->setIcon(GUI::ESystemIcon::ParticleSystem);
			m_hierachyNode->setTagData(particle, CParticleHierachyNode::Particle);

			setNodeEvent(m_hierachyNode);

			buildHierarchyData(particle);

			return m_hierachyNode;
		}

		void CParticleController::buildHierarchyData(Particle::CParticleComponent* particle)
		{
			for (u32 i = 0, n = particle->getNumOfGroup(); i < n; i++)
			{
				Particle::CGroup* group = particle->getGroup(i);

				CParticleHierachyNode* node = m_hierachyNode->addChild();
				node->setTagData(group, CParticleHierachyNode::Group);
				node->setIcon(GUI::ESystemIcon::Folder);
			}
		}

		void CParticleController::setNodeEvent(CParticleHierachyNode* node)
		{
			node->OnSelected = std::bind(&CParticleController::onSelectNode, this, std::placeholders::_1, std::placeholders::_2);
		}

		void CParticleController::onSelectNode(CParticleHierachyNode* node, bool selected)
		{

		}

		void CParticleController::onContextMenu(CParticleHierachyNode* node)
		{
			if (m_canvas == NULL)
				return;

			// if (m_contextMenuScene->onContextMenu(m_spaceHierarchy, node, m_scene, m_zone))
			m_contextNode = node;
		}
	}
}