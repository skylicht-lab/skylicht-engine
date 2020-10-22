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
#include "CSkylichtRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CSkylichtRenderer::CSkylichtRenderer(float w, float h) :
				m_width(w),
				m_height(h)
			{
				m_projection.buildProjectionMatrixOrthoLH((f32)w, -(f32)h, -1.0f, 1.0f);
				m_projection.setTranslation(core::vector3df(-1, 1, 0));
			}

			CSkylichtRenderer::~CSkylichtRenderer()
			{

			}

			void CSkylichtRenderer::resize(float w, float h)
			{
				m_width = w;
				m_height = h;

				m_projection.buildProjectionMatrixOrthoLH((f32)w, -(f32)h, -1.0f, 1.0f);
				m_projection.setTranslation(core::vector3df(-1, 1, 0));
			}

			void CSkylichtRenderer::begin()
			{
				CGraphics2D::getInstance()->beginRenderGUI(m_projection, m_view);
			}

			void CSkylichtRenderer::end()
			{
				CGraphics2D::getInstance()->flush();
			}
		}
	}
}