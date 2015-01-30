/***************************************

	Time Manager Class
	iOS specific code

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brtimedate.h"
#if defined(BURGER_IOS) || defined(DOXYGEN)
#include <CoreFoundation/CoreFoundation.h>

/***************************************
 
	\brief Obtain the current localized time.
 
	A query is made to the target platform and the structure
	is filled in with the current date and time.
 
	\return The structure is set to the current localized time.
 
 ***************************************/

void Burger::TimeDate_t::GetTime(void)
{
	CFTimeZoneRef pTimeZone = CFTimeZoneCopySystem();
	if (pTimeZone) {
		CFAbsoluteTime dTime = CFAbsoluteTimeGetCurrent();
		CFGregorianDate TimeRec = CFAbsoluteTimeGetGregorianDate(dTime,pTimeZone);
		m_uYear = static_cast<Word>(TimeRec.year);
		m_bMonth = static_cast<Word8>(TimeRec.month);
		m_bDay = static_cast<Word8>(TimeRec.day);
		m_bHour = static_cast<Word8>(TimeRec.hour);
		m_bMinute = static_cast<Word8>(TimeRec.minute);
		m_bSecond = static_cast<Word8>(TimeRec.second);
		m_usMilliseconds = static_cast<Word16>((TimeRec.second-static_cast<double>(m_bSecond))* 1000.0);
		m_bDayOfWeek = static_cast<Word8>(CFAbsoluteTimeGetDayOfWeek(dTime,pTimeZone));
		CFRelease(pTimeZone);
	} else {
		Clear();
	}
}

/*! ************************************

	\brief Convert a MacOSX NSTimeInterval into a Burger::TimeDate_t

	\note This function is only available on iOS and MacOSX
	
	\return \ref FALSE if successful, non-zero if not.

***************************************/

Word Burger::TimeDate_t::Load(double dNSTimeInterval)
{
	Word uResult = TRUE;
	CFTimeZoneRef pTimeZone = CFTimeZoneCopySystem();
	if (pTimeZone) {
		CFGregorianDate TimeRec = CFAbsoluteTimeGetGregorianDate(dNSTimeInterval,pTimeZone);
		m_uYear = static_cast<Word>(TimeRec.year);
		m_bMonth = static_cast<Word8>(TimeRec.month);
		m_bDay = static_cast<Word8>(TimeRec.day);
		m_bHour = static_cast<Word8>(TimeRec.hour);
		m_bMinute = static_cast<Word8>(TimeRec.minute);
		m_bSecond = static_cast<Word8>(TimeRec.second);
		m_usMilliseconds = static_cast<Word16>((TimeRec.second-static_cast<double>(m_bSecond))* 1000.0);
		m_bDayOfWeek = static_cast<Word8>(CFAbsoluteTimeGetDayOfWeek(dNSTimeInterval,pTimeZone));
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

Word Burger::TimeDate_t::Store(double *pNSTimeInterval) const
{
	Word uResult = TRUE;
	CFTimeZoneRef pTimeZone = CFTimeZoneCopySystem();
	if (pTimeZone) {
		CFGregorianDate TimeRec;
		TimeRec.year = static_cast<SInt32>(m_uYear);
		TimeRec.month = static_cast<SInt8>(m_bMonth);
		TimeRec.day = static_cast<SInt8>(m_bDay);
		TimeRec.hour = static_cast<SInt8>(m_bHour);
		TimeRec.minute = static_cast<SInt8>(m_bMinute);
		TimeRec.second = static_cast<double>((m_bSecond*1000)+m_usMilliseconds)/1000.0;
		pNSTimeInterval[0] = CFGregorianDateGetAbsoluteTime(TimeRec,pTimeZone);
		CFRelease(pTimeZone);
		uResult = FALSE;
	}
	return uResult;
}

#endif