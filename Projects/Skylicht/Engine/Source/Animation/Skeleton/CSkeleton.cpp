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
#include "CSkeleton.h"

#define COPY_VECTOR3DF(dest, src)	dest.X = src.X; dest.Y = src.Y; dest.Z = src.Z
#define COPY_QUATERNION(dest, src)	dest.X = src.X; dest.Y = src.Y; dest.Z = src.Z; dest.W = src.W

namespace Skylicht
{
	CSkeleton::CSkeleton(int id) :
		m_id(id),
		m_enable(true),
		m_animationType(TrackKeyFrame),
		m_clip(NULL)
	{

	}

	CSkeleton::~CSkeleton()
	{
		releaseAllEntities();
	}

	void CSkeleton::initSkeleton(core::array<CEntity*>& entities)
	{
		// release old entities
		releaseAllEntities();

		std::map<int, int> entityIDMap;

		// we clone skeleton entities struct from render mesh entities
		CEntity** worldEntities = entities.pointer();

		for (u32 i = 0, n = entities.size(); i < n; i++)
		{
			CEntity* worldEntity = worldEntities[i];
			CWorldTransformData *worldTransform = worldEntity->getData<CWorldTransformData>();

			CEntity* newEntity = m_entities.createEntity();

			// map index
			entityIDMap[worldEntity->getIndex()] = newEntity->getIndex();

			// add animation transform data
			CAnimationTransformData *animationData = newEntity->addData<CAnimationTransformData>();
			m_entitiesData.push_back(animationData);

			animationData->Name = worldTransform->Name;
			animationData->WorldTransform = worldTransform;

			// find parent
			if (entityIDMap.find(worldTransform->ParentIndex) != entityIDMap.end())
			{
				animationData->ParentID = entityIDMap[worldTransform->ParentIndex];

				CAnimationTransformData *parentTransform = m_entities.getEntity(animationData->ParentID)->getData<CAnimationTransformData>();
				animationData->Depth = parentTransform->Depth + 1;
			}
			else
			{
				animationData->ParentID = -1;
				animationData->Depth = 0;
			}

			// default relative matrix
			animationData->DefaultRelativeMatrix = worldTransform->Relative;

			// default pos, scale, rot
			animationData->DefaultPosition = worldTransform->Relative.getTranslation();
			animationData->DefaultScale = worldTransform->Relative.getScale();
			animationData->DefaultRotation = core::quaternion(worldTransform->Relative);

			// default anim pos, scale, rot
			animationData->AnimPosition = animationData->DefaultPosition;
			animationData->AnimScale = animationData->DefaultScale;
			animationData->AnimRotation = animationData->DefaultRotation;
		}
	}

	void CSkeleton::releaseAllEntities()
	{
		m_entities.releaseAllEntities();
		m_entitiesData.clear();
	}

	void CSkeleton::setAnimation(CAnimationClip *clip, bool loop, bool pause)
	{
		m_clip = clip;

		m_timeline.AnimationDuration = 0.0f;
		m_timeline.AnimationFrame = 0.0f;
		m_timeline.AnimationLoop = loop;
		m_timeline.Pause = pause;

		for (CAnimationTransformData *&entity : m_entitiesData)
		{
			CAnimationTrack& track = entity->AnimationTrack;
			track.clearAllKeyFrame();

			SEntityAnim *anim = clip->getAnimOfEntity(entity->Name);
			if (anim != NULL)
			{
				// apply new frame data
				track.Name = clip->AnimName;
				track.setFrameData(&anim->Data);

				// get anim duration
				float totalFrame = 0;
				if (anim->Data.RotationKeys.size() > 0)
					totalFrame = anim->Data.RotationKeys.getLast().frame;
				else if (anim->Data.PositionKeys.size() > 0)
					totalFrame = anim->Data.PositionKeys.getLast().frame;
				else if (anim->Data.ScaleKeys.size() > 0)
					totalFrame = anim->Data.ScaleKeys.getLast().frame;

				if (m_timeline.AnimationDuration < totalFrame)
					m_timeline.AnimationDuration = totalFrame;

				// notify this track have animation
				track.HaveAnimation = true;
			}
		}
	}

	void CSkeleton::update()
	{
		if (m_animationType == TrackKeyFrame)
			updateTrackKeyFrame();
		else
			updateBlending();
	}

	void CSkeleton::applyTransform()
	{
		for (CAnimationTransformData *&entity : m_entitiesData)
		{
			// todo calc relative matrix & position
			core::matrix4 &relativeMatrix = entity->WorldTransform->Relative;

			// rotation
			entity->AnimRotation.getMatrix(relativeMatrix);

			// position	
			f32 *m1 = relativeMatrix.pointer();

			m1[12] = entity->AnimPosition.X;
			m1[13] = entity->AnimPosition.Y;
			m1[14] = entity->AnimPosition.Z;

			// scale
			m1[0] *= entity->AnimScale.X;
			m1[1] *= entity->AnimScale.X;
			m1[2] *= entity->AnimScale.X;
			m1[3] *= entity->AnimScale.X;

			m1[4] *= entity->AnimScale.Y;
			m1[5] *= entity->AnimScale.Y;
			m1[6] *= entity->AnimScale.Y;
			m1[7] *= entity->AnimScale.Y;

			m1[8] *= entity->AnimScale.Z;
			m1[9] *= entity->AnimScale.Z;
			m1[10] *= entity->AnimScale.Z;
			m1[11] *= entity->AnimScale.Z;

			// notify transform changed for CWorldTransformSystem
			entity->WorldTransform->HasChanged = true;
		}
	}

	void CSkeleton::updateTrackKeyFrame()
	{
		m_timeline.update();

		for (CAnimationTransformData *&entity : m_entitiesData)
		{
			CAnimationTrack& track = entity->AnimationTrack;

			if (track.HaveAnimation == true)
			{
				float frame = m_timeline.AnimationFrame;
				frame = offsetFrame(frame);
				track.getFrameData(frame, entity->AnimPosition, entity->AnimScale, entity->AnimRotation);
			}
			else
			{
				COPY_VECTOR3DF(entity->AnimPosition, entity->DefaultPosition);
				COPY_VECTOR3DF(entity->AnimRotation, entity->DefaultRotation);
				COPY_VECTOR3DF(entity->AnimScale, entity->DefaultScale);
			}
		}
	}

	float CSkeleton::offsetFrame(float frame)
	{
		// need offset the timeline
		float offset = m_timeline.AnimationDuration * m_timeline.SyncSeekRatio;
		frame = frame + offset;

		if (frame > m_timeline.AnimationDuration)
			frame = frame - m_timeline.AnimationDuration;

		return frame;
	}

	void CSkeleton::updateBlending()
	{

	}
}