/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
#include "CAreaLightBakeRP.h"
#include "BakeDirectionalRP/CShadowMapBakeRP.h"

#include "Lighting/CAreaLight.h"

#include "EventManager/CEventManager.h"

#include "RenderMesh/CMesh.h"
#include "Material/Shader/CShaderManager.h"
#include "Material/Shader/ShaderCallback/CShaderShadow.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"
#include "Material/CMaterial.h"

#include "Shadow/CShadowRTTManager.h"

namespace Skylicht
{
	CAreaLightBakeRP::CAreaLightBakeRP() :
		m_light(0),
		m_lightUV0(0),
		m_lightN(0),
		m_lightNUV0(0)
	{
		m_type = Deferred;
	}

	CAreaLightBakeRP::~CAreaLightBakeRP()
	{

	}

	void CAreaLightBakeRP::initRender(int w, int h)
	{
		CShaderManager* shaderMgr = CShaderManager::getInstance();
		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeAreaLight.xml");
		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeAreaLightUV0.xml");

		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeAreaLightNormal.xml");
		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeAreaLightUV0Normal.xml");

		m_light = shaderMgr->getShaderIDByName("BakeAreaLight");
		m_lightUV0 = shaderMgr->getShaderIDByName("BakeAreaLightUV0");
		m_lightN = shaderMgr->getShaderIDByName("BakeAreaLightNormal");
		m_lightNUV0 = shaderMgr->getShaderIDByName("BakeAreaLightUV0Normal");
	}

	void CAreaLightBakeRP::resize(int w, int h)
	{

	}

	void CAreaLightBakeRP::render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId, IRenderPipeline* lastRP)
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

	void CAreaLightBakeRP::drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh)
	{
		// just render the render mesh
		IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
		if (mb != m_renderMesh)
			return;

		// shadow
		CShadowMapBakeRP* shadowRP = dynamic_cast<CShadowMapBakeRP*>(CShaderShadow::getShadowMapRP());
		if (shadowRP == NULL)
			return;

		IVideoDriver* driver = getVideoDriver();

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

		if (lightTypeId != CLight::AreaLight)
			return;

		CAreaLight* al = dynamic_cast<CAreaLight*>(currentLight);
		CShaderLighting::setAreaLight(al, 0);

		// render mesh with light bake shader
		video::SMaterial irrMaterial;

		irrMaterial.ZBuffer = video::ECFN_DISABLED;
		irrMaterial.ZWriteEnable = false;
		irrMaterial.BackfaceCulling = false;
		irrMaterial.FrontfaceCulling = false;

		/*
		CShadowRTTManager* shadowRTT = CShadowRTTManager::getInstance();
		ITexture* depthTexture = shadowRTT->createGetPointLightDepth(currentLight);

		irrMaterial.TextureLayer[0].Texture = depthTexture;
		irrMaterial.TextureLayer[0].BilinearFilter = false;
		irrMaterial.TextureLayer[0].TrilinearFilter = false;
		irrMaterial.TextureLayer[0].AnisotropicFilter = 0;
		*/

		bool inUV0 = shadowRP->isBakeInUV0();

		if (haveDetailNormal)
		{
			irrMaterial.TextureLayer[1].Texture = m_normalMap;
			irrMaterial.TextureLayer[1].BilinearFilter = false;
			irrMaterial.TextureLayer[1].TrilinearFilter = false;
			irrMaterial.TextureLayer[1].AnisotropicFilter = 8;

			// have normal map shader
			irrMaterial.MaterialType = inUV0 ? m_lightNUV0 : m_lightN;
		}
		else
		{
			irrMaterial.MaterialType = inUV0 ? m_lightUV0 : m_light;
		}

		if (irrMaterial.MaterialType <= 0)
			return;

		// set irrlicht material
		driver->setMaterial(irrMaterial);

		// draw mesh buffer
		driver->drawMeshBuffer(m_submesh[m_currentTarget]);

		CShaderLighting::setAreaLight(NULL, 0);
	}

	void CAreaLightBakeRP::drawInstancingMeshBuffer(CMesh* mesh, int bufferID, int materialRenderID, CEntityManager* entityMgr, int entityID, bool skinnedMesh)
	{
		// no render instancing mesh
	}

	bool CAreaLightBakeRP::canRenderMaterial(CMaterial* material)
	{
		if (material &&
			material->getShader() &&
			material->getShader()->isOpaque() == true)
			return true;

		return false;
	}

	bool CAreaLightBakeRP::canRenderShader(CShader* shader)
	{
		if (shader && shader->isOpaque() == true)
			return true;

		return false;
	}
}