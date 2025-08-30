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
#include "CParticleEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/SpaceController/CParticleController.h"
#include "Editor/SpaceController/CSceneController.h"

#include "ParticleSystem/Particles/Emitters/CDirectionEmitter.h"
#include "ParticleSystem/Particles/CGroup.h"
#include "ParticleSystem/Particles/CModel.h"

namespace Skylicht
{
	namespace Editor
	{
		CParticleEditor::CParticleEditor() :
			m_ps(NULL),
			m_data(NULL),
			m_isChanged(false)
		{

		}

		CParticleEditor::~CParticleEditor()
		{
			closeGUI();
		}

		void CParticleEditor::closeGUI()
		{
			if (m_data)
				delete m_data;

			m_data = NULL;
			m_ps = NULL;
		}

		void CParticleEditor::initGUI(Particle::CParticleSerializable* ps, CSpaceProperty* ui)
		{
			m_ps = ps;
			m_data = ps->createSerializable();

			std::string name = m_data->get("name", std::string());

			GUI::CCollapsibleGroup* group = ui->addGroup(name.c_str(), this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);
			serializableToControl(m_data, ui, layout);
			group->setExpand(true);
		}

		void CParticleEditor::onUpdateValue(CObjectSerializable* object)
		{
			m_isChanged = true;
			m_ps->loadSerializable(m_data);

			CParticleController::getInstance()->updateGroupName();

			CParticleGizmos* gizmos = CSceneController::getInstance()->getParticleGizmos();
			CParticleGizmos::EState state = gizmos->getState();

			switch (state)
			{
			case CParticleGizmos::Emitter:
			{
				Particle::CDirectionEmitter* directionEmitter = dynamic_cast<Particle::CDirectionEmitter*>(m_ps);
				if (directionEmitter)
					gizmos->setRotation(directionEmitter->getRotation());
			}
			break;
			case CParticleGizmos::Zone:
			{
				Particle::CPositionZone* positionZone = dynamic_cast<Particle::CPositionZone*>(m_ps);
				if (positionZone)
					gizmos->setPosition(positionZone->getPosition());
				else
					gizmos->refresh();
			}
			break;
			case CParticleGizmos::Gravity:
			case CParticleGizmos::Orientation:
			{
				Particle::CGroup* group = dynamic_cast<Particle::CGroup*>(m_ps);
				if (group)
				{
					if (state == CParticleGizmos::Gravity)
						gizmos->setRotation(core::quaternion(group->GravityRotation));
					else
						gizmos->setRotation(core::quaternion(group->ParticleRotation));
				}
				else
				{
					gizmos->refresh();
				}
			}
			break;
			default:
				break;
			}
		}

		void CParticleEditor::initCustomValueGUI(CObjectSerializable* object, CValueProperty* data, GUI::CBoxLayout* layout, CSpaceProperty* ui)
		{
			// Custom color picker for model RGB
			Particle::CModel* model = dynamic_cast<Particle::CModel*>(m_ps);
			if (model)
			{
				if (model->getType() == Particle::ColorR ||
					model->getType() == Particle::ColorG ||
					model->getType() == Particle::ColorB)
				{
					Particle::CGroup* group = model->getGroup();

					Particle::CModel* r = group->getModel(Particle::ColorR);
					Particle::CModel* g = group->getModel(Particle::ColorG);
					Particle::CModel* b = group->getModel(Particle::ColorB);

					if (data->Name == "start1" ||
						data->Name == "start2" ||
						data->Name == "end1" ||
						data->Name == "end2")
					{
						int state = 0;
						if (data->Name == "start2")
							state = 1;
						else if (data->Name == "end1")
							state = 2;
						else if (data->Name == "end2")
							state = 3;

						SColor c = getColor(r, g, b, state);

						GUI::CLayout* row = layout->beginVertical();
						GUI::CBase* space = new GUI::CBase(row);

						GUI::CColorPicker* colorPicker = new GUI::CColorPicker(row);
						colorPicker->setColor(GUI::SGUIColor(255, c.getRed(), c.getGreen(), c.getBlue()));
						colorPicker->OnChanged = [&, colorPicker, r, g, b, state, model, data](GUI::CBase* base)
							{
								const GUI::SGUIColor& guiColor = colorPicker->getColor();
								setColor(SColor(255, guiColor.R, guiColor.G, guiColor.B), r, g, b, state);

								CFloatProperty* floatProperty = dynamic_cast<CFloatProperty*>(data);
								if (floatProperty)
								{
									if (model->getType() == Particle::ColorR)
										floatProperty->set(guiColor.R / 255.0f);
									else if (model->getType() == Particle::ColorB)
										floatProperty->set(guiColor.G / 255.0f);
									else if (model->getType() == Particle::ColorB)
										floatProperty->set(guiColor.B / 255.0f);

									if (floatProperty->OnChanged != nullptr)
										floatProperty->OnChanged();
								}
							};

						layout->endVertical();
						layout->addSpace(5.0f);
					}
				}
			}
		}

		SColor CParticleEditor::getColor(Particle::CModel* r, Particle::CModel* g, Particle::CModel* b, int state)
		{
			u32 red = 255, green = 255, blue = 255;
			if (r)
			{
				if (state == 0)
					red = (u32)(r->getStartValue1() * 255.0f);
				else if (state == 1)
					red = (u32)(r->getStartValue2() * 255.0f);
				else if (state == 2)
					red = (u32)(r->getEndValue1() * 255.0f);
				else
					red = (u32)(r->getEndValue2() * 255.0f);
			}

			if (g)
			{
				if (state == 0)
					green = (u32)(g->getStartValue1() * 255.0f);
				else if (state == 1)
					green = (u32)(g->getStartValue2() * 255.0f);
				else if (state == 2)
					green = (u32)(g->getEndValue1() * 255.0f);
				else
					green = (u32)(g->getEndValue2() * 255.0f);
			}

			if (b)
			{
				if (state == 0)
					blue = (u32)(b->getStartValue1() * 255.0f);
				else if (state == 1)
					blue = (u32)(b->getStartValue2() * 255.0f);
				else if (state == 2)
					blue = (u32)(b->getEndValue1() * 255.0f);
				else
					blue = (u32)(b->getEndValue2() * 255.0f);
			}

			return SColor(255, red, green, blue);
		}

		void CParticleEditor::setColor(const SColor& c, Particle::CModel* r, Particle::CModel* g, Particle::CModel* b, int state)
		{
			float red = c.getRed() / 255.0f, green = c.getGreen() / 255.0f, blue = c.getBlue() / 255.0f;

			if (r)
			{
				if (state == 0)
					r->setStartValue1(red);
				else if (state == 1)
					r->setStartValue2(red);
				else if (state == 2)
					r->setEndValue1(red);
				else
					r->setEndValue2(red);
			}

			if (g)
			{
				if (state == 0)
					g->setStartValue1(green);
				else if (state == 1)
					g->setStartValue2(green);
				else if (state == 2)
					g->setEndValue1(green);
				else
					g->setEndValue2(green);
			}

			if (b)
			{
				if (state == 0)
					b->setStartValue1(blue);
				else if (state == 1)
					b->setStartValue2(blue);
				else if (state == 2)
					b->setEndValue1(blue);
				else
					b->setEndValue2(blue);
			}
		}
	}
}