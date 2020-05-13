#include "pch.h"
#include "CMTBaker.h"
#include "CBakeUtils.h"
#include "GameObject/CGameObject.h"
#include "RenderPipeline/CBaseRP.h"

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
			int count)
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
				for (int face = 0; face < NUM_FACES; face++)
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

			// CBaseRP::saveFBOToFile(m_radiance, "D:\\test.png");
		}
	}
}