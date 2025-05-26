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
#include "COcclusionQueryEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Entity/CEntityHandler.h"
#include "GizmosComponents/SelectObject/CSelectObjectData.h"

namespace Skylicht
{
	namespace Editor
	{
		EDITOR_REGISTER(COcclusionQueryEditor, COcclusionQuery);

		COcclusionQueryEditor::COcclusionQueryEditor() :
			m_testVisible(false),
			m_testLabel(NULL)
		{

		}

		COcclusionQueryEditor::~COcclusionQueryEditor()
		{

		}

		void COcclusionQueryEditor::closeGUI()
		{
			CDefaultEditor::closeGUI();
			m_testLabel = NULL;
		}

		void COcclusionQueryEditor::initCustomGUI(GUI::CBoxLayout* layout, CSpaceProperty* ui)
		{
			layout->addSpace(5.0f);

			m_testLabel = ui->addLabel(layout, L"Test result: ");

			layout->addSpace(5.0f);

			ui->addButton(layout, L"Compute BBox")->OnPress = [&](GUI::CBase* button)
				{
					COcclusionQuery* query = dynamic_cast<COcclusionQuery*>(m_component);
					if (query)
					{
						CGameObject* go = m_component->getGameObject();

						CWorldTransformData* t = GET_ENTITY_DATA(go->getEntity(), CWorldTransformData);
						CSelectObjectData* s = GET_ENTITY_DATA(go->getEntity(), CSelectObjectData);

						core::aabbox3df result;

						core::matrix4 worldInv;
						t->World.getInverse(worldInv);

						bool setBox = true;

						if (s)
						{
							result = s->BBox;
							setBox = false;
						}

						CEntityHandler* entityHander = m_component->getGameObject()->getComponent<CEntityHandler>();
						if (entityHander)
						{
							for (int i = 0, n = entityHander->getEntityCount(); i < n; i++)
							{
								CEntity* entity = entityHander->getEntities()[i];
								CSelectObjectData* selectData = GET_ENTITY_DATA(entity, CSelectObjectData);
								if (selectData)
								{
									CWorldTransformData* world = GET_ENTITY_DATA(entity, CWorldTransformData);
									core::matrix4 local = world->World * worldInv;

									core::aabbox3df a = selectData->BBox;
									local.transformBoxEx(a);

									if (setBox)
									{
										result = a;
										setBox = false;
									}
									else
									{
										result.addInternalBox(a);
									}
								}
							}
						}

						query->setAABBox(result);
					}
				};

			ui->addButton(layout, L"Reset BBox")->OnPress = [&](GUI::CBase* button)
				{
					COcclusionQuery* query = dynamic_cast<COcclusionQuery*>(m_component);
					if (query)
					{
						core::aabbox3df box;
						box.MinEdge.set(-0.5f, -0.5f, -0.5f);
						box.MaxEdge.set(0.5f, 0.5f, 0.5f);
						query->setAABBox(box);
					}
				};

			layout->addSpace(5.0f);
		}

		void COcclusionQueryEditor::update()
		{
			m_testVisible = false;
			if (m_component)
			{
				COcclusionQuery* query = dynamic_cast<COcclusionQuery*>(m_component);
				if (query)
					m_testVisible = query->getVisible();
			}

			if (m_testLabel)
			{
				wchar_t log[512];
				swprintf(log, 512, L"Test result: %d", m_testVisible ? 1 : 0);
				m_testLabel->setString(log);
			}
		}
	}
}