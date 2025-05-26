/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
#include "CRenderToTextureRP.h"

#include "Material/Shader/CShaderManager.h"

namespace Skylicht
{
	core::matrix4 g_projection[video::MATERIAL_MAX_TEXTURES];

	const float* CRenderToTextureRP::getMatrix(int id)
	{
		if (id >= video::MATERIAL_MAX_TEXTURES)
			id = 0;
		return g_projection[id].pointer();
	}

	CRenderToTextureRP::CRenderToTextureRP(u32 id) :
		m_id(id)
	{
		if (m_id >= video::MATERIAL_MAX_TEXTURES)
			m_id = 0;

		if (getVideoDriver()->getDriverType() == EDT_DIRECT3D11)
		{
			const float biasDX[16] = {
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.5f, 0.0f,
				0.5f, 0.5f, 0.5f, 1.0f
			};
			m_bias.setM(biasDX);
		}
		else
		{
			const float biasGL[16] = {
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.5f, 0.0f,
				0.5f, 0.5f, 0.5f, 1.0f
			};
			m_bias.setM(biasGL);
		}
	}

	CRenderToTextureRP::~CRenderToTextureRP()
	{
	}

	void CRenderToTextureRP::initRender(int w, int h)
	{

	}

	void CRenderToTextureRP::resize(int w, int h)
	{

	}

	void CRenderToTextureRP::render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId, IRenderPipeline* lastRP)
	{
		if (camera == NULL)
			return;

		IVideoDriver* driver = getVideoDriver();

		setTarget(target, cubeFaceId);

		// custom viewport
		if (viewport.getWidth() > 0 && viewport.getHeight() > 0)
			driver->setViewPort(viewport);

		setCamera(camera);
		entityManager->setCamera(camera);
		entityManager->setRenderPipeline(this);

		// update prj matrix
		core::matrix4 mvp = driver->getTransform(video::ETS_PROJECTION) * driver->getTransform(video::ETS_VIEW);
		g_projection[m_id] = m_bias * mvp;

		if (m_updateEntity == true)
		{
			entityManager->update();
			entityManager->cullingAndRender();
		}
		else
		{
			entityManager->cullingAndRender();
		}

		onNext(target, camera, entityManager, viewport, cubeFaceId);
	}
}