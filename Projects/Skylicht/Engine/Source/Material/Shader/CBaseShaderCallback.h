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
#include "CShaderManager.h"

namespace Skylicht
{

	class CBaseShaderCallback : public video::IShaderConstantSetCallBack
	{
	protected:
		int		m_materialRenderID;
		bool	m_initCallback;

	public:
		CBaseShaderCallback();
		virtual ~CBaseShaderCallback();

		inline void setMaterialRenderID(int id)
		{
			m_materialRenderID = id;
		}

		inline int getMaterialRenderID()
		{
			return m_materialRenderID;
		}

		void offsetUV(core::vector2df& uv, float speedX, float speedY);

		bool isOpenGLFamily();

		void setAmbientLight(IMaterialRenderer *matRender, int ambientID, bool vertexConstant);

		void setDirection(IMaterialRenderer *matRender, int directionID, bool vertexConstant, const core::vector3df& dir, int count = 4, bool worldDirection = false);

		void setPosition(IMaterialRenderer *matRender, int posID, const core::vector3df& pos, bool vertexConstant = true);

		void setWorldPosition(IMaterialRenderer *matRender, int posID, const core::vector3df& pos, bool vertexConstant = true);
	};

}