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
#include "CBaseRP.h"
#include "RenderMesh/CMesh.h"
#include "Material/CMaterial.h"
#include "Material/Shader/CShaderManager.h"
#include "TextureManager/CTextureManager.h"
#include "Material/Shader/ShaderCallback/CShaderCamera.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"
#include "Material/Shader/ShaderCallback/CShaderShadow.h"
#include "Material/Shader/ShaderCallback/CShaderTransformTexture.h"

#include "IndirectLighting/CIndirectLightingData.h"
#include "RenderPipeline/CShadowMapRP.h"


namespace Skylicht
{
	bool CBaseRP::s_bakeMode = false;
	bool CBaseRP::s_bakeLMMode = false;

	u32 CBaseRP::s_bakeBounce = 0;
	u32 CBaseRP::s_maxLight = 16;

	SColor CBaseRP::s_clearColor;

	CBaseRP::CBaseRP() :
		m_next(NULL),
		m_updateEntity(true)
	{
		const core::dimension2du& size = getVideoDriver()->getCurrentRenderTargetSize();
		m_viewport2DW = (float)size.Width;
		m_viewport2DH = (float)size.Height;

		m_drawBuffer = new scene::CMeshBuffer<video::S3DVertex2TCoords>(getVideoDriver()->getVertexDescriptor(video::EVT_2TCOORDS), video::EIT_16BIT);
		m_drawBuffer->setHardwareMappingHint(EHM_STREAM);

		m_verticesImage = m_drawBuffer->getVertexBuffer();
		m_indicesImage = m_drawBuffer->getIndexBuffer();

		// init indices buffer
		m_indicesImage->set_used(6);
		u16* index = (u16*)m_indicesImage->getIndices();
		index[0] = 0;
		index[1] = 1;
		index[2] = 2;
		index[3] = 0;
		index[4] = 2;
		index[5] = 3;

		m_textureColorShaderID = CShaderManager::getInstance()->getShaderIDByName("TextureColor");

		// unbind material
		m_unbindMaterial.ZBuffer = video::ECFN_DISABLED;
		m_unbindMaterial.ZWriteEnable = false;
		m_unbindMaterial.MaterialType = m_textureColorShaderID;
		ITexture* nullTexture = CTextureManager::getInstance()->getNullTexture();
		for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			m_unbindMaterial.setTexture(i, nullTexture);
	}

	CBaseRP::~CBaseRP()
	{
		m_drawBuffer->drop();
		m_verticesImage = NULL;
		m_indicesImage = NULL;
	}

	bool CBaseRP::canRenderMaterial(CMaterial* m)
	{
		// default: we don't render deferred material
		if (m->isDeferred() == true)
			return false;

		return true;
	}

	bool CBaseRP::canRenderShader(CShader* s)
	{
		if (s->isDeferred() == true)
			return false;

		return true;
	}

	void CBaseRP::setCamera(CCamera* camera)
	{
		const SViewFrustum& viewArea = camera->getViewFrustum();
		video::IVideoDriver* driver = getVideoDriver();
		driver->setTransform(video::ETS_PROJECTION, viewArea.getTransform(video::ETS_PROJECTION));
		driver->setTransform(video::ETS_VIEW, viewArea.getTransform(video::ETS_VIEW));

		CShaderCamera::setCamera(camera);
	}

	void CBaseRP::setNextPipeLine(IRenderPipeline* next)
	{
		m_next = next;
	}

	void CBaseRP::onNext(ITexture* target, CCamera* camera, CEntityManager* entity, const core::recti& viewport, int cubeFaceId)
	{
		if (m_next != NULL)
			m_next->render(target, camera, entity, viewport, cubeFaceId, this);
	}

	void CBaseRP::updateTextureResource(CMesh* mesh, int bufferID, CEntityManager* entity, int entityId, bool skinnedMesh)
	{
		IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
		video::SMaterial& irrMaterial = mb->getMaterial();

		// set shader (uniform) material
		if (mesh->Materials.size() > (u32)bufferID)
		{
			CMaterial* material = mesh->Materials[bufferID];
			if (material == NULL)
				return;

			// apply shader & material to irrlicht material
			material->updateTexture(irrMaterial);

			CShader* shader = material->getShader();
			if (shader != NULL)
			{
				if (shader->isSkinning() && !skinnedMesh)
				{
					// fallback to software skinning shader
					CShader* softwareSkinning = shader->getSoftwareSkinningShader();
					if (softwareSkinning)
						irrMaterial.MaterialType = (int)softwareSkinning->getMaterialRenderID();
					else
					{
						char log[512];
						sprintf(log, "[CBaseRP] Shader for software skinning is not loaded: %s", shader->getSoftwareSkinningName().c_str());
						os::Printer::log(log);
					}
				}

				updateShaderResource(shader, entity, entityId, irrMaterial);
			}

			CShaderMaterial::setMaterial(material);
		}
	}

	void CBaseRP::updateShaderResource(CShader* shader, CEntityManager* entity, int entityId, video::SMaterial& irrMaterial)
	{
		for (int i = 0, n = shader->getNumResource(); i < n; i++)
		{
			CShader::SResource* res = shader->getResouceInfo(i);

			if (res->Type == CShader::ReflectionProbe)
			{
				if (entityId >= 0)
				{
					SUniform* uniform = shader->getFSUniform(res->Name.c_str());
					if (uniform != NULL)
					{
						CIndirectLightingData* lightData = GET_ENTITY_DATA(entity->getEntity(entityId), CIndirectLightingData);
						u32 textureID = (u32)uniform->Value[0];

						if (lightData != NULL && lightData->ReflectionTexture != NULL && textureID < MATERIAL_MAX_TEXTURES)
							irrMaterial.setTexture(textureID, lightData->ReflectionTexture);
					}
				}
			}
			else if (res->Type == CShader::ShadowMap)
			{
				CShadowMapRP* shadowRP = CShaderShadow::getShadowMapRP();
				if (shadowRP != NULL)
				{
					SUniform* uniform = shader->getFSUniform(res->Name.c_str());
					if (uniform != NULL)
					{
						u32 textureID = (u32)uniform->Value[0];
						irrMaterial.setTexture(textureID, shadowRP->getDepthTexture());

						// disable mipmap
						irrMaterial.TextureLayer[textureID].BilinearFilter = false;
						irrMaterial.TextureLayer[textureID].TrilinearFilter = false;
						irrMaterial.TextureLayer[textureID].AnisotropicFilter = 0;

						irrMaterial.TextureLayer[textureID].TextureWrapU = video::ETC_CLAMP_TO_BORDER;
						irrMaterial.TextureLayer[textureID].TextureWrapV = video::ETC_CLAMP_TO_BORDER;
						irrMaterial.TextureLayer[textureID].BorderColor.set(0.0f, 0.0f, 0.0f, 0.0f);
					}
				}
			}
			else if (res->Type == CShader::TransformTexture ||
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

		// transparent (disable zwrite)
		if (shader->isOpaque() == false)
		{
			irrMaterial.ZWriteEnable = false;
			irrMaterial.BackfaceCulling = false;
		}
	}

	void CBaseRP::drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh)
	{
		updateTextureResource(mesh, bufferID, entity, entityID, skinnedMesh);

		IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
		IVideoDriver* driver = getVideoDriver();

		video::SMaterial& irrMaterial = mb->getMaterial();

		CShaderManager* shaderMgr = CShaderManager::getInstance();
		shaderMgr->setCurrentMaterial(irrMaterial);
		shaderMgr->setCurrentMeshBuffer(mb);

		// set irrlicht material
		driver->setMaterial(irrMaterial);

		// draw mesh buffer
		driver->drawMeshBuffer(mb);
	}

	void CBaseRP::drawInstancingMeshBuffer(CMesh* mesh, int bufferID, CShader* instancingShader, CEntityManager* entity, int entityID, bool skinnedMesh)
	{
		updateTextureResource(mesh, bufferID, entity, entityID, skinnedMesh);

		IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
		IVideoDriver* driver = getVideoDriver();

		video::SMaterial& irrMaterial = mb->getMaterial();
		irrMaterial.MaterialType = instancingShader->getMaterialRenderID();

		CShaderManager* shaderMgr = CShaderManager::getInstance();
		shaderMgr->setCurrentMaterial(irrMaterial);
		shaderMgr->setCurrentMeshBuffer(mb);

		driver->setMaterial(irrMaterial);
		driver->drawMeshBuffer(mb);
	}

	void CBaseRP::beginRender2D(float w, float h)
	{
		core::matrix4 orthoMatrix;
		orthoMatrix.makeIdentity();
		orthoMatrix.buildProjectionMatrixOrthoLH(w, -h, -1.0f, 1.0f);
		orthoMatrix.setTranslation(core::vector3df(-1, 1, 0));

		IVideoDriver* driver = getVideoDriver();

		driver->setTransform(video::ETS_PROJECTION, orthoMatrix);
		driver->setTransform(video::ETS_VIEW, core::IdentityMatrix);
		driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		m_viewport2DW = w;
		m_viewport2DH = h;
	}

	void CBaseRP::unbindRTT()
	{
		IVideoDriver* driver = getVideoDriver();

		int numVerticesUse = 4;
		m_verticesImage->set_used(numVerticesUse);
		S3DVertex2TCoords* vertices = (S3DVertex2TCoords*)m_verticesImage->getVertices();
		SColor color(255, 255, 255, 255);

		float x = -1.0f;
		float y = -1.0f;
		float w = 1.0f;
		float h = 1.0f;

		// add vertices
		vertices[0] = S3DVertex2TCoords(x, y, 0.0f, 0.0f, 0.0f, 1.0f, color, 0, 0, 1, 1);
		vertices[1] = S3DVertex2TCoords(x + w, y, 0.0f, 0.0f, 0.0f, 1.0f, color, 0, 0, 1, 1);
		vertices[2] = S3DVertex2TCoords(x + w, y + h, 0.0f, 0.0f, 0.0f, 1.0f, color, 0, 0, 1, 1);
		vertices[3] = S3DVertex2TCoords(x, y + h, 0.0f, 0.0f, 0.0f, 1.0f, color, 0, 0, 1, 1);

		// notify change vertex buffer
		m_drawBuffer->setDirty(scene::EBT_VERTEX);

		// draw buffer
		driver->setMaterial(m_unbindMaterial);
		driver->drawMeshBuffer(m_drawBuffer);
	}

	void CBaseRP::renderBufferToTarget(float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, SMaterial& material, bool flipY, bool flipX)
	{
		ITexture* tex = material.getTexture(0);
		if (tex == NULL)
			return;

		IVideoDriver* driver = getVideoDriver();

		int numVerticesUse = 4;

		m_verticesImage->set_used(numVerticesUse);
		S3DVertex2TCoords* vertices = (S3DVertex2TCoords*)m_verticesImage->getVertices();
		SColor color(255, 255, 255, 255);

		float x = dx;
		float y = dy;
		float w = dw;
		float h = dh;

		float texW = (float)tex->getSize().Width;
		float texH = (float)tex->getSize().Height;

		const f32 invW = 1.f / static_cast<f32>(texW);
		const f32 invH = 1.f / static_cast<f32>(texH);

		float tx1 = sx * invW;
		float ty1 = sy * invH;
		float tw1 = tx1 + sw * invW;
		float th1 = ty1 + sh * invH;

		float tx2 = 0.0f;
		float ty2 = 0.0f;
		float tw2 = 1.0f;
		float th2 = 1.0f;

		if (driver->getDriverType() != EDT_DIRECT3D11)
		{
			if (flipY)
			{
				ty1 = 1.0f - ty1;
				th1 = 1.0f - th1;

				ty2 = 1.0f - ty2;
				th2 = 1.0f - th2;
			}

			if (flipX)
			{
				tx1 = 1.0f - tx1;
				tw1 = 1.0f - tw1;

				tx2 = 1.0f - tx2;
				tw2 = 1.0f - tw2;
			}
		}

		// add vertices
		vertices[0] = S3DVertex2TCoords(x, y, 0.0f, 0.0f, 0.0f, 1.0f, color, tx1, ty1, tx2, ty2);
		vertices[1] = S3DVertex2TCoords(x + w, y, 0.0f, 0.0f, 0.0f, 1.0f, color, tw1, ty1, tw2, ty2);
		vertices[2] = S3DVertex2TCoords(x + w, y + h, 0.0f, 0.0f, 0.0f, 1.0f, color, tw1, th1, tw2, th2);
		vertices[3] = S3DVertex2TCoords(x, y + h, 0.0f, 0.0f, 0.0f, 1.0f, color, tx1, th1, tx2, th2);

		// notify change vertex buffer
		m_drawBuffer->setDirty(scene::EBT_VERTEX);

		// no depth test
		material.ZBuffer = video::ECFN_DISABLED;
		material.ZWriteEnable = false;

		// draw buffer
		driver->setMaterial(material);
		driver->drawMeshBuffer(m_drawBuffer);
	}

	void CBaseRP::renderBufferToTarget(float sx, float sy, float sw, float sh, SMaterial& material, bool flipY, bool flipX)
	{
		ITexture* tex = material.getTexture(0);
		if (tex == NULL)
			return;

		IVideoDriver* driver = getVideoDriver();

		int numVerticesUse = 4;

		m_verticesImage->set_used(numVerticesUse);
		S3DVertex2TCoords* vertices = (S3DVertex2TCoords*)m_verticesImage->getVertices();
		SColor color(255, 255, 255, 255);

		float x = 0;
		float y = 0;
		float w = m_viewport2DW;
		float h = m_viewport2DH;

		float texW = (float)tex->getSize().Width;
		float texH = (float)tex->getSize().Height;

		const f32 invW = 1.f / static_cast<f32>(texW);
		const f32 invH = 1.f / static_cast<f32>(texH);

		float tx1 = sx * invW;
		float ty1 = sy * invH;
		float tw1 = tx1 + sw * invW;
		float th1 = ty1 + sh * invH;

		float tx2 = 0.0f;
		float ty2 = 0.0f;
		float tw2 = 1.0f;
		float th2 = 1.0f;

		if (driver->getDriverType() != EDT_DIRECT3D11)
		{
			if (flipY)
			{
				ty1 = 1.0f - ty1;
				th1 = 1.0f - th1;

				ty2 = 1.0f - ty2;
				th2 = 1.0f - th2;
			}

			if (flipX)
			{
				tx1 = 1.0f - tx1;
				tw1 = 1.0f - tw1;

				tx2 = 1.0f - tx2;
				tw2 = 1.0f - tw2;
			}
		}

		// add vertices
		vertices[0] = S3DVertex2TCoords(x, y, 0.0f, 0.0f, 0.0f, 1.0f, color, tx1, ty1, tx2, ty2);
		vertices[1] = S3DVertex2TCoords(x + w, y, 0.0f, 0.0f, 0.0f, 1.0f, color, tw1, ty1, tw2, ty2);
		vertices[2] = S3DVertex2TCoords(x + w, y + h, 0.0f, 0.0f, 0.0f, 1.0f, color, tw1, th1, tw2, th2);
		vertices[3] = S3DVertex2TCoords(x, y + h, 0.0f, 0.0f, 0.0f, 1.0f, color, tx1, th1, tx2, th2);

		// notify change vertex buffer
		m_drawBuffer->setDirty(scene::EBT_VERTEX);

		// no depth test
		material.ZBuffer = video::ECFN_DISABLED;
		material.ZWriteEnable = false;

		// draw buffer
		driver->setMaterial(material);
		driver->drawMeshBuffer(m_drawBuffer);
	}

	void CBaseRP::renderEnvironment(CCamera* camera, CEntityManager* entityMgr, const core::vector3df& position, ITexture* texture[], int* face, int numFace)
	{
		if (texture == NULL)
			return;

		IVideoDriver* driver = getVideoDriver();

		core::matrix4 projection;
		projection.buildProjectionMatrixPerspectiveFovLH(90.0f * core::DEGTORAD, 1.0f, camera->getNearValue(), camera->getFarValue());

		core::vector3df targetDirectX[] = {
			core::vector3df(1.0f, 0.0f, 0.0f),	// right
			core::vector3df(-1.0f, 0.0f, 0.0f),	// left

			core::vector3df(0.0f, 1.0f, 0.0f),	// up
			core::vector3df(0.0f,-1.0f, 0.0f),	// down

			core::vector3df(0.0f, 0.0f, 1.0f),	// front
			core::vector3df(0.0f, 0.0f,-1.0f),	// back
		};

		// todo: FBO OpenGL is invertY
		core::vector3df targetOpenGL[] = {
			core::vector3df(1.0f, 0.0f, 0.0f),	// right
			core::vector3df(-1.0f, 0.0f, 0.0f),	// left

			core::vector3df(0.0f, 1.0f, 0.0f),	// up
			core::vector3df(0.0f,-1.0f, 0.0f),	// down

			core::vector3df(0.0f, 0.0f,-1.0f),	// front
			core::vector3df(0.0f, 0.0f, 1.0f),	// back
		};

		core::vector3df up[] = {
			core::vector3df(0.0f, 1.0f, 0.0f),
			core::vector3df(0.0f, 1.0f, 0.0f),

			core::vector3df(0.0f, 0.0f,-1.0f),
			core::vector3df(0.0f, 0.0f, 1.0f),

			core::vector3df(0.0f, 1.0f, 0.0f),
			core::vector3df(0.0f, 1.0f, 0.0f),
		};

		core::vector3df* target = targetDirectX;

		if (driver->getDriverType() != video::EDT_DIRECT3D11)
			target = targetOpenGL;

		core::matrix4 view;

		// OpenGL have flipY buffer
		// So we fix inverse Y by render to a buffer
		ITexture* tempFBO = NULL;
		E_DRIVER_TYPE driverType = driver->getDriverType();

		SMaterial material;

		if (driverType == EDT_OPENGL || driverType == EDT_OPENGLES)
		{
			video::ECOLOR_FORMAT colorFormat = texture[0]->getColorFormat();
			tempFBO = driver->addRenderTargetTexture(texture[0]->getSize(), "tempFBO", colorFormat);

			material.setTexture(0, tempFBO);
			material.MaterialType = m_textureColorShaderID;
		}

		if (face != NULL)
		{
			for (int i = 0; i < numFace; i++)
			{
				core::dimension2du size = texture[i]->getSize();
				float sizeW = (float)size.Width;
				float sizeH = (float)size.Height;

				int faceID = face[i];
				video::E_CUBEMAP_FACE cubemapFace = (video::E_CUBEMAP_FACE)faceID;

				view.makeIdentity();
				view.buildCameraLookAtMatrixLH(position, position + target[faceID] * 100.0f, up[faceID]);

				camera->setProjectionMatrix(projection);
				camera->setViewMatrix(view, position);

				if (tempFBO)
				{
					// todo: Dont flip TOP & BOTTOM
					drawSceneToTexture(tempFBO, camera, entityMgr, true);

					driver->setRenderTarget(texture[i], true, true);
					beginRender2D(sizeW, sizeH);
					renderBufferToTarget(0, 0, sizeW, sizeH, material, false);
				}
				else
				{
					drawSceneToTexture(texture[i], camera, entityMgr, true);
				}
			}
		}
		else
		{
			for (int i = 0; i < 6; i++)
			{
				core::dimension2du size = texture[i]->getSize();
				float sizeW = (float)size.Width;
				float sizeH = (float)size.Height;

				video::E_CUBEMAP_FACE cubemapFace = (video::E_CUBEMAP_FACE)i;

				view.makeIdentity();
				view.buildCameraLookAtMatrixLH(position, position + target[i] * 100.0f, up[i]);

				camera->setProjectionMatrix(projection);
				camera->setViewMatrix(view, position);

				if (tempFBO)
				{
					// todo: Dont flip TOP & BOTTOM
					drawSceneToTexture(tempFBO, camera, entityMgr, true);

					driver->setRenderTarget(texture[i], true, true);
					beginRender2D(sizeW, sizeH);
					renderBufferToTarget(0, 0, sizeW, sizeH, material, false);
				}
				else
				{
					drawSceneToTexture(texture[i], camera, entityMgr, true);
				}
			}
		}

		driver->setRenderTarget(NULL, false, false);
		if (tempFBO != NULL)
		{
			driver->removeTexture(tempFBO);
		}
	}

	void CBaseRP::renderCubeEnvironment(CCamera* camera, CEntityManager* entityMgr, const core::vector3df& position, ITexture* texture, int* face, int numFace, bool allPipeline)
	{
		if (texture == NULL)
			return;

		IVideoDriver* driver = getVideoDriver();

		core::matrix4 projection;
		projection.buildProjectionMatrixPerspectiveFovLH(90.0f * core::DEGTORAD, 1.0f, camera->getNearValue(), camera->getFarValue());

		core::vector3df targetDirectX[] = {
			core::vector3df(1.0f, 0.0f, 0.0f),	// right
			core::vector3df(-1.0f, 0.0f, 0.0f),	// left

			core::vector3df(0.0f, 1.0f, 0.0f),	// up
			core::vector3df(0.0f,-1.0f, 0.0f),	// down

			core::vector3df(0.0f, 0.0f, 1.0f),	// front
			core::vector3df(0.0f, 0.0f,-1.0f),	// back
		};

		// todo: FBO OpenGL is invertY
		core::vector3df targetOpenGL[] = {
			core::vector3df(1.0f, 0.0f, 0.0f),	// right
			core::vector3df(-1.0f, 0.0f, 0.0f),	// left

			core::vector3df(0.0f, 1.0f, 0.0f),	// up
			core::vector3df(0.0f,-1.0f, 0.0f),	// down

			core::vector3df(0.0f, 0.0f,-1.0f),	// front
			core::vector3df(0.0f, 0.0f, 1.0f),	// back
		};

		core::vector3df up[] = {
			core::vector3df(0.0f, 1.0f, 0.0f),
			core::vector3df(0.0f, 1.0f, 0.0f),

			core::vector3df(0.0f, 0.0f,-1.0f),
			core::vector3df(0.0f, 0.0f, 1.0f),

			core::vector3df(0.0f, 1.0f, 0.0f),
			core::vector3df(0.0f, 1.0f, 0.0f),
		};

		core::vector3df* target = targetDirectX;

		if (driver->getDriverType() != video::EDT_DIRECT3D11)
			target = targetOpenGL;

		core::matrix4 view;

		// OpenGL have flipY buffer
		// So we fix inverse Y by render to a buffer
		ITexture* tempFBO = NULL;
		E_DRIVER_TYPE driverType = driver->getDriverType();

		core::dimension2du size = texture->getSize();
		float sizeW = (float)size.Width;
		float sizeH = (float)size.Height;

		SMaterial material;

		if (driverType == EDT_OPENGL || driverType == EDT_OPENGLES)
		{
			video::ECOLOR_FORMAT colorFormat = texture->getColorFormat();
			tempFBO = driver->addRenderTargetTexture(size, "tempFBO", colorFormat);

			material.setTexture(0, tempFBO);
			material.MaterialType = m_textureColorShaderID;
		}

		if (face != NULL)
		{
			for (int i = 0; i < numFace; i++)
			{
				int faceID = face[i];
				video::E_CUBEMAP_FACE cubemapFace = (video::E_CUBEMAP_FACE)faceID;

				view.makeIdentity();
				view.buildCameraLookAtMatrixLH(position, position + target[faceID] * 100.0f, up[faceID]);

				if (allPipeline)
				{
					// Set transform to camera and pass it for all IRenderPipeline
					camera->setProjectionMatrix(projection);
					camera->setViewMatrix(view, position);
				}
				else
				{
					// Just need quick render in this RP
					driver->setTransform(video::ETS_PROJECTION, projection);
					driver->setTransform(video::ETS_VIEW, view);
				}

				if (tempFBO)
				{
					// todo: Dont flip TOP & BOTTOM
					drawSceneToTexture(tempFBO, camera, entityMgr, allPipeline);

					driver->setRenderTargetCube(texture, cubemapFace, true, true);
					beginRender2D(sizeW, sizeH);
					renderBufferToTarget(0, 0, sizeW, sizeH, material, false);
				}
				else
				{
					drawSceneToCubeTexture(texture, camera, cubemapFace, entityMgr, allPipeline);
				}
			}
		}
		else
		{
			for (int i = 0; i < 6; i++)
			{
				video::E_CUBEMAP_FACE cubemapFace = (video::E_CUBEMAP_FACE)i;

				view.makeIdentity();
				view.buildCameraLookAtMatrixLH(position, position + target[i] * 100.0f, up[i]);

				if (allPipeline)
				{
					// Set transform to camera and pass it for all IRenderPipeline
					camera->setProjectionMatrix(projection);
					camera->setViewMatrix(view, position);
				}
				else
				{
					// Just quick render in this RP
					driver->setTransform(video::ETS_PROJECTION, projection);
					driver->setTransform(video::ETS_VIEW, view);
				}

				if (tempFBO)
				{
					// todo: Dont flip TOP & BOTTOM
					drawSceneToTexture(tempFBO, camera, entityMgr, allPipeline);

					driver->setRenderTargetCube(texture, cubemapFace, true, true);
					beginRender2D(sizeW, sizeH);
					renderBufferToTarget(0, 0, sizeW, sizeH, material, false);
				}
				else
				{
					drawSceneToCubeTexture(texture, camera, cubemapFace, entityMgr, allPipeline);
				}
			}
		}

		driver->setRenderTarget(NULL, false, false);
		if (tempFBO != NULL)
		{
			driver->removeTexture(tempFBO);
		}
	}

	void CBaseRP::saveFBOToFile(ITexture* texture, const char* output)
	{
		video::ECOLOR_FORMAT format = texture->getColorFormat();
		if (format != ECF_R8G8B8 && format != ECF_A8R8G8B8)
			return;

		IVideoDriver* driver = getVideoDriver();
		void* imageData = texture->lock(video::ETLM_READ_ONLY);

		IImage* im = driver->createImageFromData(
			format,
			texture->getSize(),
			imageData);

		if (driver->getDriverType() == video::EDT_DIRECT3D11)
			im->swapBG();

		driver->writeImageToFile(im, output);
		im->drop();

		texture->unlock();
	}

	void CBaseRP::setBakeMode(bool b)
	{
		s_bakeMode = b;
	}

	void CBaseRP::setBakeLightmapMode(bool b)
	{
		s_bakeLMMode = b;
	}

	void CBaseRP::setBakeLightingMapBounce(u32 i)
	{
		s_bakeBounce = i;
	}

	void CBaseRP::setClearColor(const SColor& c)
	{
		s_clearColor = c;
	}

	void CBaseRP::setTarget(ITexture* target, int faceId)
	{
		IVideoDriver* driver = getVideoDriver();

		if (target == NULL)
			driver->setRenderTarget(NULL, false, false);
		else if (target->getTextureType() == video::ETT_TEXTURE_2D)
			driver->setRenderTarget(target, false, false);
		else if (target->getTextureType() == video::ETT_TEXTURE_CUBE)
		{
			if (faceId == -1)
				os::Printer::log("[CBaseRP] CBaseRP::setTarget failed!");
			else
				driver->setRenderTargetCube(target, (video::E_CUBEMAP_FACE)faceId, true, true);
		}
	}

	void CBaseRP::drawSceneToTexture(ITexture* target, CCamera* camera, CEntityManager* entityMgr, bool allPipeline)
	{
		SColor whiteColor(255, 255, 255, 255);
		IVideoDriver* driver = getVideoDriver();
		driver->setRenderTarget(target, true, true, whiteColor);

		if (allPipeline)
			render(target, camera, entityMgr, core::recti());
		else
			entityMgr->render();
	}

	void CBaseRP::drawSceneToCubeTexture(ITexture* target, CCamera* camera, video::E_CUBEMAP_FACE faceID, CEntityManager* entityMgr, bool allPipeline)
	{
		SColor clear(255, 0, 0, 0);
		if (target->getColorFormat() == ECF_R32F ||
			target->getColorFormat() == ECF_R16F ||
			target->getColorFormat() == ECF_R16G16)
		{
			// float buffer
			clear.set(255, 255, 255, 255);
		}

		IVideoDriver* driver = getVideoDriver();
		driver->setRenderTargetCube(target, faceID, true, true, clear);

		if (allPipeline)
			render(target, camera, entityMgr, core::recti(), (int)faceID);
		else
			entityMgr->render();
	}
}