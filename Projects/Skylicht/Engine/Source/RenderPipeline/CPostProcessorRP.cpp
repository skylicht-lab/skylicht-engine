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

#include "pch.h"
#include "CPostProcessorRP.h"
#include "Material/Shader/CShaderManager.h"
#include "Material/Shader/CShaderParams.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"

namespace Skylicht
{
	CPostProcessorRP::CPostProcessorRP() :
		m_adaptLum(NULL),
		m_lumTarget(0),
		m_autoExposure(false),
		m_exposure(1.0f),
		m_bloomEffect(true),
		m_fxaa(false),
		m_screenSpaceReflection(false),
		m_brightFilter(NULL),
		m_blurFilter(NULL),
		m_blurUpFilter(NULL),
		m_bloomFilter(NULL),
		m_fxaaFilter(NULL),
		m_numTarget(0),
		m_bloomThreshold(0.9f),
		m_bloomIntensity(1.0f),
		m_lastFrameBuffer(NULL)
	{
		m_luminance[0] = NULL;
		m_luminance[1] = NULL;

		for (int i = 0; i < 10; i++)
			m_rtt[i] = NULL;
	}

	CPostProcessorRP::~CPostProcessorRP()
	{
		IVideoDriver* driver = getVideoDriver();

		if (m_luminance[0] != NULL)
			driver->removeTexture(m_luminance[0]);

		if (m_luminance[1] != NULL)
			driver->removeTexture(m_luminance[1]);

		if (m_adaptLum != NULL)
			driver->removeTexture(m_adaptLum);

		releaseMainRTT();

		if (m_brightFilter != NULL)
			delete m_brightFilter;

		if (m_blurFilter != NULL)
			delete m_blurFilter;

		if (m_blurUpFilter != NULL)
			delete m_blurUpFilter;

		if (m_bloomFilter != NULL)
			delete m_bloomFilter;

		if (m_fxaaFilter != NULL)
			delete m_fxaaFilter;
	}

	void CPostProcessorRP::initMainRTT(int w, int h)
	{
		IVideoDriver* driver = getVideoDriver();

		m_size = core::dimension2du((u32)w, (u32)h);

		if (m_bloomEffect == true || m_fxaa == true)
		{
			m_rtt[0] = driver->addRenderTargetTexture(m_size, "rtt_0", ECF_A16B16G16R16F);
			m_rtt[1] = driver->addRenderTargetTexture(m_size, "rtt_1", ECF_A16B16G16R16F);

			if (m_bloomEffect == true)
			{
				m_numTarget = 2;
				core::dimension2du s = m_size;
				do
				{
					s = s / 2;

					// round for 4
					s.Width = (s.Width / 4) * 4;
					s.Height = (s.Height / 4) * 4;

					m_rtt[m_numTarget++] = driver->addRenderTargetTexture(s, "rtt", ECF_A16B16G16R16F);
				} while (s.Height > 512 && m_numTarget < 8);
			}
		}

		if (m_screenSpaceReflection)
		{
			m_lastFrameBuffer = driver->addRenderTargetTexture(m_size, "last_frame", ECF_A8R8G8B8);

			driver->setRenderTarget(m_lastFrameBuffer, true, false);
			driver->setRenderTarget(NULL);
		}
		else
			m_lastFrameBuffer = NULL;
	}

	void CPostProcessorRP::releaseMainRTT()
	{
		IVideoDriver* driver = getVideoDriver();

		for (int i = 0; i < 8; i++)
		{
			if (m_rtt[i] != NULL)
				driver->removeTexture(m_rtt[i]);
		}

		if (m_lastFrameBuffer != NULL)
			driver->removeTexture(m_lastFrameBuffer);
	}

	void CPostProcessorRP::initRender(int w, int h)
	{
		IVideoDriver* driver = getVideoDriver();
		CShaderManager* shaderMgr = CShaderManager::getInstance();

		// init size of framebuffer
		m_lumSize = core::dimension2du(1024, 1024);

		if (m_autoExposure == true)
		{
			m_luminance[0] = driver->addRenderTargetTexture(m_lumSize, "lum_0", ECF_R16F);
			m_luminance[1] = driver->addRenderTargetTexture(m_lumSize, "lum_1", ECF_R16F);
			m_adaptLum = driver->addRenderTargetTexture(m_lumSize, "lum_adapt", ECF_R16F);
		}

		// framebuffer for glow/fxaa
		initMainRTT(w, h);

		// init final pass shader
		m_finalPass.MaterialType = shaderMgr->getShaderIDByName("PostEffect");
		m_linearPass.MaterialType = shaderMgr->getShaderIDByName("TextureLinearRGB");
		m_finalManualExposurePass.MaterialType = shaderMgr->getShaderIDByName("PostEffectManualExposure");

		// init lum pass shader
		m_lumPass.MaterialType = shaderMgr->getShaderIDByName("Luminance");
		m_adaptLumPass.MaterialType = shaderMgr->getShaderIDByName("AdaptLuminance");

		m_brightFilter = new CMaterial("BrightFilter", "BuiltIn/Shader/PostProcessing/BrightFilter.xml");
		m_blurFilter = new CMaterial("DownBlurFilter", "BuiltIn/Shader/PostProcessing/DownsampleFilter.xml");
		m_blurUpFilter = new CMaterial("DownBlurFilter", "BuiltIn/Shader/PostProcessing/BlurFilter.xml");
		m_bloomFilter = new CMaterial("BloomFilter", "BuiltIn/Shader/PostProcessing/Bloom.xml");
		m_fxaaFilter = new CMaterial("BloomFilter", "BuiltIn/Shader/PostProcessing/FXAA.xml");
	}

	void CPostProcessorRP::resize(int w, int h)
	{
		releaseMainRTT();

		initMainRTT(w, h);
	}

	void CPostProcessorRP::render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId, IRenderPipeline* lastRP)
	{
		if (camera == NULL)
			return;

		onNext(target, camera, entityManager, viewport, cubeFaceId);
	}

	void CPostProcessorRP::luminanceMapGeneration(ITexture* color)
	{
		float w = (float)m_lumSize.Width;
		float h = (float)m_lumSize.Height;

		// Step 1: Generate target luminance from color to lum[0]
		IVideoDriver* driver = getVideoDriver();
		driver->setRenderTarget(m_adaptLum, true, true);

		m_lumPass.setTexture(0, color);

		beginRender2D(w, h);
		renderBufferToTarget(0.0f, 0.0f, w, h, m_lumPass);

		// Step 2: Interpolate to target luminance
		driver->setRenderTarget(m_luminance[m_lumTarget], true, true);

		m_adaptLumPass.setTexture(0, m_adaptLum);
		m_adaptLumPass.setTexture(1, m_luminance[!m_lumTarget]);

		beginRender2D(w, h);
		renderBufferToTarget(0.0f, 0.0f, w, h, m_adaptLumPass);
	}

	void CPostProcessorRP::brightFilter(ITexture* from, ITexture* to, ITexture* emission)
	{
		video::SVec4 curve;

		float threshold = m_bloomThreshold;
		float softKnee = 0.5f;
		curve.W = threshold;

		float knee = threshold * softKnee + 1e-5f;
		curve.X = threshold - knee;
		curve.Y = knee * 2.0f;
		curve.Z = 0.25f / knee;

		m_brightFilter->setUniform4("uCurve", &curve.X);
		m_brightFilter->setTexture(1, emission);

		renderEffect(from, to, m_brightFilter);
	}

	void CPostProcessorRP::blurDown(int from, int to)
	{
		core::dimension2du rrtSize = m_rtt[from]->getSize();
		core::vector2df blurSize;
		blurSize.X = 1.0f / (float)rrtSize.Width;
		blurSize.Y = 1.0f / (float)rrtSize.Height;
		m_blurFilter->setUniform2("uTexelSize", &blurSize.X);
		renderEffect(from, to, m_blurFilter);
	}

	void CPostProcessorRP::blurUp(int from, int to)
	{
		core::dimension2du rrtSize = m_rtt[from]->getSize();
		core::vector2df blurSize;
		blurSize.X = 1.0f / (float)rrtSize.Width;
		blurSize.Y = 1.0f / (float)rrtSize.Height;
		m_blurUpFilter->setUniform2("uTexelSize", &blurSize.X);
		renderEffect(from, to, m_blurUpFilter);
	}

	void CPostProcessorRP::postProcessing(ITexture* finalTarget, ITexture* color, ITexture* emission, ITexture* normal, ITexture* position, const core::recti& viewport, int cubeFaceId)
	{
		IVideoDriver* driver = getVideoDriver();

		float renderW = (float)m_size.Width;
		float renderH = (float)m_size.Height;

		if (viewport.getWidth() > 0 && viewport.getHeight() > 0)
		{
			renderW = (float)viewport.getWidth();
			renderH = (float)viewport.getHeight();
		}

		int colorID = -1;
		ITexture* colorBuffer = color;

		// BLOOM
		if (m_bloomEffect)
		{
			brightFilter(color, m_rtt[1], emission);
			blurDown(1, 2);

			for (int i = 2; i < m_numTarget - 1; i++)
				blurDown(i, i + 1);

			for (int i = m_numTarget - 1; i > 2; i--)
				blurUp(i, i - 1);

			// bloom
			m_bloomFilter->setTexture(0, color);
			m_bloomFilter->setTexture(1, m_rtt[2]);
			m_bloomFilter->applyMaterial(m_effectPass);

			m_bloomFilter->setUniform("uBloomIntensity", m_bloomIntensity);

			CShaderMaterial::setMaterial(m_bloomFilter);

			colorID = 0;
			driver->setRenderTarget(m_rtt[colorID], false, false);

			beginRender2D(renderW, renderH);
			renderBufferToTarget(0.0f, 0.0f, renderW, renderH, m_effectPass);
		}

		if (colorID >= 0)
			colorBuffer = m_rtt[colorID];
		// END BLOOM

		// SCREEN SPACE REFLECTION (save buffer for deferred rendering)
		if (m_screenSpaceReflection == true && m_lastFrameBuffer)
		{
			driver->setRenderTarget(m_lastFrameBuffer, true, false);

			m_linearPass.setTexture(0, colorBuffer);
			beginRender2D(renderW, renderH);
			renderBufferToTarget(0.0f, 0.0f, renderW, renderH, m_linearPass);

			setTarget(finalTarget, cubeFaceId);

			if (viewport.getWidth() > 0 && viewport.getHeight() > 0)
				driver->setViewPort(viewport);

			m_lastFrameBuffer->regenerateMipMapLevels();
		}
		// END SCREEN SPACE REFLECTION


		// AUTO EXPOSURE
		if (m_autoExposure == true)
			luminanceMapGeneration(colorBuffer);

		bool disableFXAA = false;

#ifdef USE_ANGLE_GLES
		disableFXAA = true;
#endif

		if (m_fxaa && !disableFXAA)
		{
			colorID = !colorID;
			driver->setRenderTarget(m_rtt[colorID]);
		}
		else
		{
			setTarget(finalTarget, cubeFaceId);

			if (viewport.getWidth() > 0 && viewport.getHeight() > 0)
				driver->setViewPort(viewport);
		}

		if (m_autoExposure == true)
		{
			m_luminance[m_lumTarget]->regenerateMipMapLevels();

			m_finalPass.setTexture(0, colorBuffer);
			m_finalPass.setTexture(1, m_luminance[m_lumTarget]);
			beginRender2D(renderW, renderH);
			renderBufferToTarget(0.0f, 0.0f, renderW, renderH, m_finalPass);
		}
		else
		{
			m_finalManualExposurePass.setTexture(0, colorBuffer);
			CShaderManager::getInstance()->ShaderVec2[0].set(m_exposure, m_exposure);
			beginRender2D(renderW, renderH);
			renderBufferToTarget(0.0f, 0.0f, renderW, renderH, m_finalManualExposurePass);
		}

		m_lumTarget = !m_lumTarget;
		// END AUTO EXPOSURE

		// BEGIN FXAA
		if (m_fxaa && !disableFXAA)
		{
			if (colorID >= 0)
				colorBuffer = m_rtt[colorID];

			core::dimension2du rrtSize = colorBuffer->getSize();
			float params[2];
			params[0] = 1.0f / (float)rrtSize.Width;
			params[1] = 1.0f / (float)rrtSize.Height;
			m_fxaaFilter->setUniform2("uRCPFrame", params);
			m_fxaaFilter->setTexture(0, colorBuffer);
			m_fxaaFilter->applyMaterial(m_effectPass);

			CShaderMaterial::setMaterial(m_fxaaFilter);

			setTarget(finalTarget, cubeFaceId);

			if (viewport.getWidth() > 0 && viewport.getHeight() > 0)
				driver->setViewPort(viewport);

			beginRender2D(renderW, renderH);
			renderBufferToTarget(0.0f, 0.0f, renderW, renderH, m_effectPass);
		}
		// END FXAA		

		// test to target
		/*
		ITexture *tex = m_rtt[2];
		SMaterial t;
		t.setTexture(0, tex);
		t.MaterialType = CShaderManager::getInstance()->getShaderIDByName("TextureLinearRGB");

		float w = (float)tex->getSize().Width;
		float h = (float)tex->getSize().Height;

		driver->setRenderTarget(finalTarget, false, false);
		beginRender2D(renderW, renderH);
		renderBufferToTarget(0.0f, 0.0f, renderW, renderH, 0.0f, 0.0f, w, h, t);
		*/
	}

	void CPostProcessorRP::renderEffect(int fromTarget, int toTarget, CMaterial* material)
	{
		renderEffect(m_rtt[fromTarget], m_rtt[toTarget], material);
	}

	void CPostProcessorRP::renderEffect(ITexture* fromTarget, ITexture* toTarget, CMaterial* material)
	{
		IVideoDriver* driver = getVideoDriver();

		driver->setRenderTarget(toTarget);

		material->setTexture(0, fromTarget);
		material->applyMaterial(m_effectPass);

		for (int i = 0; i < 2; i++)
		{
			m_effectPass.TextureLayer[i].TextureWrapU = ETC_CLAMP_TO_BORDER;
			m_effectPass.TextureLayer[i].TextureWrapV = ETC_CLAMP_TO_BORDER;
		}

		CShaderMaterial::setMaterial(material);

		const core::dimension2du& toSize = toTarget->getSize();
		const core::dimension2du& fromSize = fromTarget->getSize();

		beginRender2D((float)toSize.Width, (float)toSize.Height);
		renderBufferToTarget(0, 0, (float)fromSize.Width, (float)fromSize.Height, m_effectPass);
	}
}
