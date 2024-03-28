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
	CDirectionalLight* g_directionalLight = NULL;
	CPointLight* g_pointLight = NULL;
	CSpotLight* g_spotLight = NULL;

	SColorf s_lightAmbient = SColorf(0.4f, 0.4f, 0.4f, 1.0f);


	void CShaderLighting::setDirectionalLight(CDirectionalLight* light)
	{
		g_directionalLight = light;
	}

	CDirectionalLight* CShaderLighting::getDirectionalLight()
	{
		return g_directionalLight;
	}


	void CShaderLighting::setPointLight(CPointLight* light)
	{
		g_pointLight = light;
	}

	CPointLight* CShaderLighting::getPointLight()
	{
		return g_pointLight;
	}


	void CShaderLighting::setSpotLight(CSpotLight* light)
	{
		g_spotLight = light;
	}

	CSpotLight* CShaderLighting::getSpotLight()
	{
		return g_spotLight;
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
			if (g_directionalLight != NULL)
			{
				video::SColorf color = g_directionalLight->getColor();
				color.r = color.r * g_directionalLight->getIntensity();
				color.g = color.g * g_directionalLight->getIntensity();
				color.b = color.b * g_directionalLight->getIntensity();

				shader->setColor(matRender, uniform->UniformShaderID, vertexShader, color, g_directionalLight->getIntensity());
			}
		}
		break;
		case POINT_LIGHT_COLOR:
		{
			if (g_pointLight != NULL)
			{
				video::SColorf color = g_pointLight->getColor();
				color.r = color.r * g_pointLight->getIntensity();
				color.g = color.g * g_pointLight->getIntensity();
				color.b = color.b * g_pointLight->getIntensity();

				shader->setColor(matRender, uniform->UniformShaderID, vertexShader, color, g_pointLight->getIntensity());
			}
		}
		break;
		case LIGHT_DIRECTION:
		{
			if (g_directionalLight != NULL)
			{
				core::vector3df dir = -g_directionalLight->getDirection();
				shader->setDirection(matRender, uniform->UniformShaderID, vertexShader, dir);
			}
		}
		break;
		case WORLD_LIGHT_DIRECTION:
		{
			if (g_directionalLight != NULL)
			{
				core::vector3df dir = -g_directionalLight->getDirection();
				shader->setDirection(matRender, uniform->UniformShaderID, vertexShader, dir, 4, true);
			}
		}
		break;
		case POINT_LIGHT_POSITION:
		{
			if (g_pointLight != NULL)
			{
				core::vector3df position = g_pointLight->getPosition();
				shader->setWorldPosition(matRender, uniform->UniformShaderID, position, vertexShader);
			}
		}
		break;
		case POINT_LIGHT_ATTENUATION:
		{
			if (g_pointLight != NULL)
			{
				float attenuation[4] = { 0 };

				// set attenuation			
				attenuation[0] = 0.0f;
				attenuation[1] = g_pointLight->getAttenuation();
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
			if (g_spotLight != NULL)
			{
				video::SColorf color = g_spotLight->getColor();
				color.r = color.r * g_spotLight->getIntensity();
				color.g = color.g * g_spotLight->getIntensity();
				color.b = color.b * g_spotLight->getIntensity();

				shader->setColor(matRender, uniform->UniformShaderID, vertexShader, color, g_spotLight->getIntensity());
			}
		}
		break;
		case SPOT_LIGHT_DIRECTION:
		{
			if (g_spotLight != NULL)
			{
				core::vector3df dir = -g_spotLight->getDirection();
				shader->setDirection(matRender, uniform->UniformShaderID, vertexShader, dir, 4, true);
			}
		}
		break;
		case SPOT_LIGHT_POSITION:
		{
			if (g_spotLight != NULL)
			{
				core::vector3df position = g_spotLight->getPosition();
				shader->setWorldPosition(matRender, uniform->UniformShaderID, position, vertexShader);
			}
		}
		break;
		case SPOT_LIGHT_ATTENUATION:
		{
			if (g_spotLight != NULL)
			{
				float attenuation[4] = { 0 };

				// set attenuation			
				attenuation[0] = cosf(g_spotLight->getSplotCutoff() * core::DEGTORAD * 0.5f);
				attenuation[1] = cosf(g_spotLight->getSpotInnerCutof() * core::DEGTORAD * 0.5f);
				attenuation[2] = g_spotLight->getAttenuation();
				attenuation[3] = g_spotLight->getSpotExponent();

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