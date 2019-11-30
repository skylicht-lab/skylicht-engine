// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#ifndef _SKYLICHT_SYSTEM_ITHREAD_H_
#define _SKYLICHT_SYSTEM_ITHREAD_H_

namespace SkylichtSystem
{
	class IThreadCallback
	{
	public:
		virtual void updateThread() = 0;
	};


	class IThread
	{
	protected:
		IThreadCallback *m_callback;

	public:
		IThread(IThreadCallback* callback)
		{
			m_callback = callback;
		}

		virtual ~IThread()
		{
		}

		virtual void update() = 0;
		virtual void stop() = 0;

		// sleep
		static void sleep(unsigned int time);
		static float getTime();

		// createThread
		// create new thread
		static IThread* createThread(IThreadCallback* callback);
	};
};

#endif