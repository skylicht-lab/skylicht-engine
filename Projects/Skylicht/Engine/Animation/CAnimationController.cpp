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
#include "CAnimationManager.h"

#include "RenderMesh/CRenderMesh.h"
#include "MeshManager/CMeshManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CAnimationController);

	CATEGORY_COMPONENT(CAnimationController, "Animation Controller", "Animation");

	CAnimationController::CAnimationController() :
		m_output(NULL),
		m_loop(true)
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

	CObjectSerializable* CAnimationController::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();
		object->autoRelease(new CFilePathProperty(object, "animation", m_animFile.c_str(), CMeshManager::getMeshExts()));
		object->autoRelease(new CBoolProperty(object, "loop", m_loop));
		return object;
	}

	void CAnimationController::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		std::string animFile = object->get<std::string>("animation", "");
		m_loop = object->get<bool>("loop", true);

		if (!animFile.empty() && m_animFile != animFile)
		{
			m_animFile = animFile;

			CAnimationClip* clip = CAnimationManager::getInstance()->loadAnimation(m_animFile.c_str());
			if (clip)
			{
				releaseAllSkeleton();

				CSkeleton* skeleton = createSkeleton();
				skeleton->setAnimation(clip, m_loop);
			}
		}

		if (m_skeletons.size() > 0)
			m_skeletons[0]->getTimeline().Loop = m_loop;
	}

	CSkeleton* CAnimationController::createSkeleton()
	{
		int id = (int)m_skeletons.size();

		CSkeleton* skeleton = new CSkeleton(id);

		CRenderMesh* renderMesh = m_gameObject->getComponent<CRenderMesh>();
		if (renderMesh)
			skeleton->initSkeleton(renderMesh->getEntities());
		else
			os::Printer::log("[CAnimationController] createSkeleton with no RenderMesh");

		m_skeletons.push_back(skeleton);

		if (m_output == NULL)
			m_output = skeleton;

		return skeleton;
	}

	CSkeleton* CAnimationController::createSkeleton(core::array<CEntity*>& entities)
	{
		int id = (int)m_skeletons.size();

		CSkeleton* skeleton = new CSkeleton(id);
		skeleton->initSkeleton(entities);

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