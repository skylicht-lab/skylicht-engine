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
#include "CPostProcessorRP.h"
#include "Material/Shader/CShaderManager.h"

namespace Skylicht
{
	CPostProcessorRP::CPostProcessorRP() :
		m_currentLum(NULL),
		m_lastLum(NULL)
	{

	}

	CPostProcessorRP::~CPostProcessorRP()
	{
		IVideoDriver *driver = getVideoDriver();
		driver->removeTexture(m_luminance[0]);
		driver->removeTexture(m_luminance[1]);
	}

	void CPostProcessorRP::initRender(int w, int h)
	{
		IVideoDriver *driver = getVideoDriver();
		CShaderManager *shaderMgr = CShaderManager::getInstance();

		// init size of framebuffer
		m_size = core::dimension2du((u32)w, (u32)h);
		m_lumSize = core::dimension2du(1024, 1024);

		m_luminance[0] = driver->addRenderTargetTexture(m_lumSize, "lum_0", ECF_R16F);
		m_luminance[1] = driver->addRenderTargetTexture(m_lumSize, "lum_1", ECF_R16F);

		// init final pass shader
		m_finalPass.MaterialType = shaderMgr->getShaderIDByName("PostEffect");

		// init lum pass shader
		m_lumPass.MaterialType = shaderMgr->getShaderIDByName("Luminance");
	}

	void CPostProcessorRP::render(ITexture *target, CCamera *camera, CEntityManager *entityManager, const core::recti& viewport)
	{
		if (camera == NULL)
			return;

		onNext(target, camera, entityManager, viewport);
	}

	void CPostProcessorRP::luminanceMapGeneration(ITexture *color)
	{
		if (m_lastLum == m_luminance[0])
		{
			m_currentLum = m_luminance[1];
			m_lastLum = m_luminance[0];
		}
		else
		{
			m_currentLum = m_luminance[0];
			m_lastLum = m_luminance[1];
		}

		IVideoDriver * driver = getVideoDriver();
		driver->setRenderTarget(m_currentLum, true, true);

		m_lumPass.setTexture(0, color);

		float w = (float)m_lumSize.Width;
		float h = (float)m_lumSize.Height;

		beginRender2D(w, h);
		renderBufferToTarget(0.0f, 0.0f, w, h, m_lumPass);
	}

	void CPostProcessorRP::postProcessing(ITexture *finalTarget, ITexture *color, ITexture *normal, ITexture *position, const core::recti& viewport)
	{
		IVideoDriver *driver = getVideoDriver();

		luminanceMapGeneration(color);

		m_currentLum->regenerateMipMapLevels();

		driver->setRenderTarget(finalTarget, false, false);

		float renderW = (float)m_size.Width;
		float renderH = (float)m_size.Height;

		if (viewport.getWidth() > 0 && viewport.getHeight() > 0)
		{
			driver->setViewPort(viewport);
			renderW = (float)viewport.getWidth();
			renderH = (float)viewport.getHeight();
		}

		m_finalPass.setTexture(0, color);		
		m_finalPass.setTexture(1, m_currentLum);
		m_finalPass.setTexture(2, m_lastLum);

		beginRender2D(renderW, renderH);
		renderBufferToTarget(0.0f, 0.0f, renderW, renderH, m_finalPass);

		m_lastLum = m_currentLum;
	}
}