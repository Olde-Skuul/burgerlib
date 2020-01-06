/***************************************

	Time Manager Class
	Mac Carbon specific code

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtimedate.h"
#if defined(BURGER_MACOSX) || defined(DOXYGEN)
#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>

/***************************************

	\brief Obtain the current localized time.

	A query is made to the target platform and the structure is filled in with
	the current date and time.

	\return The structure is set to the current localized time.

***************************************/

void Burger::TimeDate_t::GetTime(void) BURGER_NOEXCEPT
{
	CFTimeZoneRef pTimeZone = CFTimeZoneCopySystem();
	if (pTimeZone) {
		CFAbsoluteTime dTime = CFAbsoluteTimeGetCurrent();
		CFGregorianDate TimeRec =
			CFAbsoluteTimeGetGregorianDate(dTime, pTimeZone);
		m_uYear = static_cast<uint_t>(TimeRec.year);
		m_bMonth = static_cast<uint8_t>(TimeRec.month);
		m_bDay = static_cast<uint8_t>(TimeRec.day);
		m_bHour = static_cast<uint8_t>(TimeRec.hour);
		m_bMinute = static_cast<uint8_t>(TimeRec.minute);
		m_bSecond = static_cast<uint8_t>(TimeRec.second);
		m_usMilliseconds = static_cast<uint16_t>(
			(TimeRec.second - static_cast<double>(m_bSecond)) * 1000.0);
		SInt32 uDay = CFAbsoluteTimeGetDayOfWeek(dTime, pTimeZone);
		if (uDay == 7) {
			uDay = 0;
		}
		m_bDayOfWeek = static_cast<uint8_t>(uDay);
		CFRelease(pTimeZone);
	} else {
		Clear();
	}
}

/***************************************

	\brief Convert a MacOS UTCDateTime into a Burger::TimeDate_t

	\note This function is only available on MacOS and MacOSX

	\return \ref FALSE if successful, non-zero if not.

***************************************/

uint_t Burger::TimeDate_t::Load(const UTCDateTime* pUTCDateTime)
{
	Clear();
	uint_t uResult = TRUE;
	CFAbsoluteTime AbsTime;
	if (!UCConvertUTCDateTimeToCFAbsoluteTime(pUTCDateTime, &AbsTime)) {
		uResult = Load(AbsTime);
	}
	return uResult;
}

/***************************************

	\brief Convert a Burger::TimeDate_t into a MacOS UTCDateTime

	\note This function is only available on MacOS and MacOSX

	\return \ref FALSE if successful, non-zero if not.

***************************************/

uint_t Burger::TimeDate_t::Store(UTCDateTime* pUTCDateTime) const
{
	CFAbsoluteTime AbsTime;
	uint_t uResult = Store(&AbsTime);
	if (!uResult) {
		uResult = static_cast<uint_t>(
			UCConvertCFAbsoluteTimeToUTCDateTime(AbsTime, pUTCDateTime));
	}
	return uResult;
}

/*! ************************************

	\brief Convert a MacOSX NSTimeInterval into a Burger::TimeDate_t

	\note This function is only available on iOS and MacOSX

	\return \ref FALSE if successful, non-zero if not.

***************************************/

uint_t Burger::TimeDate_t::Load(double dNSTimeInterval)
{
	uint_t uResult = TRUE;
	CFTimeZoneRef pTimeZone = CFTimeZoneCopySystem();
	if (pTimeZone) {
		CFGregorianDate TimeRec =
			CFAbsoluteTimeGetGregorianDate(dNSTimeInterval, pTimeZone);
		m_uYear = static_cast<uint_t>(TimeRec.year);
		m_bMonth = static_cast<uint8_t>(TimeRec.month);
		m_bDay = static_cast<uint8_t>(TimeRec.day);
		m_bHour = static_cast<uint8_t>(TimeRec.hour);
		m_bMinute = static_cast<uint8_t>(TimeRec.minute);
		m_bSecond = static_cast<uint8_t>(TimeRec.second);
		m_usMilliseconds = static_cast<uint16_t>(
			(TimeRec.second - static_cast<double>(m_bSecond)) * 1000.0);
		SInt32 uDay = CFAbsoluteTimeGetDayOfWeek(dNSTimeInterval, pTimeZone);
		if (uDay == 7) {
			uDay = 0;
		}
		m_bDayOfWeek = static_cast<uint8_t>(uDay);		
		CFRelease(pTimeZone);
		uResult = FALSE;
	}
	return uResult;
}

/*! ************************************

	\brief Convert a Burger::TimeDate_t into a MacOSX NSTimeInterval

	\note This function is only available on iOS and MacOSX

	\return \ref FALSE if successful, non-zero if not.

***************************************/

uint_t Burger::TimeDate_t::Store(double* pNSTimeInterval) const
{
	uint_t uResult = TRUE;
	CFTimeZoneRef pTimeZone = CFTimeZoneCopySystem();
	if (pTimeZone) {
		CFGregorianDate TimeRec;
		TimeRec.year = static_cast<SInt32>(m_uYear);
		TimeRec.month = static_cast<SInt8>(m_bMonth);
		TimeRec.day = static_cast<SInt8>(m_bDay);
		TimeRec.hour = static_cast<SInt8>(m_bHour);
		TimeRec.minute = static_cast<SInt8>(m_bMinute);
		TimeRec.second =
			static_cast<double>((m_bSecond * 1000) + m_usMilliseconds) / 1000.0;
		pNSTimeInterval[0] = CFGregorianDateGetAbsoluteTime(TimeRec, pTimeZone);
		CFRelease(pTimeZone);
		uResult = FALSE;
	}
	return uResult;
}

#endif