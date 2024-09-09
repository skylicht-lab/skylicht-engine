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
#include "CFileStream.h"

namespace Skylicht
{
	namespace Audio
	{
		///////////////////////////////////////////
		// CFileStream
		///////////////////////////////////////////
		
		CFileStream::CFileStream(const char *fileName)
		{
			m_fileName = fileName;
			m_file = fopen(fileName, "rb");
			
			if (m_file == NULL)
			{
				m_size = 0;
			}
		}
		
		CFileStream::~CFileStream()
		{
			if (m_file)
				fclose(m_file);
		}
		
		bool CFileStream::isError()
		{
			return m_file == NULL;
		}
		
		IStreamCursor* CFileStream::createCursor()
		{
			if (m_file == NULL)
				return NULL;
			
			return new CFileStreamCursor(m_file);
		}
		
		///////////////////////////////////////////
		// CFileStreamCursor
		///////////////////////////////////////////
		
		CFileStreamCursor::CFileStreamCursor(FILE *file)
		{
			m_file = file;
			
			fseek(m_file, 0, SEEK_END);
			m_size = ftell(m_file);
			fseek(m_file, 0, SEEK_SET);
			
			m_pos = 0;
			
			m_bufferAvailableBytes = 0;
			m_bufferPos = 0;
			m_bufferOffset = 0;
			
			m_bufferSize = 32 * 1024;	// 32kb
			m_buffer = new unsigned char[m_bufferSize];
		}
		
		CFileStreamCursor::~CFileStreamCursor()
		{
			if (m_buffer)
				delete m_buffer;
		}
		
		int CFileStreamCursor::seek(int pos, EOrigin origin)
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
			
			if (currentPosition < 0 || currentPosition > m_size || m_file == NULL)
			{
				return -1;
			}
			else
			{
				m_pos = currentPosition;
				
				if (m_pos >= m_bufferOffset && m_pos < (m_bufferOffset + m_bufferAvailableBytes))
				{
					m_bufferPos = m_pos - m_bufferOffset;
					return 0;
				}
				else
				{
					m_bufferAvailableBytes = 0;
					m_bufferPos = 0;
					m_bufferOffset = 0;
					
					fseek(m_file, m_pos, SEEK_SET);
					m_bufferOffset = m_pos;
				}
			}
			return 1;
		}
		
		int CFileStreamCursor::tell()
		{
			if (m_file == NULL)
				return 0;
			
			return m_pos;
		}
		
		int CFileStreamCursor::read(unsigned char* buff, int len)
		{
			if (m_file == NULL)
				return -1;
			
			int bytesLeft = m_bufferAvailableBytes - m_bufferPos;
			int outBytes = 0;
			
			while (len > 0)
			{
				if ((len > 0) && (bytesLeft > 0))
				{
					if (bytesLeft >= len)
					{
						memcpy(buff + outBytes, m_buffer + m_bufferPos, len);
						outBytes += len;
						m_bufferPos += len;
						len = 0;
					}
					else
					{
						memcpy(buff + outBytes, m_buffer + m_bufferPos, bytesLeft);
						outBytes += bytesLeft;
						m_bufferPos += bytesLeft;
						len -= bytesLeft;
					}
					
					bytesLeft = m_bufferAvailableBytes - m_bufferPos;
				}
				
				if (bytesLeft == 0)
				{
					fseek(m_file, m_bufferOffset, SEEK_SET);
					
					int sizeRemaining = m_size - m_bufferOffset;
					if (sizeRemaining < 0)
						sizeRemaining = 0;
					
					int readSize = m_bufferSize;
					if (readSize > sizeRemaining)
						readSize = sizeRemaining;
					
					if (readSize == 0)
					{
						m_pos += outBytes;
						return outBytes;
					}
					
					int result = (int)fread(m_buffer, readSize, 1, m_file);
					if (result == 0)
					{
						m_pos += outBytes;
						return outBytes;
					}
					
					m_bufferAvailableBytes = readSize;
					
					m_bufferOffset += m_bufferAvailableBytes;
					m_bufferPos = 0;
					
					if (m_bufferAvailableBytes == 0)
					{
						m_pos += outBytes;
						return outBytes;
					}
					
					bytesLeft = m_bufferAvailableBytes;
				}
			}
			
			m_pos += outBytes;
			return outBytes;
		}
		
		bool CFileStreamCursor::endOfStream()
		{
			if (m_file)
			{
				return m_pos >= m_size - 1;
			}
			return true;
		}
		
		int CFileStreamCursor::size()
		{
			return m_size;
		}
		
		bool CFileStreamCursor::readyReadData(int len)
		{
			return true;
		}
	}
}
