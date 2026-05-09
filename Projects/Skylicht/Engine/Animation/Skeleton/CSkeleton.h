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

#include "CAnimationTimeline.h"
#include "CAnimationTransformData.h"
#include "Entity/CEntityPrefab.h"
#include "Animation/CAnimationClip.h"

namespace Skylicht
{
	/// @brief Manages a hierarchy of animated entities, handling keyframe updates and animation blending.
	/// @ingroup Animation
	/// 
	/// Example of setting up blending
	/// 
	/// @code
	/// CSkeleton* baseSkeleton = animController->createSkeleton();
	/// baseSkeleton->setAnimationType(CSkeleton::Blending);
	/// 
	/// CSkeleton* walkLayer = new CSkeleton(1);
	/// walkLayer->initSkeleton(renderMesh->getEntities());
	/// walkLayer->setAnimation(walkClip, true);
	/// walkLayer->setTarget(baseSkeleton);
	/// @endcode
	/// 
	class SKYLICHT_API CSkeleton
	{
	public:
		/**
		 * @brief Operating modes for the skeleton.
		 */
		enum EAnimationType
		{
			KeyFrame = 0, //! Standard mode: plays a single animation clip.
			Blending,     //! Blending mode: combines multiple layers of animations.
		};

		/**
		 * @brief Blending algorithms for layers.
		 */
		enum EAnimationLayerType
		{
			DefaultBlending, //! Normal linear blending.
			Replace,         //! Overrides previous layers.
			Addtive,         //! Adds delta transform to previous layers.
		};

	protected:
		//! Collection of entities (bones) in this skeleton.
		CEntityPrefab m_entities;

		//! List of internal animation data for each bone.
		std::vector<CAnimationTransformData*> m_entitiesData;

		//! List of bones that are currently active (weight > 0).
		core::array<CAnimationTransformData*> m_entitiesActivated;

		//! Pointer to the root bone data.
		CAnimationTransformData* m_root;

		//! Unique ID of the skeleton.
		int m_id;

		//! Whether this skeleton is enabled.
		bool m_enable;

		//! Flag to trigger update of activated entities.
		bool m_needUpdateActivateEntities;

		//! Playback state for this skeleton.
		CAnimationTimeline m_timeline;

		//! Current operating mode.
		EAnimationType m_animationType;

		//! Currently assigned animation clip.
		CAnimationClip* m_clip;

		//! Blending type for this skeleton (when used as a layer).
		EAnimationLayerType m_layerType;

	protected:

		//! For blending: the skeleton that this layer blends into.
		CSkeleton* m_target;

		//! List of sub-skeletons (layers) that blend into this skeleton.
		std::vector<CSkeleton*> m_blending;

	public:
		CSkeleton(int id);

		virtual ~CSkeleton();

		/**
		 * @brief Initializes the skeleton hierarchy from a list of world entities.
		 * @param entities List of entities representing the model's bones.
		 */
		void initSkeleton(core::array<CEntity*>& entities);

		/**
		 * @brief Releases all bone entities.
		 */
		void releaseAllEntities();

		/**
		 * @brief Updates the animation state.
		 */
		void update();

		/**
		 * @brief Filters and caches bones that are currently contributing to the animation.
		 */
		void updateActivateEntities();

		/**
		 * @brief Simulates the skeleton at a specific time and returns world matrices.
		 * @param timeSecond Time in seconds.
		 * @param origin Root transform.
		 * @param transforms [out] Array to store resulting bone matrices.
		 * @param numTransform Max size of transforms array.
		 * @return Number of matrices calculated.
		 */
		int simulateTransform(float timeSecond, core::matrix4 origin, core::matrix4* transforms, int numTransform);

		/**
		 * @brief Builds a map from bone names to bone IDs.
		 * @param bones [out] Map to populate.
		 */
		void getBoneIdMap(std::map<std::string, int>& bones);

		/**
		 * @brief Applies calculated local PRS to the relative matrices of bones.
		 */
		void applyTransform();

		/**
		 * @brief Synchronizes time across multiple blended layers based on the highest weight.
		 */
		void syncAnimationByTimeScale();

		/**
		 * @brief Sets an animation clip to play on this skeleton.
		 * @param clip Pointer to the clip.
		 * @param loop Whether to loop.
		 * @param pause Whether to start paused.
		 */
		void setAnimation(CAnimationClip* clip, bool loop, bool pause = false);

		/**
		 * @brief Sets an animation clip with a specific range.
		 * @param clip Pointer to the clip.
		 * @param loop Whether to loop.
		 * @param from Start time in seconds.
		 * @param duration Playback duration in seconds.
		 * @param pause Whether to start paused.
		 */
		void setAnimation(CAnimationClip* clip, bool loop, float from, float duration, bool pause = false);

		/**
		 * @brief Sets the blending weight for ALL joints in the skeleton.
		 * @param weight Blending weight (0 to 1).
		 */
		void setJointWeights(float weight);

		/**
		 * @brief Sets the blending weight for a specific joint and its children.
		 * @param name The name of the joint.
		 * @param weight Blending weight.
		 * @param includeChild Whether to apply to children recursively.
		 */
		void setJointWeights(const char* name, float weight, bool includeChild = true);

		/**
		 * @brief Gets a list of joint data pointers starting from a named joint.
		 * @param name The name of the joint.
		 * @param joints [out] Vector to store pointers.
		 */
		void getJoints(const char* name, std::vector<CAnimationTransformData*>& joints);

		/**
		 * @brief Sets blending weights for a provided list of joints.
		 * @param joints Vector of joint data pointers.
		 * @param weight Blending weight.
		 */
		void setJointWeights(std::vector<CAnimationTransformData*>& joints, float weight);

		/**
		 * @brief Gets the joint data for a specific bone name.
		 * @param name Bone name.
		 * @return Pointer to CAnimationTransformData, or NULL if not found.
		 */
		CAnimationTransformData* getJoint(const char* name);

		/**
		 * @brief Gets the list of all joint data in the skeleton.
		 * @return Vector of joint data pointers.
		 */
		inline std::vector<CAnimationTransformData*>& getJoints()
		{
			return m_entitiesData;
		}

		/**
		 * @brief Gets the currently assigned animation clip.
		 * @return Pointer to CAnimationClip.
		 */
		inline CAnimationClip* getCurrentAnimation()
		{
			return m_clip;
		}

		/**
		 * @brief Gets the skeleton ID.
		 * @return ID.
		 */
		inline int getID()
		{
			return m_id;
		}

		/**
		 * @brief Checks if the skeleton is enabled.
		 * @return True if enabled.
		 */
		inline bool isEnable()
		{
			return m_enable;
		}

		/**
		 * @brief Sets whether the skeleton is enabled.
		 * @param b Enable state.
		 */
		inline void setEnable(bool b)
		{
			m_enable = b;
		}

		/**
		 * @brief Gets the animation timeline for this skeleton.
		 * @return Reference to CAnimationTimeline.
		 */
		inline CAnimationTimeline& getTimeline()
		{
			return m_timeline;
		}

		/**
		 * @brief Sets the operating mode of the skeleton.
		 * @param type Operating mode.
		 */
		inline void setAnimationType(EAnimationType type)
		{
			m_animationType = type;
		}

		/**
		 * @brief Gets the current operating mode.
		 * @return EAnimationType.
		 */
		inline EAnimationType getAnimationType()
		{
			return m_animationType;
		}

		/**
		 * @brief Sets the blending algorithm (Replace, Addtive, etc.) for this skeleton layer.
		 * @param type Layer blending type.
		 */
		inline void setLayerType(EAnimationLayerType type)
		{
			m_layerType = type;
		}

		/**
		 * @brief Gets the layer blending type.
		 * @return EAnimationLayerType.
		 */
		inline EAnimationLayerType getLayerType()
		{
			return m_layerType;
		}

		/**
		 * @brief Manually notifies the skeleton to refresh its active joint list.
		 */
		inline void notifyUpdateActivateEntities()
		{
			m_needUpdateActivateEntities = true;
		}

		/**
		 * @brief Sets the target skeleton that this skeleton will blend into.
		 * @param skeleton Pointer to target skeleton.
		 */
		void setTarget(CSkeleton* skeleton);

		/**
		 * @brief Gets the target skeleton.
		 * @return Pointer to target skeleton.
		 */
		inline CSkeleton* getTarget()
		{
			return m_target;
		}

		/**
		 * @brief Gets the list of layers (sub-skeletons) blending into this skeleton.
		 * @return Vector of skeleton pointers.
		 */
		inline std::vector<CSkeleton*>& getBlending()
		{
			return m_blending;
		}

		/**
		 * @brief Adds a layer to blend into this skeleton.
		 * @param skeleton Pointer to layer skeleton.
		 */
		void addBlending(CSkeleton* skeleton);

		/**
		 * @brief Removes a blended layer.
		 * @param skeleton Pointer to layer skeleton.
		 */
		void removeBlending(CSkeleton* skeleton);

		/**
		 * @brief Draws the skeleton hierarchy for debugging.
		 * @param transform Root transform matrix.
		 * @param c Color of the skeleton lines.
		 */
		void drawDebug(const core::matrix4& transform, const SColor& c);

		/**
		 * @brief Draws the default pose of the skeleton.
		 * @param transform Root transform matrix.
		 * @param c Color of the skeleton lines.
		 */
		void drawDebugDefaultSkeleton(const core::matrix4& transform, const SColor& c);

	protected:

		/**
		 * @brief Internal helper to set up keyframe data for each joint from the current clip.
		 */
		void setAnimationData();

		/**
		 * @brief Evaluates current keyframes and interpolates PRS data for all active joints.
		 */
		void updateTrackKeyFrame();

		/**
		 * @brief Orchestrates the blending of all attached layers.
		 */
		void updateBlending();

		/**
		 * @brief Performs standard linear blending from a source layer.
		 */
		void doBlending(CSkeleton* skeleton, bool first);

		/**
		 * @brief Performs additive blending from a source layer.
		 */
		void doAddtive(CSkeleton* skeleton, bool first);

		/**
		 * @brief Performs override (replace) blending from a source layer.
		 */
		void doReplace(CSkeleton* skeleton, bool first);
	};
}
