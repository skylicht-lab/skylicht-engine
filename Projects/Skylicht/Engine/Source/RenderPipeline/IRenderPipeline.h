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

#pragma once

#include "Camera/CCamera.h"

namespace Skylicht
{
	class CMesh;
	class CMaterial;
	class CShader;
	class CEntityManager;

	class SKYLICHT_API IRenderPipeline
	{
	public:
		enum ERenderPipelineType
		{
			Forwarder,
			Deferred,
			ShadowMap,
			Mix,
		};

	protected:
		ERenderPipelineType m_type;

	public:
		IRenderPipeline() :
			m_type(Forwarder)
		{
		}

		virtual ~IRenderPipeline()
		{

		}

		virtual ERenderPipelineType getType()
		{
			return m_type;
		}

		virtual bool canRenderMaterial(CMaterial* m) = 0;

		virtual bool canRenderShader(CShader* s) = 0;

		virtual void initRender(int w, int h) = 0;

		virtual void resize(int w, int h) = 0;

		virtual void render(ITexture* target, CCamera* camera, CEntityManager* entity, const core::recti& viewport, int cubeFaceId = -1, IRenderPipeline* lastRP = NULL) = 0;

		virtual void setCamera(CCamera* camera) = 0;

		virtual void setNextPipeLine(IRenderPipeline* next) = 0;

		virtual void onNext(ITexture* target, CCamera* camera, CEntityManager* entity, const core::recti& viewport, int cubeFaceId) = 0;

		virtual void drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entityMgr, int entityID, bool skinnedMesh) = 0;

		virtual void drawInstancingMeshBuffer(CMesh* mesh, int bufferID, CShader* instancingShader, CEntityManager* entityMgr, int entityID, bool skinnedMesh) = 0;
	};
}