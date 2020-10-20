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

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CRenderer
			{
			protected:
				core::vector2df m_renderOffset;
				core::rectf m_rectClipRegion;

			public:

				static CRenderer* getRenderer();

				static void setRenderer(CRenderer* r);

				CRenderer();

				virtual ~CRenderer();

				virtual void setDrawColor(SColor color) {}

				virtual void startClip() {}

				virtual void endClip() {}

				void setClipRegion(core::rectf const& rect)
				{
					m_rectClipRegion = rect;
				}

				void addClipRegion(core::rectf rect);

				bool clipRegionVisible();

				const core::rectf& clipRegion() const
				{
					return m_rectClipRegion;
				}

				void setRenderOffset(const core::vector2df& offset)
				{
					m_renderOffset = offset;
				}

				void addRenderOffset(const core::rectf& offset)
				{
					m_renderOffset += offset.UpperLeftCorner;
				}

				const core::vector2df& getRenderOffset() const
				{
					return m_renderOffset;
				}
			};
		}
	}
}