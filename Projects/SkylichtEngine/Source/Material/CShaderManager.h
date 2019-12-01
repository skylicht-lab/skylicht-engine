// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#ifndef _GAMEMATERIAL_H_
#define _GAMEMATERIAL_H_

#include "pch.h"
#include "Utils/CGameSingleton.h"

namespace Skylicht
{

class CShader;
class IShaderCallback;

class CShaderManager: public CGameSingleton<CShaderManager>
{
protected:
	IMeshBuffer				*m_currentMeshBuffer;
	SMaterial				*m_currentMatRendering;

	int						m_currentRenderID;

	std::vector<CShader*>		m_listShader;
	std::map<std::string, int>	m_listShaderID;

	bool					m_haveBakeLMShader;

public:

	// todo
	// use to save uniform that render a non object
	core::vector2df	ShaderVec2[10];
	core::vector3df	ShaderVec3[10];
	video::SVec4	ShaderVec4[10];

public:

	CShaderManager();
	virtual ~CShaderManager();

	void releaseAll();

	inline bool haveBakeLightMapShader()
	{
		return m_haveBakeLMShader;
	}

	inline void setCurrentMeshBuffer(IMeshBuffer *buffer)
	{
		m_currentMeshBuffer = buffer;
	}

	inline IMeshBuffer* getCurrentMeshBuffer()
	{
		return m_currentMeshBuffer;
	}

	inline void setCurrentMaterial(SMaterial& mat)
	{
		m_currentMatRendering = &mat;
	}
	
	inline SMaterial* getCurrentMaterial()
	{
		return m_currentMatRendering;
	}

	inline int getCurrentRenderID()
	{
		return m_currentRenderID;
	}

	inline void setCurrentRenderID(int id)
	{
		m_currentRenderID = id;
	}

	void initBasicShader();
	void initSkylichtEngineShader();

	void initShader();

	// loadShader
	// todo:
	// load game shader from file config
	CShader* loadShader(const char *shaderConfig, IShaderCallback *callback = NULL, bool releaseCallback = true);

	// getShaderIDByName
	// todo:
	// get shader id by name
	int getShaderIDByName(const char *name);

	// getMaterialInfo
	CShader* getShaderByName(const char *name);
	CShader* getShaderByPath(const char *path);
	CShader* getShaderByID(int id);

	// getShaderFileName
	std::string getShaderFileName(const char *fileName);

	// getNumMaterial
	int getNumMaterial()
	{
		return (int)m_listShader.size();
	}

	// getShader
	CShader* getMaterial(int i)
	{
		return m_listShader[i];
	}
protected:

};

}

#endif