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

#pragma once

#include "pch.h"
#include "Utils/CGameSingleton.h"

namespace Skylicht
{

	class CShader;
	class IShaderCallback;

	class CShaderManager : public CGameSingleton<CShaderManager>
	{
	protected:
		IMeshBuffer *m_currentMeshBuffer;
		SMaterial *m_currentMatRendering;

		int m_currentRenderID;

		std::vector<CShader*> m_listShader;
		std::map<std::string, int> m_listShaderID;

	public:

		// todo
		// use to save uniform for current draw command
		core::vector2df	ShaderVec2[10];
		core::vector3df	ShaderVec3[10];
		video::SVec4	ShaderVec4[10];

	public:

		CShaderManager();
		virtual ~CShaderManager();

		void releaseAll();

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
		// load game shader from file config
		CShader* loadShader(const char *shaderConfig, IShaderCallback *callback = NULL, bool releaseCallback = true);

		int getShaderIDByName(const char *name);

		CShader* getShaderByName(const char *name);

		CShader* getShaderByPath(const char *path);

		CShader* getShaderByID(int id);

		std::string getShaderFileName(const char *fileName);

		int getNumMaterial()
		{
			return (int)m_listShader.size();
		}

		CShader* getMaterial(int i)
		{
			return m_listShader[i];
		}

	};

}