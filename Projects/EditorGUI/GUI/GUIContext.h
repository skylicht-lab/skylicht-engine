/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include "GUI/Controls/CBase.h"
#include "EditorGUIAPI.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CCanvas;

			namespace Context
			{
				EDITOR_GUI_API extern CBase* HoveredControl;
				EDITOR_GUI_API extern CBase* MouseFocus;
				EDITOR_GUI_API extern CBase* KeyboardFocus;

				EDITOR_GUI_API void initGUI(float width, float height);

				EDITOR_GUI_API void destroyGUI();

				EDITOR_GUI_API void update(float time);

				EDITOR_GUI_API float getTime();

				EDITOR_GUI_API float getDeltaTime();

				EDITOR_GUI_API void render();

				EDITOR_GUI_API void resize(float width, float height);

				EDITOR_GUI_API CCanvas* getRoot();

				EDITOR_GUI_API CRenderer* getRenderer();
			};
		}
	}
}