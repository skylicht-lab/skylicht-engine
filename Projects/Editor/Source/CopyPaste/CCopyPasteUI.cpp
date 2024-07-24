/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CCopyPasteUI.h"
#include "Graphics2D/CGUIImporter.h"
#include "Graphics2D/CGUIExporter.h"
#include "Editor/SpaceController/CGUIDesignController.h"

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CCopyPasteUI);

		CCopyPasteUI::CCopyPasteUI()
		{

		}

		CCopyPasteUI::~CCopyPasteUI()
		{
			clear();
		}

		void CCopyPasteUI::clear()
		{
			for (CObjectSerializable* gui : m_guiObjects)
				delete gui;
			m_guiObjects.clear();
		}

		void CCopyPasteUI::copy(std::vector<CGUIElement*>& listUI)
		{
			clear();

			for (CGUIElement *ui: listUI)
			{
				if (checkInsideParent(ui, listUI))
					continue;
				
				m_guiObjects.push_back(CGUIExporter::createSerializable(ui));
			}
		}
		
		std::vector<CGUIElement*> CCopyPasteUI::paste(CGUIElement* target)
		{
			std::vector<CGUIElement*> results;
			CCanvas *canvas = target->getCanvas();
		
			/*
			CGUIDesignController* guiEditorController = CGUIDesignController::getInstance();
			CGUIHierachyNode* parentNode = NULL;
			CGUIHierarchyController* hierarchyController = NULL;

			if (guiEditorController->getSpaceHierarchy() != NULL)
			{
				hierarchyController = guiEditorController->getSpaceHierarchy()->getController();
				parentNode = hierarchyController->getNodeByObject(target);
			}

			for (CObjectSerializable* obj: m_guiObjects)
			{
				CGUIElement* ui = CGUIImporter::importGUI(canvas, target, obj);
				if (ui)
					results.push_back(ui);
			}
			*/
			
			return results;
		}
	
		bool CCopyPasteUI::checkInsideParent(CGUIElement* ui, std::vector<CGUIElement*> list)
		{
			for (CGUIElement *uiElement: list)
			{
				if (uiElement == ui)
					continue;
				
				if (ui->isChild(uiElement))
					return true;
			}
			return false;
		}
	}
}
