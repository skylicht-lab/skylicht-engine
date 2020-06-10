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
#include "CMemoryStream.h"

namespace Skylicht
{
	CMemoryStream::CMemoryStream(unsigned int initMem)
	{
		m_memory = new unsigned char[initMem];
		m_size = 0;
		m_pos = 0;
		m_totalSize = initMem;
		m_fromMemory = false;
	}

	CMemoryStream::CMemoryStream(unsigned char *fromMem, unsigned int size)
	{
		m_memory = fromMem;
		m_size = size;
		m_totalSize = size;
		m_pos = 0;
		m_fromMemory = true;
	}

	CMemoryStream::~CMemoryStream()
	{
		if (m_fromMemory == false)
			delete m_memory;
	}

	void CMemoryStream::writeData(const void* data, unsigned int size)
	{
		memcpy(&m_memory[m_size], data, size);
		m_size += size;
	}

	void CMemoryStream::writeChar(const char data)
	{
		int size = sizeof(data);
		memcpy(&m_memory[m_size], &data, size);
		m_size += size;
	}

	void CMemoryStream::writeShort(const short data)
	{
		int size = sizeof(data);
		memcpy(&m_memory[m_size], &data, size);
		m_size += size;
	}

	void CMemoryStream::writeInt(const int data)
	{
		int size = sizeof(data);
		memcpy(&m_memory[m_size], &data, size);
		m_size += size;
	}

	void CMemoryStream::writeLong(const u64 data)
	{
		int size = sizeof(data);
		memcpy(&m_memory[m_size], &data, size);
		m_size += size;
	}

	void CMemoryStream::writeFloat(const float data)
	{
		int size = sizeof(data);
		memcpy(&m_memory[m_size], &data, size);
		m_size += size;
	}

	void CMemoryStream::writeDouble(const double data)
	{
		int size = sizeof(data);
		memcpy(&m_memory[m_size], &data, size);
		m_size += size;
	}

	void CMemoryStream::writeString(const std::string& s)
	{
		int size = s.size() + 1;

		// write num char
		memcpy(&m_memory[m_size], &size, sizeof(int));
		m_size += sizeof(int);

		// write string data
		if (size > 0)
		{
			memcpy(&m_memory[m_size], s.c_str(), size);
			m_size += size;
		}
	}

	unsigned int CMemoryStream::readData(void* data, unsigned int size)
	{
		unsigned int maxSize = m_size - m_pos;
		if (size > maxSize)
			size = maxSize;

		if (size <= 0)
			return 0;

		memcpy(data, &m_memory[m_pos], size);
		m_pos += size;

		return size;
	}

	char CMemoryStream::readChar()
	{
		char ret = m_memory[m_pos];
		m_pos++;
		return ret;
	}

	short CMemoryStream::readShort()
	{
		short ret = 0;
		int size = sizeof(ret);
		memcpy(&ret, &m_memory[m_pos], size);
		m_pos += size;
		return ret;
	}

	int CMemoryStream::readInt()
	{
		int ret = 0;
		int size = sizeof(ret);
		memcpy(&ret, &m_memory[m_pos], size);
		m_pos += size;
		return ret;
	}

	u64 CMemoryStream::readLong()
	{
		u64 ret = 0;
		int size = sizeof(ret);
		memcpy(&ret, &m_memory[m_pos], size);
		m_pos += size;
		return ret;
	}

	float CMemoryStream::readFloat()
	{
		float ret = 0;
		int size = sizeof(ret);
		memcpy(&ret, &m_memory[m_pos], size);
		m_pos += size;
		return ret;
	}

	double CMemoryStream::readDouble()
	{
		double ret = 0;
		int size = sizeof(ret);
		memcpy(&ret, &m_memory[m_pos], size);
		m_pos += size;
		return ret;
	}

	std::string CMemoryStream::readString()
	{
		int numchar = 0;

		memcpy(&numchar, &m_memory[m_pos], sizeof(int));
		m_pos += sizeof(int);

		std::string ret = "";
		if (numchar > 0)
		{
			char *data = new char[numchar];
			memcpy(data, &m_memory[m_pos], numchar);
			m_pos += numchar;

			ret = data;
			delete data;
		}
		return ret;
	}

	void CMemoryStream::encrypt()
	{
	}

	void CMemoryStream::decrypt()
	{
	}

}