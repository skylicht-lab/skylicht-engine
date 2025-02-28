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

		inline void clear()
		{
			m_valueX.clear();
			m_valueY.clear();
		}

		virtual void setInterpolator(std::vector<CInterpolator*>& interpolator) = 0;

		virtual void getInterpolator(std::vector<CInterpolator*>& interpolator) = 0;
	};


	class SKYLICHT_API CInterpolateFloatSerializable : public CInterpolateSerializable<CFloatProperty>
	{
	protected:

	public:
		CInterpolateFloatSerializable(const char* name) :
			CInterpolateSerializable<CFloatProperty>(name)
		{
		}

		CInterpolateFloatSerializable(const char* name, CObjectSerializable* parent) :
			CInterpolateSerializable<CFloatProperty>(name, parent)
		{
		}

		virtual ~CInterpolateFloatSerializable()
		{

		}

		inline void addEntry(float x, float y)
		{
			CFloatProperty* valueX = (CFloatProperty*)m_valueX.createElement();
			valueX->set(x);

			CFloatProperty* valueY = (CFloatProperty*)m_valueY.createElement();
			valueY->set(y);
		}

		virtual void setInterpolator(std::vector<CInterpolator*>& interpolator)
		{
			if (interpolator.size() == 0)
				return;

			CInterpolator* value = interpolator[0];
			const std::set<SInterpolatorEntry>& graph = value->getGraph();

			clear();
			for (auto i : graph)
			{
				addEntry(i.x, i.y);
			}
		}

		virtual void getInterpolator(std::vector<CInterpolator*>& interpolator)
		{
			if (interpolator.size() == 0)
				return;

			CInterpolator* value = interpolator[0];
			value->clearGraph();

			for (int i = 0, n = m_valueX.getElementCount(); i < n; i++)
			{
				float x = m_valueX.getElementValue(i, CFloatProperty()).get();
				float y = m_valueY.getElementValue(i, CFloatProperty()).get();

				value->addEntry(x, y);
			}
		}

		virtual CObjectSerializable* clone()
		{
			CInterpolateFloatSerializable* object = new CInterpolateFloatSerializable(Name.c_str());
			copyTo(object);
			return object;
		}
	};
}