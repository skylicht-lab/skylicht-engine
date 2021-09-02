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
#include "CSceneExporter.h"

namespace Skylicht
{
	void CSceneExporter::loadChildObjectSerializable(CContainerObject* container, CObjectSerializable* data)
	{
		CObjectSerializable* childs = new CObjectSerializable("Childs");
		data->addProperty(childs);
		data->addAutoRelease(childs);

		ArrayGameObject* go = container->getChilds();
		for (size_t i = 0, n = go->size(); i < n; i++)
		{
			CGameObject* childObject = go->at(i);

			CObjectSerializable* childData = childObject->createSerializable();
			childs->addProperty(childData);
			childs->addAutoRelease(childData);

			CContainerObject* childContainer = dynamic_cast<CContainerObject*>(childObject);
			if (childContainer != NULL)
				loadChildObjectSerializable(childContainer, childData);
		}
	}

	void CSceneExporter::exportGameObject(CGameObject* object, const char* path)
	{
		CObjectSerializable* data = object->createSerializable();

		CContainerObject* container = dynamic_cast<CContainerObject*>(object);
		if (container != NULL)
			loadChildObjectSerializable(container, data);

		data->save(path);
		delete data;
	}
}