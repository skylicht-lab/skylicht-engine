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
#include "CShaderLighting.h"
#include "Lighting/CDirectionalLight.h"

namespace Skylicht
{
	CDirectionalLight *CShaderLighting::s_directionalLight = NULL;

	void CShaderLighting::setDirectionalLight(CDirectionalLight *light)
	{
		s_directionalLight = light;
	}

	CDirectionalLight* CShaderLighting::getDirectionalLight()
	{
		return s_directionalLight;
	}

	CShaderLighting::CShaderLighting()
	{

	}

	CShaderLighting::~CShaderLighting()
	{

	}

	void CShaderLighting::OnSetConstants(CShader *shader, SUniform *uniform, IMaterialRenderer* matRender, bool vertexShader)
	{
		switch (uniform->Type)
		{
		case LIGHT_COLOR:
		{
			if (s_directionalLight != NULL)
			{
				const video::SColorf& color = s_directionalLight->getColor();
				shader->setColor(matRender, uniform->UniformShaderID, vertexShader, color);
			}
		}
		break;
		case LIGHT_AMBIENT:
		{
		}
		break;
		case LIGHT_DIRECTION:
		{
			if (s_directionalLight != NULL)
			{
				const core::vector3df &dir = s_directionalLight->getDirection();
				shader->setDirection(matRender, uniform->UniformShaderID, vertexShader, dir);
			}
		}
		break;
		case WORLD_LIGHT_DIRECTION:
		{
			if (s_directionalLight != NULL)
			{
				const core::vector3df &dir = s_directionalLight->getDirection();
				shader->setDirection(matRender, uniform->UniformShaderID, vertexShader, dir, 4, true);
			}
		}
		break;
		case POINT_LIGHT_POSITION:
		{
		}
		break;
		case POINT_LIGHT_ATTENUATION:
		{
		}
		break;
		case SPOT_LIGHT_CUTOFF:
		{
		}
		break;
		default:
			break;
		}
	}
}