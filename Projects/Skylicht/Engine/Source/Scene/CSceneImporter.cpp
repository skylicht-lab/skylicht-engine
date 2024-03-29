/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "CSceneImporter.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	io::IXMLReader* g_sceneReader = NULL;
	std::string g_sceneReaderPath;
	CScene* g_scene = NULL;

	int g_loadSceneStep = 10;
	int g_loadingScene = 0;

	std::list<CGameObject*> g_listGameObject;
	std::list<CGameObject*>::iterator g_currentGameObject;

	void CSceneImporter::buildComponent(CGameObject* object, io::IXMLReader* reader)
	{
		std::wstring nodeName = L"node";
		std::wstring attributeName;

		int tree = 1;
		bool done = false;

		while (!done && reader->read())
		{
			switch (reader->getNodeType())
			{
			case io::EXN_ELEMENT:
				if (nodeName == reader->getNodeName())
				{
					tree++;

					// only read node of Component child
					if (reader->getAttributeValue(L"type") != NULL && tree == 2)
					{
						attributeName = reader->getAttributeValue(L"type");
						std::string componentName = CStringImp::convertUnicodeToUTF8(attributeName.c_str());

						CComponentSystem* comSystem = object->getComponentByTypeName(componentName.c_str());
						if (comSystem == NULL)
						{
							// try add component
							if (object->addComponentByTypeName(componentName.c_str()) == NULL)
							{
								char log[512];
								sprintf(log, "[CSceneImporter] Found unsupport component '%s'", componentName.c_str());
								os::Printer::log(log);

								// unsupport component
								CNullComponent* nullComponent = object->addComponent<CNullComponent>();
								nullComponent->setName(componentName);
							}
						}
					}
				}
				break;
			case io::EXN_ELEMENT_END:
				if (nodeName == reader->getNodeName())
				{
					tree--;
					done = tree == 0;
				}
				break;
			default:
				break;
			}
		}
	}

	void CSceneImporter::buildScene(CScene* scene, io::IXMLReader* reader)
	{
		std::wstring nodeName = L"node";
		std::wstring attributeName;

		std::stack<std::wstring> serializableTree;
		std::stack<CContainerObject*> container;

		g_listGameObject.clear();

		CGameObject* currentObject = NULL;

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

						bool skipPush = false;

						if (attributeName == L"CZone")
						{
							CZone* zone = scene->createZone();
							container.push((CContainerObject*)zone);
							g_listGameObject.push_back(zone);
							currentObject = zone;
						}
						else if (attributeName == L"CContainerObject")
						{
							CContainerObject* currentContainer = container.top();
							CContainerObject* object = currentContainer->createContainerObject();
							container.push(object);
							g_listGameObject.push_back(object);
							currentObject = object;
						}
						else if (attributeName == L"CGameObject")
						{
							CContainerObject* currentContainer = container.top();
							CGameObject* object = currentContainer->createEmptyObject();
							g_listGameObject.push_back(object);
							currentObject = object;
						}
						else if (attributeName == L"Components")
						{
							if (currentObject != NULL)
							{
								buildComponent(currentObject, reader);
								continue;
							}
						}

						serializableTree.push(attributeName);
					}
					else
					{
						serializableTree.push(L"");
					}
				}
				break;
			case io::EXN_ELEMENT_END:
				if (nodeName == reader->getNodeName())
				{
					std::wstring type = serializableTree.top();
					if (type == L"CContainerObject" || type == L"CZone")
					{
						container.pop();
					}
					serializableTree.pop();
				}
				break;
			default:
				break;
			}
		}

		g_currentGameObject = g_listGameObject.begin();
	}

	void CSceneImporter::exportGameObject(CObjectSerializable* data, CContainerObject* target)
	{

	}

	bool CSceneImporter::beginImportScene(CScene* scene, const char* file)
	{
		// step 1
		// build scene object
		g_sceneReader = getIrrlichtDevice()->getFileSystem()->createXMLReader(file);
		if (g_sceneReader == NULL)
			return false;

		buildScene(scene, g_sceneReader);

		// close
		g_sceneReader->drop();

		// re-open the scene file
		g_sceneReader = getIrrlichtDevice()->getFileSystem()->createXMLReader(file);
		g_sceneReaderPath = file;
		g_scene = scene;
		g_loadingScene = 0;

		return true;
	}

	bool CSceneImporter::loadStep(CScene* scene, io::IXMLReader* reader)
	{
		std::wstring nodeName = L"node";
		std::wstring attributeName;

		int step = 0;

		while (step < g_loadSceneStep && reader->read())
		{
			switch (reader->getNodeType())
			{
			case io::EXN_ELEMENT:
				if (nodeName == reader->getNodeName())
				{
					if (reader->getAttributeValue(L"type") != NULL)
					{
						attributeName = reader->getAttributeValue(L"type");

						if (attributeName == L"CScene")
						{
							CObjectSerializable* data = scene->createSerializable();
							data->parseSerializable(reader);
							scene->loadSerializable(data);
							delete data;
						}
						else if (attributeName == L"CZone" ||
							attributeName == L"CContainerObject" ||
							attributeName == L"CGameObject")
						{
							CGameObject* gameobject = dynamic_cast<CGameObject*>(*g_currentGameObject);
							++g_currentGameObject;
							++g_loadingScene;
							++step;

							std::string name = CStringImp::convertUnicodeToUTF8(attributeName.c_str());
							CObjectSerializable* data = new CObjectSerializable(name.c_str());

							// load to end of node Components
							CSerializableLoader::load(reader, data, "Components");
							gameobject->loadSerializable(data);
							gameobject->startComponent();
							delete data;
						}
					}
				}
				break;
			default:
				break;
			}
		}

		return g_currentGameObject == g_listGameObject.end();
	}

	float CSceneImporter::getLoadingPercent()
	{
		int size = (int)g_listGameObject.size();
		return g_loadingScene / (float)size;
	}

	bool CSceneImporter::updateLoadScene()
	{
		// step 2
		// load object attribute
		if (CSceneImporter::loadStep(g_scene, g_sceneReader))
		{
			// drop
			if (g_sceneReader)
			{
				g_sceneReader->drop();
				g_sceneReader = NULL;
			}

			// final index search object
			g_scene->updateIndexSearchObject();
			g_scene = NULL;
			return true;
		}

		return false;
	}
}