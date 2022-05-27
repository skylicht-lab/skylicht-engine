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

#include "pch.h"
#include "CShaderShadow.h"

#include "Lighting/CDirectionalLight.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"

namespace Skylicht
{
	CShadowMapRP* CShaderShadow::s_shadowMapRP = NULL;

	CShaderShadow::CShaderShadow()
	{

	}

	CShaderShadow::~CShaderShadow()
	{

	}

	void CShaderShadow::OnSetConstants(CShader* shader, SUniform* uniform, IMaterialRenderer* matRender, bool vertexShader)
	{
		switch (uniform->Type)
		{
		case SHADOW_MAP_MATRIX:
		{
			if (s_shadowMapRP != NULL)
			{
				const float* shadowMatrix = s_shadowMapRP->getCSM()->getShadowMatrices();
				if (vertexShader == true)
					matRender->setShaderVariable(uniform->UniformShaderID, shadowMatrix, uniform->SizeOfUniform, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform->UniformShaderID, shadowMatrix, uniform->SizeOfUniform, video::EST_PIXEL_SHADER);
			}
		}
		break;
		case SHADOW_MAP_DISTANCE:
		{
			if (s_shadowMapRP != NULL)
			{
				const float* shadowDistance = s_shadowMapRP->getCSM()->getShadowDistance();
				if (vertexShader == true)
					matRender->setShaderVariable(uniform->UniformShaderID, shadowDistance, uniform->SizeOfUniform, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform->UniformShaderID, shadowDistance, uniform->SizeOfUniform, video::EST_PIXEL_SHADER);
			}
		}
		break;
		default:
			break;
		}
	}
}