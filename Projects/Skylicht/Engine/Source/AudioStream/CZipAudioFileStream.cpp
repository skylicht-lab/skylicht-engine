#include "pch.h"

#ifdef BUILD_SKYLICHT_AUDIO
#include "CZipAudioFileStream.h"

namespace Skylicht
{
	CZipAudioFileStream::CZipAudioFileStream(const char* fileName)
	{
		m_fileName = fileName;
		m_file = getIrrlichtDevice()->getFileSystem()->createAndOpenFile(fileName);

		if (m_file == NULL)
		{
			m_size = 0;
		}
	}

	CZipAudioFileStream::~CZipAudioFileStream()
	{
		if (m_file)
			m_file->drop();
	}

	bool CZipAudioFileStream::isError()
	{
		return m_file == NULL;
	}

	SkylichtAudio::IStreamCursor* CZipAudioFileStream::createCursor()
	{
		if (m_file == NULL)
			return NULL;

		return new CZipAudioFileStreamCursor(m_file);
	}


	CZipAudioFileStreamCursor::CZipAudioFileStreamCursor(io::IReadFile* file)
	{
		m_file = file;

		m_size = (int)file->getSize();

		m_pos = 0;

		m_bufferAvailableBytes = 0;
		m_bufferPos = 0;
		m_bufferOffset = 0;

		m_bufferSize = 32 * 1024;	// 32kb
		m_buffer = new unsigned char[m_bufferSize];
	}

	CZipAudioFileStreamCursor::~CZipAudioFileStreamCursor()
	{
		if (m_buffer)
			delete m_buffer;
	}

	int CZipAudioFileStreamCursor::seek(int pos, EOrigin origin)
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

				m_file->seek(m_pos);

				m_bufferOffset = m_pos;
			}
		}
		return 1;
	}

	int CZipAudioFileStreamCursor::tell()
	{
		if (m_file == NULL)
			return 0;

		return m_pos;
	}

	int CZipAudioFileStreamCursor::read(unsigned char* buff, int len)
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
				// seedk to offset
				m_file->seek(m_bufferOffset);

				// read buffer size
				u32 readSize = m_file->read(m_buffer, m_bufferSize);

				// cache available size
				m_bufferAvailableBytes = readSize;

				m_bufferOffset += m_bufferAvailableBytes;
				m_bufferPos = 0;

				// maybe end of stream
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

	bool CZipAudioFileStreamCursor::endOfStream()
	{
		if (m_file)
		{
			return m_pos >= m_size - 1;
		}
		return true;
	}

	int CZipAudioFileStreamCursor::size()
	{
		return m_size;
	}
}

#endif