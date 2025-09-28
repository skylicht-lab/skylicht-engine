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
	/// @brief Utility class for exporting Canvas and GUI element trees to files or serializable data.
	/// @ingroup Graphics2D
	/// 
	/// CGUIExporter can be used to save a CCanvas to an XML file, or convert GUI elements to  
	/// serializable objects for other forms of storage or transfer.It supports recursive export 
	/// of child elements and can generate info files listing all element paths.
	///
	/// Example usage :
	/// @code
	/// CGUIExporter::save("ui_layout.gui", canvas);
	/// @endcode
	class SKYLICHT_API CGUIExporter
	{
	public:
		/**
		* @brief Save the specified canvas and its elements to a file (XML format).
		* Also generates a secondary info file with element paths.
		* @param file The path to save the file.
		* @param canvas Pointer to the canvas to export.
		* @return True if successful, false otherwise.
		*/
		static bool save(const char* file, CCanvas* canvas);

		/**
		 * @brief Create a serializable object representing the GUI element and its hierarchy.
		 * @param ui Pointer to the root GUI element.
		 * @return Pointer to the root CObjectSerializable.
		 */
		static CObjectSerializable* createSerializable(CGUIElement* ui);

	private:

		/**
		 * @brief Recursively add all child GUI elements to the serializable parent.
		 * @param parent Parent GUI element.
		 * @param parents Parent CObjectSerializable.
		 */
		static void addChild(CGUIElement* parent, CObjectSerializable* parents);

		/**
		 * @brief Save an info file listing all GUI element names and paths.
		 * @param file Path to save the info file.
		 * @param data Root serializable object.
		 */
		static void saveGUIPath(const char* file, CObjectSerializable* data);
	};
}
