/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "CSTDThread.h"

#ifdef USE_STDTHREAD

namespace Skylicht
{
	namespace System
	{
		CSTDThread::CSTDThread(IThreadCallback* callback) :
			IThread(callback),
			m_thread(NULL),
			m_run(false),
			m_started(false),
			m_joined(false),
			m_stopRequested(false)
		{
			printf("[CSTDThread] created\n");
			m_thread = new std::thread(CSTDThread::run, this);
			m_started = true;
		}

		CSTDThread::~CSTDThread()
		{
			stop();

			delete m_thread;
		}

		void CSTDThread::update()
		{
			if (m_callback == NULL)
			{
				printf("[CSTDThread] quit - no Callback\n");
				return;
			}

			printf("[CSTDThread] run update\n");

			// run thread
			bool run = m_callback->enableThreadLoop();

			m_loopMutex.lock();
			m_run = m_stopRequested ? false : run;
			m_loopMutex.unlock();

			m_callback->runThread();

			// callback
			while (true)
			{
				m_loopMutex.lock();
				run = m_run;
				m_loopMutex.unlock();

				if (run == false)
					break;

				m_callback->updateThread();
			}

			m_loopMutex.lock();
			m_run = false;
			m_loopMutex.unlock();

			printf("[CSTDThread] end update\n");

			// IThread::sleep(1);
		}

		void CSTDThread::stop()
		{
			bool needJoin = false;

			m_loopMutex.lock();
			m_stopRequested = true;
			m_run = false;
			needJoin = m_started && !m_joined && m_thread != NULL && m_thread->joinable() && std::this_thread::get_id() != m_thread->get_id();
			if (needJoin)
				m_joined = true;
			m_loopMutex.unlock();

			if (needJoin)
			{
				m_thread->join();
				printf("[CSTDThread] stop!\n");
			}
		}

		void* CSTDThread::run(void* param)
		{
			CSTDThread* p = reinterpret_cast<CSTDThread*>(param);
			p->update();
			return 0;
		}
	}
}

#endif
