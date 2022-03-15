/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CWorldTransformDataEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/SpaceController/CSceneController.h"

namespace Skylicht
{
	namespace Editor
	{
		ENTITYDATA_EDITOR_REGISTER(CWorldTransformDataEditor, CWorldTransformData);

		CWorldTransformDataEditor::CWorldTransformDataEditor() :
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
			m_gizmos = CSceneController::getInstance()->getWorldTransformDataGizmos();
		}

		CWorldTransformDataEditor::~CWorldTransformDataEditor()
		{

		}

		void CWorldTransformDataEditor::initGUI(IEntityData* entityData, CSpaceProperty* ui)
		{
			m_worldTransform = dynamic_cast<CWorldTransformData*>(entityData);

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

			if (m_worldTransform != NULL)
			{
				GUI::CCollapsibleGroup* group = ui->addGroup("World Transform Data", this);

				GUI::CBoxLayout* layout = ui->createBoxLayout(group);

				m_position = m_worldTransform->Relative.getTranslation();
				m_rotate = m_worldTransform->Relative.getRotationDegrees();
				m_scale = m_worldTransform->Relative.getScale();

				// POSITION OBSERVER
				X = m_position.X;
				Y = m_position.Y;
				Z = m_position.Z;

				m_gizmos->getPosition().addObserver(new CObserver([&, x = &X, y = &Y, z = &Z](ISubject* subject, IObserver* from)
					{
						CSubject<core::vector3df>* value = (CSubject<core::vector3df>*)subject;
						const core::vector3df& pos = value->get();

						x->set(pos.X);
						y->set(pos.Y);
						z->set(pos.Z);

						x->notify(from);
						y->notify(from);
						z->notify(from);

						updateMatrix();
					}), true);

				ui->addNumberInput(layout, L"Position X", &X, 0.01f);
				ui->addNumberInput(layout, L"Y", &Y, 0.01f);
				ui->addNumberInput(layout, L"Z", &Z, 0.01f);

				X.addObserver(new CObserver([&, pos = &m_position, g = m_gizmos](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							pos->X = value->get();
							updateMatrix();
							g->setPosition(*pos);
						}
					}), true);

				Y.addObserver(new CObserver([&, pos = &m_position, g = m_gizmos](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							pos->Y = value->get();
							updateMatrix();
							g->setPosition(*pos);
						}
					}), true);

				Z.addObserver(new CObserver([&, pos = &m_position, g = m_gizmos](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							pos->Z = value->get();
							updateMatrix();
							g->setPosition(*pos);
						}
					}), true);

				// ROTATE OBSERVER
				layout->addSpace(5.0f);
				RotateX = m_rotate.X;
				RotateY = m_rotate.Y;
				RotateZ = m_rotate.Z;

				m_gizmos->getRotation().addObserver(new CObserver([&, x = &RotateX, y = &RotateY, z = &RotateZ](ISubject* subject, IObserver* from)
					{
						CSubject<core::quaternion>* value = (CSubject<core::quaternion>*)subject;

						core::vector3df rot;
						value->get().toEuler(rot);

						x->set(rot.X * core::RADTODEG);
						y->set(rot.Y * core::RADTODEG);
						z->set(rot.Z * core::RADTODEG);

						x->notify(from);
						y->notify(from);
						z->notify(from);

						updateMatrix();
					}), true);

				ui->addNumberInput(layout, L"Rotation(Deg) X", &RotateX, 0.1f);
				ui->addNumberInput(layout, L"Y", &RotateY, 0.1f);
				ui->addNumberInput(layout, L"Z", &RotateZ, 0.1f);

				RotateX.addObserver(new CObserver([&, rot = &m_rotate, g = m_gizmos](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							rot->X = value->get();
							updateMatrix();
							g->setRotation(*rot);
						}
					}), true);

				RotateY.addObserver(new CObserver([&, rot = &m_rotate, g = m_gizmos](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							rot->Y = value->get();
							updateMatrix();
							g->setRotation(*rot);
						}
					}), true);

				RotateZ.addObserver(new CObserver([&, rot = &m_rotate, g = m_gizmos](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							rot->Z = value->get();
							updateMatrix();
							g->setRotation(*rot);
						}
					}), true);

				// SCALE OBSERVER
				layout->addSpace(5.0f);

				ScaleX = m_scale.X;
				ScaleY = m_scale.Y;
				ScaleZ = m_scale.Z;

				m_gizmos->getScale().addObserver(new CObserver([&, x = &ScaleX, y = &ScaleY, z = &ScaleZ](ISubject* subject, IObserver* from)
					{
						CSubject<core::vector3df>* value = (CSubject<core::vector3df>*)subject;
						const core::vector3df& scale = value->get();

						x->set(scale.X);
						y->set(scale.Y);
						z->set(scale.Z);

						x->notify(from);
						y->notify(from);
						z->notify(from);

						updateMatrix();
					}), true);

				ui->addNumberInput(layout, L"Scale X", &ScaleX, 0.01f);
				ui->addNumberInput(layout, L"Y", &ScaleY, 0.01f);
				ui->addNumberInput(layout, L"Z", &ScaleZ, 0.01f);

				ScaleX.addObserver(new CObserver([&, scale = &m_scale, g = m_gizmos](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							scale->X = value->get();
							updateMatrix();
							g->setScale(*scale);
						}
					}), true);

				ScaleY.addObserver(new CObserver([&, scale = &m_scale, g = m_gizmos](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							scale->Y = value->get();
							updateMatrix();
							g->setScale(*scale);
						}
					}), true);

				ScaleZ.addObserver(new CObserver([&, scale = &m_scale, g = m_gizmos](ISubject* subject, IObserver* from)
					{
						if (from != g)
						{
							CSubject<float>* value = (CSubject<float>*) subject;
							scale->Z = value->get();
							updateMatrix();
							g->setScale(*scale);
						}
					}), true);

				group->setExpand(true);
			}
		}

		void CWorldTransformDataEditor::closeGUI()
		{
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

		void CWorldTransformDataEditor::update()
		{

		}

		void CWorldTransformDataEditor::updateMatrix()
		{
			m_worldTransform->Relative.makeIdentity();
			m_worldTransform->Relative.setRotationDegrees(m_rotate);
			m_worldTransform->HasChanged = true;

			f32* m = m_worldTransform->Relative.pointer();

			m[0] *= m_scale.X;
			m[1] *= m_scale.X;
			m[2] *= m_scale.X;
			m[3] *= m_scale.X;

			m[4] *= m_scale.Y;
			m[5] *= m_scale.Y;
			m[6] *= m_scale.Y;
			m[7] *= m_scale.Y;

			m[8] *= m_scale.Z;
			m[9] *= m_scale.Z;
			m[10] *= m_scale.Z;
			m[11] *= m_scale.Z;

			m[12] = m_position.X;
			m[13] = m_position.Y;
			m[14] = m_position.Z;
		}
	}
}