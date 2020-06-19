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

#include "pch.h"
#include "CRasterisation.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		CRasterisation::CRasterisation(int width, int height) :
			m_width(width),
			m_height(height)
		{
			int size = width * height;

			m_bakeResult = new CSH9[size];
			m_bakedData = new bool[size];
			m_imageData = new unsigned char[size * 3];

			for (int i = 0; i < size; i++)
			{
				m_bakedData[i] = false;
				m_imageData[i * 3] = 0;
				m_imageData[i * 3 + 1] = 0;
				m_imageData[i * 3 + 2] = 0;
			}
		}

		CRasterisation::~CRasterisation()
		{
			delete[] m_bakeResult;
			delete[] m_bakedData;
			delete[] m_imageData;
		}

		core::vector2di CRasterisation::setTriangle(
			const core::vector3df *position,
			const core::vector2df *uv,
			const core::vector3df *normal,
			const core::vector3df *tangent,
			const core::vector3df *binormal)
		{
			core::vector2df minUV, maxUV;

			minUV = uv[0];
			maxUV = uv[0];

			for (int i = 0; i < 3; i++)
			{
				// copy data
				m_position[i] = position[i];
				m_uv[i] = uv[i];
				m_normal[i] = normal[i];
				m_tangent[i] = tangent[i];
				m_binormal[i] = binormal[i];

				float x = m_uv[i].X;
				float y = m_uv[i].Y;

				minUV.X = core::min_(minUV.X, x);
				minUV.Y = core::min_(minUV.Y, y);
				maxUV.X = core::max_(maxUV.X, x);
				maxUV.Y = core::max_(maxUV.Y, y);

				m_uvf[i].X = fmodf(x, 1.0f) * (float)m_width;
				m_uvf[i].Y = fmodf(y, 1.0f) * (float)m_height;
			}

			// calc bound triangle in uv coord			
			m_uvMin.X = (int)(fmodf(minUV.X, 1.0f) * (float)m_width);
			m_uvMin.Y = (int)(fmodf(minUV.Y, 1.0f) * (float)m_height);
			m_uvMax.X = (int)(fmodf(maxUV.X, 1.0f) * (float)m_width);
			m_uvMax.Y = (int)(fmodf(maxUV.Y, 1.0f) * (float)m_height);

			// offset 1 pixel
			m_uvMin.X = core::max_(m_uvMin.X - 1, 0);
			m_uvMin.Y = core::max_(m_uvMin.Y - 1, 0);
			m_uvMax.X = core::min_(m_uvMax.X + 1, m_width - 1);
			m_uvMax.Y = core::min_(m_uvMax.Y + 1, m_height - 1);

			// random color
			m_randomColor.setRed(u32((rand() % 255 + 192) * 0.5f));
			m_randomColor.setGreen(u32((rand() % 255 + 192) * 0.5f));
			m_randomColor.setBlue(u32((rand() % 255 + 192) * 0.5f));
			m_randomColor.setAlpha(255);

			// return first lm pixel of triangle
			return m_uvMin;
		}

		// pseudo cross product
		static inline float cross2(const core::vector2df& a, const core::vector2df& b)
		{
			return a.X * b.Y - a.Y * b.X;
		}

		static inline int leftOf(const core::vector2df& a, const core::vector2df& b, const core::vector2df& c)
		{
			core::vector2df v1 = b - a;
			core::vector2df v2 = c - b;
			float x = cross2(v1, v2);
			return x < 0 ? -1 : x > 0;
		}

		static bool lineIntersection(
			const core::vector2df& x0,
			const core::vector2df& x1,
			const core::vector2df& y0,
			const core::vector2df& y1,
			core::vector2df& res)
		{
			core::vector2df dx = x1 - x0;
			core::vector2df dy = y1 - y0;
			core::vector2df d = x0 - y0;

			float dyx = cross2(dy, dx);
			if (dyx == 0.0f)
				return false;

			dyx = cross2(d, dx) / dyx;
			if (dyx <= 0 || dyx >= 1)
				return false;

			res.X = y0.X + dyx * dy.X;
			res.X = y0.Y + dyx * dy.Y;
			return true;
		}

		int isClipPolyInside(core::vector2df *poly, int polyCount, core::vector2df *clip, int clipCount, core::vector2df *res)
		{
			int nRes = polyCount;

			int dir = leftOf(clip[0], clip[1], clip[2]);

			for (int i = 0, j = clipCount - 1; i < clipCount && nRes; j = i++)
			{
				if (i != 0)
				{
					for (int i = 0; i < nRes; i++)
						poly[i] = res[i];
				}

				nRes = 0;

				core::vector2df v0 = poly[polyCount - 1];

				int side0 = leftOf(clip[j], clip[i], v0);
				if (side0 != -dir)
					res[nRes++] = v0;

				for (int k = 0; k < clipCount; k++)
				{
					core::vector2df v1 = poly[k], x;

					int side1 = leftOf(clip[j], clip[i], v1);

					if (side0 + side1 == 0 && side0 && lineIntersection(clip[j], clip[i], v0, v1, x))
						res[nRes++] = x;

					if (k == polyCount - 1)
						break;

					if (side1 != -dir)
						res[nRes++] = v1;

					v0 = v1;
					side0 = side1;
				}
			}

			return nRes;
		}

		static core::vector2df toBarycentric(const core::vector2df& p1, const core::vector2df& p2, const core::vector2df& p3, const core::vector2df& p)
		{
			// http://www.blackpawn.com/texts/pointinpoly/
			// Compute vectors
			core::vector2df v0 = p3 - p1;
			core::vector2df v1 = p2 - p1;
			core::vector2df v2 = p - p1;

			// Compute dot products
			float dot00 = v0.dotProduct(v0);
			float dot01 = v0.dotProduct(v1);
			float dot02 = v0.dotProduct(v2);
			float dot11 = v1.dotProduct(v1);
			float dot12 = v1.dotProduct(v2);

			// Compute barycentric coordinates
			float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
			float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
			float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
			return core::vector2df(u, v);
		}

		bool CRasterisation::samplingTrianglePosition(
			core::vector3df& outPosition,
			core::vector3df& outNormal,
			core::vector3df& outTangent,
			core::vector3df& outBinormal,
			core::vector2di& lmPixel)
		{
			// Reference:
			// function: lm_trySamplingConservativeTriangleRasterizerPosition
			// https://github.com/ands/lightmapper/blob/master/lightmapper.h

			int x = lmPixel.X;
			int y = lmPixel.Y;

			float fx = (float)x;
			float fy = (float)y;

			// finish
			if (isFinished(lmPixel) == true)
				return false;

			// this pixel is baked
			int dataOffset = y * m_width + x;

			if (m_bakedData[dataOffset] == true)
				return false;

			// check pixel inside triangle
			core::vector2df poly[16];
			poly[0] = core::vector2df(fx, fy);
			poly[1] = core::vector2df(fx + 1.0f, fy);
			poly[2] = core::vector2df(fx + 1.0f, fy + 1.0f);
			poly[3] = core::vector2df(fx, fy + 1.0f);

			core::vector2df *res = new core::vector2df[16];
			int nRes = isClipPolyInside(poly, 4, m_uvf, 3, res);
			if (nRes == 0)
			{
				return false;
			}

			/*
			// calculate centroid position and area
			core::vector2df centroid = res[0];
			float area = res[nRes - 1].X * res[0].Y - res[nRes - 1].Y * res[0].X;
			for (int i = 1; i < nRes; i++)
			{
				centroid = centroid + res[i];
				area += res[i - 1].X * res[i].Y - res[i - 1].Y * res[i].X;
			}
			centroid = centroid / (float)nRes;
			area = fabsf(area / 2.0f);

			if (area <= 0.0f)
				return false; // no area left

			core::vector2df uv = toBarycentric(
				m_uvf[0],
				m_uvf[1],
				m_uvf[2],
				centroid);

			if (!isfinite(uv.X) || !isfinite(uv.Y))
				return false; // degenerate
			*/

			// set baked
			m_bakedData[dataOffset] = true;

			// fill color
			m_imageData[dataOffset * 3] = m_randomColor.getRed();
			m_imageData[dataOffset * 3 + 1] = m_randomColor.getGreen();
			m_imageData[dataOffset * 3 + 2] = m_randomColor.getBlue();

			// bake pixel
			return true;
		}

		bool CRasterisation::moveNextPixel(core::vector2di& lmPixel, int offset)
		{
			lmPixel.X += offset;

			if (lmPixel.X >= m_uvMax.X)
			{
				lmPixel.X = m_uvMin.X;
				lmPixel.Y += offset;

				if (isFinished(lmPixel) == true)
					return false;
			}

			return true;
		}

		bool CRasterisation::isFinished(const core::vector2di& lmPixel)
		{
			if (lmPixel.Y >= m_uvMax.Y)
				return true;

			return false;
		}
	}
}