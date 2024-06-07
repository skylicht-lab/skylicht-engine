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
#include "CPostProcessorRP.h"

namespace Skylicht
{
	class SKYLICHT_API CDeferredRP : public CBaseRP
	{
	protected:
		ITexture* m_target;

		ITexture* m_albedo;
		ITexture* m_position;
		ITexture* m_normal;
		ITexture* m_data;
		ITexture* m_indirect;

		ITexture* m_lightBuffer;

		core::dimension2du m_size;

		core::array<irr::video::IRenderTarget> m_multiRenderTarget;

		core::matrix4 m_viewMatrix;
		core::matrix4 m_projectionMatrix;

		bool m_isIndirectPass;
		int m_vertexColorShader;
		int m_textureColorShader;
		int m_textureLinearRGBShader;

		int m_lightmapArrayShader;
		int m_lightmapVertexShader;
		int m_lightmapSHShader;
		int m_lightmapColorShader;
		int m_lightmapIndirectTestShader;

		int m_colorInstancing;
		int m_lmInstancingStandard;
		int m_lmInstancingTBN;

		int m_lightDirection;
		int m_lightDirectionSSR;
		int m_lightDirectionBake;

		SMaterial m_pointLightPass;
		SMaterial m_spotLightPass;
		SMaterial m_directionalLightPass;
		SMaterial m_finalPass;

		int m_pointLightShader;
		int m_pointLightShadowShader;

		int m_spotLightShader;

		float m_indirectMultipler;
		float m_directMultipler;
		float m_lightMultipler;

		IPostProcessor* m_postProcessor;

	protected:

		void initDefferredMaterial();
		void initPointLightMaterial();
		void disableFloatTextureFilter(SMaterial& m);

	public:
		CDeferredRP();

		virtual ~CDeferredRP();

		virtual bool canRenderMaterial(CMaterial* material);

		virtual bool canRenderShader(CShader* shader);

		virtual void initRender(int w, int h);

		virtual void resize(int w, int h);

		virtual void render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId = -1, IRenderPipeline* lastRP = NULL);

		virtual void drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh);

		virtual void drawInstancingMeshBuffer(CMesh* mesh, int bufferID, CShader* instancingShader, CEntityManager* entityMgr, int entityID, bool skinnedMesh);

		inline void setPostProcessor(IPostProcessor* pp)
		{
			m_postProcessor = pp;
		}

		inline void setIndirectMultipler(float f)
		{
			m_indirectMultipler = f;
		}

		inline void setDirectMultipler(float f)
		{
			m_directMultipler = f;
		}

		inline void setLightMultipler(float f)
		{
			m_lightMultipler = f;
		}

		static void enableRenderIndirect(bool b);

		static bool isEnableRenderIndirect();

		static void enableTestIndirect(bool b);

	protected:

		void initRTT(int w, int h);

		void releaseRTT();

	};
}