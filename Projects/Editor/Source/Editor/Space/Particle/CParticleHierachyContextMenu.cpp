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
#include "CParticleHierachyContextMenu.h"
#include "Editor/SpaceController/CParticleController.h"

namespace Skylicht
{
	namespace Editor
	{
		CParticleHierachyContextMenu::CParticleHierachyContextMenu(GUI::CTreeControl* tree)
		{
			tree->OnItemContextMenu = BIND_LISTENER(&CParticleHierachyContextMenu::OnTreeContextMenu, this);
		}

		CParticleHierachyContextMenu::~CParticleHierachyContextMenu()
		{

		}

		void CParticleHierachyContextMenu::OnTreeContextMenu(GUI::CBase* row)
		{
			GUI::CTreeRowItem* rowItem = dynamic_cast<GUI::CTreeRowItem*>(row);
			if (rowItem != NULL)
			{
				CParticleHierachyNode* node = (CParticleHierachyNode*)rowItem->getNode()->getTagData();
				if (node != NULL)
				{
					CParticleController::getInstance()->onContextMenu(node);
				}
			}
		}
	}
}