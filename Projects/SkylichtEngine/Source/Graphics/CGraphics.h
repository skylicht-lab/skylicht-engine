// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include "pch.h"
#include "Utils/CGameSingleton.h"

namespace Skylicht
{

class CGraphics: public CGameSingleton<CGraphics>
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
	CGraphics();
	virtual ~CGraphics();

	void init();

	// set set2DViewport
	void set2DViewport(int w, int h);

	// begin draw 2d
	void begin2D();

	// end draw
	void end();

	// draw2DLine
	void draw2DLine(const core::position2df& start, const core::position2df& end, const SColor& color);

	// draw2DRectangle
	void draw2DRectangle(const core::rectf& pos, const SColor& color);

	// draw2DRectangle
	void draw2DRectangleOutline(const core::rectf& pos, const SColor& color);

	// draw2D
	void draw2D();

	// flush
	void flush();
	void flushBuffer(IMeshBuffer *meshBuffer, video::SMaterial& material);

	// masking
	void beginDrawMask();
	void endDrawMask();

	void beginMaskTest();
	void endMaskTest();

	// addImageBatch
	void addImageBatch(ITexture *img, const SColor& color, const core::matrix4& absoluteMatrix, int materialID, ITexture *alpha = NULL, int pivotX = 0, int pivotY = 0);
	void addImageBatch(ITexture *img, const core::rectf& dest, const core::rectf& source, const SColor& color, const core::matrix4& absoluteMatrix, int materialID, ITexture *alpha = NULL, int pivotX = 0, int pivotY = 0);	
	
	// addRect
	void addRectBatch(const core::rectf& r, const SColor& color, const core::matrix4& absoluteMatrix, bool outLine = true);

	// getScreenSize
	core::dimension2du getScreenSize();

	bool isHD();
	bool isWideScreen();

	// getScale
	float getScale()
	{
		return m_scaleRatio;
	}

	// setScale
	void setScale(float f)
	{
		m_scaleRatio = f;
	}

	// setWriteDepth
	void setWriteDepth(video::SMaterial& mat);

	// setTestDepth
	void setTestDepth(video::SMaterial& mat);

	// setNoTestDepth
	void setNoTestDepth(video::SMaterial& mat);
};

}

#endif