/***************************************

	Time Manager Class
	Playstation Vita specific code

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brtimedate.h"
#if defined(BURGER_VITA) || defined(DOXYGEN)
#include <rtc.h>
#pragma comment(lib,"SceRtc_stub")

/***************************************
 
	\brief Obtain the current localized time.
 
	A query is made to the target platform and the structure
	is filled in with the current date and time.
 
	\return The structure is set to the current localized time.
 
 ***************************************/

void Burger::TimeDate_t::GetTime(void) BURGER_NOEXCEPT
{
	SceDateTime LocalTime;

	if (sceRtcGetCurrentClockLocalTime(&LocalTime)>=SCE_OK) {
		m_uYear = static_cast<Word>(LocalTime.year);
		m_bMonth = static_cast<Word8>(LocalTime.month);
		m_bDay = static_cast<Word8>(LocalTime.day);
		m_bHour = static_cast<Word8>(LocalTime.hour);
		m_bMinute = static_cast<Word8>(LocalTime.minute);
		m_bSecond = static_cast<Word8>(LocalTime.second);
		m_usMilliseconds = static_cast<Word16>(LocalTime.microsecond/1000);
		m_bDayOfWeek = static_cast<Word8>(sceRtcGetDayOfWeek(LocalTime.year,LocalTime.month,LocalTime.day));
	} else {
		Clear();
	}
}

/*! ************************************

	\brief Convert a Playstation Vita SceDateTime into a Burger::TimeDate_t

	\note This function is only available on Playstation Vita
	
	\return \ref FALSE if successful, non-zero if not.

***************************************/

Word Burger::TimeDate_t::Load(const SceDateTime *pSceDateTime)
{
	Clear();
	Word uResult = TRUE;
	SceRtcTick TickTime;
	if (sceRtcGetTick(pSceDateTime,&TickTime)>=SCE_OK) {
		SceRtcTick LocalTick;
		if (sceRtcConvertUtcToLocalTime(&TickTime,&LocalTick)>=SCE_OK) {
			SceDateTime LocalTime;
			if (sceRtcSetTick(&LocalTime,&LocalTick)>=SCE_OK) {
				m_usMilliseconds = static_cast<Word16>(LocalTime.microsecond/1000);
				m_bSecond = static_cast<Word8>(LocalTime.second);		// Get the seconds
				m_bMinute = static_cast<Word8>(LocalTime.minute);		// Get the minute
				m_bHour = static_cast<Word8>(LocalTime.hour);			// Get the hour
				m_bDay = static_cast<Word8>(LocalTime.day);				// Get the day
				m_bDayOfWeek = static_cast<Word8>(sceRtcGetDayOfWeek(LocalTime.year,LocalTime.month,LocalTime.day));	// Weekday
				m_bMonth = static_cast<Word8>(LocalTime.month);			// Get the month
				m_uYear = static_cast<Word16>(LocalTime.year);			// Get the year
				uResult = FALSE;
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Convert a Burger::TimeDate_t into a SceDateTime

	\note This function is only available on Playstation Vita
	
	\return \ref FALSE if successful, non-zero if not.

***************************************/

Word Burger::TimeDate_t::Store(SceDateTime *pSceDateTime) const
{
	Word uResult = TRUE;
	SceDateTime LocalTime;
	LocalTime.year = static_cast<Word16>(m_uYear);
	LocalTime.month = static_cast<Word16>(m_bMonth);
	LocalTime.day = static_cast<Word16>(m_bDay);
	LocalTime.hour = static_cast<Word16>(m_bHour);
	LocalTime.minute = static_cast<Word16>(m_bMinute);
	LocalTime.second = static_cast<Word16>(m_bSecond);
	LocalTime.microsecond = static_cast<Word>(m_usMilliseconds)*1000;
	SceRtcTick LocalTick;
	// Convert to ticks
	if (sceRtcGetTick(&LocalTime,&LocalTick)>=SCE_OK) {
		SceRtcTick TickTime;
		// Convert from local time zone to UTC
		if (sceRtcConvertLocalTimeToUtc(&LocalTick,&TickTime)>=SCE_OK) {
			// Convert to data/time
			if (sceRtcSetTick(pSceDateTime,&TickTime)>=SCE_OK) {
				uResult = FALSE;
			}
		}
	}
	return uResult;
}

#endif