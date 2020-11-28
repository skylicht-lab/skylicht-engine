// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#ifndef _SKYLICHTAUDIO_ARCHIVE_FILESTREAM_H_
#define _SKYLICHTAUDIO_ARCHIVE_FILESTREAM_H_

#include "pch.h"
#include "Stream/IStream.h"

namespace Skylicht
{
	class CZipAudioFileStreamCursor : public SkylichtAudio::IStreamCursor
	{
	protected:
		io::IReadFile *m_file;
		int m_size;
		int	m_pos;

		unsigned char* m_buffer;
		int m_bufferSize;
		int m_bufferAvailableBytes;
		int m_bufferPos;
		int m_bufferOffset;
	public:
		CZipAudioFileStreamCursor(io::IReadFile *file);
		virtual ~CZipAudioFileStreamCursor();

		virtual int seek(int pos, EOrigin origin);
		virtual int tell();
		virtual int read(unsigned char* buff, int len);
		virtual bool endOfStream();
		virtual int size();
		virtual bool readyReadData(int len)
		{
			return true;
		}
	};

	class CZipAudioFileStream : public SkylichtAudio::IStream
	{
	protected:
		std::string		m_fileName;
		io::IReadFile	*m_file;
		int				m_size;

	public:
		CZipAudioFileStream(const char *fileName);
		virtual ~CZipAudioFileStream();

		bool isError();

		virtual SkylichtAudio::IStreamCursor* createCursor();
	};

}

#endif