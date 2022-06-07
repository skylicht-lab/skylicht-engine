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

#include "Utils/CGameSingleton.h"
#include "Utils/CActivator.h"

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

		void autoRelease(CValueProperty* p)
		{
			m_autoRelease.push_back(p);
		}

	public:
		CObjectSerializable(const char* name);

		CObjectSerializable(const char* name, CObjectSerializable* parent);

		virtual ~CObjectSerializable();

		void remove(CValueProperty* value);

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

		template<class T>
		T get(CValueProperty* p, T defaultValue)
		{
			CValuePropertyTemplate<T>* t = dynamic_cast<CValuePropertyTemplate<T>*>(p);
			if (t == NULL)
				return defaultValue;

			return t->get();
		}

		virtual void serialize(io::IAttributes* io);

		virtual void deserialize(io::IAttributes* io);

		virtual bool save(const char* file);

		virtual bool load(const char* file);

		virtual void save(io::IXMLWriter* writer);

		virtual void load(io::IXMLReader* reader);

		virtual void parseSerializable(io::IXMLReader* reader);

		virtual CObjectSerializable* clone();

		virtual void copyTo(CObjectSerializable* object);
	};



#define SERIALIZABLE_REGISTER(type)  \
	CObjectSerializable* type##CreateFunc() { return new type(); } \
	bool type##_activator = CSerializableActivator::createGetInstance()->registerType(#type, &type##CreateFunc);

	typedef CObjectSerializable* (*SerializableCreateInstance)();

	class CSerializableActivator : public CGameSingleton<CSerializableActivator>
	{
	protected:
		std::map<std::string, int> m_factoryName;

		std::vector<SerializableCreateInstance> m_factoryFunc;

	public:
		bool registerType(const char* type, SerializableCreateInstance func);

		CObjectSerializable* createInstance(const char* type);
	};
}