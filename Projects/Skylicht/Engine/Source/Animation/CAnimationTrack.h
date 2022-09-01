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
	enum EAnimationType
	{
		Position = 0,
		Rotation,
		Scale,
		NumAnimation
	};

	template<class T>
	class CKeyData
	{
	public:
		f32 Frame;
		T Value;
	};

	typedef CKeyData<core::vector3df> CPositionKey;
	typedef CKeyData<core::quaternion> CRotationKey;
	typedef CKeyData<core::vector3df> CScaleKey;

	class CAnimationData
	{
	public:
		core::array<CPositionKey> PositionKeys;
		core::array<CScaleKey> ScaleKeys;
		core::array<CRotationKey> RotationKeys;

		core::quaternion DefaultRot;
		core::vector3df DefaultPos;
		core::vector3df DefaultScale;

		CAnimationData()
		{
		}
	};

	class CAnimationTrack
	{
	protected:
		s32 m_posHint;
		s32 m_scaleHint;
		s32 m_rotHint;

		CAnimationData* m_data;

	public:
		std::string Name;
		bool HaveAnimation;

	public:
		CAnimationTrack();
		virtual ~CAnimationTrack();

		static void quaternionSlerp(core::quaternion& result, core::quaternion q1, core::quaternion q2, float t);

		void getFrameData(f32 frame,
			core::vector3df& position,
			core::vector3df& scale,
			core::quaternion& rotation);

		CAnimationData* getAnimData();

		void clearAllKeyFrame()
		{
			m_data = NULL;

			m_posHint = 0;
			m_scaleHint = 0;
			m_rotHint = 0;

			Name = "";
			HaveAnimation = false;
		}

		void setFrameData(CAnimationData* data)
		{
			m_data = data;
		}

		CAnimationData* getFrameData()
		{
			return m_data;
		}
	};
}