/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "IPostProcessor.h"

#include "Material/CMaterial.h"

namespace Skylicht
{
	class CPostProcessorRP :
		public CBaseRP,
		public IPostProcessor
	{
	protected:
		core::dimension2du m_size;
		core::dimension2du m_lumSize;

		ITexture* m_luminance[2];
		ITexture* m_adaptLum;
		int m_lumTarget;

		ITexture* m_lastFrameBuffer;
		ITexture* m_rtt[10];

		int m_numTarget;

		SMaterial m_finalPass;
		SMaterial m_linearPass;
		SMaterial m_lumPass;
		SMaterial m_adaptLumPass;
		SMaterial m_effectPass;

		float m_bloomThreshold;
		float m_bloomIntensity;

		bool m_autoExposure;
		bool m_bloomEffect;
		bool m_fxaa;
		bool m_screenSpaceReflection;

		CMaterial* m_brightFilter;
		CMaterial* m_blurFilter;
		CMaterial* m_blurUpFilter;
		CMaterial* m_bloomFilter;
		CMaterial* m_fxaaFilter;

	public:
		CPostProcessorRP();

		virtual ~CPostProcessorRP();

		virtual void initRender(int w, int h);

		virtual void resize(int w, int h);

		virtual void render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& vp);

		virtual void postProcessing(ITexture* finalTarget, ITexture* color, ITexture* emission, ITexture* normal, ITexture* position, const core::recti& viewport);

		void luminanceMapGeneration(ITexture* color);

		void brightFilter(ITexture* from, ITexture* to, ITexture* emission);

		void blurDown(int from, int to);

		void blurUp(int from, int to);

		inline void enableAutoExposure(bool b)
		{
			m_autoExposure = b;
		}

		inline void enableBloomEffect(bool b)
		{
			m_bloomEffect = b;
		}

		inline void enableFXAA(bool b)
		{
			m_fxaa = b;
		}

		inline void enableScreenSpaceReflection(bool b)
		{
			m_screenSpaceReflection = b;
		}

		inline void setBloomThreshold(float f)
		{
			m_bloomThreshold = f;
		}

		inline void setBloomIntensity(float f)
		{
			m_bloomIntensity = f;
		}

		virtual ITexture* getLastFrameBuffer()
		{
			return m_lastFrameBuffer;
		}

		virtual bool isEnableScreenSpaceReflection()
		{
			return m_screenSpaceReflection;
		}

	protected:

		void initMainRTT(int w, int h);

		void releaseMainRTT();

		void renderEffect(int fromTarget, int toTarget, CMaterial* material);

		void renderEffect(ITexture* fromTarget, ITexture* toTarget, CMaterial* material);
	};
}
