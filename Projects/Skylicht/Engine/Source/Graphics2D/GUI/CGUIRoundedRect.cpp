/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "CGUIRoundedRect.h"
#include "Material/Shader/CShaderManager.h"
#include "Graphics2D/CGraphics2D.h"

namespace Skylicht
{
	CGUIRoundedRect::CGUIRoundedRect(CCanvas* canvas, CGUIElement* parent, float radius) :
		CGUIElement(canvas, parent),
		m_radius(radius),
		m_buffer(NULL),
		m_vertices(NULL),
		m_indices(NULL)
	{
		m_renderData->ShaderID = CShaderManager::getInstance()->getShaderIDByName("VertexColorAlpha");
		init();
	}

	CGUIRoundedRect::CGUIRoundedRect(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect, float radius) :
		CGUIElement(canvas, parent, rect),
		m_radius(radius),
		m_buffer(NULL),
		m_vertices(NULL),
		m_indices(NULL)
	{
		m_renderData->ShaderID = CShaderManager::getInstance()->getShaderIDByName("VertexColorAlpha");
		init();
	}

	CGUIRoundedRect::~CGUIRoundedRect()
	{
		if (m_buffer != NULL)
			m_buffer->drop();
	}

	void CGUIRoundedRect::init()
	{
		if (m_buffer == NULL)
		{
			m_buffer = new CMeshBuffer<S3DVertex>(getVideoDriver()->getVertexDescriptor(EVT_STANDARD), video::EIT_16BIT);
			m_buffer->setHardwareMappingHint(EHM_STREAM);

			m_vertices = (SVertexBuffer*)m_buffer->getVertexBuffer();
			m_indices = (CIndexBuffer*)m_buffer->getIndexBuffer();
		}

		m_vertices->set_used(0);
		m_indices->set_used(0);

		// https://stackoverflow.com/questions/5369507/opengl-es-1-0-2d-rounded-rectangle

		const int roundPointCount = 8;

		core::vector3df top_left[roundPointCount];
		core::vector3df bottom_left[roundPointCount];
		core::vector3df top_right[roundPointCount];
		core::vector3df bottom_right[roundPointCount];

		int i = 0;
		float width = getRect().getWidth();
		float height = getRect().getHeight();
		float x = 0.0f;
		float y = height;

		float x_offset, y_offset;
		float step = (2.0f * core::PI) / (roundPointCount * 4);
		float angle = 0.0f;

		float radius = m_radius;
		radius = core::min_(radius, width * 0.5f);
		radius = core::min_(radius, height * 0.5f);

		core::vector3df bottom_left_corner(x + radius, y - height + radius, 0.0f);

		while (i != roundPointCount)
		{
			x_offset = cosf(angle);
			y_offset = sinf(angle);

			top_left[i].X = bottom_left_corner.X - (x_offset * radius);
			top_left[i].Y = (height - (radius * 2.0f)) + bottom_left_corner.Y - (y_offset * radius);

			top_right[i].X = (width - (radius * 2.0f)) + bottom_left_corner.X + (x_offset * radius);
			top_right[i].Y = (height - (radius * 2.0f)) + bottom_left_corner.Y - (y_offset * radius);

			bottom_right[i].X = (width - (radius * 2.0f)) + bottom_left_corner.X + (x_offset * radius);
			bottom_right[i].Y = bottom_left_corner.Y + (y_offset * radius);

			bottom_left[i].X = bottom_left_corner.X - (x_offset * radius);
			bottom_left[i].Y = bottom_left_corner.Y + (y_offset * radius);

			angle -= step;

			++i;
		}

		core::vector3df n(0.0f, 0.0f, 1.0f);
		core::vector2df t(0.0f, 0.0f);

		int idx = 0;

		SColor color = getColor();

		// top
		for (i = roundPointCount - 1; i >= 1; i -= 1)
		{
			m_vertices->addVertex(S3DVertex(top_right[i], n, color, t));
			m_vertices->addVertex(S3DVertex(top_left[i], n, color, t));
			m_vertices->addVertex(S3DVertex(top_left[i - 1], n, color, t));

			m_indices->addIndex(idx++);
			m_indices->addIndex(idx++);
			m_indices->addIndex(idx++);

			m_vertices->addVertex(S3DVertex(top_right[i], n, color, t));
			m_vertices->addVertex(S3DVertex(top_left[i - 1], n, color, t));
			m_vertices->addVertex(S3DVertex(top_right[i - 1], n, color, t));

			m_indices->addIndex(idx++);
			m_indices->addIndex(idx++);
			m_indices->addIndex(idx++);
		}

		// center
		m_vertices->addVertex(S3DVertex(top_right[0], n, color, t));
		m_vertices->addVertex(S3DVertex(top_left[0], n, color, t));
		m_vertices->addVertex(S3DVertex(bottom_left[0], n, color, t));

		m_indices->addIndex(idx++);
		m_indices->addIndex(idx++);
		m_indices->addIndex(idx++);

		m_vertices->addVertex(S3DVertex(top_right[0], n, color, t));
		m_vertices->addVertex(S3DVertex(bottom_left[0], n, color, t));
		m_vertices->addVertex(S3DVertex(bottom_right[0], n, color, t));

		m_indices->addIndex(idx++);
		m_indices->addIndex(idx++);
		m_indices->addIndex(idx++);

		// bottom
		for (i = 0; i < roundPointCount - 1; i++)
		{
			m_vertices->addVertex(S3DVertex(bottom_right[i], n, color, t));
			m_vertices->addVertex(S3DVertex(bottom_left[i], n, color, t));
			m_vertices->addVertex(S3DVertex(bottom_left[i + 1], n, color, t));

			m_indices->addIndex(idx++);
			m_indices->addIndex(idx++);
			m_indices->addIndex(idx++);

			m_vertices->addVertex(S3DVertex(bottom_right[i], n, color, t));
			m_vertices->addVertex(S3DVertex(bottom_left[i + 1], n, color, t));
			m_vertices->addVertex(S3DVertex(bottom_right[i + 1], n, color, t));

			m_indices->addIndex(idx++);
			m_indices->addIndex(idx++);
			m_indices->addIndex(idx++);
		}
	}

	void CGUIRoundedRect::render(CCamera* camera)
	{
		CGraphics2D* g = CGraphics2D::getInstance();
		g->addExternalBuffer(m_buffer, m_transform->World, getShaderID());

		CGUIElement::render(camera);
	}
}