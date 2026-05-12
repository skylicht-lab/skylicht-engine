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

#ifndef _STREAM_FACTORY_
#define _STREAM_FACTORY_

#include "IStream.h"

namespace Skylicht
{
	namespace Audio
	{
		/**
		 * @brief Interface for stream factories. 
		 * Register a custom factory to CAudioEngine to support different data sources (e.g. ZIP, Network).
		 * @ingroup Audio
		 * @code
		 * // Example: Implementing a factory for ZIP archives using Irrlicht's file system
		 * class CZipAudioStreamFactory : public Audio::CStreamFactory {
		 * public:
		 *     virtual Audio::IStream* createStreamFromFile(const char* fileName) {
		 *         // Create a custom stream that reads from ZIP
		 *         CZipAudioFileStream* file = new CZipAudioFileStream(fileName);
		 *         if (file->isError()) {
		 *             delete file;
		 *             return NULL;
		 *         }
		 *         return file;
		 *     }
		 * };
		 * 
		 * // Registration
		 * CZipAudioStreamFactory* factory = new CZipAudioStreamFactory();
		 * Audio::CAudioEngine::getSoundEngine()->registerStreamFactory(factory);
		 * @endcode
		 */
		class IStreamFactory
		{
		public:
			virtual ~IStreamFactory()
			{
			}
			
			/**
			 * @brief Create an audio stream from a memory buffer.
			 * @param buffer The memory buffer
			 * @param size Size in bytes
			 * @param takeOwnership If true, the factory/stream will delete the buffer
			 * @return Pointer to IStream
			 */
			virtual IStream* createStreamFromMemory(unsigned char* buffer, int size, bool takeOwnership) = 0;

			/**
			 * @brief Create an audio stream from a file.
			 * @param fileName Path to the file
			 * @return Pointer to IStream
			 */
			virtual IStream* createStreamFromFile(const char* fileName) = 0;

			/**
			 * @brief Create a stream for dynamic/online data.
			 * @return Pointer to IStream
			 */
			virtual IStream* createOnlineStream() = 0;
		};
	}
}

#endif
