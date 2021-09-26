/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CPropertyController.h"
#include "CSceneController.h"

#include "Activator/CEditorActivator.h"
#include "Editor/Components/CComponentEditor.h"
#include "Editor/Components/Default/CDefaultEditor.h"

#include "Reactive/CObserver.h"

namespace Skylicht
{
	namespace Editor
	{
		CPropertyController::CPropertyController() :
			m_spaceProperty(NULL)
		{

		}

		CPropertyController::~CPropertyController()
		{

		}

		void CPropertyController::setProperty(CSelectObject* object)
		{
			CSceneController* sceneController = CSceneController::getInstance();
			CScene* scene = sceneController->getScene();

			if (object != NULL && object->getType() == CSelectObject::GameObject)
			{
				CGameObject* obj = scene->searchObjectInChildByID(object->getID().c_str());
				if (obj != NULL)
				{
					// Name and icon
					m_spaceProperty->setIcon(GUI::ESystemIcon::Res3D);
					m_spaceProperty->setLabel(obj->getName());

					// Clear old ui
					m_spaceProperty->clearAllGroup();

					// When change the name
					object->addObserver(new CObserver<CGameObject>(obj, [space = m_spaceProperty](ISubject* subject, IObserver* from, CGameObject* target) {
						space->setLabel(target->getName());
						}), true);

					// Tabable
					m_spaceProperty->getWindow()->getCanvas()->TabableGroup.clear();

					// Activator
					CEditorActivator* activator = CEditorActivator::getInstance();

					// GameObject property
					CComponentEditor* editor = activator->getEditorInstance(obj->getTypeName().c_str());
					if (editor != NULL)
						m_spaceProperty->addComponent(editor, obj);

					// Component property					
					ArrayComponent& listComponents = obj->getListComponent();
					for (CComponentSystem* component : listComponents)
					{
						CComponentEditor* editor = activator->getEditorInstance(component->getTypeName().c_str());
						if (editor != NULL)
						{
							// custom editor
							m_spaceProperty->addComponent(editor, component);
						}
						else if (component->isSerializable())
						{
							// default editor
							m_spaceProperty->addComponent(new CDefaultEditor(), component, true);
						}
					}
				}
			}
			else
			{
				m_spaceProperty->setIcon(GUI::ESystemIcon::None);
				m_spaceProperty->setLabel(L"");
				m_spaceProperty->clearAllGroup();
			}

			// force update layout
			m_spaceProperty->getWindow()->forceUpdateLayout();
		}
	}
}