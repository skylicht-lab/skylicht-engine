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
	class CFrameData
	{
	public:
		struct SPositionKey
		{
			f32 frame;
			core::vector3df position;
		};

		struct SScaleKey
		{
			f32 frame;
			core::vector3df scale;
		};

		struct SRotationKey
		{
			f32 frame;
			core::quaternion rotation;
		};

		struct SEventKey
		{
			f32 frame;
			core::stringc event;
		};

		core::array<CFrameData::SPositionKey> PositionKeys;
		core::array<CFrameData::SScaleKey> ScaleKeys;
		core::array<CFrameData::SRotationKey> RotationKeys;
		core::array<CFrameData::SEventKey> EventKeys;

		core::quaternion DefaultRot;
		core::vector3df DefaultPos;
		core::vector3df DefaultScale;

		CFrameData()
		{
		}
	};

	class CAnimationTrack
	{
	protected:
		s32 m_posHint;
		s32 m_scaleHint;
		s32 m_rotHint;

		CFrameData *m_data;
	public:

		std::string Name;
		int TrackID;
		bool Enable;

	public:
		CAnimationTrack();
		virtual ~CAnimationTrack();

		static void quaternionSlerp(core::quaternion& result, core::quaternion q1, core::quaternion q2, float t);

		void getFrameData(f32 frame,
			core::vector3df &position,
			core::vector3df &scale,
			core::quaternion &rotation);

		CFrameData* getAnimData();

		void update(float timeStep);

		void clearAllKeyFrame()
		{
			m_data = NULL;

			// clear hint
			m_posHint = 0;
			m_scaleHint = 0;
			m_rotHint = 0;

			Name = "";

			// disable animation
			Enable = false;
		}

		void setFrameData(CFrameData *data)
		{
			m_data = data;
		}

		CFrameData* getFrameData()
		{
			return m_data;
		}
	};
}