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

#ifndef _CSKYLICHT_AUDIO_H_
#define _CSKYLICHT_AUDIO_H_

#include "stdafx.h"

#include "SkylichtAudioConfig.h"
#include "Stream/IStreamFactory.h"

#include "Decoder/IAudioDecoder.h"
#include "Driver/ISoundDriver.h"
#include "Thread/IMutex.h"
#include "Thread/IThread.h"

#include "CAudioEmitter.h"
#include "CAudioReader.h"

using namespace Skylicht::System;

namespace Skylicht
{
	namespace Audio
	{
		/**
		 * @brief Initialize the Skylicht Audio system.
		 * @ingroup Audio
		 */
		void initSkylichtAudio();

		/**
		 * @brief Release the Skylicht Audio system.
		 * @ingroup Audio
		 */
		void releaseSkylichtAudio();

		/**
		 * @brief Update the audio driver. Should be called every frame if multithreading is disabled.
		 * @ingroup Audio
		 */
		void updateSkylichtAudio();

		/**
		 * @brief The main audio engine singleton for managing audio playback and resources.
		 * @ingroup Audio
		 * @code
		 * #include "SkylichtAudio.h"
		 * 
		 * // Setup audio listener (e.g., from camera)
		 * Audio::CAudioEngine::getSoundEngine()->setListener(
		 *     pos.X, pos.Y, pos.Z,
		 *     up.X, up.Y, up.Z,
		 *     front.X, front.Y, front.Z
		 * );
		 * 
		 * // Create an emitter
		 * Audio::CAudioEmitter* music = Audio::CAudioEngine::getSoundEngine()->createAudioEmitter("music.mp3", false);
		 * music->play();
		 * @endcode
		 */
		class CAudioEngine : public IThreadCallback
		{
		protected:
			IThread* m_thread;

			IMutex* m_mutex;

			ISoundDriver* m_driver;

			IStreamFactory* m_defaultStreamFactory;

			std::vector<IStreamFactory*> m_streamFactorys;

			std::vector<CAudioEmitter*> m_emitters;

			std::vector<CAudioReader*> m_readers;

			std::map<std::string, IStream*>	m_fileToStream;

			SListener m_listener;

			bool m_pause;

		public:
			/**
			 * @brief Get the singleton instance of the audio engine.
			 * @return Pointer to CAudioEngine
			 */
			static CAudioEngine* getSoundEngine();

			/**
			 * @brief Shutdown and release the audio engine instance.
			 */
			static void shutdownEngine();

			CAudioEngine();

			virtual ~CAudioEngine();

			/**
			 * @brief Initialize the audio engine, driver, and background thread.
			 */
			void init();

			/**
			 * @brief Shutdown the audio engine and release all resources.
			 */
			void shutdown();

			/**
			 * @brief Suspend the audio engine and driver.
			 */
			void pauseEngine();

			/**
			 * @brief Resume the audio engine and driver.
			 */
			void resumeEngine();

			/**
			 * @brief Manually update the audio driver.
			 */
			void updateDriver();

			/**
			 * @brief Stop all active sounds.
			 */
			void stopAllSound();

			/**
			 * @brief Update all active emitters. Called by updateThread or updateSkylichtAudio.
			 */
			virtual void updateEmitter();

			/**
			 * @brief Background thread update loop.
			 */
			virtual void updateThread();

			/**
			 * @brief Lock audio thread for safe state modification.
			 */
			void lockThread()
			{
				m_mutex->lock();

				if (m_driver)
					m_driver->lockThread();
			}

			/**
			 * @brief Unlock audio thread.
			 */
			void unLockThread()
			{
				m_mutex->unlock();

				if (m_driver)
					m_driver->unlockThread();
			}

			/**
			 * @brief Get the active sound driver.
			 * @return Pointer to ISoundDriver
			 */
			ISoundDriver* getSoundDriver()
			{
				return m_driver;
			}

			/**
			 * @brief Register a custom stream factory.
			 * @param streamFactory The factory to register
			 */
			void registerStreamFactory(IStreamFactory* streamFactory);

			/**
			 * @brief Unregister a stream factory.
			 * @param streamFactory The factory to unregister
			 */
			void unRegisterStreamFactory(IStreamFactory* streamFactory);

			/**
			 * @brief Create an audio stream from a memory buffer.
			 * @param buffer The memory buffer containing audio data
			 * @param size The size of the buffer
			 * @param takeOwnerShip If true, the stream will delete the buffer upon destruction
			 * @return Pointer to IStream
			 */
			IStream* createStreamFromMemory(unsigned char* buffer, int size, bool takeOwnerShip = false);

			/**
			 * @brief Create an audio stream from a file.
			 * @param fileName Path to the audio file
			 * @return Pointer to IStream
			 */
			IStream* createStreamFromFile(const char* fileName);

			/**
			 * @brief Create an audio stream from a file with optional caching.
			 * @param fileName Path to the audio file
			 * @param cache If true, the file data will be cached in memory
			 * @return Pointer to IStream
			 */
			IStream* createStreamFromFileAndCache(const char* fileName, bool cache);

			/**
			 * @brief Create an empty online stream for streaming data over the network.
			 * @return Pointer to IStream
			 */
			IStream* createOnlineStream();

			/**
			 * @brief Create an audio emitter from a stream.
			 * @param stream The audio stream
			 * @param decode The decoder type
			 * @return Pointer to CAudioEmitter
			 */
			CAudioEmitter* createAudioEmitter(IStream* stream, IAudioDecoder::EDecoderType decode);

			/**
			 * @brief Create an audio emitter from a file.
			 * @param fileName Path to the audio file
			 * @param cache If true, the file data will be cached in memory
			 * @return Pointer to CAudioEmitter
			 */
			CAudioEmitter* createAudioEmitter(const char* fileName, bool cache);

			/**
			 * @brief Create an audio emitter for raw PCM data.
			 * @param stream The PCM data stream
			 * @return Pointer to CAudioEmitter
			 */
			CAudioEmitter* createRawAudioEmitter(IStream* stream);

			/**
			 * @brief Create an audio reader for decoding audio data without playback.
			 * @param stream The audio stream
			 * @param decode The decoder type
			 * @return Pointer to CAudioReader
			 */
			CAudioReader* createAudioReader(IStream* stream, IAudioDecoder::EDecoderType decode);

			/**
			 * @brief Create an audio reader from a file.
			 * @param fileName Path to the audio file
			 * @return Pointer to CAudioReader
			 */
			CAudioReader* createAudioReader(const char* fileName);

			/**
			 * @brief Destroy an audio emitter.
			 * @param emitter The emitter to destroy
			 */
			void destroyEmitter(CAudioEmitter* emitter);

			/**
			 * @brief Destroy an audio reader.
			 * @param reader The reader to destroy
			 */
			void destroyReader(CAudioReader* reader);

			/**
			 * @brief Destroy all audio emitters.
			 */
			void destroyAllEmitter();

			/**
			 * @brief Destroy all audio readers.
			 */
			void destroyAllReader();

			/**
			 * @brief Release all cached streams.
			 */
			void releaseAllStream();

			/**
			 * @brief Set the listener parameters for 3D sound.
			 * @param posx X coordinate of the listener
			 * @param posy Y coordinate of the listener
			 * @param posz Z coordinate of the listener
			 * @param upx X coordinate of the up vector
			 * @param upy Y coordinate of the up vector
			 * @param upz Z coordinate of the up vector
			 * @param frontx X coordinate of the forward vector
			 * @param fronty Y coordinate of the forward vector
			 * @param frontz Z coordinate of the forward vector
			 */
			void setListener(float posx, float posy, float posz, float upx, float upy, float upz, float frontx, float fronty, float frontz);

			/**
			 * @brief Get current listener parameters.
			 * @return SListener structure
			 */
			inline const SListener& getListener()
			{
				return m_listener;
			}

			/**
			 * @brief Check if the audio engine is paused.
			 * @return True if paused
			 */
			inline bool isPause()
			{
				return m_pause;
			}
		};
	}
};

#endif
