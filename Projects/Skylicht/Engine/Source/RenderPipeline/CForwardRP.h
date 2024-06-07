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
	class SKYLICHT_API CForwardRP : public CBaseRP
	{
	protected:

		bool m_useLinearRGB;

		ITexture* m_target;
		ITexture* m_emission;

		core::dimension2du m_size;

		IPostProcessor* m_postProcessor;

		SMaterial m_finalPass;

	public:
		CForwardRP(bool linearRGB = true);

		virtual ~CForwardRP();

		virtual void initRender(int w, int h);

		virtual void resize(int w, int h);

		virtual void render(ITexture* target, CCamera* camera, CEntityManager* entityManager, const core::recti& viewport, int cubeFaceId = -1, IRenderPipeline* lastRP = NULL);

		inline void setPostProcessor(IPostProcessor* pp)
		{
			m_postProcessor = pp;
		}

		inline ITexture* getEmissionTexture()
		{
			return m_emission;
		}

	protected:

		void initRTT(int w, int h);

		void releaseRTT();

	};
}