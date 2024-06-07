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

#pragma once

#include "RenderPipeline/CBaseRP.h"
#include "Shadow/CBoundShadowMaps.h"

namespace Skylicht
{
	class CDirectionalLightBakeRP : public CBaseRP
	{
	protected:
		IMeshBuffer* m_renderMesh;

		IMeshBuffer** m_renderSubmesh;
		ITexture** m_renderTarget;
		int m_numTarget;
		int m_currentTarget;

		int m_bakeDirectionMaterialID;
	public:
		CDirectionalLightBakeRP();

		virtual ~CDirectionalLightBakeRP();

		inline void setRenderMesh(IMeshBuffer* mb, IMeshBuffer** submesh, ITexture** targets, int numTarget)
		{
			m_renderMesh = mb;
			m_renderSubmesh = submesh;
			m_renderTarget = targets;
			m_numTarget = numTarget;
		}

		virtual void initRender(int w, int h);

		virtual void resize(int w, int h);

		virtual void render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId = -1, IRenderPipeline* lastRP = NULL);

		virtual void drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh);

		virtual void drawInstancingMeshBuffer(CMesh* mesh, int bufferID, int materialRenderID, CEntityManager* entityMgr, int entityID, bool skinnedMesh);

		virtual bool canRenderMaterial(CMaterial* material);

		virtual bool canRenderShader(CShader* shader);

	};
}