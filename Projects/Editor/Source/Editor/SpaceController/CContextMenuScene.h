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

#pragma once

#include "SkylichtEngine.h"

#include "Scene/CScene.h"
#include "GameObject/CZone.h"

#include "Editor/Space/CSpace.h"
#include "Editor/Space/Hierarchy/CHierachyNode.h"
#include "Editor/Space/Hierarchy/CSpaceHierarchy.h"

namespace Skylicht
{
	namespace Editor
	{
		class CContextMenuScene
		{
		protected:
			GUI::CCanvas* m_canvas;

			CSpaceHierarchy* m_spaceHierarchy;
			CHierachyNode* m_contextNode;

			GUI::CMenu* m_contextMenuScene;
			GUI::CMenu* m_contextMenuGameObject;
			GUI::CMenu* m_contextMenuContainer;
			GUI::CMenuItem* m_setCurrentZoneItem;
			GUI::CMenuSeparator* m_spaceZone;

			CScene* m_scene;
			CZone* m_zone;

		public:
			CContextMenuScene(GUI::CCanvas* canvas);

			~CContextMenuScene();

			bool onContextMenu(CSpaceHierarchy *spaceHierachy, CHierachyNode* node, CScene* scene, CZone* zone);

		protected:

			void OnContextMenuCommand(GUI::CBase* sender);
		};
	}
}