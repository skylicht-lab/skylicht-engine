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
#include "Lighting/CAreaLight.h"

namespace Skylicht
{
	CDirectionalLight* g_directionalLight = NULL;
	CDirectionalLight* g_mainShadowLight = NULL;

	CPointLight* g_pointLight[4] = { NULL, NULL, NULL, NULL };
	CSpotLight* g_spotLight[4] = { NULL, NULL, NULL, NULL };
	CAreaLight* g_areaLight[4] = { NULL, NULL, NULL, NULL };

	SColorf s_lightAmbient = SColorf(0.4f, 0.4f, 0.4f, 1.0f);

	void CShaderLighting::setMainShadowLight(CDirectionalLight* light)
	{
		g_mainShadowLight = light;
	}

	CDirectionalLight* CShaderLighting::getMainShadowLight()
	{
		return g_mainShadowLight;
	}

	void CShaderLighting::setDirectionalLight(CDirectionalLight* light)
	{
		g_directionalLight = light;
	}

	CDirectionalLight* CShaderLighting::getDirectionalLight()
	{
		return g_directionalLight;
	}


	void CShaderLighting::setPointLight(CPointLight* light, int lightId)
	{
		g_pointLight[lightId] = light;
	}

	CPointLight* CShaderLighting::getPointLight(int lightId)
	{
		return g_pointLight[lightId];
	}


	void CShaderLighting::setSpotLight(CSpotLight* light, int lightId)
	{
		g_spotLight[lightId] = light;
	}

	CSpotLight* CShaderLighting::getSpotLight(int lightId)
	{
		return g_spotLight[lightId];
	}

	void CShaderLighting::setAreaLight(CAreaLight* light, int lightId)
	{
		g_areaLight[lightId] = light;
	}

	CAreaLight* CShaderLighting::getAreaLight(int lightId)
	{
		return g_areaLight[lightId];
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
			video::SColorf color;
			float intensity = 0.0f;

			if (g_directionalLight != NULL)
			{
				color = g_directionalLight->getColor();
				color.r = color.r * g_directionalLight->getIntensity();
				color.g = color.g * g_directionalLight->getIntensity();
				color.b = color.b * g_directionalLight->getIntensity();
				intensity = g_directionalLight->getIntensity();
			}

			shader->setColor(matRender, uniform->UniformShaderID, vertexShader, color, intensity);
		}
		break;
		case POINT_LIGHT_COLOR:
		{
			video::SColorf color;
			float intensity = 0.0f;

			int lightId = core::clamp(uniform->ValueIndex, 0, 3);
			if (g_pointLight[lightId] != NULL)
			{
				color = g_pointLight[lightId]->getColor();
				intensity = g_pointLight[lightId]->getIntensity();
			}

			shader->setColor(matRender, uniform->UniformShaderID, vertexShader, color, intensity);
		}
		break;
		case WORLD_LIGHT_DIRECTION:
		{
			core::vector3df dir(0.0f, 0.1, 0.0);

			if (g_directionalLight != NULL)
				dir = -g_directionalLight->getDirection();

			shader->setWorldDirection(matRender, uniform->UniformShaderID, vertexShader, dir, 4);
		}
		break;
		case POINT_LIGHT_POSITION:
		{
			core::vector3df position;

			int lightId = core::clamp(uniform->ValueIndex, 0, 3);
			if (g_pointLight[lightId] != NULL)
				position = g_pointLight[lightId]->getPosition();

			shader->setWorldPosition(matRender, uniform->UniformShaderID, position, vertexShader);
		}
		break;
		case POINT_LIGHT_ATTENUATION:
		{
			float attenuation[4] = { 0 };

			int lightId = core::clamp(uniform->ValueIndex, 0, 3);
			if (g_pointLight[lightId] != NULL)
			{
				// set attenuation
				attenuation[0] = 0.0f;
				attenuation[1] = g_pointLight[lightId]->getAttenuation();
				attenuation[2] = 0.0f;
			}

			// shader variable
			if (vertexShader == true)
				matRender->setShaderVariable(uniform->UniformShaderID, attenuation, 4, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform->UniformShaderID, attenuation, 4, video::EST_PIXEL_SHADER);
		}
		break;
		case SPOT_LIGHT_COLOR:
		{
			video::SColorf color;
			float intensity = 0.0f;

			int lightId = core::clamp(uniform->ValueIndex, 0, 3);
			if (g_spotLight[lightId] != NULL)
			{
				color = g_spotLight[lightId]->getColor();
				intensity = g_spotLight[lightId]->getIntensity();
			}

			shader->setColor(matRender, uniform->UniformShaderID, vertexShader, color, intensity);
		}
		break;
		case SPOT_LIGHT_DIRECTION:
		{
			core::vector3df dir(0.0f, 1.0f, 0.0f);

			int lightId = core::clamp(uniform->ValueIndex, 0, 3);
			if (g_spotLight[lightId] != NULL)
				dir = -g_spotLight[lightId]->getDirection();

			shader->setWorldDirection(matRender, uniform->UniformShaderID, vertexShader, dir, 4);
		}
		break;
		case SPOT_LIGHT_POSITION:
		{
			int lightId = core::clamp(uniform->ValueIndex, 0, 3);
			if (g_spotLight[lightId] != NULL)
			{
				core::vector3df position = g_spotLight[lightId]->getPosition();
				shader->setWorldPosition(matRender, uniform->UniformShaderID, position, vertexShader);
			}
		}
		break;
		case SPOT_LIGHT_ATTENUATION:
		{
			float attenuation[4] = { 0 };

			int lightId = core::clamp(uniform->ValueIndex, 0, 3);
			if (g_spotLight[lightId] != NULL)
			{
				// set attenuation
				attenuation[0] = cosf(g_spotLight[lightId]->getSplotCutoff() * core::DEGTORAD * 0.5f);
				attenuation[1] = cosf(g_spotLight[lightId]->getSpotInnerCutof() * core::DEGTORAD * 0.5f);
				attenuation[2] = g_spotLight[lightId]->getAttenuation();
				attenuation[3] = g_spotLight[lightId]->getSpotExponent();
			}

			// shader variable
			if (vertexShader == true)
				matRender->setShaderVariable(uniform->UniformShaderID, attenuation, 4, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform->UniformShaderID, attenuation, 4, video::EST_PIXEL_SHADER);
		}
		break;
		case AREA_LIGHT_POSITION:
		{
			core::vector3df position;

			int lightId = core::clamp(uniform->ValueIndex, 0, 3);
			if (g_areaLight[lightId] != NULL)
				position = g_areaLight[lightId]->getPosition();

			shader->setWorldPosition(matRender, uniform->UniformShaderID, position, vertexShader);
		}
		break;
		case AREA_LIGHT_DIR_X:
		{
			core::vector3df dir(1.0f, 0.0f, 0.0f);

			int lightId = core::clamp(uniform->ValueIndex, 0, 3);
			if (g_areaLight[lightId] != NULL)
				g_areaLight[lightId]->getWorldTransform().rotateVect(dir);

			shader->setWorldDirection(matRender, uniform->UniformShaderID, vertexShader, dir, 3);
		}
		break;
		case AREA_LIGHT_DIR_Y:
		{
			core::vector3df dir(0.0f, 1.0f, 0.0f);

			int lightId = core::clamp(uniform->ValueIndex, 0, 3);
			if (g_areaLight[lightId] != NULL)
				g_areaLight[lightId]->getWorldTransform().rotateVect(dir);

			shader->setWorldDirection(matRender, uniform->UniformShaderID, vertexShader, dir, 3);
		}
		break;
		case AREA_LIGHT_SIZE:
		{
			float size[2] = { 0.1f, 0.1f };

			int lightId = core::clamp(uniform->ValueIndex, 0, 3);
			if (g_areaLight[lightId] != NULL)
			{
				CAreaLight* light = g_areaLight[lightId];

				core::vector3df sx(light->getSizeX() * 0.5f, 0.0f, 0.0f);
				core::vector3df sy(0.0f, light->getSizeY() * 0.5f, 0.0f);

				light->getWorldTransform().rotateVect(sx);
				light->getWorldTransform().rotateVect(sy);

				size[0] = sx.getLength();
				size[1] = sy.getLength();
			}

			if (vertexShader == true)
				matRender->setShaderVariable(uniform->UniformShaderID, size, 2, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform->UniformShaderID, size, 2, video::EST_PIXEL_SHADER);
		}
		break;
		case AREA_LIGHT_COLOR:
		{
			video::SColorf color;
			float intensity = 0.0f;

			int lightId = core::clamp(uniform->ValueIndex, 0, 3);
			if (g_areaLight[lightId] != NULL)
			{
				color = g_areaLight[lightId]->getColor();
				intensity = g_areaLight[lightId]->getIntensity();
			}

			shader->setColor(matRender, uniform->UniformShaderID, vertexShader, color, intensity);
		}
		break;
		case AREA_LIGHT_SHADOW_MATRIX:
		{
			int lightId = core::clamp(uniform->ValueIndex, 0, 3);
			if (g_areaLight[lightId] != NULL)
			{
				float* shadowMatrix = g_areaLight[lightId]->getShadowMatrices();

				if (vertexShader == true)
					matRender->setShaderVariable(uniform->UniformShaderID, shadowMatrix, uniform->SizeOfUniform, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform->UniformShaderID, shadowMatrix, uniform->SizeOfUniform, video::EST_PIXEL_SHADER);
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