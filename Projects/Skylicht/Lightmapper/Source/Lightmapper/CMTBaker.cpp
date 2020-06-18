#include "pch.h"
#include "CMTBaker.h"
#include "CBakeUtils.h"
#include "GameObject/CGameObject.h"
#include "RenderPipeline/CBaseRP.h"
#include "RenderPipeline/CDeferredRP.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		CMTBaker::CMTBaker() :
			m_weightSum(0.0f)
		{
			IVideoDriver *driver = getVideoDriver();
			core::dimension2du s(RT_SIZE * NUM_FACES, RT_SIZE * NUM_MTBAKER);
			m_radiance = driver->addRenderTargetTexture(s, "lmrt", video::ECF_A8R8G8B8);

			for (int y = 0; y < RT_SIZE; y++)
			{
				for (int x = 0; x < RT_SIZE; x++)
				{
					float u = ((x / float(RT_SIZE)) * 2.0f - 1.0f);
					float v = -((y / float(RT_SIZE)) * 2.0f - 1.0f);

					float temp = 1.0f + u * u + v * v;
					float weight = 4.0f / (sqrt(temp) * temp);
					m_weightSum = m_weightSum + weight;
				}
			}
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

#pragma omp parallel for
			for (int tid = 0; tid < count; tid++)
				m_sh[tid].zero();

			int height = RT_SIZE * count;
			int width = RT_SIZE * numFace;

			u8 *faceData = NULL;
			u8 *data = NULL;
			int x, y, face, tid;
			float u, v, temp, weight;

			// Compute SH by radiance (use OpenMP)
#pragma omp parallel for private(dirTS, color, data, x, y, face, tid, u, v, temp, weight)
			for (int imgy = 0; imgy < height; imgy++)
			{
				for (int imgx = 0; imgx < width; imgx++)
				{
					// calc face & tid
					face = imgx / RT_SIZE;
					tid = imgy / RT_SIZE;

					y = imgy - tid * RT_SIZE;
					x = imgx - face * RT_SIZE;

					// face data
					faceData = imageData + RT_SIZE * face * bpp;

					// offset to tid and row y
					data = faceData + (RT_SIZE * tid + y) * rowSize;

					// offset to pixel x 
					data += x * bpp;

					// Calculate the location in [-1, 1] texture space
					u = ((x / float(RT_SIZE)) * 2.0f - 1.0f);
					v = -((y / float(RT_SIZE)) * 2.0f - 1.0f);

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
					toTangentSpace[tid][face].rotateVect(dirTS);
					dirTS.normalize();

					m_sh[tid].projectAddOntoSH(dirTS, color);
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
			if (CDeferredRP::isEnableRenderIndirect() == true && test == true)
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