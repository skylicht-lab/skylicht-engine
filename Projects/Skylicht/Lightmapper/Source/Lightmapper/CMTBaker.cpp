#include "pch.h"
#include "CMTBaker.h"
#include "CBakeUtils.h"
#include "GameObject/CGameObject.h"
#include "RenderPipeline/CBaseRP.h"
#include "RenderPipeline/CDeferredRP.h"

#if defined(USE_OPENMP)
#include <omp.h>
#endif

namespace Skylicht
{
	namespace Lightmapper
	{
		CMTBaker::CMTBaker()
		{
			IVideoDriver *driver = getVideoDriver();
			core::dimension2du s(RT_SIZE * NUM_FACES, RT_SIZE * NUM_MTBAKER);
			m_radiance = driver->addRenderTargetTexture(s, "lmrt", video::ECF_A8R8G8B8);
		}

		CMTBaker::~CMTBaker()
		{
			IVideoDriver *driver = getVideoDriver();
			driver->removeTexture(m_radiance);
			m_radiance = NULL;
		}

		void CMTBaker::bake(CCamera *camera,
			IRenderPipeline* rp,
			CEntityManager* entityMgr,
			const core::vector3df* position,
			const core::vector3df* normal,
			const core::vector3df* tangent,
			const core::vector3df* binormal,
			int count,
			int numFace)
		{
			IVideoDriver *driver = getVideoDriver();

			// apply projection
			camera->setAspect(1.0f);
			camera->setFOV(90.0f);

			// clear rtt
			getVideoDriver()->setRenderTarget(m_radiance, true, true);

			core::matrix4 toTangentSpace[NUM_MTBAKER][NUM_FACES];

			for (int tid = 0; tid < count; tid++)
			{
				for (int face = 0; face < numFace; face++)
				{
					core::matrix4 cameraWorld;
					core::matrix4 viewToWorld;

					core::recti viewport;

					getWorldView(normal[tid], tangent[tid], binormal[tid], position[tid], face, cameraWorld);

					// to tangent space
					toTangentSpace[tid][face] = cameraWorld;
					setRow(toTangentSpace[tid][face], 3, core::vector3df(0.0f, 0.0f, 0.0f), 1.0f);

					// camera world
					camera->getGameObject()->getTransform()->setMatrixTransform(cameraWorld);

					// view matrix is world inverse
					cameraWorld.makeInverse();
					camera->setViewMatrix(cameraWorld, position[tid]);

					// apply viewport
					u32 offsetX = face * RT_SIZE;
					u32 offsetY = tid * RT_SIZE;
					viewport.UpperLeftCorner.set(offsetX, offsetY);
					viewport.LowerRightCorner.set(offsetX + RT_SIZE, offsetY + RT_SIZE);

					// render
					rp->render(m_radiance, camera, entityMgr, viewport);
				}
			}

			driver->setRenderTarget(NULL, false, false);

			// Cubemap to SH
			u8 *imageData = (u8*)m_radiance->lock(video::ETLM_READ_ONLY);
			u32 bpp = 4;
			u32 rowSize = RT_SIZE * NUM_FACES * bpp;
			float c = 1.0f / 255.0f;

			core::vector3df color;
			core::vector3df dirTS;

			bool isBGR = false;

			// Note: DX11 & OpenGLES is RGB after read texture data
			if (getVideoDriver()->getDriverType() == video::EDT_OPENGL)
				isBGR = true;

			for (int tid = 0; tid < count; tid++)
			{
				m_sh[tid].zero();

				// Compute the final weight for integration
				float weightSum = 0.0f;

				// Compute SH by radiance (use OpenMP)
				for (int face = 0; face < numFace; face++)
				{
					// scan pixels
					// offset to face data
					u8 *faceData = imageData + RT_SIZE * face * bpp;
					u8 *data = NULL;

#pragma omp parallel for private(dirTS) private(color) private(data)
					for (int y = 0; y < RT_SIZE; y++)
					{
						for (int x = 0; x < RT_SIZE; x++)
						{
							// offset to tid and row y
							data = faceData + (RT_SIZE * tid + y) * rowSize;

							// offset to pixel x 
							data += x * bpp;

							// Calculate the location in [-1, 1] texture space
							float u = ((x / float(RT_SIZE)) * 2.0f - 1.0f);
							float v = -((y / float(RT_SIZE)) * 2.0f - 1.0f);

							float temp = 1.0f + u * u + v * v;
							float weight = 4.0f / (sqrt(temp) * temp);
							weightSum = weightSum + weight;

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
							toTangentSpace[tid][face].rotateVect(dirTS);
							dirTS.normalize();

							m_sh[tid].projectAddOntoSH(dirTS, color);
						}
					}
				}

				// finalWeight is weight for 1 pixel on Sphere
				// S = 4 * PI * R^2
				float finalWeight = (4.0f * 3.14159f) / weightSum;
				m_sh[tid] *= finalWeight;
			}

			m_radiance->unlock();

			// test radiance
			/*
			static int t = 0;
			static bool test = false;
			if (CDeferredRP::isEnableRenderIndirect() == true && test == false)
			{
				char filename[512];
				sprintf(filename, "C:\\SVN\\test_%d.png", t);
				CBaseRP::saveFBOToFile(m_radiance, filename);
				test = true;
			}
			t++;
			*/
		}
	}
}