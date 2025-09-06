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

#include "RenderPipeline/CBaseRP.h"
#include "Lighting/CLight.h"

namespace Skylicht
{
	class CLightBakeRP : public CBaseRP
	{
	protected:
		IMeshBuffer* m_renderMesh;
		ITexture* m_normalMap;

		IMeshBuffer** m_submesh;
		ITexture** m_renderTarget;
		int m_numTarget;
		int m_currentTarget;

	public:
		CLightBakeRP();

		virtual ~CLightBakeRP();

		inline void setRenderMesh(IMeshBuffer* mb, ITexture* normalMap, IMeshBuffer** submesh, ITexture** targets, int numTarget)
		{
			m_renderMesh = mb;
			m_normalMap = normalMap;
			m_submesh = submesh;
			m_renderTarget = targets;
			m_numTarget = numTarget;
		}
	};
}