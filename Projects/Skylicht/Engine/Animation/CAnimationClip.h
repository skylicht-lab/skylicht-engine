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

#include "pch.h"
#include "CAnimationTrack.h"

namespace Skylicht
{
	struct SEntityAnim
	{
		std::string Name;
		CAnimationData Data;
	};

	/// @brief Represents an animation clip containing multiple animation tracks for different entities.
	/// @ingroup Animation
	class SKYLICHT_API CAnimationClip
	{
	public:
		//! The name of the animation clip.
		std::string AnimName;
		
		//! Duration of the clip in seconds.
		float Duration; // Second
		
		//! Whether the clip should loop during playback.
		bool Loop;

		//! List of entity animations (tracks) within this clip.
		std::vector<SEntityAnim*> AnimInfo;
		
		//! Map of entity animations indexed by entity name.
		std::map<std::string, SEntityAnim*> AnimNameToInfo;

		CAnimationClip()
		{
			AnimName = "";
			Duration = 0.0f;
			Loop = true;
		}

		virtual ~CAnimationClip()
		{
			releaseAllAnim();
		}

		/**
		 * @brief Releases and deletes all entity animations in this clip.
		 */
		void releaseAllAnim()
		{
			for (SEntityAnim* &i : AnimInfo)
			{
				delete i;
			}
			AnimInfo.clear();
			AnimNameToInfo.clear();
		}

		/**
		 * @brief Adds an entity animation to the clip.
		 * @param anim Pointer to the entity animation structure.
		 */
		void addAnim(SEntityAnim* anim)
		{
			for (SEntityAnim *&i : AnimInfo)
			{
				if (i->Name == anim->Name)
				{
					delete i;
					i = anim;
					AnimNameToInfo[i->Name] = anim;
					return;
				}
			}

			AnimInfo.push_back(anim);
			AnimNameToInfo[anim->Name] = anim;
		}

		/**
		 * @brief Gets the number of entity animations (tracks) in the clip.
		 * @return Track count.
		 */
		int getNodeAnimCount()
		{
			return (int)AnimInfo.size();
		}

		/**
		 * @brief Gets an entity animation by index.
		 * @param i The track index.
		 * @return Pointer to SEntityAnim.
		 */
		SEntityAnim* getAnimOfEntity(int i)
		{
			return AnimInfo[i];
		}

		/**
		 * @brief Gets an entity animation by the name of the scene node/entity.
		 * @param sceneNodeName The name of the entity.
		 * @return Pointer to SEntityAnim, or NULL if not found.
		 */
		SEntityAnim* getAnimOfEntity(const std::string &sceneNodeName)
		{
			return AnimNameToInfo[sceneNodeName];
		}

		/**
		 * @brief Gets the real-time length of the clip in milliseconds based on a target FPS.
		 * @param baseFps The reference frames per second (default: 30).
		 * @return Duration in milliseconds.
		 */
		float getRealTimeLength(float baseFps = 30.0f)
		{
			return Duration * 1000.0f / baseFps;
		}
	};

}