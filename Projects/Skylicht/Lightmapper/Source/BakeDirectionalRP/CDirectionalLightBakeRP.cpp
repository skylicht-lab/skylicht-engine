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

#include "Lighting/CDirectionalLight.h"
#include "Lighting/CPointLight.h"

#include "EventManager/CEventManager.h"

#include "RenderMesh/CMesh.h"

namespace Skylicht
{
	CDirectionalLightBakeRP::CDirectionalLightBakeRP() :
		m_renderMesh(NULL)
	{

	}

	CDirectionalLightBakeRP::~CDirectionalLightBakeRP()
	{

	}

	void CDirectionalLightBakeRP::initRender(int w, int h)
	{

	}

	void CDirectionalLightBakeRP::resize(int w, int h)
	{

	}

	void CDirectionalLightBakeRP::render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport)
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

		driver->setRenderTarget(target, false, false);

		if (useCustomViewport)
			driver->setViewPort(viewport);

		// render scene (see function drawMeshBuffer)
		entityManager->cullingAndRender();

		// custom viewport
		if (useCustomViewport)
			driver->setViewPort(customViewport);

		// next
		onNext(target, camera, entityManager, viewport);
	}

	void CDirectionalLightBakeRP::drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh)
	{
		// just render the render mesh
		IMeshBuffer* mb = mesh->getMeshBuffer(bufferID);
		if (mb != m_renderMesh)
			return;

		// render mesh with light bake shader

	}

	void CDirectionalLightBakeRP::drawInstancingMeshBuffer(CMesh* mesh, int bufferID, int materialRenderID, CEntityManager* entityMgr, bool skinnedMesh)
	{
		// no render instancing mesh
	}

	bool CDirectionalLightBakeRP::canRenderMaterial(CMaterial* material)
	{
		return true;
	}

	bool CDirectionalLightBakeRP::canRenderShader(CShader* shader)
	{
		return true;
	}
}