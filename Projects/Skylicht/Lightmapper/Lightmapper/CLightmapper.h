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

#include "Utils/CSingleton.h"
#include "CBaker.h"
#include "CMTBaker.h"
#include "CGPUBaker.h"
#include "LightProbes/CLightProbe.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		class CLightmapper
		{
		public:
			DECLARE_SINGLETON(CLightmapper)

		protected:
			CBaker* m_singleBaker;
			CMTBaker* m_multiBaker;
			CGPUBaker* m_gpuBaker;

			CSH9 m_temp;

		public:
			CLightmapper();

			virtual ~CLightmapper();

			void initBaker(u32 hemisphereBakeSize = 128);

			void release();

			const CSH9& bakeAtPosition(
				CCamera* camera, IRenderPipeline* rp, CEntityManager* entityMgr,
				const core::vector3df& position,
				const core::vector3df& normal,
				const core::vector3df& tangent,
				const core::vector3df& binormal,
				int numFace = NUM_FACES);

			void bakeAtPosition(
				CCamera* camera, IRenderPipeline* rp, CEntityManager* entityMgr,
				const core::vector3df* position,
				const core::vector3df* normal,
				const core::vector3df* tangent,
				const core::vector3df* binormal,
				std::vector<CSH9>& out,
				int count,
				int numFace = NUM_FACES);

			void bakeProbes(std::vector<CLightProbe*>& probes, CCamera* camera, IRenderPipeline* rp, CEntityManager* entityMgr);

			void bakeProbes(std::vector<core::vector3df>& position, std::vector<CSH9>& probes, CCamera* camera, IRenderPipeline* rp, CEntityManager* entityMgr);

			int bakeMeshBuffer(IMeshBuffer* mb, const core::matrix4& transform, CCamera* camera, IRenderPipeline* rp, CEntityManager* entityMgr, int begin, int count, core::array<SColor>& outColor, core::array<CSH9>& outSH);

			static void setNumThread(u32 num);

			static void setHemisphereBakeSize(u32 size);

			static u32 getNumThread();

			static u32 getHemisphereBakeSize();
		};
	}
}