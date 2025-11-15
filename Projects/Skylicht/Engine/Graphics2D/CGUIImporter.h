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
	/// The CGUIImporter class helps load, import, and initialize GUI components for a CCanvas,
	/// from.gui files or serializable data, typically exported by CGUIExporter or designed in the Skylicht-Editor.
	/// 
	/// @see CCanvas
	/// 
	/// @code
	/// CZone* zone = scene->getZone(0);
	/// CGameObject* guiObj = zone->createEmptyObject();
	/// CCanvas* canvas = guiObj->addComponent<CCanvas>();
	/// float applyScale = 0.0f;
	/// CGUIImporter::loadGUI("menu.gui", canvas);
	/// canvas->applyGUIScale(applyScale);
	/// canvas->updateEntities();
	/// @endcode
	class SKYLICHT_API CGUIImporter
	{
	public:
		/**
		 * @class CGUIImporter
		 * @brief Utility class for importing GUI layouts and components into a CCanvas from files or serializable data.
		 * @ingroup Graphics2D
		 *
		 * CGUIImporter provides static methods for loading GUI layouts from XML (.gui) files, importing from serialized
		 * data, and managing the loading lifecycle (including asynchronous or step-based loading).
		 */
		static bool loadGUI(const char* file, CCanvas* canvas);

		/**
		 * @brief Begin importing a GUI layout from file. Initializes structures for step-based loading.
		 * @param file Path to the .gui file.
		 * @param canvas Pointer to the CCanvas to initialize.
		 * @return True if initialization was successful.
		 */
		static bool beginImport(const char* file, CCanvas* canvas);

		/**
		 * @brief Perform one step in the GUI loading process. Used for asynchronous or progressive loading.
		 * @return True if loading is finished.
		 */
		static bool updateLoadGUI();

		/**
		 * @brief Get percent progress of the GUI loading process.
		 * @return Loading progress as a value between 0.0 and 1.0.
		 */
		static float getLoadingPercent();

		/**
		 * @brief Load a GUI layout from file into a serializable object structure.
		 * @param file Path to the .gui file.
		 * @param canvas Pointer to CCanvas (for context).
		 * @return Pointer to the loaded CObjectSerializable (must be deleted by caller).
		 */
		static CObjectSerializable* loadGUIToSerializable(const char* file, CCanvas* canvas);

		/**
		 * @brief Initialize a canvas from a serializable GUI object.
		 * @param gui Pointer to root serializable object.
		 * @param canvas Pointer to CCanvas to initialize.
		 */
		static void loadGUIFromSerializable(CObjectSerializable* gui, CCanvas* canvas);

		/**
		 * @brief Import a GUI element tree from a serializable object into the canvas.
		 * @param canvas Pointer to CCanvas.
		 * @param target Parent GUI element.
		 * @param obj Serializable object for element.
		 * @param generateNewId If true, assigns new IDs to elements.
		 * @return Pointer to imported CGUIElement.
		 */
		static CGUIElement* importGUI(CCanvas* canvas, CGUIElement* target, CObjectSerializable* obj, bool generateNewId = false);

		/**
		 * @brief Find a serializable GUI object by its path in the hierarchy.
		 * @param obj Root serializable object.
		 * @param path Path string (separated by / or \).
		 * @return Pointer to found CObjectSerializable, or NULL.
		 */
		static CObjectSerializable* getSerializableByPath(CObjectSerializable* obj, const char* path);

		/**
		 * @brief Reset the canvas to default state after import (position, size, background color).
		 * @param canvas Pointer to canvas to reset.
		 */
		static void reset(CCanvas* canvas);

	protected:
		/**
		 * @brief Internal loading step for canvas from XML reader.
		 * @param canvas Pointer to CCanvas.
		 * @param reader XML reader.
		 * @return True if loading is finished.
		 */
		static bool loadStep(CCanvas* canvas, io::IXMLReader* reader);

		/**
		 * @brief Build the initial canvas structure from XML reader.
		 * @param canvas Pointer to CCanvas.
		 * @param reader XML reader.
		 */
		static void buildCanvas(CCanvas* canvas, io::IXMLReader* reader);

		/**
		 * @brief Internal loading step for a serializable object from XML reader.
		 * @param obj Serializable object.
		 * @param canvas Pointer to CCanvas.
		 * @param reader XML reader.
		 * @return True if loading is finished.
		 */
		static bool loadObjStep(CObjectSerializable* obj, CCanvas* canvas, io::IXMLReader* reader);

		/**
		 * @brief Load all children for a serializable object from XML reader.
		 * @param obj Serializable object.
		 * @param canvas Pointer to CCanvas.
		 * @param reader XML reader.
		 * @return True if loading is finished.
		 */
		static bool loadObjChilds(CObjectSerializable* obj, CCanvas* canvas, io::IXMLReader* reader);

		static CGUIElement* createElementByType(const wchar_t* type, CCanvas* canvas, CGUIElement* parent);
	};
}
