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
#include "GameObject/CGameObject.h"
#include "CAnimationController.h"

#include "RenderMesh/CRenderMesh.h"
#include "VertexAnimation/CRenderMeshInstancingVAT.h"

namespace Skylicht
{
	CAnimationController::CAnimationController() :
		m_output(NULL)
	{

	}

	CAnimationController::~CAnimationController()
	{
		releaseAllSkeleton();
	}

	void CAnimationController::initComponent()
	{

	}

	void CAnimationController::updateComponent()
	{
		for (CSkeleton*& skeleton : m_skeletons)
		{
			if (skeleton->isEnable() == true && skeleton->getAnimationType() == CSkeleton::KeyFrame)
				skeleton->getTimeline().update();
		}

		for (CSkeleton*& skeleton : m_skeletons)
		{
			if (skeleton->isEnable() == true && skeleton->getAnimationType() == CSkeleton::Blending)
				skeleton->syncAnimationByTimeScale();
		}

		for (CSkeleton*& skeleton : m_skeletons)
		{
			if (skeleton->isEnable() == true)
			{
				skeleton->update();
			}
		}

		if (m_output != NULL)
			m_output->applyTransform();
	}

	CSkeleton* CAnimationController::createSkeleton()
	{
		int id = (int)m_skeletons.size();

		CSkeleton* skeleton = new CSkeleton(id);

		CRenderMesh* renderMesh = m_gameObject->getComponent<CRenderMesh>();
		if (renderMesh)
			skeleton->initSkeleton(renderMesh->getEntities());
		else
		{
			CRenderMeshInstancingVAT* renderMeshVAT = m_gameObject->getComponent<CRenderMeshInstancingVAT>();
			if (renderMeshVAT)
				skeleton->initSkeleton(renderMeshVAT->getBaseEntities());
		}

		m_skeletons.push_back(skeleton);

		if (m_output == NULL)
			m_output = skeleton;

		return skeleton;
	}

	void CAnimationController::releaseAllSkeleton()
	{
		for (CSkeleton*& skeleton : m_skeletons)
		{
			delete skeleton;
		}
		m_skeletons.clear();
		m_output = NULL;
	}
}