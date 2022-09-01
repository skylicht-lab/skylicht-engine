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
		m_animationType(KeyFrame),
		m_clip(NULL),
		m_target(NULL)
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
			CWorldTransformData* worldTransform = GET_ENTITY_DATA(worldEntity, CWorldTransformData);

			CEntity* newEntity = m_entities.createEntity();

			// map world entity index to local entity
			entityIDMap[worldEntity->getIndex()] = newEntity->getIndex();

			// add animation transform data
			CAnimationTransformData* animationData = newEntity->addData<CAnimationTransformData>();
			m_entitiesData.push_back(animationData);

			animationData->Name = worldTransform->Name;

			// handle real world transform
			animationData->WorldTransform = worldTransform;

			// find parent
			if (entityIDMap.find(worldTransform->ParentIndex) != entityIDMap.end())
			{
				animationData->ParentID = entityIDMap[worldTransform->ParentIndex];

				CAnimationTransformData* parentTransform = GET_ENTITY_DATA(m_entities.getEntity(animationData->ParentID), CAnimationTransformData);
				animationData->Depth = parentTransform->Depth + 1;
			}
			else
			{
				animationData->ParentID = -1;
				animationData->Depth = 0;
			}

			// default pos, scale, rot
			animationData->DefaultPosition = worldTransform->Relative.getTranslation();
			animationData->DefaultScale = worldTransform->Relative.getScale();
			animationData->DefaultRotation = core::quaternion(worldTransform->Relative);

			// default anim pos, scale, rot
			COPY_VECTOR3DF(animationData->AnimPosition, animationData->DefaultPosition);
			COPY_VECTOR3DF(animationData->AnimScale, animationData->DefaultScale);
			COPY_QUATERNION(animationData->AnimRotation, animationData->DefaultRotation);
		}
	}

	void CSkeleton::releaseAllEntities()
	{
		m_entities.releaseAllEntities();
		m_entitiesData.clear();
	}

	void CSkeleton::setAnimation(CAnimationClip* clip, bool loop, float from, float duration, bool pause)
	{
		if (duration < 0.0f)
			duration = 0.0f;

		bool updateFrameData = false;
		if (m_clip != clip)
		{
			m_clip = clip;
			updateFrameData = true;
		}

		m_timeline.From = from;
		m_timeline.To = from + duration;
		m_timeline.Frame = from;
		m_timeline.Loop = loop;
		m_timeline.Pause = pause;

		if (updateFrameData)
			setFrameData();
	}

	void CSkeleton::setAnimation(CAnimationClip* clip, bool loop, bool pause)
	{
		bool updateFrameData = false;
		if (m_clip != clip)
		{
			m_clip = clip;
			updateFrameData = true;
		}

		m_timeline.From = 0.0f;
		m_timeline.Duration = 0.0f;
		m_timeline.Frame = 0.0f;
		m_timeline.Loop = loop;
		m_timeline.Pause = pause;

		if (updateFrameData)
			setFrameData();

		m_timeline.To = m_timeline.Duration;
	}

	void CSkeleton::setFrameData()
	{
		for (CAnimationTransformData*& entity : m_entitiesData)
		{
			CAnimationTrack& track = entity->AnimationTrack;
			track.clearAllKeyFrame();

			SEntityAnim* anim = m_clip->getAnimOfEntity(entity->Name);
			if (anim != NULL)
			{
				// apply new frame data
				track.Name = m_clip->AnimName;
				track.setFrameData(&anim->Data);

				// get anim duration
				float totalFrame = 0;
				if (anim->Data.RotationKeys.size() > 0)
					totalFrame = anim->Data.RotationKeys.getLast().Frame;
				else if (anim->Data.PositionKeys.size() > 0)
					totalFrame = anim->Data.PositionKeys.getLast().Frame;
				else if (anim->Data.ScaleKeys.size() > 0)
					totalFrame = anim->Data.ScaleKeys.getLast().Frame;

				if (m_timeline.Duration < totalFrame)
					m_timeline.Duration = totalFrame;

				// notify this track have animation
				track.HaveAnimation = true;
			}
		}
	}

	void CSkeleton::update()
	{
		if (m_animationType == KeyFrame)
			updateTrackKeyFrame();
		else
			updateBlending();
	}

	void CSkeleton::applyTransform()
	{
		for (CAnimationTransformData*& entity : m_entitiesData)
		{
			// todo calc relative matrix & position
			core::matrix4& relativeMatrix = entity->WorldTransform->Relative;
			relativeMatrix.makeIdentity();

			// rotation
			entity->AnimRotation.getMatrix(relativeMatrix);

			// position	
			f32* m1 = relativeMatrix.pointer();

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
		for (CAnimationTransformData*& entity : m_entitiesData)
		{
			CAnimationTrack& track = entity->AnimationTrack;

			if (track.HaveAnimation == true)
			{
				float frame = m_timeline.Frame;
				track.getFrameData(frame, entity->AnimPosition, entity->AnimScale, entity->AnimRotation);
			}
			else
			{
				COPY_VECTOR3DF(entity->AnimPosition, entity->DefaultPosition);
				COPY_VECTOR3DF(entity->AnimScale, entity->DefaultScale);
				COPY_QUATERNION(entity->AnimRotation, entity->DefaultRotation);
			}
		}
	}

	void CSkeleton::setTarget(CSkeleton* skeleton)
	{
		if (m_target != NULL)
			m_target->removeBlending(this);

		m_target = skeleton;
		m_target->addBlending(this);
	}

	void CSkeleton::addBlending(CSkeleton* skeleton)
	{
		if (std::find(m_blending.begin(), m_blending.end(), skeleton) == m_blending.end())
			m_blending.push_back(skeleton);
	}

	void CSkeleton::removeBlending(CSkeleton* skeleton)
	{
		std::vector<CSkeleton*>::iterator i = std::find(m_blending.begin(), m_blending.end(), skeleton);
		if (i != m_blending.end())
			m_blending.erase(i);
	}

	void CSkeleton::updateBlending()
	{
		int id = 0;

		for (CAnimationTransformData*& entity : m_entitiesData)
		{
			bool first = true;

			for (CSkeleton*& skeleton : m_blending)
			{
				core::quaternion* rotation = &skeleton->m_entitiesData[id]->AnimRotation;
				core::vector3df* position = &skeleton->m_entitiesData[id]->AnimPosition;
				core::vector3df* scale = &skeleton->m_entitiesData[id]->AnimScale;

				float weight = skeleton->getTimeline().Weight;
				if (weight == 0.0f)
					continue;

				if (first == true)
				{
					first = false;

					entity->AnimRotation.X = rotation->X * weight;
					entity->AnimRotation.Y = rotation->Y * weight;
					entity->AnimRotation.Z = rotation->Z * weight;
					entity->AnimRotation.W = rotation->W * weight;

					entity->AnimPosition.X = position->X * weight;
					entity->AnimPosition.Y = position->Y * weight;
					entity->AnimPosition.Z = position->Z * weight;

					entity->AnimScale.X = scale->X * weight;
					entity->AnimScale.Y = scale->Y * weight;
					entity->AnimScale.Z = scale->Z * weight;
				}
				else
				{
					entity->AnimPosition.X += position->X * weight;
					entity->AnimPosition.Y += position->Y * weight;
					entity->AnimPosition.Z += position->Z * weight;

					entity->AnimScale.X += scale->X * weight;
					entity->AnimScale.Y += scale->Y * weight;
					entity->AnimScale.Z += scale->Z * weight;

					float Rx = rotation->X;
					float Ry = rotation->Y;
					float Rz = rotation->Z;
					float Rw = rotation->W;

					float dot = entity->AnimRotation.X * Rx + entity->AnimRotation.Y * Ry + entity->AnimRotation.Z * Rz + entity->AnimRotation.W * Rw;

					if (dot < 0.0f)
					{
						Rx *= -1.0f;
						Ry *= -1.0f;
						Rz *= -1.0f;
						Rw *= -1.0f;
					}

					entity->AnimRotation.X += Rx * weight;
					entity->AnimRotation.Y += Ry * weight;
					entity->AnimRotation.Z += Rz * weight;
					entity->AnimRotation.W += Rw * weight;
				}
			}

			id++;
		}
	}

	void CSkeleton::syncAnimationByTimeScale()
	{
		float frameRatio = 0.0f;
		float maxWeight = -1;

		CSkeleton* baseSkeleton = NULL;

		for (CSkeleton*& skeleton : m_blending)
		{
			CAnimationTimeline& trackInfo = skeleton->getTimeline();
			if (trackInfo.Duration > 0 && trackInfo.Weight > 0.0f)
			{
				float targetBlend = 1.0f;

				float animWeight = trackInfo.Weight * targetBlend;
				float animDuration = trackInfo.To - trackInfo.From;

				// find frame ratio
				if (maxWeight < animWeight)
				{
					maxWeight = animWeight;
					frameRatio = trackInfo.Frame / animDuration;
					baseSkeleton = skeleton;
				}
			}
		}

		if (baseSkeleton == NULL)
			return;

		// sync speed
		for (CSkeleton*& skeleton : m_blending)
		{
			CAnimationTimeline& trackInfo = skeleton->getTimeline();
			frameRatio = core::clamp<float>(frameRatio, 0.0f, 1.0f);
			trackInfo.Frame = trackInfo.From + frameRatio * (trackInfo.To - trackInfo.From);
		}
	}
}