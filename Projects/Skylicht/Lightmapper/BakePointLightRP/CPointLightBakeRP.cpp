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

#include "Shadow/CShadowRTTManager.h"

#include "CPointLightShadowBakeRP.h"

namespace Skylicht
{
	CPointLightBakeRP::CPointLightBakeRP()
	{
		m_type = Deferred;

		for (int i = 0; i < 4; i++)
		{
			m_light[i] = 0;
			m_lightUV0[i] = 0;
			m_lightN[i] = 0;
			m_lightNUV0[i] = 0;
		}
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

		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakePointLightNormal.xml");
		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeSpotLightNormal.xml");
		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakePointLightUV0Normal.xml");
		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeSpotLightUV0Normal.xml");

		m_light[CLight::PointLight] = shaderMgr->getShaderIDByName("BakePointLight");
		m_light[CLight::SpotLight] = shaderMgr->getShaderIDByName("BakeSpotLight");

		m_lightUV0[CLight::PointLight] = shaderMgr->getShaderIDByName("BakePointLightUV0");
		m_lightUV0[CLight::SpotLight] = shaderMgr->getShaderIDByName("BakeSpotLightUV0");

		m_lightN[CLight::PointLight] = shaderMgr->getShaderIDByName("BakePointLightNormal");
		m_lightN[CLight::SpotLight] = shaderMgr->getShaderIDByName("BakeSpotLightNormal");

		m_lightNUV0[CLight::PointLight] = shaderMgr->getShaderIDByName("BakePointLightUV0Normal");
		m_lightNUV0[CLight::SpotLight] = shaderMgr->getShaderIDByName("BakeSpotLightUV0Normal");
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

			driver->setRenderTarget(m_renderTarget[i], false, true);

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
		bool haveLM = vtxType == video::EVT_2TCOORDS || vtxType == video::EVT_2TCOORDS_TANGENTS;
		if (!shadowRP->isBakeInUV0() && !haveLM)
			return;

		// check detail normal
		bool haveDetailNormal = false;
		bool haveTangent = vtxType == EVT_TANGENTS || vtxType == video::EVT_2TCOORDS_TANGENTS;
		if (shadowRP->isBakeDetailNormal() && haveTangent && m_normalMap)
			haveDetailNormal = true;

		CLight* currentLight = shadowRP->getCurrentLight();
		int lightTypeId = currentLight->getLightTypeId();

		if (lightTypeId != CLight::PointLight &&
			lightTypeId != CLight::SpotLight)
			return;

		if (lightTypeId == CLight::PointLight)
		{
			CPointLight* pl = dynamic_cast<CPointLight*>(currentLight);
			CShaderLighting::setPointLight(pl, 0);
		}
		else if (lightTypeId == CLight::SpotLight)
		{
			CSpotLight* sl = dynamic_cast<CSpotLight*>(currentLight);
			CShaderLighting::setSpotLight(sl, 0);
		}

		// render mesh with light bake shader
		video::SMaterial irrMaterial;

		irrMaterial.ZBuffer = video::ECFN_DISABLED;
		irrMaterial.ZWriteEnable = false;
		irrMaterial.BackfaceCulling = false;
		irrMaterial.FrontfaceCulling = false;

		CShadowRTTManager* shadowRTT = CShadowRTTManager::getInstance();
		ITexture* depthTexture = shadowRTT->createGetPointLightDepth(currentLight);

		irrMaterial.TextureLayer[0].Texture = depthTexture;
		irrMaterial.TextureLayer[0].BilinearFilter = false;
		irrMaterial.TextureLayer[0].TrilinearFilter = false;
		irrMaterial.TextureLayer[0].AnisotropicFilter = 0;

		bool inUV0 = shadowRP->isBakeInUV0();

		if (haveDetailNormal)
		{
			irrMaterial.TextureLayer[1].Texture = m_normalMap;
			irrMaterial.TextureLayer[1].BilinearFilter = false;
			irrMaterial.TextureLayer[1].TrilinearFilter = false;
			irrMaterial.TextureLayer[1].AnisotropicFilter = 8;

			// have normal map shader
			irrMaterial.MaterialType = inUV0 ? m_lightNUV0[lightTypeId] : m_lightN[lightTypeId];
		}
		else
		{
			irrMaterial.MaterialType = inUV0 ? m_lightUV0[lightTypeId] : m_light[lightTypeId];
		}

		if (irrMaterial.MaterialType <= 0)
			return;

		// set irrlicht material
		driver->setMaterial(irrMaterial);

		// draw mesh buffer
		driver->drawMeshBuffer(m_submesh[m_currentTarget]);

		CShaderLighting::setSpotLight(NULL, 0);
		CShaderLighting::setPointLight(NULL, 0);
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