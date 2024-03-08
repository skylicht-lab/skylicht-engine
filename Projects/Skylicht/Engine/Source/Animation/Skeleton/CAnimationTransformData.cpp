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
#include "CAnimationTransformData.h"

namespace Skylicht
{
	IMPLEMENT_DATA_TYPE_INDEX(CAnimationTransformData);

	CAnimationTransformData::CAnimationTransformData() :
		ParentID(-1),
		Depth(0),
		Weight(1.0f),
		DisableAnimation(false)
	{

	}

	CAnimationTransformData::~CAnimationTransformData()
	{

	}

	void CAnimationTransformData::updateTransform()
	{
		core::matrix4& relativeMatrix = WorldTransform->Relative;
		relativeMatrix.makeIdentity();

		// rotation
		AnimRotation.getMatrix(relativeMatrix);

		// position	
		f32* m1 = relativeMatrix.pointer();

		m1[12] = AnimPosition.X;
		m1[13] = AnimPosition.Y;
		m1[14] = AnimPosition.Z;

		// scale
		m1[0] *= AnimScale.X;
		m1[1] *= AnimScale.X;
		m1[2] *= AnimScale.X;
		m1[3] *= AnimScale.X;

		m1[4] *= AnimScale.Y;
		m1[5] *= AnimScale.Y;
		m1[6] *= AnimScale.Y;
		m1[7] *= AnimScale.Y;

		m1[8] *= AnimScale.Z;
		m1[9] *= AnimScale.Z;
		m1[10] *= AnimScale.Z;
		m1[11] *= AnimScale.Z;
	}
}