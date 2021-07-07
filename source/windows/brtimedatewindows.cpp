/***************************************

    Time Manager Class
    Windows specific code

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brtimedate.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#if !defined(WIN32_LEAN_AND_MEAN) && !defined(DOXYGEN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

/***************************************

    \brief Obtain the current localized time.

    A query is made to the target platform and the structure is filled in with
    the current date and time.

    \return The structure is set to the current localized time.

***************************************/

void Burger::TimeDate_t::GetTime(void) BURGER_NOEXCEPT
{
    SYSTEMTIME MySystemTime;       // Windows time structure
    ::GetLocalTime(&MySystemTime); // Call windows

    m_uYear = MySystemTime.wYear; // Windows has 16 bit values
    m_usMilliseconds = MySystemTime.wMilliseconds;
    m_bMonth = static_cast<uint8_t>(MySystemTime.wMonth);
    m_bDay = static_cast<uint8_t>(MySystemTime.wDay);
    m_bDayOfWeek = static_cast<uint8_t>(MySystemTime.wDayOfWeek);
    m_bHour = static_cast<uint8_t>(MySystemTime.wHour);
    m_bMinute = static_cast<uint8_t>(MySystemTime.wMinute);
    m_bSecond = static_cast<uint8_t>(MySystemTime.wSecond);
}

/*! ************************************

    \brief Convert a Windows FILETIME into a Burger::TimeDate_t

    \note This function is only available on the Xbox 360 and Windows

    \return \ref FALSE if successful, non-zero if not.

***************************************/

Burger::eError Burger::TimeDate_t::Load(const _FILETIME* pFileTime)
{
    Clear();
    FILETIME Local;
    eError uResult = kErrorReadFailure;
    if (FileTimeToLocalFileTime(pFileTime, &Local)) { // Convert to local time
        SYSTEMTIME Temp2;
        if (FileTimeToSystemTime(
                &Local, &Temp2)) { // Convert the time to sections
            m_usMilliseconds = static_cast<uint16_t>(Temp2.wMilliseconds);
            m_bSecond = static_cast<uint8_t>(Temp2.wSecond); // Get the seconds
            m_bMinute = static_cast<uint8_t>(Temp2.wMinute); // Get the minute
            m_bHour = static_cast<uint8_t>(Temp2.wHour);     // Get the hour
            m_bDay = static_cast<uint8_t>(Temp2.wDay);       // Get the day
            m_bDayOfWeek = static_cast<uint8_t>(Temp2.wDayOfWeek); // Weekday
            m_bMonth = static_cast<uint8_t>(Temp2.wMonth); // Get the month
            m_uYear = static_cast<uint16_t>(Temp2.wYear);  // Get the year
            uResult = kErrorNone;
        }
    }
    return uResult;
}

/*! ************************************

    \brief Convert a Burger::TimeDate_t into a Windows FILETIME

    \note This function is only available on the Xbox 360 and Windows

    \return \ref FALSE if successful, non-zero if not.

***************************************/

Burger::eError Burger::TimeDate_t::Store(_FILETIME* pFileTime) const
{
    eError uResult = kErrorWriteFailure;
    SYSTEMTIME Temp2;
    Temp2.wMilliseconds = m_usMilliseconds;
    Temp2.wSecond = m_bSecond;                // Get the seconds
    Temp2.wMinute = m_bMinute;                // Get the minute
    Temp2.wHour = m_bHour;                    // Get the hour
    Temp2.wDay = m_bDay;                      // Get the day
    Temp2.wDayOfWeek = m_bDayOfWeek;          // Weekday
    Temp2.wMonth = m_bMonth;                  // Get the month
    Temp2.wYear = static_cast<WORD>(m_uYear); // Get the year
    FILETIME Local;
    if (SystemTimeToFileTime(
            &Temp2, &Local)) { // Convert from sections to timestamp
        if (LocalFileTimeToFileTime(&Local, pFileTime)) { // Convert to GMT
            uResult = kErrorNone;
        }
    }
    return uResult;
}

#endif
