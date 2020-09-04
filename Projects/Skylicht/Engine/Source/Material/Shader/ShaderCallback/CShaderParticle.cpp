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
#include "CShaderParticle.h"

namespace Skylicht
{
	core::vector3df CShaderParticle::s_viewUp;
	core::vector3df CShaderParticle::s_viewLook;

	CShaderParticle::CShaderParticle()
	{

	}

	CShaderParticle::~CShaderParticle()
	{

	}

	void CShaderParticle::setViewUp(const core::vector3df& up)
	{
		s_viewUp = up;
	}

	void CShaderParticle::setViewLook(const core::vector3df& look)
	{
		s_viewLook = look;
	}

	void CShaderParticle::OnSetConstants(CShader *shader, SUniform *uniform, IMaterialRenderer* matRender, bool vertexShader)
	{
		switch (uniform->Type)
		{
		case PARTICLE_VIEW_UP:
		{
			if (vertexShader == true)
				matRender->setShaderVariable(uniform->UniformShaderID, &s_viewUp.X, uniform->SizeOfUniform, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform->UniformShaderID, &s_viewUp.X, uniform->SizeOfUniform, video::EST_PIXEL_SHADER);
		}
		break;
		case PARTICLE_VIEW_LOOK:
		{
			if (vertexShader == true)
				matRender->setShaderVariable(uniform->UniformShaderID, &s_viewLook.X, uniform->SizeOfUniform, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform->UniformShaderID, &s_viewLook.X, uniform->SizeOfUniform, video::EST_PIXEL_SHADER);
		}
		break;
		default:
			break;
		}
	}
}