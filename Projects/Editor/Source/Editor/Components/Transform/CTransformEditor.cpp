/*
!@
MIT License

CopyRight (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "Transform/CTransform.h"
#include "CTransformEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/SpaceController/CSceneController.h"

namespace Skylicht
{
	namespace Editor
	{
		CTransformEditor::CTransformEditor() :
			m_gameObject(NULL),
			m_transform(NULL),
			m_skip(false),
			X(0.0f),
			Y(0.0f),
			Z(0.0f)
		{
			m_gizmos = new CTransformGizmos();
		}

		CTransformEditor::~CTransformEditor()
		{
			CSceneController::getInstance()->removeGizmos(m_gizmos);
			delete m_gizmos;
		}

		void CTransformEditor::initGUI(CComponentSystem* target, CSpaceProperty* ui)
		{
			m_gameObject = target->getGameObject();
			m_transform = dynamic_cast<CTransformEuler*>(target);

			X.removeAllObserver();
			Y.removeAllObserver();
			Z.removeAllObserver();
			m_gizmos->getPosition().removeAllObserver();

			// init ui event & sync gizmos
			if (m_gameObject->isEnableEditorChange() && m_transform != NULL)
			{
				// setup gizmos
				CSceneController::getInstance()->addGizmos(m_gizmos);

				// setup gui
				GUI::CCollapsibleGroup* group = ui->addGroup("Transform", this);

				const core::vector3df& pos = m_transform->getPosition();
				const core::vector3df& rot = m_transform->getRotation();
				const core::vector3df& scale = m_transform->getScale();

				GUI::CBoxLayout* layout = ui->createBoxLayout(group);

				X = pos.X;
				Y = pos.Y;
				Z = pos.Z;

				// Position change callback
				m_gizmos->getPosition().addObserver(new CObserver<CTransformEditor>(this,
					[x = &X, y = &Y, z = &Z](ISubject* subject, IObserver* from, CTransformEditor* target) {
						CSubject<core::vector3df>* value = (CSubject<core::vector3df>*)subject;
						const core::vector3df& pos = value->get();

						// update subject
						x->set(pos.X);
						y->set(pos.Y);
						z->set(pos.Z);

						// notify change ui
						x->notify(from);
						y->notify(from);
						z->notify(from);
					}), true);

				// input x,y,z
				ui->addNumberInput(layout, L"Position X", &X, 0.01f);
				ui->addNumberInput(layout, L"Y", &Y, 0.01f);
				ui->addNumberInput(layout, L"Z", &Z, 0.01f);

				X.addObserver(new CObserver<CTransformEditor>(this,
					[t = m_transform, g = m_gizmos](ISubject* subject, IObserver* from, CTransformEditor* target)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							core::vector3df pos = t->getPosition();
							pos.X = value->get();
							t->setPosition(pos);
							g->setPosition(pos);
						}
					}), true);

				Y.addObserver(new CObserver<CTransformEditor>(this,
					[t = m_transform, g = m_gizmos](ISubject* subject, IObserver* from, CTransformEditor* target)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							core::vector3df pos = t->getPosition();
							pos.Y = value->get();
							t->setPosition(pos);
							g->setPosition(pos);
						}
					}), true);

				Z.addObserver(new CObserver<CTransformEditor>(this,
					[t = m_transform, g = m_gizmos](ISubject* subject, IObserver* from, CTransformEditor* target)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							core::vector3df pos = t->getPosition();
							pos.Z = value->get();
							t->setPosition(pos);
							g->setPosition(pos);
						}
					}), true);

				/*
				layout->addSpace(5.0f);
				ui->addNumberInput(layout, L"Rotation(Deg) X", rot.X, 0.1f);
				ui->addNumberInput(layout, L"Y", rot.Y, 0.1f);
				ui->addNumberInput(layout, L"Z", rot.Z, 0.1f);
				layout->addSpace(5.0f);
				ui->addNumberInput(layout, L"Scale X", scale.X, 0.01f);
				ui->addNumberInput(layout, L"Y", scale.Y, 0.01f);
				ui->addNumberInput(layout, L"Z", scale.Z, 0.01f);
				*/

				group->setExpand(true);
				m_skip = false;
			}
			else
			{
				// remove current gizmos
				CSceneController::getInstance()->removeGizmos(m_gizmos);
				m_skip = true;
			}
		}

		void CTransformEditor::update()
		{

		}

		EDITOR_REGISTER(CTransformEditor, CTransform)
	}
}