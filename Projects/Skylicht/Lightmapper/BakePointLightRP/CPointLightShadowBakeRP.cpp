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
#include "CPointLightShadowBakeRP.h"

#include "Lighting/CDirectionalLight.h"
#include "Lighting/CPointLight.h"

#include "Material/Shader/ShaderCallback/CShaderLighting.h"
#include "Material/Shader/ShaderCallback/CShaderShadow.h"

#include "EventManager/CEventManager.h"

#include "Shadow/CShadowRTTManager.h"

namespace Skylicht
{
	CPointLightShadowBakeRP::CPointLightShadowBakeRP()
	{
		// CEventManager::getInstance()->registerProcessorEvent("ShadowBakeRP", this);
	}

	CPointLightShadowBakeRP::~CPointLightShadowBakeRP()
	{
		// CEventManager::getInstance()->unRegisterProcessorEvent(this);
	}

	void CPointLightShadowBakeRP::initRender(int w, int h)
	{

	}

	void CPointLightShadowBakeRP::render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId, IRenderPipeline* lastRP)
	{
		if (camera == NULL || m_currentLight == NULL)
			return;

		CDirectionalLight* direction = dynamic_cast<CDirectionalLight*>(m_currentLight);
		if (direction)
			return;

		if (m_currentLight->getRenderLightType() != CLight::Baked &&
			m_currentLight->getRenderLightType() != CLight::Mixed)
			return;

		if (!m_currentLight->isAffectingDefaultObjects())
			return;

		setCamera(camera);

		entityManager->setCamera(camera);
		entityManager->setRenderPipeline(this);
		CShaderShadow::setShadowMapRP(this);

		// rtt
		CShadowRTTManager* shadowRTT = CShadowRTTManager::getInstance();
		shadowRTT->clearDynamicTextures();

		// set state point light
		m_renderShadowState = ERenderShadowState::PointLight;

		// note: alway enable shadow on baked mode
		CPointLight* pointLight = dynamic_cast<CPointLight*>(m_currentLight);
		if (pointLight != NULL)
		{
			// for uLightPosition in shader write depth distance
			CShaderLighting::setPointLight(pointLight, 0);

			pointLight->beginRenderShadowDepth();

			core::vector3df lightPosition = pointLight->getGameObject()->getPosition();

			m_depthTexture = shadowRTT->createGetDepthCube(pointLight);
			if (m_depthTexture != NULL)
				renderCubeEnvironment(camera, entityManager, lightPosition, m_depthTexture, NULL, 0, false);

			pointLight->endRenderShadowDepth();

			// debug
			/*
			{
				m_saveDebugPL = true;
				ITexture* debugTexture[6];
				for (int i = 0; i < 6; i++)
					debugTexture[i] = getVideoDriver()->addRenderTargetTexture(core::dimension2du(512, 512), "bake_reflection", video::ECF_A8R8G8B8);

				renderEnvironment(camera, entityManager, lightPosition, debugTexture, NULL, 0, false);
				setTarget(target, cubeFaceId);

				static int lightId = 0;
				lightId++;
				for (int face = 0; face < 6; face++)
				{
					char filename[32];
					sprintf(filename, "pl_bake_shadow_%d_f%d.png", lightId, face);
					CBaseRP::saveFBOToFile(debugTexture[face], filename);
					os::Printer::log(filename);

					getVideoDriver()->removeTexture(debugTexture[face]);
				}
				m_saveDebugPL = false;
			}
			*/
		}

		setTarget(target, cubeFaceId);

		// todo on next render pipeline
		onNext(target, camera, entityManager, viewport, cubeFaceId);
	}

	const core::aabbox3df& CPointLightShadowBakeRP::getFrustumBox()
	{
		if (m_currentLight)
			m_cullingBox = m_currentLight->getBBBox();
		return m_cullingBox;
	}
}