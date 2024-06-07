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
#include "Material/CMaterial.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"
#include "Material/Shader/ShaderCallback/CShaderShadow.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"
#include "Material/Shader/ShaderCallback/CShaderTransformTexture.h"
#include "Material/Shader/CShaderManager.h"
#include "Lighting/CLightCullingSystem.h"
#include "Lighting/CPointLight.h"
#include "Lighting/CSpotLight.h"
#include "Lighting/CDirectionalLight.h"
#include "Shadow/CShadowRTTManager.h"
#include "EventManager/CEventManager.h"

namespace Skylicht
{
	CShadowMapRP::CShadowMapRP() :
		m_depthTexture(NULL),
		m_csm(NULL),
		m_sm(NULL),
		m_shadowMapType(CascadedShadow),
		m_shadowFar(300.0f),
		m_shadowMapSize(2048),
		m_numCascade(3),
		m_currentCSM(0),
		m_saveDebug(false),
		m_screenWidth(0),
		m_screenHeight(0)
	{
		m_type = ShadowMap;
		m_lightDirection.set(-1.0f, -1.0f, -1.0f);

		// write depth material
		CShaderManager* shaderMgr = CShaderManager::getInstance();

		m_texColorShader = shaderMgr->getShaderIDByName("TextureColor");
		m_skinShader = shaderMgr->getShaderIDByName("Skin");

		m_depthWriteShader = shaderMgr->getShaderIDByName("ShadowDepthWrite");
		m_depthWriteSkinMeshShader = shaderMgr->getShaderIDByName("ShadowDepthWriteSkinMesh");

		m_distanceWriteShader = shaderMgr->getShaderIDByName("ShadowLightDistanceWrite");
		m_distanceWriteSkinMeshShader = shaderMgr->getShaderIDByName("ShadowLightDistanceWriteSkinMesh");

		m_depthWriteStandardSGInstancing = shaderMgr->getShaderIDByName("SDWStandardInstancing");
		m_depthWriteTBNSGInstancing = shaderMgr->getShaderIDByName("SDWTBNInstancing");

		m_distanceWriteStandardSGInstancing = shaderMgr->getShaderIDByName("SDWLightDistanceStandardInstancing");
		m_distanceWriteTBNSGInstancing = shaderMgr->getShaderIDByName("SDWLightDistanceTBNInstancing");

		m_depthWriteSkinnedInstancing = shaderMgr->getShaderIDByName("SDWSkinInstancing");

		m_writeDepthMaterial.BackfaceCulling = false;
		m_writeDepthMaterial.FrontfaceCulling = false;

		// CEventManager::getInstance()->registerEvent("ShadowRP", this);
	}

	CShadowMapRP::~CShadowMapRP()
	{
		// CEventManager::getInstance()->unRegisterEvent(this);

		release();
	}

	void CShadowMapRP::release()
	{
		if (m_depthTexture != NULL)
			getVideoDriver()->removeTexture(m_depthTexture);

		if (m_csm != NULL)
			delete m_csm;

		if (m_sm != NULL)
			delete m_sm;

		m_depthTexture = NULL;
		m_csm = NULL;
		m_sm = NULL;
	}

	void CShadowMapRP::setShadowMapping(EShadowMapType type)
	{
		if (m_shadowMapType != type)
		{
			if (type == CShadowMapRP::CascadedShadow)
			{
				setShadowCascade(m_numCascade, m_shadowMapSize);
			}
			else
			{
				setNoShadowCascade(m_shadowMapSize);
			}
		}

		m_shadowMapType = type;
	}

	void CShadowMapRP::setShadowCascade(int numCascade, int shadowMapSize, float farValue)
	{
		m_numCascade = numCascade;
		if (m_numCascade > 3)
			m_numCascade = 3;
		if (m_numCascade < 2)
			m_numCascade = 2;

		m_shadowMapSize = shadowMapSize;
		m_shadowFar = farValue;
		m_shadowMapType = CShadowMapRP::CascadedShadow;

		if (m_screenWidth > 0 && m_screenHeight > 0)
		{
			release();
			initRender(m_screenWidth, m_screenHeight);
		}
	}

	void CShadowMapRP::setNoShadowCascade(int shadowMapSize, float farValue)
	{
		m_shadowMapSize = shadowMapSize;
		m_shadowFar = farValue;
		m_numCascade = 1;
		m_shadowMapType = CShadowMapRP::ShadowMapping;

		if (m_screenWidth > 0 && m_screenHeight > 0)
		{
			release();
			initRender(m_screenWidth, m_screenHeight);
		}
	}

	void CShadowMapRP::initRender(int w, int h)
	{
		m_screenWidth = w;
		m_screenHeight = h;

		if (m_shadowMapType == CShadowMapRP::CascadedShadow)
		{
			m_csm = new CCascadedShadowMaps();
			m_csm->init(m_numCascade, m_shadowMapSize, m_shadowFar, w, h);

			core::dimension2du size = core::dimension2du((u32)m_shadowMapSize, (u32)m_shadowMapSize);
			m_depthTexture = getVideoDriver()->addRenderTargetTextureArray(size, m_numCascade, "shadow_depth", ECF_R32F);
		}
		else
		{
			m_sm = new CShadowMaps();
			m_sm->init(m_shadowMapSize, m_shadowFar, w, h);

			core::dimension2du size = core::dimension2du((u32)m_shadowMapSize, (u32)m_shadowMapSize);
			m_depthTexture = getVideoDriver()->addRenderTargetTextureArray(size, 1, "shadow_depth", ECF_R32F);
		}
	}

	void CShadowMapRP::resize(int w, int h)
	{
		m_screenWidth = w;
		m_screenHeight = h;

		if (m_shadowMapType == CShadowMapRP::CascadedShadow)
		{
			if (m_csm != NULL)
			{
				delete m_csm;

				m_csm = new CCascadedShadowMaps();
				m_csm->init(m_numCascade, m_shadowMapSize, m_shadowFar, w, h);
			}
		}
		else
		{
			if (m_sm != NULL)
			{
				delete m_sm;

				m_sm = new CShadowMaps();
				m_sm->init(m_shadowMapSize, m_shadowFar, w, h);
			}
		}
	}

	bool CShadowMapRP::canRenderMaterial(CMaterial* m)
	{
		// render all object
		return true;
	}

	bool CShadowMapRP::canRenderShader(CShader* s)
	{
		return true;
	}

	void CShadowMapRP::drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh)
	{
		CShader* shader = NULL;

		if (mesh->Materials.size() > (u32)bufferID)
		{
			// set shader material
			CMaterial* material = mesh->Materials[bufferID];
			if (material)
				shader = material->getShader();

			CShaderMaterial::setMaterial(material);
		}

		if (shader && !shader->isDrawDepthShadow())
			return;

		IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
		IVideoDriver* driver = getVideoDriver();

		CShaderManager* shaderMgr = CShaderManager::getInstance();

		// override write depth material
		if (m_saveDebug == true)
		{
			SMaterial m = mb->getMaterial();

			if (skinnedMesh)
				m.MaterialType = m_skinShader;
			else
				m.MaterialType = m_texColorShader;

			driver->setMaterial(m);
			shaderMgr->setCurrentMaterial(m);
		}
		else
		{
			switch (m_renderShadowState)
			{
			case DirectionLight:
			{
				if (shader && shader->getShadowDepthWriteShader())
				{
					// have custom write depth
					m_writeDepthMaterial.MaterialType = shader->getShadowDepthWriteShader()->getMaterialRenderID();
				}
				else
				{
					if (skinnedMesh)
						m_writeDepthMaterial.MaterialType = m_depthWriteSkinMeshShader;
					else
						m_writeDepthMaterial.MaterialType = m_depthWriteShader;
				}
				break;
			}
			case PointLight:
				if (shader && shader->getShadowDistanceWriteShader())
				{
					// have custom write depth
					m_writeDepthMaterial.MaterialType = shader->getShadowDistanceWriteShader()->getMaterialRenderID();
				}
				else
				{
					if (skinnedMesh)
						m_writeDepthMaterial.MaterialType = m_distanceWriteSkinMeshShader;
					else
						m_writeDepthMaterial.MaterialType = m_distanceWriteShader;
					break;
				}
			}

			driver->setMaterial(m_writeDepthMaterial);
			shaderMgr->setCurrentMaterial(m_writeDepthMaterial);
		}

		// draw mesh buffer
		driver->drawMeshBuffer(mb);
	}

	void CShadowMapRP::updateShaderResource(CShader* shader, CEntityManager* entity, int entityID, video::SMaterial& irrMaterial)
	{
		for (int i = 0, n = shader->getNumResource(); i < n; i++)
		{
			CShader::SResource* res = shader->getResouceInfo(i);

			if (res->Type == CShader::TransformTexture ||
				res->Type == CShader::VertexPositionTexture ||
				res->Type == CShader::VertexNormalTexture)
			{
				ITexture* ttexture = NULL;

				if (res->Type == CShader::TransformTexture)
					ttexture = CShaderTransformTexture::getTexture();
				else if (res->Type == CShader::VertexPositionTexture)
					ttexture = CShaderTransformTexture::getPositionTexture();
				else if (res->Type == CShader::VertexNormalTexture)
					ttexture = CShaderTransformTexture::getNormalTexture();

				SUniform* uniform = shader->getVSUniform(res->Name.c_str());
				if (uniform != NULL)
				{
					u32 textureID = (u32)uniform->Value[0];
					irrMaterial.setTexture(textureID, ttexture);

					// disable mipmap
					irrMaterial.TextureLayer[textureID].BilinearFilter = false;
					irrMaterial.TextureLayer[textureID].TrilinearFilter = false;
					irrMaterial.TextureLayer[textureID].AnisotropicFilter = 0;
				}
			}
		}
	}

	void CShadowMapRP::drawInstancingMeshBuffer(CMesh* mesh, int bufferID, CShader* instancingShader, CEntityManager* entityMgr, int entityID, bool skinnedMesh)
	{
		if (m_saveDebug == true)
			return;

		if (instancingShader && !instancingShader->isDrawDepthShadow())
			return;

		IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
		IVideoDriver* driver = getVideoDriver();

		u32 vertexSize = mb->getVertexBuffer()->getVertexSize();

		bool setMaterial = false;

		CShader* shader = instancingShader->getShadowDepthWriteShader();
		if (shader == NULL)
			shader = instancingShader;

		if (shader)
			updateShaderResource(shader, entityMgr, entityID, m_writeDepthMaterial);

		if (skinnedMesh)
		{
			// skinned instancing
			switch (m_renderShadowState)
			{
			case DirectionLight:
			{
				if (instancingShader && instancingShader->getShadowDepthWriteShader())
				{
					m_writeDepthMaterial.MaterialType = instancingShader->getShadowDepthWriteShader()->getMaterialRenderID();
				}
				else
				{
					m_writeDepthMaterial.MaterialType = m_depthWriteSkinnedInstancing;
				}
				setMaterial = true;
				break;
			}
			default:
				break;
			}
		}
		else
		{
			// static instancing
			switch (m_renderShadowState)
			{
			case DirectionLight:
			{
				if (instancingShader && instancingShader->getShadowDepthWriteShader())
				{
					m_writeDepthMaterial.MaterialType = instancingShader->getShadowDepthWriteShader()->getMaterialRenderID();
					setMaterial = true;
				}
				else
				{
					if (vertexSize == sizeof(S3DVertex))
					{
						m_writeDepthMaterial.MaterialType = m_depthWriteStandardSGInstancing;
						setMaterial = true;
					}
					else if (vertexSize == sizeof(S3DVertexTangents))
					{
						m_writeDepthMaterial.MaterialType = m_depthWriteTBNSGInstancing;
						setMaterial = true;
					}
				}
				break;
			}
			case PointLight:
				if (instancingShader && instancingShader->getShadowDistanceWriteShader())
				{
					m_writeDepthMaterial.MaterialType = instancingShader->getShadowDistanceWriteShader()->getMaterialRenderID();
					setMaterial = true;
				}
				else
				{
					if (vertexSize == sizeof(S3DVertex))
					{
						m_writeDepthMaterial.MaterialType = m_distanceWriteStandardSGInstancing;
						setMaterial = true;
					}
					else if (vertexSize == sizeof(S3DVertexTangents))
					{
						m_writeDepthMaterial.MaterialType = m_distanceWriteTBNSGInstancing;
						setMaterial = true;
					}
				}
				break;
			}
		}

		if (setMaterial)
		{
			CShaderManager::getInstance()->setCurrentMaterial(m_writeDepthMaterial);

			driver->setMaterial(m_writeDepthMaterial);
			driver->drawMeshBuffer(mb);
		}
	}

	bool CShadowMapRP::OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_KEY_INPUT_EVENT)
		{
			if (event.KeyInput.Key == KEY_KEY_1 && event.KeyInput.PressedDown == false)
			{
				m_saveDebug = true;
			}
		}
		return false;
	}

	const core::aabbox3df& CShadowMapRP::getFrustumBox()
	{
		if (m_shadowMapType == CShadowMapRP::CascadedShadow)
			return m_csm->getFrustumBox(m_currentCSM);

		return m_sm->getFrustumBox();
	}

	float* CShadowMapRP::getShadowDistance()
	{
		if (m_shadowMapType == CShadowMapRP::CascadedShadow)
			return m_csm->getShadowDistance();

		return m_sm->getShadowDistance();
	}

	float* CShadowMapRP::getShadowMatrices()
	{
		if (m_shadowMapType == CShadowMapRP::CascadedShadow)
			return m_csm->getShadowMatrices();

		return m_sm->getShadowMatrices();
	}

	void CShadowMapRP::render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId, IRenderPipeline* lastRP)
	{
		if (camera == NULL)
			return;

		// use direction light
		bool castShadow = true;

		CDirectionalLight* light = CShaderLighting::getDirectionalLight();
		if (light != NULL)
		{
			m_lightDirection = light->getDirection();
			castShadow = light->isCastShadow();
		}

		// no render shadow on bake light
		if (s_bakeMode == false && light->getLightType() == CLight::Baked)
			castShadow = false;

		if (m_shadowMapType == CShadowMapRP::CascadedShadow)
			m_csm->update(camera, m_lightDirection);
		else
			m_sm->update(camera, m_lightDirection);

		setCamera(camera);

		entityManager->setCamera(camera);
		entityManager->setRenderPipeline(this);

		if (m_updateEntity == true)
			entityManager->update();

		CShaderShadow::setShadowMapRP(this);

		// render directional light shadow
		m_renderShadowState = CShadowMapRP::DirectionLight;

		IVideoDriver* driver = getVideoDriver();

		if (m_shadowMapType == CShadowMapRP::CascadedShadow)
		{
			for (int i = m_numCascade - 1; i >= 0; i--)
			{
				// note: clear while 0xFFFFFFFF for max depth value
				driver->setRenderTargetArray(m_depthTexture, i, true, true, SColor(255, 255, 255, 255));
				driver->setTransform(video::ETS_PROJECTION, m_csm->getProjectionMatrices(i));
				driver->setTransform(video::ETS_VIEW, m_csm->getViewMatrices(i));

				m_currentCSM = i;

				if (castShadow)
				{
					if (i == m_numCascade - 1)
						entityManager->cullingAndRender();
					else
						entityManager->render();
				}
			}
		}
		else
		{
			// note: clear while 0xFFFFFFFF for max depth value
			driver->setRenderTargetArray(m_depthTexture, 0, true, true, SColor(255, 255, 255, 255));
			driver->setTransform(video::ETS_PROJECTION, m_sm->getProjectionMatrices());
			driver->setTransform(video::ETS_VIEW, m_sm->getViewMatrices());

			m_currentCSM = 0;

			if (castShadow)
			{
				entityManager->cullingAndRender();
			}
		}

		// todo
		// Save output to file to test
		if (m_saveDebug == true)
		{
			core::dimension2du size = core::dimension2du((u32)m_shadowMapSize, (u32)m_shadowMapSize);
			ITexture* rtt = getVideoDriver()->addRenderTargetTexture(size, "rt", video::ECF_A8R8G8B8);

			if (m_shadowMapType == CShadowMapRP::CascadedShadow)
			{
				for (int i = 0; i < m_numCascade; i++)
				{
					driver->setRenderTarget(rtt, true, true);
					driver->setTransform(video::ETS_PROJECTION, m_csm->getProjectionMatrices(i));
					driver->setTransform(video::ETS_VIEW, m_csm->getViewMatrices(i));

					m_currentCSM = i;

					entityManager->cullingAndRender();

					driver->setRenderTarget(NULL, false, false);

					char filename[32];
					sprintf(filename, "shadow_%d.png", i);
					CBaseRP::saveFBOToFile(rtt, filename);
					os::Printer::log(filename);
				}
			}
			else
			{
				driver->setRenderTarget(rtt, true, true);
				driver->setTransform(video::ETS_PROJECTION, m_sm->getProjectionMatrices());
				driver->setTransform(video::ETS_VIEW, m_sm->getViewMatrices());

				m_currentCSM = 0;

				entityManager->cullingAndRender();

				driver->setRenderTarget(NULL, false, false);

				char filename[32];
				CBaseRP::saveFBOToFile(rtt, "shadow.png");
				os::Printer::log(filename);
			}

			getVideoDriver()->removeTexture(rtt);

			m_saveDebug = false;
		}

		// render point light shadow
		m_renderShadowState = CShadowMapRP::PointLight;

		std::vector<ITexture*> listDepthTexture;

		CLightCullingSystem* lightCullingSystem = entityManager->getSystem<CLightCullingSystem>();
		if (lightCullingSystem != NULL)
		{
			CShadowRTTManager* shadowRTT = CShadowRTTManager::getInstance();
			shadowRTT->clearLightData();

			core::array<CLightCullingData*>& listLight = lightCullingSystem->getLightVisible();
			for (u32 i = 0, n = listLight.size(); i < n && i < s_maxLight; i++)
			{
				CLight* light = listLight[i]->Light;
				CPointLight* pointLight = dynamic_cast<CPointLight*>(light);

				// no render shadow on bake light
				if (s_bakeMode == false && light->getLightType() == CLight::Baked)
					continue;

				if (pointLight != NULL &&
					pointLight->isCastShadow() == true &&
					(pointLight->needRenderShadowDepth() || pointLight->isDynamicShadow()))
				{
					CShaderLighting::setPointLight(pointLight);
					pointLight->beginRenderShadowDepth();

					core::vector3df lightPosition = pointLight->getGameObject()->getPosition();

					ITexture* depth = shadowRTT->createGetPointLightDepth(pointLight);
					if (depth != NULL)
					{
						renderCubeEnvironment(camera, entityManager, lightPosition, depth, NULL, 0, false);
						listDepthTexture.push_back(depth);
					}

					pointLight->endRenderShadowDepth();
				}
				else
				{
					CSpotLight* spotLight = dynamic_cast<CSpotLight*>(light);
					if (spotLight != NULL &&
						spotLight->isCastShadow() == true &&
						(spotLight->needRenderShadowDepth() || spotLight->isDynamicShadow()))
					{
						spotLight->beginRenderShadowDepth();

						// todo later
						// ....

						spotLight->endRenderShadowDepth();
					}
				}
			}
		}

		if (listDepthTexture.size() > 0)
		{
			setTarget(target, cubeFaceId);
		}

		onNext(target, camera, entityManager, viewport, cubeFaceId);
	}
}