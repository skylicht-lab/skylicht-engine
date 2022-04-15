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

#pragma once

#include "GameObject/CZone.h"
#include "CScene.h"
#include "Serializable/CObjectSerializable.h"

namespace Skylicht
{
	class CSceneImporter
	{
		static io::IXMLReader* s_reader;

		static std::string s_scenePath;

		static CScene* s_scene;

		static int s_loadStep;

		static int s_loading;

		static std::list<CGameObject*> s_listObject;

		static std::list<CGameObject*>::iterator s_current;

		static void buildComponent(CGameObject* object, io::IXMLReader* xmlReader);

		static void buildScene(CScene* scene, io::IXMLReader* xmlReader);

		static bool loadStep(CScene* scene, io::IXMLReader* reader);

	public:
		static void exportGameObject(CObjectSerializable* data, CContainerObject* target);

		static bool beginImportScene(CScene* scene, const char* path);

		static bool updateLoadScene();

		static float getLoadingPercent();
	};
}