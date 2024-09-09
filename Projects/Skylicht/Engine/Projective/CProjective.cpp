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
#include "CProjective.h"

namespace Skylicht
{
	bool CProjective::getScreenCoordinatesFrom3DPosition(CCamera* camera, const core::vector3df& pos3d, float& x, float& y, int viewportW, int viewportH)
	{
		core::dimension2d<f32> dim(
			(f32)viewportW * 0.5f,
			(f32)viewportH * 0.5f
		);

		core::matrix4 trans = camera->getProjectionMatrix();
		trans *= camera->getViewMatrix();

		f32 transformedPos[4] = { pos3d.X, pos3d.Y, pos3d.Z, 1.0f };

		trans.multiplyWith1x4Matrix(transformedPos);

		f32 zDiv = transformedPos[3] == 0.0f ? 1.0f : core::reciprocal(transformedPos[3]);

		if (zDiv < 0)
			zDiv = -zDiv;

		x = dim.Width + dim.Width * transformedPos[0] * zDiv;
		y = dim.Height - dim.Height * transformedPos[1] * zDiv;

		return transformedPos[3] >= 0;
	}

	core::line3df CProjective::getViewRay(CCamera* camera, float x, float y, int viewportW, int viewportH)
	{
		core::line3d<f32> ln(0, 0, 0, 0, 0, 0);

		const SViewFrustum& f = camera->getViewFrustum();

		core::vector3df farLeftUp = f.getFarLeftUp();
		core::vector3df lefttoright = f.getFarRightUp() - farLeftUp;
		core::vector3df uptodown = f.getFarLeftDown() - farLeftUp;

		f32 dx = x / (f32)viewportW;
		f32 dy = y / (f32)viewportH;

		if (camera->getProjectionType() == CCamera::Ortho || camera->getProjectionType() == CCamera::OrthoUI)
			ln.start = f.cameraPosition + (lefttoright * (dx - 0.5f)) + (uptodown * (dy - 0.5f));
		else
			ln.start = f.cameraPosition;

		ln.end = farLeftUp + (lefttoright * dx) + (uptodown * dy);
		return ln;
	}

	// References: https://github.com/CedricGuillemet/ImGuizmo/blob/master/ImGuizmo.cpp
	float CProjective::getSegmentLengthClipSpace(CCamera* camera, const core::vector3df& start, const core::vector3df& end)
	{
		core::matrix4 trans = getVideoDriver()->getTransform(video::ETS_VIEW_PROJECTION);

		f32 start4[4] = { start.X, start.Y, start.Z, 1.0f };
		f32 end4[4] = { end.X, end.Y, end.Z, 1.0f };

		trans.multiplyWith1x4Matrix(start4);
		core::vector3df startOfSegment(start4[0], start4[1], start4[2]);
		if (fabsf(start4[3]) > FLT_EPSILON) // check for axis aligned with camera direction
		{
			startOfSegment *= 1.f / start4[3];
		}

		trans.multiplyWith1x4Matrix(end4);
		core::vector3df endOfSegment(end4[0], end4[1], end4[2]);
		if (fabsf(end4[3]) > FLT_EPSILON) // check for axis aligned with camera direction
		{
			endOfSegment *= 1.f / end4[3];
		}

		core::vector3df clipSpaceAxis = endOfSegment - startOfSegment;
		clipSpaceAxis.Y /= camera->getAspect();
		float segmentLengthInClipSpace = sqrtf(clipSpaceAxis.X * clipSpaceAxis.X + clipSpaceAxis.Y * clipSpaceAxis.Y);
		return segmentLengthInClipSpace;
	}
}