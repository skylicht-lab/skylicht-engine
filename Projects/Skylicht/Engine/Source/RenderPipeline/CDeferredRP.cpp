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
#include "CDeferredRP.h"
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
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"
#include "Shadow/CShadowRTTManager.h"
#include "Culling/CCullingSystem.h"

namespace Skylicht
{
	bool g_enableRenderIndirect = true;
	bool g_enableRenderTestIndirect = false;

	CDeferredRP::CDeferredRP() :
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
		m_lightmapArrayShader(0),
		m_lightmapVertexShader(0),
		m_lightmapSHShader(0),
		m_lightmapColorShader(0),
		m_lightmapIndirectTestShader(0),
		m_colorInstancing(0),
		m_lmInstancingStandard(0),
		m_lmInstancingTBN(0),
		m_indirectMultipler(1.0f),
		m_directMultipler(1.0f),
		m_lightMultipler(1.0f),
		m_postProcessor(NULL)
	{
		m_type = IRenderPipeline::Deferred;
	}

	CDeferredRP::~CDeferredRP()
	{
		releaseRTT();
	}

	void CDeferredRP::initRTT(int w, int h)
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

	void CDeferredRP::releaseRTT()
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

	void CDeferredRP::initRender(int w, int h)
	{
		IVideoDriver* driver = getVideoDriver();

		// init render target
		initRTT(w, h);

		// get basic shader
		CShaderManager* shaderMgr = CShaderManager::getInstance();
		m_textureColorShader = shaderMgr->getShaderIDByName("TextureColor");
		m_textureLinearRGBShader = shaderMgr->getShaderIDByName("TextureLinearRGB");
		m_vertexColorShader = shaderMgr->getShaderIDByName("VertexColor");

		m_lightmapArrayShader = shaderMgr->getShaderIDByName("Lightmap");
		m_lightmapVertexShader = shaderMgr->getShaderIDByName("LightmapVertex");
		m_lightmapSHShader = shaderMgr->getShaderIDByName("LightmapSH");
		m_lightmapColorShader = shaderMgr->getShaderIDByName("LightmapColor");

		m_colorInstancing = shaderMgr->getShaderIDByName("ColorInstancing");

		m_lmInstancingStandard = shaderMgr->getShaderIDByName("LMStandardSGInstancing");
		m_lmInstancingTBN = shaderMgr->getShaderIDByName("LMTBNSGInstancing");

		m_lightmapIndirectTestShader = shaderMgr->getShaderIDByName("IndirectTest");

		m_lightDirection = shaderMgr->getShaderIDByName("SGDirectionalLight");
		m_lightDirectionSSR = shaderMgr->getShaderIDByName("SGDirectionalLightSSR");
		m_lightDirectionBake = shaderMgr->getShaderIDByName("SGDirectionalLightBake");

		// setup material
		initDefferredMaterial();
		initPointLightMaterial();
	}

	void CDeferredRP::resize(int w, int h)
	{
		releaseRTT();
		initRTT(w, h);

		initDefferredMaterial();
		initPointLightMaterial();
	}

	void CDeferredRP::initDefferredMaterial()
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

		m_directionalLightPass.TextureLayer[6].BilinearFilter = false;
		m_directionalLightPass.TextureLayer[6].TrilinearFilter = false;
		m_directionalLightPass.TextureLayer[6].AnisotropicFilter = 0;
		m_directionalLightPass.TextureLayer[6].TextureWrapU = video::ETC_CLAMP_TO_BORDER;
		m_directionalLightPass.TextureLayer[6].TextureWrapV = video::ETC_CLAMP_TO_BORDER;
		m_directionalLightPass.TextureLayer[6].BorderColor.set(0.0f, 0.0f, 0.0f, 0.0f);

		// disable Z
		m_directionalLightPass.ZBuffer = video::ECFN_DISABLED;
		m_directionalLightPass.ZWriteEnable = false;
	}

	void CDeferredRP::initPointLightMaterial()
	{
		CShaderManager* shaderMgr = CShaderManager::getInstance();

		m_pointLightShader = shaderMgr->getShaderIDByName("SGPointLight");
		m_pointLightShadowShader = shaderMgr->getShaderIDByName("SGPointLightShadow");

		m_pointLightPass.MaterialType = m_pointLightShader;

		m_pointLightPass.setTexture(0, m_position);
		m_pointLightPass.setTexture(1, m_normal);
		m_pointLightPass.setTexture(2, m_data);

		m_spotLightShader = shaderMgr->getShaderIDByName("SGSpotLight");

		m_spotLightPass.MaterialType = m_spotLightShader;

		m_spotLightPass.setTexture(0, m_position);
		m_spotLightPass.setTexture(1, m_normal);
		m_spotLightPass.setTexture(2, m_data);

		disableFloatTextureFilter(m_pointLightPass);
		disableFloatTextureFilter(m_spotLightPass);
	}

	void CDeferredRP::disableFloatTextureFilter(SMaterial& m)
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

	void CDeferredRP::enableTestIndirect(bool b)
	{
		g_enableRenderTestIndirect = b;
	}

	void CDeferredRP::enableRenderIndirect(bool b)
	{
		g_enableRenderIndirect = b;
	}

	bool CDeferredRP::isEnableRenderIndirect()
	{
		return g_enableRenderIndirect;
	}

	bool CDeferredRP::canRenderMaterial(CMaterial* material)
	{
		if (material->isDeferred() == true)
			return true;

		return false;
	}

	bool CDeferredRP::canRenderShader(CShader* shader)
	{
		if (shader->isDeferred() == true)
			return true;

		return false;
	}

	void CDeferredRP::drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh)
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

			if (indirectData->Type == CIndirectLightingData::VertexColor)
			{
				// change shader to vertex color
				SMaterial vertexLightmap;
				vertexLightmap.MaterialType = m_lightmapVertexShader;

				// set irrlicht material
				driver->setMaterial(vertexLightmap);

				// draw mesh buffer
				driver->drawMeshBuffer(mb);
			}
			else if (indirectData->Type == CIndirectLightingData::LightmapArray)
			{
				if (indirectData->IndirectTexture)
				{
					// change shader to vertex color
					SMaterial indirectColor;

					indirectColor.MaterialType = m_lightmapArrayShader;
					indirectColor.setTexture(0, indirectData->IndirectTexture);

					// set irrlicht material
					driver->setMaterial(indirectColor);

					// draw mesh buffer
					driver->drawMeshBuffer(mb);
				}
			}
			else if (indirectData->Type == CIndirectLightingData::SH9 && indirectData->SH)
			{
				CShaderSH::setSH9(indirectData->SH);

				// change shader to vertex color
				SMaterial shMaterial;
				shMaterial.MaterialType = m_lightmapSHShader;

				// set irrlicht material
				driver->setMaterial(shMaterial);

				// draw mesh buffer
				driver->drawMeshBuffer(mb);
			}
			else if (indirectData->Type == CIndirectLightingData::AmbientColor)
			{
				CShaderLighting::setLightAmbient(indirectData->Color);

				// change shader to color
				SMaterial shMaterial;
				shMaterial.MaterialType = m_lightmapColorShader;

				// set irrlicht material
				driver->setMaterial(shMaterial);

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

	void CDeferredRP::drawInstancingMeshBuffer(CMesh* mesh, int bufferID, CShader* instancingShader, CEntityManager* entityMgr, int entityID, bool skinnedMesh)
	{
		if (m_isIndirectPass == true)
		{
			if (mesh->IndirectLightingMesh)
			{
				IMeshBuffer* mb = mesh->IndirectLightingMesh->getMeshBuffer(bufferID);

				// Note: mesh type is change to Instancing Mesh, so compare size of vertex
				u32 size = mb->getVertexBuffer(0)->getVertexSize();

				video::SMaterial& irrMaterial = mb->getMaterial();

				if (size == sizeof(video::S3DVertex))
					irrMaterial.MaterialType = m_lmInstancingStandard;
				else if (size == sizeof(video::S3DVertexTangents))
					irrMaterial.MaterialType = m_lmInstancingTBN;

				IVideoDriver* driver = getVideoDriver();
				driver->setMaterial(irrMaterial);
				driver->drawMeshBuffer(mb);
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

	void CDeferredRP::render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId, IRenderPipeline* lastRP)
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

		float renderW = (float)m_size.Width;
		float renderH = (float)m_size.Height;

		// save projection/view for advance shader SSR
		CShaderDeferred::setProjection(driver->getTransform(video::ETS_PROJECTION));

		m_viewMatrix = driver->getTransform(video::ETS_VIEW);
		CShaderDeferred::setView(m_viewMatrix);

		// STEP 01:
		// draw baked indirect & direction lighting
		{
			m_isIndirectPass = true;
			driver->setRenderTarget(m_indirect, true, true, SColor(255, 0, 0, 0));
			if (useCustomViewport)
				driver->setViewPort(customViewport);

			if (m_updateEntity == true)
				entityManager->update();

			if (g_enableRenderIndirect == true)
				entityManager->cullingAndRender();
			m_isIndirectPass = false;
		}

		// STEP 02:
		// Render multi target to: albedo, position, normal, data (also clear Depth)
		{
			driver->setRenderTarget(m_multiRenderTarget);
			if (useCustomViewport)
				driver->setViewPort(customViewport);

			// draw entity to buffer
			if (g_enableRenderIndirect == true)
				entityManager->render();
			else
				entityManager->cullingAndRender();

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
		}

		// STEP 03:
		// draw point lighting & spot lighting
		{
			m_projectionMatrix = driver->getTransform(video::ETS_PROJECTION);
			m_viewMatrix = driver->getTransform(video::ETS_VIEW);

			if (useCustomViewport)
			{
				renderW = (float)viewport.getWidth();
				renderH = (float)viewport.getHeight();
			}

			// render light pass, clear black color
			driver->setRenderTarget(m_lightBuffer, true, false);

			// custom viewport
			if (useCustomViewport)
				driver->setViewPort(customViewport);

			u32 totalBounce = 1;
			if (CDirectionalLight::getCurrentDirectionLight() != NULL)
				totalBounce = CDirectionalLight::getCurrentDirectionLight()->getBounce();

			CLightCullingSystem* lightCullingSystem = entityManager->getSystem<CLightCullingSystem>();
			if (lightCullingSystem != NULL)
			{
				CShadowRTTManager* shadowRTT = CShadowRTTManager::getInstance();

				core::array<CLightCullingData*>& listLight = lightCullingSystem->getLightVisible();
				for (u32 i = 0, n = (u32)listLight.size(); i < n && i < s_maxLight; i++)
				{
					CLight* light = listLight[i]->Light;

					bool renderLight = true;

					if (s_bakeMode == true && s_bakeLMMode == true)
					{
						u32 lightBounce = light->getBounce();
						if (s_bakeBounce < totalBounce - lightBounce)
							renderLight = false;
					}

					// no render shadow on bake light
					if (s_bakeMode == false && light->getLightType() == CLight::Baked)
						renderLight = false;

					if (renderLight == true)
					{
						CPointLight* pointLight = dynamic_cast<CPointLight*>(light);
						if (pointLight)
						{
							CShaderLighting::setPointLight(pointLight);

							if (pointLight->isCastShadow() == true)
							{
								ITexture* depthTexture = shadowRTT->createGetPointLightDepth(pointLight);

								m_pointLightPass.MaterialType = m_pointLightShadowShader;
								m_pointLightPass.setTexture(3, depthTexture);
							}
							else
							{
								m_pointLightPass.MaterialType = m_pointLightShader;
								m_pointLightPass.setTexture(3, NULL);
							}

							beginRender2D(renderW, renderH);
							renderBufferToTarget(0.0f, 0.0f, renderW, renderH, m_pointLightPass);
						}
						else
						{
							CSpotLight* spotLight = dynamic_cast<CSpotLight*>(light);
							if (spotLight)
							{
								CShaderLighting::setSpotLight(spotLight);

								m_spotLightPass.MaterialType = m_spotLightShader;
								m_spotLightPass.setTexture(3, NULL);

								beginRender2D(renderW, renderH);
								renderBufferToTarget(0.0f, 0.0f, renderW, renderH, m_spotLightPass);
							}
						}
					}
				}
			}
		}

		// STEP 04
		// Render final direction lighting to screen
		{
			driver->setRenderTarget(m_target, false, false);

			// custom viewport
			if (useCustomViewport)
				driver->setViewPort(customViewport);

			// shadow
			CShadowMapRP* shadowRP = CShaderShadow::getShadowMapRP();
			if (shadowRP != NULL)
				m_directionalLightPass.TextureLayer[6].Texture = shadowRP->getDepthTexture();

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
			if (s_bakeMode == true)
				m_directionalLightPass.MaterialType = m_lightDirectionBake;
			else
			{
				if (enableSSR)
					m_directionalLightPass.MaterialType = m_lightDirectionSSR;
				else
					m_directionalLightPass.MaterialType = m_lightDirection;
			}

			renderBufferToTarget(0.0f, 0.0f, renderW, renderH, m_directionalLightPass);
		}

		// STEP 05
		// call forwarder rp?
		{
			// Cache culling: true will tell CCullingSystem keep the last test results, just cull the material
			CCullingSystem::useCacheCulling(true);

			core::recti fwvp(0, 0, (int)renderW, (int)renderH);
			onNext(m_target, camera, entityManager, fwvp, -1);

			CCullingSystem::useCacheCulling(false);
		}

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
		if (g_enableRenderTestIndirect == true)
		{
			m_indirect->regenerateMipMapLevels();
			SMaterial t = m_finalPass;
			t.setTexture(0, m_indirect);
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
