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

#include "pch.h"
#include "CParticleBufferData.h"

namespace Skylicht
{
	namespace Particle
	{
		IMPLEMENT_DATA_TYPE_INDEX(CParticleBufferData);

		CParticleBufferData::CParticleBufferData()
		{

		}

		CParticleBufferData::~CParticleBufferData()
		{
			clear();
		}

		void CParticleBufferData::clear()
		{
			for (u32 i = 0, n = Groups.size(); i < n; i++)
				delete Groups[i];
			Groups.clear();
		}

		CGroup* CParticleBufferData::createGroup()
		{
			CGroup* g = new CGroup();
			Groups.push_back(g);
			return g;
		}

		CGroup* CParticleBufferData::getGroupByName(const wchar_t* name)
		{
			for (u32 i = 0, n = Groups.size(); i < n; i++)
			{
				if (Groups[i]->Name == name)
					return Groups[i];
			}
			return NULL;
		}

		CSubGroup* CParticleBufferData::createSubGroup(CGroup* group)
		{
			CSubGroup* g = new CSubGroup(group);
			Groups.push_back(g);
			return g;
		}

		void CParticleBufferData::removeGroup(CGroup* group)
		{
			int index = Groups.linear_search(group);
			if (index >= 0)
			{
				Groups.erase(index);
				delete group;
			}
		}

		void CParticleBufferData::bringToNext(CGroup* group, CGroup* target, bool behind)
		{
			int index = Groups.linear_search(group);
			if (index == -1)
				return;

			Groups.erase(index);

			int newIndex = Groups.linear_search(target);
			if (newIndex == -1)
			{
				Groups.push_back(group);
				return;
			}

			if (behind)
				++newIndex;

			Groups.insert(group, newIndex);
		}
	}
}