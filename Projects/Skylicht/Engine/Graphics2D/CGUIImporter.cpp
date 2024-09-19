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
#include "Graphics2D/CGraphics2D.h"
#include "Utils/CStringImp.h"
#include "Serializable/CSerializableLoader.h"

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
		std::stack<int> parentTypes;

		g_listGUIObject.clear();

		CGUIElement* currentObject = NULL;

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
							element = canvas->createNullElement(parent, attributeName.c_str());
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

						if (currentObject)
						{
							if (attributeName == L"Childs")
							{
								parentTypes.push(1);
								parents.push(currentObject);
							}
							else
							{
								parentTypes.push(0);
							}
						}
						else
						{
							parentTypes.push(0);
						}
					}
				}
				break;
			case io::EXN_ELEMENT_END:
				if (nodeName == reader->getNodeName())
				{
					if (parentTypes.top() == 1)
					{
						parentTypes.pop();
						parents.pop();
					}
					else
					{
						parentTypes.pop();
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
						if (attributeName != L"Childs")
						{
							CGUIElement* guiObject = dynamic_cast<CGUIElement*>(*g_currentGUI);

							if (guiObject)
							{
								++g_currentGUI;
								++g_guiLoading;
								++step;

								CObjectSerializable* data = guiObject->createSerializable();
								data->parseSerializable(reader);
								guiObject->loadSerializable(data);
								delete data;
							}
							else
							{
								char log[512];
								sprintf(log, "[CGUIImporter] Can't load element: %s", CStringImp::convertUnicodeToUTF8(attributeName.c_str()).c_str());
								os::Printer::log(log);

								++g_currentGUI;
								++g_guiLoading;
								++step;
							}
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

			reset(g_canvas);

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
		} while (!finish);

		return true;
	}

	void CGUIImporter::loadGUI(CObjectSerializable* gui, CCanvas* canvas)
	{
		CValueProperty* childs = gui->getProperty("Childs");
		if (childs == NULL)
			return;

		CGUIElement* root = canvas->getRootElement();
		root->loadSerializable(gui);

		if (childs && childs->getType() == Skylicht::Object)
		{
			CObjectSerializable* objs = dynamic_cast<CObjectSerializable*>(childs);
			if (objs)
			{
				for (u32 i = 0, n = objs->getNumProperty(); i < n; i++)
				{
					CValueProperty* obj = objs->getPropertyID(i);
					if (obj->getType() == Skylicht::Object)
					{
						CObjectSerializable* uiObj = dynamic_cast<CObjectSerializable*>(obj);
						importGUI(canvas, root, uiObj);
					}
				}
			}
		}

		reset(canvas);
	}

	CObjectSerializable* CGUIImporter::loadGUIObject(const char* file, CCanvas* canvas)
	{
		io::IXMLReader* reader = getIrrlichtDevice()->getFileSystem()->createXMLReader(file);
		if (!reader)
			return NULL;

		std::wstring nodeName = L"node";
		std::wstring attributeName;

		CObjectSerializable* obj = NULL;

		while (reader->read())
		{
			switch (reader->getNodeType())
			{
			case io::EXN_ELEMENT:
				if (nodeName == reader->getNodeName())
				{
					attributeName = reader->getAttributeValue(L"type");

					CGUIElement* element = canvas->createNullElement(canvas->getRootElement(), attributeName.c_str());
					if (element != NULL)
					{
						if (obj)
						{
							// note: only 1 node root
							delete obj;
							obj = NULL;
						}

						obj = element->createSerializable();
						element->remove();

						if (!loadObjStep(obj, canvas, reader))
						{
							reader->drop();
							return obj;
						}
					}
				}
				break;
			default:
				break;
			}
		}

		reader->drop();
		return obj;
	}

	bool CGUIImporter::loadObjStep(CObjectSerializable* obj, CCanvas* canvas, io::IXMLReader* reader)
	{
		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();

		// read attribute
		io::IAttributes* attr = fs->createEmptyAttributes();
		attr->read(reader);
		obj->deserialize(attr);
		attr->drop();

		// read childs
		std::wstring nodeName = L"node";
		std::wstring attributeName;

		CObjectSerializable* childs = NULL;

		while (reader->read())
		{
			switch (reader->getNodeType())
			{
			case io::EXN_ELEMENT:
				if (nodeName == reader->getNodeName())
				{
					attributeName = reader->getAttributeValue(L"type");
					if (attributeName == L"Childs")
					{
						childs = new CObjectSerializable("Childs");
						obj->addProperty(childs);
						obj->autoRelease(childs);

						if (!loadObjChilds(childs, canvas, reader))
							return false;
					}
					else
					{
						return false;
					}
				}
				break;
			case io::EXN_ELEMENT_END:
				if (nodeName == reader->getNodeName())
				{
					return true;
				}
				break;
			default:
				break;
			}
		}

		return false;
	}

	bool CGUIImporter::loadObjChilds(CObjectSerializable* obj, CCanvas* canvas, io::IXMLReader* reader)
	{
		std::wstring nodeName = L"node";
		std::wstring attributeName;

		while (reader->read())
		{
			switch (reader->getNodeType())
			{
			case io::EXN_ELEMENT:
				if (nodeName == reader->getNodeName())
				{
					attributeName = reader->getAttributeValue(L"type");
					if (attributeName == L"Childs")
					{
						return false;
					}
					else
					{
						CGUIElement* element = canvas->createNullElement(canvas->getRootElement(), attributeName.c_str());
						if (element == NULL)
							return false;

						CObjectSerializable* newObj = element->createSerializable();
						element->remove();

						loadObjStep(newObj, canvas, reader);
						obj->addProperty(newObj);
						obj->autoRelease(newObj);
					}
				}
				break;
			case io::EXN_ELEMENT_END:
				if (nodeName == reader->getNodeName())
				{
					return true;
				}
				break;
			default:
				break;
			}
		}
		return false;
	}

	CGUIElement* CGUIImporter::importGUI(CCanvas* canvas, CGUIElement* parent, CObjectSerializable* obj)
	{
		if (obj == NULL)
			return NULL;

		const std::string& type = obj->Name;

		CGUIElement* element = canvas->createNullElement(parent, type.c_str());
		if (!element)
			return NULL;

		element->loadSerializable(obj);

		CValueProperty* childs = obj->getProperty("Childs");
		if (childs && childs->getType() == Skylicht::Object)
		{
			CObjectSerializable* objs = dynamic_cast<CObjectSerializable*>(childs);
			if (objs)
			{
				for (u32 i = 0, n = objs->getNumProperty(); i < n; i++)
				{
					CValueProperty* obj = objs->getPropertyID(i);
					if (obj->getType() == Skylicht::Object)
					{
						CObjectSerializable* uiObj = dynamic_cast<CObjectSerializable*>(obj);
						importGUI(canvas, element, uiObj);
					}
				}
			}
		}

		return element;
	}

	std::vector<CObjectSerializable*> getObjChilds(CObjectSerializable* obj)
	{
		std::vector<CObjectSerializable*> ret;
		CValueProperty* childs = obj->getProperty("Childs");
		if (childs && childs->getType() == Skylicht::Object)
		{
			CObjectSerializable* objs = dynamic_cast<CObjectSerializable*>(childs);
			if (objs)
			{
				for (u32 i = 0, n = objs->getNumProperty(); i < n; i++)
				{
					CValueProperty* obj = objs->getPropertyID(i);
					if (obj->getType() == Skylicht::Object)
					{
						CObjectSerializable* child = dynamic_cast<CObjectSerializable*>(obj);
						if (child)
							ret.push_back(child);
					}
				}
			}
		}
		return ret;
	}

	CObjectSerializable* CGUIImporter::getGUIByPath(CObjectSerializable* obj, const char* path)
	{
		if (obj == NULL)
			return NULL;

		CObjectSerializable* ret = obj;

		std::vector<CObjectSerializable*> childs;
		std::vector<std::string> splitPath;
		CStringImp::splitString(path, "\\/", splitPath);

		while (splitPath.size() > 0)
		{
			const char* currentGUI = splitPath[0].c_str();
			bool found = false;

			childs = getObjChilds(ret);

			for (CObjectSerializable* gui : childs)
			{
				std::string name = gui->get("name", std::string("No name"));
				if (CStringImp::comp(name.c_str(), currentGUI) == 0)
				{
					ret = gui;
					found = true;
					break;
				}
			}

			if (!found)
			{
				ret = NULL;
				break;
			}

			splitPath.erase(splitPath.begin());
		}

		return ret;
	}

	void CGUIImporter::reset(CCanvas* canvas)
	{
		if (canvas->IsInEditor)
			return;

		CGUIElement* root = canvas->getRootElement();
		if (!root)
			return;

		// reset position (because it moved from editor)
		root->setPosition(core::vector3df());

		// reset width/height
		core::dimension2du screenSize = getVideoDriver()->getScreenSize();

		core::rectf r = root->getRect();

		core::rectf defaultGUIRect(0.0f, 0.0f, r.getWidth(), r.getHeight());
		canvas->setDefaultRect(defaultGUIRect);

		r.LowerRightCorner.X = r.UpperLeftCorner.X + screenSize.Width;
		r.LowerRightCorner.Y = r.UpperLeftCorner.Y + screenSize.Height;
		root->setRect(r);

		// reset background color
		if (root->getChilds().size() > 0)
		{
			CGUIRect* bg = dynamic_cast<CGUIRect*>(root->getChilds()[0]);
			if (bg)
				bg->setColor(SColor(0, 0, 0, 0));
		}
	}
}
