/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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

#include "Animation/CAnimationClip.h"
#include "RenderMesh/CRenderMesh.h"
#include "RenderMesh/CRenderMeshInstancing.h"

namespace Skylicht
{
	class SKYLICHT_API CRenderMeshInstancingVAT : public CRenderMeshInstancing
	{
	protected:
		int m_clipOffset[10];

	public:
		CRenderMeshInstancingVAT();

		virtual ~CRenderMeshInstancingVAT();

		virtual void initFromPrefab(CEntityPrefab* prefab);

		virtual CEntity* spawn();

		static bool setAnimation(CEntity* entity, int clipId, CAnimationClip* clipInfo, float currentTime = 0.0f, int bakeFps = 60, int skeletonId = 0, bool loop = true, bool pause = false);

		static bool setAnimation(CEntity* entity, int clipId, CAnimationClip* clipInfo, float clipBegin, float clipDuration, float currentTime = 0.0f, int bakeFps = 60, int skeletonId = 0, bool loop = true, bool pause = false);

		static void setAnimationWeight(CEntity* entity, int skeletonId, float weight);

		DECLARE_GETTYPENAME(CRenderMeshInstancingVAT)

	public:

		inline int getClipFrameOffset(int clipId)
		{
			return m_clipOffset[clipId];
		}

		void allocFrames(u32 numFrames);

		void bakeSkinnedMesh(u32 frame);

		void beginBake();

		void setClipFrameOffset(u32 id, u32 frames);

		void endBake();
	};
}