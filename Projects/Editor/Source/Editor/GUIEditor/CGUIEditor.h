/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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

#include "Graphics2D/GUI/CGUIElement.h"
#include "Editor/Serializable/CSerializableEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpaceProperty;

		class CGUIEditor :public CSerializableEditor
		{
		protected:
			CGUIElement* m_gui;
			CObjectSerializable* m_guiData;

		public:
			CGUIEditor();

			virtual ~CGUIEditor();

			virtual void closeGUI();

			virtual void initGUI(CGUIElement* gui, CSpaceProperty* ui);

			virtual void onUpdateValue(CObjectSerializable* object);

			virtual void update() = 0;

			CGUIElement* getGUIElement()
			{
				return m_gui;
			}

			void showEditorRect(bool b);
			void showEditorTransform(bool b);
			void showEditorAlign(bool b);
			void showEditorMargin(EGUIDock dock);
		};
	}
}