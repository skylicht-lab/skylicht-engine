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
#include "CDeferredLightmapRP.h"
#include "CForwardRP.h"
#include "RenderMesh/CMesh.h"
#include "Material/CMaterial.h"
#include "Material/Shader/CShaderManager.h"
#include "Material/Shader/ShaderCallback/CShaderShadow.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"
#include "Material/Shader/ShaderCallback/CShaderDeferred.h"
#include "Material/Shader/ShaderCallback/CShaderSH.h"
#include "Lighting/CLightCullingSystem.h"
#include "Lighting/CPointLight.h"
#include "Lighting/CSpotLight.h"
#include "Lighting/CDirectionalLight.h"
#include "IndirectLighting/CIndirectLightingData.h"
#include "Lightmap/CLightmapData.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"
#include "Shadow/CShadowRTTManager.h"
#include "Culling/CCullingSystem.h"

namespace Skylicht
{
	bool CDeferredLightmapRP::s_enableRenderTestIndirect = false;

	CDeferredLightmapRP::CDeferredLightmapRP() :
		m_albedo(NULL),
		m_position(NULL),
		m_normal(NULL),
		m_data(NULL),
		m_isIndirectPass(false),
		m_vertexColorShader(0),
		m_textureColorShader(0),
		m_textureLinearRGBShader(0),
		m_pointLightShader(0),
		m_pointLightShadowShader(0),
		m_lightmapIndirectShader(0),
		m_lightmapDirectionalShader(0),
		m_lightmapIndirectTestShader(0),
		m_indirectMultipler(1.0f),
		m_directMultipler(1.0f),
		m_lightMultipler(1.0f),
		m_postProcessor(NULL)
	{
		m_type = IRenderPipeline::Deferred;
	}

	CDeferredLightmapRP::~CDeferredLightmapRP()
	{
		releaseRTT();
	}

	void CDeferredLightmapRP::initRTT(int w, int h)
	{
		IVideoDriver* driver = getVideoDriver();

		// init render target
		m_size = core::dimension2du((u32)w, (u32)h);
		m_albedo = driver->addRenderTargetTexture(m_size, "albedo", ECF_A8R8G8B8);
		m_position = driver->addRenderTargetTexture(m_size, "position", ECF_A32B32G32R32F);
		m_normal = driver->addRenderTargetTexture(m_size, "normal", ECF_A32B32G32R32F);
		m_data = driver->addRenderTargetTexture(m_size, "data", ECF_A8R8G8B8);

		m_indirect = driver->addRenderTargetTexture(m_size, "indirect", ECF_A16B16G16R16F);
		m_lightBuffer = driver->addRenderTargetTexture(m_size, "light", ECF_A16B16G16R16F);

		m_target = driver->addRenderTargetTexture(m_size, "target", ECF_A16B16G16R16F);

		// setup multi render target
		// opengles just support 4 buffer
		m_multiRenderTarget.push_back(m_albedo);
		m_multiRenderTarget.push_back(m_position);
		m_multiRenderTarget.push_back(m_normal);
		m_multiRenderTarget.push_back(m_data);
	}

	void CDeferredLightmapRP::releaseRTT()
	{
		IVideoDriver* driver = getVideoDriver();

		if (m_albedo != NULL)
			driver->removeTexture(m_albedo);

		if (m_position != NULL)
			driver->removeTexture(m_position);

		if (m_normal != NULL)
			driver->removeTexture(m_normal);

		if (m_data != NULL)
			driver->removeTexture(m_data);

		if (m_indirect != NULL)
			driver->removeTexture(m_indirect);

		if (m_lightBuffer != NULL)
			driver->removeTexture(m_lightBuffer);

		if (m_target != NULL)
			driver->removeTexture(m_target);

		m_multiRenderTarget.clear();
	}

	void CDeferredLightmapRP::initRender(int w, int h)
	{
		IVideoDriver* driver = getVideoDriver();

		// init render target
		initRTT(w, h);

		// get basic shader
		CShaderManager* shaderMgr = CShaderManager::getInstance();
		m_textureColorShader = shaderMgr->getShaderIDByName("TextureColor");
		m_textureLinearRGBShader = shaderMgr->getShaderIDByName("TextureLinearRGB");
		m_vertexColorShader = shaderMgr->getShaderIDByName("VertexColor");

		m_lightmapIndirectShader = shaderMgr->getShaderIDByName("Lightmap");
		m_lightmapDirectionalShader = shaderMgr->getShaderIDByName("LightmapDirection");

		m_lightmapIndirectTestShader = shaderMgr->getShaderIDByName("IndirectTest");

		m_lightDirection = shaderMgr->getShaderIDByName("SGLightmap");

		// setup material
		initDefferredMaterial();
	}

	void CDeferredLightmapRP::resize(int w, int h)
	{
		releaseRTT();
		initRTT(w, h);

		initDefferredMaterial();
	}

	void CDeferredLightmapRP::initDefferredMaterial()
	{
		m_directionalLightPass.MaterialType = m_lightDirection;

		m_directionalLightPass.setTexture(0, m_albedo);
		m_directionalLightPass.setTexture(1, m_position);
		m_directionalLightPass.setTexture(2, m_normal);
		m_directionalLightPass.setTexture(3, m_data);

		// point light color
		m_directionalLightPass.setTexture(4, m_lightBuffer);

		// indirect light color
		m_directionalLightPass.setTexture(5, m_indirect);

		// turn off mipmap on float texture	
		m_directionalLightPass.TextureLayer[1].BilinearFilter = false;
		m_directionalLightPass.TextureLayer[1].TrilinearFilter = false;
		m_directionalLightPass.TextureLayer[1].AnisotropicFilter = 0;

		m_directionalLightPass.TextureLayer[2].BilinearFilter = false;
		m_directionalLightPass.TextureLayer[2].TrilinearFilter = false;
		m_directionalLightPass.TextureLayer[2].AnisotropicFilter = 0;

		m_directionalLightPass.TextureLayer[4].BilinearFilter = false;
		m_directionalLightPass.TextureLayer[4].TrilinearFilter = false;
		m_directionalLightPass.TextureLayer[4].AnisotropicFilter = 0;

		m_directionalLightPass.TextureLayer[5].BilinearFilter = false;
		m_directionalLightPass.TextureLayer[5].TrilinearFilter = false;
		m_directionalLightPass.TextureLayer[5].AnisotropicFilter = 0;

		// disable Z
		m_directionalLightPass.ZBuffer = video::ECFN_DISABLED;
		m_directionalLightPass.ZWriteEnable = false;
	}

	void CDeferredLightmapRP::disableFloatTextureFilter(SMaterial& m)
	{
		// turn off mipmap on float texture	
		m.TextureLayer[0].BilinearFilter = false;
		m.TextureLayer[0].TrilinearFilter = false;
		m.TextureLayer[0].AnisotropicFilter = 0;

		m.TextureLayer[1].BilinearFilter = false;
		m.TextureLayer[1].TrilinearFilter = false;
		m.TextureLayer[1].AnisotropicFilter = 0;

		m.TextureLayer[2].BilinearFilter = false;
		m.TextureLayer[2].TrilinearFilter = false;
		m.TextureLayer[2].AnisotropicFilter = 0;

		m.TextureLayer[3].BilinearFilter = false;
		m.TextureLayer[3].TrilinearFilter = false;
		m.TextureLayer[3].AnisotropicFilter = 0;

		// disable Z
		m.ZBuffer = video::ECFN_DISABLED;
		m.ZWriteEnable = false;
	}

	void CDeferredLightmapRP::enableTestIndirect(bool b)
	{
		s_enableRenderTestIndirect = b;
	}

	bool CDeferredLightmapRP::canRenderMaterial(CMaterial* material)
	{
		if (material->isDeferred() == true)
			return true;

		return false;
	}

	bool CDeferredLightmapRP::canRenderShader(CShader* shader)
	{
		if (shader->isDeferred() == true)
			return true;

		return false;
	}

	void CDeferredLightmapRP::drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh)
	{
		if (m_isIndirectPass == true)
		{
			// read indirect lighting data
			CIndirectLightingData* indirectData = GET_ENTITY_DATA(entity->getEntity(entityID), CIndirectLightingData);
			if (indirectData == NULL)
				return;

			// set shader (uniform) material
			if (mesh->Materials.size() > (u32)bufferID)
				CShaderMaterial::setMaterial(mesh->Materials[bufferID]);

			IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
			IVideoDriver* driver = getVideoDriver();

			video::SMaterial& material = mb->getMaterial();

			if (indirectData->Type == CIndirectLightingData::LightmapArray)
			{
				// change shader to vertex color
				SMaterial lightmapMat;

				lightmapMat.MaterialType = m_lightmapIndirectShader;
				lightmapMat.setTexture(0, indirectData->IndirectTexture);

				// set irrlicht material
				driver->setMaterial(lightmapMat);

				// draw mesh buffer
				driver->drawMeshBuffer(mb);
			}
		}
		else if (m_isDirectionalPass)
		{
			CIndirectLightingData* indirectData = GET_ENTITY_DATA(entity->getEntity(entityID), CIndirectLightingData);
			if (indirectData == NULL)
				return;

			CLightmapData* lightmapData = GET_ENTITY_DATA(entity->getEntity(entityID), CLightmapData);
			if (lightmapData == NULL)
				return;

			if (indirectData->IndirectTexture == NULL ||
				indirectData->IndirectTexture != lightmapData->LightmapTexture)
				return;

			// set shader (uniform) material
			if (mesh->Materials.size() > (u32)bufferID)
				CShaderMaterial::setMaterial(mesh->Materials[bufferID]);

			IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
			IVideoDriver* driver = getVideoDriver();

			video::SMaterial& material = mb->getMaterial();

			if (indirectData->Type == CIndirectLightingData::LightmapArray)
			{
				// change shader to vertex color
				SMaterial lightmapMat;

				lightmapMat.MaterialType = m_lightmapDirectionalShader;
				lightmapMat.setTexture(0, indirectData->IndirectTexture);

				CShaderManager::getInstance()->LightmapIndex = (float)lightmapData->LightmapIndex;

				// set irrlicht material
				driver->setMaterial(lightmapMat);

				// draw mesh buffer
				driver->drawMeshBuffer(mb);
			}
		}
		else
		{
			if (s_bakeMode == true)
			{
				// update texture resource
				updateTextureResource(mesh, bufferID, entity, entityID, skinnedMesh);

				IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
				IVideoDriver* driver = getVideoDriver();

				video::SMaterial irrMaterial = mb->getMaterial();
				irrMaterial.BackfaceCulling = false;

				// set irrlicht material
				driver->setMaterial(irrMaterial);

				// draw mesh buffer
				driver->drawMeshBuffer(mb);
			}
			else
			{
				// default render
				CBaseRP::drawMeshBuffer(mesh, bufferID, entity, entityID, skinnedMesh);
			}
		}
	}

	void CDeferredLightmapRP::drawInstancingMeshBuffer(CMesh* mesh, int bufferID, CShader* instancingShader, CEntityManager* entityMgr, int entityID, bool skinnedMesh)
	{
		if (m_isIndirectPass == true)
		{
			if (mesh->IndirectLightingMesh)
			{
				// not yet support instancing lightmap
			}
		}
		else
		{
			if (s_bakeMode == true)
			{
				IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
				IVideoDriver* driver = getVideoDriver();

				video::SMaterial irrMaterial = mb->getMaterial();
				irrMaterial.MaterialType = instancingShader->getMaterialRenderID();
				irrMaterial.BackfaceCulling = false;

				driver->setMaterial(irrMaterial);
				driver->drawMeshBuffer(mb);
			}
			else
			{
				CBaseRP::drawInstancingMeshBuffer(mesh, bufferID, instancingShader, entityMgr, entityID, skinnedMesh);
			}
		}
	}

	void CDeferredLightmapRP::render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId, IRenderPipeline* lastRP)
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

		// save projection/view for advance shader SSR
		CShaderDeferred::setProjection(driver->getTransform(video::ETS_PROJECTION));

		m_viewMatrix = driver->getTransform(video::ETS_VIEW);
		CShaderDeferred::setView(m_viewMatrix);

		// STEP 01:
		// draw baked indirect
		m_isIndirectPass = true;
		driver->setRenderTarget(m_indirect, true, true, SColor(255, 0, 0, 0));
		if (useCustomViewport)
			driver->setViewPort(customViewport);

		if (m_updateEntity == true)
		{
			entityManager->update();
			entityManager->render();
		}
		else
		{
			entityManager->cullingAndRender();
		}
		m_isIndirectPass = false;

		// baked direction lighting pass
		m_isDirectionalPass = true;
		driver->setRenderTarget(m_lightBuffer, true, true, SColor(255, 0, 0, 0));
		if (useCustomViewport)
			driver->setViewPort(customViewport);
		entityManager->render();
		m_isDirectionalPass = false;


		// STEP 02:
		// Render multi target to: albedo, position, normal, data		
		driver->setRenderTarget(m_multiRenderTarget);
		if (useCustomViewport)
			driver->setViewPort(customViewport);

		// draw entity to buffer
		entityManager->render();

		// Apply uniform: uLightMultiplier
		if (CBaseRP::s_bakeMode == true && CBaseRP::s_bakeLMMode)
		{
			// default light setting
			CShaderManager::getInstance()->ShaderVec3[0] = core::vector3df(1.0f, 1.0f, 1.0f);
		}
		else
		{
			CShaderManager::getInstance()->ShaderVec3[0] = core::vector3df(m_directMultipler, m_indirectMultipler, m_lightMultipler);
		}

		// STEP 03:
		// save camera transform
		m_projectionMatrix = driver->getTransform(video::ETS_PROJECTION);
		m_viewMatrix = driver->getTransform(video::ETS_VIEW);

		float renderW = (float)m_size.Width;
		float renderH = (float)m_size.Height;

		if (useCustomViewport)
		{
			renderW = (float)viewport.getWidth();
			renderH = (float)viewport.getHeight();
		}

		// STEP 04
		// Render final direction lighting to screen
		driver->setRenderTarget(m_target, false, false);

		// custom viewport
		if (useCustomViewport)
			driver->setViewPort(customViewport);

		// ssr (last frame)
		bool enableSSR = false;

		if (m_postProcessor != NULL &&
			m_postProcessor->getLastFrameBuffer() &&
			m_postProcessor->isEnableScreenSpaceReflection())
		{
			enableSSR = true;
			m_directionalLightPass.TextureLayer[7].Texture = m_postProcessor->getLastFrameBuffer();
			m_directionalLightPass.TextureLayer[7].TextureWrapU = ETC_CLAMP_TO_BORDER;
			m_directionalLightPass.TextureLayer[7].TextureWrapV = ETC_CLAMP_TO_BORDER;
		}

		beginRender2D(renderW, renderH);

		// enable backface shader if bake lightmap mode
		m_directionalLightPass.MaterialType = m_lightDirection;

		renderBufferToTarget(0.0f, 0.0f, renderW, renderH, m_directionalLightPass);

		CCullingSystem::useCacheCulling(true);

		// STEP 05
		// call forwarder rp?
		core::recti fwvp(0, 0, (int)renderW, (int)renderH);
		onNext(m_target, camera, entityManager, fwvp, cubeFaceId);

		CCullingSystem::useCacheCulling(false);

		// STEP 06
		// final pass to screen
		if (m_postProcessor != NULL && s_bakeMode == false)
		{
			ITexture* emission = NULL;
			if (m_next->getType() == IRenderPipeline::Forwarder)
				emission = ((CForwardRP*)m_next)->getEmissionTexture();

			m_postProcessor->postProcessing(target, m_target, emission, m_normal, m_position, viewport, cubeFaceId);
		}
		else
		{
			setTarget(target, cubeFaceId);

			if (useCustomViewport)
				driver->setViewPort(viewport);

			beginRender2D(renderW, renderH);
			m_finalPass.setTexture(0, m_target);
			m_finalPass.MaterialType = m_textureLinearRGBShader;
			renderBufferToTarget(0.0f, 0.0f, renderW, renderH, m_finalPass);
		}

		// test
		if (s_enableRenderTestIndirect == true)
		{
			SMaterial t = m_finalPass;
			t.setTexture(0, m_indirect);
			// t.setTexture(0, m_lightBuffer);
			t.MaterialType = m_lightmapIndirectTestShader;
			renderBufferToTarget(0.0f, 0.0f, renderW, renderH, 0.0f, 0.0f, renderW, renderH, t);
		}

		// test
		/*
		SMaterial t = m_finalPass;
		t.setTexture(0, m_data);
		t.MaterialType = m_textureColorShader;
		renderBufferToTarget(0.0f, 0.0f, renderW, renderH, 0.0f, 0.0f, renderW, renderH, t);
		*/
	}
}
