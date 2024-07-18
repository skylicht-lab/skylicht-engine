#pragma once

#include "Base.hh"
#include "Thread/IThread.h"
#include "Thread/IMutex.h"

using namespace Skylicht;

class TestThreadCallback: public Skylicht::System::IThreadCallback
{
protected:
	Skylicht::System::IMutex *m_mutex;

public:
	TestThreadCallback();	

	virtual ~TestThreadCallback();

	virtual void runThread();

	virtual void updateThread();

};

void testSystemThread();

bool isSystemThreadPass();
