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
	bool CProjective::getScreenCoordinatesFrom3DPosition(CCamera *camera, const core::vector3df& pos3d, float &x, float &y)
	{
		const core::rect<s32>& viewPort = getVideoDriver()->getViewPort();

		core::dimension2d<f32> dim(
			(f32)viewPort.getWidth() * 0.5f,
			(f32)viewPort.getHeight() * 0.5f
		);

		core::matrix4 trans = camera->getProjectionMatrix();
		trans *= camera->getViewMatrix();

		f32 transformedPos[4] = { pos3d.X, pos3d.Y, pos3d.Z, 1.0f };

		trans.multiplyWith1x4Matrix(transformedPos);

		f32 zDiv = transformedPos[3] == 0.0f ? 1.0f : core::reciprocal(transformedPos[3]);

		if (zDiv < 0)
			zDiv = -zDiv;

		x = dim.Width + dim.Width  * transformedPos[0] * zDiv;
		y = dim.Height - dim.Height * transformedPos[1] * zDiv;

		return transformedPos[3] >= 0;
	}
}