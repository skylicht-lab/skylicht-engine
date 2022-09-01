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
#include "CAnimationTrack.h"

namespace Skylicht
{
	CAnimationTrack::CAnimationTrack() :
		m_data(NULL),
		m_posHint(0),
		m_scaleHint(0),
		m_rotHint(0),
		HaveAnimation(false)
	{
	}

	CAnimationTrack::~CAnimationTrack()
	{
	}

	CAnimationData* CAnimationTrack::getAnimData()
	{
		return m_data;
	}

	void CAnimationTrack::getFrameData(f32 frame,
		core::vector3df& position,
		core::vector3df& scale,
		core::quaternion& rotation)
	{
		CAnimationData* data = getAnimData();

		if (data == NULL)
		{
			return;
		}

		s32 foundPositionIndex = -1;
		s32 foundScaleIndex = -1;
		s32 foundRotationIndex = -1;

		// get last hint
		s32 positionHint = m_posHint;
		s32 scaleHint = m_scaleHint;
		s32 rotationHint = m_rotHint;

#pragma region QueryPosition
		s32 numPositionKey = data->PositionKeys.size();
		CPositionKey* pPositionKey = data->PositionKeys.pointer();

		if (numPositionKey)
		{
			foundPositionIndex = -1;

			// Test the Hints...
			if (positionHint >= 0 && positionHint < numPositionKey)
			{
				// check this hint
				if (positionHint > 0 && pPositionKey[positionHint].Frame >= frame && pPositionKey[positionHint - 1].Frame < frame)
					foundPositionIndex = positionHint;
				else if (positionHint + 1 < numPositionKey)
				{
					// check the next index
					if (pPositionKey[positionHint + 1].Frame >= frame && pPositionKey[positionHint + 0].Frame < frame)
					{
						positionHint++;
						foundPositionIndex = positionHint;
					}
				}
			}

			// The hint test failed, do a full scan...
			if (foundPositionIndex == -1)
			{
				for (s32 i = 0; i < numPositionKey; ++i)
				{
					if (pPositionKey[i].Frame >= frame) // Keys should to be sorted by frame
					{
						foundPositionIndex = i;
						positionHint = i;
						break;
					}
				}
			}

			// Do interpolation...
			if (foundPositionIndex == 0)
			{
				position.X = pPositionKey[0].Value.X;
				position.Y = pPositionKey[0].Value.Y;
				position.Z = pPositionKey[0].Value.Z;
			}
			else if (foundPositionIndex != -1)
			{
				const CPositionKey& KeyA = pPositionKey[foundPositionIndex];
				const CPositionKey& KeyB = pPositionKey[foundPositionIndex - 1];

				const f32 fd1 = frame - KeyA.Frame;
				const f32 fd2 = KeyB.Frame - frame;
				const f32 fd = fd1 + fd2;

				// inline func
				position.X = ((KeyB.Value.X - KeyA.Value.X) / fd) * fd1 + KeyA.Value.X;
				position.Y = ((KeyB.Value.Y - KeyA.Value.Y) / fd) * fd1 + KeyA.Value.Y;
				position.Z = ((KeyB.Value.Z - KeyA.Value.Z) / fd) * fd1 + KeyA.Value.Z;
			}
			else if (foundPositionIndex == -1)
			{
				int key = numPositionKey - 1;
				position.X = pPositionKey[key].Value.X;
				position.Y = pPositionKey[key].Value.Y;
				position.Z = pPositionKey[key].Value.Z;
			}
		}
		else
		{
			position.X = data->DefaultPos.X;
			position.Y = data->DefaultPos.Y;
			position.Z = data->DefaultPos.Z;
		}

#pragma endregion	


#pragma region QueryScale
		s32 numScaleKey = data->ScaleKeys.size();
		CScaleKey* pScaleKey = data->ScaleKeys.pointer();

		if (numScaleKey)
		{
			foundScaleIndex = -1;

			// Test the Hints...
			if (scaleHint >= 0 && scaleHint < numScaleKey)
			{
				// check this hint
				if (scaleHint > 0 && pScaleKey[scaleHint].Frame >= frame && pScaleKey[scaleHint - 1].Frame < frame)
					foundScaleIndex = scaleHint;
				else if (scaleHint + 1 < numScaleKey)
				{
					// check the next index
					if (pScaleKey[scaleHint + 1].Frame >= frame && pScaleKey[scaleHint].Frame < frame)
					{
						scaleHint++;
						foundScaleIndex = scaleHint;
					}
				}
			}


			// The hint test failed, do a full scan...
			if (foundScaleIndex == -1)
			{
				for (s32 i = 0; i < numScaleKey; ++i)
				{
					if (pScaleKey[i].Frame >= frame) // Keys should to be sorted by frame
					{
						foundScaleIndex = i;
						scaleHint = i;
						break;
					}
				}
			}

			// Do interpolation...
			if (foundScaleIndex == 0)
			{
				scale.X = pScaleKey[0].Value.X;
				scale.Y = pScaleKey[0].Value.Y;
				scale.Z = pScaleKey[0].Value.Z;
			}
			else if (foundScaleIndex == -1)
			{
				int key = numScaleKey - 1;
				scale.X = pScaleKey[key].Value.X;
				scale.Y = pScaleKey[key].Value.Y;
				scale.Z = pScaleKey[key].Value.Z;
			}
			else
			{
				const CScaleKey& KeyA = pScaleKey[foundScaleIndex];
				const CScaleKey& KeyB = pScaleKey[foundScaleIndex - 1];

				const f32 fd1 = frame - KeyA.Frame;
				const f32 fd2 = KeyB.Frame - frame;
				const f32 fd = fd1 + fd2;

				// inline func
				scale.X = ((KeyB.Value.X - KeyA.Value.X) / fd) * fd1 + KeyA.Value.X;
				scale.Y = ((KeyB.Value.Y - KeyA.Value.Y) / fd) * fd1 + KeyA.Value.Y;
				scale.Z = ((KeyB.Value.Z - KeyA.Value.Z) / fd) * fd1 + KeyA.Value.Z;
			}
		}
		else
		{
			scale.X = data->DefaultScale.X;
			scale.Y = data->DefaultScale.Y;
			scale.Z = data->DefaultScale.Z;
		}
#pragma endregion


#pragma region QueryRotation
		s32 numRotKey = data->RotationKeys.size();
		CRotationKey* pRotKey = data->RotationKeys.pointer();

		if (numRotKey)
		{
			foundRotationIndex = -1;

			// Test the Hints...
			if (rotationHint >= 0 && rotationHint < numRotKey)
			{
				// check this hint
				if (rotationHint > 0 && pRotKey[rotationHint].Frame >= frame && pRotKey[rotationHint - 1].Frame < frame)
					foundRotationIndex = rotationHint;
				else if (rotationHint + 1 < numRotKey)
				{
					// check the next index
					if (pRotKey[rotationHint + 1].Frame >= frame && pRotKey[rotationHint].Frame < frame)
					{
						rotationHint++;
						foundRotationIndex = rotationHint;
					}
				}
			}


			// The hint test failed, do a full scan...
			if (foundRotationIndex == -1)
			{
				for (s32 i = 0; i < numRotKey; ++i)
				{
					if (pRotKey[i].Frame >= frame) // Keys should be sorted by frame
					{
						foundRotationIndex = i;
						rotationHint = i;
						break;
					}
				}
			}

			// Do interpolation...
			if (foundRotationIndex == 0)
			{
				rotation.X = pRotKey[0].Value.X;
				rotation.Y = pRotKey[0].Value.Y;
				rotation.Z = pRotKey[0].Value.Z;
				rotation.W = pRotKey[0].Value.W;
			}
			else if (foundRotationIndex == -1)
			{
				int key = numRotKey - 1;
				rotation.X = pRotKey[key].Value.X;
				rotation.Y = pRotKey[key].Value.Y;
				rotation.Z = pRotKey[key].Value.Z;
				rotation.W = pRotKey[key].Value.W;
			}
			else
			{
				const CRotationKey& KeyA = pRotKey[foundRotationIndex];
				const CRotationKey& KeyB = pRotKey[foundRotationIndex - 1];

				const f32 fd1 = frame - KeyA.Frame;
				const f32 fd2 = KeyB.Frame - frame;
				const f32 t = fd1 / (fd1 + fd2);

				quaternionSlerp(rotation, KeyA.Value, KeyB.Value, t);
			}
		}
		else
		{
			rotation.X = data->DefaultRot.X;
			rotation.Y = data->DefaultRot.Y;
			rotation.Z = data->DefaultRot.Z;
			rotation.W = data->DefaultRot.W;
		}
#pragma endregion

		// save hint
		m_posHint = positionHint;
		m_scaleHint = scaleHint;
		m_rotHint = rotationHint;
	}

	void CAnimationTrack::quaternionSlerp(core::quaternion& result, core::quaternion q1, core::quaternion q2, float time)
	{
		f32 angle = (q1.X * q2.X) + (q1.Y * q2.Y) + (q1.Z * q2.Z) + (q1.W * q2.W);

		// make sure we use the short rotation
		if (angle < 0.0f)
		{
			q1.X *= -1.0f;
			q1.Y *= -1.0f;
			q1.Z *= -1.0f;
			q1.W *= -1.0f;

			angle *= -1.0f;
		}

		f32 scale = 1.0f - time;
		f32 invscale = time;

		if ((angle + 1.0f) > 0.05f)
		{
			if ((1.0f - angle) >= 0.05f) // spherical interpolation
			{
				const f32 theta = acosf(angle);
				const f32 invsintheta = core::reciprocal(sinf(theta));
				scale = sinf(theta * (1.0f - time)) * invsintheta;
				invscale = sinf(theta * time) * invsintheta;
			}
			else // linear interploation
			{
				scale = 1.0f - time;
				invscale = time;
			}
		}
		else
		{
			q2.X = -q1.Y;
			q2.Y = q1.X;
			q2.Z = -q1.W;
			q2.W = q1.Z;

			scale = sinf(core::PI * (0.5f - time));
			invscale = sinf(core::PI * time);
		}

		// inline func
		result.X = q1.X * scale + q2.X * invscale;
		result.Y = q1.Y * scale + q2.Y * invscale;
		result.Z = q1.Z * scale + q2.Z * invscale;
		result.W = q1.W * scale + q2.W * invscale;
	}
}