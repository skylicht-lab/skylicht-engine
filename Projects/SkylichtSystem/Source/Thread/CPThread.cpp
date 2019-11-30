// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

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