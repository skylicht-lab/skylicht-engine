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
	/// @brief Template for keyframe data.
	/// @ingroup Animation
	template<class T>
	class SKYLICHT_API CKeyFrameData
	{
	public:
		//! Frame time in seconds.
		f32 Frame; // Second

		//! Value of the keyframe.
		T Value;
	};

	typedef CKeyFrameData<core::vector3df> CPositionKey;
	typedef CKeyFrameData<core::quaternion> CRotationKey;
	typedef CKeyFrameData<core::vector3df> CScaleKey;

	/// @brief Container for an array of keyframes with interpolation support.
	/// @ingroup Animation
	template<class T>
	class SKYLICHT_API CArrayKeyFrame
	{
	public:
		//! List of keyframes, sorted by time.
		core::array<CKeyFrameData<T>> Data;

		//! Default value used if no keyframes are present.
		T Default;

		//! Internal hint for optimizing keyframe searching.
		int Hint;

		CArrayKeyFrame() :
			Hint(0)
		{
		}

		/**
		 * @brief Resets the search hint.
		 */
		inline void clearHint()
		{
			Hint = 0;
		}

		/**
		 * @brief Finds the index of the keyframe at or just before the specified time.
		 * @param frame Time in seconds.
		 * @return Index in the Data array.
		 */
		int getIndex(f32 frame);

		/**
		 * @brief Gets the number of keyframes.
		 * @return Size of Data array.
		 */
		inline u32 size()
		{
			return Data.size();
		}

		/**
		 * @brief Returns the raw pointer to the keyframe data.
		 * @return Pointer to CKeyFrameData<T>.
		 */
		inline CKeyFrameData<T>* pointer()
		{
			return Data.pointer();
		}

		/**
		 * @brief Gets the time of the last keyframe.
		 * @return Time in seconds.
		 */
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

	/**
	 * @brief Container for PRS (Position, Rotation, Scale) animation data.
	 * @ingroup Animation
	 */
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

	/**
	 * @brief Handles interpolation and data retrieval for a single animation track.
	 * @ingroup Animation
	 */
	class SKYLICHT_API CAnimationTrack
	{
	protected:
		//! Pointer to the shared animation data.
		CAnimationData* m_data;

	public:
		//! The name of the track.
		std::string Name;

		//! Whether this track currently has animation data.
		bool HaveAnimation;

	public:
		CAnimationTrack();

		virtual ~CAnimationTrack();

		/**
		 * @brief Static utility for spherical linear interpolation between quaternions.
		 * @param result Resulting quaternion.
		 * @param q1 Start quaternion.
		 * @param q2 End quaternion.
		 * @param t Interpolation factor (0 to 1).
		 */
		static void quaternionSlerp(core::quaternion& result, core::quaternion q1, core::quaternion q2, float t);

		/**
		 * @brief Calculates the interpolated PRS data for a specific frame.
		 * @param frame Time in seconds.
		 * @param position [out] Interpolated position.
		 * @param scale [out] Interpolated scale.
		 * @param rotation [out] Interpolated rotation.
		 */
		void getFrameData(f32 frame,
			core::vector3df& position,
			core::vector3df& scale,
			core::quaternion& rotation);

		/**
		 * @brief Gets the underlying animation data.
		 * @return Pointer to CAnimationData.
		 */
		CAnimationData* getAnimData();

		/**
		 * @brief Clears all keyframe search hints and resets the track.
		 */
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

		/**
		 * @brief Sets the source animation data for this track.
		 * @param data Pointer to animation data.
		 */
		void setAnimationData(CAnimationData* data)
		{
			m_data = data;
		}

		/**
		 * @brief Gets the underlying animation data.
		 * @return Pointer to CAnimationData.
		 */
		CAnimationData* getFrameData()
		{
			return m_data;
		}
	};
}