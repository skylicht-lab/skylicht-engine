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
#include "CRenderToTexture.h"
#include "GameObject/CGameObject.h"

#include "RenderPipeline/CBaseRP.h"

namespace Skylicht
{
	namespace Lightmapper
	{
		CRenderToTexture::CRenderToTexture()
		{
			IVideoDriver *driver = getVideoDriver();
			core::dimension2du s(RT_SIZE * NUM_FACES, RT_SIZE);
			m_radiance = driver->addRenderTargetTexture(s, "lmrt", video::ECF_A8R8G8B8);
		}

		CRenderToTexture::~CRenderToTexture()
		{
			IVideoDriver *driver = getVideoDriver();
			driver->removeTexture(m_radiance);
			m_radiance = NULL;
		}

		void CRenderToTexture::bake(CCamera *camera,
			IRenderPipeline* rp,
			CEntityManager* entityMgr,
			const core::vector3df& position,
			const core::vector3df& normal,
			const core::vector3df& tangent,
			const core::vector3df& binormal)
		{
			IVideoDriver *driver = getVideoDriver();

			// apply projection
			camera->setAspect(1.0f);
			camera->setFOV(90.0f);

			// clear rtt
			getVideoDriver()->setRenderTarget(m_radiance, true, true);

			for (int face = 0; face < NUM_FACES; face++)
			{
				core::matrix4 cameraWorld;
				core::recti viewport;

				getWorldView(normal, tangent, binormal, position, face, cameraWorld);				

				// camera world
				camera->getGameObject()->getTransform()->setMatrixTransform(cameraWorld);

				// view matrix is world inverse
				cameraWorld.makeInverse();
				camera->setViewMatrix(cameraWorld);

				// apply viewport
				u32 offsetX = face * RT_SIZE;
				u32 offsetY = 0;
				viewport.UpperLeftCorner.set(offsetX, offsetY);
				viewport.LowerRightCorner.set(offsetX + RT_SIZE, offsetY + RT_SIZE);

				// render to target
				getVideoDriver()->clearZBuffer();

				// render
				rp->render(m_radiance, camera, entityMgr, viewport);
			}

			driver->setRenderTarget(NULL, false, false);

			// CBaseRP::saveFBOToFile(m_radiance, "C:\\SVN\\test.png");
		}

		void CRenderToTexture::getWorldView(
			const core::vector3df& normal,
			const core::vector3df& tangent,
			const core::vector3df& binormal,
			const core::vector3df& position,
			int face,
			core::matrix4& out)
		{
			core::vector3df x = tangent;
			core::vector3df y = binormal;
			core::vector3df z = normal;

			if (face == 0)
			{
				// top
				setRow(out, 0, x);
				setRow(out, 1, y);
				setRow(out, 2, z);
			}
			else if (face == 1)
			{
				setRow(out, 0, -z);
				setRow(out, 1, y);
				setRow(out, 2, x);
			}
			else if (face == 2)
			{
				setRow(out, 0, z);
				setRow(out, 1, y);
				setRow(out, 2, -x);
			}
			else if (face == 3)
			{
				setRow(out, 0, x);
				setRow(out, 1, -z);
				setRow(out, 2, y);
			}
			else if (face == 4)
			{
				setRow(out, 0, x);
				setRow(out, 1, z);
				setRow(out, 2, -y);
			}
			else
			{
				// bottom
				setRow(out, 0, x);
				setRow(out, 1, -y);
				setRow(out, 2, -z);
			}

			// translate
			setRow(out, 3, position, 1.0f);
		}

		void CRenderToTexture::setRow(core::matrix4& mat, int row, const core::vector3df& v, float w)
		{
			mat(row, 0) = v.X;
			mat(row, 1) = v.Y;
			mat(row, 2) = v.Z;
			mat(row, 3) = w;
		}
	}
}