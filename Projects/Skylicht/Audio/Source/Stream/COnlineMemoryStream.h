#ifndef _ONLINE_MEMORY_STREAM_H_
#define _ONLINE_MEMORY_STREAM_H_

#include "IStream.h"
#include "Thread/IMutex.h"

using namespace Skylicht::System;

namespace SkylichtAudio
{
	class COnlineMemoryStream;

	class COnlineMemoryStreamCursor : public IStreamCursor
	{
	public:
		int m_pos;
		COnlineMemoryStream* m_stream;

	public:
		COnlineMemoryStreamCursor(COnlineMemoryStream* stream);
		virtual ~COnlineMemoryStreamCursor();

		virtual int seek(int pos, EOrigin origin);
		virtual int tell();
		virtual int read(unsigned char* buff, int len);
		virtual bool endOfStream();
		virtual int size();
		virtual bool readyReadData(int len);
		virtual void trim();
	};

	class COnlineMemoryStream : public IStream
	{
	protected:
		unsigned char* m_buffer;
		int m_size;
		int m_allocSize;

		IMutex* m_mutex;
		bool m_fullDataStream;

		bool m_stopStream;
	public:
		COnlineMemoryStream();
		virtual ~COnlineMemoryStream();

		virtual IStreamCursor* createCursor();

		// remember lock stream mutex before run this function
		unsigned char* getDataBuffer();

		// remember lock stream mutex before run this function
		int getDataBufferSize();

		virtual void uploadData(unsigned char* buffer, int size);

		IMutex* getMutex()
		{
			return m_mutex;
		}

		void setFullData(bool b)
		{
			m_fullDataStream = b;
		}

		bool isFullData()
		{
			return m_fullDataStream;
		}

		bool isStopStream()
		{
			return m_stopStream;
		}

		void setStopStream(bool b)
		{
			m_stopStream = b;
		}

		virtual void trim(int position);

		virtual void stopStream();
	};

}

#endif
