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