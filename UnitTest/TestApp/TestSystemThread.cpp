#include "Base.hh"
#include "TestSystemThread.h"

#include "pch.h"

bool g_threadRunPass = false;
bool g_threadPass = false;
bool g_threadRelease = false;
SkylichtSystem::IThread* g_thread = NULL;

TestThreadCallback::TestThreadCallback()
{
	TEST_CASE("Mutex thread created");
	//m_mutex = SkylichtSystem::IMutex::createMutex();
	//TEST_ASSERT_THROW(m_mutex != NULL);
}

TestThreadCallback::~TestThreadCallback()
{
	TEST_CASE("System thread release");
	//delete m_mutex;
	g_threadRelease = true;
}

void TestThreadCallback::runThread()
{
	TEST_CASE("System thread run");
	g_threadRunPass = true;
}

void TestThreadCallback::updateThread()
{
	if (g_threadPass == false)
		TEST_CASE("System mutex lock");

	g_threadPass = true;
	//SkylichtSystem::SScopeMutex lock(m_mutex);
}

void testSystemThread()
{
	TEST_CASE("System thread start");
	g_thread = SkylichtSystem::IThread::createThread(new TestThreadCallback());

	getIrrlichtDevice()->sleep(100);
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
	return g_threadPass && g_threadRunPass && g_threadRelease;
}
