/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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

#pragma once

#include "Utils/CGameSingleton.h"

namespace Skylicht
{
	class CConsoleLog : public CGameSingleton<CConsoleLog>
	{
	public:
		struct SLogInfo
		{
			u32 ID;
			std::string Log;
			ELOG_LEVEL LogLevel;

			SLogInfo(const char *log, ELOG_LEVEL level, u32 id)
			{
				ID = id;
				Log = log;
				LogLevel = level;
			}
		};

	protected:
		bool m_enable;

		u32 m_id;
		u32 m_maxLineBuffer;

		std::list<SLogInfo> m_logs;

		std::string m_buffer;

	public:
		CConsoleLog();

		virtual ~CConsoleLog();

		void setMaxLineBuffer(u32 num);

		void clear();

		void write(const char *log, ELOG_LEVEL logLevel);

		const std::string& getBuffer(bool update);

		inline u32 getLogCount()
		{
			return (u32)m_logs.size();
		}

		inline std::list<SLogInfo>& getLogs()
		{
			return m_logs;
		}

		inline const SLogInfo& getLast()
		{
			return m_logs.back();
		}

		inline void setEnable(bool b)
		{
			m_enable = b;
		}

		inline bool isEnable()
		{
			return m_enable;
		}
	};
}