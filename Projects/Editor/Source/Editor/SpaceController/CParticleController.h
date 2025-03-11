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

#pragma once

#include "CContextMenuParticle.h"

#include "Utils/CSingleton.h"
#include "GameObject/CGameObject.h"
#include "ParticleSystem/CParticleComponent.h"
#include "Editor/Space/Particle/CParticleHierachyNode.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpaceParticle;

		class CParticleController
		{
		protected:
			CParticleHierachyNode* m_hierachyNode;
			CParticleHierachyNode* m_contextNode;

			GUI::CCanvas* m_canvas;
			CContextMenuParticle* m_contextMenuParticle;

			CSpaceParticle* m_spaceParticle;

			Particle::CParticleComponent* m_particle;

		public:
			DECLARE_SINGLETON(CParticleController)

			CParticleController();

			virtual ~CParticleController();

			void setSpaceParticle(CSpaceParticle* space);

			void closeSpaceParticle(CSpaceParticle* space);

			void initContextMenu(GUI::CCanvas* canvas);

			void setGameObject(CGameObject* obj);

			CParticleHierachyNode* buildNode(Particle::CParticleComponent* particle);

			void buildHierarchyData(Particle::CParticleComponent* particle);

			CParticleHierachyNode* buildGroupHierachy(Particle::CGroup* group);

			CParticleHierachyNode* getNodeByTagData(void* data);

			void removeGroup(Particle::CGroup* group);

			void updateGroupHierachy(Particle::CGroup* group);

			void updateGroupHierachy(CParticleHierachyNode* node, Particle::CGroup* group);

			void updateGroupName();

			void setNodeEvent(CParticleHierachyNode* node);

			void onSelectNode(CParticleHierachyNode* node, bool selected);

			void onContextMenu(CParticleHierachyNode* node);

			void createGroup(Particle::CParticleComponent* particle);

			void onCreateGroup(Particle::CGroup* group);
		};
	}
}