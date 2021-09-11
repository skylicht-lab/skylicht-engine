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
		sprintf(elementName, "CObjectSerializable type=\"%s\"", Name.c_str());

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

		writer->writeClosingTag(L"CObjectSerializable");
		writer->writeLineBreak();

		attr->drop();
	}

	void CObjectSerializable::load(io::IXMLReader* reader)
	{
		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();
		io::IAttributes* attr = fs->createEmptyAttributes();

		bool done = false;

		std::wstring nodeName = L"CObjectSerializable";
		std::wstring attributeName = CStringImp::convertUTF8ToUnicode(Name.c_str());

		while (reader->read() && !done)
		{
			switch (reader->getNodeType())
			{
			case io::EXN_ELEMENT:
				if (nodeName == reader->getNodeName() && attributeName == reader->getAttributeValue(L"type"))
				{
					attr->read(reader);
					deserialize(attr);
					done = true;
				}
				else
				{
					char log[512];
					sprintf(log, "[CObjectSerializable::load] Skip wrong data: type: %s <- %s",
						Name.c_str(),
						CStringImp::convertUnicodeToUTF8(reader->getAttributeValue(L"type")).c_str()
					);
					os::Printer::log(log);
				}
				break;
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

		attr->drop();
	}
}