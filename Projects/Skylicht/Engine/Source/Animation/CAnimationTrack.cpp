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


#pragma region QueryPosition
		u32 numPositionKey = data->Positions.size();

		if (numPositionKey)
		{
			foundPositionIndex = data->Positions.getIndex(frame);
			CPositionKey* pPositions = data->Positions.pointer();

			// Do interpolation...
			if (foundPositionIndex == 0)
			{
				position.X = pPositions[0].Value.X;
				position.Y = pPositions[0].Value.Y;
				position.Z = pPositions[0].Value.Z;
			}
			else if (foundPositionIndex != -1)
			{
				const CPositionKey& KeyA = pPositions[foundPositionIndex];
				const CPositionKey& KeyB = pPositions[foundPositionIndex - 1];

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
				position.X = pPositions[key].Value.X;
				position.Y = pPositions[key].Value.Y;
				position.Z = pPositions[key].Value.Z;
			}
		}
		else
		{
			position.X = data->Positions.Default.X;
			position.Y = data->Positions.Default.Y;
			position.Z = data->Positions.Default.Z;
		}

#pragma endregion	


#pragma region QueryScale
		u32 numScaleKey = data->Scales.size();
		if (numScaleKey)
		{
			foundScaleIndex = data->Scales.getIndex(frame);
			CScaleKey* pScale = data->Scales.pointer();

			// Do interpolation...
			if (foundScaleIndex == 0)
			{
				scale.X = pScale[0].Value.X;
				scale.Y = pScale[0].Value.Y;
				scale.Z = pScale[0].Value.Z;
			}
			else if (foundScaleIndex == -1)
			{
				int key = numScaleKey - 1;
				scale.X = pScale[key].Value.X;
				scale.Y = pScale[key].Value.Y;
				scale.Z = pScale[key].Value.Z;
			}
			else
			{
				const CScaleKey& KeyA = pScale[foundScaleIndex];
				const CScaleKey& KeyB = pScale[foundScaleIndex - 1];

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
			scale.X = data->Scales.Default.X;
			scale.Y = data->Scales.Default.Y;
			scale.Z = data->Scales.Default.Z;
		}
#pragma endregion


#pragma region QueryRotation
		s32 numRotKey = data->Rotations.size();

		if (numRotKey)
		{
			foundRotationIndex = data->Rotations.getIndex(frame);
			CRotationKey* pRotation = data->Rotations.pointer();

			// Do interpolation...
			if (foundRotationIndex == 0)
			{
				rotation.X = pRotation[0].Value.X;
				rotation.Y = pRotation[0].Value.Y;
				rotation.Z = pRotation[0].Value.Z;
				rotation.W = pRotation[0].Value.W;
			}
			else if (foundRotationIndex == -1)
			{
				int key = numRotKey - 1;
				rotation.X = pRotation[key].Value.X;
				rotation.Y = pRotation[key].Value.Y;
				rotation.Z = pRotation[key].Value.Z;
				rotation.W = pRotation[key].Value.W;
			}
			else
			{
				const CRotationKey& KeyA = pRotation[foundRotationIndex];
				const CRotationKey& KeyB = pRotation[foundRotationIndex - 1];

				const f32 fd1 = frame - KeyA.Frame;
				const f32 fd2 = KeyB.Frame - frame;
				const f32 t = fd1 / (fd1 + fd2);

				quaternionSlerp(rotation, KeyA.Value, KeyB.Value, t);
			}
		}
		else
		{
			rotation.X = data->Rotations.Default.X;
			rotation.Y = data->Rotations.Default.Y;
			rotation.Z = data->Rotations.Default.Z;
			rotation.W = data->Rotations.Default.W;
		}
#pragma endregion
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