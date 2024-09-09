/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include "pch.h"
#include "CDateTimeUtils.h"

#include <ctime>

#define DAYSPERWEEK (7)
#define DAYSPERNORMYEAR (365U)
#define DAYSPERLEAPYEAR (366U)

#define SECSPERDAY (86400UL) /// == ( 24 // 60 // 60) */
#define SECSPERHOUR (3600UL) /// == ( 60 // 60) */
#define SECSPERMIN (60UL) /// == ( 60) */

#define LEAPYEAR(year)          (!((year) % 4) && (((year) % 100) || !((year) % 400)))

namespace Skylicht
{

	const int k_ytab[2][12] = {
		{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
		{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
	};

	// getSecondByDate
	// Input     : int fromYear (ie, 1970)
	// Input     : int ptr to returned month
	// Input     : int ptr to returned day
	// Input     : int ptr to returned years since fromYear
	// Input     : int ptr to returned hour
	// Input     : int ptr to returned minute
	// Input     : int ptr to returned seconds
	//
	// Converts MM/DD/YY HH:MM:SS to actual seconds since fromYear.
	// fromYear year is assumed at Jan 1, 00:00:01am.
	unsigned long CDateTimeUtils::getSecondByDate(int fromYear, int month, int day, int years, int hour, int minute, int second)
	{
		unsigned long secs = 0;
		int countleap = 0;
		int i;
		int dayspermonth;

		years = years - fromYear;
		if (years < 0)
			years = 0;

		secs = years * (SECSPERDAY * 365);
		for (i = 0; i < years; i++)
		{
			if (LEAPYEAR(fromYear + i))
				countleap++;
		}
		secs += (countleap * SECSPERDAY);

		secs += second;
		secs += (hour * SECSPERHOUR);
		secs += (minute * SECSPERMIN);
		secs += ((day - 1) * SECSPERDAY);

		if (month > 1)
		{
			dayspermonth = 0;

			if (LEAPYEAR((fromYear + years))) // Only counts when we're on leap day or past it
			{
				dayspermonth = 1;
			}

			for (i = 0; i < month - 1; i++)
			{
				secs += (k_ytab[dayspermonth][i] * SECSPERDAY);
			}
		}

		return secs;
	}

	unsigned long CDateTimeUtils::getTimeBySecond()
	{
		ITimer::RealTimeDate dateTime = os::Timer::getRealTimeAndDate();
		return getSecondByDate(1970, dateTime.Month, dateTime.Day, dateTime.Year, dateTime.Hour, dateTime.Minute, dateTime.Second);
	}

	ITimer::RealTimeDate CDateTimeUtils::getDateBySecond(unsigned long second)
	{
		struct tm* timeinfo;

		// get time from 1970 to current time
		// time(&rawtime);
		time_t rawtime = second;

		// convert to UTM time
		timeinfo = gmtime(&rawtime);

		ITimer::RealTimeDate date;
		date.Hour = (u32)timeinfo->tm_hour;
		date.Minute = (u32)timeinfo->tm_min;
		date.Second = (u32)timeinfo->tm_sec;
		date.Day = (u32)timeinfo->tm_mday;
		date.Month = (u32)timeinfo->tm_mon + 1;
		date.Year = (u32)timeinfo->tm_year + 1900;
		date.Weekday = (ITimer::EWeekday)timeinfo->tm_wday;
		date.Yearday = (u32)timeinfo->tm_yday + 1;
		date.IsDST = timeinfo->tm_isdst != 0;

		return date;
	}

	void CDateTimeUtils::getDateTimeBySecond(unsigned long second, int& day, int& hour, int& min, int& sec)
	{
		const int cseconds_in_day = 86400;
		const int cseconds_in_hour = 3600;
		const int cseconds_in_minute = 60;
		const int cseconds = 1;

		day = second / cseconds_in_day;
		hour = (second % cseconds_in_day) / cseconds_in_hour;
		min = ((second % cseconds_in_day) % cseconds_in_hour) / cseconds_in_minute;
		sec = (((second % cseconds_in_day) % cseconds_in_hour) % cseconds_in_minute) / cseconds;
	}

	long CDateTimeUtils::getDateToSecond(int day)
	{
		const int cseconds_in_day = 86400;
		return day * cseconds_in_day;
	}

	long CDateTimeUtils::getHourToSecond(int hour)
	{
		const int cseconds_in_hour = 3600;
		return hour * cseconds_in_hour;
	}

}