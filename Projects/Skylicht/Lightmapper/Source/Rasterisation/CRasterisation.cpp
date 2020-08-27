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
			m_currentPass(Space4A),
			m_interpolationThreshold(2.0f)
		{
			int size = width * height;

			m_bakedData = new bool[size];

			m_testBakedData = new unsigned char[size * 3];
			m_lightmapData = new unsigned char[size * 3];

			for (int i = 0; i < size; i++)
			{
				m_bakedData[i] = false;

				m_testBakedData[i * 3] = 0;
				m_testBakedData[i * 3 + 1] = 0;
				m_testBakedData[i * 3 + 2] = 0;

				m_lightmapData[i * 3] = 0;
				m_lightmapData[i * 3 + 1] = 0;
				m_lightmapData[i * 3 + 2] = 0;
			}
		}

		CRasterisation::~CRasterisation()
		{
			delete[] m_bakedData;
			delete[] m_testBakedData;
			delete[] m_lightmapData;
		}

		void CRasterisation::resetBake()
		{
			int size = m_width * m_height;

			m_currentPass = Space4A;

			for (int i = 0; i < size; i++)
			{
				m_bakedData[i] = false;

				m_testBakedData[i * 3] = 0;
				m_testBakedData[i * 3 + 1] = 0;
				m_testBakedData[i * 3 + 2] = 0;

				m_lightmapData[i * 3] = 0;
				m_lightmapData[i * 3 + 1] = 0;
				m_lightmapData[i * 3 + 2] = 0;
			}
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

		bool CRasterisation::isInterpolateX(ERasterPass pass)
		{
			bool ret = false;
			switch (pass)
			{
			case Space4A:
				break;
			case Space2BX:
				ret = true;
				break;
			case Space2BY:
				break;
			case Space4C:
				ret = true;
				break;
			case Space1DX:
				ret = true;
				break;
			case Space1DY:
				break;
			case Space1E:
				ret = true;
				break;
			default:
				break;
			}

			return ret;
		}

		bool CRasterisation::isInterpolateY(ERasterPass pass)
		{
			bool ret = false;
			switch (pass)
			{
			case Space4A:
				break;
			case Space2BX:
				break;
			case Space2BY:
				ret = true;
				break;
			case Space4C:
				ret = true;
				break;
			case Space1DX:
				break;
			case Space1DY:
				ret = true;
				break;
			case Space1E:
				ret = true;
				break;
			default:
				break;
			}

			return ret;
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

			// this pixel is baked
			int dataOffset = y * m_width + x;

			// finish
			if (isFinished(lmPixel) == true)
				return false;

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

			bool useInterpolate = false;

			if (m_currentPass >= Space2BX)
			{
				useInterpolate = tryInterpolate(x, y);
				if (useInterpolate == true)
				{
					// fill test color
					m_testBakedData[dataOffset * 3] = 10;
					m_testBakedData[dataOffset * 3 + 1] = 0;
					m_testBakedData[dataOffset * 3 + 2] = 0;
				}
			}

			if (useInterpolate == false)
			{
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
				m_testBakedData[dataOffset * 3] = 255;
				m_testBakedData[dataOffset * 3 + 1] = 0;
				m_testBakedData[dataOffset * 3 + 2] = 0;
			}

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

		bool CRasterisation::isBaked(int x, int y)
		{
			int dataOffset = y * m_width + x;
			return m_bakedData[dataOffset];
		}

		void CRasterisation::getLightmapPixel(int x, int y, float *color)
		{
			int dataOffset = y * m_width + x;

			u8 r = m_lightmapData[dataOffset * 3];
			u8 g = m_lightmapData[dataOffset * 3 + 1];
			u8 b = m_lightmapData[dataOffset * 3 + 2];

			color[0] = (float)r;
			color[1] = (float)g;
			color[2] = (float)b;
		}

		bool CRasterisation::tryInterpolate(int x, int y)
		{
			bool interpolateX = isInterpolateX(m_currentPass);
			bool interpolateY = isInterpolateY(m_currentPass);

			int d = getPixelStep(m_currentPass) / 2;

			float neighbors[4][3];

			int neighborCount = 0;
			int neighborsExpected = 0;

			if (interpolateX == true)
			{
				neighborsExpected += 2;
				if (x - d >= m_uvMin.X && x + d <= m_uvMax.X)
				{
					float c[3];

					if (isBaked(x - d, y) == true)
					{
						getLightmapPixel(x - d, y, c);
						neighbors[neighborCount][0] = c[0];
						neighbors[neighborCount][1] = c[1];
						neighbors[neighborCount][2] = c[2];
						neighborCount++;
					}

					if (isBaked(x + d, y) == true)
					{
						getLightmapPixel(x + d, y, c);
						neighbors[neighborCount][0] = c[0];
						neighbors[neighborCount][1] = c[1];
						neighbors[neighborCount][2] = c[2];
						neighborCount++;
					}
				}
			}

			if (interpolateY == true)
			{
				neighborsExpected += 2;
				if (y - d >= m_uvMin.Y && y + d <= m_uvMax.Y)
				{
					float c[3];

					if (isBaked(x, y - d) == true)
					{
						getLightmapPixel(x, y - d, c);
						neighbors[neighborCount][0] = c[0];
						neighbors[neighborCount][1] = c[1];
						neighbors[neighborCount][2] = c[2];
						neighborCount++;
					}

					if (isBaked(x, y + d) == true)
					{
						getLightmapPixel(x, y + d, c);
						neighbors[neighborCount][0] = c[0];
						neighbors[neighborCount][1] = c[1];
						neighbors[neighborCount][2] = c[2];
						neighborCount++;
					}
				}
			}

			if (neighborCount == neighborsExpected)
			{
				// calculate average neighbor pixel value
				float avg[3] = { 0 };
				for (int i = 0; i < neighborCount; i++)
				{
					avg[0] += neighbors[i][0];
					avg[1] += neighbors[i][1];
					avg[2] += neighbors[i][2];
				}

				float ni = 1.0f / (float)neighborCount;
				for (int j = 0; j < 3; j++)
					avg[j] *= ni;

				// check if error from average pixel to neighbors is above the interpolation threshold
				bool interpolate = true;

				for (int i = 0; i < neighborCount; i++)
				{
					float c[3] = { 0 };
					c[0] = neighbors[i][0];
					c[1] = neighbors[i][1];
					c[2] = neighbors[i][2];

					for (int j = 0; j < 3; j++)
					{
						if (fabs(c[j] - avg[j]) > m_interpolationThreshold)
						{
							interpolate = false;
							break;
						}
					}

					if (!interpolate)
						break;
				}

				if (interpolate == true)
				{
					int dataOffset = y * m_width + x;

					m_lightmapData[dataOffset * 3] = (u8)(avg[0]);
					m_lightmapData[dataOffset * 3 + 1] = (u8)(avg[1]);
					m_lightmapData[dataOffset * 3 + 2] = (u8)(avg[2]);
				}

				return interpolate;
			}

			return false;
		}

		void CRasterisation::flushPixel(std::vector<CSH9>& bakeResults)
		{
			int n = (int)m_bakePixels.size();

			core::vector3df result;
			float r, g, b;
			u32 colorR, colorG, colorB;
			int dataOffset;

#pragma omp parallel for private(result, r, g, b, colorR, colorG, colorB, dataOffset)
			for (int i = 0; i < n; i++)
			{
				SBakePixel& p = m_bakePixels[i];

				bakeResults[i].getSHIrradiance(p.Normal, result);

				const core::vector3df& ambient = bakeResults[i].getValue()[0];

				// dark multipler
				float l = 1.0f - core::clamp(0.21f * result.X + 0.72f * result.Y + 0.07f * result.Z, 0.0f, 1.0f);

				// use QuadraticEaseIn function (y = x^2) or CubicEaseIn (y = x^3)
				// [x -> 0.0 - 1.0] 
				// [y -> 1.0 - 1.5]
				float darkMultipler = 1.0f + 1.5f * l * l * l;

				// compress lighting by 3.0
				result *= darkMultipler / 3.0f;

				r = core::clamp(result.X, 0.0f, 1.0f);
				g = core::clamp(result.Y, 0.0f, 1.0f);
				b = core::clamp(result.Z, 0.0f, 1.0f);

				colorR = (u32)(r * 255.0f);
				colorG = (u32)(g * 255.0f);
				colorB = (u32)(b * 255.0f);

				dataOffset = p.Pixel.Y * m_width + p.Pixel.X;

				m_lightmapData[dataOffset * 3] = colorR;
				m_lightmapData[dataOffset * 3 + 1] = colorG;
				m_lightmapData[dataOffset * 3 + 2] = colorB;
			}

			m_bakePixels.set_used(0);
		}

		void CRasterisation::imageDilate()
		{
			int c = 3;

			u32 sizeLightmapImage = m_width * m_height * 3;
			unsigned char *tempData = new unsigned char[sizeLightmapImage];

			for (int y = 0; y < m_height; y++)
			{
				for (int x = 0; x < m_width; x++)
				{
					f32 color[4];

					bool valid = false;

					for (int i = 0; i < c; i++)
					{
						color[i] = m_lightmapData[(y * m_width + x) * c + i];
						valid |= color[i] > 0.0f;
					}

					if (!valid)
					{
						int n = 0;
						const int dx[] = { -1, 0, 1,  0 };
						const int dy[] = { 0, 1, 0, -1 };

						for (int d = 0; d < 4; d++)
						{
							int cx = x + dx[d];
							int cy = y + dy[d];

							if (cx >= 0 && cx < m_width && cy >= 0 && cy < m_height)
							{
								f32 dcolor[4];

								bool dvalid = false;

								for (int i = 0; i < c; i++)
								{
									dcolor[i] = (f32)m_lightmapData[(cy * m_width + cx) * c + i];
									dvalid |= dcolor[i] > 0.0f;
								}
								if (dvalid)
								{
									for (int i = 0; i < c; i++)
										color[i] += dcolor[i];
									n++;
								}
							}
						}
						if (n)
						{
							float in = 1.0f / n;
							for (int i = 0; i < c; i++)
								color[i] *= in;
						}
					}

					for (int i = 0; i < c; i++)
					{
						tempData[(y * m_width + x) * c + i] = (u8)color[i];
					}
				}
			}

			memcpy(m_lightmapData, tempData, sizeLightmapImage);
			delete tempData;
		}

		void CRasterisation::save(CMemoryStream* stream)
		{
			int size = m_width * m_height;

			// write size
			stream->writeInt(m_width);
			stream->writeInt(m_height);

			// write current task
			stream->writeInt((int)m_currentPass);

			stream->writeInt(m_uvMin.X);
			stream->writeInt(m_uvMin.Y);

			stream->writeInt(m_uvMax.X);
			stream->writeInt(m_uvMax.Y);

			for (int i = 0; i < 3; i++)
			{
				stream->writeFloatArray(&m_uvf[i].X, 3);
				stream->writeFloatArray(&m_position[i].X, 3);
				stream->writeFloatArray(&m_uv[i].X, 3);
				stream->writeFloatArray(&m_normal[i].X, 3);
				stream->writeFloatArray(&m_tangent[i].X, 3);
			}

			// write queue pixels
			u32 queuePixelsCount = m_bakePixels.size();
			stream->writeUInt(queuePixelsCount);

			for (u32 i = 0; i < queuePixelsCount; i++)
			{
				SBakePixel& p = m_bakePixels[i];

				stream->writeInt(p.Pixel.X);
				stream->writeInt(p.Pixel.Y);

				stream->writeFloatArray(&p.Position.X, 3);
				stream->writeFloatArray(&p.Normal.X, 3);
				stream->writeFloatArray(&p.Tangent.X, 3);
				stream->writeFloatArray(&p.Binormal.X, 3);

				core::vector3df* sh = p.SH.getValue();
				for (int j = 0; j < 9; j++)
					stream->writeFloatArray(&sh[j].X, 3);
			}

			// write data
			stream->writeData(m_bakedData, sizeof(bool) * size);
			stream->writeData(m_testBakedData, size * 3);
			stream->writeData(m_lightmapData, size * 3);
		}

		void CRasterisation::load(CMemoryStream* stream)
		{
			// read size
			m_width = stream->readInt();
			m_height = stream->readInt();

			int size = m_width * m_height;

			// read current task
			m_currentPass = (ERasterPass)stream->readInt();

			m_uvMin.X = stream->readInt();
			m_uvMin.Y = stream->readInt();

			m_uvMax.X = stream->readInt();
			m_uvMax.Y = stream->readInt();

			for (int i = 0; i < 3; i++)
			{
				stream->readFloatArray(&m_uvf[i].X, 3);
				stream->readFloatArray(&m_position[i].X, 3);
				stream->readFloatArray(&m_uv[i].X, 3);
				stream->readFloatArray(&m_normal[i].X, 3);
				stream->readFloatArray(&m_tangent[i].X, 3);
			}

			// read bake queue
			m_bakePixels.clear();
			u32 queuePixelsCount = stream->readUInt();
			for (u32 i = 0; i < queuePixelsCount; i++)
			{
				m_bakePixels.push_back(SBakePixel());
				SBakePixel& p = m_bakePixels[i];

				p.Pixel.X = stream->readInt();
				p.Pixel.Y = stream->readInt();

				stream->readFloatArray(&p.Position.X, 3);
				stream->readFloatArray(&p.Normal.X, 3);
				stream->readFloatArray(&p.Tangent.X, 3);
				stream->readFloatArray(&p.Binormal.X, 3);

				core::vector3df* sh = p.SH.getValue();
				for (int j = 0; j < 9; j++)
					stream->readFloatArray(&sh[j].X, 3);
			}

			// read data
			if (m_bakedData != NULL)
				delete[] m_bakedData;

			m_bakedData = new bool[size];
			stream->readData(m_bakedData, sizeof(bool) * size);

			if (m_testBakedData != NULL)
				delete[] m_testBakedData;

			m_testBakedData = new unsigned char[size * 3];
			stream->readData(m_testBakedData, size * 3);

			if (m_lightmapData != NULL)
				delete[] m_lightmapData;

			m_lightmapData = new unsigned char[size * 3];
			stream->readData(m_lightmapData, size * 3);
		}
	}
}