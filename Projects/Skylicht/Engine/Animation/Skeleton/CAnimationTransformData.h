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

#include "Entity/IEntityData.h"

#include "Animation/CAnimationTrack.h"
#include "Transform/CWorldTransformData.h"
#include "RenderMesh/CJointData.h"

namespace Skylicht
{
	/// @brief ECS data component that holds animation and transform state for an entity within a skeleton.
	/// @ingroup Animation
	class SKYLICHT_API CAnimationTransformData : public IEntityData
	{
	public:
		//! Name of the bone or joint.
		std::string Name;

		//! Internal entity ID.
		int ID;
		
		//! ID of the parent entity (-1 for root).
		int ParentID;
		
		//! Hierarchy depth (0 for root).
		int Depth;

		//! Index of the bone in the hardware skinning buffer.
		int BoneID;

		//! If true, animation evaluation is skipped for this entity.
		bool DisableAnimation;

		//! Blending weight for this specific bone.
		float Weight;

		//! Default local position when no animation is playing.
		core::vector3df DefaultPosition;
		
		//! Default local scale when no animation is playing.
		core::vector3df DefaultScale;
		
		//! Default local rotation when no animation is playing.
		core::quaternion DefaultRotation;

		//! Calculated local position from the animation track.
		core::vector3df AnimPosition;
		
		//! Calculated local scale from the animation track.
		core::vector3df AnimScale;
		
		//! Calculated local rotation from the animation track.
		core::quaternion AnimRotation;

		//! Pointer to the world transform ECS data for this entity.
		CWorldTransformData* WorldTransform;

		//! The animation track evaluator for this entity.
		CAnimationTrack AnimationTrack;

	public:
		CAnimationTransformData();

		virtual ~CAnimationTransformData();

		/**
		 * @brief Updates the relative matrix in WorldTransform using AnimPosition, AnimScale, and AnimRotation.
		 */
		void updateTransform();
	};

	DECLARE_PUBLIC_DATA_TYPE_INDEX(CAnimationTransformData);
}