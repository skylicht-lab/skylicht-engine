/*
!@
MIT License

CopyRight (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#pragma once

#include "Utils/CGameSingleton.h"

#include "Scene/CScene.h"

#include "Editor/Space/Scene/CSpaceScene.h"
#include "Editor/Space/Hierarchy/CSpaceHierarchy.h"
#include "Editor/Space/Hierarchy/CHierachyNode.h"

#include "CContextMenuScene.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSceneController : public CGameSingleton<CSceneController>
		{
		protected:
			CSpaceScene* m_spaceScene;

			CSpaceHierarchy* m_spaceHierarchy;

			CHierachyNode* m_hierachyNode;

			CHierachyNode* m_contextNode;

			CScene* m_scene;

			CZone* m_zone;

			GUI::CCanvas* m_canvas;

			CContextMenuScene* m_contextMenuScene;

		public:
			CSceneController();

			virtual ~CSceneController();

			void initContextMenu(GUI::CCanvas* canvas);

			void setSpaceScene(CSpaceScene* scene);

			inline CSpaceScene* getSpaceScene()
			{
				return m_spaceScene;
			}

			void setSpaceHierarchy(CSpaceHierarchy* hierarchy);

			inline CSpaceHierarchy* getSpaceHierarchy()
			{
				return m_spaceHierarchy;
			}

			void setScene(CScene* scene);

			void setZone(CZone* zone);

			CZone* getZone()
			{
				return m_zone;
			}

			void buildHierarchyNodes();

			void onCommand(const std::wstring& objectType);

			void onContextMenu(CHierachyNode* node);

			inline CHierachyNode* getContextNode()
			{
				return m_contextNode;
			}

			inline void clearContextNode()
			{
				m_contextNode = NULL;
			}

			void createZone();

			void createEmptyObject(CContainerObject* parent);

			void createContainerObject(CContainerObject* parent);

		protected:

			CHierachyNode* buildHierarchyNodes(CGameObject* object, CHierachyNode* parentNode);

		};
	}
}