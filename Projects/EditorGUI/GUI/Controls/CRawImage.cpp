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
#include "CRawImage.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Renderer/CRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CRawImage::CRawImage(CBase* parent) :
				CBase(parent),
				m_color(CThemeConfig::White),
				m_image(NULL)
			{
				setMouseInputEnabled(false);
			}

			CRawImage::CRawImage(CBase* parent, CGUIImage* image, const SRect& sourceRect) :
				CBase(parent),
				m_color(CThemeConfig::White),
				m_image(image),
				m_sourceRect(sourceRect)
			{
				setMouseInputEnabled(false);
				setSize(sourceRect.Width, sourceRect.Height);
			}

			CRawImage::~CRawImage()
			{

			}

			void CRawImage::render()
			{
				if (m_image != NULL)
				{
					CRenderer::getRenderer()->drawImage(m_image, m_color, m_sourceRect, getRenderBounds());
				}
				else
				{
					CRenderer::getRenderer()->drawFillRect(getRenderBounds(), m_color);
				}
			}
		}
	}
}