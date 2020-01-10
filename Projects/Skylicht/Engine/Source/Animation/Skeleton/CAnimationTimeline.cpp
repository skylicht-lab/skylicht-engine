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

#pragma once

#include "pch.h"
#include "CAnimationTimeline.h"

namespace Skylicht
{

	CAnimationTimeline::CAnimationTimeline() :
		AnimationFrame(0.0f),
		AnimationDuration(0.0f),
		AnimationSpeed(1.0f),
		AnimationSpeedMultiply(1.0f),
		AnimationDurationSyncRatio(1.0f),
		AnimationWeight(1.0f),
		AnimationSleep(0.0f),
		EndTrackSleep(0.0f),
		SyncSeekRatio(0.0f),
		Pause(false)
	{

	}

	void CAnimationTimeline::update()
	{
		float milisecondToSecond = 1.0f / 1000.0f;
		if (Pause == false)
		{
			float secFrameStep = getTimeStep()*AnimationSpeed*AnimationSpeedMultiply*milisecondToSecond;

			// seek animation frame
			AnimationFrame = AnimationFrame + secFrameStep;

			// if end of animation
			if (AnimationFrame > AnimationDuration)
			{
				AnimationFrame = AnimationDuration;

				// if animation is loop
				if (AnimationLoop == true)
				{
					if (AnimationSleep > 0)
					{
						AnimationSleep = AnimationSleep - secFrameStep;
					}
					else
					{
						AnimationFrame = 0.0f;
						AnimationSleep = EndTrackSleep;
					}
				}
			}
		}
	}

}