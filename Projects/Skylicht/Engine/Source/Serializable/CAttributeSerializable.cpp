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
#include "CAttributeSerializable.h"
#include "CValueProperty.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	CAttributeSerializable::CAttributeSerializable(const char* name) :
		CObjectSerializable(name)
	{
		m_attribute = getIrrlichtDevice()->getFileSystem()->createEmptyAttributes();
	}

	CAttributeSerializable::CAttributeSerializable(const char* name, CAttributeSerializable* parent) :
		CObjectSerializable(name, parent)
	{
		m_attribute = getIrrlichtDevice()->getFileSystem()->createEmptyAttributes();
	}

	CAttributeSerializable::~CAttributeSerializable()
	{
		m_attribute->drop();
	}

	void CAttributeSerializable::serialize(io::IAttributes* io)
	{
		// write to attribute
		m_attribute->clear();
		copyAttribute(io, m_attribute);

	}

	void CAttributeSerializable::deserialize(io::IAttributes* io)
	{
		// write to io
		copyAttribute(m_attribute, io);
	}

	void CAttributeSerializable::copyAttribute(io::IAttributes* from, io::IAttributes* to)
	{
		for (s32 i = 0, n = from->getAttributeCount(); i < n; i++)
		{
			io::E_ATTRIBUTE_TYPE type = from->getAttributeType(i);
			const c8* name = from->getAttributeName(i);
			switch (type)
			{
			case io::EAT_INT:
				to->addInt(name, from->getAttributeAsInt(i));
				break;
			case io::EAT_FLOAT:
				to->addFloat(name, from->getAttributeAsFloat(i));
				break;
			case io::EAT_BOOL:
				to->addBool(name, from->getAttributeAsBool(i));
				break;
			case io::EAT_STRING:
				to->addString(name, from->getAttributeAsStringW(i).c_str());
				break;
			case io::EAT_VECTOR3D:
				to->addVector3d(name, from->getAttributeAsVector3d(i));
				break;
			case io::EAT_QUATERNION:
				to->addQuaternion(name, from->getAttributeAsQuaternion(i));
				break;
			case io::EAT_COLOR:
				to->addColor(name, from->getAttributeAsColor(i));
				break;
			case io::EAT_MATRIX:
				to->addMatrix(name, from->getAttributeAsMatrix(i));
				break;
			};
		}
	}

	bool CAttributeSerializable::save(const char* file)
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

	bool CAttributeSerializable::load(const char* file)
	{
		io::IXMLReader* reader = getIrrlichtDevice()->getFileSystem()->createXMLReader(file);
		if (reader == NULL)
			return false;

		load(reader);

		reader->drop();
		m_savePath = file;

		return true;
	}

	void CAttributeSerializable::save(io::IXMLWriter* writer)
	{
		char elementName[512];
		sprintf(elementName, "CObjectSerializable type=\"%s\"", Name.c_str());

		writer->writeElement(CStringImp::convertUTF8ToUnicode(elementName).c_str(), false);
		writer->writeLineBreak();

		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();

		// save attribute
		m_attribute->write(writer);

		writer->writeClosingTag(L"CObjectSerializable");
		writer->writeLineBreak();
	}

	void CAttributeSerializable::parseSerializable(io::IXMLReader* reader)
	{
		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();

		std::wstring nodeName = L"CObjectSerializable";
		std::wstring attributeName = CStringImp::convertUTF8ToUnicode(Name.c_str());

		m_attribute->clear();

		if (nodeName == reader->getNodeName() && attributeName == reader->getAttributeValue(L"type"))
		{
			m_attribute->read(reader);
		}
		else
		{
			char log[512];
			sprintf(log, "[CAttributeSerializable::load] Skip wrong data: type: %s", Name.c_str());
			os::Printer::log(log);
			return;
		}
	}

	void CAttributeSerializable::load(io::IXMLReader* reader)
	{
		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();
		io::IAttributes* attr = fs->createEmptyAttributes();

		bool done = false;

		std::wstring nodeName = L"CObjectSerializable";
		std::wstring attributeName = CStringImp::convertUTF8ToUnicode(Name.c_str());

		m_attribute->clear();

		while (!done && reader->read())
		{
			switch (reader->getNodeType())
			{
			case io::EXN_ELEMENT:
				if (nodeName == reader->getNodeName() && attributeName == reader->getAttributeValue(L"type"))
				{
					m_attribute->read(reader);
					done = true;
				}
				else
				{
					char log[512];
					sprintf(log, "[CAttributeSerializable::load] Skip wrong data: type: %s", Name.c_str());
					os::Printer::log(log);
				}
				break;
			default:
				break;
			}
		}
	}
}