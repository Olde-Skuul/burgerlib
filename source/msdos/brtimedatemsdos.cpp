/***************************************

	Time Manager Class
	MSDOS specific code

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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

void Burger::TimeDate_t::LoadMSDOS(Word32 uMSDOSTime)
{
	m_bSecond = static_cast<Word8>((uMSDOSTime&0x1F)<<1);	// Get the seconds
	uMSDOSTime=uMSDOSTime>>5;
	m_bMinute = static_cast<Word8>(uMSDOSTime&0x3F);		// Get the minute
	uMSDOSTime=uMSDOSTime>>6;
	m_bHour = static_cast<Word8>(uMSDOSTime&0x1F);			// Get the hour
	uMSDOSTime=uMSDOSTime>>5;
	m_bDay = static_cast<Word8>(uMSDOSTime&0x1F);			// Get the day
	uMSDOSTime=uMSDOSTime>>5;
	m_bMonth = static_cast<Word8>(uMSDOSTime&0xF);			// Get the month
	uMSDOSTime=uMSDOSTime>>4;
	m_uYear = static_cast<Word16>(uMSDOSTime+1980);			// Get the year
	m_usMilliseconds = 0;
	m_bDayOfWeek = 0;
}

/*! ************************************

	\brief Convert a Burger::TimeDate_t into an MSDOS FILETIME
	
    http://msdn.microsoft.com/en-us/library/windows/desktop/ms724247(v=vs.85).aspx

	\msdosonly

	\return MSDOS FILETIME

***************************************/

Word32 Burger::TimeDate_t::StoreMSDOS(void) const
{
    Word32 uMSDOSTime = m_bSecond>>1;       // Seconds are divided by 2
    uMSDOSTime |= m_bMinute<<5;
    uMSDOSTime |= m_bHour<<11;
    uMSDOSTime |= m_bDay<<16;
    uMSDOSTime |= m_bMonth<<21;
    uMSDOSTime |= (m_uYear-1980)<<25;       // Normalize the year to 1980
	return uMSDOSTime;
}

#endif