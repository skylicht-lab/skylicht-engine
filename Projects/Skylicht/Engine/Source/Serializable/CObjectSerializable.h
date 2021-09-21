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
#include "CValuePropertyTemplate.h"

namespace Skylicht
{
	class CObjectSerializable : public CValueProperty
	{
		friend class CValueProperty;

	protected:
		std::vector<CValueProperty*> m_value;
		std::vector<CValueProperty*> m_autoRelease;
		std::string m_savePath;

	public:

		void addProperty(CValueProperty* p)
		{
			p->setOwner(this);
			m_value.push_back(p);
		}

		void addAutoRelease(CValueProperty* p)
		{
			m_autoRelease.push_back(p);
		}

	public:
		CObjectSerializable(const char* name);

		CObjectSerializable(const char* name, CObjectSerializable* parent);

		virtual ~CObjectSerializable();

		inline u32 getNumProperty()
		{
			return (u32)m_value.size();
		}

		inline CValueProperty* getPropertyID(int i)
		{
			return m_value[i];
		}

		CValueProperty* getProperty(const char* name);

		template<class T>
		T* getProperty(const char* name)
		{
			CValueProperty* p = getProperty(name);
			if (p == NULL)
				return NULL;

			return dynamic_cast<T*>(p);
		}

		template<class T>
		T get(const char* name, T defaultValue)
		{
			CValueProperty* p = getProperty(name);
			if (p == NULL)
				return defaultValue;

			CValuePropertyTemplate<T>* t = dynamic_cast<CValuePropertyTemplate<T>*>(p);
			if (t == NULL)
				return defaultValue;

			return t->get();
		}

		virtual void serialize(io::IAttributes* io);

		virtual void deserialize(io::IAttributes* io);

		bool save(const char* file);

		bool load(const char* file);

		void save(io::IXMLWriter* writer);

		void load(io::IXMLReader* reader);

		void parseSerializable(io::IXMLReader* reader);
	};
}