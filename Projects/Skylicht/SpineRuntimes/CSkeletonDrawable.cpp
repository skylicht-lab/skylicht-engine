/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "CSkeletonDrawable.h"

using namespace Skylicht;

spine::SpineExtension* spine::getDefaultExtension() {
	return new spine::DefaultSpineExtension();
}

namespace spine
{
	CSkeletonDrawable::CSkeletonDrawable(spine::SkeletonData* skeletonData, spine::AnimationStateData* animationStateData) :
		m_skeleton(NULL),
		m_animationState(NULL),
		m_usePremultipliedAlpha(false),
		m_ownsAnimationStateData(false)
	{
		m_skeleton = new spine::Skeleton(skeletonData);

		m_ownsAnimationStateData = animationStateData == NULL;
		if (m_ownsAnimationStateData)
			animationStateData = new spine::AnimationStateData(skeletonData);

		m_animationState = new spine::AnimationState(animationStateData);
	}

	CSkeletonDrawable::~CSkeletonDrawable()
	{
		if (m_ownsAnimationStateData)
			delete m_animationState->getData();

		delete m_animationState;
		delete m_skeleton;
	}

	void CSkeletonDrawable::update(float delta, spine::Physics physics)
	{
		m_animationState->update(delta);
		m_animationState->apply(*m_skeleton);
		m_skeleton->update(delta);
		m_skeleton->updateWorldTransform(physics);
	}

	void CSkeletonDrawable::render()
	{

	}
}