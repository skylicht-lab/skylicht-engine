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

#include "IFont.h"
#include "CSpriteFrame.h"

namespace Skylicht
{
	/// @brief This object class is designed to load fonts, which are created using the BMFont software (https://www.angelcode.com/products/bmfont).
	/// 
	/// Use the CGraphics2D object to draw text with the font.
	/// @ingroup Graphics2D
	class SKYLICHT_API CSpriteFont :
		public CSpriteFrame,
		public IFont
	{
	protected:
		int* m_moduleMap;
		int m_numModuleMap;

	public:
		CSpriteFont();

		virtual ~CSpriteFont();

		bool loadFont(const char* fileName);

		virtual SModuleOffset* getCharacterModule(int character);
	};
}