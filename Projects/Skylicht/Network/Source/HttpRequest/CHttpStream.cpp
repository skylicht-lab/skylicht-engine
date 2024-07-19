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

#include "pch.h"
#include "CHttpStream.h"

namespace Skylicht
{
	namespace Network
	{
		CHttpStream::CHttpStream(unsigned int datasize)
		{
			m_memory = new CMemoryStream(datasize);
			
			// erase data
			memset(m_memory->getData(), 0, datasize);
		}
		
		CHttpStream::~CHttpStream()
		{
			delete m_memory;
		}
		
		void CHttpStream::write(void* data, unsigned int size)
		{
			m_memory->writeData(data, size);
		}
		
		const unsigned char* CHttpStream::getData()
		{
			return m_memory->getData();
		}
		
		unsigned int CHttpStream::getDataSize()
		{
			return m_memory->getSize();
		}
		
		
		CHttpFileStream::CHttpFileStream(const char* fileName)
		{
			m_file = getIrrlichtDevice()->getFileSystem()->createAndWriteFile(fileName);
			m_filePath = fileName;
		}
		
		CHttpFileStream::~CHttpFileStream()
		{
			endStream();
		}
		
		void CHttpFileStream::write(void* data, unsigned int size)
		{
			if (m_file)
				m_file->write(data, size);
		}
		
		void CHttpFileStream::endStream()
		{
			if (m_file)
				m_file->drop();
			m_file = NULL;
		}
	}
}
