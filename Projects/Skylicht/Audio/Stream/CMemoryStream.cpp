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
#include "CMemoryStream.h"

namespace Skylicht
{
	namespace Audio
	{
		///////////////////////////////////////////
		// CMemoryStream
		///////////////////////////////////////////
		CMemoryStream::CMemoryStream(unsigned char *buffer, int size, bool takeOwnership)
		{
			m_takeOwnership = takeOwnership;
			m_size = size;
			
			if (takeOwnership)
			{
				m_buffer = buffer;
				m_size = size;
			}
			else
			{
				m_buffer = new unsigned char[m_size];
				m_size = size;
				
				memcpy(m_buffer, buffer, size);
			}
		}
		
		CMemoryStream::~CMemoryStream()
		{
			if (m_takeOwnership == false)
				delete m_buffer;
		}
		
		IStreamCursor* CMemoryStream::createCursor()
		{
			return new CMemoryStreamCursor(m_buffer, m_size);
		}
		
		
		///////////////////////////////////////////
		// CMemoryStreamCursor
		///////////////////////////////////////////
		
		CMemoryStreamCursor::CMemoryStreamCursor(unsigned char *buffer, int size)
		{
			m_buffer = buffer;
			m_size = size;
			m_pos = 0;
		}
		
		CMemoryStreamCursor::~CMemoryStreamCursor()
		{
		}
		
		int CMemoryStreamCursor::seek(int pos, EOrigin origin)
		{
			int currentPosition = m_pos;
			
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
					currentPosition = m_size - pos;
					break;
				}
			}
			
			if (currentPosition < 0 || currentPosition > m_size)
			{
				return -1;
			}
			else
			{
				m_pos = currentPosition;
				return m_pos;
			}
		}
		
		int CMemoryStreamCursor::tell()
		{
			return m_pos;
		}
		
		int CMemoryStreamCursor::read(unsigned char* buff, int len)
		{
			if (buff  && len > 0)
			{
				int remain = m_size - m_pos;
				
				if (len > remain)
					len = remain;
				
				memcpy(buff, m_buffer + m_pos, len);
				m_pos += len;
				return len;
			}
			else
			{
				return 0;
			}
		}
		
		bool CMemoryStreamCursor::endOfStream()
		{
			return m_pos >= m_size - 1;
		}
		
		int CMemoryStreamCursor::size()
		{
			return m_size;
		}
		
		bool CMemoryStreamCursor::readyReadData(int len)
		{
			return true;
		}
	}
}
