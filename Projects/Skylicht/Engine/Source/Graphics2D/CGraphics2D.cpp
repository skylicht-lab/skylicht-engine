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

#include "CCanvas.h"

#include "CGraphics2D.h"
#include "Material/Shader/CShaderManager.h"

// camera transform
#include "GameObject/CGameObject.h"

#define MAX_VERTICES (1024*4)
#define MAX_INDICES	(1024*6)

namespace Skylicht
{

	CGraphics2D::CGraphics2D() :
		m_currentW(-1),
		m_currentH(-1),
		m_scaleRatio(1.0f)
	{
		m_driver = getVideoDriver();

		m_buffer = new CMeshBuffer<S3DVertex>(m_driver->getVertexDescriptor(EVT_STANDARD), video::EIT_16BIT);
		m_vertices = (SVertexBuffer*)m_buffer->getVertexBuffer();
		m_indices = (CIndexBuffer*)m_buffer->getIndexBuffer();

		m_2dMaterial.ZBuffer = ECFN_ALWAYS;
		m_2dMaterial.ZWriteEnable = false;
		m_2dMaterial.BackfaceCulling = false;
	}

	void CGraphics2D::init()
	{
		if (m_buffer)
			m_buffer->drop();
	}

	CGraphics2D::~CGraphics2D()
	{
	}

	core::dimension2du CGraphics2D::getScreenSize()
	{
		core::dimension2du screenSize = getVideoDriver()->getScreenSize();

		float w = (float)screenSize.Width * m_scaleRatio;
		float h = (float)screenSize.Height * m_scaleRatio;

		screenSize.set((int)w, (int)h);

		return screenSize;
	}

	bool CGraphics2D::isHD()
	{
		core::dimension2du size = getVideoDriver()->getScreenSize();
		if (size.Width > 1400 || size.Height > 1400)
			return true;
		return false;
	}

	bool CGraphics2D::isWideScreen()
	{
		core::dimension2du size = getVideoDriver()->getScreenSize();
		float r = size.Width / (float)size.Height;
		if (r >= 1.5f)
			return true;
		else
			return false;
	}

	void CGraphics2D::addCanvas(CCanvas *canvas)
	{
		if (std::find(m_canvas.begin(), m_canvas.end(), canvas) == m_canvas.end())
			m_canvas.push_back(canvas);
	}

	void CGraphics2D::removeCanvas(CCanvas *canvas)
	{
		std::vector<CCanvas*>::iterator i = std::find(m_canvas.begin(), m_canvas.end(), canvas);
		if (i != m_canvas.end())
			m_canvas.erase(i);
	}

	void CGraphics2D::render(CCamera *camera)
	{
		// sort canvas by depth
		struct {
			bool operator()(CCanvas* a, CCanvas* b) const
			{
				return a->getSortDepth() < b->getSortDepth();
			}
		} customLess;
		std::sort(m_canvas.begin(), m_canvas.end(), customLess);

		// set projection & view
		IVideoDriver *driver = getVideoDriver();
		const SViewFrustum& viewArea = camera->getViewFrustum();
		driver->setTransform(video::ETS_PROJECTION, viewArea.getTransform(video::ETS_PROJECTION));
		driver->setTransform(video::ETS_VIEW, viewArea.getTransform(video::ETS_VIEW));

		// Calculate billboard matrix
		// this is invert view camera
		const core::matrix4& cameraTransform = camera->getGameObject()->getTransform()->getMatrixTransform();

		// look vector
		core::vector3df cameraPosition = cameraTransform.getTranslation();
		core::vector3df look(0.0f, 0.0f, -1.0f);
		cameraTransform.rotateVect(look);
		look.normalize();

		// up vector
		core::vector3df up(0.0f, 1.0f, 0.0f);
		cameraTransform.rotateVect(up);
		up.normalize();

		// right vector
		core::vector3df right = up.crossProduct(look);
		up = look.crossProduct(right);
		up.normalize();

		// billboard matrix
		core::matrix4 billboardMatrix;
		f32 *matData = billboardMatrix.pointer();

		matData[0] = right.X;
		matData[1] = right.Y;
		matData[2] = right.Z;
		matData[3] = 0.0f;

		matData[4] = up.X;
		matData[5] = up.Y;
		matData[6] = up.Z;
		matData[7] = 0.0f;

		matData[8] = look.X;
		matData[9] = look.Y;
		matData[10] = look.Z;
		matData[11] = 0.0f;

		matData[12] = 0.0f;
		matData[13] = 0.0f;
		matData[14] = 0.0f;
		matData[15] = 1.0f;

		// render graphics
		for (CCanvas *canvas : m_canvas)
		{
			CGUIElement* root = canvas->getRootElement();

			if (canvas->isEnable3DBillboard() == true)
			{
				// rotation canvas to billboard
				core::matrix4 world = billboardMatrix;

				// scale canvas
				core::matrix4 scale;
				scale.setScale(root->getScale());
				world *= scale;

				// move to canvas position
				world.setTranslation(root->getPosition());

				// apply billboard transform to world and skip recalc ui transform
				driver->setTransform(video::ETS_WORLD, world);
			}
			else
			{
				// world is real transform
				const core::matrix4& world = root->getRelativeTransform(true);
				driver->setTransform(video::ETS_WORLD, world);
			}

			// render this canvas
			canvas->render(camera);
		}

		flush();
	}

	void CGraphics2D::flushBuffer(IMeshBuffer *meshBuffer, video::SMaterial& material)
	{
		scene::IVertexBuffer* vertices = meshBuffer->getVertexBuffer();
		scene::IIndexBuffer* indices = meshBuffer->getIndexBuffer();

		if (indices->getIndexCount() > 0 && vertices->getVertexCount() > 0)
		{
			// set shader if default
			if (material.MaterialType > 0)
			{

				if (m_scaleRatio != 1.0f)
					material.setFlag(video::EMF_TRILINEAR_FILTER, true);

				// set material
				m_driver->setMaterial(material);

				// draw mesh buffer
				meshBuffer->setDirty();
				meshBuffer->setPrimitiveType(scene::EPT_TRIANGLES);

				m_driver->drawMeshBuffer(meshBuffer);
			}

			// clear buffer
			indices->set_used(0);
			vertices->set_used(0);
		}
	}

	void CGraphics2D::flush()
	{
		flushBuffer(m_buffer, m_2dMaterial);
	}

	void CGraphics2D::addImageBatch(ITexture *img, const SColor& color, const core::matrix4& absoluteMatrix, int shaderID, float pivotX, float pivotY)
	{
		if (m_2dMaterial.getTexture(0) != img)
			flush();

		int numVertices = m_vertices->getVertexCount();
		int numVerticesUse = numVertices + 6;
		int numIndex = m_indices->getIndexCount();
		int numIndexUse = numIndex + 6;

		if (numVerticesUse > MAX_VERTICES || numIndexUse > MAX_INDICES)
		{
			flush();
			numVertices = 0;
			numIndex = 0;
			numVerticesUse = 6;
			numIndexUse = 4;
		}

		m_indices->set_used(numIndexUse);
		u16 *index = (u16*)m_indices->getIndices();
		index[numIndex + 0] = numVertices + 0;
		index[numIndex + 1] = numVertices + 1;
		index[numIndex + 2] = numVertices + 2;
		index[numIndex + 3] = numVertices + 0;
		index[numIndex + 4] = numVertices + 2;
		index[numIndex + 5] = numVertices + 3;

		m_vertices->set_used(numVerticesUse);
		S3DVertex *vertices = (S3DVertex*)m_vertices->getVertices();

		core::rectf pos;
		pos.UpperLeftCorner.X = -pivotX;
		pos.UpperLeftCorner.Y = -pivotY;
		pos.LowerRightCorner.X = pos.UpperLeftCorner.X + (f32)img->getSize().Width;
		pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + (f32)img->getSize().Height;

		// add vertices
		vertices[numVertices + 0] = S3DVertex(pos.UpperLeftCorner.X, pos.UpperLeftCorner.Y, 0, 0, 0, 1, color, 0.0f, 0.0f);
		vertices[numVertices + 1] = S3DVertex(pos.LowerRightCorner.X, pos.UpperLeftCorner.Y, 0, 0, 0, 1, color, 1.0f, 0.0f);
		vertices[numVertices + 2] = S3DVertex(pos.LowerRightCorner.X, pos.LowerRightCorner.Y, 0, 0, 0, 1, color, 1.0f, 1.0f);
		vertices[numVertices + 3] = S3DVertex(pos.UpperLeftCorner.X, pos.LowerRightCorner.Y, 0, 0, 0, 1, color, 0.0f, 1.0f);

		// transform
		for (int i = 0; i < 4; i++)
			absoluteMatrix.transformVect(vertices[numVertices + i].Pos);

		m_2dMaterial.setTexture(0, img);
		m_2dMaterial.MaterialType = shaderID;

		m_buffer->setDirty();

		// todo wait flush
		//m_driver->setMaterial(m_2dMaterial);
		//m_driver->drawMeshBuffer(m_buffer);
	}

	void CGraphics2D::addImageBatch(ITexture *img, const core::rectf& dest, const core::rectf& source, const SColor& color, const core::matrix4& absoluteMatrix, int shaderID, float pivotX, float pivotY)
	{
		if (m_2dMaterial.getTexture(0) != img)
			flush();

		int numVertices = m_vertices->getVertexCount();
		int numVerticesUse = numVertices + 6;
		int numIndex = m_indices->getIndexCount();
		int numIndexUse = numIndex + 6;

		if (numVerticesUse > MAX_VERTICES || numIndexUse > MAX_INDICES)
		{
			flush();
			numVertices = 0;
			numIndex = 0;
			numVerticesUse = 6;
			numIndexUse = 4;
		}

		m_indices->set_used(numIndexUse);
		u16 *index = (u16*)m_indices->getIndices();
		index[numIndex + 0] = numVertices + 0;
		index[numIndex + 1] = numVertices + 1;
		index[numIndex + 2] = numVertices + 2;
		index[numIndex + 3] = numVertices + 0;
		index[numIndex + 4] = numVertices + 2;
		index[numIndex + 5] = numVertices + 3;

		m_vertices->set_used(numVerticesUse);
		S3DVertex *vertices = (S3DVertex*)m_vertices->getVertices();

		core::rectf pos;
		pos.UpperLeftCorner.X = -pivotX;
		pos.UpperLeftCorner.Y = -pivotY;
		pos.LowerRightCorner.X = pos.UpperLeftCorner.X + dest.getWidth();
		pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + dest.getHeight();

		float tx1 = source.UpperLeftCorner.X / (float)img->getSize().Width;
		float ty1 = source.UpperLeftCorner.Y / (float)img->getSize().Height;
		float tx2 = source.LowerRightCorner.X / (float)img->getSize().Width;
		float ty2 = source.LowerRightCorner.Y / (float)img->getSize().Height;

		// add vertices
		vertices[numVertices + 0] = S3DVertex(pos.UpperLeftCorner.X, pos.UpperLeftCorner.Y, 0, 0, 0, 1, color, tx1, ty1);
		vertices[numVertices + 1] = S3DVertex(pos.LowerRightCorner.X, pos.UpperLeftCorner.Y, 0, 0, 0, 1, color, tx2, ty1);
		vertices[numVertices + 2] = S3DVertex(pos.LowerRightCorner.X, pos.LowerRightCorner.Y, 0, 0, 0, 1, color, tx2, ty2);
		vertices[numVertices + 3] = S3DVertex(pos.UpperLeftCorner.X, pos.LowerRightCorner.Y, 0, 0, 0, 1, color, tx1, ty2);

		// transform
		for (int i = 0; i < 4; i++)
			absoluteMatrix.transformVect(vertices[numVertices + i].Pos);

		m_2dMaterial.setTexture(0, img);
		m_2dMaterial.MaterialType = shaderID;
	}
}