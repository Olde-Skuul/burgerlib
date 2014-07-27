/***************************************

	Time Manager Class
	Mac Carbon specific code

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brtimedate.h"
#if defined(BURGER_MAC) || defined(DOXYGEN)
#include <DateTimeUtils.h>
#include <UTCUtils.h>

/***************************************

	\brief Obtain the current localized time.

	A query is made to the target platform and the structure
	is filled in with the current date and time.

	\return The structure is set to the current localized time.

***************************************/

void Burger::TimeDate_t::GetTime(void)
{
	DateTimeRec MyDate;					// MacOS time structure
	::GetTime(&MyDate);					// Call MacOS

	m_uYear = MyDate.year;
	m_usMilliseconds = 0;
	m_bMonth = static_cast<Word8>(MyDate.month);
	m_bDay = static_cast<Word8>(MyDate.day);
	m_bDayOfWeek = static_cast<Word8>(MyDate.dayOfWeek-1);	// Start the day at zero, since the mac returns 1-7
	m_bHour = static_cast<Word8>(MyDate.hour);
	m_bMinute = static_cast<Word8>(MyDate.minute);
	m_bSecond = static_cast<Word8>(MyDate.second);
}

/*! ************************************

	\brief Convert a MacOS UTCDateTime into a Burger::TimeDate_t
	
	\note This function is only available on iOS, MacOS and MacOSX

	\return \ref FALSE if successful, non-zero if not.

***************************************/

Word Burger::TimeDate_t::Load(const UTCDateTime *pUTCDateTime)
{
	Clear();
	Word uResult = TRUE;

	// Convert to my time zone

	LocalDateTime TempTime;
	if (!ConvertUTCToLocalDateTime(pUTCDateTime,&TempTime)) {
		LongDateRec LongTimeRec;
		LongDateCvt Convert;
		Convert.hl.lHigh = TempTime.highSeconds;
		Convert.hl.lLow = TempTime.lowSeconds;
		LongSecondsToDate(&Convert.c,&LongTimeRec);
		m_uYear = (Word)LongTimeRec.od.oldDate.year;
		m_bMonth = (Word8)LongTimeRec.od.oldDate.month;
		m_bDay = (Word8)LongTimeRec.od.oldDate.day;
		m_bHour = (Word8)LongTimeRec.od.oldDate.hour;
		m_bMinute = (Word8)LongTimeRec.od.oldDate.minute;
		m_bSecond = (Word8)LongTimeRec.od.oldDate.second;
		m_usMilliseconds = (Word16)(((Word)TempTime.fraction * 1000)>>16);
		m_bDayOfWeek = (Word8)LongTimeRec.od.oldDate.dayOfWeek;
		uResult = FALSE;
	}
	return uResult;
}

/*! ************************************

	\brief Convert a Burger::TimeDate_t into a MacOS UTCDateTime 
	
	\note This function is only available on iOS, MacOS and MacOSX

	\return \ref FALSE if successful, non-zero if not.

***************************************/

// TODO
Word Burger::TimeDate_t::Store(UTCDateTime *pUTCDateTime) const
{
	Word uResult = TRUE;
	return uResult;
}

#endif