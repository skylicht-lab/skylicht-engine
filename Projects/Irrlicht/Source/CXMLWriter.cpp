// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"
#include "CXMLWriter.h"

#ifdef _IRR_COMPILE_WITH_XML_

#include <wchar.h>
#include "irrString.h"
#include "IrrCompileConfig.h"

namespace irr
{
namespace io
{

void CXMLWriter::appendUTF16CodeUnit(u16 value)
{
	UTF16Buffer.push_back(static_cast<u8>(value & 0xff));
	UTF16Buffer.push_back(static_cast<u8>((value >> 8) & 0xff));
}


void CXMLWriter::appendUTF16CodePoint(u32 codePoint)
{
	if (codePoint > 0x10ffff)
	{
		appendUTF16CodeUnit(static_cast<u16>('?'));
		return;
	}

	if (codePoint <= 0xffff)
	{
		appendUTF16CodeUnit(static_cast<u16>(codePoint));
		return;
	}

	codePoint -= 0x10000;
	appendUTF16CodeUnit(static_cast<u16>(0xd800 + (codePoint >> 10)));
	appendUTF16CodeUnit(static_cast<u16>(0xdc00 + (codePoint & 0x3ff)));
}


void CXMLWriter::writeUTF16(const wchar_t* text)
{
	if (!text)
		return;

	writeUTF16(text, static_cast<u32>(wcslen(text)));
}


void CXMLWriter::writeUTF16(const wchar_t* text, u32 length)
{
	if (!File || !text || length == 0)
		return;

	UTF16Buffer.set_used(0);

	if (sizeof(wchar_t) == 2)
	{
		UTF16Buffer.reallocate(length * 2, false);
		for (u32 i = 0; i < length; ++i)
			appendUTF16CodeUnit(static_cast<u16>(text[i]));
	}
	else
	{
		UTF16Buffer.reallocate(length * 4, false);
		for (u32 i = 0; i < length; ++i)
			appendUTF16CodePoint(static_cast<u32>(text[i]));
	}

	if (!UTF16Buffer.empty())
		File->write(UTF16Buffer.const_pointer(), UTF16Buffer.size());
}


void CXMLWriter::writeUTF16Literal(const c8* text)
{
	if (!File || !text || !text[0])
		return;

	UTF16Buffer.set_used(0);

	for (const c8* p = text; *p; ++p)
		appendUTF16CodeUnit(static_cast<u8>(*p));

	File->write(UTF16Buffer.const_pointer(), UTF16Buffer.size());
}


//! Constructor
CXMLWriter::CXMLWriter(IWriteFile* file)
: File(file), Tabs(0), TextWrittenLast(false)
{
	#ifdef _DEBUG
	setDebugName("CXMLWriter");
	#endif

	if (File)
		File->grab();
}



//! Destructor
CXMLWriter::~CXMLWriter()
{
	if (File)
		File->drop();
}



//! Writes a xml 1.0 header like <?xml version="1.0"?>
void CXMLWriter::writeXMLHeader()
{
	if (!File)
		return;

	const u8 bom[] = { 0xff, 0xfe };
	File->write(bom, 2);

	writeUTF16Literal("<?xml version=\"1.0\"?>");

	writeLineBreak();
	TextWrittenLast = false;
}



//! Writes an xml element with maximal 5 attributes
void CXMLWriter::writeElement(const wchar_t* name, bool empty,
	const wchar_t* attr1Name, const wchar_t* attr1Value,
	const wchar_t* attr2Name, const wchar_t* attr2Value,
	const wchar_t* attr3Name, const wchar_t* attr3Value,
	const wchar_t* attr4Name, const wchar_t* attr4Value,
	const wchar_t* attr5Name, const wchar_t* attr5Value)
{
	if (!File || !name)
		return;

	if (Tabs > 0)
	{
		for (int i=0; i<Tabs; ++i)
			writeUTF16Literal("\t");
	}

	// write name

	writeUTF16Literal("<");
	writeUTF16(name);

	// write attributes

	writeAttribute(attr1Name, attr1Value);
	writeAttribute(attr2Name, attr2Value);
	writeAttribute(attr3Name, attr3Value);
	writeAttribute(attr4Name, attr4Value);
	writeAttribute(attr5Name, attr5Value);

	// write closing tag
	if (empty)
		writeUTF16Literal(" />");
	else
	{
		writeUTF16Literal(">");
		++Tabs;
	}

	TextWrittenLast = false;
}

//! Writes an xml element with any number of attributes
void CXMLWriter::writeElement(const wchar_t* name, bool empty,
				  core::array<core::stringw> &names,
				  core::array<core::stringw> &values)
{
	if (!File || !name)
		return;

	if (Tabs > 0)
	{
		for (int i=0; i<Tabs; ++i)
			writeUTF16Literal("\t");
	}

	// write name

	writeUTF16Literal("<");
	writeUTF16(name);

	// write attributes
	u32 i=0;
	for (; i < names.size() && i < values.size(); ++i)
		writeAttribute(names[i].c_str(), values[i].c_str());

	// write closing tag
	if (empty)
		writeUTF16Literal(" />");
	else
	{
		writeUTF16Literal(">");
		++Tabs;
	}

	TextWrittenLast = false;
}


void CXMLWriter::writeAttribute(const wchar_t* name, const wchar_t* value)
{
	if (!name || !value)
		return;

	writeUTF16Literal(" ");
	writeUTF16(name);
	writeUTF16Literal("=\"");
	writeText(value);
	writeUTF16Literal("\"");
}


//! Writes a comment into the xml file
void CXMLWriter::writeComment(const wchar_t* comment)
{
	if (!File || !comment)
		return;

	writeUTF16Literal("<!--");
	writeText(comment);
	writeUTF16Literal("-->");
}


//! Writes the closing tag for an element. Like </foo>
void CXMLWriter::writeClosingTag(const wchar_t* name)
{
	if (!File || !name)
		return;

	--Tabs;

	if (Tabs > 0 && !TextWrittenLast)
	{
		for (int i=0; i<Tabs; ++i)
			writeUTF16Literal("\t");
	}

	writeUTF16Literal("</");
	writeUTF16(name);
	writeUTF16Literal(">");
	TextWrittenLast = false;
}



const CXMLWriter::XMLSpecialCharacters XMLWSChar[] =
{
	{ L'&', L"&amp;" },
	{ L'<', L"&lt;" },
	{ L'>', L"&gt;" },
	{ L'"', L"&quot;" },
	{ L'\0', 0 }
};


//! Writes a text into the file. All occurrences of special characters like
//! & (&amp;), < (&lt;), > (&gt;), and " (&quot;) are automaticly replaced.
void CXMLWriter::writeText(const wchar_t* text)
{
	if (!File || !text)
		return;

	// TODO: we have to get rid of that reserve call as well as it slows down xml-writing seriously.
	// Making a member-variable would work, but a lot of memory would stay around after writing.
	// So the correct solution is probably using fixed block here and always write when that is full.
	core::stringw s;
	s.reserve(wcslen(text)+1);
	const wchar_t* p = text;

	while(*p)
	{
		// check if it is matching
		bool found = false;
		for (s32 i=0; XMLWSChar[i].Character != '\0'; ++i)
			if (*p == XMLWSChar[i].Character)
			{
				s.append(XMLWSChar[i].Symbol);
				found = true;
				break;
			}

		if (!found)
			s.append(*p);
		++p;
	}

	// write new string
	writeUTF16(s.c_str(), static_cast<u32>(s.size()));
	TextWrittenLast = true;
}


//! Writes a line break
void CXMLWriter::writeLineBreak()
{
	if (!File)
		return;

#if defined(_IRR_OSX_PLATFORM_)
	writeUTF16Literal("\r");
#elif defined(_IRR_WINDOWS_API_)
	writeUTF16Literal("\r\n");
#else
	writeUTF16Literal("\n");
#endif

}


} // end namespace irr
} // end namespace io

#endif // _IRR_COMPILE_WITH_XML_
