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

namespace Skylicht
{
	namespace Editor
	{
		class CHitTest2D
		{
		public:
			enum EHit
			{
				None = 0,
				Top = 1,
				Bottom = 1 << 2,
				Left = 1 << 3,
				Right = 1 << 4
			};
		public:
			static bool isLineHit(const core::vector2df& p1, const core::vector2df& p2, const core::vector2df& point);

			static int isRectBorderHit(const core::rectf& rect, const core::matrix4& transform, const core::vector2df& point);

			static bool isInsideRect(const core::rectf& rect, const core::matrix4& transform, const core::vector2df& point);

			static bool isInsidePoly(
				const core::vector2df& p1,
				const core::vector2df& p2,
				const core::vector2df& p3,
				const core::vector2df& p4,
				const core::vector2df& point);
		};
	}
}
