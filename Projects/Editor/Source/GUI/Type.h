/*
!@
MIT License

CopyRight (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyRight notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
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
		namespace GUI
		{
			struct SMargin
			{
				SMargin(float left = 0, float top = 0, float right = 0, float bottom = 0)
					: Top(top)
					, Bottom(bottom)
					, Left(left)
					, Right(right)
				{}

				SMargin operator + (const SMargin& margin) const
				{
					SMargin m;
					m.Top = Top + margin.Top;
					m.Bottom = Bottom + margin.Bottom;
					m.Left = Left + margin.Left;
					m.Right = Right + margin.Right;
					return m;
				}

				float Top, Bottom, Left, Right;
			};

			typedef SMargin SPadding;

			enum class EPosition
			{
				None = 0,
				Left = (1 << 1),
				Right = (1 << 2),
				Top = (1 << 3),
				Bottom = (1 << 4),
				CenterV = (1 << 5),
				CenterH = (1 << 6),
				Fill = (1 << 7),
				Center = CenterV | CenterH,
			};

			inline EPosition operator | (EPosition a, EPosition b)
			{
				return EPosition(static_cast<int>(a) | static_cast<int>(b));
			}

			inline bool operator & (EPosition a, EPosition b)
			{
				return (static_cast<int>(a) & static_cast<int>(b)) != 0;
			}

			struct SRect
			{
				float X;
				float Y;
				float Width;
				float Height;

				SRect()
				{
					X = 0.0f;
					Y = 0.0f;
					Width = 0.0f;
					Height = 0.0f;
				}

				SRect(const SRect& r)
				{
					X = r.X;
					Y = r.Y;
					Width = r.Width;
					Height = r.Height;
				}

				SRect(float x, float y, float w, float h)
				{
					X = x;
					Y = y;
					Width = w;
					Height = h;
				}

				bool operator==(const SRect& r)
				{
					return X == r.X && Y == r.Y && Width == r.Width && Height == r.Height;
				}
			};

			struct SPoint
			{
				float X;
				float Y;

				SPoint()
				{
					X = 0.0f;
					Y = 0.0f;
				}

				SPoint(const SPoint& p)
				{
					X = p.X;
					Y = p.Y;
				}

				SPoint(float x, float y)
				{
					X = x;
					Y = y;
				}

				bool operator==(const SPoint& p)
				{
					return X == p.X && Y == p.Y;
				}
			};

			struct SDimension
			{
				float Width;
				float Height;

				SDimension()
				{
					Width = 0.0f;
					Height = 0.0f;
				}

				SDimension(SDimension& p)
				{
					Width = p.Width;
					Height = p.Height;
				}

				SDimension(float w, float h)
				{
					Width = w;
					Height = h;
				}

				bool operator==(const SDimension& p)
				{
					return Width == p.Width && Height == p.Height;
				}
			};
		}
	}
}