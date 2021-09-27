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
#include "CComponentSystem.h"
#include "GameObject/CGameObject.h"

namespace Skylicht
{
	CComponentSystem::CComponentSystem() :
		m_enable(true),
		m_serializable(true)
	{
		m_gameObject = NULL;
	}

	const char* CComponentSystem::getName()
	{
		return m_gameObject->getNameA();
	}

	CComponentSystem::~CComponentSystem()
	{
		for (CComponentSystem* comp : m_linkComponent)
			m_gameObject->removeComponent(comp);

		m_linkComponent.clear();
	}

	void CComponentSystem::reset()
	{

	}

	void CComponentSystem::startComponent()
	{

	}

	void CComponentSystem::postUpdateComponent()
	{

	}

	void CComponentSystem::endUpdate()
	{

	}

	void CComponentSystem::onEnable(bool b)
	{

	}

	void CComponentSystem::setEnable(bool b)
	{
		if (m_enable != b)
		{
			onEnable(b);
			m_enable = b;
		}
	}

	CObjectSerializable* CComponentSystem::createSerializable()
	{
		CObjectSerializable* object = new CObjectSerializable(getTypeName().c_str());
		object->addAutoRelease(new CBoolProperty(object, "enable", isEnable()));
		return object;
	}

	void CComponentSystem::loadSerializable(CObjectSerializable* object)
	{
		setEnable(object->get("enable", true));
	}
}