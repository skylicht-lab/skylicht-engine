/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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

#include "pch.h"
#include "Utils/CMemoryStream.h"

namespace Skylicht
{
	class IHttpStream
	{
	public:
		IHttpStream()
		{

		}

		virtual ~IHttpStream()
		{
		}

		virtual void write(void* data, unsigned int size) = 0;

		virtual const unsigned char* getData() = 0;

		virtual unsigned int getDataSize() = 0;

		virtual const char* getStreamPath() = 0;

		virtual void endStream()
		{
		}
	};


	class CHttpStream : public IHttpStream
	{
	protected:
		CMemoryStream* m_memory;

	public:
		CHttpStream(unsigned int datasize = 65536);
		
		virtual ~CHttpStream();

		virtual void write(void* data, unsigned int size);

		virtual const unsigned char* getData();

		virtual unsigned int getDataSize();

		virtual const char* getStreamPath()
		{
			return NULL;
		}

	};

	class CHttpFileStream : public IHttpStream
	{
	protected:
		io::IWriteFile* m_file;
		std::string m_filePath;

	public:
		CHttpFileStream(const char* fileName);

		virtual ~CHttpFileStream();

		virtual void write(void* data, unsigned int size);

		virtual void endStream();

		virtual const unsigned char* getData()
		{
			return NULL;
		}

		virtual unsigned int getDataSize()
		{
			return 0;
		}

		virtual const char* getStreamPath()
		{
			return m_filePath.c_str();
		}
	};

}
