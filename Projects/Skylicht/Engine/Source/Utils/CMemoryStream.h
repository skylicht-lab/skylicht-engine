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

namespace Skylicht
{
	class CMemoryStream
	{
	protected:
		unsigned char *m_memory;
		unsigned int m_size;
		unsigned int m_totalSize;
		unsigned int m_pos;
		bool m_fromMemory;
	public:
		CMemoryStream(unsigned int initMem = 512);

		CMemoryStream(unsigned char *fromMem, unsigned int size);

		CMemoryStream(const CMemoryStream& stream);

		virtual ~CMemoryStream();

		bool autoGrow(unsigned int writeSize);

		void writeData(const void* data, unsigned int size);
		void writeStream(CMemoryStream *stream);

		void writeChar(char data);
		void writeShort(short data);
		void writeUShort(unsigned short data);
		void writeInt(int data);
		void writeUInt(unsigned int data);
		void writeFloat(float data);
		void writeDouble(double data);
		void writeString(const std::string& s);
		void writeFloatArray(const float *f, int count);

		unsigned int readData(void* data, unsigned int size);

		char readChar();
		short readShort();
		unsigned short readUShort();
		int readInt();
		unsigned int readUInt();		
		float readFloat();
		double readDouble();
		std::string readString();
		void readFloatArray(float *f, int count);

		unsigned char *getData()
		{
			return m_memory;
		}

		unsigned int getSize()
		{
			return m_size;
		}

		unsigned int getPos()
		{
			return m_pos;
		}

		unsigned int getTotalSize()
		{
			return m_totalSize;
		}

		void setPos(unsigned int pos)
		{
			m_pos = pos;
		}

		void resetWrite()
		{
			m_size = 0;
			m_pos = 0;
		}

		void encrypt();
		void decrypt();
	};

}