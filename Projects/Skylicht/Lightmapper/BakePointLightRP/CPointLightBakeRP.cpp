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
#include "Material/Shader/ShaderCallback/CShaderLighting.h"
#include "Material/CMaterial.h"

#include "CPointLightShadowBakeRP.h"

namespace Skylicht
{
	CPointLightBakeRP::CPointLightBakeRP() :
		m_renderMesh(NULL),
		m_submesh(NULL),
		m_renderTarget(NULL),
		m_numTarget(0),
		m_currentTarget(0),
		m_bakePointLightMaterialID(0),
		m_bakeSpotLightMaterialID(0),
		m_bakePointLightUV0MaterialID(0),
		m_bakeSpotLightUV0MaterialID(0)
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
		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeSpotLight.xml");

		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakePointLightUV0.xml");
		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeSpotLightUV0.xml");

		m_bakePointLightMaterialID = shaderMgr->getShaderIDByName("BakePointLight");
		m_bakeSpotLightMaterialID = shaderMgr->getShaderIDByName("BakeSpotLight");

		m_bakePointLightUV0MaterialID = shaderMgr->getShaderIDByName("BakePointLightUV0");
		m_bakeSpotLightUV0MaterialID = shaderMgr->getShaderIDByName("BakeSpotLightUV0");
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
			if (m_renderTarget[i] == NULL || m_submesh[i] == NULL)
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

		// shadow
		CPointLightShadowBakeRP* shadowRP = dynamic_cast<CPointLightShadowBakeRP*>(CShaderShadow::getShadowMapRP());
		if (shadowRP == NULL)
			return;

		// check uv2
		video::E_VERTEX_TYPE vtxType = m_submesh[m_currentTarget]->getVertexType();
		bool haveTangent = vtxType == video::EVT_2TCOORDS || vtxType == video::EVT_2TCOORDS_TANGENTS;
		if (!shadowRP->isBakeInUV0() && !haveTangent)
			return;

		CLight* currentLight = shadowRP->getCurrentLight();
		CPointLight* pl = dynamic_cast<CPointLight*>(currentLight);
		if (pl == NULL)
			return;

		CSpotLight* sl = dynamic_cast<CSpotLight*>(currentLight);
		if (sl)
			CShaderLighting::setSpotLight(sl);
		else
			CShaderLighting::setPointLight(pl);

		// render mesh with light bake shader
		video::SMaterial irrMaterial;

		if (shadowRP->isBakeInUV0())
			irrMaterial.MaterialType = sl != NULL ? m_bakeSpotLightUV0MaterialID : m_bakePointLightUV0MaterialID;
		else
			irrMaterial.MaterialType = sl != NULL ? m_bakeSpotLightMaterialID : m_bakePointLightMaterialID;

		irrMaterial.ZBuffer = video::ECFN_DISABLED;
		irrMaterial.ZWriteEnable = false;
		irrMaterial.BackfaceCulling = false;
		irrMaterial.FrontfaceCulling = false;

		if (irrMaterial.MaterialType <= 0)
			return;

		ITexture* depthTexture = shadowRP->getDepthTexture();
		depthTexture->regenerateMipMapLevels();

		irrMaterial.TextureLayer[0].Texture = depthTexture;
		irrMaterial.TextureLayer[0].BilinearFilter = false;
		irrMaterial.TextureLayer[0].TrilinearFilter = false;
		irrMaterial.TextureLayer[0].AnisotropicFilter = 0;

		// set irrlicht material
		driver->setMaterial(irrMaterial);

		// draw mesh buffer
		driver->drawMeshBuffer(m_submesh[m_currentTarget]);
	}

	void CPointLightBakeRP::drawInstancingMeshBuffer(CMesh* mesh, int bufferID, int materialRenderID, CEntityManager* entityMgr, int entityID, bool skinnedMesh)
	{
		// no render instancing mesh
	}

	bool CPointLightBakeRP::canRenderMaterial(CMaterial* material)
	{
		if (material &&
			material->getShader() &&
			material->getShader()->isOpaque() == true)
			return true;

		return false;
	}

	bool CPointLightBakeRP::canRenderShader(CShader* shader)
	{
		if (shader && shader->isOpaque() == true)
			return true;

		return false;
	}
}