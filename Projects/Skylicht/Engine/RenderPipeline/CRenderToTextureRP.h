/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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

namespace Skylicht
{
	/// @brief This is the object class that performs the draw call onto a texture. It's used in cases where you need to display planar reflections.
	/// @ingroup RP
	/// 
	/// The setCustomCamera method is used to set up the camera for rendering, for example, a reflection camera with a viewing direction symmetrical to the current rendering camera.
	/// 
	/// Here's an example of a shader that needs to get parameters from RenderToTexture.
	/// @code
	/// <shaderConfig name="WaterPlanarReflection" baseShader="SOLID">
	/// <uniforms>
	/// 	<vs>
	/// 		...
	/// 		<uniform name="uRTTMatrix" type="RENDER_TEXTURE_MATRIX" valueIndex="0" value="0" float="16" matrix = "true"/>
	/// 	</vs>
	/// 	<fs>
	/// 		...
	/// 		<uniform name="uTexReflect" type="DEFAULT_VALUE" value="3" float="1" directX="false"/>
	/// 	</fs>
	/// </uniforms>
	/// <resources>
	/// 	<resource name="uTexReflect" type="RTT0"/>
	/// 	...
	/// </resources>
	/// <customUI>
	/// 	...
	/// </customUI>
	///	...
	/// </shaderConfig>
	/// @endcode
	/// 
	/// - In which the **RENDER_TEXTURE_MATRIX uniform type** will return the matrix that transforms 3D world into 2D coordinates already projected onto the texture.
	/// 
	/// - And **resource type RTT0** will help the engine find the source texture that has been Rendered To Texture.
	/// 
	/// You can view the shader details in the file `BuiltIn\Shader\PBR\Forward\PBRLightmapPlanarReflection.xml`.
	/// 
	/// When you initialize a CRenderToTextureRP with id=1, the resource type will be RTT1, and RENDER_TEXTURE_MATRIX valueIndex=1.
	/// 
	/// You can attach it to the main render pipeline and *setEnable* it whenever you need to use RTT.
	/// 
	/// @code
	/// // 1st
	/// m_shadowMapRendering = new CShadowMapRP();
	/// m_shadowMapRendering->initRender(w, h);

	/// m_rttRP = new CRenderToTextureRP(0, video::ECF_A8R8G8B8, core::dimension2du(1024, 1024));
	/// m_rttRP->initRender(w, h);
	/// m_rttRP->setEnable(false);

	/// // 2rd
	/// m_forwardRP = new CForwardRP(!postEffect);
	/// m_forwardRP->enableUpdateEntity(false);

	/// // link rp
	/// m_shadowMapRendering->setNextPipeLine(m_rttRP);
	/// m_rttRP->setNextPipeLine(m_forwardRP);
	/// @endcode
	/// 
	/// @see CShaderRTT
	class SKYLICHT_API CRenderToTextureRP : public CBaseRP
	{
	protected:
		int m_id;
		bool m_enable;
		core::matrix4 m_bias;
		float m_scale;
		core::dimension2du m_size;
		core::dimension2du m_customSize;
		CCamera* m_customCamera;
		ITexture* m_renderTarget;
		IRenderPipeline* m_customPipleline;
		video::ECOLOR_FORMAT m_format;
		bool m_autoGenerateMipmap;

	public:
		CRenderToTextureRP(u32 id, video::ECOLOR_FORMAT format, const core::dimension2du& customSize = core::dimension2du(), float scale = 1.0f);

		virtual ~CRenderToTextureRP();

		virtual void initRender(int w, int h);

		virtual void resize(int w, int h);

		virtual void render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId = -1, IRenderPipeline* lastRP = NULL);

		inline void setCustomCamera(CCamera* camera)
		{
			m_customCamera = camera;
		}

		inline CCamera* getCustomCamera()
		{
			return m_customCamera;
		}

		inline int getId()
		{
			return m_id;
		}

		inline bool isEnable()
		{
			return m_enable;
		}

		inline void setEnable(bool b)
		{
			m_enable = b;
		}

		inline ITexture* getTarget()
		{
			return m_renderTarget;
		}

		inline void setCustomPipleline(IRenderPipeline* pipeline)
		{
			m_customPipleline = pipeline;
		}

		inline void enableAutoGenerateMipmap(bool b)
		{
			m_autoGenerateMipmap = b;
		}

		inline bool isAutoGenerateMipmap()
		{
			return m_autoGenerateMipmap;
		}

		static const float* getMatrix(int id);

	protected:

		void initRTT(int w, int h);

		void releaseRTT();
	};
}