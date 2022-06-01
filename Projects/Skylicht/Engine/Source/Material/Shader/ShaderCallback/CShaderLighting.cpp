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
#include "Lighting/CPointLight.h"
#include "Lighting/CSpotLight.h"

namespace Skylicht
{
	CDirectionalLight* CShaderLighting::s_directionalLight = NULL;
	CPointLight* CShaderLighting::s_pointLight = NULL;
	CSpotLight* CShaderLighting::s_spotLight = NULL;

	SColorf CShaderLighting::s_lightAmbient = SColorf(0.4f, 0.4f, 0.4f, 1.0f);


	void CShaderLighting::setDirectionalLight(CDirectionalLight* light)
	{
		s_directionalLight = light;
	}

	CDirectionalLight* CShaderLighting::getDirectionalLight()
	{
		return s_directionalLight;
	}


	void CShaderLighting::setPointLight(CPointLight* light)
	{
		s_pointLight = light;
	}

	CPointLight* CShaderLighting::getPointLight()
	{
		return s_pointLight;
	}


	void CShaderLighting::setSpotLight(CSpotLight* light)
	{
		s_spotLight = light;
	}

	CSpotLight* CShaderLighting::getSpotLight()
	{
		return s_spotLight;
	}


	void CShaderLighting::setLightAmbient(const SColorf& c)
	{
		s_lightAmbient = c;
	}


	CShaderLighting::CShaderLighting()
	{

	}

	CShaderLighting::~CShaderLighting()
	{

	}

	void CShaderLighting::OnSetConstants(CShader* shader, SUniform* uniform, IMaterialRenderer* matRender, bool vertexShader)
	{
		switch (uniform->Type)
		{
		case LIGHT_COLOR:
		{
			if (s_directionalLight != NULL)
			{
				video::SColorf color = s_directionalLight->getColor();
				color.r = color.r * s_directionalLight->getIntensity();
				color.g = color.g * s_directionalLight->getIntensity();
				color.b = color.b * s_directionalLight->getIntensity();

				shader->setColor(matRender, uniform->UniformShaderID, vertexShader, color, s_directionalLight->getIntensity());
			}
		}
		break;
		case POINT_LIGHT_COLOR:
		{
			if (s_pointLight != NULL)
			{
				video::SColorf color = s_pointLight->getColor();
				color.r = color.r * s_pointLight->getIntensity();
				color.g = color.g * s_pointLight->getIntensity();
				color.b = color.b * s_pointLight->getIntensity();

				shader->setColor(matRender, uniform->UniformShaderID, vertexShader, color, s_pointLight->getIntensity());
			}
		}
		break;
		case LIGHT_DIRECTION:
		{
			if (s_directionalLight != NULL)
			{
				core::vector3df dir = -s_directionalLight->getDirection();
				shader->setDirection(matRender, uniform->UniformShaderID, vertexShader, dir);
			}
		}
		break;
		case WORLD_LIGHT_DIRECTION:
		{
			if (s_directionalLight != NULL)
			{
				core::vector3df dir = -s_directionalLight->getDirection();
				shader->setDirection(matRender, uniform->UniformShaderID, vertexShader, dir, 4, true);
			}
		}
		break;
		case POINT_LIGHT_POSITION:
		{
			if (s_pointLight != NULL)
			{
				core::vector3df position = s_pointLight->getPosition();
				shader->setWorldPosition(matRender, uniform->UniformShaderID, position, vertexShader);
			}
		}
		break;
		case POINT_LIGHT_ATTENUATION:
		{
			if (s_pointLight != NULL)
			{
				float attenuation[4] = { 0 };

				// set attenuation			
				attenuation[0] = 0.0f;
				attenuation[1] = s_pointLight->getAttenuation();
				attenuation[2] = 0.0f;

				// shader variable
				if (vertexShader == true)
					matRender->setShaderVariable(uniform->UniformShaderID, attenuation, 4, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform->UniformShaderID, attenuation, 4, video::EST_PIXEL_SHADER);
			}
		}
		break;
		case SPOT_LIGHT_COLOR:
		{
			if (s_spotLight != NULL)
			{
				video::SColorf color = s_spotLight->getColor();
				color.r = color.r * s_spotLight->getIntensity();
				color.g = color.g * s_spotLight->getIntensity();
				color.b = color.b * s_spotLight->getIntensity();

				shader->setColor(matRender, uniform->UniformShaderID, vertexShader, color, s_spotLight->getIntensity());
			}
		}
		break;
		case SPOT_LIGHT_DIRECTION:
		{
			if (s_spotLight != NULL)
			{
				core::vector3df dir = -s_spotLight->getDirection();
				shader->setDirection(matRender, uniform->UniformShaderID, vertexShader, dir, 4, true);
			}
		}
		break;
		case SPOT_LIGHT_POSITION:
		{
			if (s_spotLight != NULL)
			{
				core::vector3df position = s_spotLight->getPosition();
				shader->setWorldPosition(matRender, uniform->UniformShaderID, position, vertexShader);
			}
		}
		break;
		case SPOT_LIGHT_ATTENUATION:
		{
			if (s_spotLight != NULL)
			{
				float attenuation[4] = { 0 };

				// set attenuation			
				attenuation[0] = cosf(s_spotLight->getSplotCutoff() * core::DEGTORAD * 0.5f);
				attenuation[1] = cosf(s_spotLight->getSpotInnerCutof() * core::DEGTORAD * 0.5f);
				attenuation[2] = s_spotLight->getAttenuation();
				attenuation[3] = s_spotLight->getSpotExponent();

				// shader variable
				if (vertexShader == true)
					matRender->setShaderVariable(uniform->UniformShaderID, attenuation, 4, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform->UniformShaderID, attenuation, 4, video::EST_PIXEL_SHADER);
			}
		}
		break;
		case LIGHT_AMBIENT:
		{
			shader->setColor(matRender, uniform->UniformShaderID, vertexShader, s_lightAmbient, 1.0f);
		}
		break;
		default:
			break;
		}
	}
}