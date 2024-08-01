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
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"

// camera transform
#include "GameObject/CGameObject.h"

#define MAX_VERTICES (1024*4)
#define MAX_INDICES	(1024*6)

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CGraphics2D);

	CGraphics2D::CGraphics2D() :
		m_currentW(-1),
		m_currentH(-1),
		m_vertexColorShader(0),
		m_bufferID(0)
	{
		m_driver = getVideoDriver();

		m_buffer = new CMeshBuffer<S3DVertex>(m_driver->getVertexDescriptor(EVT_STANDARD), video::EIT_16BIT);
		m_buffer->setHardwareMappingHint(EHM_STREAM);

		m_vertices = (SVertexBuffer*)m_buffer->getVertexBuffer();
		m_indices = (CIndexBuffer*)m_buffer->getIndexBuffer();

		m_allBuffers.push_back(m_buffer);
		m_bufferID = 0;

		m_2dMaterial.ZBuffer = ECFN_ALWAYS;
		m_2dMaterial.ZWriteEnable = false;
		m_2dMaterial.BackfaceCulling = false;

		for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
			m_2dMaterial.setFlag(EMF_TRILINEAR_FILTER, true, 8);
	}

	CGraphics2D::~CGraphics2D()
	{
		for (IMeshBuffer* buffer : m_allBuffers)
		{
			if (buffer)
				buffer->drop();
		}

		m_allBuffers.clear();
	}

	void CGraphics2D::resize()
	{
		for (CCanvas* c : m_canvas)
		{
			c->onResize();
		}
	}

	core::dimension2du CGraphics2D::getScreenSize()
	{
		core::dimension2du screenSize = getVideoDriver()->getScreenSize();

		float w = (float)screenSize.Width;
		float h = (float)screenSize.Height;

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

	void CGraphics2D::addCanvas(CCanvas* canvas)
	{
		if (std::find(m_canvas.begin(), m_canvas.end(), canvas) == m_canvas.end())
			m_canvas.push_back(canvas);
	}

	void CGraphics2D::removeCanvas(CCanvas* canvas)
	{
		std::vector<CCanvas*>::iterator i = std::find(m_canvas.begin(), m_canvas.end(), canvas);
		if (i != m_canvas.end())
			m_canvas.erase(i);
	}

	void CGraphics2D::render(CCamera* camera)
	{
		if (camera == NULL)
			return;

		prepareBuffer();

		// sort canvas by depth
		struct {
			bool operator()(CCanvas* a, CCanvas* b) const
			{
				return a->getSortDepth() < b->getSortDepth();
			}
		} customLess;
		std::sort(m_canvas.begin(), m_canvas.end(), customLess);

		core::matrix4 billboardMatrix;

		IVideoDriver* driver = getVideoDriver();

		// set projection & view			
		const SViewFrustum& viewArea = camera->getViewFrustum();
		driver->setTransform(video::ETS_PROJECTION, viewArea.getTransform(video::ETS_PROJECTION));
		driver->setTransform(video::ETS_VIEW, viewArea.getTransform(video::ETS_VIEW));

		u32 cameraCullingMask = camera->getCullingMask();

		if (camera->getProjectionType() != CCamera::OrthoUI)
		{
			// Calculate billboard matrix
			// this is invert view camera
			const core::matrix4& cameraTransform = camera->getGameObject()->getTransform()->getRelativeTransform();

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
			f32* matData = billboardMatrix.pointer();

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
		}

		// render graphics
		for (CCanvas* canvas : m_canvas)
		{
			if (canvas->getGameObject()->isVisible() == false)
				continue;

			// culling
			u32 cullingLayer = canvas->getGameObject()->getCullingLayer();
			u32 test = cameraCullingMask & cullingLayer;
			if (test == 0)
				continue;

			CGUIElement* root = canvas->getRootElement();

			if (root->getDock() != EGUIDock::NoDock)
			{
				// set screensize for root
				core::dimension2du s = getScreenSize();
				float w = (float)s.Width;
				float h = (float)s.Height;
				root->setRect(core::rectf(0.0f, 0.0f, w, h));
			}

			// update canvas layout, position...
			canvas->updateEntities();

			core::matrix4 world;

			if (canvas->isEnable3DBillboard() == true && camera->getProjectionType() != CCamera::OrthoUI)
			{
				// rotation canvas to billboard
				world = billboardMatrix;

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
				world = root->getRelativeTransform();
				driver->setTransform(video::ETS_WORLD, world);
			}

			// save real world transform
			// root transform alway identity transform see (CGUIElement::calcAbsoluteTransform)
			canvas->setRenderWorldTransform(world);

			// render this canvas
			canvas->render(camera);

			flush();
		}
	}

	void CGraphics2D::beginRenderGUI(const core::matrix4& projection, const core::matrix4& view)
	{
		IVideoDriver* driver = getVideoDriver();

		// set projection & view
		driver->setTransform(video::ETS_PROJECTION, projection);
		driver->setTransform(video::ETS_VIEW, view);
		driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		prepareBuffer();
	}

	void CGraphics2D::endRenderGUI()
	{
		// flush to screen
		flush();
	}

	void CGraphics2D::prepareBuffer()
	{
		m_bufferID = 0;
		m_buffer = m_allBuffers[m_bufferID];
		m_vertices = (SVertexBuffer*)m_buffer->getVertexBuffer();
		m_indices = (CIndexBuffer*)m_buffer->getIndexBuffer();
	}

	void CGraphics2D::nextBuffer()
	{
		m_bufferID++;

		if (m_bufferID >= 50)
			m_bufferID = 0;

		if (m_bufferID >= m_allBuffers.size())
		{
			m_buffer = new CMeshBuffer<S3DVertex>(m_driver->getVertexDescriptor(EVT_STANDARD), video::EIT_16BIT);
			m_buffer->setHardwareMappingHint(EHM_STREAM);

			m_vertices = (SVertexBuffer*)m_buffer->getVertexBuffer();
			m_indices = (CIndexBuffer*)m_buffer->getIndexBuffer();

			m_allBuffers.push_back(m_buffer);
		}
		else
		{
			m_buffer = m_allBuffers[m_bufferID];
			m_vertices = (SVertexBuffer*)m_buffer->getVertexBuffer();
			m_indices = (CIndexBuffer*)m_buffer->getIndexBuffer();
		}
	}

	void CGraphics2D::flushBuffer(IMeshBuffer* meshBuffer, video::SMaterial& material)
	{
		scene::IVertexBuffer* vertices = meshBuffer->getVertexBuffer();
		scene::IIndexBuffer* indices = meshBuffer->getIndexBuffer();

		if (indices->getIndexCount() > 0 && vertices->getVertexCount() > 0)
		{
			// set shader if default
			if (material.MaterialType > 0)
			{
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

			nextBuffer();
		}
	}

	void CGraphics2D::flush()
	{
		if (m_vertexColorShader == 0)
			m_vertexColorShader = CShaderManager::getInstance()->getShaderIDByName("VertexColorAlpha");

		flushBuffer(m_buffer, m_2dMaterial);
	}

	void CGraphics2D::flushWithMaterial(CMaterial* material)
	{
		CShaderMaterial::setMaterial(material);

		material->setTexture(0, m_2dMaterial.getTexture(0));
		material->updateTexture(m_customMaterial);

		flushBuffer(m_buffer, m_customMaterial);
	}

	void CGraphics2D::addExternalBuffer(IMeshBuffer* meshBuffer, const core::matrix4& absoluteMatrix, int shaderID, CMaterial* material)
	{
		if (m_2dMaterial.MaterialType != shaderID || material != NULL)
			flush();

		scene::IVertexBuffer* vtxBuffer = meshBuffer->getVertexBuffer();
		scene::IIndexBuffer* idxBuffer = meshBuffer->getIndexBuffer();

		u16* idxData = (u16*)idxBuffer->getIndices();
		S3DVertex* vtxData = (S3DVertex*)vtxBuffer->getVertices();

		int numVtx = vtxBuffer->getVertexCount();
		int numIdx = idxBuffer->getIndexCount();

		int numVertices = m_vertices->getVertexCount();
		int numVerticesUse = numVertices + numVtx;
		int numIndex = m_indices->getIndexCount();
		int numIndexUse = numIndex + numIdx;

		if (numVerticesUse > MAX_VERTICES || numIndexUse > MAX_INDICES)
		{
			flush();
			numVertices = 0;
			numIndex = 0;
			numVerticesUse = 6;
			numIndexUse = 4;
		}

		m_indices->set_used(numIndexUse);
		u16* index = (u16*)m_indices->getIndices();
		for (int i = 0; i < numIdx; i++)
			index[numIndex + i] = numVertices + idxData[i];

		m_vertices->set_used(numVerticesUse);
		S3DVertex* vertices = (S3DVertex*)m_vertices->getVertices();

		// add vertices
		for (int i = 0; i < numVtx; i++)
			vertices[numVertices + i] = vtxData[i];


		// transform
		for (int i = 0; i < numVtx; i++)
			absoluteMatrix.transformVect(vertices[numVertices + i].Pos);

		m_2dMaterial.MaterialType = shaderID;
		m_buffer->setDirty();

		if (material != NULL)
			flushWithMaterial(material);
	}

	void CGraphics2D::addImageBatch(ITexture* img, const SColor& color, const core::matrix4& absoluteMatrix, int shaderID, CMaterial* material, float pivotX, float pivotY)
	{
		if (m_2dMaterial.getTexture(0) != img || m_2dMaterial.MaterialType != shaderID || material != NULL)
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
		u16* index = (u16*)m_indices->getIndices();
		index[numIndex + 0] = numVertices + 0;
		index[numIndex + 1] = numVertices + 1;
		index[numIndex + 2] = numVertices + 2;
		index[numIndex + 3] = numVertices + 0;
		index[numIndex + 4] = numVertices + 2;
		index[numIndex + 5] = numVertices + 3;

		m_vertices->set_used(numVerticesUse);
		S3DVertex* vertices = (S3DVertex*)m_vertices->getVertices();

		core::rectf pos;
		pos.UpperLeftCorner.X = -pivotX;
		pos.UpperLeftCorner.Y = -pivotY;
		pos.LowerRightCorner.X = pos.UpperLeftCorner.X + (f32)img->getSize().Width;
		pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + (f32)img->getSize().Height;

		// add vertices
		vertices[numVertices + 0] = S3DVertex(pos.UpperLeftCorner.X, pos.UpperLeftCorner.Y, 0.0f, 0.0f, 0.0f, 1.0f, color, 0.0f, 0.0f);
		vertices[numVertices + 1] = S3DVertex(pos.LowerRightCorner.X, pos.UpperLeftCorner.Y, 0.0f, 1.0f, 0.0f, 1.0f, color, 1.0f, 0.0f);
		vertices[numVertices + 2] = S3DVertex(pos.LowerRightCorner.X, pos.LowerRightCorner.Y, 0.0f, 1.0f, 1.0f, 1.0f, color, 1.0f, 1.0f);
		vertices[numVertices + 3] = S3DVertex(pos.UpperLeftCorner.X, pos.LowerRightCorner.Y, 0.0f, 0.0f, 1.0f, 1.0f, color, 0.0f, 1.0f);

		// transform
		for (int i = 0; i < 4; i++)
			absoluteMatrix.transformVect(vertices[numVertices + i].Pos);

		m_2dMaterial.setTexture(0, img);
		m_2dMaterial.MaterialType = shaderID;

		m_buffer->setDirty();

		if (material != NULL)
			flushWithMaterial(material);
	}

	void CGraphics2D::addImageBatch(ITexture* img, const core::rectf& dest, const core::rectf& source, const SColor& color, const core::matrix4& absoluteMatrix, int shaderID, CMaterial* material, float pivotX, float pivotY)
	{
		if (m_2dMaterial.getTexture(0) != img || m_2dMaterial.MaterialType != shaderID || material != NULL)
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
		u16* index = (u16*)m_indices->getIndices();
		index[numIndex + 0] = numVertices + 0;
		index[numIndex + 1] = numVertices + 1;
		index[numIndex + 2] = numVertices + 2;
		index[numIndex + 3] = numVertices + 0;
		index[numIndex + 4] = numVertices + 2;
		index[numIndex + 5] = numVertices + 3;

		m_vertices->set_used(numVerticesUse);
		S3DVertex* vertices = (S3DVertex*)m_vertices->getVertices();

		core::rectf pos;
		pos.UpperLeftCorner.X = -pivotX + dest.UpperLeftCorner.X;
		pos.UpperLeftCorner.Y = -pivotY + dest.UpperLeftCorner.Y;
		pos.LowerRightCorner.X = pos.UpperLeftCorner.X + dest.getWidth();
		pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + dest.getHeight();

		float tx1 = source.UpperLeftCorner.X / (float)img->getSize().Width;
		float ty1 = source.UpperLeftCorner.Y / (float)img->getSize().Height;
		float tx2 = source.LowerRightCorner.X / (float)img->getSize().Width;
		float ty2 = source.LowerRightCorner.Y / (float)img->getSize().Height;

		// add vertices
		vertices[numVertices + 0] = S3DVertex(pos.UpperLeftCorner.X, pos.UpperLeftCorner.Y, 0.0f, 0.0f, 0.0f, 1.0f, color, tx1, ty1);
		vertices[numVertices + 1] = S3DVertex(pos.LowerRightCorner.X, pos.UpperLeftCorner.Y, 0.0f, 1.0f, 0.0f, 1.0f, color, tx2, ty1);
		vertices[numVertices + 2] = S3DVertex(pos.LowerRightCorner.X, pos.LowerRightCorner.Y, 0.0f, 1.0f, 1.0f, 1.0f, color, tx2, ty2);
		vertices[numVertices + 3] = S3DVertex(pos.UpperLeftCorner.X, pos.LowerRightCorner.Y, 0.0f, 0.0f, 1.0f, 1.0f, color, tx1, ty2);

		// transform
		for (int i = 0; i < 4; i++)
			absoluteMatrix.transformVect(vertices[numVertices + i].Pos);

		m_2dMaterial.setTexture(0, img);
		m_2dMaterial.MaterialType = shaderID;

		if (material != NULL)
			flushWithMaterial(material);
	}

	void CGraphics2D::addModuleBatch(SModuleOffset* module, const SColor& color, const core::matrix4& absoluteMatrix, float offsetX, float offsetY, int shaderID, CMaterial* material)
	{
		ITexture* tex = module->Frame->Image->Texture;

		if (m_2dMaterial.getTexture(0) != tex || m_2dMaterial.MaterialType != shaderID || material != NULL)
			flush();

		int numSpriteVertex = 4;

		int numVertices = m_vertices->getVertexCount();
		int vertexUse = numVertices + numSpriteVertex;

		int numSpriteIndex = 6;
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

		m_2dMaterial.setTexture(0, tex);
		m_2dMaterial.MaterialType = shaderID;

		// alloc vertex
		m_vertices->set_used(vertexUse);

		// get vertex pointer
		video::S3DVertex* vertices = (video::S3DVertex*)m_vertices->getVertices();
		vertices += numVertices;

		// alloc index
		m_indices->set_used(indexUse);

		// get indices pointer
		u16* indices = (u16*)m_indices->getIndices();
		indices += numIndices;

		float texWidth = 512.0f;
		float texHeight = 512.0f;

		if (tex)
		{
			texWidth = (float)tex->getSize().Width;
			texHeight = (float)tex->getSize().Height;
		}

		module->getPositionBuffer(vertices, indices, numVertices, offsetX, offsetY, absoluteMatrix);
		module->getTexCoordBuffer(vertices, texWidth, texHeight);
		module->getColorBuffer(vertices, color);

		if (material != NULL)
			flushWithMaterial(material);
	}

	void CGraphics2D::addModuleBatch(SModuleOffset* module,
		const SColor& color,
		const core::matrix4& absoluteMatrix,
		const core::rectf& r,
		int shaderID,
		CMaterial* material)
	{
		ITexture* tex = module->Frame->Image->Texture;

		if (m_2dMaterial.getTexture(0) != tex || m_2dMaterial.MaterialType != shaderID || material != NULL)
			flush();

		int numSpriteVertex = 4;

		int numVertices = m_vertices->getVertexCount();
		int vertexUse = numVertices + numSpriteVertex;

		int numSpriteIndex = 6;
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

		m_2dMaterial.setTexture(0, tex);
		m_2dMaterial.MaterialType = shaderID;

		// alloc vertex
		m_vertices->set_used(vertexUse);

		// get vertex pointer
		video::S3DVertex* vertices = (video::S3DVertex*)m_vertices->getVertices();
		vertices += numVertices;

		// alloc index
		m_indices->set_used(indexUse);

		// get indices pointer
		u16* indices = (u16*)m_indices->getIndices();
		indices += numIndices;

		float texWidth = 512.0f;
		float texHeight = 512.0f;

		if (tex)
		{
			texWidth = (float)tex->getSize().Width;
			texHeight = (float)tex->getSize().Height;
		}

		{
			u16* ib = indices;
			video::S3DVertex* vb = vertices;
			int vertex = numVertices;

			ib[0] = vertex;
			ib[1] = vertex + 1;
			ib[2] = vertex + 2;
			ib[3] = vertex;
			ib[4] = vertex + 2;
			ib[5] = vertex + 3;

			vb[0].Color = color;
			vb[1].Color = color;
			vb[2].Color = color;
			vb[3].Color = color;
		}

		SModuleRect* m = module->Module;
		float width = r.getWidth();
		float height = r.getHeight();

		// [0]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner;
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(width, height);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X, m->Y);
			uv.LowerRightCorner = core::vector2df(m->X + m->W, m->Y + m->H);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		if (material != NULL)
			flushWithMaterial(material);
	}

	void CGraphics2D::addModuleBatchLR(SModuleOffset* module,
		const SColor& color,
		const core::matrix4& absoluteMatrix,
		const core::rectf& r,
		float anchorLeft,
		float anchorRight,
		int shaderID,
		CMaterial* material)
	{
		ITexture* tex = module->Frame->Image->Texture;

		if (m_2dMaterial.getTexture(0) != tex || m_2dMaterial.MaterialType != shaderID || material != NULL)
			flush();

		int numRect = 3;

		int numSpriteVertex = 4 * numRect;

		int numVertices = m_vertices->getVertexCount();
		int vertexUse = numVertices + numSpriteVertex;

		int numSpriteIndex = 6 * numRect;
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

		m_2dMaterial.setTexture(0, tex);
		m_2dMaterial.MaterialType = shaderID;

		// alloc vertex
		m_vertices->set_used(vertexUse);

		// get vertex pointer
		video::S3DVertex* vertices = (video::S3DVertex*)m_vertices->getVertices();
		vertices += numVertices;

		// alloc index
		m_indices->set_used(indexUse);

		// get indices pointer
		u16* indices = (u16*)m_indices->getIndices();
		indices += numIndices;

		float texWidth = 512.0f;
		float texHeight = 512.0f;

		if (tex)
		{
			texWidth = (float)tex->getSize().Width;
			texHeight = (float)tex->getSize().Height;
		}

		for (int i = 0; i < numRect; i++)
		{
			u16* ib = indices + i * 6;
			video::S3DVertex* vb = vertices + i * 4;
			int vertex = numVertices + i * 4;

			ib[0] = vertex;
			ib[1] = vertex + 1;
			ib[2] = vertex + 2;
			ib[3] = vertex;
			ib[4] = vertex + 2;
			ib[5] = vertex + 3;

			vb[0].Color = color;
			vb[1].Color = color;
			vb[2].Color = color;
			vb[3].Color = color;
		}

		/*
		*   [0][   1    ][2]
		*/

		SModuleRect* m = module->Module;
		float width = r.getWidth();
		float height = r.getHeight();
		float right = m->W - anchorRight;

		// [0]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner;
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(anchorLeft, height);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X, m->Y);
			uv.LowerRightCorner = core::vector2df(m->X + anchorLeft, m->Y + m->H);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		// [1]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner + core::vector2df(anchorLeft, 0.0f);
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(width - right, height);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X + anchorLeft, m->Y);
			uv.LowerRightCorner = core::vector2df(m->X + m->W - right, m->Y + m->H);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		// [2]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner + core::vector2df(width - right, 0.0f);
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(width, height);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X + m->W - right, m->Y);
			uv.LowerRightCorner = core::vector2df(m->X + m->W, m->Y + m->H);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		if (material != NULL)
			flushWithMaterial(material);
	}

	void CGraphics2D::addModuleBatchTB(SModuleOffset* module,
		const SColor& color,
		const core::matrix4& absoluteMatrix,
		const core::rectf& r,
		float anchorTop,
		float anchorBottom,
		int shaderID,
		CMaterial* material)
	{
		ITexture* tex = module->Frame->Image->Texture;

		if (m_2dMaterial.getTexture(0) != tex || m_2dMaterial.MaterialType != shaderID || material != NULL)
			flush();

		int numRect = 3;

		int numSpriteVertex = 4 * numRect;

		int numVertices = m_vertices->getVertexCount();
		int vertexUse = numVertices + numSpriteVertex;

		int numSpriteIndex = 6 * numRect;
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

		m_2dMaterial.setTexture(0, tex);
		m_2dMaterial.MaterialType = shaderID;

		// alloc vertex
		m_vertices->set_used(vertexUse);

		// get vertex pointer
		video::S3DVertex* vertices = (video::S3DVertex*)m_vertices->getVertices();
		vertices += numVertices;

		// alloc index
		m_indices->set_used(indexUse);

		// get indices pointer
		u16* indices = (u16*)m_indices->getIndices();
		indices += numIndices;

		float texWidth = 512.0f;
		float texHeight = 512.0f;

		if (tex)
		{
			texWidth = (float)tex->getSize().Width;
			texHeight = (float)tex->getSize().Height;
		}

		for (int i = 0; i < numRect; i++)
		{
			u16* ib = indices + i * 6;
			video::S3DVertex* vb = vertices + i * 4;
			int vertex = numVertices + i * 4;

			ib[0] = vertex;
			ib[1] = vertex + 1;
			ib[2] = vertex + 2;
			ib[3] = vertex;
			ib[4] = vertex + 2;
			ib[5] = vertex + 3;

			vb[0].Color = color;
			vb[1].Color = color;
			vb[2].Color = color;
			vb[3].Color = color;
		}

		/*
		*   [       0       ]
		*   [       1       ]
		*   [       2       ]
		*/

		SModuleRect* m = module->Module;
		float width = r.getWidth();
		float height = r.getHeight();
		float bottom = m->H - anchorBottom;

		// [0]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner;
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(width, anchorTop);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X, m->Y);
			uv.LowerRightCorner = core::vector2df(m->X + m->W, m->Y + anchorTop);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		// [1]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner + core::vector2df(0.0, anchorTop);
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(width, height - bottom);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X, m->Y + anchorTop);
			uv.LowerRightCorner = core::vector2df(m->X + m->W, m->Y + m->H - bottom);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		// [2]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner + core::vector2df(0.0f, height - bottom);
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(width, height);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X, m->Y + m->H - bottom);
			uv.LowerRightCorner = core::vector2df(m->X + m->W, m->Y + m->H);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		if (material != NULL)
			flushWithMaterial(material);
	}

	void CGraphics2D::addModuleBatch(SModuleOffset* module,
		const SColor& color,
		const core::matrix4& absoluteMatrix,
		const core::rectf& r,
		float anchorLeft,
		float anchorRight,
		float anchorTop,
		float anchorBottom,
		int shaderID,
		CMaterial* material)
	{
		ITexture* tex = module->Frame->Image->Texture;

		if (m_2dMaterial.getTexture(0) != tex || m_2dMaterial.MaterialType != shaderID || material != NULL)
			flush();

		int numRect = 9;

		int numSpriteVertex = 4 * numRect;

		int numVertices = m_vertices->getVertexCount();
		int vertexUse = numVertices + numSpriteVertex;

		int numSpriteIndex = 6 * numRect;
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

		m_2dMaterial.setTexture(0, tex);
		m_2dMaterial.MaterialType = shaderID;

		// alloc vertex
		m_vertices->set_used(vertexUse);

		// get vertex pointer
		video::S3DVertex* vertices = (video::S3DVertex*)m_vertices->getVertices();
		vertices += numVertices;

		// alloc index
		m_indices->set_used(indexUse);

		// get indices pointer
		u16* indices = (u16*)m_indices->getIndices();
		indices += numIndices;

		float texWidth = 512.0f;
		float texHeight = 512.0f;

		if (tex)
		{
			texWidth = (float)tex->getSize().Width;
			texHeight = (float)tex->getSize().Height;
		}

		for (int i = 0; i < numRect; i++)
		{
			u16* ib = indices + i * 6;
			video::S3DVertex* vb = vertices + i * 4;
			int vertex = numVertices + i * 4;

			ib[0] = vertex;
			ib[1] = vertex + 1;
			ib[2] = vertex + 2;
			ib[3] = vertex;
			ib[4] = vertex + 2;
			ib[5] = vertex + 3;

			vb[0].Color = color;
			vb[1].Color = color;
			vb[2].Color = color;
			vb[3].Color = color;
		}

		/*
		*   [0][   1    ][2]
		*   [3][   4    ][5]
		*   [6][   7    ][8]
		*/

		SModuleRect* m = module->Module;
		float width = r.getWidth();
		float height = r.getHeight();
		float right = m->W - anchorRight;
		float bottom = m->H - anchorBottom;

		// [0]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner;
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(anchorLeft, anchorTop);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X, m->Y);
			uv.LowerRightCorner = core::vector2df(m->X + anchorLeft, m->Y + anchorTop);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		// [1]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner + core::vector2df(anchorLeft, 0.0f);
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(width - right, anchorTop);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X + anchorLeft, m->Y);
			uv.LowerRightCorner = core::vector2df(m->X + m->W - right, m->Y + anchorTop);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		// [2]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner + core::vector2df(width - right, 0.0f);
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(width, anchorTop);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X + m->W - right, m->Y);
			uv.LowerRightCorner = core::vector2df(m->X + m->W, m->Y + anchorTop);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		// [3]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner + core::vector2df(0.0f, anchorTop);
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(anchorLeft, height - bottom);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X, m->Y + anchorTop);
			uv.LowerRightCorner = core::vector2df(m->X + anchorLeft, m->Y + m->H - bottom);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		// [4]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner + core::vector2df(anchorLeft, anchorTop);
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(width - right, height - bottom);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X + anchorLeft, m->Y + anchorTop);
			uv.LowerRightCorner = core::vector2df(m->X + m->W - right, m->Y + m->H - bottom);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		// [5]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner + core::vector2df(width - right, anchorTop);
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(width, height - bottom);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X + m->W - right, m->Y + anchorTop);
			uv.LowerRightCorner = core::vector2df(m->X + m->W, m->Y + m->H - bottom);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		// [6]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner + core::vector2df(0.0f, height - bottom);
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(anchorLeft, height);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X, m->Y + m->H - bottom);
			uv.LowerRightCorner = core::vector2df(m->X + anchorLeft, m->Y + m->H);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		// [7]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner + core::vector2df(anchorLeft, height - bottom);
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(width - right, height);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X + anchorLeft, m->Y + m->H - bottom);
			uv.LowerRightCorner = core::vector2df(m->X + m->W - right, m->Y + m->H);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		// [8]
		{
			// position
			core::rectf rect;
			rect.UpperLeftCorner = r.UpperLeftCorner + core::vector2df(width - right, height - bottom);
			rect.LowerRightCorner = r.UpperLeftCorner + core::vector2df(width, height);
			updateRectBuffer(vertices, rect, absoluteMatrix);

			// uv
			core::rectf uv;
			uv.UpperLeftCorner = core::vector2df(m->X + m->W - right, m->Y + m->H - bottom);
			uv.LowerRightCorner = core::vector2df(m->X + m->W, m->Y + m->H);
			updateRectTexcoordBuffer(vertices, uv, texWidth, texHeight, m);

			vertices += 4;
			indices += 6;
		}

		if (material != NULL)
			flushWithMaterial(material);
	}

	void CGraphics2D::updateRectBuffer(video::S3DVertex* vertices, const core::rectf& r, const core::matrix4& mat)
	{
		float x1 = (float)r.UpperLeftCorner.X;
		float y1 = (float)r.UpperLeftCorner.Y;
		float x2 = (float)r.LowerRightCorner.X;
		float y2 = (float)r.LowerRightCorner.Y;

		vertices[0].Pos.X = x1;
		vertices[0].Pos.Y = y1;
		vertices[0].Pos.Z = 0.0f;
		mat.transformVect(vertices[0].Pos);

		vertices[1].Pos.X = x2;
		vertices[1].Pos.Y = y1;
		vertices[1].Pos.Z = 0.0f;
		mat.transformVect(vertices[1].Pos);

		vertices[2].Pos.X = x2;
		vertices[2].Pos.Y = y2;
		vertices[2].Pos.Z = 0.0f;
		mat.transformVect(vertices[2].Pos);

		vertices[3].Pos.X = x1;
		vertices[3].Pos.Y = y2;
		vertices[3].Pos.Z = 0.0f;
		mat.transformVect(vertices[3].Pos);
	}

	void CGraphics2D::updateRectTexcoordBuffer(video::S3DVertex* vertices, const core::rectf& r, float texWidth, float texHeight, SModuleRect* moduleRect)
	{
		float x1 = r.UpperLeftCorner.X / texWidth;
		float y1 = r.UpperLeftCorner.Y / texHeight;
		float x2 = r.LowerRightCorner.X / texWidth;
		float y2 = r.LowerRightCorner.Y / texHeight;

		vertices[0].TCoords.X = x1;
		vertices[0].TCoords.Y = y1;

		vertices[1].TCoords.X = x2;
		vertices[1].TCoords.Y = y1;

		vertices[2].TCoords.X = x2;
		vertices[2].TCoords.Y = y2;

		vertices[3].TCoords.X = x1;
		vertices[3].TCoords.Y = y2;

		if (moduleRect && moduleRect->W > 0 && moduleRect->H > 0)
		{
			vertices[0].Normal.X = (r.UpperLeftCorner.X - moduleRect->X) / moduleRect->W;
			vertices[0].Normal.Y = (r.UpperLeftCorner.Y - moduleRect->Y) / moduleRect->H;

			vertices[1].Normal.X = (r.LowerRightCorner.X - moduleRect->X) / moduleRect->W;
			vertices[1].Normal.Y = (r.UpperLeftCorner.Y - moduleRect->Y) / moduleRect->H;

			vertices[2].Normal.X = (r.LowerRightCorner.X - moduleRect->X) / moduleRect->W;
			vertices[2].Normal.Y = (r.LowerRightCorner.Y - moduleRect->Y) / moduleRect->H;

			vertices[3].Normal.X = (r.UpperLeftCorner.X - moduleRect->X) / moduleRect->W;
			vertices[3].Normal.Y = (r.LowerRightCorner.Y - moduleRect->Y) / moduleRect->H;
		}
	}

	void CGraphics2D::addFrameBatch(SFrame* frame, const SColor& color, const core::matrix4& absoluteMatrix, int materialID, CMaterial* material)
	{
		if (m_2dMaterial.getTexture(0) != frame->Image->Texture || m_2dMaterial.MaterialType != materialID || material != NULL)
			flush();

		int numSpriteVertex = (int)frame->ModuleOffset.size() * 4;

		int numVertices = m_vertices->getVertexCount();
		int vertexUse = numVertices + numSpriteVertex;

		int numSpriteIndex = (int)frame->ModuleOffset.size() * 6;
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

		m_2dMaterial.setTexture(0, frame->Image->Texture);
		m_2dMaterial.MaterialType = materialID;

		m_vertices->set_used(vertexUse);

		video::S3DVertex* vertices = (video::S3DVertex*)m_vertices->getVertices();
		vertices += numVertices;

		m_indices->set_used(indexUse);

		u16* indices = (u16*)m_indices->getIndices();
		indices += numIndices;

		float texWidth = 512.0f;
		float texHeight = 512.0f;

		if (frame->Image->Texture)
		{
			core::dimension2du size = frame->Image->Texture->getSize();
			texWidth = (float)size.Width;
			texHeight = (float)size.Height;
		}

		std::vector<SModuleOffset>::iterator it = frame->ModuleOffset.begin(), end = frame->ModuleOffset.end();
		while (it != end)
		{
			SModuleOffset& module = (*it);

			module.getPositionBuffer(vertices, indices, numVertices, absoluteMatrix);
			module.getTexCoordBuffer(vertices, texWidth, texHeight);
			module.getColorBuffer(vertices, color);

			numVertices += 4;
			vertices += 4;
			indices += 6;

			++it;
		}

		if (material != NULL)
			flushWithMaterial(material);
	}

	void CGraphics2D::addRectangleBatch(const core::rectf& pos, const core::rectf& uv, const SColor& color, const core::matrix4& absoluteTransform, int shaderID, CMaterial* material)
	{
		if (m_2dMaterial.MaterialType != shaderID || material != NULL)
			flush();

		int numVertices = m_vertices->getVertexCount();
		int vertexUse = numVertices + 4;

		int numIndices = m_indices->getIndexCount();
		int indexUse = numIndices + 6;

		m_indices->set_used(indexUse);
		u16* index = (u16*)m_indices->getIndices();
		index[numIndices + 0] = numVertices + 0;
		index[numIndices + 1] = numVertices + 1;
		index[numIndices + 2] = numVertices + 2;
		index[numIndices + 3] = numVertices + 0;
		index[numIndices + 4] = numVertices + 2;
		index[numIndices + 5] = numVertices + 3;

		float u1 = uv.UpperLeftCorner.X;
		float v1 = uv.UpperLeftCorner.Y;
		float u2 = uv.LowerRightCorner.X;
		float v2 = uv.LowerRightCorner.Y;

		m_vertices->set_used(vertexUse);
		S3DVertex* vertices = (S3DVertex*)m_vertices->getVertices();
		vertices[numVertices + 0] = S3DVertex(pos.UpperLeftCorner.X, pos.UpperLeftCorner.Y, 0.0f, 0.0f, 0.0f, 1.0f, color, u1, v1);
		vertices[numVertices + 1] = S3DVertex(pos.LowerRightCorner.X, pos.UpperLeftCorner.Y, 0.0f, 1.0f, 0.0f, 1.0f, color, u2, v1);
		vertices[numVertices + 2] = S3DVertex(pos.LowerRightCorner.X, pos.LowerRightCorner.Y, 0.0f, 1.0f, 1.0f, 1.0f, color, u2, v2);
		vertices[numVertices + 3] = S3DVertex(pos.UpperLeftCorner.X, pos.LowerRightCorner.Y, 0.0f, 0.0f, 1.0f, 1.0f, color, u1, v2);

		// transform
		for (int i = 0; i < 4; i++)
			absoluteTransform.transformVect(vertices[numVertices + i].Pos);

		m_2dMaterial.MaterialType = shaderID;
		m_driver->setMaterial(m_2dMaterial);

		m_buffer->setPrimitiveType(scene::EPT_TRIANGLES);
		m_buffer->setDirty();

		if (material != NULL)
			flushWithMaterial(material);
	}

	void CGraphics2D::beginDrawDepth()
	{
		flush();

		m_driver->clearZBuffer();

		setWriteDepth(m_2dMaterial);

		m_driver->setAllowZWriteOnTransparent(true);
	}

	void CGraphics2D::endDrawDepth()
	{
		flush();

		setNoDepthTest(m_2dMaterial);

		m_driver->setAllowZWriteOnTransparent(false);
	}

	void CGraphics2D::beginDepthTest()
	{
		setDepthTest(m_2dMaterial);
	}

	void CGraphics2D::endDepthTest()
	{
		flush();
		setNoDepthTest(m_2dMaterial);
	}

	void CGraphics2D::setWriteDepth(video::SMaterial& mat)
	{
		mat.ZBuffer = video::ECFN_ALWAYS;
		mat.ZWriteEnable = true;
		mat.ColorMask = ECP_NONE;
	}

	void CGraphics2D::setDepthTest(video::SMaterial& mat)
	{
		mat.ZBuffer = video::ECFN_GREATEREQUAL;
		mat.ZWriteEnable = false;
		mat.ColorMask = ECP_ALL;
	}

	void CGraphics2D::setNoDepthTest(video::SMaterial& mat)
	{
		mat.ZBuffer = video::ECFN_ALWAYS;
		mat.ZWriteEnable = false;
		mat.ColorMask = ECP_ALL;
	}

	void CGraphics2D::draw2DTriangle(
		const core::position2df& a,
		const core::position2df& b,
		const core::position2df& c,
		const SColor& color)
	{
		flush();

		m_indices->set_used(0);
		m_indices->addIndex(0);
		m_indices->addIndex(1);
		m_indices->addIndex(2);

		video::S3DVertex vert;
		vert.Color = color;

		vert.Pos.X = a.X;
		vert.Pos.Y = a.Y;
		vert.Pos.Z = 0;

		m_vertices->set_used(0);
		m_vertices->addVertex(vert);

		vert.Pos.X = b.X;
		vert.Pos.Y = b.Y;
		vert.Pos.Z = 0;
		m_vertices->addVertex(vert);

		vert.Pos.X = c.X;
		vert.Pos.Y = c.Y;
		vert.Pos.Z = 0;
		m_vertices->addVertex(vert);

		m_2dMaterial.setTexture(0, NULL);
		m_2dMaterial.setTexture(1, NULL);
		m_2dMaterial.MaterialType = m_vertexColorShader;
		m_driver->setMaterial(m_2dMaterial);

		m_buffer->setPrimitiveType(scene::EPT_TRIANGLES);
		m_buffer->setDirty();

		m_driver->drawMeshBuffer(m_buffer);

		m_indices->set_used(0);
		m_vertices->set_used(0);
	}

	void CGraphics2D::draw2DLines(const std::vector<core::position2df>& points, const SColor& color)
	{
		flush();

		m_indices->set_used(0);
		m_vertices->set_used(0);

		video::S3DVertex vert;
		int idx = 0;

		for (int i = 0, n = (int)points.size() - 1; i < n; i++)
		{
			vert.Color = color;

			vert.Pos.X = points[i].X;
			vert.Pos.Y = points[i].Y;
			vert.Pos.Z = 0;
			m_vertices->addVertex(vert);

			vert.Pos.X = points[i + 1].X;
			vert.Pos.Y = points[i + 1].Y;
			vert.Pos.Z = 0;
			m_vertices->addVertex(vert);

			m_indices->addIndex(idx++);
			m_indices->addIndex(idx++);
		}

		m_2dMaterial.setTexture(0, NULL);
		m_2dMaterial.setTexture(1, NULL);
		m_2dMaterial.MaterialType = m_vertexColorShader;
		m_driver->setMaterial(m_2dMaterial);

		m_buffer->setPrimitiveType(scene::EPT_LINES);
		m_buffer->setDirty();

		m_driver->drawMeshBuffer(m_buffer);

		m_indices->set_used(0);
		m_vertices->set_used(0);
	}

	void CGraphics2D::draw2DLine(const core::position2df& start, const core::position2df& end, const SColor& color)
	{
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

		m_indices->set_used(0);
		m_vertices->set_used(0);
	}

	void CGraphics2D::draw2DRectangle(const core::rectf& pos, const SColor& color)
	{
		flush();

		m_indices->set_used(6);
		u16* index = (u16*)m_indices->getIndices();
		index[0] = 0;
		index[1] = 1;
		index[2] = 2;
		index[3] = 0;
		index[4] = 2;
		index[5] = 3;

		m_vertices->set_used(4);
		S3DVertex* vertices = (S3DVertex*)m_vertices->getVertices();
		vertices[0] = S3DVertex(pos.UpperLeftCorner.X, pos.UpperLeftCorner.Y, 0.0f, 0.0f, 0.0f, 1.0f, color, 0, 0);
		vertices[1] = S3DVertex(pos.LowerRightCorner.X, pos.UpperLeftCorner.Y, 0.0f, 0.0f, 0.0f, 1.0f, color, 0, 0);
		vertices[2] = S3DVertex(pos.LowerRightCorner.X, pos.LowerRightCorner.Y, 0.0f, 0.0f, 0.0f, 1.0f, color, 0, 0);
		vertices[3] = S3DVertex(pos.UpperLeftCorner.X, pos.LowerRightCorner.Y, 0.0f, 0.0f, 0.0f, 1.0f, color, 0, 0);

		m_2dMaterial.setTexture(0, NULL);
		m_2dMaterial.setTexture(1, NULL);
		m_2dMaterial.MaterialType = m_vertexColorShader;
		m_driver->setMaterial(m_2dMaterial);

		m_buffer->setPrimitiveType(scene::EPT_TRIANGLES);
		m_buffer->setDirty();

		m_driver->drawMeshBuffer(m_buffer);

		m_indices->set_used(0);
		m_vertices->set_used(0);
	}

	void CGraphics2D::draw2DRectangle(const core::vector3df& upleft, const core::vector3df& lowerright, const SColor& color)
	{
		flush();

		m_indices->set_used(6);
		u16* index = (u16*)m_indices->getIndices();
		index[0] = 0;
		index[1] = 1;
		index[2] = 2;
		index[3] = 0;
		index[4] = 2;
		index[5] = 3;

		m_vertices->set_used(4);
		S3DVertex* vertices = (S3DVertex*)m_vertices->getVertices();
		vertices[0] = S3DVertex(upleft.X, upleft.Y, upleft.Z, 0, 0, 1, color, 0, 0);
		vertices[1] = S3DVertex(lowerright.X, upleft.Y, upleft.Z, 0, 0, 1, color, 0, 0);
		vertices[2] = S3DVertex(lowerright.X, lowerright.Y, lowerright.Z, 0, 0, 1, color, 0, 0);
		vertices[3] = S3DVertex(upleft.X, lowerright.Y, upleft.Z, 0, 0, 1, color, 0, 0);

		m_2dMaterial.setTexture(0, NULL);
		m_2dMaterial.setTexture(1, NULL);
		m_2dMaterial.MaterialType = m_vertexColorShader;
		m_driver->setMaterial(m_2dMaterial);

		m_buffer->setPrimitiveType(scene::EPT_TRIANGLES);
		m_buffer->setDirty();

		m_driver->drawMeshBuffer(m_buffer);

		m_indices->set_used(0);
		m_vertices->set_used(0);
	}

	void CGraphics2D::draw2DRectangleOutline(const core::vector3df& upleft, const core::vector3df& lowerright, const SColor& color)
	{
		flush();

		m_indices->set_used(5);
		u16* index = (u16*)m_indices->getIndices();
		index[0] = 0;
		index[1] = 1;
		index[2] = 2;
		index[3] = 3;
		index[4] = 0;

		m_vertices->set_used(4);
		S3DVertex* vertices = (S3DVertex*)m_vertices->getVertices();
		vertices[0] = S3DVertex(upleft.X, upleft.Y, upleft.Z, 0, 0, 1, color, 0, 0);
		vertices[1] = S3DVertex(lowerright.X, upleft.Y, upleft.Z, 0, 0, 1, color, 0, 0);
		vertices[2] = S3DVertex(lowerright.X, lowerright.Y, lowerright.Z, 0, 0, 1, color, 0, 0);
		vertices[3] = S3DVertex(upleft.X, lowerright.Y, upleft.Z, 0, 0, 1, color, 0, 0);

		m_2dMaterial.setTexture(0, NULL);
		m_2dMaterial.setTexture(1, NULL);
		m_2dMaterial.MaterialType = m_vertexColorShader;
		m_driver->setMaterial(m_2dMaterial);

		m_buffer->setPrimitiveType(scene::EPT_LINE_STRIP);
		m_buffer->setDirty();

		m_driver->drawMeshBuffer(m_buffer);

		m_indices->set_used(0);
		m_vertices->set_used(0);
	}

	void CGraphics2D::draw2DRectangleOutline(const core::rectf& pos, const SColor& color)
	{
		flush();

		m_indices->set_used(5);
		u16* index = (u16*)m_indices->getIndices();
		index[0] = 0;
		index[1] = 1;
		index[2] = 2;
		index[3] = 3;
		index[4] = 0;

		m_vertices->set_used(4);
		S3DVertex* vertices = (S3DVertex*)m_vertices->getVertices();
		vertices[0] = S3DVertex(pos.UpperLeftCorner.X, pos.UpperLeftCorner.Y, 0.0f, 0.0f, 0.0f, 1.0f, color, 0, 0);
		vertices[1] = S3DVertex(pos.LowerRightCorner.X, pos.UpperLeftCorner.Y, 0.0f, 0.0f, 0.0f, 1.0f, color, 0, 0);
		vertices[2] = S3DVertex(pos.LowerRightCorner.X, pos.LowerRightCorner.Y, 0.0f, 0.0f, 0.0f, 1.0f, color, 0, 0);
		vertices[3] = S3DVertex(pos.UpperLeftCorner.X, pos.LowerRightCorner.Y, 0.0f, 0.0f, 0.0f, 1.0f, color, 0, 0);

		m_2dMaterial.setTexture(0, NULL);
		m_2dMaterial.setTexture(1, NULL);
		m_2dMaterial.MaterialType = m_vertexColorShader;
		m_driver->setMaterial(m_2dMaterial);

		m_buffer->setPrimitiveType(scene::EPT_LINE_STRIP);
		m_buffer->setDirty();

		m_driver->drawMeshBuffer(m_buffer);

		m_indices->set_used(0);
		m_vertices->set_used(0);
	}

	void CGraphics2D::drawText(const core::position2df& pos, CGlyphFont* font, const SColor& color, const std::wstring& string, int materialID, CMaterial* material)
	{
		std::vector<SModuleOffset*> modules;
		const wchar_t* lpString = string.c_str();

		int i = 0, n = 0;
		float charSpacePadding = 0.0f;
		float charPadding = 0.0f;
		float x = (float)pos.X;
		float y = (float)pos.Y;

		while (lpString[i] != 0)
		{
			if (lpString[i] != (int)'\n' &&
				lpString[i] != (int)'\r')
			{
				SModuleOffset* c = font->getCharacterModule((int)lpString[i]);
				if (c != NULL)
				{
					modules.push_back(c);
				}
			}
			i++;
		}

		for (i = 0, n = (int)modules.size(); i < n; i++)
		{
			SModuleOffset* moduleOffset = modules[i];

			addModuleBatch(moduleOffset, color, core::IdentityMatrix, (float)x, (float)y, materialID, material);

			if (moduleOffset->Character == ' ')
				x += ((int)moduleOffset->XAdvance + charSpacePadding);
			else
				x += ((int)moduleOffset->XAdvance + charPadding);
		}
	}

	float CGraphics2D::measureCharWidth(CGlyphFont* font, wchar_t c)
	{
		if (c == '\n' || c == '\r')
			return 0.0f;

		float charSpacePadding = 0.0f;
		float charPadding = 0.0f;

		SModuleOffset* module = font->getCharacterModule(c);

		float charWidth = 0.0f;
		if (module->Character == ' ')
			charWidth = module->XAdvance + charSpacePadding;
		else
			charWidth = module->XAdvance + charPadding;

		return charWidth;
	}

	core::dimension2df CGraphics2D::measureText(CGlyphFont* font, const std::wstring& string)
	{
		float stringWidth = 0.0f;
		float charSpacePadding = 0.0f;
		float charPadding = 0.0f;
		float textHeight = 0.0f;
		float textOffsetY = 0.0f;

		// get text height
		SModuleOffset* moduleCharA = font->getCharacterModule((int)'A');
		if (moduleCharA)
		{
			textHeight = moduleCharA->OffsetY + moduleCharA->Module->H;
			textOffsetY = moduleCharA->OffsetY;
		}

		// get text width
		std::vector<SModuleOffset*>	listModule;

		int i = 0, n;
		while (string[i] != 0)
		{
			if (string[i] != (int)'\n' &&
				string[i] != (int)'\r')
			{
				SModuleOffset* c = font->getCharacterModule((int)string[i]);
				if (c != NULL)
				{
					listModule.push_back(c);
				}
			}
			i++;
		}

		for (i = 0, n = (int)listModule.size(); i < n; i++)
		{
			SModuleOffset* moduleOffset = listModule[i];
			if (moduleOffset->Character == ' ')
				stringWidth = stringWidth + (moduleOffset->XAdvance + charSpacePadding);
			else
				stringWidth = stringWidth + (moduleOffset->XAdvance + charPadding);
		}

		return core::dimension2df(stringWidth, textHeight + textOffsetY);
	}
}