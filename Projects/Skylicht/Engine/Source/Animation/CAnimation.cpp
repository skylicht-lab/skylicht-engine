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

#include "pch.h"

#include "CAnimation.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	CAnimation::CAnimation()
	{
	}

	CAnimation::~CAnimation()
	{
		removeAll();
	}

	void CAnimation::removeAll()
	{
		m_clips.clear();
		m_clipName.clear();
	}

	void CAnimation::sortAnimByName()
	{
		m_clipName.clear();

		u32 numAnim = (u32)m_clips.size();
		for (u32 i = 0; i < numAnim - 1; i++)
		{
			for (u32 j = i + 1; j < numAnim; j++)
			{
				if (strcmp(m_clips[i]->AnimName.c_str(), m_clips[j]->AnimName.c_str()) > 0)
				{
					CAnimationClip *clip = m_clips[i];
					m_clips[i] = m_clips[j];
					m_clips[j] = clip;
				}
			}
		}

		for (u32 i = 0; i < numAnim; i++)
		{
			m_clipName[m_clips[i]->AnimName] = m_clips[i];
		}
	}

	void CAnimation::removeClip(const std::string& clipName)
	{
		CAnimationClip *clip = m_clipName[clipName];
		if (clip == NULL)
			return;

		std::vector<CAnimationClip*>::iterator it = std::find(m_clips.begin(), m_clips.end(), clip);
		if (it != m_clips.end())
		{
			m_clips.erase(it);
			m_clipName[clipName] = NULL;
			return;
		}
	}

	void CAnimation::renameClip(const std::string& oldName, const std::string& newName)
	{
		CAnimationClip *clip = m_clipName[oldName];
		if (clip == NULL)
			return;

		for (CAnimationClip *&i : m_clips)
		{
			if (i == clip)
			{
				i->AnimName = newName;

				m_clipName[oldName] = NULL;
				m_clipName[newName] = i;
				return;
			}
		}
	}
}