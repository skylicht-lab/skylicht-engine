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

#include "SkylichtEngine.h"
#include "Editor/Space/CSpace.h"

#include "CParticleHierarchyController.h"
#include "CParticleHierachyContextMenu.h"

#include "CParticleHierachyNode.h"

#include "ParticleSystem/CParticleComponent.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpaceParticle : public CSpace
		{
		protected:
			GUI::CTreeControl* m_tree;

			CParticleHierarchyController* m_hierarchyController;
			CParticleHierachyContextMenu* m_hierarchyContextMenu;

			Particle::CParticleComponent* m_particle;

		public:
			CSpaceParticle(GUI::CWindow* window, CEditor* editor);

			virtual ~CSpaceParticle();

			void setParticle(Particle::CParticleComponent* particle);

			void setNull();

			void onSave(GUI::CBase* base);

			void onPlay(GUI::CBase* base);

			void onStop(GUI::CBase* base);

			void addToTreeNode(CParticleHierachyNode* node);

			void updateTreeNode(CParticleHierachyNode* node);
		};
	}
}