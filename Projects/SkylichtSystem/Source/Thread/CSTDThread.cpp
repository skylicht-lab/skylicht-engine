// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#include "stdafx.h"
#include "CSTDThread.h"

#ifdef USE_STDTHREAD

namespace SkylichtSystem
{

CSTDThread::CSTDThread(IThreadCallback *callback)
	:IThread(callback)
{
	m_run = false;

	m_thread = new std::thread(CSTDThread::run, this);
}

CSTDThread::~CSTDThread()
{
	stop();
}

void CSTDThread::update()
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

void CSTDThread::stop()
{
	if (m_run)
	{
		m_run = false;
		m_thread->join();
		printf("CSTDThread::stop Thread is stop!\n");
	}
}

void* CSTDThread::run(void *param)
{
	CSTDThread* p = (CSTDThread*)param;
	p->update();
	return 0;
}

}

#endif