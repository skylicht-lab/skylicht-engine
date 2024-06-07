#include "pch.h"
#include "CMTBaker.h"
#include "CBakeUtils.h"
#include "GameObject/CGameObject.h"
#include "RenderPipeline/CBaseRP.h"
#include "RenderPipeline/CDeferredRP.h"
#include "Lightmapper/CLightmapper.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		CMTBaker::CMTBaker() :
			m_weightSum(0.0f)
		{
			IVideoDriver* driver = getVideoDriver();

			u32 rtSize = CLightmapper::getHemisphereBakeSize();
			core::dimension2du s(rtSize * NUM_FACES, rtSize * MAX_NUM_THREAD);

			m_radiance = driver->addRenderTargetTexture(s, "lmrt", video::ECF_A8R8G8B8);

			for (u32 y = 0; y < rtSize; y++)
			{
				for (u32 x = 0; x < rtSize; x++)
				{
					float u = ((x / float(rtSize)) * 2.0f - 1.0f);
					float v = -((y / float(rtSize)) * 2.0f - 1.0f);

					float temp = 1.0f + u * u + v * v;
					float weight = 4.0f / (sqrt(temp) * temp);
					m_weightSum = m_weightSum + weight;
				}
			}
		}

		CMTBaker::~CMTBaker()
		{
			IVideoDriver* driver = getVideoDriver();
			driver->removeTexture(m_radiance);
			m_radiance = NULL;
		}

		void CMTBaker::bake(CCamera* camera,
			IRenderPipeline* rp,
			CEntityManager* entityMgr,
			const core::vector3df* position,
			const core::vector3df* normal,
			const core::vector3df* tangent,
			const core::vector3df* binormal,
			int count,
			int numFace)
		{
			IVideoDriver* driver = getVideoDriver();

			// render radiance
			// apply projection
			camera->setAspect(1.0f);
			camera->setFOV(90.0f);

			// clear rtt
			getVideoDriver()->setRenderTarget(m_radiance, true, true);

			u32 rtSize = CLightmapper::getHemisphereBakeSize();

			for (int tid = 0; tid < count; tid++)
			{
				for (int face = 0; face < numFace; face++)
				{
					core::matrix4 cameraWorld;
					core::matrix4 viewToWorld;

					core::recti viewport;

					getWorldView(normal[tid], tangent[tid], binormal[tid], position[tid], face, cameraWorld);

					// to tangent space
					m_toTangentSpace[tid * NUM_FACES + face] = cameraWorld;

					// remove position
					setRow(m_toTangentSpace[tid * NUM_FACES + face], 3, core::vector3df(0.0f, 0.0f, 0.0f), 1.0f);

					// camera world
					camera->getGameObject()->getTransform()->setRelativeTransform(cameraWorld);

					// view matrix is world inverse
					cameraWorld.makeInverse();
					camera->setViewMatrix(cameraWorld, position[tid]);

					// apply viewport
					u32 offsetX = face * rtSize;
					u32 offsetY = tid * rtSize;
					viewport.UpperLeftCorner.set(offsetX, offsetY);
					viewport.LowerRightCorner.set(offsetX + rtSize, offsetY + rtSize);

					// render
					CBaseRP::setBakeMode(true);
					rp->render(m_radiance, camera, entityMgr, viewport);
					CBaseRP::setBakeMode(false);
				}
			}

			driver->setRenderTarget(NULL, false, false);

			// compute sh from radiance
			computeSH(count, numFace);
		}

		void CMTBaker::computeSH(int count, int numFace)
		{
			// render target size
			u32 rtSize = CLightmapper::getHemisphereBakeSize();

			// Cubemap to SH
			u8* imageData = (u8*)m_radiance->lock(video::ETLM_READ_ONLY);
			u32 bpp = 4;
			u32 rowSize = rtSize * NUM_FACES * bpp;
			float c = 1.0f / 255.0f;

			core::vector3df color;
			core::vector3df dirTS;

			bool isBGR = false;

			// Note: DX11 & OpenGLES is RGB after read texture data
			if (getVideoDriver()->getDriverType() == video::EDT_OPENGL)
				isBGR = true;

#pragma omp parallel for
			for (int tid = 0; tid < count; tid++)
				m_sh[tid].zero();

			int height = rtSize * count;
			int width = rtSize * numFace;

			u8* faceData = NULL;
			u8* data = NULL;
			int x, face;
			float u, v, temp, weight;

			// Compute SH by radiance (use OpenMP)
#pragma omp parallel for private(dirTS, color, data, x, face, u, v, temp, weight)
			for (int tid = 0; tid < count; tid++)
			{
				for (int y = 0; y < rtSize; y++)
				{
					for (int imgx = 0; imgx < width; imgx++)
					{
						// calc face & tid
						face = imgx / rtSize;

						x = imgx - face * rtSize;

						// face data
						faceData = imageData + rtSize * face * bpp;

						// offset to tid and row y
						data = faceData + (rtSize * tid + y) * rowSize;

						// offset to pixel x
						data += x * bpp;

						// Calculate the location in [-1, 1] texture space
						u = ((x / float(rtSize)) * 2.0f - 1.0f);
						v = -((y / float(rtSize)) * 2.0f - 1.0f);

						temp = 1.0f + u * u + v * v;
						weight = 4.0f / (sqrt(temp) * temp);

						if (isBGR == true)
						{
							color.X = data[2] * c * weight; // b
							color.Y = data[1] * c * weight; // g
							color.Z = data[0] * c * weight; // r
						}
						else
						{
							color.X = data[0] * c * weight; // r
							color.Y = data[1] * c * weight; // g
							color.Z = data[2] * c * weight; // b
						}

						dirTS.X = u;
						dirTS.Y = v;
						dirTS.Z = 1.0f;

						m_toTangentSpace[tid * NUM_FACES + face].rotateVect(dirTS);
						dirTS.normalize();

						m_sh[tid].projectAddOntoSH(dirTS, color);
					}
				}
			}

			// finalWeight is weight for 1 pixel on Sphere
			// S = 4 * PI * R^2
			float finalWeight = (4.0f * 3.14159f) / (m_weightSum * numFace);

#pragma omp parallel for
			for (int tid = 0; tid < count; tid++)
				m_sh[tid] *= finalWeight;

			m_radiance->unlock();

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