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
#include "Transform/CTransformEuler.h"
#include "CTransformEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/SpaceController/CSceneController.h"

#include "Utils/CVector.h"

namespace Skylicht
{
	namespace Editor
	{
		EDITOR_REGISTER(CTransformEditor, CTransformEuler);

		CTransformEditor::CTransformEditor() :
			m_transform(NULL),
			m_skip(false),
			X(0.0f),
			Y(0.0f),
			Z(0.0f),
			ScaleX(1.0f),
			ScaleY(1.0f),
			ScaleZ(1.0f),
			RotateX(0.0f),
			RotateY(0.0f),
			RotateZ(0.0f)
		{
			m_gizmos = CSceneController::getInstance()->getTransformGizmos();
		}

		CTransformEditor::~CTransformEditor()
		{

		}

		void CTransformEditor::closeGUI()
		{
			m_changed = false;
			m_component = NULL;
			m_gameObject = NULL;
			m_transform = NULL;

			// clear all registered observer
			X.removeAllObserver();
			Y.removeAllObserver();
			Z.removeAllObserver();

			ScaleX.removeAllObserver();
			ScaleY.removeAllObserver();
			ScaleZ.removeAllObserver();

			RotateX.removeAllObserver();
			RotateY.removeAllObserver();
			RotateZ.removeAllObserver();

			m_gizmos->getPosition().removeAllObserver();
			m_gizmos->getRotation().removeAllObserver();
			m_gizmos->getScale().removeAllObserver();
		}

		void CTransformEditor::initGUI(CComponentSystem* target, CSpaceProperty* ui)
		{
			m_changed = false;
			m_component = target;
			m_gameObject = target->getGameObject();
			m_transform = dynamic_cast<CTransformEuler*>(target);

			// clear all registered observer
			X.removeAllObserver();
			Y.removeAllObserver();
			Z.removeAllObserver();

			ScaleX.removeAllObserver();
			ScaleY.removeAllObserver();
			ScaleZ.removeAllObserver();

			RotateX.removeAllObserver();
			RotateY.removeAllObserver();
			RotateZ.removeAllObserver();

			m_gizmos->getPosition().removeAllObserver();
			m_gizmos->getRotation().removeAllObserver();
			m_gizmos->getScale().removeAllObserver();

			// init ui event & sync gizmos
			if (m_gameObject->isEnableEditorChange() && m_transform != NULL)
			{
				// setup gui
				GUI::CCollapsibleGroup* group = ui->addGroup("Transform", this);

				const core::vector3df& pos = m_transform->getPosition();
				const core::vector3df& rot = m_transform->getRotation();
				const core::vector3df& scale = m_transform->getScale();

				GUI::CBoxLayout* layout = ui->createBoxLayout(group);

				// POSITION OBSERVER
				X = pos.X;
				Y = pos.Y;
				Z = pos.Z;

				m_gizmos->getPosition().addObserver(new CObserver([x = &X, y = &Y, z = &Z, t = m_transform, changed = &m_changed](ISubject* subject, IObserver* from)
					{
						CSubject<core::vector3df>* value = (CSubject<core::vector3df>*)subject;
						const core::vector3df& pos = value->get();

						x->set(pos.X);
						y->set(pos.Y);
						z->set(pos.Z);

						x->notify(from);
						y->notify(from);
						z->notify(from);

						t->setPosition(pos);
						*changed = true;
					}), true);

				ui->addNumberInput(layout, L"Position X", &X, 0.01f);
				ui->addNumberInput(layout, L"Y", &Y, 0.01f);
				ui->addNumberInput(layout, L"Z", &Z, 0.01f);

				X.addObserver(new CObserver([t = m_transform, g = m_gizmos, changed = &m_changed](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							core::vector3df pos = t->getPosition();
							pos.X = value->get();
							t->setPosition(pos);
							g->setPosition(pos);
							*changed = true;
						}
					}), true);

				Y.addObserver(new CObserver([t = m_transform, g = m_gizmos, changed = &m_changed](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							core::vector3df pos = t->getPosition();
							pos.Y = value->get();
							t->setPosition(pos);
							g->setPosition(pos);
							*changed = true;
						}
					}), true);

				Z.addObserver(new CObserver([t = m_transform, g = m_gizmos, changed = &m_changed](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							core::vector3df pos = t->getPosition();
							pos.Z = value->get();
							t->setPosition(pos);
							g->setPosition(pos);
							*changed = true;
						}
					}), true);

				// ROTATE OBSERVER
				layout->addSpace(5.0f);
				RotateX = rot.X;
				RotateY = rot.Y;
				RotateZ = rot.Z;

				ui->addNumberInput(layout, L"Rotation(Deg) X", &RotateX, 0.1f);
				ui->addNumberInput(layout, L"Y", &RotateY, 0.1f);
				ui->addNumberInput(layout, L"Z", &RotateZ, 0.1f);

				m_gizmos->getRotation().addObserver(new CObserver([x = &RotateX, y = &RotateY, z = &RotateZ, t = m_transform, changed = &m_changed](ISubject* subject, IObserver* from)
					{
						CSubject<core::quaternion>* value = (CSubject<core::quaternion>*)subject;

						core::vector3df rot = CVector::quaternionToEuler(value->get());

						x->set(rot.X * core::RADTODEG);
						y->set(rot.Y * core::RADTODEG);
						z->set(rot.Z * core::RADTODEG);

						x->notify(from);
						y->notify(from);
						z->notify(from);

						t->setRotation(rot * core::RADTODEG);
						*changed = true;
					}), true);

				RotateX.addObserver(new CObserver([t = m_transform, g = m_gizmos, changed = &m_changed](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							core::vector3df rot = t->getRotation();
							rot.X = value->get();
							t->setRotation(rot);
							g->setRotation(rot);
							*changed = true;
						}
					}), true);

				RotateY.addObserver(new CObserver([t = m_transform, g = m_gizmos, changed = &m_changed](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							core::vector3df rot = t->getRotation();
							rot.Y = value->get();
							t->setRotation(rot);
							g->setRotation(rot);
							*changed = true;
						}
					}), true);

				RotateZ.addObserver(new CObserver([t = m_transform, g = m_gizmos, changed = &m_changed](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							core::vector3df rot = t->getRotation();
							rot.Z = value->get();
							t->setRotation(rot);
							g->setRotation(rot);
							*changed = true;
						}
					}), true);


				// SCALE OBSERVER
				layout->addSpace(5.0f);

				ScaleX = scale.X;
				ScaleY = scale.Y;
				ScaleZ = scale.Z;

				ui->addNumberInput(layout, L"Scale X", &ScaleX, 0.01f);
				ui->addNumberInput(layout, L"Y", &ScaleY, 0.01f);
				ui->addNumberInput(layout, L"Z", &ScaleZ, 0.01f);

				m_gizmos->getScale().addObserver(new CObserver([x = &ScaleX, y = &ScaleY, z = &ScaleZ, t = m_transform, changed = &m_changed](ISubject* subject, IObserver* from)
					{
						CSubject<core::vector3df>* value = (CSubject<core::vector3df>*)subject;
						const core::vector3df& scale = value->get();

						x->set(scale.X);
						y->set(scale.Y);
						z->set(scale.Z);

						x->notify(from);
						y->notify(from);
						z->notify(from);

						t->setScale(scale);
						*changed = true;
					}), true);

				ScaleX.addObserver(new CObserver([t = m_transform, g = m_gizmos, changed = &m_changed](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							core::vector3df scale = t->getScale();
							scale.X = value->get();
							t->setScale(scale);
							g->setScale(scale);
							*changed = true;
						}
					}), true);

				ScaleY.addObserver(new CObserver([t = m_transform, g = m_gizmos, changed = &m_changed](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							core::vector3df scale = t->getScale();
							scale.Y = value->get();
							t->setScale(scale);
							g->setScale(scale);
							*changed = true;
						}
					}), true);

				ScaleZ.addObserver(new CObserver([t = m_transform, g = m_gizmos, changed = &m_changed](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							core::vector3df scale = t->getScale();
							scale.Z = value->get();
							t->setScale(scale);
							g->setScale(scale);
							*changed = true;
						}
					}), true);

				group->setExpand(true);
				m_skip = false;
			}
			else
			{
				m_skip = true;
			}
		}

		void CTransformEditor::update()
		{

		}
	}
}