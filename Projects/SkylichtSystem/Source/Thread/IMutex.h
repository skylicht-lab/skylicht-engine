// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#ifndef _SKYLICHTSYSTEM_IMUTEX_H_
#define _SKYLICHTSYSTEM_IMUTEX_H_

namespace SkylichtSystem
{
	class IMutex
	{
	public:
		virtual ~IMutex()
		{
		}

		virtual void lock() = 0;
		virtual void unlock() = 0;

		static IMutex* createMutex();
	};


	struct SScopeMutex
	{
		IMutex* m;

		SScopeMutex(IMutex* mutex)
			:m(mutex)
		{
			if (m != NULL)
				m->lock();
		}

		~SScopeMutex()
		{
			if (m != NULL)
				m->unlock();
		}
	};
};

#endif