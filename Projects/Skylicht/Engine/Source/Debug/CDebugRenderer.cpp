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
#include "CDebugRenderer.h"
#include "CSceneDebug.h"

namespace Skylicht
{
	CDebugRenderer::CDebugRenderer()
	{
		m_drawData = new CLineDrawData();
		m_drawNoZData = new CLineDrawData();
		m_drawNoZData->LineBuffer->getMaterial().ZBuffer = ECFN_DISABLED;
	}

	CDebugRenderer::~CDebugRenderer()
	{
		delete m_drawData;
	}

	void CDebugRenderer::beginQuery(CEntityManager* entityManager)
	{

	}

	void CDebugRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{

	}

	void CDebugRenderer::init(CEntityManager* entityManager)
	{

	}

	void CDebugRenderer::update(CEntityManager* entityManager)
	{

	}

	void CDebugRenderer::render(CEntityManager* entityManager)
	{

	}

	void CDebugRenderer::postRender(CEntityManager* entityManager)
	{
		IVideoDriver* videoDriver = getVideoDriver();

		CSceneDebug* debug = CSceneDebug::getInstance();
		CSceneDebug* noZDebug = debug->getNoZDebug();

		m_drawData->clearBuffer();
		m_drawNoZData->clearBuffer();

		for (int i = 0, n = debug->getLinesCount(); i < n; i++)
		{
			const SLineDebug& line = debug->getLine(i);
			m_drawData->addLine(line.line.start, line.line.end, line.color);
		}

		for (int i = 0, n = noZDebug->getLinesCount(); i < n; i++)
		{
			const SLineDebug& line = noZDebug->getLine(i);
			m_drawNoZData->addLine(line.line.start, line.line.end, line.color);
		}

		for (int i = 0, n = debug->getBoxCount(); i < n; i++)
		{
			m_drawData->add3DBox(debug->getBox(i).box, debug->getBox(i).color);
		}

		for (int i = 0, n = noZDebug->getBoxCount(); i < n; i++)
		{
			m_drawNoZData->add3DBox(noZDebug->getBox(i).box, noZDebug->getBox(i).color);
		}

		for (int i = 0, n = debug->getTriCount(); i < n; i++)
		{
			core::vector3df normal = debug->getTri(i).tri.getNormal();
			normal.normalize();
			normal *= 0.02f; // move 2cm to fix the depth

			const core::triangle3df& tri = debug->getTri(i).tri;
			core::vector3df a = tri.pointA + normal;
			core::vector3df b = tri.pointB + normal;
			core::vector3df c = tri.pointC + normal;
			SColor color = debug->getTri(i).color;

			m_drawData->addLine(a, b, color);
			m_drawData->addLine(b, c, color);
			m_drawData->addLine(c, a, color);
		}

		for (int i = 0, n = noZDebug->getTriCount(); i < n; i++)
		{
			core::vector3df normal = noZDebug->getTri(i).tri.getNormal();
			normal.normalize();
			normal *= 0.02f; // move 2cm to fix the depth

			const core::triangle3df& tri = noZDebug->getTri(i).tri;
			core::vector3df a = tri.pointA + normal;
			core::vector3df b = tri.pointB + normal;
			core::vector3df c = tri.pointC + normal;
			SColor color = noZDebug->getTri(i).color;

			m_drawNoZData->addLine(a, b, color);
			m_drawNoZData->addLine(b, c, color);
			m_drawNoZData->addLine(c, a, color);
		}

		for (int i = 0; i < debug->getLineStripCount(); i++)
		{
			const core::array<core::vector3df>& listPoint = debug->getLineStrip(i).point;
			m_drawData->addPolyline(listPoint.const_pointer(), listPoint.size(), false, debug->getLineStrip(i).color);
		}

		for (int i = 0; i < noZDebug->getLineStripCount(); i++)
		{
			const core::array<core::vector3df>& listPoint = noZDebug->getLineStrip(i).point;
			m_drawNoZData->addPolyline(listPoint.const_pointer(), listPoint.size(), false, noZDebug->getLineStrip(i).color);
		}

		m_drawData->updateBuffer();
		m_drawNoZData->updateBuffer();

		// reset world transform
		videoDriver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		IMeshBuffer* buffer = m_drawData->LineBuffer;
		if (buffer->getPrimitiveCount() > 0)
		{
			videoDriver->setMaterial(buffer->getMaterial());
			videoDriver->drawMeshBuffer(buffer);
		}

		buffer = m_drawNoZData->LineBuffer;
		if (buffer->getPrimitiveCount() > 0)
		{
			videoDriver->setMaterial(buffer->getMaterial());
			videoDriver->drawMeshBuffer(buffer);
		}

		debug->clear();
		noZDebug->clear();

	}
}