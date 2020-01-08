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
	struct SNodeAnim
	{
		std::string SceneNodeName;
		CFrameData Data;
	};

	class CAnimationClip
	{
	public:
		std::string	AnimName;
		float Time;
		float Duration;
		bool Loop;

		std::vector<SNodeAnim*> AnimInfo;
		std::map<std::string, SNodeAnim*> AnimNameToInfo;

		CAnimationClip()
		{
			AnimName = "";
			Time = 0.0f;
			Duration = 0.0f;
			Loop = true;
		}

		virtual ~CAnimationClip()
		{
			freeAllNodeAnim();
		}

		void freeAllNodeAnim()
		{
			for (SNodeAnim* &i : AnimInfo)
			{
				delete i;
			}
			AnimInfo.clear();
			AnimNameToInfo.clear();
		}

		void addNodeAnim(SNodeAnim* anim)
		{
			for (SNodeAnim *&i : AnimInfo)
			{
				if (i->SceneNodeName == anim->SceneNodeName)
				{
					delete i;
					i = anim;
					AnimNameToInfo[i->SceneNodeName] = anim;
					return;
				}
			}

			AnimInfo.push_back(anim);
			AnimNameToInfo[anim->SceneNodeName] = anim;
		}

		int getNodeAnimCount()
		{
			return (int)AnimInfo.size();
		}

		SNodeAnim* getAnimOfSceneNode(int i)
		{
			return AnimInfo[i];
		}

		SNodeAnim* getAnimOfSceneNode(const std::string &sceneNodeName)
		{
			return AnimNameToInfo[sceneNodeName];
		}

		float getRealTimeLength(float baseFps = 30.0f)
		{
			return Duration * 1000.0f / baseFps;
		}
	};

}