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

#pragma once

#include "CCanvas.h"

namespace Skylicht
{
	/// @brief This object class helps initialize GUI components for the CCanvas from files and data.
	/// @ingroup Graphics2D
	/// This object class helps initialize GUI components for the CCanvas from files (.gui) and data, which are extracted by the CGUIExporter class or created from Skylicht-Editor
	/// 
	/// @see CCanvas
	/// 
	/// @code
	/// CZone* zone = scene->getZone(0);
	/// CGameObject* guiObj = zone->createEmptyObject();
	/// CCanvas* canvas = guiObj->addComponent<CCanvas>();
	/// float applyScale = 0.0f;
	/// CGUIImporter::loadGUI("menu.gui", canvas);
	/// canvas->applyScaleGUI(applyScale);
	/// canvas->updateEntities();
	/// @endcode
	class SKYLICHT_API CGUIImporter
	{
	public:
		static bool loadGUI(const char* file, CCanvas* canvas);

		static bool beginImport(const char* file, CCanvas* canvas);

		static bool updateLoadGUI();

		static float getLoadingPercent();

		static CObjectSerializable* loadGUIToSerializable(const char* file, CCanvas* canvas);

		static void loadGUIFromSerializable(CObjectSerializable* gui, CCanvas* canvas);

		static CGUIElement* importGUI(CCanvas* canvas, CGUIElement* target, CObjectSerializable* obj, bool generateNewId = false);

		static CObjectSerializable* getSerializableByPath(CObjectSerializable* obj, const char* path);

		static void reset(CCanvas* canvas);

	protected:

		static bool loadStep(CCanvas* canvas, io::IXMLReader* reader);

		static void buildCanvas(CCanvas* canvas, io::IXMLReader* reader);

		static bool loadObjStep(CObjectSerializable* obj, CCanvas* canvas, io::IXMLReader* reader);

		static bool loadObjChilds(CObjectSerializable* obj, CCanvas* canvas, io::IXMLReader* reader);
	};
}
