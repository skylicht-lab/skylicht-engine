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
#include "CPointLightBakeRP.h"

#include "Lighting/CDirectionalLight.h"
#include "Lighting/CPointLight.h"

#include "EventManager/CEventManager.h"

#include "RenderMesh/CMesh.h"
#include "Material/Shader/CShaderManager.h"
#include "Material/Shader/ShaderCallback/CShaderShadow.h"
#include "Material/CMaterial.h"

#include "CPointLightShadowBakeRP.h"

namespace Skylicht
{
	CPointLightBakeRP::CPointLightBakeRP() :
		m_renderMesh(NULL),
		m_renderSubmesh(NULL),
		m_renderTarget(NULL),
		m_numTarget(0),
		m_currentTarget(0),
		m_bakePointLightMaterialID(0)
	{
		m_type = Deferred;
	}

	CPointLightBakeRP::~CPointLightBakeRP()
	{

	}

	void CPointLightBakeRP::initRender(int w, int h)
	{
		CShaderManager* shaderMgr = CShaderManager::getInstance();
		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakePointLight.xml");

		m_bakePointLightMaterialID = shaderMgr->getShaderIDByName("BakePointLight");
	}

	void CPointLightBakeRP::resize(int w, int h)
	{

	}

	void CPointLightBakeRP::render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId, IRenderPipeline* lastRP)
	{
		if (camera == NULL)
			return;

		IVideoDriver* driver = getVideoDriver();

		// custom viewport
		bool useCustomViewport = false;
		core::recti customViewport;
		if (viewport.getWidth() > 0 && viewport.getHeight() > 0)
		{
			customViewport.LowerRightCorner.set(
				viewport.getWidth(),
				viewport.getHeight()
			);

			useCustomViewport = true;
		}

		// setup camera
		setCamera(camera);
		entityManager->setCamera(camera);
		entityManager->setRenderPipeline(this);

		for (int i = 0; i < m_numTarget; i++)
		{
			if (m_renderTarget[i] == NULL || m_renderSubmesh[i] == NULL)
				continue;

			driver->setRenderTarget(m_renderTarget[i], false, false);

			m_currentTarget = i;

			if (useCustomViewport)
				driver->setViewPort(viewport);

			// render scene (see function drawMeshBuffer)
			entityManager->cullingAndRender();
		}

		// custom viewport
		if (useCustomViewport)
			driver->setViewPort(customViewport);

		// next
		onNext(target, camera, entityManager, viewport, cubeFaceId);
	}

	void CPointLightBakeRP::drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh)
	{
		// just render the render mesh
		IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
		if (mb != m_renderMesh)
			return;

		IVideoDriver* driver = getVideoDriver();

		// render mesh with light bake shader
		video::SMaterial irrMaterial;
		irrMaterial.MaterialType = m_bakePointLightMaterialID;
		irrMaterial.ZBuffer = video::ECFN_DISABLED;
		irrMaterial.ZWriteEnable = false;
		irrMaterial.BackfaceCulling = false;
		irrMaterial.FrontfaceCulling = false;

		// shadow
		CShadowMapRP* shadowRP = CShaderShadow::getShadowMapRP();
		if (shadowRP != NULL)
		{
			ITexture* depthTexture = shadowRP->getDepthTexture();
			depthTexture->regenerateMipMapLevels();

			irrMaterial.TextureLayer[0].Texture = depthTexture;
			irrMaterial.TextureLayer[0].BilinearFilter = false;
			irrMaterial.TextureLayer[0].TrilinearFilter = false;
			irrMaterial.TextureLayer[0].AnisotropicFilter = 0;
		}

		// set irrlicht material
		driver->setMaterial(irrMaterial);

		// draw mesh buffer
		driver->drawMeshBuffer(m_renderSubmesh[m_currentTarget]);
	}

	void CPointLightBakeRP::drawInstancingMeshBuffer(CMesh* mesh, int bufferID, int materialRenderID, CEntityManager* entityMgr, int entityID, bool skinnedMesh)
	{
		// no render instancing mesh
	}

	bool CPointLightBakeRP::canRenderMaterial(CMaterial* material)
	{
		if (material->isDeferred() == true)
			return true;

		return false;
	}

	bool CPointLightBakeRP::canRenderShader(CShader* shader)
	{
		if (shader->isDeferred() == true)
			return true;

		return false;
	}
}