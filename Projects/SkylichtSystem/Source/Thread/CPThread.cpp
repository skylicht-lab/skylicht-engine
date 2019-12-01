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

namespace SkylichtSystem
{
	CPThread::CPThread(IThreadCallback *callback)
		:IThread(callback)
	{
		m_run = false;

		int result = pthread_create(&m_pthread, 0,CPThread::run, this);

		if(result != 0)
		{
			printf("CPThread::CPThread error in creating thread\n");
		}
	}

	CPThread::~CPThread()
	{
		stop();
	}

	void* CPThread::run(void *param)
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
		m_run = true;

		// callback
		while(m_run)
		{
			m_callback->updateThread();
		}

		IThread::sleep(500);
	}

	void CPThread::stop()
	{	
		if (m_run)
		{
			printf("CPThread::stop\n");
			m_run = false;

			pthread_join(m_pthread, 0);

			printf("CPThread::stop Thread is stop!\n");
		}
	}
}

#endif