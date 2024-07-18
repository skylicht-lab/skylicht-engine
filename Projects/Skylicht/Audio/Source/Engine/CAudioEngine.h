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
		void initSkylichtAudio();
		
		void releaseSkylichtAudio();
		
		void updateSkylichtAudio();
		
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
			
		public:
			static CAudioEngine* getSoundEngine();
			
			static void shutdownEngine();
			
			CAudioEngine();
			
			virtual ~CAudioEngine();
			
			void init();
			
			void shutdown();
			
			void pauseEngine();
			
			void resumeEngine();
			
			void updateDriver();
			
			void stopAllSound();
			
			virtual void updateEmitter();
			
			virtual void updateThread();
			
			void lockThread()
			{
				m_mutex->lock();
				
				if (m_driver)
					m_driver->lockThread();
			}
			
			void unLockThread()
			{
				m_mutex->unlock();
				
				if (m_driver)
					m_driver->unlockThread();
			}
			
			ISoundDriver* getSoundDriver()
			{
				return m_driver;
			}
			
			void registerStreamFactory(IStreamFactory* streamFactory);
			
			void unRegisterStreamFactory(IStreamFactory* streamFactory);
			
			IStream* createStreamFromMemory(unsigned char* buffer, int size, bool takeOwnerShip = false);
			
			IStream* createStreamFromFile(const char* fileName);
			
			IStream* createStreamFromFileAndCache(const char* fileName, bool cache);
			
			IStream* createOnlineStream();
			
			CAudioEmitter* createAudioEmitter(IStream* stream, IAudioDecoder::EDecoderType decode);
			
			CAudioEmitter* createAudioEmitter(const char* fileName, bool cache);
			
			CAudioEmitter* createRawAudioEmitter(IStream* stream);
			
			CAudioReader* createAudioReader(IStream* stream, IAudioDecoder::EDecoderType decode);
			
			CAudioReader* createAudioReader(const char* fileName);
			
			void destroyEmitter(CAudioEmitter* emitter);
			
			void destroyReader(CAudioReader* reader);
			
			void destroyAllEmitter();
			
			void destroyAllReader();
			
			void releaseAllStream();
			
			void setListener(float posx, float posy, float posz, float upx, float upy, float upz, float frontx, float fronty, float frontz);
			
			const SListener& getListener()
			{
				return m_listener;
			}
		};
	}
};

#endif
