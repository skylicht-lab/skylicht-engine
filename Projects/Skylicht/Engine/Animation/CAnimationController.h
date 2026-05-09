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

#include "Skeleton/CSkeleton.h"
#include "Components/CComponentSystem.h"

namespace Skylicht
{
	/**
	 * @brief Component that manages and updates animation skeletons for a GameObject.
	 * @ingroup Animation
	 * 
	 * This component acts as a container for one or more CSkeleton objects.
	 * It is responsible for updating the animation state and applying transforms to the entities.
	 * 
	 * Example: Setting up animation layers
	 * @code
	 * CAnimationController* animController = gameObject->addComponent<CAnimationController>();
	 * 
	 * // Create base skeleton (output)
	 * CSkeleton* result = animController->createSkeleton();
	 * result->setAnimationType(CSkeleton::Blending);
	 * animController->setOutput(result);
	 * 
	 * // Create a movement layer
	 * CSkeleton* movement = animController->createSkeleton();
	 * movement->setTarget(result);
	 * 
	 * // Create an action layer (e.g., Attack)
	 * CSkeleton* attack = animController->createSkeleton();
	 * attack->setTarget(result);
	 * attack->setLayerType(CSkeleton::Replace); // Overrides movement
	 * @endcode
	 */
	class SKYLICHT_API CAnimationController : public CComponentSystem
	{
	protected:
		//! List of skeletons managed by this controller.
		std::vector<CSkeleton*> m_skeletons;

		//! The output skeleton which transforms are applied from.
		CSkeleton* m_output;

		//! Path to the primary animation file.
		std::string m_animFile;
		
		//! Whether the default animation should loop.
		bool m_loop;

	public:
		CAnimationController();

		virtual ~CAnimationController();

		virtual void initComponent();

		virtual void updateComponent();

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CAnimationController)

	public:

		/**
		 * @brief Creates a new skeleton managed by this controller, using the GameObject's entities.
		 * @return Pointer to the newly created skeleton.
		 */
		CSkeleton* createSkeleton();

		/**
		 * @brief Creates a new skeleton managed by this controller, using the provided entities.
		 * @param entities List of entities to initialize the skeleton with.
		 * @return Pointer to the newly created skeleton.
		 */
		CSkeleton* createSkeleton(core::array<CEntity*>& entities);

		/**
		 * @brief Gets the number of skeletons managed by this controller.
		 * @return Skeleton count.
		 */
		int getNumSkeleton()
		{
			return (int)m_skeletons.size();
		}

		/**
		 * @brief Gets a skeleton by index.
		 * @param id The skeleton index.
		 * @return Pointer to the skeleton.
		 */
		CSkeleton* getSkeleton(int id)
		{
			return m_skeletons[id];
		}

		/**
		 * @brief Releases all skeletons and clears the controller.
		 */
		void releaseAllSkeleton();

		/**
		 * @brief Sets the skeleton that will be used for final transform application.
		 * @param skeleton Pointer to the output skeleton.
		 */
		inline void setOutput(CSkeleton* skeleton)
		{
			m_output = skeleton;
		}
	};
}