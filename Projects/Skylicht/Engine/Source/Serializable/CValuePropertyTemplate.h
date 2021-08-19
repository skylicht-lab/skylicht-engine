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

#include "CValueProperty.h"

namespace Skylicht
{
	template<class T>
	class CValuePropertyTemplate : public CValueProperty
	{
	protected:
		T m_value;

	public:
		CValuePropertyTemplate(CObjectSerializable* owner, EPropertyDataType dataType, const char* name) :
			CValueProperty(owner, dataType, name)
		{
		}

		virtual ~CValuePropertyTemplate()
		{

		}

		inline void set(const T& v)
		{
			m_value = v;
		}

		inline const T& get()
		{
			return m_value;
		}

		T& operator*()
		{
			return m_value;
		}
	};

	class CIntProperty : public CValuePropertyTemplate<int>
	{
	public:
		CIntProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, Integer, name)
		{
			set(0);
		}

		CIntProperty(CObjectSerializable* owner, const char* name, int value) :
			CValuePropertyTemplate(owner, Integer, name)
		{
			set(value);
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addInt(Name.c_str(), m_value);
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsInt(Name.c_str());
		}
	};

	class CFloatProperty : public CValuePropertyTemplate<float>
	{
	public:
		CFloatProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, Float, name)
		{
			set(0.0f);
		}

		CFloatProperty(CObjectSerializable* owner, const char* name, float value) :
			CValuePropertyTemplate(owner, Float, name)
		{
			set(value);
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addFloat(Name.c_str(), m_value);
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsFloat(Name.c_str());
		}
	};

	class CStringProperty : public CValuePropertyTemplate<std::string>
	{
	public:
		CStringProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, String, name)
		{
		}

		CStringProperty(CObjectSerializable* owner, const char* name, const char* value) :
			CValuePropertyTemplate(owner, String, name)
		{
			set(value);
		}

		const char* getString()
		{
			return m_value.c_str();
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addString(Name.c_str(), m_value.c_str());
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsString(Name.c_str()).c_str();
		}
	};

	class CDateTimeProperty : public CValuePropertyTemplate<long>
	{
	public:
		CDateTimeProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, DateTime, name)
		{
			set(0);
		}

		CDateTimeProperty(CObjectSerializable* owner, const char* name, long value) :
			CValuePropertyTemplate(owner, DateTime, name)
		{
			set(value);
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addFloat(Name.c_str(), (float)m_value);
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = (long)io->getAttributeAsFloat(Name.c_str());
		}
	};
}