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
#include "SutherlandHodgman.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		CRasterisation::CRasterisation(int width, int height) :
			m_width(width),
			m_height(height),
			m_currentPass(Space4A)
		{
			int size = width * height;

			m_bakeResult = new CSH9[size];
			m_bakedData = new bool[size];

			m_testBakedData = new unsigned char[size * 3];

			for (int i = 0; i < size; i++)
			{
				m_bakedData[i] = false;
				
				m_testBakedData[i * 3] = 0;
				m_testBakedData[i * 3 + 1] = 0;
				m_testBakedData[i * 3 + 2] = 0;
			}
		}

		CRasterisation::~CRasterisation()
		{
			delete[] m_bakeResult;
			delete[] m_bakedData;
			delete[] m_testBakedData;
		}

		/*
			* ERasterPass
			* http://web.archive.org/web/20160311085440/http://freespace.virgin.net/hugo.elias/radiosity/radiosity.htm
			* Section: Increasing the accuracy of the solution
			*
			* Step 1: Space4A (All bake)
			*         A           A
			*
			*
			*
			*         A           A
			* Step 2,3: Space2BX, Space2BY (Interpolate + Bake if needed)
			*         A     B     A
			*
			*         B           B
			*
			*         A     B     A
			* Step 4: Space4C (Interpolate + Bake if needed)
			*         A     B     A
			*
			*         B     C     B
			*
			*         A     B     A
			* Step 5, 6: Space1DX, Space1DY (Interpolate + Bake if needed)
			*         A  D  B  D  A
			*         D     D     D
			*         B  D  C  D  B
			*         D     D     D
			*         A  D  B  D  A
			* Step 7: Space1E (Interpolate + Bake if needed)
			*         A  D  B  D  A
			*         D  E  D  E  D
			*         B  D  C  D  B
			*         D  E  D  E  D
			*         A  D  B  D  A
			*/

		int CRasterisation::getPixelStep(ERasterPass pass)
		{
			int offset = 4;
			switch (pass)
			{
			case Space4A:
			case Space2BX:
			case Space2BY:
			case Space4C:
				offset = 4;
				break;
			case Space1DX:
			case Space1DY:
			case Space1E:
				offset = 2;
				break;
			default:
				break;
			}
			return offset;
		}

		int CRasterisation::getPassOffsetX(ERasterPass pass)
		{
			int offsetX = 0;
			switch (pass)
			{
			case Space4A:
				break;
			case Space2BX:
				offsetX = 2;
				break;
			case Space2BY:
				break;
			case Space4C:
				offsetX = 2;
				break;
			case Space1DX:
				offsetX = 1;
				break;
			case Space1DY:
				break;
			case Space1E:
				offsetX = 1;
				break;
			default:
				break;
			}
			return offsetX;
		}

		int CRasterisation::getPassOffsetY(ERasterPass pass)
		{
			int offsetY = 0;
			switch (pass)
			{
			case Space4A:
				break;
			case Space2BX:
				break;
			case Space2BY:
				offsetY = 2;
				break;
			case Space4C:
				offsetY = 2;
				break;
			case Space1DX:
				break;
			case Space1DY:
				offsetY = 1;
				break;
			case Space1E:
				offsetY = 1;
				break;
			default:
				break;
			}
			return offsetY;
		}

		core::vector2di CRasterisation::setTriangle(
			const core::vector3df *position,
			const core::vector2df *uv,
			const core::vector3df *normal,
			const core::vector3df *tangent,
			ERasterPass pass)
		{
			core::vector2df minUV, maxUV;

			minUV = uv[0];
			maxUV = uv[0];

			m_currentPass = pass;

			for (int i = 0; i < 3; i++)
			{
				// copy data
				m_position[i] = position[i];
				m_uv[i] = uv[i];
				m_normal[i] = normal[i];
				m_tangent[i] = tangent[i];

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
			core::vector2di pixel = m_uvMin;

			pixel.X += getPassOffsetX(m_currentPass);
			pixel.Y += getPassOffsetY(m_currentPass);

			return pixel;
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

		core::vector3df CRasterisation::sampleVector3(const core::vector3df* p, const core::vector2df& uv)
		{
			const core::vector3df& p0 = p[0];
			const core::vector3df& p1 = p[1];
			const core::vector3df& p2 = p[2];

			core::vector3df v1 = p1 - p0;
			core::vector3df v2 = p2 - p0;

			return p0 + ((v2 * uv.X) + (v1 * uv.Y));
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
			core::vector2df poly[4];
			poly[0] = core::vector2df(fx, fy);
			poly[1] = core::vector2df(fx + 1.0f, fy);
			poly[2] = core::vector2df(fx + 1.0f, fy + 1.0f);
			poly[3] = core::vector2df(fx, fy + 1.0f);

			core::vector2df res[16];
			int nRes = 0;

			// clip rect poly with triangle
			SutherlandHodgman(m_uvf, 3, poly, 4, res, nRes);
			if (nRes == 0)
			{
				return false;
			}

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

			// set baked
			m_bakedData[dataOffset] = true;

			m_bakePixels.push_back(SBakePixel());
			SBakePixel &p = m_bakePixels.getLast();
			p.Pixel = lmPixel;

			// calc position 			
			p.Position = sampleVector3(m_position, uv);

			// calc normal
			p.Normal = sampleVector3(m_normal, uv);
			p.Normal.normalize();

			// calc tangent
			p.Tangent = sampleVector3(m_tangent, uv);
			p.Tangent.normalize();

			// calc binormal
			p.Binormal = p.Normal.crossProduct(p.Tangent);
			p.Binormal.normalize();

			// fill test color
			m_testBakedData[dataOffset * 3] = m_randomColor.getRed();
			m_testBakedData[dataOffset * 3 + 1] = m_randomColor.getGreen();
			m_testBakedData[dataOffset * 3 + 2] = m_randomColor.getBlue();

			// bake pixel
			return true;
		}

		bool CRasterisation::moveNextPixel(core::vector2di& lmPixel)
		{
			int step = getPixelStep(m_currentPass);
			lmPixel.X += step;

			if (lmPixel.X >= m_uvMax.X)
			{
				lmPixel.X = m_uvMin.X + getPassOffsetX(m_currentPass);
				lmPixel.Y += step;

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