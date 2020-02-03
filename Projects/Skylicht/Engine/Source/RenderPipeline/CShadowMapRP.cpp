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
#include "Material/Shader/CShaderManager.h"

namespace Skylicht
{
	CShadowMapRP::CShadowMapRP() :
		m_depthTexture(NULL),
		m_csm(NULL),
		m_shadowMapSize(512)
	{
		m_type = ShadowMap;
		m_lightDirection.set(-1.0f, -1.0f, -1.0f);

		m_writeDepthMaterial.MaterialType = CShaderManager::getInstance()->getShaderIDByName("ShadowDepthWrite");
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
		m_csm->init(2, m_shadowMapSize, 100.0f, w, h);

		core::dimension2du size = core::dimension2du((u32)m_shadowMapSize, (u32)m_shadowMapSize);
		m_depthTexture = getVideoDriver()->addRenderTargetTexture(size, "shadow_depth", ECF_A8R8G8B8);
	}
	
	void CShadowMapRP::drawMeshBuffer(CMesh *mesh, int bufferID)
	{
		// set shader material
		CShaderMaterial::setMaterial(mesh->Material[bufferID]);

		IMeshBuffer *mb = mesh->getMeshBuffer(bufferID);
		IVideoDriver *driver = getVideoDriver();

		// override write depth material
		driver->setMaterial(m_writeDepthMaterial);

		// draw mesh buffer
		driver->drawMeshBuffer(mb);
	}

	void CShadowMapRP::render(ITexture *target, CCamera *camera, CEntityManager *entityManager)
	{
		if (camera == NULL)
			return;

		m_csm->update(camera, m_lightDirection);

		IVideoDriver *driver = getVideoDriver();
		driver->setRenderTarget(m_depthTexture, true, true);

		setCamera(camera);

		driver->setTransform(video::ETS_PROJECTION, m_csm->getProjectionMatrices(0));
		driver->setTransform(video::ETS_VIEW, m_csm->getViewMatrices(0));

		entityManager->setCamera(camera);
		entityManager->setRenderPipeline(this);

		if (m_updateEntity == true)
			entityManager->update();

		entityManager->render();

		onNext(target, camera, entityManager);
	}
}