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
#include "CGPUBaker.h"
#include "CLightmapper.h"

#include "RenderPipeline/CBaseRP.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		CGPUBaker::CGPUBaker()
		{
			IVideoDriver* driver = getVideoDriver();

			// load compute shader
			if (driver->getDriverType() == video::EDT_DIRECT3D11)
			{
				// output buffer
				m_shBuffer = driver->createRWBuffer(video::ECF_A32B32G32R32F, MAX_NUM_THREAD * NUM_FACES * 9);

				// gpu compute program
				m_shCompute = driver->getGPUProgrammingServices()->createComputeProgramFromFile("BuiltIn/Shader/Compute/HLSL/IrradianceSH.hlsl");

				// uniform matrix
				m_tangentToSpaceData = new float[MAX_NUM_THREAD * NUM_FACES * 16];
			}
			else
			{
				m_shBuffer = NULL;
				m_shCompute = NULL;
				m_tangentToSpaceData = NULL;
			}
		}

		CGPUBaker::~CGPUBaker()
		{
			if (m_shBuffer != NULL)
				m_shBuffer->drop();

			if (m_shCompute != NULL)
				m_shCompute->drop();

			if (m_tangentToSpaceData != NULL)
				delete m_tangentToSpaceData;
		}

		bool CGPUBaker::canUseGPUBaker()
		{
			if (getVideoDriver()->getDriverType() == video::EDT_DIRECT3D11 &&
				m_shBuffer != NULL &&
				m_shCompute != NULL)
				return true;

			return false;
		}

		void CGPUBaker::computeSH(int count, int numFace)
		{
			// render target size
			u32 rtSize = CLightmapper::getHemisphereBakeSize();

			// set radiance as texture0
			m_shCompute->setTexture(0, m_radiance);

			// set buffer
			m_shCompute->setBuffer(0, m_shBuffer);

			int groupThreadSize = 16;

			int numCell = rtSize / groupThreadSize;

			// clear sh value
			for (int tid = 0; tid < count; tid++)
				m_sh[tid].zero();

			// set const buffer
			s32 uToTangentSpace = m_shCompute->getVariableID("uToTangentSpace");
			if (uToTangentSpace >= 0)
			{
				for (int i = 0; i < MAX_NUM_THREAD * NUM_FACES; i++)
					memcpy(&m_tangentToSpaceData[i * 16], m_toTangentSpace[i].pointer(), sizeof(float) * 16);

				// update compute constance
				m_shCompute->setVariable(uToTangentSpace, m_tangentToSpaceData, MAX_NUM_THREAD * NUM_FACES * 16);
			}

			for (int loopY = 0; loopY < numCell; loopY++)
			{
				for (int loopX = 0; loopX < numCell; loopX++)
				{
					s32 uPixelOffset = m_shCompute->getVariableID("uPixelOffset");
					if (uPixelOffset >= 0)
					{
						core::vector2df offset;
						offset.X = (float)(loopX * groupThreadSize);
						offset.Y = (float)(loopY * groupThreadSize);
						m_shCompute->setVariable(uPixelOffset, &offset.X, 2);
					}

					s32 uFaceSize = m_shCompute->getVariableID("uFaceSize");
					if (uFaceSize >= 0)
					{
						core::vector2df faceSize;
						faceSize.X = (float)rtSize;
						faceSize.Y = (float)rtSize;
						m_shCompute->setVariable(uFaceSize, &faceSize.X, 2);
					}

					// run thread
					m_shCompute->dispatch(count, numFace, 1);
				}
			}

			// get result buffer data
			video::SVec4* data = (video::SVec4*)m_shBuffer->lock(true);

			// copy SH value compute from GPU to data
			for (int tid = 0; tid < count; tid++)
			{
				core::vector3df shResult[9];

				// sum sh each face
				for (int fid = 0; fid < numFace; fid++)
				{
					video::SVec4* computeResult = &data[(tid * NUM_FACES + fid) * 9];

					for (int i = 0; i < 9; i++)
					{
						shResult[i].X += computeResult[i].X;
						shResult[i].Y += computeResult[i].Y;
						shResult[i].Z += computeResult[i].Z;
					}
				}

				core::vector3df* shValue = m_sh[tid].getValue();

				for (int i = 0; i < 9; i++)
					shValue[i] = shResult[i];
			}

			m_shBuffer->unlock();

			// finalWeight is weight for 1 pixel on Sphere
			// S = 4 * PI * R^2
			float finalWeight = (4.0f * 3.14159f) / (m_weightSum * numFace);
			for (int tid = 0; tid < count; tid++)
			{
				m_sh[tid] *= finalWeight;
			}

			// test radiance
			/*
			static int t = 0;
			static bool test = true;
			if (test == true)
			{
				char filename[512];
				sprintf(filename, "test_%d.png", t);
				CBaseRP::saveFBOToFile(m_radiance, filename);
				test = true;
			}
			t++;
			*/
		}
	}
}