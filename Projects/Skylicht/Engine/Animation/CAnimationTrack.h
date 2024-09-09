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
	template<class T>
	class SKYLICHT_API CKeyFrameData
	{
	public:
		f32 Frame; // Second
		T Value;
	};

	typedef CKeyFrameData<core::vector3df> CPositionKey;
	typedef CKeyFrameData<core::quaternion> CRotationKey;
	typedef CKeyFrameData<core::vector3df> CScaleKey;

	template<class T>
	class SKYLICHT_API CArrayKeyFrame
	{
	public:
		core::array<CKeyFrameData<T>> Data;

		T Default;

		int Hint;

		CArrayKeyFrame() :
			Hint(0)
		{
		}

		inline void clearHint()
		{
			Hint = 0;
		}

		int getIndex(f32 frame);

		inline u32 size()
		{
			return Data.size();
		}

		inline CKeyFrameData<T>* pointer()
		{
			return Data.pointer();
		}

		inline f32 getLastFrame()
		{
			if (Data.size() == 0)
				return 0.0f;

			return Data.getLast().Frame;
		}
	};

	template<class T>
	int CArrayKeyFrame<T>::getIndex(f32 frame)
	{
		int foundPositionIndex = -1;

		int numKey = (int)Data.size();
		CKeyFrameData<T>* pData = Data.pointer();

		// Test the Hints...
		if (Hint >= 0 && Hint < numKey)
		{
			if (Hint > 0 && pData[Hint].Frame >= frame && pData[Hint - 1].Frame < frame)
				foundPositionIndex = Hint;
			else if (Hint + 1 < numKey)
			{
				if (pData[Hint + 1].Frame >= frame && pData[Hint + 0].Frame < frame)
				{
					Hint++;
					foundPositionIndex = Hint;
				}
			}
		}

		// The Hint test failed, do a full scan...
		if (foundPositionIndex == -1)
		{
			for (s32 i = 0; i < numKey; ++i)
			{
				if (pData[i].Frame >= frame) // Keys should to be sorted by frame
				{
					foundPositionIndex = i;
					Hint = i;
					break;
				}
			}
		}

		return foundPositionIndex;
	}

	class SKYLICHT_API CAnimationData
	{
	public:
		CArrayKeyFrame<core::vector3df> Positions;
		CArrayKeyFrame<core::quaternion> Rotations;
		CArrayKeyFrame<core::vector3df> Scales;

		CAnimationData()
		{
		}
	};

	class SKYLICHT_API CAnimationTrack
	{
	protected:

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
			if (m_data)
			{
				m_data->Positions.clearHint();
				m_data->Rotations.clearHint();
				m_data->Scales.clearHint();
			}

			m_data = NULL;

			Name = "";
			HaveAnimation = false;
		}

		void setAnimationData(CAnimationData* data)
		{
			m_data = data;
		}

		CAnimationData* getFrameData()
		{
			return m_data;
		}
	};
}