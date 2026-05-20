/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "Particles/CGroup.h"
#include "Particles/CSubGroup.h"
#include "DataTypeIndex.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @class CParticleBufferData
		 * @brief ECS data structure holding particle groups for an entity.
		 * @details This data is managed by CParticleComponent and processed by CParticleGroupSystem and CParticleRenderer.
		 */
		class COMPONENT_API CParticleBufferData : public IEntityData
		{
		public:
			/**
			 * @brief List of top-level particle groups.
			 */
			core::array<CGroup*> Groups;

			/**
			 * @brief List of sub-particle groups (attached to parent groups).
			 */
			core::array<CSubGroup*> SubGroups;

			/**
			 * @brief Flat list of all particle groups (Groups + SubGroups).
			 */
			core::array<CGroup*> AllGroups;
		public:
			CParticleBufferData();

			virtual ~CParticleBufferData();

			/**
			 * @brief Clears all groups and sub-groups.
			 */
			void clear();

			/**
			 * @brief Creates a new top-level particle group.
			 * @return Pointer to the new CGroup.
			 */
			CGroup* createGroup();

			/**
			 * @brief Finds a group by its name.
			 * @param name The name of the group.
			 * @return Pointer to CGroup if found, NULL otherwise.
			 */
			CGroup* getGroupByName(const wchar_t* name);

			/**
			 * @brief Creates a new sub-group attached to a parent group.
			 * @param group The parent group.
			 * @return Pointer to the new CSubGroup.
			 */
			CSubGroup* createSubGroup(CGroup* group);

			/**
			 * @brief Gets all sub-groups belonging to a specific parent.
			 * @param parent The parent group.
			 * @return Vector of CSubGroup pointers.
			 */
			std::vector<CSubGroup*> getSubGroup(CGroup* parent);

			/**
			 * @brief Removes and deletes a top-level group and its sub-groups.
			 * @param group The group to remove.
			 */
			void removeGroup(CGroup* group);

			/**
			 * @brief Removes and deletes a specific sub-group.
			 * @param group The sub-group to remove.
			 */
			void removeSubGroup(CSubGroup* group);

			/**
			 * @brief Reorders a group within the Groups list.
			 * @param group The group to move.
			 * @param target The target group for positioning.
			 * @param behind If true, places it after the target; otherwise before.
			 */
			void bringToNext(CGroup* group, CGroup* target, bool behind);

			/**
			 * @brief Rebuilds the AllGroups flat list from Groups and SubGroups.
			 */
			void updateListGroup();
		};

		DECLARE_COMPONENT_DATA_TYPE_INDEX(CParticleBufferData);
	}
}