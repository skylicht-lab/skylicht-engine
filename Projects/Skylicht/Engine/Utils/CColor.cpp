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
#include "CColor.h"

namespace Skylicht
{
	SColor CColor::toSRGB(const SColor& c)
	{
		float fr = pow((float)(c.getRed() / 255.0f), 2.2f);
		float fg = pow((float)(c.getGreen() / 255.0f), 2.2f);
		float fb = pow((float)(c.getBlue() / 255.0f), 2.2f);

		u32 r = core::clamp<u32>((u32)(fr * 255.0f), 0, 255);
		u32 g = core::clamp<u32>((u32)(fg * 255.0f), 0, 255);
		u32 b = core::clamp<u32>((u32)(fb * 255.0f), 0, 255);

		return SColor(c.getAlpha(), r, g, b);
	}

	SColor CColor::toLinear(const SColor& c)
	{
		float fr = pow((float)(c.getRed() / 255.0f), 1.0f / 2.2f);
		float fg = pow((float)(c.getGreen() / 255.0f), 1.0f / 2.2f);
		float fb = pow((float)(c.getBlue() / 255.0f), 1.0f / 2.2f);

		u32 r = core::clamp<u32>((u32)(fr * 255.0f), 0, 255);
		u32 g = core::clamp<u32>((u32)(fg * 255.0f), 0, 255);
		u32 b = core::clamp<u32>((u32)(fb * 255.0f), 0, 255);

		return SColor(c.getAlpha(), r, g, b);
	}

	SColor CColor::fromHex(const char* hex)
	{
		int r = 255, g = 255, b = 255, a = 255;
		if (strlen(hex) == 6)
		{
			sscanf(hex, "%02x%02x%02x", &r, &g, &b);
		}
		else if (strlen(hex) == 8)
		{
			sscanf(hex, "%02x%02x%02x%02x", &a, &r, &g, &b);
		}

		return SColor(a, r, g, b);
	}
}