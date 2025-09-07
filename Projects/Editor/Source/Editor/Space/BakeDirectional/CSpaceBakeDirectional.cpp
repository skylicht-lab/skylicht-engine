/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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

#ifdef BUILD_SKYLICHT_LIGHMAPPER

#include "GUI/GUIContext.h"
#include "GUI/Controls/CMessageBox.h"

#include "CSpaceBakeDirectional.h"
#include "AssetManager/CAssetManager.h"
#include "Editor/CEditor.h"

#include "Serializable/CSerializableLoader.h"

#include "Shadow/CShadowRTTManager.h"
#include "Material/Shader/ShaderCallback/CShaderShadow.h"

#include <filesystem>
#if defined(__APPLE_CC__)
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace Skylicht
{
	namespace Editor
	{
		CSpaceBakeDirectional::CSpaceBakeDirectional(GUI::CWindow* window, CEditor* editor) :
			CSpaceLoading(window, editor),
			m_progressBar(NULL),
			m_statusText(NULL),
			m_state(None),
			m_position(0),
			m_total(0),
			m_shadowBias(0.0004f),
			m_bakeAll(false),
			m_bakeUV0(false),
			m_bakeDetailNormal(false),
			m_combineDirectionLightColor(true),
			m_bakeSize(2048),
			m_cameraObj(NULL),
			m_shadowRP(NULL),
			m_bakeLightRP(NULL),
			m_shadowPLRP(NULL),
			m_bakePointLightRP(NULL),
			m_shadowALRP(NULL),
			m_bakeAreaLightRP(NULL)
		{
			m_progressBar = new GUI::CProgressBar(window);
			m_progressBar->dock(GUI::EPosition::Top);
			m_progressBar->setMargin(GUI::SMargin(14.0f, 14.0, 14.0, 0.0f));

			m_statusText = new GUI::CLabel(window);
			m_statusText->dock(GUI::EPosition::Fill);
			m_statusText->setMargin(GUI::SMargin(14.0f, 5.0, 14.0, 0.0f));
			m_statusText->setWrapMultiline(true);
			m_statusText->setString(L"Bake directional...");

			for (int i = 0; i < MAX_LIGHTMAP_ATLAS; i++)
			{
				m_directionLightBake[i] = NULL;
				m_pointLightBake[i] = NULL;
				m_result[i] = NULL;
				m_subMesh[i] = NULL;
			}
		}

		CSpaceBakeDirectional::~CSpaceBakeDirectional()
		{
			if (m_shadowRP)
				delete m_shadowRP;

			if (m_bakeLightRP)
				delete m_bakeLightRP;

			if (m_shadowPLRP)
				delete m_shadowPLRP;

			if (m_bakePointLightRP)
				delete m_bakePointLightRP;

			if (m_shadowALRP)
				delete m_shadowALRP;

			if (m_bakeAreaLightRP)
				delete m_bakeAreaLightRP;

			IVideoDriver* driver = getVideoDriver();

			for (int i = 0; i < MAX_LIGHTMAP_ATLAS; i++)
			{
				if (m_directionLightBake[i])
					driver->removeTexture(m_directionLightBake[i]);

				if (m_pointLightBake[i])
					driver->removeTexture(m_pointLightBake[i]);

				if (m_result[i])
					driver->removeTexture(m_result[i]);

				if (m_subMesh[i])
					m_subMesh[i]->drop();
			}

			if (m_cameraObj)
				m_cameraObj->remove();
		}

		void CSpaceBakeDirectional::update()
		{
			if (m_state == Init)
			{
				m_state = Bake;
			}
			else if (m_state == Bake)
			{
				if (m_position >= m_total)
					m_state = Finish;
				else
				{
					// clean last frame mesh
					for (int i = 0; i < MAX_LIGHTMAP_ATLAS; i++)
					{
						if (m_subMesh[i])
						{
							m_subMesh[i]->drop();
							m_subMesh[i] = NULL;
						}
					}

					if (m_bakeUV0)
						doBakeUV0();
					else
						doBakeLightmap();
				}
			}
			else if (m_state == Finish)
			{
				saveOutput();

				// done
				CEditor::getInstance()->refresh();
				m_finished = true;

				// need refresh all lighting
				CShadowRTTManager::getInstance()->clearTextures();

				if (m_total == 0)
				{
					GUI::CMessageBox* msb = new GUI::CMessageBox(m_window->getCanvas(), GUI::CMessageBox::OK);
					msb->setMessage("Lightmapper not found RenderMesh or Static Object", "");
				}
			}

			CSpace::update();
		}

		void CSpaceBakeDirectional::doBakeUV0()
		{
			IMeshBuffer* mb = m_meshBuffers[m_position];
			ITexture* normalMap = m_normalMaps[m_position];

			CEntityManager* entityMgr = m_cameraObj->getEntityManager();

			int numTargetTexture = 1;
			m_subMesh[0] = mb;

			// init render target
			for (int i = 0; i < numTargetTexture; i++)
			{
				if (m_directionLightBake[i] == NULL)
				{
					core::dimension2du size(m_bakeSize, m_bakeSize);

					m_directionLightBake[i] = getVideoDriver()->addRenderTargetTexture(size, "bakeLM", video::ECF_A8R8G8B8);
					getVideoDriver()->setRenderTarget(m_directionLightBake[i], true, true, SColor(0, 0, 0, 0));

					m_pointLightBake[i] = getVideoDriver()->addRenderTargetTexture(size, "bakeLM", video::ECF_A8R8G8B8);
					getVideoDriver()->setRenderTarget(m_pointLightBake[i], true, true, SColor(0, 0, 0, 0));
				}
			}

			// calc bbox of mesh
			const core::matrix4& transform = m_meshTransforms[m_position];
			core::aabbox3df box = mb->getBoundingBox();
			transform.transformBoxEx(box);

			// set box & mesh that render light
			m_shadowRP->setBound(box);

			// setup target for pipeline
			m_bakeLightRP->setRenderMesh(mb, normalMap, m_subMesh, m_directionLightBake, numTargetTexture);
			m_bakePointLightRP->setRenderMesh(mb, normalMap, m_subMesh, m_pointLightBake, numTargetTexture);
			m_bakeAreaLightRP->setRenderMesh(mb, normalMap, m_subMesh, m_pointLightBake, numTargetTexture);

			// set camera view
			core::vector3df center = box.getCenter();
			core::vector3df v = box.MaxEdge - center;

			// set camera, that make sure the meshbuffer always render, that is view
			CCamera* bakeCamera = m_cameraObj->getComponent<CCamera>();
			bakeCamera->setPosition(center + v * 1.5f);
			bakeCamera->lookAt(center, core::vector3df(0.0f, 1.0f, 0.0f));

			// update scene first
			m_cameraObj->getScene()->update();

			// bake all point light
			for (CLight* light : m_lights)
			{
				if (light->getLightTypeId() == CLight::DirectionalLight)
				{
					SVec4 oldBias = CShaderShadow::getShadowBias();
					SVec4 bias = oldBias;
					bias.X = m_shadowBias;
					CShaderShadow::setShadowBias(bias);

					m_shadowRP->setCurrentLight(light);
					m_shadowRP->setBakeInUV0(m_bakeUV0);
					m_shadowRP->setBakeDetailNormal(m_bakeDetailNormal);
					m_shadowRP->render(NULL, bakeCamera, entityMgr, core::recti());

					CShaderShadow::setShadowBias(oldBias);
				}
				else if (light->getLightTypeId() == CLight::AreaLight)
				{
					m_shadowALRP->setCurrentLight(light);
					m_shadowALRP->setBakeInUV0(m_bakeUV0);
					m_shadowALRP->setBakeDetailNormal(m_bakeDetailNormal);
					m_shadowALRP->render(NULL, bakeCamera, entityMgr, core::recti());
				}
				else
				{
					m_shadowPLRP->setCurrentLight(light);
					m_shadowPLRP->setBakeInUV0(m_bakeUV0);
					m_shadowPLRP->setBakeDetailNormal(m_bakeDetailNormal);
					m_shadowPLRP->render(NULL, bakeCamera, entityMgr, core::recti());
				}
			}

			m_position++;
		}

		void CSpaceBakeDirectional::doBakeLightmap()
		{
			IMeshBuffer* mb = m_meshBuffers[m_position];
			ITexture* normalMap = m_normalMaps[m_position];

			video::E_VERTEX_TYPE vtxType = mb->getVertexType();
			if (vtxType != video::EVT_2TCOORDS &&
				vtxType != video::EVT_2TCOORDS_TANGENTS)
			{
				m_position++;
				return;
			}

			CEntityManager* entityMgr = m_cameraObj->getEntityManager();
			int numTargetTexture = 0;

			IVertexBuffer* srcVtx = mb->getVertexBuffer();
			IIndexBuffer* srcIdx = mb->getIndexBuffer();

			u32 vtxSize = srcVtx->getVertexSize();

			unsigned char* vertices = (unsigned char*)srcVtx->getVertices();
			void* indices = srcIdx->getIndices();

			E_INDEX_TYPE type = srcIdx->getType();

			// collect the lightmap index to many submesh
			int count = srcIdx->getIndexCount();
			for (int i = 0; i < count; i += 3)
			{
				u32 vertexId0 = 0;
				u32 vertexId1 = 0;
				u32 vertexId2 = 0;

				if (type == EIT_16BIT)
				{
					u16* data = (u16*)indices;
					vertexId0 = (u32)data[i];
					vertexId1 = (u32)data[i + 1];
					vertexId2 = (u32)data[i + 2];
				}
				else
				{
					u32* data = (u32*)indices;
					vertexId0 = data[i];
					vertexId1 = data[i + 1];
					vertexId2 = data[i + 2];
				}

				// query triangle lightmap index
				int lightmapIndex = -1;
				void* vtxData = vertices + vertexId0 * vtxSize;

				if (vtxType == video::EVT_2TCOORDS)
					lightmapIndex = (int)((video::S3DVertex2TCoords*)vtxData)->Lightmap.Z;
				else
					lightmapIndex = (int)((video::S3DVertex2TCoordsTangents*)vtxData)->Lightmap.Z;

				if (lightmapIndex < 0 || lightmapIndex > MAX_LIGHTMAP_ATLAS)
					continue;

				if (m_subMesh[lightmapIndex] == NULL)
				{
					m_subMesh[lightmapIndex] = new CMeshBuffer<S3DVertex2TCoordsTangents>(getVideoDriver()->getVertexDescriptor(EVT_2TCOORDS_TANGENTS), type);
					CMeshUtils::copyVertices(srcVtx, m_subMesh[lightmapIndex]->getVertexBuffer());
				}

				IIndexBuffer* idx = m_subMesh[lightmapIndex]->getIndexBuffer();
				idx->addIndex(vertexId0);
				idx->addIndex(vertexId1);
				idx->addIndex(vertexId2);

				// calc max textures
				if (lightmapIndex + 1 > numTargetTexture)
					numTargetTexture = lightmapIndex + 1;
			}

			// init render target
			for (int i = 0; i < numTargetTexture; i++)
			{
				if (m_directionLightBake[i] == NULL)
				{
					core::dimension2du size(m_bakeSize, m_bakeSize);

					m_directionLightBake[i] = getVideoDriver()->addRenderTargetTexture(size, "bakeLM", video::ECF_A8R8G8B8);
					getVideoDriver()->setRenderTarget(m_directionLightBake[i], true, true, SColor(0, 0, 0, 0));

					m_pointLightBake[i] = getVideoDriver()->addRenderTargetTexture(size, "bakeLM", video::ECF_A8R8G8B8);
					getVideoDriver()->setRenderTarget(m_pointLightBake[i], true, true, SColor(0, 0, 0, 0));
				}
			}

			// calc bbox of mesh
			const core::matrix4& transform = m_meshTransforms[m_position];
			core::aabbox3df box = mb->getBoundingBox();
			transform.transformBoxEx(box);

			// set box & mesh that render light
			m_shadowRP->setBound(box);

			// setup target for pipeline
			m_bakeLightRP->setRenderMesh(mb, normalMap, m_subMesh, m_directionLightBake, numTargetTexture);
			m_bakePointLightRP->setRenderMesh(mb, normalMap, m_subMesh, m_pointLightBake, numTargetTexture);
			m_bakeAreaLightRP->setRenderMesh(mb, normalMap, m_subMesh, m_pointLightBake, numTargetTexture);

			// set camera view
			core::vector3df center = box.getCenter();
			core::vector3df v = box.MaxEdge - center;

			// set camera, that make sure the meshbuffer always render, that is view
			CCamera* bakeCamera = m_cameraObj->getComponent<CCamera>();
			bakeCamera->setPosition(center + v * 1.5f);
			bakeCamera->lookAt(center, core::vector3df(0.0f, 1.0f, 0.0f));

			// update scene first
			m_cameraObj->getScene()->update();

			// bake all point light
			for (CLight* light : m_lights)
			{
				if (light->getLightTypeId() == CLight::DirectionalLight)
				{
					SVec4 oldBias = CShaderShadow::getShadowBias();

					SVec4 bias = oldBias;
					bias.X = m_shadowBias;
					CShaderShadow::setShadowBias(bias);

					m_shadowRP->setCurrentLight(light);
					m_shadowRP->setBakeInUV0(m_bakeUV0);
					m_shadowRP->setBakeDetailNormal(m_bakeDetailNormal);
					m_shadowRP->render(NULL, bakeCamera, entityMgr, core::recti());

					CShaderShadow::setShadowBias(oldBias);
				}
				else if (light->getLightTypeId() == CLight::AreaLight)
				{
					m_shadowALRP->setCurrentLight(light);
					m_shadowALRP->setBakeInUV0(m_bakeUV0);
					m_shadowALRP->setBakeDetailNormal(m_bakeDetailNormal);
					m_shadowALRP->render(NULL, bakeCamera, entityMgr, core::recti());
				}
				else
				{
					m_shadowPLRP->setCurrentLight(light);
					m_shadowPLRP->setBakeInUV0(m_bakeUV0);
					m_shadowPLRP->setBakeDetailNormal(m_bakeDetailNormal);
					m_shadowPLRP->render(NULL, bakeCamera, entityMgr, core::recti());
				}
			}

			m_position++;
		}

		void CSpaceBakeDirectional::saveOutput()
		{
			CShaderManager* shaderMgr = CShaderManager::getInstance();
			shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeFinal.xml");
			shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeFinalNoDirectionLight.xml");

			for (int i = 0; i < MAX_LIGHTMAP_ATLAS; i++)
			{
				if (m_directionLightBake[i] == NULL)
					continue;

				// bake final, that merge direction light & point light & shadow mask in alpha channel
				std::string output;

				core::dimension2du size(m_bakeSize, m_bakeSize);
				float lmSize = (float)m_bakeSize;

				m_result[i] = getVideoDriver()->addRenderTargetTexture(size, "resultLM", video::ECF_A8R8G8B8);

				getVideoDriver()->setRenderTarget(m_result[i]);
				m_bakeLightRP->beginRender2D(lmSize, lmSize);
				SMaterial material;
				material.setTexture(0, m_directionLightBake[i]);
				material.setTexture(1, m_pointLightBake[i]);

				if (m_combineDirectionLightColor)
					material.MaterialType = shaderMgr->getShaderIDByName("BakeFinal");
				else
					material.MaterialType = shaderMgr->getShaderIDByName("BakeFinalNoDirectionLight");

				bool flipY = false;
				if (getVideoDriver()->getDriverType() != video::EDT_DIRECT3D11)
					flipY = true;

				m_bakeLightRP->renderBufferToTarget(0.0f, 0.0f, lmSize, lmSize, material, flipY);
				getVideoDriver()->setRenderTarget(NULL);

				/*
				if (m_directionLightBake[i])
				{
					char outFileName[512];
					sprintf(outFileName, "LightMapDirectional_%d.png", i);
					output = m_folder + outFileName;
					CBaseRP::saveFBOToFile(m_directionLightBake[i], output.c_str());
				}

				if (m_pointLightBake[i])
				{
					char outFileName[512];
					sprintf(outFileName, "LightMapPLight_%d.png", i);
					output = m_folder + outFileName;
					CBaseRP::saveFBOToFile(m_pointLightBake[i], output.c_str());
				}
				*/

				// final texture
				{
					char outFileName[512];
					sprintf(outFileName, m_file.c_str(), i);
					output = m_folder + outFileName;
					CBaseRP::saveFBOToFile(m_result[i], output.c_str());
				}
			}
		}

		void CSpaceBakeDirectional::onDestroy(GUI::CBase* base)
		{
			CSpace::onDestroy(base);
		}

		void CSpaceBakeDirectional::bake(Lightmapper::CBakeLightComponent* bakeLight)
		{
			m_folder = bakeLight->getOutputFolder();
			if (!m_folder.empty())
				m_folder += "/";

			m_file = bakeLight->getOutputFile();
			m_shadowBias = bakeLight->getShadowBias();
			m_bakeAll = bakeLight->bakeAll();
			m_bakeUV0 = bakeLight->bakeUV0();
			m_bakeDetailNormal = bakeLight->bakeDetailNormal();
			m_combineDirectionLightColor = bakeLight->combineDirectionLightColor();
			m_bakeSize = bakeLight->getBakeSize();

			CZone* zone = bakeLight->getGameObject()->getZone();

			// bake camera
			m_cameraObj = zone->createEmptyObject();
			m_cameraObj->addComponent<CCamera>();

			// all lights
			m_lights.clear();
			std::vector<CLight*> lights = zone->getComponentsInChild<CLight>(false);
			for (CLight* l : lights)
			{
				if (!l->isEnable() || !l->getGameObject()->isVisible())
					continue;

				if (l->getRenderLightType() == CLight::Baked || l->getRenderLightType() == CLight::Mixed)
					m_lights.push_back(l);
			}

			// all meshs, that will bake
			std::vector<CRenderMesh*> renderMeshs;
			if (m_bakeAll)
				renderMeshs = zone->getComponentsInChild<CRenderMesh>(false);
			else
			{
				CGameObject* obj = bakeLight->getGameObject();

				CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
				if (container)
					renderMeshs = container->getComponentsInChild<CRenderMesh>(true);
				else
				{
					CRenderMesh* renderMesh = obj->getComponent<CRenderMesh>();
					if (renderMesh)
						renderMeshs.push_back(renderMesh);
				}
			}

			for (CRenderMesh* renderMesh : renderMeshs)
			{
				if (renderMesh->getGameObject()->isStatic() == true)
				{
					// only list static object
					std::vector<CRenderMeshData*>& renderers = renderMesh->getRenderers();
					for (CRenderMeshData* r : renderers)
					{
						if (r->isSkinnedMesh() == false)
						{
							core::matrix4 transform;
							CWorldTransformData* worldTransform = GET_ENTITY_DATA(r->Entity, CWorldTransformData);
							if (worldTransform != NULL)
								transform = worldTransform->World;

							CMesh* mesh = r->getMesh();

							u32 bufferCount = mesh->getMeshBufferCount();
							for (u32 i = 0; i < bufferCount; i++)
							{
								IMeshBuffer* mb = mesh->getMeshBuffer(i);
								ITexture* normalMap = NULL;

								if (mesh->Materials[i] != NULL)
									normalMap = mesh->Materials[i]->getTexture(1);

								if (mb->getVertexBufferCount() > 0)
								{
									// add mesh buffer, that will bake lighting
									m_meshBuffers.push_back(mb);
									m_meshTransforms.push_back(transform);
									m_normalMaps.push_back(normalMap);
								}
							}
						}
					}
				}
			}

			// progress
			m_position = 0;
			m_total = (int)m_meshBuffers.size();

			// need refresh all lighting
			CShadowRTTManager::getInstance()->clearTextures();

			// direction light
			m_shadowRP = new CShadowMapBakeRP();
			m_shadowRP->initRender(2048, 2048);

			m_bakeLightRP = new CDirectionalLightBakeRP();
			m_bakeLightRP->initRender(m_bakeSize, m_bakeSize);
			m_shadowRP->setNextPipeLine(m_bakeLightRP);

			// point light
			m_shadowPLRP = new CPointLightShadowBakeRP();
			m_shadowPLRP->initRender(1024, 1024);

			m_bakePointLightRP = new CPointLightBakeRP();
			m_bakePointLightRP->initRender(m_bakeSize, m_bakeSize);
			m_shadowPLRP->setNextPipeLine(m_bakePointLightRP);

			// area light
			m_shadowALRP = new CShadowMapBakeRP();
			m_shadowALRP->setShadowMapSize(1024);
			m_shadowALRP->initRender(m_bakeSize, m_bakeSize);

			m_bakeAreaLightRP = new CAreaLightBakeRP();
			m_bakeAreaLightRP->initRender(m_bakeSize, m_bakeSize);
			m_shadowALRP->setNextPipeLine(m_bakeAreaLightRP);

			m_state = Init;
		}
	}
}

#endif