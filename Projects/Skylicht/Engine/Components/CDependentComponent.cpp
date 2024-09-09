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
#include "CDependentComponent.h"
#include "CComponentSystem.h"
#include "Utils/CActivator.h"
#include "GameObject/CGameObject.h"

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CDependentComponent);

	CDependentComponent::CDependentComponent()
	{

	}

	CDependentComponent::~CDependentComponent()
	{

	}

	bool CDependentComponent::registerDependent(const char* componentName, const char* dependentComponent)
	{
		std::map<std::string, std::vector<std::string>>::iterator it = m_dependent.find(componentName);
		if (it != m_dependent.end())
			return false;

		m_dependent[componentName].push_back(dependentComponent);
		return true;
	}

	void CDependentComponent::createDependentComponent(CComponentSystem* component)
	{
		std::map<std::string, std::vector<std::string>>::iterator it = m_dependent.find(component->getTypeName().c_str());
		if (it == m_dependent.end())
			return;

		std::vector<std::string>& list = it->second;

		CGameObject* gameObject = component->getGameObject();

		for (std::string componentName : list)
		{
			if (gameObject->getComponentByTypeName(componentName.c_str()))
				continue;

			IActivatorObject* object = CActivator::getInstance()->createInstance(componentName.c_str());
			CComponentSystem* dependentComp = dynamic_cast<CComponentSystem*>(object);
			if (dependentComp != NULL)
			{
				gameObject->m_components.push_back(dependentComp);

				dependentComp->setOwner(gameObject);
				dependentComp->initComponent();

				component->addLinkComponent(dependentComp);
			}
			else
			{
				delete object;
			}
		}
	}
}