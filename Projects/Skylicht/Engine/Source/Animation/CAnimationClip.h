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

	class SKYLICHT_API CAnimationClip
	{
	public:
		std::string AnimName;
		float Duration; // Second
		bool Loop;

		std::vector<SEntityAnim*> AnimInfo;
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

		void releaseAllAnim()
		{
			for (SEntityAnim* &i : AnimInfo)
			{
				delete i;
			}
			AnimInfo.clear();
			AnimNameToInfo.clear();
		}

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

		int getNodeAnimCount()
		{
			return (int)AnimInfo.size();
		}

		SEntityAnim* getAnimOfEntity(int i)
		{
			return AnimInfo[i];
		}

		SEntityAnim* getAnimOfEntity(const std::string &sceneNodeName)
		{
			return AnimNameToInfo[sceneNodeName];
		}

		float getRealTimeLength(float baseFps = 30.0f)
		{
			return Duration * 1000.0f / baseFps;
		}
	};

}