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
#include "CAnimationTimeline.h"

namespace Skylicht
{

	CAnimationTimeline::CAnimationTimeline() :
		From(0.0f),
		Frame(0.0f),
		Duration(0.0f),
		Speed(1.0f),
		Weight(1.0f),
		Pause(false)
	{

	}

	void CAnimationTimeline::update()
	{
		float milisecondToSecond = 1.0f / 1000.0f;
		if (Pause == false)
		{
			float secFrameStep = getTimeStep() * Speed * milisecondToSecond;

			// seek animation frame
			Frame = Frame + secFrameStep;

			// if end of animation
			if (Frame > To)
			{
				Frame = To;

				// if animation is loop
				if (Loop == true)
					Frame = From;
			}
		}
	}

}