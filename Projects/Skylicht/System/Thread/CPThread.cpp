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
#include "CPThread.h"

#ifdef USE_PTHREAD

namespace Skylicht
{
	namespace System
	{
		CPThread::CPThread(IThreadCallback* callback) :
			IThread(callback),
			m_run(false),
			m_started(false),
			m_joined(false),
			m_stopRequested(false)
		{
			pthread_mutex_init(&m_loopMutex, 0);

			int result = pthread_create(&m_pthread, 0, CPThread::run, this);
			if (result != 0)
			{
				printf("CPThread::CPThread error in creating thread\n");
			}
			else
			{
				m_started = true;
			}
		}

		CPThread::~CPThread()
		{
			stop();

			pthread_mutex_destroy(&m_loopMutex);
		}

		void* CPThread::run(void* param)
		{
			CPThread* p = (CPThread*)param;
			p->update();
			return 0;
		}

		void CPThread::update()
		{
			if (m_callback == NULL)
				return;

			// todo run thread
			bool run = m_callback->enableThreadLoop();

			pthread_mutex_lock(&m_loopMutex);
			m_run = m_stopRequested ? false : run;
			pthread_mutex_unlock(&m_loopMutex);

			m_callback->runThread();

			// callback
			while (true)
			{
				pthread_mutex_lock(&m_loopMutex);
				run = m_run;
				pthread_mutex_unlock(&m_loopMutex);

				if (run == false)
					break;

				m_callback->updateThread();
			}

			pthread_mutex_lock(&m_loopMutex);
			m_run = false;
			pthread_mutex_unlock(&m_loopMutex);

			// IThread::sleep(1);
		}

		void CPThread::stop()
		{
			bool needJoin = false;

			pthread_mutex_lock(&m_loopMutex);
			m_stopRequested = true;
			m_run = false;
			needJoin = m_started && !m_joined && pthread_equal(pthread_self(), m_pthread) == 0;
			pthread_mutex_unlock(&m_loopMutex);

			if (needJoin)
			{
				printf("CPThread::stop\n");
				pthread_join(m_pthread, 0);

				pthread_mutex_lock(&m_loopMutex);
				m_joined = true;
				pthread_mutex_unlock(&m_loopMutex);

				printf("CPThread::stop Thread is stop!\n");
			}
		}
	}
}

#endif
