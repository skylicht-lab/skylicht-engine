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
	io::IXMLReader* CGUIImporter::s_reader = NULL;
	std::string CGUIImporter::s_guiPath;
	CCanvas* CGUIImporter::s_canvas = NULL;

	int CGUIImporter::s_loadStep = 10;
	int CGUIImporter::s_loading = 0;

	std::list<CGUIElement*> CGUIImporter::s_listObject;
	std::list<CGUIElement*>::iterator CGUIImporter::s_current;

	void CGUIImporter::buildCanvas(CCanvas* canvas, io::IXMLReader* reader)
	{
		std::wstring nodeName = L"node";
		std::wstring attributeName;

		std::stack<CGUIElement*> parents;

		s_listObject.clear();

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
								element = canvas->createCustomSizeSprite(parent, nullRect, NULL);
						}
						else
						{
							if (attributeName == L"CGUIElement")
								element = canvas->getRootElement();
						}

						if (element)
						{
							s_listObject.push_back(element);
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

		s_current = s_listObject.begin();
	}

	bool CGUIImporter::beginImport(const char* file, CCanvas* canvas)
	{
		// step 1
		// build scene object
		s_reader = getIrrlichtDevice()->getFileSystem()->createXMLReader(file);
		if (s_reader == NULL)
			return false;

		buildCanvas(canvas, s_reader);

		// close
		s_reader->drop();

		// re-open the scene file
		s_reader = getIrrlichtDevice()->getFileSystem()->createXMLReader(file);
		s_guiPath = file;
		s_canvas = canvas;
		s_loading = 0;

		return true;
	}

	bool CGUIImporter::loadStep(CCanvas* canvas, io::IXMLReader* reader)
	{
		std::wstring nodeName = L"node";
		std::wstring attributeName;

		int step = 0;

		while (step < s_loadStep && reader->read())
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
							CGUIElement* guiObject = dynamic_cast<CGUIElement*>(*s_current);
							++s_current;
							++s_loading;
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

		return s_current == s_listObject.end();
	}

	bool CGUIImporter::updateLoadGUI()
	{
		if (CGUIImporter::loadStep(s_canvas, s_reader))
		{
			// drop
			if (s_reader)
			{
				s_reader->drop();
				s_reader = NULL;
			}

			s_canvas = NULL;
			return true;
		}

		return false;
	}

	float CGUIImporter::getLoadingPercent()
	{
		int size = (int)s_listObject.size();
		return s_loading / (float)size;
	}
}