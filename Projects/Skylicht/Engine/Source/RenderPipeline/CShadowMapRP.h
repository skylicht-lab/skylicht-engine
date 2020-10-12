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

#include "CBaseRP.h"
#include "Shadow/CCascadedShadowMaps.h"

namespace Skylicht
{
	class CShadowMapRP : 
		public CBaseRP,
		public IEventReceiver
	{
	protected:
		ITexture *m_depthTexture;

		int m_shadowMapSize;
		int m_numCascade;

		SMaterial m_writeDepthMaterial;
		core::vector3df m_lightDirection;

		CCascadedShadowMaps *m_csm;
		int m_currentCSM;

		int m_depthWriteShader;
		int m_cubeDepthWriteShader;

		bool m_saveDebug;
	public:
		CShadowMapRP();

		virtual ~CShadowMapRP();

		virtual void initRender(int w, int h);

		virtual void resize(int w, int h);

		virtual void render(ITexture *target, CCamera *camera, CEntityManager *entityManager, const core::recti& viewport);

		virtual bool canRenderMaterial(CMaterial *m);

		virtual void drawMeshBuffer(CMesh *mesh, int bufferID, CEntityManager* entity, int entityID);

		virtual bool OnEvent(const SEvent& event);

	public:

		const core::aabbox3df& getFrustumBox();

		inline ITexture* getDepthTexture()
		{
			return m_depthTexture;
		}

		inline CCascadedShadowMaps* getCSM()
		{
			return m_csm;
		}

		inline void setLightDirection(const core::vector3df& v)
		{
			m_lightDirection = v;
		}

	};
}