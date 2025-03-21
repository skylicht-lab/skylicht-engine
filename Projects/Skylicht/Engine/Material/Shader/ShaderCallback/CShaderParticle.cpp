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
	SVec4 g_viewUp;
	SVec4 g_viewLook;

	SVec4 g_orientationNormal;
	SVec4 g_orientationUp;

	CShaderParticle::CShaderParticle()
	{

	}

	CShaderParticle::~CShaderParticle()
	{

	}

	void CShaderParticle::setViewUp(const core::vector3df& up)
	{
		g_viewUp.X = up.X;
		g_viewUp.Y = up.Y;
		g_viewUp.Z = up.Z;
		g_viewUp.W = 0.0f;
	}

	void CShaderParticle::setViewLook(const core::vector3df& look)
	{
		g_viewLook.X = look.X;
		g_viewLook.Y = look.Y;
		g_viewLook.Z = look.Z;
		g_viewLook.W = 0.0f;
	}

	void CShaderParticle::setOrientationUp(const core::vector3df& up)
	{
		g_orientationUp.X = up.X;
		g_orientationUp.Y = up.Y;
		g_orientationUp.Z = up.Z;
		g_orientationUp.W = 0.0f;
	}

	void CShaderParticle::setOrientationNormal(const core::vector3df& normal)
	{
		g_orientationNormal.X = normal.X;
		g_orientationNormal.Y = normal.Y;
		g_orientationNormal.Z = normal.Z;
		g_orientationNormal.W = 0.0f;
	}

	core::vector3df CShaderParticle::getViewUp()
	{
		return core::vector3df(g_viewUp.X, g_viewUp.Y, g_viewUp.Z);
	}

	core::vector3df CShaderParticle::getViewLook()
	{
		return core::vector3df(g_viewLook.X, g_viewLook.Y, g_viewLook.Z);
	}

	void CShaderParticle::OnSetConstants(CShader* shader, SUniform* uniform, IMaterialRenderer* matRender, bool vertexShader)
	{
		switch (uniform->Type)
		{
		case PARTICLE_VIEW_UP:
		{
			if (vertexShader == true)
				matRender->setShaderVariable(uniform->UniformShaderID, &g_viewUp.X, uniform->SizeOfUniform, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform->UniformShaderID, &g_viewUp.X, uniform->SizeOfUniform, video::EST_PIXEL_SHADER);
		}
		break;
		case PARTICLE_VIEW_LOOK:
		{
			if (vertexShader == true)
				matRender->setShaderVariable(uniform->UniformShaderID, &g_viewLook.X, uniform->SizeOfUniform, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform->UniformShaderID, &g_viewLook.X, uniform->SizeOfUniform, video::EST_PIXEL_SHADER);
		}
		break;
		case PARTICLE_ORIENTATION_UP:
		{
			if (vertexShader == true)
				matRender->setShaderVariable(uniform->UniformShaderID, &g_orientationUp.X, uniform->SizeOfUniform, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform->UniformShaderID, &g_orientationUp.X, uniform->SizeOfUniform, video::EST_PIXEL_SHADER);
		}
		break;
		case PARTICLE_ORIENTATION_NORMAL:
		{
			if (vertexShader == true)
				matRender->setShaderVariable(uniform->UniformShaderID, &g_orientationNormal.X, uniform->SizeOfUniform, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform->UniformShaderID, &g_orientationNormal.X, uniform->SizeOfUniform, video::EST_PIXEL_SHADER);
		}
		break;
		default:
			break;
		}
	}
}