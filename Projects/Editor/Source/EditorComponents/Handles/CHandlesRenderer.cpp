/*
!@
MIT License

CopyRight (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CHandlesRenderer.h"
#include "Handles/CHandles.h"

namespace Skylicht
{
	namespace Editor
	{
		CHandlesRenderer::CHandlesRenderer()
		{
			m_data = new CHandlesData();
		}

		CHandlesRenderer::~CHandlesRenderer()
		{
			delete m_data;
		}

		void CHandlesRenderer::beginQuery()
		{

		}

		void CHandlesRenderer::onQuery(CEntityManager* entityManager, CEntity* entity)
		{

		}

		void CHandlesRenderer::init(CEntityManager* entityManager)
		{

		}

		void CHandlesRenderer::update(CEntityManager* entityManager)
		{
			if (m_enable == false)
				return;

			m_data->clearBuffer();
		}

		void CHandlesRenderer::render(CEntityManager* entityManager)
		{
			if (m_enable == false)
				return;

			IVideoDriver* driver = getVideoDriver();
			driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

			// IMeshBuffer* buffer = m_data->Buffer;
			// driver->setMaterial(buffer->getMaterial());
			// driver->drawMeshBuffer(buffer);
		}
	}
}