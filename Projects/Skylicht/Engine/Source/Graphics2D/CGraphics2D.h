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
#include "Graphics2D/SpriteFrame/CSpriteFrame.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	class CCamera;
	class CCanvas;

	class CGraphics2D : public CGameSingleton<CGraphics2D>
	{
	protected:
		int m_currentW;
		int m_currentH;

		float m_scaleRatio;

		IVideoDriver* m_driver;

		std::vector<CCanvas*> m_canvas;

	protected:

		video::SMaterial m_2dMaterial;
		video::SMaterial m_customMaterial;

		int m_vertexColorShader;

		IMeshBuffer* m_buffer;
		scene::SVertexBuffer* m_vertices;
		scene::CIndexBuffer* m_indices;

	public:
		CGraphics2D();
		virtual ~CGraphics2D();

		void init();

		void resize();

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

		void addCanvas(CCanvas* canvas);

		void removeCanvas(CCanvas* canvas);

		void render(CCamera* camera);

		void beginRenderGUI(const core::matrix4& projection, const core::matrix4& view);

		void endRenderGUI();

	public:

		void flushBuffer(IMeshBuffer* meshBuffer, video::SMaterial& material);

		void flush();

		void flushWithMaterial(CMaterial* material);

		void addExternalBuffer(IMeshBuffer* meshBuffer, const core::matrix4& absoluteMatrix, int shaderID, CMaterial* material = NULL);

		void addImageBatch(ITexture* img, const SColor& color, const core::matrix4& absoluteMatrix, int shaderID, CMaterial* material = NULL, float pivotX = 0, float pivotY = 0);

		void addImageBatch(ITexture* img, const core::rectf& dest, const core::rectf& source, const SColor& color, const core::matrix4& absoluteMatrix, int shaderID, CMaterial* material = NULL, float pivotX = 0, float pivotY = 0);

		void addModuleBatch(SModuleOffset* module, const SColor& color, const core::matrix4& absoluteMatrix, float offsetX, float offsetY, int shaderID, CMaterial* material = NULL);

		void addModuleBatch(SModuleOffset* module,
			const SColor& color,
			const core::matrix4& absoluteMatrix,
			const core::rectf& r,
			float anchorLeft,
			float anchorRight,
			float anchorTop,
			float anchorBottom,
			int shaderID,
			CMaterial* material = NULL);

		void addModuleBatch(SModuleOffset* module,
			const SColor& color,
			const core::matrix4& absoluteMatrix,
			const core::rectf& r,
			int shaderID,
			CMaterial* material = NULL);

		void addModuleBatchLR(SModuleOffset* module,
			const SColor& color,
			const core::matrix4& absoluteMatrix,
			const core::rectf& r,
			float anchorLeft,
			float anchorRight,
			int shaderID,
			CMaterial* material = NULL);

		void addModuleBatchTB(SModuleOffset* module,
			const SColor& color,
			const core::matrix4& absoluteMatrix,
			const core::rectf& r,
			float anchorTop,
			float anchorBottom,
			int shaderID,
			CMaterial* material = NULL);

		void addFrameBatch(SFrame* frame, const SColor& color, const core::matrix4& absoluteMatrix, int materialID, CMaterial* material = NULL);

		void addRectangleBatch(const core::rectf& pos, const core::rectf& uv, const SColor& color, const core::matrix4& absoluteTransform, int shaderID, CMaterial* material = NULL);

		void beginDrawDepth();

		void endDrawDepth();

		void beginDepthTest();

		void endDepthTest();

		void setWriteDepth(video::SMaterial& mat);

		void setDepthTest(video::SMaterial& mat);

		void setNoDepthTest(video::SMaterial& mat);

		void draw2DLine(const core::position2df& start, const core::position2df& end, const SColor& color);

		void draw2DRectangle(const core::rectf& pos, const SColor& color);

		void draw2DRectangle(const core::vector3df& upleft, const core::vector3df& lowerright, const SColor& color);

		void draw2DRectangleOutline(const core::rectf& pos, const SColor& color);

		video::SMaterial& getMaterial()
		{
			return m_2dMaterial;
		}

	private:

		void updateRectBuffer(video::S3DVertex* vtx, const core::rectf& r, const core::matrix4& mat);

		void updateRectTexcoordBuffer(video::S3DVertex* vtx, const core::rectf& r, float texWidth, float texHeight);
	};

}