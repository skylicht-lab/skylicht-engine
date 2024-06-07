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
#include "CForwardRP.h"

#include "Material/Shader/CShaderManager.h"

namespace Skylicht
{
	CForwardRP::CForwardRP(bool linearRGB) :
		m_useLinearRGB(linearRGB),
		m_postProcessor(NULL),
		m_target(NULL)
	{

	}

	CForwardRP::~CForwardRP()
	{
		releaseRTT();
	}

	void CForwardRP::initRTT(int w, int h)
	{
		m_size.set(w, h);

		if (m_useLinearRGB == true || m_postProcessor)
			m_target = getVideoDriver()->addRenderTargetTexture(m_size, "target", ECF_A16B16G16R16F);

		m_emission = getVideoDriver()->addRenderTargetTexture(m_size, "emission", ECF_A16B16G16R16F);
	}

	void CForwardRP::releaseRTT()
	{
		IVideoDriver* driver = getVideoDriver();

		if (m_target != NULL)
			driver->removeTexture(m_target);

		if (m_emission != NULL)
			driver->removeTexture(m_emission);
	}

	void CForwardRP::initRender(int w, int h)
	{
		initRTT(w, h);

		m_finalPass.MaterialType = CShaderManager::getInstance()->getShaderIDByName("TextureLinearRGB");
	}

	void CForwardRP::resize(int w, int h)
	{
		releaseRTT();

		initRTT(w, h);
	}

	void CForwardRP::render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId, IRenderPipeline* lastRP)
	{
		if (camera == NULL)
			return;

		IVideoDriver* driver = getVideoDriver();

		ITexture* currentTarget = NULL;

		if (m_useLinearRGB == true || m_postProcessor)
		{
			driver->setRenderTarget(m_target, true, true, s_clearColor);

			currentTarget = m_target;

			if (viewport.getWidth() > 0 && viewport.getHeight() > 0)
			{
				core::recti vp(0, 0, (int)viewport.getWidth(), (int)viewport.getHeight());
				driver->setViewPort(vp);
			}
		}
		else
		{
			setTarget(target, cubeFaceId);

			currentTarget = target;

			// custom viewport
			if (viewport.getWidth() > 0 && viewport.getHeight() > 0)
				driver->setViewPort(viewport);
		}

		setCamera(camera);
		entityManager->setCamera(camera);
		entityManager->setRenderPipeline(this);

		if (m_updateEntity == true)
		{
			entityManager->update();
			entityManager->cullingAndRender();
		}
		else
		{
			entityManager->cullingAndRender();
		}

		// render emission
		if (m_emission != NULL)
		{
			driver->setRenderTarget(m_emission, true, false);
			entityManager->renderEmission();

			setTarget(currentTarget, cubeFaceId);
		}

		onNext(currentTarget, camera, entityManager, viewport, cubeFaceId);

		if (m_postProcessor != NULL && s_bakeMode == false)
		{
			m_postProcessor->postProcessing(target, m_target, m_emission, NULL, NULL, viewport, cubeFaceId);
		}
		else if (m_useLinearRGB && m_target != NULL)
		{
			setTarget(target, cubeFaceId);

			float renderW = (float)m_size.Width;
			float renderH = (float)m_size.Height;

			if (viewport.getWidth() > 0 && viewport.getHeight() > 0)
			{
				driver->setViewPort(viewport);
				renderW = (float)viewport.getWidth();
				renderH = (float)viewport.getHeight();
			}

			m_finalPass.setTexture(0, m_target);

			beginRender2D(renderW, renderH);
			renderBufferToTarget(0.0f, 0.0f, renderW, renderH, m_finalPass);
		}
	}
}