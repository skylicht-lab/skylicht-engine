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
#include "CGridPlaneData.h"
#include "Utils/CColor.h"

namespace Skylicht
{
	IMPLEMENT_DATA_TYPE_INDEX(CGridPlaneData);

	CGridPlaneData::CGridPlaneData() :
		NumGrid(50),
		GridSize(1.0f)
	{
		init();
	}

	CGridPlaneData::~CGridPlaneData()
	{

	}

	void CGridPlaneData::init()
	{
		clearBuffer();

		SColor color;

		SColor grey = CColor::toSRGB(SColor(255, 70, 70, 70));
		SColor red = CColor::toSRGB(SColor(255, 155, 65, 80));
		SColor green = CColor::toSRGB(SColor(255, 105, 140, 45));

		core::vector3df start;
		core::vector3df end;

		float size = NumGrid * GridSize * 0.5f;

		start = core::vector3df(-size, 0, -size);
		end = core::vector3df(size, 0, -size);

		for (int i = 0; i <= NumGrid; i++)
		{
			if (i == NumGrid / 2)
				color = red;
			else
				color = grey;

			addLine(start, end, color);

			start.Z += GridSize;
			end.Z += GridSize;
		}

		start = core::vector3df(-size, 0, -size);
		end = core::vector3df(-size, 0, size);

		for (int i = 0; i <= NumGrid; i++)
		{
			if (i == NumGrid / 2)
				color = green;
			else
				color = grey;

			addLine(start, end, color);

			start.X += GridSize;
			end.X += GridSize;
		}

		updateBuffer();
	}
}
