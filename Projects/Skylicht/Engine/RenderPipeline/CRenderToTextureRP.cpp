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

#include "pch.h"
#include "CRenderToTextureRP.h"

#include "Material/Shader/CShaderManager.h"
#include "Material/Shader/ShaderCallback/CShaderRTT.h"

namespace Skylicht
{
	core::matrix4 g_rttMatrix[video::MATERIAL_MAX_TEXTURES];

	const float* CRenderToTextureRP::getMatrix(int id)
	{
		if (id >= video::MATERIAL_MAX_TEXTURES)
			id = 0;
		return g_rttMatrix[id].pointer();
	}

	CRenderToTextureRP::CRenderToTextureRP(u32 id, video::ECOLOR_FORMAT format, const core::dimension2du& customSize, float scale) :
		m_id(id),
		m_enable(true),
		m_customCamera(NULL),
		m_renderTarget(NULL),
		m_format(format),
		m_scale(scale),
		m_customSize(customSize),
		m_customPipleline(NULL),
		m_autoGenerateMipmap(false)
	{
		if (m_id >= video::MATERIAL_MAX_TEXTURES)
			m_id = 0;

		if (getVideoDriver()->getDriverType() == EDT_DIRECT3D11)
		{
			const float biasDX[16] = {
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.5f, 0.0f,
				0.5f, 0.5f, 0.5f, 1.0f
			};
			m_bias.setM(biasDX);
		}
		else
		{
			const float biasGL[16] = {
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.5f, 0.0f,
				0.5f, 0.5f, 0.5f, 1.0f
			};
			m_bias.setM(biasGL);
		}

		m_updateEntity = false;
	}

	CRenderToTextureRP::~CRenderToTextureRP()
	{
		releaseRTT();
	}

	void CRenderToTextureRP::initRTT(int w, int h)
	{
		if (m_customSize.Width > 0 && m_customSize.Height > 0)
		{
			m_size.set(m_customSize.Width, m_customSize.Height);
		}
		else
		{
			int newW = (int)((float)w * m_scale);
			int newH = (int)((float)h * m_scale);
			m_size.set(newW, newH);
		}

		m_renderTarget = getVideoDriver()->addRenderTargetTexture(m_size, "target", m_format);
		CShaderRTT::setRTTTexture(m_id, m_renderTarget);
	}

	void CRenderToTextureRP::releaseRTT()
	{
		if (m_renderTarget)
		{
			getVideoDriver()->removeTexture(m_renderTarget);
			CShaderRTT::setRTTTexture(m_id, NULL);
		}
	}

	void CRenderToTextureRP::initRender(int w, int h)
	{
		initRTT(w, h);
	}

	void CRenderToTextureRP::resize(int w, int h)
	{
		if (m_customSize.Width == 0 || m_customSize.Height == 0)
		{
			releaseRTT();
			initRTT(w, h);
		}
	}

	void CRenderToTextureRP::render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId, IRenderPipeline* lastRP)
	{
		if (camera == NULL)
			return;

		if (m_enable)
		{
			IVideoDriver* driver = getVideoDriver();
			CCamera* renderCamera = camera;
			if (m_customCamera)
				renderCamera = m_customCamera;

			if (m_customPipleline)
			{
				// maybe: need deferred & postprocess
				m_customPipleline->render(m_renderTarget, renderCamera, entityManager, core::recti());
			}
			else
			{
				// quick render forward pass or custom shader
				driver->setRenderTarget(m_renderTarget, true, true);

				setCamera(renderCamera);
				entityManager->setCamera(renderCamera);
				entityManager->setRenderPipeline(this);

				// update prj matrix
				core::matrix4 mvp = driver->getTransform(video::ETS_PROJECTION) * driver->getTransform(video::ETS_VIEW);
				g_rttMatrix[m_id] = m_bias * mvp;

				if (m_updateEntity == true)
				{
					entityManager->update();
					entityManager->cullingAndRender();
				}
				else
				{
					entityManager->cullingAndRender();
				}
			}

			if (m_autoGenerateMipmap)
				m_renderTarget->regenerateMipMapLevels();

			CShaderRTT::setRTTTexture(m_id, m_renderTarget);
		}

		onNext(target, camera, entityManager, viewport, cubeFaceId);
	}
}