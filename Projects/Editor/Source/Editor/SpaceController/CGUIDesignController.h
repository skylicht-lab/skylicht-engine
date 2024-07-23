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

#include "GUI/CGUIContext.h"

#include "Utils/CSingleton.h"
#include "AssetManager/CAssetManager.h"
#include "Reactive/IObserver.h"
#include "Editor/Space/GUIHierarchy/CGUIHierachyNode.h"
#include "Editor/Space/GUIHierarchy/CSpaceGUIHierarchy.h"
#include "Editor/Space/GUIDesign/CSpaceGUIDesign.h"
#include "CContextMenuGUIElement.h"

namespace Skylicht
{
	namespace Editor
	{
		class CGUIDesignController :
			public IFileLoader,
			public IObserver
		{
		public:
			DECLARE_SINGLETON(CGUIDesignController)

		protected:
			GUI::CCanvas* m_canvas;

			Skylicht::CCanvas* m_guiCanvas;

			CGUIHierachyNode* m_rootNode;

			CSpaceGUIHierarchy* m_spaceHierarchy;

			CSpaceGUIDesign* m_spaceDesign;

			CContextMenuGUIElement* m_contextMenu;

			std::string m_guiFilePath;

		public:
			CGUIDesignController();

			virtual ~CGUIDesignController();

			inline void setSpaceHierarchy(CSpaceGUIHierarchy* space)
			{
				m_spaceHierarchy = space;
			}

			inline CSpaceGUIHierarchy* getSpaceHierarchy()
			{
				return m_spaceHierarchy;
			}

			inline void setSpaceDesign(CSpaceGUIDesign* space)
			{
				m_spaceDesign = space;
			}

			inline CSpaceGUIDesign* getSpaceDesign()
			{
				return m_spaceDesign;
			}

			void initContextMenu(GUI::CCanvas* canvas);

			inline const std::string& getSaveGUIPath()
			{
				return m_guiFilePath;
			}

			void save(const char* path);

			virtual void loadFile(const std::string& path);

			void rebuildGUIHierachy();

			void onContextMenu(CGUIHierachyNode* node);

			void createGUINode(CGUIHierachyNode* parent, const std::wstring& command);

			void deselectAllOnHierachy();

			void onDelete();

			void refresh();

			CGUIHierachyNode* deselectOnHierachy(CGUIElement* element);

			CGUIHierachyNode* selectOnHierachy(CGUIElement* element);

			void onCopy();
			
			void onPaste();
			
			void onDuplicate();
			
			void onCut();
			
		protected:

			void rebuildGUIHierachy(CGUIElement* parent, CGUIHierachyNode* parentNode);

			void setNodeEvent(CGUIHierachyNode* node);

			void onUpdateNode(CGUIHierachyNode* node);

			void onSelectNode(CGUIHierachyNode* node, bool selected);

			virtual void onNotify(ISubject* subject, IObserver* from);
		};
	}
}
