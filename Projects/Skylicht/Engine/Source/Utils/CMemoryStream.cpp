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

	CMemoryStream::CMemoryStream(const CMemoryStream& stream)
	{
		m_size = stream.m_size;
		m_totalSize = stream.m_totalSize;
		m_pos = 0;

		m_memory = new unsigned char[m_totalSize];
		memcpy(m_memory, stream.m_memory, m_size);

		m_fromMemory = true;
	}

	CMemoryStream::~CMemoryStream()
	{
		if (m_fromMemory == false)
			delete[]m_memory;
	}

	bool CMemoryStream::autoGrow(unsigned int writeSize)
	{
		if (m_fromMemory == true)
			return false;

		unsigned int expectedSize = m_size + writeSize;
		if (expectedSize < m_totalSize)
			return true;

		unsigned int newSize = expectedSize * 2;

		unsigned char *newMemory = new unsigned char[newSize];
		memcpy(newMemory, m_memory, m_size);
		delete m_memory;

		m_memory = newMemory;
		m_totalSize = newSize;

		return true;
	}

	void CMemoryStream::writeData(const void* data, unsigned int size)
	{
		autoGrow(size);
		memcpy(&m_memory[m_size], data, size);
		m_size += size;
	}

	void CMemoryStream::writeStream(CMemoryStream *stream)
	{
		writeData(stream->getData(), stream->getSize());
	}

	void CMemoryStream::writeChar(char data)
	{
		int size = sizeof(data);
		autoGrow(size);
		memcpy(&m_memory[m_size], &data, size);
		m_size += size;
	}

	void CMemoryStream::writeShort(short data)
	{
		int size = sizeof(data);
		autoGrow(size);
		memcpy(&m_memory[m_size], &data, size);
		m_size += size;
	}

	void CMemoryStream::writeUShort(unsigned short data)
	{
		int size = sizeof(data);
		autoGrow(size);
		memcpy(&m_memory[m_size], &data, size);
		m_size += size;
	}

	void CMemoryStream::writeInt(int data)
	{
		int size = sizeof(data);
		autoGrow(size);
		memcpy(&m_memory[m_size], &data, size);
		m_size += size;
	}

	void CMemoryStream::writeUInt(unsigned int data)
	{
		int size = sizeof(data);
		autoGrow(size);
		memcpy(&m_memory[m_size], &data, size);
		m_size += size;
	}

	void CMemoryStream::writeFloat(float data)
	{
		int size = sizeof(data);
		autoGrow(size);
		memcpy(&m_memory[m_size], &data, size);
		m_size += size;
	}

	void CMemoryStream::writeFloatArray(const float *f, int count)
	{
		int size = sizeof(float) * count;
		autoGrow(size);
		memcpy(&m_memory[m_size], f, size);
		m_size += size;
	}

	void CMemoryStream::writeDouble(double data)
	{
		int size = sizeof(data);
		autoGrow(size);
		memcpy(&m_memory[m_size], &data, size);
		m_size += size;
	}

	void CMemoryStream::writeString(const std::string& s)
	{
		int size = (int)s.size() + 1;
		autoGrow(size);

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

	unsigned short CMemoryStream::readUShort()
	{
		unsigned short ret = 0;
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

	unsigned int CMemoryStream::readUInt()
	{
		unsigned int ret = 0;
		int size = sizeof(ret);
		memcpy(&ret, &m_memory[m_pos], size);
		m_pos += size;
		return ret;
	}

	/*
	u64 CMemoryStream::readLong()
	{
		u64 ret = 0;
		int size = sizeof(ret);
		memcpy(&ret, &m_memory[m_pos], size);
		m_pos += size;
		return ret;
	}
	*/

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

	void CMemoryStream::readFloatArray(float *f, int count)
	{
		int size = sizeof(float) * count;
		memcpy(f, &m_memory[m_pos], size);
		m_pos += size;
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
			delete[]data;
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