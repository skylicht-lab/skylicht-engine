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

namespace Skylicht
{
	/**
	 * @brief Manages the playback state of an animation (current time, speed, loop state).
	 * @ingroup Animation
	 * 
	 * Example: Controlling playback
	 * @code
	 * CAnimationTimeline& timeline = skeleton->getTimeline();
	 * timeline.Speed = 1.5f;   // Play faster
	 * timeline.Loop = false;   // Play once
	 * timeline.Pause = true;   // Pause
	 * 
	 * if (timeline.isEnd())
	 *     os::Printer::log("Animation finished!");
	 * @endcode
	 */
	class SKYLICHT_API CAnimationTimeline
	{
	public:
		//! Start frame of the playback range in seconds.
		float From;

		//! End frame of the playback range in seconds.
		float To;

		//! Total duration of the animation clip in seconds.
		float Duration;

		//! Current playback position in seconds.
		float Frame;

		//! Playback speed multiplier (default: 1.0).
		float Speed;

		//! Blending weight for this timeline (0 to 1).
		float Weight;

		//! Whether the animation should restart when it reaches the 'To' frame.
		bool Loop;

		//! Whether playback is currently paused.
		bool Pause;

	public:
		CAnimationTimeline();

		/**
		 * @brief Updates the current frame based on time step and speed.
		 */
		void update();

		/**
		 * @brief Starts or resumes playback.
		 * @param fromBegin If true, resets Frame to the 'From' position.
		 */
		void play(bool fromBegin = true);

		/**
		 * @brief Gets the duration of the current playback range (To - From).
		 * @return Range duration in seconds.
		 */
		inline float getAnimDuration()
		{
			return To - From;
		}

		/**
		 * @brief Gets the total duration of the source animation clip.
		 * @return Clip duration in seconds.
		 */
		inline float getClipDuration()
		{
			return Duration;
		}

		/**
		 * @brief Checks if the animation has reached or passed the 'To' frame.
		 * @return True if at the end.
		 */
		bool isEnd();
	};
}