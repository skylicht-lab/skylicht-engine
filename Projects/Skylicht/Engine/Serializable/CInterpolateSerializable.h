/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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

#pragma once

#include "CObjectSerializable.h"
#include "CArraySerializable.h"
#include "Utils/CInterpolator.h"

namespace Skylicht
{
	template <class T>
	class SKYLICHT_API CInterpolateSerializable : public CObjectSerializable
	{
	protected:
		CArrayTypeSerializable<CFloatProperty> m_valueX;
		CArrayTypeSerializable<T> m_valueY;

	public:
		CInterpolateSerializable(const char* name) :
			CObjectSerializable(name),
			m_valueX("valueX", this),
			m_valueY("valueY", this)
		{
			m_objectType = ObjectInterpolate;
		}

		CInterpolateSerializable(const char* name, CObjectSerializable* parent) :
			CObjectSerializable(name, parent),
			m_valueX("valueX", this),
			m_valueY("valueY", this)
		{
			m_objectType = ObjectInterpolate;
		}

		virtual ~CInterpolateSerializable()
		{

		}

		inline int count()
		{
			return m_valueX.getElementCount();
		}

		virtual void clear()
		{
			m_valueX.clear();
			m_valueY.clear();
		}

		virtual void setInterpolator(const CInterpolator* interpolator) = 0;

		virtual void getInterpolator(CInterpolator* interpolator) = 0;
	};


	class SKYLICHT_API CInterpolateFloatSerializable : public CInterpolateSerializable<CFloatProperty>
	{
	protected:
		CArrayTypeSerializable<CVector2Property> m_control;
		CArrayTypeSerializable<CVector2Property> m_controlL;
		CArrayTypeSerializable<CVector2Property> m_controlR;
		CArrayTypeSerializable<CIntProperty> m_controlType;

	public:
		CInterpolateFloatSerializable(const char* name) :
			CInterpolateSerializable<CFloatProperty>(name),
			m_control("control", this),
			m_controlL("controlLeft", this),
			m_controlR("controlRight", this),
			m_controlType("controlType", this)
		{
		}

		CInterpolateFloatSerializable(const char* name, CObjectSerializable* parent) :
			CInterpolateSerializable<CFloatProperty>(name, parent),
			m_control("control", this),
			m_controlL("controlLeft", this),
			m_controlR("controlRight", this),
			m_controlType("controlType", this)
		{
		}

		virtual ~CInterpolateFloatSerializable()
		{

		}

		virtual void clear()
		{
			m_valueX.clear();
			m_valueY.clear();
			m_control.clear();
			m_controlL.clear();
			m_controlR.clear();
			m_controlType.clear();
		}

		inline void addEntry(float x, float y)
		{
			CFloatProperty* valueX = (CFloatProperty*)m_valueX.createElement();
			valueX->set(x);

			CFloatProperty* valueY = (CFloatProperty*)m_valueY.createElement();
			valueY->set(y);
		}

		inline void addControl(const SControlPoint& p)
		{
			CVector2Property* control = (CVector2Property*)m_control.createElement();
			control->set(p.Position);

			CVector2Property* left = (CVector2Property*)m_controlL.createElement();
			left->set(p.Left);

			CVector2Property* right = (CVector2Property*)m_controlR.createElement();
			right->set(p.Right);

			CIntProperty* type = (CIntProperty*)m_controlType.createElement();
			type->set(p.Type);
		}

		virtual void setInterpolator(const CInterpolator* interpolator)
		{
			const std::set<SInterpolatorEntry>& graph = interpolator->getGraph();
			const std::vector<SControlPoint>& controls = interpolator->getControlPoints();

			clear();
			for (auto& i : graph)
			{
				addEntry(i.X, i.Value[0]);
			}

			for (auto& i : controls)
			{
				addControl(i);
			}
		}

		virtual void getInterpolator(CInterpolator* interpolator)
		{
			interpolator->clearGraph();

			for (int i = 0, n = m_valueX.getElementCount(); i < n; i++)
			{
				float x = m_valueX.getElementValue(i, CFloatProperty()).get();
				float y = m_valueY.getElementValue(i, CFloatProperty()).get();

				interpolator->addEntry(x, y);
			}

			for (int i = 0, n = m_control.getElementCount(); i < n; i++)
			{
				SControlPoint& control = interpolator->addControlPoint();

				control.Position = m_control.getElementValue(i, CVector2Property()).get();
				control.Left = m_controlL.getElementValue(i, CVector2Property()).get();
				control.Right = m_controlR.getElementValue(i, CVector2Property()).get();
				control.Type = (SControlPoint::EControlType)m_controlType.getElementValue(i, CIntProperty()).get();
			}

			interpolator->setType(CInterpolator::Float);
		}

		virtual CObjectSerializable* clone()
		{
			CInterpolateFloatSerializable* object = new CInterpolateFloatSerializable(Name.c_str());
			copyTo(object);
			return object;
		}
	};
}