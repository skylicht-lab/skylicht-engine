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

#include "CSH9.h"
#include "Camera/CCamera.h"
#include "RenderPipeline/IRenderPipeline.h"
#include "Entity/CEntityManager.h"

#include "CMTBaker.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		class CGPUBaker : public CMTBaker
		{
		protected:
			IGPUCompute *m_shCompute;
			
			IRWBuffer *m_shBuffer;

			float *m_tangentToSpaceData;
		public:
			CGPUBaker();
			
			virtual ~CGPUBaker();

			bool canUseGPUBaker();

			virtual void computeSH(int count, int numFace);
		};
	}
}