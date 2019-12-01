// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#ifndef _GAMESINGLETON_H_
#define _GAMESINGLETON_H_

namespace Skylicht
{

template <class T> class CGameSingleton
{
	private:
		CGameSingleton(const CGameSingleton&);
		CGameSingleton& operator = (const CGameSingleton&);
	protected:
		CGameSingleton() {}
		virtual ~CGameSingleton() { s_instance = 0; }
	public:
		static T* s_instance;

		static T* createGetInstance()
		{                    
			if (!s_instance)				
				s_instance = new T();
			
			return s_instance;
		};

		inline static T* getInstance()
		{
			return s_instance;
		}

		static void releaseInstance()
		{
			if (s_instance)
			{
				delete s_instance;
				s_instance = NULL;
			}
		}
};

template <class T>
T* CGameSingleton<T>::s_instance = 0;

}

#endif