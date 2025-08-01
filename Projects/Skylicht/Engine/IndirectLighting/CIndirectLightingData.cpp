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
#include "CIndirectLightingData.h"

#include "Material/Shader/ShaderCallback/CShaderSH.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"

namespace Skylicht
{
	IMPLEMENT_DATA_TYPE_INDEX(CIndirectLightingData);

	CIndirectLightingData::CIndirectLightingData() :
		Type(LightmapArray),
		IndirectTexture(NULL),
		LightTexture(NULL),
		ReflectionTexture(NULL),
		SH(NULL),
		AutoSH(NULL),
		InvalidateProbe(true),
		InvalidateReflection(true),
		Intensity(NULL),
		ReleaseSH(false)
	{

	}

	CIndirectLightingData::~CIndirectLightingData()
	{
		releaseSH();
	}

	void CIndirectLightingData::initSH()
	{
		Type = CIndirectLightingData::SH9;

		SH = new core::vector3df[9];

		AutoSH = new bool();
		Intensity = new float();

		*AutoSH = true;
		*Intensity = 1.0f;

		ReleaseSH = true;
	}

	void CIndirectLightingData::releaseSH()
	{
		if (ReleaseSH)
		{
			if (SH)
			{
				delete[]SH;
				SH = NULL;
			}
			if (AutoSH)
			{
				delete AutoSH;
				AutoSH = NULL;
			}
			if (Intensity)
			{
				delete Intensity;
				Intensity = NULL;
			}
			ReleaseSH = false;
		}
	}

	void CIndirectLightingData::applyShader()
	{
		if (Type == CIndirectLightingData::SH9)
		{
			if (SH)
			{
				float intensity = Intensity ? *Intensity : 1.0f;
				CShaderSH::setSH9(SH, intensity);
			}
		}
		else if (Type == CIndirectLightingData::AmbientColor)
			CShaderLighting::setLightAmbient(Color);
	}
}