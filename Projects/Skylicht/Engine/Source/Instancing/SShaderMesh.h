/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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

#include "irrlicht.h"
using namespace irr::scene;

namespace Skylicht
{
	struct SShaderMesh
	{
		CShader* Shader;
		CMesh* Mesh;
		ITexture* Textures[MATERIAL_MAX_TEXTURES];
		ITexture* IndirectLM;
		ITexture* DirectLM;

		SShaderMesh()
		{
			Shader = NULL;
			Mesh = NULL;
			IndirectLM = NULL;
			DirectLM = NULL;
			for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
				Textures[i] = NULL;
		}

		bool operator<(const SShaderMesh& other) const
		{
			if (Shader == other.Shader)
			{
				if (Mesh == other.Mesh)
				{
					if (IndirectLM == other.IndirectLM)
					{
						if (DirectLM == other.DirectLM)
						{
							for (int i = 0; i < _IRR_MATERIAL_MAX_TEXTURES_; i++)
							{
								if (Textures[i] != other.Textures[i])
									return Textures[i] < other.Textures[i];
							}
							// all same
							return false;
						}
						return DirectLM < other.DirectLM;
					}
					return IndirectLM < other.IndirectLM;
				}
				return Mesh < other.Mesh;
			}
			return Shader < other.Shader;
		}
	};
}