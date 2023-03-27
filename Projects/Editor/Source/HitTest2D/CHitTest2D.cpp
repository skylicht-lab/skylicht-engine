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

#include "pch.h"
#include "CHitTest2D.h"

namespace Skylicht
{
	namespace Editor
	{
		core::vector2df toVec2(const core::vector3df& v)
		{
			return core::vector2df(v.X, v.Y);
		}

		core::vector3df toVec3(const core::vector2df& v)
		{
			return core::vector3df(v.X, v.Y, 0.0f);
		}

		bool CHitTest2D::isLineHit(const core::vector2df& p1, const core::vector2df& p2, const core::vector2df& point)
		{
			const float SelectDistance = 7.0;

			float x1 = p1.X;
			float y1 = p1.Y;

			float x2 = p2.X;
			float y2 = p2.Y;

			float xHit = point.X;
			float yHit = point.Y;

			if (x1 > x2)
			{
				if (xHit < (x2 - SelectDistance) || xHit >(x1 + SelectDistance))
					return false;
			}
			else
			{
				if (xHit < (x1 - SelectDistance) || xHit >(x2 + SelectDistance))
					return false;
			}

			if (y1 > y2)
			{
				if (yHit < (y2 - SelectDistance) || yHit >(y1 + SelectDistance))
					return false;
			}
			else
			{
				if (yHit < (y1 - SelectDistance) || yHit >(y2 + SelectDistance))
					return false;
			}


			core::vector2df v(x2 - x1, y2 - y1);
			core::vector2df w(xHit - x1, yHit - y1);

			float c1 = w.dotProduct(v);
			if (c1 <= 0)
			{
				core::vector2df r(xHit - x1, yHit - y1);
				float distance = r.getLength();

				if (distance <= SelectDistance)
					return true;
			}

			float c2 = v.dotProduct(v);

			if (c2 <= c1)
			{
				core::vector2df r(xHit - x2, yHit - y2);
				float distance = r.getLength();

				if (distance <= SelectDistance)
					return true;
			}

			float b = c1 / c2;

			core::vector2df Pb(x1 + b * v.X, y1 + b * v.Y);

			// distance from point & edge
			core::vector2df r(xHit - Pb.X, yHit - Pb.Y);
			float distance = r.getLength();

			if (distance <= SelectDistance)
				return true;

			return false;
		}

		int CHitTest2D::isRectBorderHit(const core::rectf& rect, const core::matrix4& transform, const core::vector2df& point)
		{
			int ret = 0;

			core::vector3df p1(rect.UpperLeftCorner.X, rect.UpperLeftCorner.Y, 0.0f);
			core::vector3df p2(rect.LowerRightCorner.X, rect.UpperLeftCorner.Y, 0.0f);
			core::vector3df p3(rect.LowerRightCorner.X, rect.LowerRightCorner.Y, 0.0f);
			core::vector3df p4(rect.UpperLeftCorner.X, rect.LowerRightCorner.Y, 0.0f);

			transform.transformVect(p1);
			transform.transformVect(p2);
			transform.transformVect(p3);
			transform.transformVect(p4);

			if (isLineHit(toVec2(p1), toVec2(p2), point))
				ret |= CHitTest2D::Top;

			if (isLineHit(toVec2(p3), toVec2(p4), point))
				ret |= CHitTest2D::Bottom;

			if (isLineHit(toVec2(p1), toVec2(p4), point))
				ret |= CHitTest2D::Left;

			if (isLineHit(toVec2(p2), toVec2(p3), point))
				ret |= CHitTest2D::Right;

			return ret;
		}

		bool CHitTest2D::isInsideRect(const core::rectf& rect, const core::matrix4& transform, const core::vector2df& point)
		{
			core::vector3df p1(rect.UpperLeftCorner.X, rect.UpperLeftCorner.Y, 0.0f);
			core::vector3df p2(rect.LowerRightCorner.X, rect.UpperLeftCorner.Y, 0.0f);
			core::vector3df p3(rect.LowerRightCorner.X, rect.LowerRightCorner.Y, 0.0f);
			core::vector3df p4(rect.UpperLeftCorner.X, rect.LowerRightCorner.Y, 0.0f);

			transform.transformVect(p1);
			transform.transformVect(p2);
			transform.transformVect(p3);
			transform.transformVect(p4);

			core::triangle3df t1(p1, p2, p3);
			core::triangle3df t2(p1, p3, p4);

			core::vector3df p = toVec3(point);

			return t1.isPointInside(p) || t2.isPointInside(p);
		}

		bool CHitTest2D::isInsidePoly(
			const core::vector2df& p1,
			const core::vector2df& p2,
			const core::vector2df& p3,
			const core::vector2df& p4,
			const core::vector2df& point)
		{
			core::triangle3df t1(toVec3(p1), toVec3(p2), toVec3(p3));
			core::triangle3df t2(toVec3(p1), toVec3(p3), toVec3(p4));

			core::vector3df p = toVec3(point);

			return t1.isPointInside(p) || t2.isPointInside(p);
		}
	}
}