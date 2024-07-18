/*
!@
MIT License

Copyright (c) 2012 - 2019 Skylicht Technology CO., LTD

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

#include "stdafx.h"
#include "COnlineMemoryStream.h"

namespace Skylicht
{
	namespace Audio
	{
		///////////////////////////////////////////
		// COnlineMemoryStream
		///////////////////////////////////////////
		COnlineMemoryStream::COnlineMemoryStream()
		{
			m_size = 0;
			m_allocSize = 0;
			m_fullDataStream = false;
			m_stopStream = false;
			m_buffer = NULL;
			m_mutex = IMutex::createMutex();
		}
		
		COnlineMemoryStream::~COnlineMemoryStream()
		{
			delete m_buffer;
			delete m_mutex;
		}
		
		IStreamCursor* COnlineMemoryStream::createCursor()
		{
			return new COnlineMemoryStreamCursor(this);
		}
		
		// remember lock mutex before run this function
		unsigned char* COnlineMemoryStream::getDataBuffer()
		{
			return m_buffer;
		}
		
		// remember lock mutex before run this function
		int COnlineMemoryStream::getDataBufferSize()
		{
			return m_size;
		}
		
		void COnlineMemoryStream::uploadData(unsigned char* buffer, int size)
		{
			SScopeMutex lock(m_mutex);
			
			bool renewBuffer = false;
			
			while (m_allocSize < m_size + size)
			{
				if (m_allocSize < 512)
					m_allocSize = 512;
				
				m_allocSize = m_allocSize * 2;
				renewBuffer = true;
			}
			
			unsigned char* newBuffer = m_buffer;
			
			if (renewBuffer == true)
			{
				// copy current buffer to new buffer
				newBuffer = new unsigned char[m_allocSize];
				memcpy(newBuffer, m_buffer, m_size);
				
				// delete old buffer
				if (m_buffer != NULL)
				{
					delete m_buffer;
					m_buffer = NULL;
				}
			}
			
			// add data
			memcpy(newBuffer + m_size, buffer, size);
			
			m_buffer = newBuffer;
			m_size = m_size + size;
		}
		
		void COnlineMemoryStream::trim(int position)
		{
			m_size = m_size - position;
			
			unsigned char* newBuffer = new unsigned char[m_allocSize];
			memcpy(newBuffer, m_buffer + position, m_size);
			
			delete m_buffer;
			m_buffer = newBuffer;
		}
		
		void COnlineMemoryStream::stopStream()
		{
			SScopeMutex lock(m_mutex);
			if (m_buffer)
				memset(m_buffer, 0, m_size);
			m_size = 0;
		}
		
		///////////////////////////////////////////
		// COnlineMemoryStreamCursor
		///////////////////////////////////////////
		
		COnlineMemoryStreamCursor::COnlineMemoryStreamCursor(COnlineMemoryStream* stream)
		{
			m_pos = 0;
			m_stream = stream;
		}
		
		COnlineMemoryStreamCursor::~COnlineMemoryStreamCursor()
		{
		}
		
		int COnlineMemoryStreamCursor::seek(int pos, EOrigin origin)
		{
			SScopeMutex lock(m_stream->getMutex());
			
			int currentPosition = m_pos;
			int size = m_stream->getDataBufferSize();
			
			switch (origin)
			{
				case OriginStart:
				{
					currentPosition = pos;
					break;
				}
				case OriginCurrent:
				{
					currentPosition += pos;
					break;
				}
				case OriginEnd:
				{
					currentPosition = size - pos;
					break;
				}
			}
			
			if (currentPosition < 0 || currentPosition > size)
			{
				return -1;
			}
			else
			{
				m_pos = currentPosition;
				return m_pos;
			}
		}
		
		int COnlineMemoryStreamCursor::tell()
		{
			return m_pos;
		}
		
		int COnlineMemoryStreamCursor::read(unsigned char* buff, int len)
		{
			SScopeMutex lock(m_stream->getMutex());
			
			int size = m_stream->getDataBufferSize();
			unsigned char* buffer = m_stream->getDataBuffer();
			
			if (buff && len > 0)
			{
				int remain = size - m_pos;
				
				if (len > remain)
					len = remain;
				
				memcpy(buff, buffer + m_pos, len);
				m_pos += len;
				return len;
			}
			else
			{
				return 0;
			}
		}
		
		bool COnlineMemoryStreamCursor::endOfStream()
		{
			SScopeMutex lock(m_stream->getMutex());
			return m_pos >= m_stream->getDataBufferSize() - 1;
		}
		
		int COnlineMemoryStreamCursor::size()
		{
			SScopeMutex lock(m_stream->getMutex());
			return m_stream->getDataBufferSize();
		}
		
		bool COnlineMemoryStreamCursor::readyReadData(int len)
		{
			SScopeMutex lock(m_stream->getMutex());
			
			if (m_stream->isFullData() == true)
				return true;
			
			if (m_pos + len > m_stream->getDataBufferSize())
				return false;
			
			return true;
		}
		
		void COnlineMemoryStreamCursor::trim()
		{
			SScopeMutex lock(m_stream->getMutex());
			m_stream->trim(m_pos);
			m_pos = 0;
		}
	}
}
