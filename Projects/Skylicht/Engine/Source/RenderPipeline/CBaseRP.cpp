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

#include "IndirectLighting/CIndirectLightingData.h"

namespace Skylicht
{
	bool CBaseRP::s_bakeMode = false;

	CBaseRP::CBaseRP() :
		m_next(NULL),
		m_updateEntity(true)
	{
		const core::dimension2du &size = getVideoDriver()->getCurrentRenderTargetSize();
		m_viewport2DW = (float)size.Width;
		m_viewport2DH = (float)size.Height;

		m_drawBuffer = new scene::CMeshBuffer<video::S3DVertex2TCoords>(getVideoDriver()->getVertexDescriptor(video::EVT_2TCOORDS), video::EIT_16BIT);
		m_drawBuffer->setHardwareMappingHint(EHM_STREAM);

		m_verticesImage = m_drawBuffer->getVertexBuffer();
		m_indicesImage = m_drawBuffer->getIndexBuffer();

		// init indices buffer
		m_indicesImage->set_used(6);
		u16 *index = (u16*)m_indicesImage->getIndices();
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
		ITexture *nullTexture = CTextureManager::getInstance()->getNullTexture();
		for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			m_unbindMaterial.setTexture(i, nullTexture);
	}

	CBaseRP::~CBaseRP()
	{
		m_drawBuffer->drop();
		m_verticesImage = NULL;
		m_indicesImage = NULL;
	}

	bool CBaseRP::canRenderMaterial(CMaterial *m)
	{
		// default: we don't render deferred material
		if (m->isDeferred() == true)
			return false;

		return true;
	}

	void CBaseRP::setCamera(CCamera *camera)
	{
		const SViewFrustum& viewArea = camera->getViewFrustum();
		video::IVideoDriver* driver = getVideoDriver();
		driver->setTransform(video::ETS_PROJECTION, viewArea.getTransform(video::ETS_PROJECTION));
		driver->setTransform(video::ETS_VIEW, viewArea.getTransform(video::ETS_VIEW));

		CShaderCamera::setCamera(camera);
	}

	void CBaseRP::setNextPipeLine(IRenderPipeline *next)
	{
		m_next = next;
	}

	void CBaseRP::onNext(ITexture *target, CCamera *camera, CEntityManager* entity, const core::recti& viewport)
	{
		if (m_next != NULL)
			m_next->render(target, camera, entity, viewport);
	}

	void CBaseRP::updateTextureResource(CMesh *mesh, int bufferID, CEntityManager* entity, int entityID)
	{
		IMeshBuffer *mb = mesh->getMeshBuffer(bufferID);
		video::SMaterial& irrMaterial = mb->getMaterial();

		// set shader (uniform) material
		if (mesh->Material.size() > (u32)bufferID)
		{
			CMaterial *material = mesh->Material[bufferID];
			if (material == NULL)
				return;

			CShader *shader = material->getShader();
			if (shader != NULL)
			{
				for (int i = 0, n = shader->getNumResource(); i < n; i++)
				{
					CShader::SResource *res = shader->getResouceInfo(i);

					if (res->Type == CShader::ReflectionProbe)
					{
						SUniform *uniform = shader->getFSUniform(res->Name.c_str());
						if (uniform != NULL)
						{
							CIndirectLightingData *lightData = entity->getEntity(entityID)->getData<CIndirectLightingData>();
							u32 textureID = (u32)uniform->Value[0];

							if (lightData != NULL && lightData->ReflectionTexture != NULL && textureID < MATERIAL_MAX_TEXTURES)
								irrMaterial.setTexture(textureID, lightData->ReflectionTexture);
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

			CShaderMaterial::setMaterial(material);
		}
	}

	void CBaseRP::drawMeshBuffer(CMesh *mesh, int bufferID, CEntityManager* entity, int entityID)
	{
		// update texture resource
		updateTextureResource(mesh, bufferID, entity, entityID);

		IMeshBuffer *mb = mesh->getMeshBuffer(bufferID);
		IVideoDriver *driver = getVideoDriver();

		video::SMaterial& irrMaterial = mb->getMaterial();

		// set irrlicht material
		driver->setMaterial(irrMaterial);

		// draw mesh buffer
		driver->drawMeshBuffer(mb);
	}

	void CBaseRP::beginRender2D(float w, float h)
	{
		core::matrix4 orthoMatrix;
		orthoMatrix.makeIdentity();
		orthoMatrix.buildProjectionMatrixOrthoLH(w, -h, -1.0f, 1.0f);
		orthoMatrix.setTranslation(core::vector3df(-1, 1, 0));

		IVideoDriver *driver = getVideoDriver();

		driver->setTransform(video::ETS_PROJECTION, orthoMatrix);
		driver->setTransform(video::ETS_VIEW, core::IdentityMatrix);
		driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		m_viewport2DW = w;
		m_viewport2DH = h;
	}

	void CBaseRP::unbindRTT()
	{
		IVideoDriver *driver = getVideoDriver();

		int numVerticesUse = 4;
		m_verticesImage->set_used(numVerticesUse);
		S3DVertex2TCoords *vertices = (S3DVertex2TCoords*)m_verticesImage->getVertices();
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
		ITexture *tex = material.getTexture(0);
		if (tex == NULL)
			return;

		IVideoDriver *driver = getVideoDriver();

		int numVerticesUse = 4;

		m_verticesImage->set_used(numVerticesUse);
		S3DVertex2TCoords *vertices = (S3DVertex2TCoords*)m_verticesImage->getVertices();
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
		ITexture *tex = material.getTexture(0);
		if (tex == NULL)
			return;

		IVideoDriver *driver = getVideoDriver();

		int numVerticesUse = 4;

		m_verticesImage->set_used(numVerticesUse);
		S3DVertex2TCoords *vertices = (S3DVertex2TCoords*)m_verticesImage->getVertices();
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

	void CBaseRP::renderEnvironment(CCamera *camera, CEntityManager *entityMgr, const core::vector3df& position, ITexture *texture[], int* face, int numFace)
	{
		if (texture == NULL)
			return;

		IVideoDriver *driver = getVideoDriver();

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

		core::vector3df *target = targetDirectX;

		if (driver->getDriverType() != video::EDT_DIRECT3D11)
			target = targetOpenGL;

		core::matrix4 view;

		// OpenGL have flipY buffer
		// So we fix inverse Y by render to a buffer
		ITexture *tempFBO = NULL;
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
				driver->setTransform(video::ETS_PROJECTION, projection);
				driver->setTransform(video::ETS_VIEW, view);

				if (tempFBO)
				{
					// todo: Dont flip TOP & BOTTOM
					drawSceneToTexture(tempFBO, entityMgr);

					driver->setRenderTargetCube(texture[i], cubemapFace, true, true);
					beginRender2D(sizeW, sizeH);
					renderBufferToTarget(0, 0, sizeW, sizeH, material, false);
				}
				else
				{
					drawSceneToCubeTexture(texture[i], cubemapFace, entityMgr);
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
				driver->setTransform(video::ETS_PROJECTION, projection);
				driver->setTransform(video::ETS_VIEW, view);

				if (tempFBO)
				{
					// todo: Dont flip TOP & BOTTOM
					drawSceneToTexture(tempFBO, entityMgr);

					driver->setRenderTarget(texture[i], cubemapFace, true, true);
					beginRender2D(sizeW, sizeH);
					renderBufferToTarget(0, 0, sizeW, sizeH, material, false);
				}
				else
				{
					drawSceneToCubeTexture(texture[i], cubemapFace, entityMgr);
				}
			}
		}

		driver->setRenderTarget(NULL);
		if (tempFBO != NULL)
		{
			driver->removeTexture(tempFBO);
		}
	}

	void CBaseRP::renderCubeEnvironment(CCamera *camera, CEntityManager *entityMgr, const core::vector3df& position, ITexture *texture, int* face, int numFace)
	{
		if (texture == NULL)
			return;

		IVideoDriver *driver = getVideoDriver();

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

		core::vector3df *target = targetDirectX;

		if (driver->getDriverType() != video::EDT_DIRECT3D11)
			target = targetOpenGL;

		core::matrix4 view;

		// OpenGL have flipY buffer
		// So we fix inverse Y by render to a buffer
		ITexture *tempFBO = NULL;
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
				driver->setTransform(video::ETS_PROJECTION, projection);
				driver->setTransform(video::ETS_VIEW, view);

				if (tempFBO)
				{
					// todo: Dont flip TOP & BOTTOM
					drawSceneToTexture(tempFBO, entityMgr);

					driver->setRenderTargetCube(texture, cubemapFace, true, true);
					beginRender2D(sizeW, sizeH);
					renderBufferToTarget(0, 0, sizeW, sizeH, material, false);
				}
				else
				{
					drawSceneToCubeTexture(texture, cubemapFace, entityMgr);
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
				driver->setTransform(video::ETS_PROJECTION, projection);
				driver->setTransform(video::ETS_VIEW, view);

				if (tempFBO)
				{
					// todo: Dont flip TOP & BOTTOM
					drawSceneToTexture(tempFBO, entityMgr);

					driver->setRenderTargetCube(texture, cubemapFace, true, true);
					beginRender2D(sizeW, sizeH);
					renderBufferToTarget(0, 0, sizeW, sizeH, material, false);
				}
				else
				{
					drawSceneToCubeTexture(texture, cubemapFace, entityMgr);
				}
			}
		}

		driver->setRenderTarget(NULL);
		if (tempFBO != NULL)
		{
			driver->removeTexture(tempFBO);
		}
	}

	void CBaseRP::saveFBOToFile(ITexture *texture, const char *output)
	{
		video::ECOLOR_FORMAT format = texture->getColorFormat();
		if (format != ECF_R8G8B8 && format != ECF_A8R8G8B8)
			return;

		IVideoDriver *driver = getVideoDriver();
		void *imageData = texture->lock(video::ETLM_READ_ONLY);

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

	void CBaseRP::drawSceneToTexture(ITexture *target, CEntityManager *entityMgr)
	{
		SColor whiteColor(255, 255, 255, 255);
		IVideoDriver *driver = getVideoDriver();
		driver->setRenderTarget(target, true, true, whiteColor);
		entityMgr->render();
	}

	void CBaseRP::drawSceneToCubeTexture(ITexture *target, video::E_CUBEMAP_FACE faceID, CEntityManager *entityMgr)
	{
		SColor whiteColor(255, 255, 255, 255);
		IVideoDriver *driver = getVideoDriver();
		driver->setRenderTargetCube(target, faceID, true, true, whiteColor);
		entityMgr->render();
	}
}