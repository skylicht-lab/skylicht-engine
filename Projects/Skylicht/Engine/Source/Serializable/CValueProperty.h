/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include "CObjectSerizable.h"

namespace Skylicht
{
	enum EPropertyDataType
	{
		String,
		Integer,
		Float,
		Decimal,
		DateTime
	};

	class CValueProperty
	{
	protected:
		EPropertyDataType m_dataType;
		CObjectSerizable* m_owner;

	public:
		std::string Name;

		CValueProperty(CObjectSerizable *owner, EPropertyDataType dataType, const char *name) :
			m_owner(owner),
			m_dataType(dataType),
			Name(name)
		{
			owner->addProperty(this);
		}

		virtual ~CValueProperty()
		{

		}

		EPropertyDataType getType()
		{
			return m_dataType;
		}
	};

	template<class T>
	class CValuePropertyTemplate : public CValueProperty
	{
	protected:
		T m_value;

	public:
		CValuePropertyTemplate(CObjectSerizable *owner, EPropertyDataType dataType, const char *name) :
			CValueProperty(owner, dataType, name)
		{

		}

		virtual ~CValuePropertyTemplate()
		{

		}

		inline void setValue(const T& v)
		{
			m_value = v;
		}

		inline const T& getValue()
		{
			return m_value;
		}
	};

	class CIntProperty : public CValuePropertyTemplate<int>
	{
	public:
		CIntProperty(CObjectSerizable *owner, const char *name) :
			CValuePropertyTemplate(owner, Integer, name)
		{
			setValue(0);
		}

		CIntProperty(CObjectSerizable *owner, const char *name, int value) :
			CValuePropertyTemplate(owner, Integer, name)
		{
			setValue(value);
		}
	};

	class CFloatProperty : public CValuePropertyTemplate<float>
	{
	public:
		CFloatProperty(CObjectSerizable *owner, const char *name) :
			CValuePropertyTemplate(owner, Float, name)
		{
			setValue(0.0f);
		}

		CFloatProperty(CObjectSerizable *owner, const char *name, float value) :
			CValuePropertyTemplate(owner, Float, name)
		{
			setValue(value);
		}
	};

	class CStringProperty : public CValuePropertyTemplate<std::string>
	{
	public:
		CStringProperty(CObjectSerizable *owner, const char *name) :
			CValuePropertyTemplate(owner, String, name)
		{
		}

		CStringProperty(CObjectSerizable *owner, const char *name, const char* value) :
			CValuePropertyTemplate(owner, String, name)
		{
			setValue(value);
		}
	};

	class CDateTimeProperty : public CValuePropertyTemplate<long>
	{
	public:
		CDateTimeProperty(CObjectSerizable *owner, const char *name) :
			CValuePropertyTemplate(owner, DateTime, name)
		{
		}

		CDateTimeProperty(CObjectSerizable *owner, const char *name, long value) :
			CValuePropertyTemplate(owner, DateTime, name)
		{
			setValue(value);
		}
	};
}