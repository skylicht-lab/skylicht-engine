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
#include "CShadowMapRP.h"
#include "RenderMesh/CMesh.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"
#include "Material/Shader/ShaderCallback/CShaderShadow.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"
#include "Material/Shader/CShaderManager.h"
#include "Lighting/CLightCullingSystem.h"
#include "Lighting/CPointLight.h"

namespace Skylicht
{
	CShadowMapRP::CShadowMapRP() :
		m_depthTexture(NULL),
		m_csm(NULL),
		m_shadowMapSize(2048),
		m_numCascade(3)
	{
		m_type = ShadowMap;
		m_lightDirection.set(-1.0f, -1.0f, -1.0f);

		// write depth material
		CShaderManager *shaderMgr = CShaderManager::getInstance();
		m_depthWriteShader = shaderMgr->getShaderIDByName("ShadowDepthWrite");
		m_cubeDepthWriteShader = shaderMgr->getShaderIDByName("ShadowCubeDepthWrite");
	}

	CShadowMapRP::~CShadowMapRP()
	{
		if (m_depthTexture != NULL)
			getVideoDriver()->removeTexture(m_depthTexture);

		if (m_csm != NULL)
			delete m_csm;
	}

	void CShadowMapRP::initRender(int w, int h)
	{
		m_csm = new CCascadedShadowMaps();
		m_csm->init(m_numCascade, m_shadowMapSize, 200.0f, w, h);

		core::dimension2du size = core::dimension2du((u32)m_shadowMapSize, (u32)m_shadowMapSize);
		m_depthTexture = getVideoDriver()->addRenderTargetTextureArray(size, m_numCascade, "shadow_depth", ECF_R32F);
	}

	bool CShadowMapRP::canRenderMaterial(CMaterial *m)
	{
		// render all object
		return true;
	}

	void CShadowMapRP::drawMeshBuffer(CMesh *mesh, int bufferID)
	{
		if (mesh->Material.size() > (u32)bufferID)
		{
			// set shader material
			CShaderMaterial::setMaterial(mesh->Material[bufferID]);
		}

		IMeshBuffer *mb = mesh->getMeshBuffer(bufferID);
		IVideoDriver *driver = getVideoDriver();

		// override write depth material
		driver->setMaterial(m_writeDepthMaterial);

		// draw mesh buffer
		driver->drawMeshBuffer(mb);
	}

	void CShadowMapRP::render(ITexture *target, CCamera *camera, CEntityManager *entityManager, const core::recti& viewport)
	{
		if (camera == NULL)
			return;

		m_csm->update(camera, m_lightDirection);

		setCamera(camera);

		entityManager->setCamera(camera);
		entityManager->setRenderPipeline(this);

		if (m_updateEntity == true)
			entityManager->update();

		CShaderShadow::setShadowMapRP(this);

		// render directional light shadow
		m_writeDepthMaterial.MaterialType = m_depthWriteShader;

		IVideoDriver *driver = getVideoDriver();
		for (int i = 0; i < m_numCascade; i++)
		{
			driver->setRenderTargetArray(m_depthTexture, i, true, true);
			driver->setTransform(video::ETS_PROJECTION, m_csm->getProjectionMatrices(i));
			driver->setTransform(video::ETS_VIEW, m_csm->getViewMatrices(i));

			// todo
			// We inorge last cascade shadow
			if (i < m_numCascade - 1)
				entityManager->render();
		}

		// render point light shadow
		m_writeDepthMaterial.MaterialType = m_cubeDepthWriteShader;

		std::vector<ITexture*> listDepthTexture;

		CLightCullingSystem *lightCullingSystem = entityManager->getSystem<CLightCullingSystem>();
		if (lightCullingSystem != NULL)
		{
			core::array<CLightCullingData*>& listLight = lightCullingSystem->getLightVisible();
			for (u32 i = 0, n = listLight.size(); i < n; i++)
			{
				CLight *light = listLight[i]->Light;
				CPointLight *pointLight = dynamic_cast<CPointLight*>(light);

				if (pointLight != NULL &&
					pointLight->isCastShadow() == true &&
					pointLight->needRenderShadowDepth() == true)
				{
					CShaderLighting::setPointLight(pointLight);
					pointLight->beginRenderShadowDepth();

					core::vector3df lightPosition = pointLight->getGameObject()->getPosition();
					ITexture *depth = pointLight->createGetDepthTexture();
					if (depth != NULL)
					{
						renderCubeEnvironment(camera, entityManager, lightPosition, depth, NULL, 0);
						listDepthTexture.push_back(depth);
					}

					pointLight->endRenderShadowDepth();
				}
			}
		}

		if (listDepthTexture.size() > 0)
		{
			driver->setRenderTarget(target, false, false);

			// Generate all depth texture
			for (ITexture *d : listDepthTexture)
				d->regenerateMipMapLevels();
		}

		onNext(target, camera, entityManager, viewport);
	}
}