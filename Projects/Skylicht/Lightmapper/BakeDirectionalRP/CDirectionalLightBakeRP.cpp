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
#include "CDirectionalLightBakeRP.h"
#include "CShadowMapBakeRP.h"

#include "Lighting/CDirectionalLight.h"
#include "Lighting/CPointLight.h"

#include "EventManager/CEventManager.h"

#include "RenderMesh/CMesh.h"
#include "Material/Shader/CShaderManager.h"
#include "Material/Shader/ShaderCallback/CShaderShadow.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	CDirectionalLightBakeRP::CDirectionalLightBakeRP() :
		m_bakeDirection(0),
		m_bakeDirectionUV0(0),
		m_bakeDirectionNormal(0),
		m_bakeDirectionUV0Normal(0)
	{
		m_type = Deferred;
	}

	CDirectionalLightBakeRP::~CDirectionalLightBakeRP()
	{

	}

	void CDirectionalLightBakeRP::initRender(int w, int h)
	{
		CShaderManager* shaderMgr = CShaderManager::getInstance();
		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeDirectionalLight.xml");
		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeDirectionalLightNormal.xml");
		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeDirectionalLightUV0.xml");
		shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeDirectionalLightUV0Normal.xml");

		m_bakeDirection = shaderMgr->getShaderIDByName("BakeDirectionalLight");
		m_bakeDirectionUV0 = shaderMgr->getShaderIDByName("BakeDirectionalLightUV0");

		m_bakeDirectionNormal = shaderMgr->getShaderIDByName("BakeDirectionalLightNormal");
		m_bakeDirectionUV0Normal = shaderMgr->getShaderIDByName("BakeDirectionalLightUV0Normal");
	}

	void CDirectionalLightBakeRP::resize(int w, int h)
	{

	}

	void CDirectionalLightBakeRP::render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId, IRenderPipeline* lastRP)
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

	void CDirectionalLightBakeRP::drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh)
	{
		IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
		if (mb != m_renderMesh)
			return;

		IVideoDriver* driver = getVideoDriver();

		// shadow
		CShadowMapBakeRP* shadowRP = dynamic_cast<CShadowMapBakeRP*>(CShaderShadow::getShadowMapRP());
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

		video::SMaterial irrMaterial;
		ITexture* depthTexture = shadowRP->getDepthTexture();
		depthTexture->regenerateMipMapLevels();

		irrMaterial.TextureLayer[0].Texture = depthTexture;
		irrMaterial.TextureLayer[0].BilinearFilter = false;
		irrMaterial.TextureLayer[0].TrilinearFilter = false;
		irrMaterial.TextureLayer[0].AnisotropicFilter = 0;

		if (haveDetailNormal)
		{
			irrMaterial.TextureLayer[1].Texture = m_normalMap;
			irrMaterial.TextureLayer[1].BilinearFilter = false;
			irrMaterial.TextureLayer[1].TrilinearFilter = false;
			irrMaterial.TextureLayer[1].AnisotropicFilter = 8;

			irrMaterial.MaterialType = shadowRP->isBakeInUV0() ? m_bakeDirectionUV0Normal : m_bakeDirectionNormal;
		}
		else
		{
			irrMaterial.MaterialType = shadowRP->isBakeInUV0() ? m_bakeDirectionUV0 : m_bakeDirection;
		}

		irrMaterial.ZBuffer = video::ECFN_DISABLED;
		irrMaterial.ZWriteEnable = false;
		irrMaterial.BackfaceCulling = false;
		irrMaterial.FrontfaceCulling = false;

		if (irrMaterial.MaterialType <= 0)
			return;

		// set irrlicht material
		driver->setMaterial(irrMaterial);

		// draw mesh buffer
		driver->drawMeshBuffer(m_submesh[m_currentTarget]);
	}

	void CDirectionalLightBakeRP::drawInstancingMeshBuffer(CMesh* mesh, int bufferID, int materialRenderID, CEntityManager* entityMgr, int entityID, bool skinnedMesh)
	{
		// no render instancing mesh
	}

	bool CDirectionalLightBakeRP::canRenderMaterial(CMaterial* material)
	{
		if (material &&
			material->getShader() &&
			material->getShader()->isOpaque() == true)
			return true;

		return false;
	}

	bool CDirectionalLightBakeRP::canRenderShader(CShader* shader)
	{
		if (shader && shader->isOpaque() == true)
			return true;

		return false;
	}
}