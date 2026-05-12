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

#ifndef _SKYLICHTAUDIO_ISTREAM_H_
#define _SKYLICHTAUDIO_ISTREAM_H_

#include "SkylichtAudioAPI.h"

namespace Skylicht
{
	namespace Audio
	{
		/**
		 * @brief Interface for a stream cursor, providing seek and read operations on an audio stream.
		 * @ingroup Audio
		 */
		class IStreamCursor
		{
		public:
			/**
			 * @brief Origin for seek operations.
			 */
			enum EOrigin
			{
				OriginStart,   ///< Seek from the beginning of the stream
				OriginCurrent, ///< Seek from the current position
				OriginEnd      ///< Seek from the end of the stream
			};
		public:
			virtual ~IStreamCursor()
			{
			}
			
			/**
			 * @brief Move the cursor position.
			 * @param pos Offset in bytes
			 * @param origin Where to seek from
			 * @return 0 on success, or -1 on error
			 */
			virtual int seek(int pos, EOrigin origin) = 0;

			/**
			 * @brief Get current cursor position.
			 * @return Current position in bytes
			 */
			virtual int tell() = 0;

			/**
			 * @brief Read data from the stream.
			 * @param buff Destination buffer
			 * @param len Number of bytes to read
			 * @return Number of bytes actually read
			 */
			virtual int read(unsigned char* buff, int len) = 0;

			/**
			 * @brief Check if the cursor is at the end of the stream.
			 * @return True if at EOF
			 */
			virtual bool endOfStream() = 0;

			/**
			 * @brief Get total size of the stream.
			 * @return Size in bytes
			 */
			virtual int size() = 0;

			/**
			 * @brief Check if a certain amount of data is ready to be read.
			 * @param len Number of bytes to check
			 * @return True if data is available
			 */
			virtual bool readyReadData(int len) = 0;

			/**
			 * @brief Trim read data from the stream (used in dynamic streams).
			 */
			virtual void trim() {}
		};
		
		/**
		 * @brief Interface for an audio data stream (File, Memory, etc.).
		 * @ingroup Audio
		 */
		class IStream
		{
		protected:
			int	m_referenceCount;
			
			// the sampleRate & channel for stream recoder
			int m_sampleRate;
			int m_channels;
			
		public:
			IStream()
			{
				m_referenceCount = 1;
				m_channels = 2;
				m_sampleRate = 0;
			}
			
			virtual ~IStream()
			{
			}
			
			/**
			 * @brief Create a new cursor for this stream.
			 * @return Pointer to a new IStreamCursor
			 */
			virtual IStreamCursor* createCursor() = 0;
			
			/**
			 * @brief Increment reference count.
			 */
			virtual void grab()
			{
				m_referenceCount++;
			}
			
			/**
			 * @brief Decrement reference count and delete if zero.
			 * @return True if the object was deleted
			 */
			virtual bool drop()
			{
				m_referenceCount--;
				if (m_referenceCount <= 0)
				{
					delete this;
					return true;
				}
				return false;
			}
			
			/**
			 * @brief Get expected sample rate (for recorder/procedural streams).
			 * @return Sample rate in Hz
			 */
			int getSampleRate()
			{
				return m_sampleRate;
			}
			
			/**
			 * @brief Get number of channels (for recorder/procedural streams).
			 * @return Number of channels
			 */
			int getChannels()
			{
				return m_channels;
			}
			
			/**
			 * @brief Set stream audio properties.
			 * @param sampleRate Sample rate in Hz
			 * @param channels Number of channels
			 */
			void setStreamAudio(int sampleRate, int channels)
			{
				m_sampleRate = sampleRate;
				m_channels = channels;
			}
			
			/**
			 * @brief Upload new data to the stream (used in online/dynamic streams).
			 * @param buffer Data buffer
			 * @param size Size in bytes
			 */
			virtual void uploadData(unsigned char* buffer, int size)
			{
				
			}
			
			/**
			 * @brief Stop the stream and clear buffers.
			 */
			virtual void stopStream()
			{
				
			}
		};
	}
}

#endif
