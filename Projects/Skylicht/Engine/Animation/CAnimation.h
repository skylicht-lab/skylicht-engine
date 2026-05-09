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
#include "CAnimationClip.h"

namespace Skylicht
{

	/// @brief Container for a collection of animation clips.
	/// @ingroup Animation
	class SKYLICHT_API CAnimation
	{
	protected:
		std::vector<CAnimationClip*> m_clips;
		std::map<std::string, CAnimationClip*> m_clipName;

	public:
		CAnimation();

		virtual ~CAnimation();

		/**
		 * @brief Gets an animation clip by name.
		 * @param lpAnimName The name of the animation.
		 * @return Pointer to CAnimationClip, or NULL if not found.
		 */
		CAnimationClip* getAnim(const char *lpAnimName)
		{
			return m_clipName[lpAnimName];
		}

		/**
		 * @brief Gets an animation clip by index.
		 * @param animID The index of the animation.
		 * @return Pointer to CAnimationClip.
		 */
		CAnimationClip* getAnim(int animID)
		{
			return m_clips[animID];
		}

		/**
		 * @brief Gets the total number of animation clips.
		 * @return Number of clips.
		 */
		int getAnimCount()
		{
			return (int)m_clips.size();
		}

		/**
		 * @brief Adds a new animation clip to the collection.
		 * @param clip Pointer to the clip to add.
		 */
		void addClip(CAnimationClip* clip)
		{
			if (clip == NULL)
				return;

			m_clips.push_back(clip);
			m_clipName[clip->AnimName] = clip;
		}

		/**
		 * @brief Sorts animation clips alphabetically by name.
		 */
		void sortAnimByName();

		/**
		 * @brief Returns the internal vector of animation clips.
		 * @return Pointer to vector of clips.
		 */
		std::vector<CAnimationClip*>* getAllAnimClip()
		{
			return &m_clips;
		}

		/**
		 * @brief Returns the internal map of animation clips.
		 * @return Pointer to map of clips indexed by name.
		 */
		std::map<std::string, CAnimationClip*>* getAllAnimNameClip()
		{
			return &m_clipName;
		}

		/**
		 * @brief Removes all animation clips from the collection.
		 */
		void removeAll();

		/**
		 * @brief Removes a specific animation clip by name.
		 * @param clipName The name of the clip to remove.
		 */
		void removeClip(const std::string& clipName);

		/**
		 * @brief Renames an animation clip.
		 * @param oldName The current name of the clip.
		 * @param newName The new name for the clip.
		 */
		void renameClip(const std::string& oldName, const std::string& newName);

	};

}