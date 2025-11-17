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
	/// @brief This object class will perform draw commands using "Deferred rendering"
	/// @ingroup RP
	/// 
	/// You can find more references here: https://learnopengl.com/Advanced-Lighting/Deferred-Shading
	/// 
	/// Only materials with the *deferred=true* property will be drawn in this method.
	/// @code
	/// <shaderConfig name="Color" baseShader="SOLID" deferred="true">
	/// @endcode
	/// So, the render pipeline only draws shaders located in the "Assets/BuiltIn/Shader/SpecularGlossiness/Deferred" folder.
	/// You can still combine drawing by first using Deferred rendering, and then drawing materials with Forward shaders afterward.
	/// 
	/// The advantage of this method is its support for real-time lights (CPointLight, CSpotLight, CDirectionalLight) and lighting. 
	/// However, on smartphone devices like iOS and Android, it causes rapid overheating. 
	/// Therefore, Deferred Rendering should only be used on PC.
	/// 
	/// Note that you need to set up an additional Shadow pass before Deferred Rendering.
	/// 
	/// @code
	/// CScene* scene = new CScene();
	/// CZone* zone = scene->createZone();
	/// ...
	/// // load or setup a scene
	/// ...
	/// ...
	/// // camera
	/// CGameObject* camObj = zone->createEmptyObject();
	/// CCamera* camera = camObj->addComponent<CCamera>();
	/// camera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
	/// camera->lookAt(
	/// 	core::vector3df(0.0f, 0.0f, 0.0f),  // look at target position
	/// 	core::vector3df(0.0f, 1.0f, 0.0f)   // head up vector
	/// );
	/// 
	/// // init render pipeline
	/// CBaseApp* app = getApplication();
	/// u32 w = app->getWidth();
	/// u32 h = app->getHeight();
	/// 
	/// CShadowMapRP* shadowRP = new CShadowMapRP();
	/// shadowRP->initRender(w, h);
	/// 
	/// CDeferredRP* deferredRP = new CDeferredRP();
	/// deferredRP->initRender(w, h);
	/// 
	/// shadowRP->setNextPipeLine(deferredRP);
	/// 
	/// // render function
	/// shadowRP->render(
	/// 	NULL, // render target is screen
	/// 	camera, // the camera
	/// 	scene->getEntityManager(), // all entities in scene
	/// 	core::recti() // the viewport is fullscreen
	/// );
	/// @endcode
	/// 
	/// @see CShadowMapRP, CForwardRP
	class SKYLICHT_API CDeferredRP :
		public CBaseRP,
		public IEventReceiver
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
		int m_lightmapSkinSHShader;
		int m_lightmapColorShader;
		int m_lightmapIndirectTestShader;

		int m_colorInstancing;
		int m_lmInstancingStandardSG;
		int m_lmInstancingTangentSG;

		int m_lightDirection;
		int m_lightDirectionSSR;
		int m_lightDirectionBake;

		SMaterial m_lightPass;
		SMaterial m_directionalLightPass;
		SMaterial m_finalPass;

		int m_pointLightShader;
		int m_pointLightShadowShader;

		int m_spotLightShader;
		int m_spotLightShadowShader;

		int m_areaLightShader;
		int m_areaLightShadowShader;

		float m_indirectMultipler;
		float m_directMultipler;
		float m_lightMultipler;

		IPostProcessor* m_postProcessor;

	protected:

		void initDefferredMaterial();
		void initLightMaterial();

	public:
		CDeferredRP();

		virtual ~CDeferredRP();

		virtual bool OnEvent(const SEvent& event);

		virtual bool canRenderMaterial(CMaterial* material);

		virtual bool canRenderShader(CShader* shader);

		virtual void initRender(int w, int h);

		virtual void resize(int w, int h);

		virtual void render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId = -1, IRenderPipeline* lastRP = NULL);

		virtual void drawMeshBuffer(CMesh* mesh, int bufferID, CEntityManager* entity, int entityID, bool skinnedMesh);

		virtual void drawInstancingMeshBuffer(CMesh* mesh, int bufferID, CShader* instancingShader, CEntityManager* entityMgr, int entityID, bool skinnedMesh);

		void getRenderLightRect(CCamera* camera, const core::aabbox3df& box, float& x, float& y, float& w, float& h, float maxW, float maxH);

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

		static void enableTestBuffer(int bufferId);

	protected:

		void initRTT(int w, int h);

		void releaseRTT();

	};
}