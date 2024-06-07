/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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
#include "CShadowMapBakeRP.h"

#include "Lighting/CDirectionalLight.h"
#include "Lighting/CPointLight.h"

#include "Material/Shader/ShaderCallback/CShaderLighting.h"
#include "Material/Shader/ShaderCallback/CShaderShadow.h"

#include "EventManager/CEventManager.h"

namespace Skylicht
{
	CShadowMapBakeRP::CShadowMapBakeRP() :
		m_sm(NULL)
	{
		CEventManager::getInstance()->registerEvent("ShadowBakeRP", this);
	}

	CShadowMapBakeRP::~CShadowMapBakeRP()
	{
		if (m_sm != NULL)
			delete m_sm;

		CEventManager::getInstance()->unRegisterEvent(this);
	}

	void CShadowMapBakeRP::initRender(int w, int h)
	{
		m_sm = new CBoundShadowMaps();
		m_sm->init(m_shadowMapSize, 300.0f, w, h);

		// default 10m
		float bbSize = 10.0f;
		m_bound.MinEdge = core::vector3df(-1.0f, -1.0f, -1.0f) * bbSize;
		m_bound.MaxEdge = core::vector3df(1.0f, 1.0f, 1.0f) * bbSize;

		core::dimension2du size = core::dimension2du((u32)m_shadowMapSize, (u32)m_shadowMapSize);
		m_depthTexture = getVideoDriver()->addRenderTargetTexture(size, "shadow_depth", ECF_R32F);
	}

	const core::aabbox3df& CShadowMapBakeRP::getFrustumBox()
	{
		return m_sm->getFrustumBox();
	}

	float* CShadowMapBakeRP::getShadowDistance()
	{
		return NULL;
	}

	float* CShadowMapBakeRP::getShadowMatrices()
	{
		return m_sm->getShadowMatrices();
	}

	void CShadowMapBakeRP::render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId, IRenderPipeline* lastRP)
	{
		if (camera == NULL)
			return;

		// use direction light
		bool castShadow = true;

		CDirectionalLight* light = CShaderLighting::getDirectionalLight();
		if (light != NULL)
		{
			m_lightDirection = light->getDirection();
			castShadow = light->isCastShadow();
		}

		m_sm->update(camera, m_lightDirection, m_bound);

		setCamera(camera);

		entityManager->setCamera(camera);
		entityManager->setRenderPipeline(this);

		if (m_updateEntity == true)
			entityManager->update();

		CShaderShadow::setShadowMapRP(this);

		// render directional light shadow
		m_renderShadowState = CShadowMapRP::DirectionLight;

		IVideoDriver* driver = getVideoDriver();

		// Render depth
		{
			// note: clear while 0xFFFFFFFF for max depth value
			driver->setRenderTarget(m_depthTexture, true, true, SColor(255, 255, 255, 255));
			driver->setTransform(video::ETS_PROJECTION, m_sm->getProjectionMatrices());
			driver->setTransform(video::ETS_VIEW, m_sm->getViewMatrices());

			if (castShadow)
			{
				entityManager->cullingAndRender();
			}
		}

		// todo
		// Save output to file to test
		if (m_saveDebug == true)
		{
			core::dimension2du size = core::dimension2du((u32)m_shadowMapSize, (u32)m_shadowMapSize);
			ITexture* rtt = getVideoDriver()->addRenderTargetTexture(size, "rt", video::ECF_A8R8G8B8);

			{
				driver->setRenderTarget(rtt, true, true);
				driver->setTransform(video::ETS_PROJECTION, m_sm->getProjectionMatrices());
				driver->setTransform(video::ETS_VIEW, m_sm->getViewMatrices());

				entityManager->cullingAndRender();

				driver->setRenderTarget(NULL, false, false);

				char filename[32];
				sprintf(filename, "shadow.png");
				CBaseRP::saveFBOToFile(rtt, filename);
				os::Printer::log(filename);
			}

			getVideoDriver()->removeTexture(rtt);
			m_saveDebug = false;
		}
		
		// todo on next render pipeline
		onNext(target, camera, entityManager, viewport, cubeFaceId);
	}
}