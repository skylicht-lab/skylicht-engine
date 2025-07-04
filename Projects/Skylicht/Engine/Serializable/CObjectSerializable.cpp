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

#include "pch.h"
#include "CObjectSerializable.h"
#include "CValueProperty.h"
#include "CArraySerializable.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	CObjectSerializable::CObjectSerializable(const char* name) :
		CValueProperty(NULL, EPropertyDataType::Object, name)
	{

	}

	CObjectSerializable::CObjectSerializable(const char* name, CObjectSerializable* parent) :
		CValueProperty(parent, EPropertyDataType::Object, name)
	{

	}

	CObjectSerializable::~CObjectSerializable()
	{
		for (CValueProperty* p : m_autoRelease)
		{
			delete p;
		}
		m_autoRelease.clear();
		m_value.clear();
	}

	void CObjectSerializable::remove(CValueProperty* value)
	{
		std::vector<CValueProperty*>::iterator i = m_value.begin(), e = m_value.end();
		while (i != e)
		{
			if ((*i) == value)
			{
				m_value.erase(i);
				break;
			}
			++i;
		}

		i = m_autoRelease.begin();
		e = m_autoRelease.end();

		while (i != e)
		{
			if ((*i) == value)
			{
				delete (*i);
				m_autoRelease.erase(i);
				break;
			}
			++i;
		}
	}

	CValueProperty* CObjectSerializable::getProperty(const char* name)
	{
		for (CValueProperty* p : m_value)
		{
			if (p->Name == name)
				return p;
		}
		return NULL;
	}

	void CObjectSerializable::serialize(io::IAttributes* io)
	{
		for (CValueProperty* p : m_value)
		{
			if (p->getType() != EPropertyDataType::Object)
				p->serialize(io);
		}
	}

	void CObjectSerializable::deserialize(io::IAttributes* io)
	{
		for (CValueProperty* p : m_value)
		{
			if (p->getType() != EPropertyDataType::Object)
				p->deserialize(io);
		}
	}

	bool CObjectSerializable::save(const char* file)
	{
		io::IXMLWriter* writer = getIrrlichtDevice()->getFileSystem()->createXMLWriter(file);
		if (writer == NULL)
			return false;

		writer->writeXMLHeader();

		save(writer);

		writer->drop();
		m_savePath = file;

		return true;
	}

	bool CObjectSerializable::saveToFile()
	{
		if (!m_savePath.empty())
			return save(m_savePath.c_str());
		return false;
	}

	bool CObjectSerializable::load(const char* file)
	{
		io::IXMLReader* reader = getIrrlichtDevice()->getFileSystem()->createXMLReader(file);
		if (reader == NULL)
			return false;

		load(reader);

		reader->drop();
		m_savePath = file;

		return true;
	}

	void CObjectSerializable::save(io::IXMLWriter* writer)
	{
		char elementName[512];

		if (m_objectType == ObjectArray ||
			m_objectType == FileArray ||
			m_objectType == TextureArray)
			sprintf(elementName, "node type=\"%s\" array=\"true\"", Name.c_str());
		else
			sprintf(elementName, "node type=\"%s\"", Name.c_str());

		writer->writeElement(CStringImp::convertUTF8ToUnicode(elementName).c_str(), false);
		writer->writeLineBreak();

		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();
		io::IAttributes* attr = fs->createEmptyAttributes();

		// save attribute
		serialize(attr);
		attr->write(writer);

		// save child object
		for (CValueProperty* p : m_value)
		{
			if (p->getType() == EPropertyDataType::Object)
			{
				((CObjectSerializable*)p)->save(writer);
			}
		}

		writer->writeClosingTag(L"node");
		writer->writeLineBreak();

		attr->drop();
	}

	void CObjectSerializable::parseSerializable(io::IXMLReader* reader)
	{
		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();
		io::IAttributes* attr = fs->createEmptyAttributes();

		std::wstring nodeName = L"node";
		std::wstring attributeName = CStringImp::convertUTF8ToUnicode(Name.c_str());

		if (nodeName == reader->getNodeName() && attributeName == reader->getAttributeValue(L"type"))
		{
			attr->read(reader);
			deserialize(attr);
		}
		else
		{
			char log[512];
			sprintf(log, "[CObjectSerializable::load] Skip wrong data: type: %s", Name.c_str());
			os::Printer::log(log);
			attr->drop();
			return;
		}

		// load child object
		for (CValueProperty* p : m_value)
		{
			if (p->getType() == EPropertyDataType::Object)
			{
				((CObjectSerializable*)p)->load(reader);
			}
		}

		attr->drop();
	}

	void CObjectSerializable::load(io::IXMLReader* reader)
	{
		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();
		io::IAttributes* attr = fs->createEmptyAttributes();

		bool logError = true;
		bool done = false;

		std::wstring attributeNode = L"attributes";
		std::wstring nodeName = L"node";

		while (!done && reader->read())
		{
			switch (reader->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				bool acceptName = false;
				std::string type;

				const wchar_t* typePtr = reader->getAttributeValue(L"type");
				if (typePtr)
				{
					type = CStringImp::convertUnicodeToUTF8(typePtr);

					acceptName = Name == type;
					if (!acceptName)
					{
						for (const std::string& name : OtherName)
						{
							if (name == type)
							{
								acceptName = true;
								break;
							}
						}
					}
				}

				if (nodeName == reader->getNodeName() && acceptName)
				{
					attr->read(reader);

					// try to load Array
					if (attr->getAttributeCount() > 0 && m_value.size() == 0)
					{
						CArraySerializable* arrayData = dynamic_cast<CArraySerializable*>(this);
						if (arrayData && arrayData->haveCreateElementFunction())
						{
							// create null data & deserialize later
							for (u32 i = 0, n = attr->getAttributeCount(); i < n; i++)
								arrayData->createElement();
						}
						else
						{
							char log[1024];
							sprintf(log, "[CObjectSerializable::load] Can't load array '%s'", type.c_str());
							os::Printer::log(log);
						}
					}

					deserialize(attr);
					done = true;
				}
				else
				{
					if (logError)
					{
						char log[1024];
						sprintf(log, "[CAttributeSerializable::load] Skip wrong data: type: %s", Name.c_str());
						os::Printer::log(log);
						logError = false;
					}
				}
			}
			break;
			case io::EXN_ELEMENT_END:
				if (attributeNode == reader->getNodeName())
				{
					done = true;
					break;
				}
			default:
				break;
			}
		}

		// load child object
		for (CValueProperty* p : m_value)
		{
			if (p->getType() == EPropertyDataType::Object)
			{
				((CObjectSerializable*)p)->load(reader);
			}
		}

		done = false;

		// read to end </node>
		while (!done && reader->read())
		{
			switch (reader->getNodeType())
			{
			case io::EXN_ELEMENT_END:
			{
				if (nodeName == reader->getNodeName())
					done = true;
				break;
			}
			default:
			{
				os::Printer::log("[CObjectSerializable::load] not found '</node>'");
				done = true;
			}
			break;
			};
		}

		attr->drop();
	}

	CObjectSerializable* CObjectSerializable::clone()
	{
		CObjectSerializable* object = new CObjectSerializable(Name.c_str(), NULL);
		object->m_savePath = m_savePath;

		for (CValueProperty* value : m_value)
		{
			CValueProperty* newValue = value->clone();
			newValue->setOwner(object);
			newValue->setUIHeader(value->getUIHeader().c_str());
			newValue->setUISpace(value->getUISpace());

			object->m_value.push_back(newValue);
			object->m_autoRelease.push_back(newValue);
		}

		return object;
	}

	void CObjectSerializable::copyTo(CObjectSerializable* object)
	{
		object->m_savePath = m_savePath;

		io::IAttributes* io = getIrrlichtDevice()->getFileSystem()->createEmptyAttributes();

		for (CValueProperty* value : m_value)
		{
			io->clear();
			value->serialize(io);

			CValueProperty* dst = object->getProperty(value->Name.c_str());
			if (dst)
				dst->deserialize(io);
		}

		io->drop();
	}

	IMPLEMENT_SINGLETON(CSerializableActivator);

	bool CSerializableActivator::registerType(const char* type, SerializableCreateInstance func)
	{
		std::map<std::string, int>::iterator i = m_factoryName.find(type);
		if (i != m_factoryName.end())
			return false;

		m_factoryName[type] = (int)m_factoryFunc.size();
		m_factoryFunc.push_back(func);
		return true;
	}

	CObjectSerializable* CSerializableActivator::createInstance(const char* type)
	{
		std::map<std::string, int>::iterator i = m_factoryName.find(type);
		if (i == m_factoryName.end())
			return NULL;

		int id = (*i).second;
		return m_factoryFunc[id]();
	}
}