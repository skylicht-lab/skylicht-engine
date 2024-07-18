/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "CWinThread.h"

#ifdef USE_WINTHREAD

namespace Skylicht
{
	namespace System
	{
		DWORD WINAPI threadFunction(LPVOID lpParam)
		{
			CWinThread* p = reinterpret_cast<CWinThread*>(lpParam);
			p->update();
			return 0;
		}

		CWinThread::CWinThread(IThreadCallback* callback) :
		IThread(callback),
		m_run(false)
		{
			printf("[CWinThread] created\n");
			DWORD threadID;
			m_thread = CreateThread(
									NULL,       // default security attributes
									0,          // default stack size
									(LPTHREAD_START_ROUTINE)threadFunction,
									this,       // thread function arguments
									0,          // default creation flags
									&threadID); // receive thread identifier;
			
			if (m_thread == NULL)
			{
				printf("[CWinThread] Warning: init error: %d\n", GetLastError());
			}
			
			m_loopMutex = CreateMutex(NULL, FALSE, NULL);
		}

		CWinThread::~CWinThread()
		{
			stop();
			
			CloseHandle(m_thread);
			CloseHandle(m_loopMutex);
		}

		void CWinThread::update()
		{
			if (m_callback == NULL)
			{
				printf("[CWinThread] quit - no Callback\n");
				return;
			}
			
			printf("[CWinThread] run update\n");
			
			// run thread
			m_run = m_callback->enableThreadLoop();
			
			bool needUnlock = false;
			if (m_run)
			{
				WaitForSingleObject(m_loopMutex, INFINITE);
				needUnlock = true;
			}
			
			m_callback->runThread();
			
			// callback
			while (m_run)
			{
				ReleaseMutex(m_loopMutex);
				m_callback->updateThread();
				WaitForSingleObject(m_loopMutex, INFINITE);
			}
			
			if (needUnlock)
			{
				ReleaseMutex(m_loopMutex);
			}
			
			printf("[CWinThread] end update\n");
			
			// IThread::sleep(1);
		}

		void CWinThread::stop()
		{
			if (m_run)
			{
				WaitForSingleObject(m_loopMutex, INFINITE);
				m_run = false;
				ReleaseMutex(m_loopMutex);
				
				WaitForSingleObject(m_thread, INFINITE);
				printf("[CWinThread] stop!\n");
			}
		}
	}
}

#endif
