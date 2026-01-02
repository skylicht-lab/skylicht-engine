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

#pragma once

namespace Skylicht
{
	/**
	 * @class CDateTimeUtils
	 * @brief Utility functions for converting between date/time representations and seconds.
	 *
	 * CDateTimeUtils provides static helpers to:
	 *  - Convert an `ITimer::RealTimeDate` or date components to a number of seconds.
	 *  - Convert seconds into days/hours/minutes/seconds or into an `ITimer::RealTimeDate`.
	 *  - Convert simple day/hour counts into seconds.
	 *
	 * All conversion functions use the same calendar rules (Gregorian calendar with leap years).
	 * The precise epoch or reference year used by the engine is intentionally left unspecified
	 * in these helpers; when using the overload that accepts `fromYear`, that parameter acts as
	 * the reference start year for the conversion.
	 *
	 * @note These utilities do not perform timezone conversions. If you need timezone-aware
	 *       conversions or localization (DST shifting), handle those adjustments before calling
	 *       these methods or use the engine's timer / platform time utilities.
	 *
	 * @see ITimer::RealTimeDate
	 */
	class SKYLICHT_API CDateTimeUtils
	{
	public:
		/**
		 * @brief Convert a `ITimer::RealTimeDate` structure to a total number of seconds.
		 *
		 * The returned value represents the number of seconds that correspond to the provided
		 * date/time according to the engine's internal conversion rules (Gregorian calendar).
		 *
		 * @param date The input date/time to convert. Fields are those defined in `RealTimeDate`
		 *             (Year, Month [1-12], Day [1-31], Hour [0-23], Minute [0-59], Second).
		 * @return Total seconds representing the provided `date` relative to the engine's epoch.
		 */
		static unsigned long getSecondByDate(const ITimer::RealTimeDate& date);

		/**
		 * @brief Convert explicit date/time components to a total number of seconds.
		 *
		 * Use this overload when you want to specify a reference start year (`fromYear`) from which
		 * to compute the total seconds. Leap years are taken into account when computing
		 * the result.
		 *
		 * @param fromYear Reference start year (the epoch or base year for the conversion: 1970).
		 * @param month Month of the year [1-12].
		 * @param day Day of the month [1-31].
		 * @param years Absolute year value for the target date (e.g., 2026).
		 * @param hour Hour of the day [0-23].
		 * @param minute Minute of the hour [0-59].
		 * @param second Second of the minute [0-61].
		 * @return Total seconds between `fromYear` and the specified date/time.
		 */
		static unsigned long getSecondByDate(int fromYear, int month, int day, int years, int hour, int minute, int second);

		/**
		 * @brief Get the current engine time represented in seconds.
		 *
		 * The returned value uses the same epoch/reference as the other conversion functions
		 * in this class. This is a convenience wrapper to obtain the current time as a
		 * numeric seconds value suitable for comparisons or arithmetic with values produced
		 * by `getSecondByDate`.
		 *
		 * @return Current time in seconds (engine epoch).
		 */
		static unsigned long getTimeBySecond();

		/**
		 * @brief Decompose a duration in seconds into days, hours, minutes and seconds.
		 *
		 * This converts a positive number of seconds into its equivalent component parts:
		 * days, hours, minutes and seconds. The `day` output is the number of full days,
		 * `hour` is the number of hours remaining after days are removed, etc.
		 *
		 * @param second Input duration in seconds to decompose.
		 * @param day Output parameter that receives the number of full days.
		 * @param hour Output parameter that receives the remaining hours [0-23].
		 * @param min Output parameter that receives the remaining minutes [0-59].
		 * @param sec Output parameter that receives the remaining seconds [0-59].
		 */
		static void getDateTimeBySecond(unsigned long second, int& day, int& hour, int& min, int& sec);

		/**
		 * @brief Convert a seconds value into an `ITimer::RealTimeDate`.
		 *
		 * This reverses the effect of `getSecondByDate` and returns a calendar date/time
		 * corresponding to the supplied seconds value according to the engine's conversion rules.
		 *
		 * @param second Seconds value to convert (engine epoch).
		 * @return `ITimer::RealTimeDate` representing the given seconds value.
		 */
		static ITimer::RealTimeDate getDateBySecond(unsigned long second);

		/**
		 * @brief Convert a day count into seconds.
		 *
		 * This is a simple utility that multiplies `day` by the number of seconds in a day
		 * (24 * 60 * 60). Use when you need to add or compare durations expressed in days.
		 *
		 * @param day Number of days.
		 * @return Number of seconds corresponding to `day` days.
		 */
		static long getDateToSecond(int day);

		/**
		 * @brief Convert an hour count into seconds.
		 *
		 * Multiplies `hour` by the number of seconds in an hour (60 * 60).
		 *
		 * @param hour Number of hours.
		 * @return Number of seconds corresponding to `hour` hours.
		 */
		static long getHourToSecond(int hour);
	};
}