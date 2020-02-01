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
#include "CBaseShaderCallback.h"

namespace Skylicht
{

	CBaseShaderCallback::CBaseShaderCallback() :
		m_materialRenderID(-1),
		m_initCallback(true)
	{
	}

	CBaseShaderCallback::~CBaseShaderCallback()
	{
	}


	void CBaseShaderCallback::offsetUV(core::vector2df& uv, float speedX, float speedY)
	{
		float timeStep = getTimeStep();

		uv.X = uv.X + speedX * timeStep;
		uv.Y = uv.Y + speedY * timeStep;

		uv.X = fmodf(uv.X, 1.0f);
		uv.Y = fmodf(uv.Y, 1.0f);
	}

	bool CBaseShaderCallback::isOpenGLFamily()
	{
		IVideoDriver *driver = getVideoDriver();

		if (driver->getDriverType() == EDT_OPENGL || driver->getDriverType() == EDT_OPENGLES)
			return true;

		return false;
	}

	void CBaseShaderCallback::setColor(IMaterialRenderer *matRender, int colorID, bool vertexConstant, const SColorf& color)
	{
		float constBuffer[] = { color.r, color.g, color.b, 1.0f };

		if (vertexConstant == true)
			matRender->setShaderVariable(colorID, constBuffer, 4, video::EST_VERTEX_SHADER);
		else
			matRender->setShaderVariable(colorID, constBuffer, 4, video::EST_PIXEL_SHADER);
	}

	void CBaseShaderCallback::setDirection(IMaterialRenderer *matRender, int directionID, bool vertexConstant, const core::vector3df& dir, int count, bool worldDirection)
	{
		core::vector3df directionVec;
		float dirVec[4] = { 0 };

		if (worldDirection == true)
		{
			directionVec = dir;
		}
		else
		{
			const core::matrix4& worldInv = getVideoDriver()->getTransform(ETS_WORLD_INVERSE);
			worldInv.rotateVect(directionVec, dir);
		}

		directionVec.normalize();

		dirVec[0] = directionVec.X;
		dirVec[1] = directionVec.Y;
		dirVec[2] = directionVec.Z;
		dirVec[3] = 0.0f;

		if (vertexConstant == true)
			matRender->setShaderVariable(directionID, dirVec, count, video::EST_VERTEX_SHADER);
		else
			matRender->setShaderVariable(directionID, dirVec, count, video::EST_PIXEL_SHADER);
	}

	// setPosition
	void CBaseShaderCallback::setPosition(IMaterialRenderer *matRender, int posID, const core::vector3df& pos, bool vertexConstant)
	{
		video::IVideoDriver* driver = getVideoDriver();
		core::vector3df objPos = pos;

		const core::matrix4& worldInv = getVideoDriver()->getTransform(ETS_WORLD_INVERSE);

		worldInv.transformVect(objPos);

		float uPos[4];
		uPos[0] = objPos.X;
		uPos[1] = objPos.Y;
		uPos[2] = objPos.Z;
		uPos[3] = 1.0;

		if (vertexConstant)
			matRender->setShaderVariable(posID, uPos, 4, video::EST_VERTEX_SHADER);
		else
			matRender->setShaderVariable(posID, uPos, 4, video::EST_PIXEL_SHADER);
	}

	void CBaseShaderCallback::setWorldPosition(IMaterialRenderer *matRender, int posID, const core::vector3df& pos, bool vertexConstant)
	{
		float uPos[4];
		uPos[0] = pos.X;
		uPos[1] = pos.Y;
		uPos[2] = pos.Z;
		uPos[3] = 1.0;

		if (vertexConstant)
			matRender->setShaderVariable(posID, uPos, 4, video::EST_VERTEX_SHADER);
		else
			matRender->setShaderVariable(posID, uPos, 4, video::EST_PIXEL_SHADER);
	}

}