/***************************************

	Time Manager Class
	MSDOS specific code

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtimedate.h"
#if defined(BURGER_MSDOS) || defined(DOXYGEN)

/*! ************************************

	\brief Convert an MSDOS FILETIME into a Burger::TimeDate_t

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms724247(v=vs.85).aspx

	\msdosonly

	\param uMSDOSTime Time/date from an MSDOS file

***************************************/

void Burger::TimeDate_t::LoadMSDOS(uint32_t uMSDOSTime) BURGER_NOEXCEPT
{
	// Get the seconds
	m_bSecond = static_cast<uint8_t>((uMSDOSTime & 0x1F) << 1);
	uMSDOSTime = uMSDOSTime >> 5;

	// Get the minute
	m_bMinute = static_cast<uint8_t>(uMSDOSTime & 0x3F);
	uMSDOSTime = uMSDOSTime >> 6;

	// Get the hour
	m_bHour = static_cast<uint8_t>(uMSDOSTime & 0x1F);
	uMSDOSTime = uMSDOSTime >> 5;

	// Get the day
	m_bDay = static_cast<uint8_t>(uMSDOSTime & 0x1F);
	uMSDOSTime = uMSDOSTime >> 5;

	// Get the month
	m_bMonth = static_cast<uint8_t>(uMSDOSTime & 0xF);
	uMSDOSTime = uMSDOSTime >> 4;

	// Get the year
	m_uYear = static_cast<uint16_t>(uMSDOSTime + 1980);
	m_usMilliseconds = 0;
	m_bDayOfWeek = static_cast<uint8_t>(CalcDayOfWeek());
}

/*! ************************************

	\brief Convert a Burger::TimeDate_t into an MSDOS FILETIME

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms724247(v=vs.85).aspx

	\msdosonly

	\return MSDOS FILETIME

***************************************/

uint32_t Burger::TimeDate_t::StoreMSDOS(void) const BURGER_NOEXCEPT
{
	// Seconds are divided by 2
	uint32_t uMSDOSTime = m_bSecond >> 1;
	uMSDOSTime |= m_bMinute << 5;
	uMSDOSTime |= m_bHour << 11;
	uMSDOSTime |= m_bDay << 16;
	uMSDOSTime |= m_bMonth << 21;

	// Normalize the year to 1980
	uMSDOSTime |= (m_uYear - 1980) << 25;
	return uMSDOSTime;
}

#endif
