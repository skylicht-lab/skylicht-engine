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
#include "Shadow/CShadowMaps.h"

namespace Skylicht
{
	class SKYLICHT_API CShadowMapRP :
		public CBaseRP,
		public IEventReceiver
	{
	public:
		enum EShadowMapType
		{
			CascadedShadow = 0,
			ShadowMapping
		};

	protected:
		enum ERenderShadowState
		{
			DirectionLight = 0,
			PointLight,
		};

		EShadowMapType m_shadowMapType;

		ERenderShadowState m_renderShadowState;

		ITexture* m_depthTexture;

		float m_shadowFar;
		int m_shadowMapSize;

		int m_numCascade;

		int m_screenWidth;
		int m_screenHeight;

		SMaterial m_writeDepthMaterial;
		core::vector3df m_lightDirection;

		CShadowMaps* m_sm;
		CCascadedShadowMaps* m_csm;
		int m_currentCSM;

		int m_texColorShader;
		int m_skinShader;

		int m_depthWriteShader;
		int m_depthWriteSkinMeshShader;
		int m_distanceWriteShader;
		int m_distanceWriteSkinMeshShader;

		int m_depthWriteStandardSGInstancing;
		int m_depthWriteTBNSGInstancing;

		int m_distanceWriteStandardSGInstancing;
		int m_distanceWriteTBNSGInstancing;

		int m_depthWriteSkinnedInstancing;

		bool m_saveDebug;
	public:
		CShadowMapRP();

		virtual ~CShadowMapRP();

		void release();

		void setShadowCascade(int numCascade, int shadowMapSize = 2048, float farValue = 300.0f);

		void setNoShadowCascade(int shadowMapSize = 2048, float farValue = 50.0f);

		void setShadowMapping(EShadowMapType type);

		virtual void initRender(int w, int h);

		virtual void resize(int w, int h);

		virtual void render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId = -1, IRenderPipeline* lastRP = NULL);

		virtual bool canRenderMaterial(CMaterial* m);

		virtual bool canRenderShader(CShader* s);

		virtual void drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh);

		virtual void drawInstancingMeshBuffer(CMesh* mesh, int bufferID, CShader* instancingShader, CEntityManager* entityMgr, int entityID, bool skinnedMesh);

		virtual bool OnEvent(const SEvent& event);

	public:

		virtual void updateShaderResource(CShader* shader, CEntityManager* entity, int entityID, video::SMaterial& irrMaterial);

		virtual const core::aabbox3df& getFrustumBox();

		inline ITexture* getDepthTexture()
		{
			return m_depthTexture;
		}

		virtual float* getShadowDistance();

		virtual float* getShadowMatrices();

	protected:
		CCascadedShadowMaps* getCSM()
		{
			return m_csm;
		}
	};
}