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
		class CRasterisation
		{
		protected:
			CSH9 *m_bakeResult;
			
			bool *m_bakedData;
			unsigned char *m_imageData;
			int m_width;
			int m_height;

			core::vector2di m_uvMin;
			core::vector2di m_uvMax;

			core::vector2df m_uvf[3];

			const core::vector3df *m_position;
			const core::vector2df *m_uv;
			const core::vector3df *m_normal;
			const core::vector3df *m_tangent;
			const core::vector3df *m_binormal;

			SColor m_randomColor;
		public:
			CRasterisation(int width, int height);

			virtual ~CRasterisation();

			core::vector2di setTriangle(
				const core::vector3df *position,
				const core::vector2df *uv,
				const core::vector3df *normal,
				const core::vector3df *tangent,
				const core::vector3df *binormal);

			bool samplingTrianglePosition(
				core::vector3df& outPosition,
				core::vector3df& outNormal,
				core::vector3df& outTangent,
				core::vector3df& outBinormal,
				core::vector2di& lmPixel);

			bool moveNextPixel(core::vector2di& lmPixel, int offset);
		};
	}
}