// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#include "pch.h"

#include "CGraphics.h"
#include "Material/CShaderManager.h"

#define MAX_VERTICES	(128*4)
#define MAX_INDICES		(128*6)

namespace Skylicht
{

	CGraphics::CGraphics()
	{
		m_currentW = -1;
		m_currentH = -1;
		m_scaleRatio = 1.0f;

		m_buffer = NULL;
		m_bufferFont = NULL;
		m_bufferImage = NULL;

		m_driver = getVideoDriver();
		if (m_driver->getDriverType() == EDT_NULL)
			return;

		// buffer sprite
		m_buffer = new CMeshBuffer<S3DVertex>(m_driver->getVertexDescriptor(EVT_STANDARD), video::EIT_16BIT);
		m_vertices = (SVertexBuffer*)m_buffer->getVertexBuffer();
		m_indices = (CIndexBuffer*)m_buffer->getIndexBuffer();

		// buffer font
		m_bufferFont = new CMeshBuffer<S3DVertex>(m_driver->getVertexDescriptor(EVT_STANDARD), video::EIT_16BIT);
		m_verticesFont = (SVertexBuffer*)m_bufferFont->getVertexBuffer();
		m_indicesFont = (CIndexBuffer*)m_bufferFont->getIndexBuffer();

		// buffer image
		m_bufferImage = new CMeshBuffer<S3DVertex>(m_driver->getVertexDescriptor(EVT_STANDARD), video::EIT_16BIT);
		m_verticesImage = (SVertexBuffer*)m_bufferImage->getVertexBuffer();
		m_indicesImage = (CIndexBuffer*)m_bufferImage->getIndexBuffer();

		m_buffer->setHardwareMappingHint(EHM_STREAM);
		m_bufferFont->setHardwareMappingHint(EHM_STREAM);
		m_bufferImage->setHardwareMappingHint(EHM_STREAM);

		m_vertexColorShader = -1;

		m_isDrawMask = false;
		m_isMaskTest = false;

		m_drawState = NoThing;
	}

	void CGraphics::init()
	{
	}

	CGraphics::~CGraphics()
	{
		if (m_buffer)
			m_buffer->drop();

		if (m_bufferFont)
			m_bufferFont->drop();

		if (m_bufferImage)
			m_bufferImage->drop();
	}

	// set set2DViewport
	void CGraphics::set2DViewport(int w, int h)
	{
		m_orthoMatrix.makeIdentity();
		m_orthoMatrix.buildProjectionMatrixOrthoLH(w, -h, -1.0f, 1.0f);
		m_orthoMatrix.setTranslation(core::vector3df(-1, 1, 0));

		m_currentW = w;
		m_currentH = h;
	}

	// begin draw 2d
	void CGraphics::begin2D()
	{
		if (m_vertexColorShader == -1)
		{
			CShaderManager *shaderManager = CShaderManager::getInstance();

			m_vertexColorShader = shaderManager->getShaderIDByName("VertexColorAlpha");
			m_textureColorShader = shaderManager->getShaderIDByName("TextureColorAlpha");
			m_textureColorAlphaMaskShader = shaderManager->getShaderIDByName("AlphaBlend");
		}

		int w = (int)m_driver->getCurrentRenderTargetSize().Width;
		int h = (int)m_driver->getCurrentRenderTargetSize().Height;

		w = (int)(w*m_scaleRatio);
		h = (int)(h*m_scaleRatio);

		if (m_currentW != w || m_currentH != h)
			set2DViewport(w, h);

		m_prjMatrix = m_driver->getTransform(video::ETS_PROJECTION);
		m_viewMatrix = m_driver->getTransform(video::ETS_VIEW);

		m_driver->setTransform(video::ETS_PROJECTION, m_orthoMatrix);
		m_driver->setTransform(video::ETS_VIEW, core::IdentityMatrix);
		m_driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		m_numFlushTime = 0;
	}

	// end draw
	void CGraphics::end()
	{
		flush();
		m_driver->setTransform(video::ETS_PROJECTION, m_prjMatrix);
		m_driver->setTransform(video::ETS_VIEW, m_viewMatrix);

		//printf("Graphics::end numFlush: %d\n", m_numFlushTime);
	}

	// draw2DLine
	void CGraphics::draw2DLine(const core::position2df& start, const core::position2df& end, const SColor& color)
	{
		// flush buffer
		flush();

		m_indices->set_used(0);
		m_indices->addIndex(0);
		m_indices->addIndex(1);

		video::S3DVertex vert;
		vert.Color = color;

		vert.Pos.X = start.X;
		vert.Pos.Y = start.Y;
		vert.Pos.Z = 0;

		m_vertices->set_used(0);
		m_vertices->addVertex(vert);

		vert.Pos.X = end.X;
		vert.Pos.Y = end.Y;
		vert.Pos.Z = 0;
		m_vertices->addVertex(vert);

		m_2dMaterial.setTexture(0, NULL);
		m_2dMaterial.setTexture(1, NULL);
		m_2dMaterial.MaterialType = m_vertexColorShader;
		m_driver->setMaterial(m_2dMaterial);

		m_buffer->setPrimitiveType(scene::EPT_LINES);
		m_buffer->setDirty();

		m_driver->drawMeshBuffer(m_buffer);

		// clear buffer
		m_indices->set_used(0);
		m_vertices->set_used(0);
	}

	void CGraphics::draw2DRectangle(const core::rectf& pos, const SColor& color)
	{
		// flush buffer
		flush();

		m_indices->set_used(6);
		u16 *index = (u16*)m_indices->getIndices();
		index[0] = 0;
		index[1] = 1;
		index[2] = 2;
		index[3] = 0;
		index[4] = 2;
		index[5] = 3;

		m_vertices->set_used(4);
		S3DVertex *vertices = (S3DVertex*)m_vertices->getVertices();
		vertices[0] = S3DVertex(pos.UpperLeftCorner.X, pos.UpperLeftCorner.Y, 0, 0, 0, 1, color, 0, 0);
		vertices[1] = S3DVertex(pos.LowerRightCorner.X, pos.UpperLeftCorner.Y, 0, 0, 0, 1, color, 0, 0);
		vertices[2] = S3DVertex(pos.LowerRightCorner.X, pos.LowerRightCorner.Y, 0, 0, 0, 1, color, 0, 0);
		vertices[3] = S3DVertex(pos.UpperLeftCorner.X, pos.LowerRightCorner.Y, 0, 0, 0, 1, color, 0, 0);

		m_2dMaterial.setTexture(0, NULL);
		m_2dMaterial.setTexture(1, NULL);
		m_2dMaterial.MaterialType = m_vertexColorShader;
		m_driver->setMaterial(m_2dMaterial);

		m_buffer->setPrimitiveType(scene::EPT_TRIANGLES);
		m_buffer->setDirty();

		m_driver->drawMeshBuffer(m_buffer);

		// clear buffer
		m_indices->set_used(0);
		m_vertices->set_used(0);
	}

	void CGraphics::draw2DRectangleOutline(const core::rectf& pos, const SColor& color)
	{
		// flush buffer
		flush();

		m_indices->set_used(5);
		u16 *index = (u16*)m_indices->getIndices();
		index[0] = 0;
		index[1] = 1;
		index[2] = 2;
		index[3] = 3;
		index[4] = 0;

		m_vertices->set_used(4);
		S3DVertex *vertices = (S3DVertex*)m_vertices->getVertices();
		vertices[0] = S3DVertex(pos.UpperLeftCorner.X, pos.UpperLeftCorner.Y, 0, 0, 0, 1, color, 0, 0);
		vertices[1] = S3DVertex(pos.LowerRightCorner.X, pos.UpperLeftCorner.Y, 0, 0, 0, 1, color, 0, 0);
		vertices[2] = S3DVertex(pos.LowerRightCorner.X, pos.LowerRightCorner.Y, 0, 0, 0, 1, color, 0, 0);
		vertices[3] = S3DVertex(pos.UpperLeftCorner.X, pos.LowerRightCorner.Y, 0, 0, 0, 1, color, 0, 0);

		m_2dMaterial.setTexture(0, NULL);
		m_2dMaterial.setTexture(1, NULL);
		m_2dMaterial.MaterialType = m_vertexColorShader;
		m_driver->setMaterial(m_2dMaterial);

		m_buffer->setPrimitiveType(scene::EPT_LINE_STRIP);
		m_buffer->setDirty();

		m_driver->drawMeshBuffer(m_buffer);

		// clear buffer
		m_indices->set_used(0);
		m_vertices->set_used(0);
	}

	// draw2D
	void CGraphics::draw2D()
	{
	}

	// flush
	void CGraphics::flush()
	{
		flushBuffer(m_buffer, m_2dMaterial);
		flushBuffer(m_bufferFont, m_2dMaterialFont);
		flushBuffer(m_bufferImage, m_2dMaterialImage);

		m_drawState = NoThing;
	}

	void CGraphics::flushBuffer(IMeshBuffer *meshBuffer, video::SMaterial& material)
	{
		scene::IVertexBuffer* vertices = meshBuffer->getVertexBuffer();
		scene::IIndexBuffer* indices = meshBuffer->getIndexBuffer();

		if (indices->getIndexCount() > 0 && vertices->getVertexCount() > 0)
		{
			m_numFlushTime++;

			// set shader if default
			if (material.MaterialType < 0)
			{
				if (material.getTexture(0) != NULL && material.getTexture(1) == NULL)
					material.MaterialType = m_textureColorShader;
				else if (material.getTexture(0) != NULL && material.getTexture(1) != NULL)
					material.MaterialType = m_textureColorAlphaMaskShader;
				else
					material.MaterialType = m_vertexColorShader;
			}

			if (m_scaleRatio != 1.0f)
				material.setFlag(video::EMF_TRILINEAR_FILTER, true);

			// set material
			m_driver->setMaterial(material);

			// draw mesh buffer
			meshBuffer->setDirty();
			meshBuffer->setPrimitiveType(scene::EPT_TRIANGLES);

			m_driver->drawMeshBuffer(meshBuffer);

			// clear buffer
			indices->set_used(0);
			vertices->set_used(0);
		}
	}

	// setWriteDepth
	void CGraphics::setWriteDepth(video::SMaterial& mat)
	{
		mat.ZBuffer = video::ECFN_ALWAYS;
		mat.ZWriteEnable = true;
		mat.ColorMask = ECP_NONE;
	}

	// setTestDepth
	void CGraphics::setTestDepth(video::SMaterial& mat)
	{
		mat.ZBuffer = video::ECFN_EQUAL;
		mat.ZWriteEnable = false;
		mat.ColorMask = ECP_ALL;
	}

	// setNoTestDepth
	void CGraphics::setNoTestDepth(video::SMaterial& mat)
	{
		mat.ZBuffer = video::ECFN_ALWAYS;
		mat.ZWriteEnable = false;
		mat.ColorMask = ECP_ALL;
	}

	// masking
	void CGraphics::beginDrawMask()
	{
		// render all
		flush();

		// clear Z
		m_driver->clearZBuffer();

		m_isDrawMask = true;

		// setting ZBuffer write
		setWriteDepth(m_2dMaterial);
		setWriteDepth(m_2dMaterialFont);
		setWriteDepth(m_2dMaterialImage);

		m_driver->setAllowZWriteOnTransparent(true);
	}

	void CGraphics::endDrawMask()
	{
		// render all
		flush();

		m_isDrawMask = false;

		// disable ZBuffer write
		setNoTestDepth(m_2dMaterial);
		setNoTestDepth(m_2dMaterialFont);
		setNoTestDepth(m_2dMaterialImage);

		m_driver->setAllowZWriteOnTransparent(false);
	}

	void CGraphics::beginMaskTest()
	{
		// depth test
		setTestDepth(m_2dMaterial);
		setTestDepth(m_2dMaterialFont);
		setTestDepth(m_2dMaterialImage);
	}

	void CGraphics::endMaskTest()
	{
		// render all
		flush();

		// disable ZBuffer write & test
		setNoTestDepth(m_2dMaterial);
		setNoTestDepth(m_2dMaterialFont);
		setNoTestDepth(m_2dMaterialImage);
	}

	void CGraphics::addImageBatch(ITexture *img, const core::rectf& dest, const core::rectf& source, const SColor& color, const core::matrix4& absoluteMatrix, int materialID, ITexture *alpha, int pivotX, int pivotY)
	{
		if (m_2dMaterialImage.getTexture(0) != img || m_drawState != DrawImage)
			flush();

		int numVertices = m_verticesImage->getVertexCount();
		int numVerticesUse = numVertices + 6;
		int numIndex = m_indicesImage->getIndexCount();
		int numIndexUse = numIndex + 6;

		if (numVerticesUse > MAX_VERTICES || numIndexUse > MAX_INDICES)
		{
			flush();
			numVertices = 0;
			numIndex = 0;
			numVerticesUse = 6;
			numIndexUse = 4;
		}

		m_drawState = DrawImage;

		m_indicesImage->set_used(numIndexUse);
		u16 *index = (u16*)m_indicesImage->getIndices();
		index[numIndex + 0] = numVertices + 0;
		index[numIndex + 1] = numVertices + 1;
		index[numIndex + 2] = numVertices + 2;
		index[numIndex + 3] = numVertices + 0;
		index[numIndex + 4] = numVertices + 2;
		index[numIndex + 5] = numVertices + 3;

		m_verticesImage->set_used(numVerticesUse);
		S3DVertex *vertices = (S3DVertex*)m_verticesImage->getVertices();

		core::recti pos;
		pos.UpperLeftCorner.X = -pivotX;
		pos.UpperLeftCorner.Y = -pivotY;
		pos.LowerRightCorner.X = pos.UpperLeftCorner.X + dest.getWidth();
		pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + dest.getHeight();

		float tx1 = source.UpperLeftCorner.X / (float)img->getSize().Width;
		float ty1 = source.UpperLeftCorner.Y / (float)img->getSize().Height;
		float tx2 = source.LowerRightCorner.X / (float)img->getSize().Width;
		float ty2 = source.LowerRightCorner.Y / (float)img->getSize().Height;

		// add vertices
		vertices[numVertices + 0] = S3DVertex((f32)pos.UpperLeftCorner.X, (f32)pos.UpperLeftCorner.Y, 0, 0, 0, 1, color, tx1, ty1);
		vertices[numVertices + 1] = S3DVertex((f32)pos.LowerRightCorner.X, (f32)pos.UpperLeftCorner.Y, 0, 0, 0, 1, color, tx2, ty1);
		vertices[numVertices + 2] = S3DVertex((f32)pos.LowerRightCorner.X, (f32)pos.LowerRightCorner.Y, 0, 0, 0, 1, color, tx2, ty2);
		vertices[numVertices + 3] = S3DVertex((f32)pos.UpperLeftCorner.X, (f32)pos.LowerRightCorner.Y, 0, 0, 0, 1, color, tx1, ty2);

		// transform
		for (int i = 0; i < 4; i++)
			absoluteMatrix.transformVect(vertices[numVertices + i].Pos);

		m_2dMaterialImage.setTexture(0, img);
		m_2dMaterialImage.setTexture(1, alpha);
		m_2dMaterialImage.MaterialType = materialID;
	}

	// addImageBatch
	void CGraphics::addImageBatch(ITexture *img, const SColor& color, const core::matrix4& absoluteMatrix, int materialID, ITexture *alpha, int pivotX, int pivotY)
	{
		if (m_2dMaterialImage.getTexture(0) != img || m_drawState != DrawImage)
			flush();

		int numVertices = m_verticesImage->getVertexCount();
		int numVerticesUse = numVertices + 6;
		int numIndex = m_indicesImage->getIndexCount();
		int numIndexUse = numIndex + 6;

		if (numVerticesUse > MAX_VERTICES || numIndexUse > MAX_INDICES)
		{
			flush();
			numVertices = 0;
			numIndex = 0;
			numVerticesUse = 6;
			numIndexUse = 4;
		}

		m_drawState = DrawImage;

		m_indicesImage->set_used(numIndexUse);
		u16 *index = (u16*)m_indicesImage->getIndices();
		index[numIndex + 0] = numVertices + 0;
		index[numIndex + 1] = numVertices + 1;
		index[numIndex + 2] = numVertices + 2;
		index[numIndex + 3] = numVertices + 0;
		index[numIndex + 4] = numVertices + 2;
		index[numIndex + 5] = numVertices + 3;

		m_verticesImage->set_used(numVerticesUse);
		S3DVertex *vertices = (S3DVertex*)m_verticesImage->getVertices();

		core::recti pos;
		pos.UpperLeftCorner.X = -pivotX;
		pos.UpperLeftCorner.Y = -pivotY;
		pos.LowerRightCorner.X = pos.UpperLeftCorner.X + img->getSize().Width;
		pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + img->getSize().Height;

		// add vertices
		vertices[numVertices + 0] = S3DVertex((f32)pos.UpperLeftCorner.X, (f32)pos.UpperLeftCorner.Y, 0, 0, 0, 1, color, 0.0f, 0.0f);
		vertices[numVertices + 1] = S3DVertex((f32)pos.LowerRightCorner.X, (f32)pos.UpperLeftCorner.Y, 0, 0, 0, 1, color, 1.0f, 0.0f);
		vertices[numVertices + 2] = S3DVertex((f32)pos.LowerRightCorner.X, (f32)pos.LowerRightCorner.Y, 0, 0, 0, 1, color, 1.0f, 1.0f);
		vertices[numVertices + 3] = S3DVertex((f32)pos.UpperLeftCorner.X, (f32)pos.LowerRightCorner.Y, 0, 0, 0, 1, color, 0.0f, 1.0f);

		// transform
		for (int i = 0; i < 4; i++)
			absoluteMatrix.transformVect(vertices[numVertices + i].Pos);

		m_2dMaterialImage.setTexture(0, img);
		m_2dMaterialImage.setTexture(1, alpha);
		m_2dMaterialImage.MaterialType = materialID;

		m_bufferImage->setDirty();

		// todo wait flush
		m_driver->setMaterial(m_2dMaterial);
		m_driver->drawMeshBuffer(m_bufferImage);

		// clear buffer
		//m_indices->set_used(0);
		//m_vertices->set_used(0);
	}

	// addRect
	void CGraphics::addRectBatch(const core::rectf& r, const SColor& color, const core::matrix4& absoluteMatrix, bool outLine)
	{
		flush();

		int numSpriteVertex = 4;

		int numVertices = m_vertices->getVertexCount();
		int vertexUse = numVertices + numSpriteVertex;

		int numSpriteIndex = 6;
		if (outLine)
			numSpriteIndex = 5;

		int numIndices = m_indices->getIndexCount();
		int indexUse = numIndices + numSpriteIndex;

		if (vertexUse > MAX_VERTICES || indexUse > MAX_INDICES)
		{
			flush();
			numVertices = 0;
			numIndices = 0;
			vertexUse = numSpriteVertex;
			indexUse = numSpriteIndex;
		}

		m_drawState = DrawRect;

		// alloc vertex
		m_vertices->set_used(vertexUse);

		// get vertex pointer
		video::S3DVertex *vertices = (video::S3DVertex*)m_vertices->getVertices();
		vertices += numVertices;

		// alloc index
		m_indices->set_used(indexUse);

		// get indices pointer
		s16 *indices = (s16*)m_indices->getIndices();
		indices += numIndices;

		vertices[0].Pos = core::vector3df((f32)r.UpperLeftCorner.X, (f32)r.UpperLeftCorner.Y, 0.0f);
		vertices[0].Color = color;
		absoluteMatrix.transformVect(vertices[0].Pos);

		vertices[1].Pos = core::vector3df((f32)r.LowerRightCorner.X, (f32)r.UpperLeftCorner.Y, 0.0f);
		vertices[1].Color = color;
		absoluteMatrix.transformVect(vertices[1].Pos);

		vertices[2].Pos = core::vector3df((f32)r.LowerRightCorner.X, (f32)r.LowerRightCorner.Y, 0.0f);
		vertices[2].Color = color;
		absoluteMatrix.transformVect(vertices[2].Pos);

		vertices[3].Pos = core::vector3df((f32)r.UpperLeftCorner.X, (f32)r.LowerRightCorner.Y, 0.0f);
		vertices[3].Color = color;
		absoluteMatrix.transformVect(vertices[3].Pos);

		// set material
		m_2dMaterial.setTexture(0, NULL);
		m_2dMaterial.setTexture(1, NULL);
		m_2dMaterial.MaterialType = m_vertexColorShader;

		if (outLine)
		{
			indices[0] = numVertices;
			indices[1] = numVertices + 1;
			indices[2] = numVertices + 2;
			indices[3] = numVertices + 3;
			indices[4] = numVertices;

			// set material
			m_driver->setMaterial(m_2dMaterial);

			// draw mesh buffer
			m_buffer->setPrimitiveType(scene::EPT_LINE_STRIP);
			m_buffer->setDirty();

			m_driver->drawMeshBuffer(m_buffer);

			m_indices->set_used(0);
			m_vertices->set_used(0);
		}
		else
		{
			indices[0] = numVertices;
			indices[1] = numVertices + 1;
			indices[2] = numVertices + 2;

			indices[3] = numVertices;
			indices[4] = numVertices + 2;
			indices[5] = numVertices + 3;
		}
	}

	// getScreenSize
	core::dimension2du CGraphics::getScreenSize()
	{
		core::dimension2du screenSize = getVideoDriver()->getScreenSize();

		float w = (float)screenSize.Width * m_scaleRatio;
		float h = (float)screenSize.Height * m_scaleRatio;

		screenSize.set((int)w, (int)h);

		return screenSize;
	}

	bool CGraphics::isHD()
	{
		core::dimension2du size = getVideoDriver()->getScreenSize();
		if (size.Width > 1400 || size.Height > 1400)
			return true;
		return false;
	}

	bool CGraphics::isWideScreen()
	{
		core::dimension2du size = getVideoDriver()->getScreenSize();
		float r = size.Width / (float)size.Height;
		if (r >= 1.5f)
			return true;
		else
			return false;
	}

}