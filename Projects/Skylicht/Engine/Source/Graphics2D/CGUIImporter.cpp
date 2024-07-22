/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
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
#include "CGUIImporter.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	io::IXMLReader* g_guiReader = NULL;
	std::string g_guiPath;
	CCanvas* g_canvas = NULL;

	int g_loadGUIStep = 10;
	int g_guiLoading = 0;

	std::list<CGUIElement*> g_listGUIObject;
	std::list<CGUIElement*>::iterator g_currentGUI;

	void CGUIImporter::buildCanvas(CCanvas* canvas, io::IXMLReader* reader)
	{
		std::wstring nodeName = L"node";
		std::wstring attributeName;

		std::stack<CGUIElement*> parents;

		g_listGUIObject.clear();

		CGUIElement* currentObject = NULL;

		core::rectf nullRect;
		SColor nullColor;

		while (reader->read())
		{
			switch (reader->getNodeType())
			{
			case io::EXN_ELEMENT:
				if (nodeName == reader->getNodeName())
				{
					if (reader->getAttributeValue(L"type") != NULL)
					{
						attributeName = reader->getAttributeValue(L"type");

						CGUIElement* parent = NULL;
						if (parents.size() > 0)
							parent = parents.top();

						CGUIElement* element = NULL;

						// No need create root
						if (parent)
						{
							// create null object, that will load data in later
							if (attributeName == L"CGUIElement")
								element = canvas->createElement(parent, nullRect);
							else if (attributeName == L"CGUIImage")
								element = canvas->createImage(parent, nullRect);
							else if (attributeName == L"CGUIMask")
								element = canvas->createMask(parent, nullRect);
							else if (attributeName == L"CGUIRect")
								element = canvas->createRect(parent, nullRect, nullColor);
							else if (attributeName == L"CGUISprite")
								element = canvas->createSprite(parent, nullRect, NULL);
							else if (attributeName == L"CGUIText")
								element = canvas->createText(parent, nullRect, NULL);
							else if (attributeName == L"CGUILayout")
								element = canvas->createLayout(parent, nullRect);
							else if (attributeName == L"CGUICustomSizeSprite")
								element = canvas->createFitSprite(parent, nullRect, NULL);
						}
						else
						{
							if (attributeName == L"CGUIElement")
								element = canvas->getRootElement();
						}

						if (element)
						{
							g_listGUIObject.push_back(element);
							currentObject = element;
						}

						if (currentObject && attributeName == L"Childs")
						{
							parents.push(currentObject);
						}
					}
				}
				break;
			case io::EXN_ELEMENT_END:
				if (nodeName == reader->getNodeName())
				{
					if (attributeName == L"Childs")
					{
						parents.pop();
					}
				}
				break;
			default:
				break;
			}
		}

		g_currentGUI = g_listGUIObject.begin();
	}

	bool CGUIImporter::beginImport(const char* file, CCanvas* canvas)
	{
		// step 1
		// build scene object
		g_guiReader = getIrrlichtDevice()->getFileSystem()->createXMLReader(file);
		if (g_guiReader == NULL)
			return false;

		buildCanvas(canvas, g_guiReader);

		// close
		g_guiReader->drop();

		// re-open the scene file
		g_guiReader = getIrrlichtDevice()->getFileSystem()->createXMLReader(file);
		g_guiPath = file;
		g_canvas = canvas;
		g_guiLoading = 0;

		return true;
	}

	bool CGUIImporter::loadStep(CCanvas* canvas, io::IXMLReader* reader)
	{
		std::wstring nodeName = L"node";
		std::wstring attributeName;

		int step = 0;

		while (step < g_loadGUIStep && reader->read())
		{
			switch (reader->getNodeType())
			{
			case io::EXN_ELEMENT:
				if (nodeName == reader->getNodeName())
				{
					if (reader->getAttributeValue(L"type") != NULL)
					{
						attributeName = reader->getAttributeValue(L"type");

						if (attributeName == L"CGUIElement" ||
							attributeName == L"CGUIImage" ||
							attributeName == L"CGUIMask" ||
							attributeName == L"CGUIRect" ||
							attributeName == L"CGUISprite" ||
							attributeName == L"CGUIText" ||
							attributeName == L"CGUILayout")
						{
							CGUIElement* guiObject = dynamic_cast<CGUIElement*>(*g_currentGUI);
							++g_currentGUI;
							++g_guiLoading;
							++step;

							CObjectSerializable* data = guiObject->createSerializable();
							data->parseSerializable(reader);
							guiObject->loadSerializable(data);
							delete data;
						}
					}
				}
				break;
			default:
				break;
			}
		}

		return g_currentGUI == g_listGUIObject.end();
	}

	bool CGUIImporter::updateLoadGUI()
	{
		if (CGUIImporter::loadStep(g_canvas, g_guiReader))
		{
			// drop
			if (g_guiReader)
			{
				g_guiReader->drop();
				g_guiReader = NULL;
			}

			g_canvas = NULL;
			g_listGUIObject.clear();
			return true;
		}

		return false;
	}

	float CGUIImporter::getLoadingPercent()
	{
		int size = (int)g_listGUIObject.size();
		return g_guiLoading / (float)size;
	}

	bool CGUIImporter::loadGUI(const char* file, CCanvas* canvas)
	{
		if (!beginImport(file, canvas))
			return false;
		
		bool finish = false;
		do
		{
			finish = updateLoadGUI();
		}
		while (!finish);
		
		return true;
	}
}
