/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CGUIRenderData.h"

namespace Skylicht
{
	IMPLEMENT_DATA_TYPE_INDEX(CGUIRenderData);

	CGUIRenderData::CGUIRenderData() :
		Color(255, 255, 255, 255),
		Material(NULL),
		AffectOpacityInChild(false),
		m_applyParentOpacity(false),
		m_opacity(1.0f)
	{
		ShaderID = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");
	}

	CGUIRenderData::~CGUIRenderData()
	{

	}

	const SColor& CGUIRenderData::getColor()
	{
		m_renderColor = Color;

		if (m_opacity != 1.0f)
		{
			u32 a = m_renderColor.getAlpha();
			a = core::clamp<u32>((u32)(a * m_opacity), 0, 255);
			m_renderColor.setAlpha(a);
		}

		return m_renderColor;
	}
}