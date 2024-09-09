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
#include "CSkinnedInstanceData.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CSkinnedInstanceData);

	IMPLEMENT_DATA_TYPE_INDEX(CSkinnedInstanceData);

	CSkinnedInstanceData::CSkinnedInstanceData() :
		IsVertexAnimationTexture(false),
		ClipOffset(NULL)
	{
		for (int i = 0; i < 2; i++)
		{
			Skeletons[i].ClipId = 0;
			Skeletons[i].Time = 0.0f;
			Skeletons[i].TimeFrom = 0.0f;
			Skeletons[i].TimeTo = 0.0f;
			Skeletons[i].FPS = 60;
			Skeletons[i].Frame = 0;
			if (i == 0)
				Skeletons[i].Weight = 1.0f;
			else
				Skeletons[i].Weight = 0.0f;
			Skeletons[i].Loop = true;
			Skeletons[i].Pause = false;
		}
	}

	CSkinnedInstanceData::~CSkinnedInstanceData()
	{
		// delete materials that created from CSkinnedMeshRendererInstancing
		u32 numMaterial = Materials.size();
		for (u32 i = 0; i < numMaterial; i++)
		{
			if (Materials[i])
				Materials[i]->drop();
		}
		Materials.clear();
	}

	void CSkinnedInstanceData::setAnimation(int animTextureIndex, CAnimationClip* clipInfo, float currentTime, int bakeFps, int skeletonId, bool loop, bool pause)
	{
		SSkeletonAnimation* data = &Skeletons[skeletonId];

		// tps to fix last frame is not baked
		float tps = 1.0f / bakeFps;

		if (IsVertexAnimationTexture)
			data->ClipId = ClipOffset[animTextureIndex];
		else
			data->ClipId = animTextureIndex;

		data->FPS = bakeFps;

		data->Time = currentTime;
		data->TimeFrom = 0.0f;
		data->TimeTo = clipInfo->Duration - tps;

		data->Frame = (int)(data->Time * bakeFps);

		data->Loop = loop;
		data->Pause = pause;

		// clamp
		data->Time = core::clamp(data->Time, data->TimeFrom, data->TimeTo);
	}

	void CSkinnedInstanceData::setAnimation(int animTextureIndex, CAnimationClip* clipInfo, float clipBegin, float clipDuration, float currentTime, int bakeFps, int skeletonId, bool loop, bool pause)
	{
		SSkeletonAnimation* data = &Skeletons[skeletonId];

		if (clipBegin >= clipInfo->Duration)
			clipBegin = 0.0f;

		// tps to fix last frame is not baked
		float tps = 1.0f / bakeFps;

		float clipEnd = clipBegin + clipDuration;
		float theEndTime = clipInfo->Duration - tps;
		if (clipEnd >= theEndTime)
			clipEnd = theEndTime;

		if (IsVertexAnimationTexture)
			data->ClipId = ClipOffset[animTextureIndex];
		else
			data->ClipId = animTextureIndex;

		data->FPS = bakeFps;

		data->Time = currentTime;
		data->TimeFrom = clipBegin;
		data->TimeTo = clipEnd;

		data->Frame = (int)(data->Time * bakeFps);

		data->Loop = loop;
		data->Pause = pause;

		// clamp
		data->Time = core::clamp(data->Time, data->TimeFrom, data->TimeTo);
	}

	void CSkinnedInstanceData::setAnimationWeight(int skeletonId, float weight)
	{
		Skeletons[skeletonId].Weight = weight;
	}
}