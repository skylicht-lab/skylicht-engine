// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#ifndef _BASESHADER_CALLBACK_H_
#define _BASESHADER_CALLBACK_H_

#include "pch.h"
#include "CShaderManager.h"

namespace Skylicht
{

class CLightObject;

class CBaseShaderCallback: public video::IShaderConstantSetCallBack
{
protected:
	int		m_materialRenderID;
	bool	m_initCallback;

public:
	CBaseShaderCallback();
	virtual ~CBaseShaderCallback();

	// setMaterialRenderID
	inline void setMaterialRenderID(int id)
	{
		m_materialRenderID = id;
	}

	// getMaterialRenderID
	inline int getMaterialRenderID()
	{
		return m_materialRenderID;
	}

	void offsetUV(core::vector2df& uv, float speedX, float speedY);

	bool isOpenGLFamily();

	// setAmbientLight
	void setAmbientLight(IMaterialRenderer *matRender, int ambientID, bool vertexConstant);

	// setDirection
	void setDirection(IMaterialRenderer *matRender, int directionID, bool vertexConstant, const core::vector3df& dir, int count = 4, bool worldDirection = false);

	// setPosition
	void setPosition(IMaterialRenderer *matRender, int posID, const core::vector3df& pos, bool vertexConstant = true);
	void setWorldPosition(IMaterialRenderer *matRender, int posID, const core::vector3df& pos, bool vertexConstant = true);
};

}

#endif