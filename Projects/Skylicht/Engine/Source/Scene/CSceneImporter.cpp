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

namespace Skylicht
{
	io::IXMLReader* CSceneImporter::s_reader = NULL;
	std::string CSceneImporter::s_scenePath;
	CScene* CSceneImporter::s_scene = NULL;

	void CSceneImporter::buildScene(CScene* scene, io::IXMLReader* reader)
	{
		std::wstring nodeName = L"CObjectSerializable";
		std::wstring attributeName;

		std::stack<std::wstring> serializableTree;
		std::stack<CContainerObject*> container;

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

						if (attributeName == L"CZone")
						{
							CZone* zone = scene->createZone();
							container.push((CContainerObject*)zone);
						}
						else if (attributeName == L"CContainerObject")
						{
							CContainerObject* currentContainer = container.top();
							CContainerObject* object = currentContainer->createContainerObject();
							container.push(object);
						}
						else if (attributeName == L"CGameObject")
						{
							CContainerObject* currentContainer = container.top();
							currentContainer->createEmptyObject();
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
	}

	bool CSceneImporter::beginImportScene(CScene* scene, const char* file)
	{
		// step 1
		// build scene object
		s_reader = getIrrlichtDevice()->getFileSystem()->createXMLReader(file);
		if (s_reader == NULL)
			return false;

		buildScene(scene, s_reader);

		s_reader->drop();

		s_reader = getIrrlichtDevice()->getFileSystem()->createXMLReader(s_scenePath.c_str());

		s_scenePath = file;
		s_scene = scene;

		return true;
	}

	bool CSceneImporter::loadStep()
	{
		return true;
	}

	bool CSceneImporter::updateLoadScene()
	{
		// step 2
		// load object attribute
		if (CSceneImporter::loadStep())
		{
			// drop
			if (s_reader)
			{
				s_reader->drop();
				s_reader = NULL;
			}

			// final index search object
			s_scene->updateIndexSearchObject();
			s_scene = NULL;
			return true;
		}

		return false;
	}
}