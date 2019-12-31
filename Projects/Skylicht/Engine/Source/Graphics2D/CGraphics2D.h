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

	class CGraphics2D : public CGameSingleton<CGraphics2D>
	{
	public:
		enum EDrawState
		{
			NoThing,
			DrawSprite,
			DrawFont,
			DrawImage,
			DrawRect
		};

	protected:
		core::matrix4		m_orthoMatrix;

		video::SMaterial	m_2dMaterial;
		video::SMaterial	m_2dMaterialFont;
		video::SMaterial	m_2dMaterialImage;

		int m_currentW;
		int m_currentH;

		core::matrix4	m_prjMatrix;
		core::matrix4	m_viewMatrix;

		IVideoDriver*	m_driver;

		IMeshBuffer	*m_buffer;
		scene::SVertexBuffer*	m_vertices;
		scene::CIndexBuffer*	m_indices;

		IMeshBuffer	*m_bufferFont;
		scene::SVertexBuffer*	m_verticesFont;
		scene::CIndexBuffer*	m_indicesFont;

		IMeshBuffer	*m_bufferImage;
		scene::SVertexBuffer*	m_verticesImage;
		scene::CIndexBuffer*	m_indicesImage;

		bool	m_isDrawMask;
		bool	m_isMaskTest;

		int		m_numFlushTime;

		float	m_scaleRatio;

		EDrawState m_drawState;

		int		m_vertexColorShader;
		int		m_textureColorShader;
		int		m_textureColorAlphaMaskShader;

	public:
		CGraphics2D();
		virtual ~CGraphics2D();

		void init();

		void set2DViewport(int w, int h);

		void begin2D();

		void end();

		void draw2DLine(const core::position2df& start, const core::position2df& end, const SColor& color);

		void draw2DRectangle(const core::rectf& pos, const SColor& color);

		void draw2DRectangleOutline(const core::rectf& pos, const SColor& color);

		void draw2D();

		void flush();

		void flushBuffer(IMeshBuffer *meshBuffer, video::SMaterial& material);

		void beginDrawMask();

		void endDrawMask();

		void beginMaskTest();

		void endMaskTest();

		void addImageBatch(ITexture *img, const SColor& color, const core::matrix4& absoluteMatrix, int materialID, ITexture *alpha = NULL, int pivotX = 0, int pivotY = 0);

		void addImageBatch(ITexture *img, const core::rectf& dest, const core::rectf& source, const SColor& color, const core::matrix4& absoluteMatrix, int materialID, ITexture *alpha = NULL, int pivotX = 0, int pivotY = 0);

		void addRectBatch(const core::rectf& r, const SColor& color, const core::matrix4& absoluteMatrix, bool outLine = true);

		core::dimension2du getScreenSize();

		bool isHD();

		bool isWideScreen();

		float getScale()
		{
			return m_scaleRatio;
		}

		void setScale(float f)
		{
			m_scaleRatio = f;
		}

		void setWriteDepth(video::SMaterial& mat);

		void setTestDepth(video::SMaterial& mat);

		void setNoTestDepth(video::SMaterial& mat);
	};

}