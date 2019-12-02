#pragma once

#include "Base.hh"
#include "Thread/IThread.h"
#include "Thread/IMutex.h"

bool g_threadPass = false;
SkylichtSystem::IThread *g_thread = NULL;

class TestThreadCallback: public SkylichtSystem::IThreadCallback
{
protected:
	SkylichtSystem::IMutex *m_mutex;

public:
	TestThreadCallback()
	{
		m_mutex = SkylichtSystem::IMutex::createMutex();
	}

	virtual ~TestThreadCallback()
	{

	}

	virtual void updateThread()
	{
		SkylichtSystem::SScopeMutex lock(m_mutex);
		g_threadPass = true;
	}
};

void testSystemThread()
{
	TEST_CASE("System Thread Start");
	g_thread = SkylichtSystem::IThread::createThread(new TestThreadCallback());
}