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

#include "Lightmapper/CSH9.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		struct SBakePixel
		{
			core::vector2di Pixel;
			core::vector3df Position;
			core::vector3df Normal;
			core::vector3df Tangent;
			core::vector3df Binormal;

			CSH9 SH;
		};

		class CRasterisation
		{
		public:

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
			* Step 5: Space1D (Interpolate + Bake if needed)
			*         A  D  B  D  A
			*         D     D     D
			*         B  D  C  D  B
			*         D     D     D
			*         A  D  B  D  A
			* Step 6: Space1E (Interpolate + Bake if needed)
			*         A  D  B  D  A
			*         D  E  D  E  D
			*         B  D  C  D  B
			*         D  E  D  E  D
			*         A  D  B  D  A
			*/
			enum ERasterPass
			{
				Space4A = 0,
				Space2BX,
				Space2BY,
				Space4C,
				Space1DX,
				Space1DY,
				Space1E,
				PassCount,
			};

		protected:
			bool *m_bakedData;

			int m_width;
			int m_height;

			core::vector2di m_uvMin;
			core::vector2di m_uvMax;

			core::vector2df m_uvf[3];

			core::vector3df m_position[3];
			core::vector2df m_uv[3];
			core::vector3df m_normal[3];
			core::vector3df m_tangent[3];

			core::array<SBakePixel> m_bakePixels;

			ERasterPass m_currentPass;

			unsigned char *m_lightmapData;
			unsigned char *m_testBakedData;

			float m_interpolationThreshold;

		private:
			core::vector3df sampleVector3(const core::vector3df* p, const core::vector2df& uv);

		public:
			CRasterisation(int width, int height);

			virtual ~CRasterisation();

			void resetBake();

			int getPixelStep(ERasterPass pass);

			int getPassOffsetX(ERasterPass pass);

			int getPassOffsetY(ERasterPass pass);

			bool isInterpolateX(ERasterPass pass);

			bool isInterpolateY(ERasterPass pass);

			core::vector2di setTriangle(
				const core::vector3df *position,
				const core::vector2df *uv,
				const core::vector3df *normal,
				const core::vector3df *tangent,
				ERasterPass pass);

			bool samplingTrianglePosition(
				core::vector3df& outPosition,
				core::vector3df& outNormal,
				core::vector3df& outTangent,
				core::vector3df& outBinormal,
				core::vector2di& lmPixel);

			void imageDilate();

			bool moveNextPixel(core::vector2di& lmPixel);

			bool isFinished(const core::vector2di& lmPixel);

			inline void setInterpolationThreshold(float f)
			{
				m_interpolationThreshold = f;
			}

			inline int getWidth()
			{
				return m_width;
			}

			inline int getHeight()
			{
				return m_height;
			}

			unsigned char* getTestBakeImage()
			{
				return m_testBakedData;
			}

			unsigned char* getLightmapData()
			{
				return m_lightmapData;
			}

			bool tryInterpolate(int x, int y);

			void getLightmapPixel(int x, int y, float *color);

			core::array<SBakePixel>& getBakePixelQueue()
			{
				return m_bakePixels;
			}

			void flushPixel(std::vector<CSH9>& bakeResults);

		};
	}
}