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
#include "CAudioEngine.h"
#include "CStreamFactory.h"

// mp3 library
#include "mpg123.h"

namespace Skylicht
{
	namespace Audio
	{
		CAudioEngine* g_engine = NULL;
		
		void initSkylichtAudio()
		{
			CAudioEngine::getSoundEngine()->init();
		}
		
		void releaseSkylichtAudio()
		{
			CAudioEngine::shutdownEngine();
		}
		
		void updateSkylichtAudio()
		{
			if (g_engine != NULL)
				g_engine->updateDriver();
		}
		
		CAudioEngine* CAudioEngine::getSoundEngine()
		{
			if (g_engine == NULL)
				g_engine = new CAudioEngine();
			
			return g_engine;
		}
		
		void CAudioEngine::shutdownEngine()
		{
			if (g_engine)
				delete g_engine;
			
			g_engine = NULL;
		}
		
		CAudioEngine::CAudioEngine()
		{
			m_mutex = IMutex::createMutex();
		}
		
		CAudioEngine::~CAudioEngine()
		{
			shutdown();
			delete m_mutex;
		}
		
		void CAudioEngine::init()
		{
			// sound driver
			m_driver = ISoundDriver::createDriver();
			
			// init stream factory
			m_defaultStreamFactory = new CStreamFactory();
			registerStreamFactory(m_defaultStreamFactory);
			
			// init mp3 library
			mpg123_init();
			
			// start thread
	#ifdef USE_MULTITHREAD_UPDATE
			m_thread = IThread::createThread(this);
	#else
			m_thread = NULL;
	#endif
		}
		
		void CAudioEngine::shutdown()
		{
			// stop thread
			if (m_thread != NULL)
			{
				m_thread->stop();
				delete m_thread;
			}
			
			// release emitter
			destroyAllEmitter();
			
			// release stream
			releaseAllStream();
			
			// release stream factory
			unRegisterStreamFactory(m_defaultStreamFactory);
			delete m_defaultStreamFactory;
			
			// delete driver
			delete m_driver;
			
			// todo: free mp3 library
			mpg123_exit();
		}
		
		void CAudioEngine::pauseEngine()
		{
			if (m_driver)
				m_driver->suspend();
			
			// stop thread
			if (m_thread != NULL)
			{
				m_thread->stop();
				delete m_thread;
				m_thread = NULL;
			}
		}
		
		void CAudioEngine::resumeEngine()
		{
			if (m_driver)
				m_driver->resume();
			
			// start thread
	#ifdef USE_MULTITHREAD_UPDATE
			m_thread = IThread::createThread(this);
	#else
			m_thread = NULL;
	#endif
		}
		
		void CAudioEngine::updateDriver()
		{
			if (m_driver)
				m_driver->update();
		}
		
		void CAudioEngine::stopAllSound()
		{
			m_mutex->lock();
			std::vector<CAudioEmitter*>::iterator i = m_emitters.begin(), end = m_emitters.end();
			while (i != end)
			{
				(*i)->stop();
				++i;
			}
			m_mutex->unlock();
		}
		
		void CAudioEngine::updateEmitter()
		{
			m_mutex->lock();
			
			// todo update sound engine
			std::vector<CAudioEmitter*>::iterator i = m_emitters.begin(), end = m_emitters.end();
			
			while (i != end)
			{
				CAudioEmitter* emitter = (*i);
				emitter->update();
				++i;
			}
			
			m_mutex->unlock();
		}
		
		void CAudioEngine::updateThread()
		{
			// todo update in-thread
			unsigned int tpf = 1000 / 30;
			float timeBegin = IThread::getTime();
			
			updateEmitter();
			
			// calc time to sleep
			float timeEnd = IThread::getTime();
			float dt = timeEnd - timeBegin;
			int sleep = tpf - (int)dt;
			if (sleep < 0)
				sleep = 1;
			
			IThread::sleep(sleep);
		}
		
		void CAudioEngine::registerStreamFactory(IStreamFactory* streamFactory)
		{
			std::vector<IStreamFactory*>::iterator i = m_streamFactorys.begin(), end = m_streamFactorys.end();
			while (i != end)
			{
				if ((*i) == streamFactory)
					return;
				++i;
			}
			m_streamFactorys.push_back(streamFactory);
		}
		
		void CAudioEngine::unRegisterStreamFactory(IStreamFactory* streamFactory)
		{
			std::vector<IStreamFactory*>::iterator i = m_streamFactorys.begin(), end = m_streamFactorys.end();
			while (i != end)
			{
				if ((*i) == streamFactory)
				{
					m_streamFactorys.erase(i);
					return;
				}
				++i;
			}
		}
		
		IStream* CAudioEngine::createStreamFromMemory(unsigned char* buffer, int size, bool takeOwnerShip)
		{
			int numFactory = (int)m_streamFactorys.size();
			for (int i = numFactory - 1; i >= 0; i--)
			{
				IStream* s = m_streamFactorys[i]->createStreamFromMemory(buffer, size, takeOwnerShip);
				if (s)
					return s;
			}
			return NULL;
		}
		
		IStream* CAudioEngine::createStreamFromFile(const char* fileName)
		{
			int numFactory = (int)m_streamFactorys.size();
			for (int i = numFactory - 1; i >= 0; i--)
			{
				IStream* s = m_streamFactorys[i]->createStreamFromFile(fileName);
				if (s)
					return s;
			}
			printLog("[SkylichtAudio] Can not open file: %s", fileName);
			return NULL;
		}
		
		IStream* CAudioEngine::createStreamFromFileAndCache(const char* fileName, bool cache)
		{
			IStream* stream = NULL;
			
			// search in cache
			std::map<std::string, IStream*>::iterator it = m_fileToStream.find(fileName);
			
			if (it == m_fileToStream.end())
			{
				// need open stream
				stream = createStreamFromFile(fileName);
				
				if (stream && cache)
				{
					Skylicht::Audio::IStream* oldStream = stream;
					Skylicht::Audio::IStreamCursor* streamCursor = stream->createCursor();
					int size = streamCursor->size();
					
					unsigned char* buffer = new unsigned char[size];
					streamCursor->read(buffer, size);
					
					stream = CAudioEngine::getSoundEngine()->createStreamFromMemory(buffer, size);
					
					delete streamCursor;
					delete oldStream;
					delete[]buffer;
					
					// cache the memory stream
					if (stream != NULL)
					{
						m_fileToStream[fileName] = stream;
						stream->grab();
					}
				}
			}
			else
			{
				// get from cache
				stream = it->second;
			}
			
			return stream;
		}
		
		IStream* CAudioEngine::createOnlineStream()
		{
			int numFactory = (int)m_streamFactorys.size();
			for (int i = numFactory - 1; i >= 0; i--)
			{
				IStream* s = m_streamFactorys[i]->createOnlineStream();
				if (s)
					return s;
			}
			return NULL;
		}
		
		CAudioEmitter* CAudioEngine::createAudioEmitter(IStream* stream, IAudioDecoder::EDecoderType decode)
		{
			SScopeMutex lockScope(m_mutex);
			
			CAudioEmitter* emitter = new CAudioEmitter(stream, decode, m_driver);
			m_emitters.push_back(emitter);
			return emitter;
		}
		
		CAudioEmitter* CAudioEngine::createAudioEmitter(const char* fileName, bool cache)
		{
			SScopeMutex lockScope(m_mutex);
			
			CAudioEmitter* emitter = new CAudioEmitter(fileName, cache, m_driver);
			m_emitters.push_back(emitter);
			return emitter;
		}
		
		CAudioEmitter* CAudioEngine::createRawAudioEmitter(IStream* stream)
		{
			SScopeMutex lockScope(m_mutex);
			
			CAudioEmitter* emitter = new CAudioEmitter(stream, IAudioDecoder::RawWav, m_driver);
			m_emitters.push_back(emitter);
			return emitter;
		}
		
		CAudioReader* CAudioEngine::createAudioReader(IStream* stream, IAudioDecoder::EDecoderType decode)
		{
			SScopeMutex lockScope(m_mutex);
			
			CAudioReader* reader = new CAudioReader(stream, decode, m_driver);
			m_readers.push_back(reader);
			return reader;
		}
		
		CAudioReader* CAudioEngine::createAudioReader(const char* fileName)
		{
			SScopeMutex lockScope(m_mutex);
			
			CAudioReader* reader = new CAudioReader(fileName, m_driver);
			m_readers.push_back(reader);
			return reader;
		}
		
		void CAudioEngine::destroyEmitter(CAudioEmitter* emitter)
		{
			SScopeMutex lockScope(m_mutex);
			std::vector<CAudioEmitter*>::iterator i = m_emitters.begin(), end = m_emitters.end();
			
			while (i != end)
			{
				if ((*i) == emitter)
				{
					delete emitter;
					m_emitters.erase(i);
					return;
				}
				++i;
			}
		}
		
		void CAudioEngine::destroyReader(CAudioReader* reader)
		{
			SScopeMutex lockScope(m_mutex);
			std::vector<CAudioReader*>::iterator i = m_readers.begin(), end = m_readers.end();
			
			while (i != end)
			{
				if ((*i) == reader)
				{
					delete reader;
					m_readers.erase(i);
					return;
				}
				++i;
			}
		}
		
		void CAudioEngine::releaseAllStream()
		{
			std::map<std::string, IStream*>::iterator i = m_fileToStream.begin(), end = m_fileToStream.end();
			while (i != end)
			{
				i->second->drop();
				++i;
			}
			m_fileToStream.clear();
		}
		
		void CAudioEngine::destroyAllEmitter()
		{
			SScopeMutex lockScope(m_mutex);
			std::vector<CAudioEmitter*>::iterator i = m_emitters.begin(), end = m_emitters.end();
			
			while (i != end)
			{
				delete (*i);
				++i;
			}
			
			m_emitters.clear();
		}
		
		void CAudioEngine::destroyAllReader()
		{
			SScopeMutex lockScope(m_mutex);
			std::vector<CAudioReader*>::iterator i = m_readers.begin(), end = m_readers.end();
			
			while (i != end)
			{
				delete (*i);
				++i;
			}
			
			m_readers.clear();
		}
		
		void CAudioEngine::setListener(float posx, float posy, float posz, float upx, float upy, float upz, float frontx, float fronty, float frontz)
		{
			SScopeMutex lockScope(m_mutex);
			m_listener.Position.X = posx;
			m_listener.Position.Y = posy;
			m_listener.Position.Z = posz;
			
			m_listener.Up.X = upx;
			m_listener.Up.Y = upy;
			m_listener.Up.Z = upz;
			
			m_listener.LookAt.X = frontx;
			m_listener.LookAt.Y = fronty;
			m_listener.LookAt.Z = frontz;
		}
	}
}
