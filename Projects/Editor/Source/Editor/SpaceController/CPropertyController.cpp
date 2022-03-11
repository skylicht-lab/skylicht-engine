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
		CPropertyController::CPropertyController()
		{

		}

		CPropertyController::~CPropertyController()
		{

		}

		void CPropertyController::setProperty(CSelectObject* object)
		{
			CSceneController* sceneController = CSceneController::getInstance();
			CScene* scene = sceneController->getScene();

			CSpaceProperty* spaceProperty = (CSpaceProperty*)CEditor::getInstance()->getWorkspaceByName(L"Property");
			if (spaceProperty == NULL)
				return;

			if (object != NULL)
			{
				if (object->getType() == CSelectObject::GameObject)
				{
					CGameObject* obj = scene->searchObjectInChildByID(object->getID().c_str());
					if (obj != NULL)
					{
						// Name and icon
						spaceProperty->setIcon(GUI::ESystemIcon::Res3D);
						spaceProperty->setLabel(obj->getName());

						// Clear old ui
						spaceProperty->clearAllGroup();

						// When change the name
						object->addObserver(new CObserver([spaceProperty, obj](ISubject* subject, IObserver* from)
							{
								spaceProperty->setLabel(obj->getName());
							}), true);

						// Tabable
						spaceProperty->getWindow()->getCanvas()->TabableGroup.clear();

						// Activator
						CEditorActivator* activator = CEditorActivator::getInstance();

						// GameObject property
						CComponentEditor* editor = activator->getEditorInstance(obj->getTypeName().c_str());
						if (editor != NULL)
							spaceProperty->addComponent(editor, obj);

						// Component property					
						ArrayComponent& listComponents = obj->getListComponent();
						for (CComponentSystem* component : listComponents)
						{
							CComponentEditor* editor = activator->getEditorInstance(component->getTypeName().c_str());
							if (editor != NULL)
							{
								// custom editor
								spaceProperty->addComponent(editor, component);
							}
							else if (component->isSerializable())
							{
								// default editor
								CDefaultEditor* defaultEditor = new CDefaultEditor();

								// check unsupported component
								CNullComponent* nullComp = dynamic_cast<CNullComponent*>(component);
								if (nullComp != NULL)
								{
									std::string name = nullComp->getName() + std::string(" (Unsupported)");
									defaultEditor->setName(name.c_str());
								}

								// set default editor
								spaceProperty->addComponent(defaultEditor, component, true);
							}
						}

						// add button "add" to popup menu component
						bool isZone = dynamic_cast<CZone*>(obj) != NULL;
						if (!isZone)
						{
							GUI::CButton* button = spaceProperty->addButton(L"Add");
							button->OnPress = [&, obj, spaceProperty](GUI::CBase* button)
							{
								spaceProperty->popupComponentMenu(obj, button);
							};
						}
					}
				}
				else if (object->getType() == CSelectObject::Entity)
				{
					// Name and icon
					spaceProperty->setIcon(GUI::ESystemIcon::Poly);
					spaceProperty->setLabel(L"Entity");

					// Clear old ui
					spaceProperty->clearAllGroup();

					// Tabable
					spaceProperty->getWindow()->getCanvas()->TabableGroup.clear();

					CEntity* entity = scene->getEntityManager()->getEntityByID(object->getID().c_str());
					if (entity != NULL)
					{
						// Activator
						CEditorActivator* activator = CEditorActivator::getInstance();

						int dataCount = entity->getDataCount();
						for (int i = 0; i < dataCount; i++)
						{
							IEntityData* data = entity->getData(i);

							// GameObject property							
							CEntityDataEditor* editor = activator->getEntityDataEditorInstance(data->getTypeName().c_str());
							if (editor != NULL)
							{
								spaceProperty->addEntityData(editor, data);
							}
						}
					}
				}
			}
			else
			{
				spaceProperty->setIcon(GUI::ESystemIcon::None);
				spaceProperty->setLabel(L"");
				spaceProperty->clearAllGroup();
			}

			// force update layout
			spaceProperty->getWindow()->forceUpdateLayout();
		}
	}
}