#include "Base.hh"
#include "TestSystemThread.h"

bool g_threadRunPass = false;
bool g_threadPass = false;
SkylichtSystem::IThread *g_thread = NULL;

TestThreadCallback::TestThreadCallback()
{
	m_mutex = SkylichtSystem::IMutex::createMutex();
}

TestThreadCallback::~TestThreadCallback()
{
	delete m_mutex;
}

void TestThreadCallback::runThread()
{
	g_threadRunPass = true;
}

void TestThreadCallback::updateThread()
{
	SkylichtSystem::SScopeMutex lock(m_mutex);
	g_threadPass = true;
}

void testSystemThread()
{
	TEST_CASE("System thread start");
	g_thread = SkylichtSystem::IThread::createThread(new TestThreadCallback());
}

bool isSystemThreadPass()
{
	TEST_CASE("System thread end");
    if (g_thread != NULL)
    {
        g_thread->stop();
        delete g_thread->getCallback();
        delete g_thread;
    }
	return g_threadPass && g_threadRunPass;
}
