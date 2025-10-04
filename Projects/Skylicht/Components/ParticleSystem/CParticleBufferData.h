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
		class COMPONENT_API CParticleBufferData : public IEntityData
		{
		public:
			core::array<CGroup*> Groups;
			core::array<CSubGroup*> SubGroups;
			core::array<CGroup*> AllGroups;
		public:
			CParticleBufferData();

			virtual ~CParticleBufferData();

			void clear();

			CGroup* createGroup();

			CGroup* getGroupByName(const wchar_t* name);

			CSubGroup* createSubGroup(CGroup* group);

			std::vector<CSubGroup*> getSubGroup(CGroup* parent);

			void removeGroup(CGroup* group);

			void removeSubGroup(CSubGroup* group);

			void bringToNext(CGroup* group, CGroup* target, bool behind);

			void updateListGroup();
		};

		DECLARE_COMPONENT_DATA_TYPE_INDEX(CParticleBufferData);
	}
}