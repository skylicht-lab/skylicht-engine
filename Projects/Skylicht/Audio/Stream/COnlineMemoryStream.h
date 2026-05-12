#ifndef _ONLINE_MEMORY_STREAM_H_
#define _ONLINE_MEMORY_STREAM_H_

#include "IStream.h"
#include "Thread/IMutex.h"

using namespace Skylicht::System;

namespace Skylicht
{
	namespace Audio
	{
		class COnlineMemoryStream;
		
		/**
		 * @brief Stream cursor for dynamic/online memory streams.
		 * Supports trimming data from the start of the stream to save memory.
		 * @ingroup Audio
		 */
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

			/**
			 * @brief Remove all data before the current cursor position from the stream.
			 */
			virtual void trim();
		};
		
		/**
		 * @brief A thread-safe dynamic memory stream for online data (e.g. voice chat or network audio).
		 * New data can be uploaded at runtime while the cursor reads from it.
		 * @ingroup Audio
		 */
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
			
			/**
			 * @brief Create a new online cursor.
			 * @return Pointer to COnlineMemoryStreamCursor
			 */
			virtual IStreamCursor* createCursor();
			
			/**
			 * @brief Get pointer to the underlying buffer. 
			 * @note Must lock the stream mutex before access.
			 * @return Pointer to unsigned char buffer
			 */
			unsigned char* getDataBuffer();
			
			/**
			 * @brief Get total size of available data.
			 * @note Must lock the stream mutex before access.
			 * @return Size in bytes
			 */
			int getDataBufferSize();
			
			/**
			 * @brief Append new data to the stream. Thread-safe.
			 * @param buffer Data to upload
			 * @param size Size in bytes
			 */
			virtual void uploadData(unsigned char* buffer, int size);
			
			/**
			 * @brief Get the internal synchronization mutex.
			 * @return Pointer to IMutex
			 */
			inline IMutex* getMutex()
			{
				return m_mutex;
			}
			
			/**
			 * @brief Mark the stream as finished (no more data will be uploaded).
			 * @param b True if the stream is complete
			 */
			inline void setFullData(bool b)
			{
				m_fullDataStream = b;
			}
			
			/**
			 * @brief Check if the stream is marked as complete.
			 * @return True if complete
			 */
			inline bool isFullData()
			{
				return m_fullDataStream;
			}
			
			/**
			 * @brief Check if the stream has been stopped.
			 * @return True if stopped
			 */
			inline bool isStopStream()
			{
				return m_stopStream;
			}
			
			/**
			 * @brief Stop the stream.
			 * @param b True to stop
			 */
			inline void setStopStream(bool b)
			{
				m_stopStream = b;
			}
			
			/**
			 * @brief Remove data before the specified position.
			 * @param position Number of bytes to trim from the start
			 */
			virtual void trim(int position);
			
			/**
			 * @brief Stop the stream and clear all buffers.
			 */
			virtual void stopStream();
		};
	}
}

#endif
