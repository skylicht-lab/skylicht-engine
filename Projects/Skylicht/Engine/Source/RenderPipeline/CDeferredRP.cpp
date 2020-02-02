/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "CDeferredRP.h"
#include "Material/Shader/CShaderManager.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	CDeferredRP::CDeferredRP() :
		m_albedo(NULL),
		m_position(NULL),
		m_normal(NULL),
		m_data(NULL)
	{
		m_type = IRenderPipeline::Deferred;
	}

	CDeferredRP::~CDeferredRP()
	{
		IVideoDriver *driver = getVideoDriver();

		if (m_albedo != NULL)
			driver->removeTexture(m_albedo);

		if (m_position != NULL)
			driver->removeTexture(m_position);

		if (m_normal != NULL)
			driver->removeTexture(m_normal);

		if (m_data != NULL)
			driver->removeTexture(m_data);

		m_multiRenderTarget.clear();

	}

	void CDeferredRP::initRender(int w, int h)
	{
		IVideoDriver *driver = getVideoDriver();

		// init render target
		m_size = core::dimension2du((u32)w, (u32)h);
		m_albedo = driver->addRenderTargetTexture(m_size, "albedo", ECF_A8R8G8B8);
		m_position = driver->addRenderTargetTexture(m_size, "position", ECF_A32B32G32R32F);
		m_normal = driver->addRenderTargetTexture(m_size, "normal", ECF_A32B32G32R32F);
		m_data = driver->addRenderTargetTexture(m_size, "data", ECF_A8R8G8B8);

		// setup multi render target
		m_multiRenderTarget.push_back(m_albedo);
		m_multiRenderTarget.push_back(m_position);
		m_multiRenderTarget.push_back(m_normal);
		m_multiRenderTarget.push_back(m_data);

		// setup material
		m_material.MaterialType = CShaderManager::getInstance()->getShaderIDByName("SGLighting");

		m_material.setTexture(0, m_albedo);
		m_material.setTexture(1, m_position);
		m_material.setTexture(2, m_normal);
		m_material.setTexture(3, m_data);

		// turn off mipmap on float texture	
		m_material.TextureLayer[1].BilinearFilter = false;
		m_material.TextureLayer[1].TrilinearFilter = false;
		m_material.TextureLayer[1].AnisotropicFilter = 0;

		m_material.TextureLayer[2].BilinearFilter = false;
		m_material.TextureLayer[2].TrilinearFilter = false;
		m_material.TextureLayer[2].AnisotropicFilter = 0;

		// disable Z
		m_material.ZBuffer = video::ECFN_DISABLED;
		m_material.ZWriteEnable = false;
	}

	bool CDeferredRP::canRenderMaterial(CMaterial *material)
	{
		if (material->isDeferred() == true)
			return true;

		return false;
	}

	void CDeferredRP::render(ITexture *target, CCamera *camera, CEntityManager *entityManager)
	{
		if (camera == NULL)
			return;

		// set multi rtt
		IVideoDriver *driver = getVideoDriver();
		driver->setRenderTarget(m_multiRenderTarget);

		// draw entity to buffer
		setCamera(camera);
		entityManager->setCamera(camera);
		entityManager->setRenderPipeline(this);
		entityManager->update();
		entityManager->render();

		// save camera transform
		m_projectionMatrix = driver->getTransform(video::ETS_PROJECTION);
		m_viewMatrix = driver->getTransform(video::ETS_VIEW);

		// render to screen
		float renderW = (float)m_size.Width;
		float renderH = (float)m_size.Height;

		driver->setRenderTarget(target, true, false);

		beginRender2D(renderW, renderH);
		renderBufferToTarget(0.0f, 0.0f, renderW, renderH, m_material);

		// fix DX11: [AndUnorderedAccessViews]: Forcing PS shader resource
		unbindRTT();
	}
}