/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CGUIHandlesRenderer.h"
#include "Handles/CGUIHandles.h"

namespace Skylicht
{
	namespace Editor
	{
		CGUIHandlesRenderer::CGUIHandlesRenderer()
		{

		}

		CGUIHandlesRenderer::~CGUIHandlesRenderer()
		{

		}

		void CGUIHandlesRenderer::render(CCamera* camera, float scale)
		{
			CGraphics2D* g = CGraphics2D::getInstance();
			CGUIHandles* handles = CGUIHandles::getInstance();

			core::vector3df pos = handles->getHandlePosition();
			handles->getWorld().transformVect(pos);

			core::quaternion worldRot(handles->getWorld());
			worldRot.normalize();

			core::quaternion rot = worldRot * handles->getHandleRotation();
			rot.normalize();

			if (handles->isHandlePosition())
			{
				drawTranslateGizmo(pos, rot);
			}
			else if (handles->isHandleRotation())
			{
				drawRotationGizmo(pos, rot);
			}
			else if (handles->isHandleScale())
			{
				drawScaleGizmo(pos, rot);
			}
		}

		void CGUIHandlesRenderer::drawRotationGizmo(const core::vector3df& pos, const core::quaternion& rot)
		{
		}

		void CGUIHandlesRenderer::drawScaleGizmo(const core::vector3df& pos, const core::quaternion& rot)
		{
		}

		void CGUIHandlesRenderer::drawTranslateGizmo(const core::vector3df& pos, const core::quaternion& rot)
		{
		}
	}
}