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

#include "GUI/Type.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CRenderer
			{
			protected:
				SPoint m_renderOffset;
				SRect m_rectClipRegion;

			public:

				static CRenderer* getRenderer();

				static void setRenderer(CRenderer* r);

				CRenderer();

				virtual ~CRenderer();

				virtual void begin() {}

				virtual void end() {}

				virtual void resize(float w, float h) {}

				virtual void setDrawColor(SColor color) {}

				virtual void startClip() {}

				virtual void endClip() {}

				virtual void drawFillRect(const SRect &r, const SGUIColor& color) {}

				virtual void renderText(const SRect &r, EFontSize fontSize, const SGUIColor& textColor, const std::wstring& string) {}

				virtual SDimension measureText(EFontSize fontSize, const std::wstring& string) = 0;

				void setClipRegion(const SRect& rect)
				{
					m_rectClipRegion = rect;
				}

				void addClipRegion(SRect rect);

				bool clipRegionVisible();

				const SRect& clipRegion() const
				{
					return m_rectClipRegion;
				}

				void setRenderOffset(const SPoint& offset)
				{
					m_renderOffset = offset;
				}

				void addRenderOffset(const SRect& offset)
				{
					m_renderOffset.X = m_renderOffset.X + offset.X;
					m_renderOffset.Y = m_renderOffset.Y + offset.Y;
				}

				const SPoint& getRenderOffset() const
				{
					return m_renderOffset;
				}
			};
		}
	}
}