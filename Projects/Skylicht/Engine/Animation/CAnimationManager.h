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

#include "Utils/CSingleton.h"
#include "CAnimation.h"
#include "CAnimationClip.h"

#include "Importer/IAnimationImporter.h"
#include "Exporter/IAnimationExporter.h"

namespace Skylicht
{
	/**
	 * @brief Singleton manager for loading, caching, and exporting animations.
	 * @ingroup Animation
	 * 
	 * Example: Loading and playing an animation
	 * @code
	 * // Load animation clip from file (.dae, .fbx, or .sanim)
	 * CAnimationClip* clip = CAnimationManager::getInstance()->loadAnimation("Hero@Idle.sanim");
	 * 
	 * if (clip)
	 * {
	 *     // Apply to a skeleton
	 *     skeleton->setAnimation(clip, true);
	 * }
	 * @endcode
	 */
	class SKYLICHT_API CAnimationManager
	{
	public:
		DECLARE_SINGLETON(CAnimationManager)

	protected:
		//! Cached animation clips indexed by resource path.
		std::map<std::string, CAnimationClip*> m_clips;
		
		//! Cached animation containers indexed by name.
		std::map<std::string, CAnimation*> m_animations;

	public:
		CAnimationManager();

		virtual ~CAnimationManager();

		/**
		 * @brief Creates or retrieves a named animation container.
		 * @param name The unique name for the container.
		 * @return Pointer to CAnimation.
		 */
		CAnimation* createAnimation(const char* name);

		/**
		 * @brief Gets a named animation container.
		 * @param animName The name of the container.
		 * @return Pointer to CAnimation, or NULL if not found.
		 */
		CAnimation* getAnimation(const char* animName)
		{
			return m_animations[animName];
		}

		/**
		 * @brief Loads an animation clip from a file.
		 * @param resource Path to the animation file (.dae, .fbx, .sanim).
		 * @return Pointer to the loaded CAnimationClip, or NULL on failure.
		 */
		CAnimationClip* loadAnimation(const char* resource);

		/**
		 * @brief Loads an animation clip using a specific importer.
		 * @param resource Path or identifier for the animation resource.
		 * @param importer Pointer to the importer implementation.
		 * @return Pointer to the loaded CAnimationClip, or NULL on failure.
		 */
		CAnimationClip* loadAnimation(const char* resource, IAnimationImporter* importer);

		/**
		 * @brief Exports an animation clip to a file.
		 * @param clip Pointer to the clip to export.
		 * @param output Destination file path.
		 * @return True on success, false on failure.
		 */
		bool exportAnimation(CAnimationClip* clip, const char* output);

		/**
		 * @brief Exports an animation clip using a specific exporter.
		 * @param clip Pointer to the clip to export.
		 * @param output Destination file path.
		 * @param exporter Pointer to the exporter implementation.
		 * @return True on success, false on failure.
		 */
		bool exportAnimation(CAnimationClip* clip, const char* output, IAnimationExporter* exporter);

		/**
		 * @brief Releases all cached animation clips.
		 */
		void releaseAllClips();

		/**
		 * @brief Releases all cached animation containers.
		 */
		void releaseAllAnimations();
	};
}