// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#include "pch.h"
#include "CBaseShaderCallback.h"

namespace Skylicht
{

	CBaseShaderCallback::CBaseShaderCallback()
	{
		m_materialRenderID = -1;
		m_initCallback = true;
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

	// setAmbientLight
	void CBaseShaderCallback::setAmbientLight(IMaterialRenderer *matRender, int ambientID, bool vertexConstant)
	{

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
		video::IVideoDriver* driver = getVideoDriver();

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