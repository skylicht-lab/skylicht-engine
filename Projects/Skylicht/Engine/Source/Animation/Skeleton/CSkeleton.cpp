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

#include "Debug/CSceneDebug.h"

#define COPY_VECTOR3DF(dest, src)	dest.X = src.X; dest.Y = src.Y; dest.Z = src.Z
#define COPY_QUATERNION(dest, src)	dest.X = src.X; dest.Y = src.Y; dest.Z = src.Z; dest.W = src.W

namespace Skylicht
{
	CSkeleton::CSkeleton(int id) :
		m_id(id),
		m_enable(true),
		m_animationType(KeyFrame),
		m_clip(NULL),
		m_target(NULL),
		m_root(NULL),
		m_layerType(DefaultBlending),
		m_needUpdateActivateEntities(true)
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

			// save root nodes
			if (m_root == NULL)
				m_root = animationData;

			// save this id
			animationData->ID = (int)m_entitiesData.size() - 1;

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
		m_root = NULL;
	}

	void CSkeleton::setAnimation(CAnimationClip* clip, bool loop, float from, float duration, bool pause)
	{
		if (duration < 0.0f)
			duration = 0.0f;

		bool updateAnimationData = false;
		if (m_clip != clip)
		{
			m_clip = clip;
			updateAnimationData = true;
		}

		m_timeline.From = from;
		m_timeline.To = from + duration;
		m_timeline.Frame = from;
		m_timeline.Loop = loop;
		m_timeline.Pause = pause;

		if (updateAnimationData)
			setAnimationData();
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
			setAnimationData();

		m_timeline.To = m_timeline.Duration;
	}

	void CSkeleton::setAnimationData()
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
				track.setAnimationData(&anim->Data);

				// get anim duration
				float totalFrame = anim->Data.Positions.getLastFrame();
				totalFrame = core::max_(totalFrame, anim->Data.Rotations.getLastFrame());
				totalFrame = core::max_(totalFrame, anim->Data.Scales.getLastFrame());

				if (m_timeline.Duration < totalFrame)
					m_timeline.Duration = totalFrame;

				// notify this track have animation
				track.HaveAnimation = true;
			}
		}
	}

	void CSkeleton::updateActivateEntities()
	{
		if (m_needUpdateActivateEntities)
		{
			m_needUpdateActivateEntities = false;
			m_entitiesActivated.set_used(0);

			for (CAnimationTransformData*& entity : m_entitiesData)
			{
				if (entity->DisableAnimation || entity->Weight == 0.0f)
					continue;
				m_entitiesActivated.push_back(entity);
			}
		}
	}

	void CSkeleton::update()
	{
		updateActivateEntities();

		if (m_animationType == KeyFrame)
			updateTrackKeyFrame();
		else
			updateBlending();
	}

	void CSkeleton::applyTransform()
	{
		for (CAnimationTransformData*& entity : m_entitiesData)
		{
			if (entity->DisableAnimation)
				continue;

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
		CAnimationTransformData** entities = m_entitiesActivated.pointer();
		u32 count = (u32)m_entitiesActivated.size();

		for (u32 i = 0; i < count; i++)
		{
			CAnimationTransformData* entity = entities[i];

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
		bool first = true;

		for (CSkeleton*& skeleton : m_blending)
		{
			float weight = skeleton->getTimeline().Weight;
			if (weight == 0.0f)
				continue;

			EAnimationLayerType type = skeleton->getLayerType();
			switch (type)
			{
			case CSkeleton::DefaultBlending:
				doBlending(skeleton, first);
				break;
			case CSkeleton::Replace:
				doReplace(skeleton, first);
				break;
			case CSkeleton::Addtive:
				doAddtive(skeleton, first);
				break;
			default:
				break;
			}

			first = false;
		}
	}

	void CSkeleton::doBlending(CSkeleton* skeleton, bool first)
	{
		float skeletonWeight = skeleton->getTimeline().Weight;

		CAnimationTransformData** entities = m_entitiesActivated.pointer();
		u32 count = (u32)m_entitiesActivated.size();

		for (u32 i = 0; i < count; i++)
		{
			CAnimationTransformData* entity = entities[i];

			CAnimationTransformData* src = skeleton->m_entitiesData[entity->ID];

			float weight = skeletonWeight * src->Weight;

			core::quaternion* rotation = &src->AnimRotation;
			core::vector3df* position = &src->AnimPosition;
			core::vector3df* scale = &src->AnimScale;

			if (first == true)
			{
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
	}

	void CSkeleton::doAddtive(CSkeleton* skeleton, bool first)
	{
		core::quaternion addRotate;

		float skeletonWeight = skeleton->getTimeline().Weight;

		// Reference:
		// https://github.com/guillaumeblanc/ozz-animation/blob/master/src/animation/runtime/blending_job.cc
		// OZZ_ADD_PASS

		CAnimationTransformData** entities = m_entitiesActivated.pointer();
		u32 count = (u32)m_entitiesActivated.size();

		for (u32 i = 0; i < count; i++)
		{
			CAnimationTransformData* entity = entities[i];
			CAnimationTransformData* src = skeleton->m_entitiesData[entity->ID];

			float weight = skeletonWeight * src->Weight;
			float oneMinsWeight = 1.0f - weight;

			core::quaternion* rotation = &src->AnimRotation;
			core::vector3df* position = &src->AnimPosition;
			core::vector3df* scale = &src->AnimScale;

			entity->AnimPosition.X += position->X * weight;
			entity->AnimPosition.Y += position->Y * weight;
			entity->AnimPosition.Z += position->Z * weight;

			entity->AnimScale.X = entity->AnimScale.X * (oneMinsWeight + (scale->X * weight));
			entity->AnimScale.Y = entity->AnimScale.Y * (oneMinsWeight + (scale->Y * weight));
			entity->AnimScale.Z = entity->AnimScale.Z * (oneMinsWeight + (scale->Z * weight));

			float Rx = rotation->X;
			float Ry = rotation->Y;
			float Rz = rotation->Z;
			float Rw = rotation->W;

			if (Rw < 0.0f)
			{
				Rx *= -1.0f;
				Ry *= -1.0f;
				Rz *= -1.0f;
				Rw *= -1.0f;
			}

			addRotate.set(
				Rx * weight,
				Ry * weight,
				Rz * weight,
				(Rw - 1.0f) * weight + 1.0f
			);
			addRotate.normalize();
			entity->AnimRotation = addRotate * entity->AnimRotation;
		}
	}

	void CSkeleton::doReplace(CSkeleton* skeleton, bool first)
	{
		float skeletonWeight = skeleton->getTimeline().Weight;

		CAnimationTransformData** entities = m_entitiesActivated.pointer();
		u32 count = (u32)m_entitiesActivated.size();

		for (u32 i = 0; i < count; i++)
		{
			CAnimationTransformData* entity = entities[i];

			CAnimationTransformData* src = skeleton->m_entitiesData[entity->ID];

			float weight = skeletonWeight * src->Weight;
			float oneMinWeight = 1.0f - weight;

			core::quaternion* rotation = &src->AnimRotation;
			core::vector3df* position = &src->AnimPosition;
			core::vector3df* scale = &src->AnimScale;

			CAnimationTrack::quaternionSlerp(entity->AnimRotation, entity->AnimRotation, *rotation, weight);

			entity->AnimPosition.X = entity->AnimPosition.X * oneMinWeight + position->X * weight;
			entity->AnimPosition.Y = entity->AnimPosition.Y * oneMinWeight + position->Y * weight;
			entity->AnimPosition.Z = entity->AnimPosition.Z * oneMinWeight + position->Z * weight;

			entity->AnimScale.X = entity->AnimScale.X * oneMinWeight + scale->X * weight;
			entity->AnimScale.Y = entity->AnimScale.Y * oneMinWeight + scale->Y * weight;
			entity->AnimScale.Z = entity->AnimScale.Z * oneMinWeight + scale->Z * weight;
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
				float animWeight = trackInfo.Weight;
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

	void CSkeleton::setJointWeights(float weight)
	{
		for (CAnimationTransformData*& entity : m_entitiesData)
		{
			entity->Weight = weight;
		}

		m_needUpdateActivateEntities = true;
	}

	void CSkeleton::setJointWeights(const char* name, float weight, bool includeChild)
	{
		// search root
		int rootId = -1;

		for (CAnimationTransformData*& entity : m_entitiesData)
		{
			if (entity->Name == name)
			{
				entity->Weight = weight;
				rootId = entity->ID;
				break;
			}
		}

		if (includeChild && rootId > 0)
		{
			core::array<int> listParent;
			listParent.push_back(rootId);

			for (CAnimationTransformData*& entity : m_entitiesData)
			{
				if (listParent.linear_reverse_search(entity->ParentID) >= 0)
				{
					// if reference parent
					entity->Weight = weight;
					listParent.push_back(entity->ID);
				}
			}
		}

		m_needUpdateActivateEntities = true;
	}

	void CSkeleton::getJoints(const char* name, std::vector<CAnimationTransformData*>& joints)
	{
		int rootId = -1;

		for (CAnimationTransformData*& entity : m_entitiesData)
		{
			if (entity->Name == name)
			{
				rootId = entity->ID;
				joints.push_back(entity);
				break;
			}
		}

		if (rootId == -1)
			return;

		core::array<int> listParent;
		listParent.push_back(rootId);

		for (CAnimationTransformData*& entity : m_entitiesData)
		{
			if (listParent.linear_reverse_search(entity->ParentID) >= 0)
			{
				// if reference parent
				joints.push_back(entity);
				listParent.push_back(entity->ID);
			}
		}
	}

	void CSkeleton::setJointWeights(std::vector<CAnimationTransformData*>& joints, float weight)
	{
		for (CAnimationTransformData* entity : joints)
		{
			entity->Weight = weight;
		}

		m_needUpdateActivateEntities = true;
	}

	CAnimationTransformData* CSkeleton::getJoint(const char* name)
	{
		for (CAnimationTransformData*& entity : m_entitiesData)
		{
			if (entity->Name == name)
			{
				return entity;
			}
		}

		return NULL;
	}

	void CSkeleton::drawDebug(const core::matrix4& transform, const SColor& c)
	{
		CSceneDebug* debug = CSceneDebug::getInstance()->getNoZDebug();

		core::matrix4 relative;

		core::array<core::matrix4> worlds;
		worlds.set_used((u32)m_entitiesData.size());

		int id = 0;

		for (CAnimationTransformData*& entity : m_entitiesData)
		{
			relative.makeIdentity();

			if (!entity->DisableAnimation)
			{
				// todo calc relative matrix & position		
				// rotation
				entity->AnimRotation.getMatrix(relative);

				// position	
				f32* m1 = relative.pointer();

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
			}

			if (entity->ParentID < 0)
			{
				worlds[id] = transform * relative;
			}
			else
			{
				worlds[id] = worlds[entity->ParentID] * relative;

				// draw skeleton line
				debug->addLine(worlds[id].getTranslation(), worlds[entity->ParentID].getTranslation(), c);
			}

			id++;
		}
	}
}