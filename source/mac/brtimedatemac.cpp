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

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include <DateTimeUtils.h>
#include <UTCUtils.h>

/***************************************

    \brief Obtain the current localized time.

    A query is made to the target platform and the structure is filled in with
    the current date and time.

    \return The structure is set to the current localized time.

***************************************/

void Burger::TimeDate_t::GetTime(void) BURGER_NOEXCEPT
{
    DateTimeRec MyDate; // MacOS time structure
    ::GetTime(&MyDate); // Call MacOS

    m_uYear = MyDate.year;
    m_usMilliseconds = 0;
    m_bMonth = static_cast<uint8_t>(MyDate.month);
    m_bDay = static_cast<uint8_t>(MyDate.day);
    m_bDayOfWeek = static_cast<uint8_t>(MyDate.dayOfWeek -
        1); // Start the day at zero, since the mac returns 1-7
    m_bHour = static_cast<uint8_t>(MyDate.hour);
    m_bMinute = static_cast<uint8_t>(MyDate.minute);
    m_bSecond = static_cast<uint8_t>(MyDate.second);
}

/*! ************************************

    \brief Convert a MacOS UTCDateTime into a \ref TimeDate_t

    \note This function is only available on MacOS and MacOSX

    \param pUTCDateTime Pointer to a UTCDateTime to convert to a \ref TimeDate_t
    \return \ref FALSE if successful, non-zero if not.

***************************************/

uint_t Burger::TimeDate_t::Load(const UTCDateTime* pUTCDateTime) BURGER_NOEXCEPT
{
    Clear();
    uint_t uResult = TRUE;

#if !defined(BURGER_68K)
    // Convert to my time zone

    LocalDateTime TempTime;
    if (!ConvertUTCToLocalDateTime(pUTCDateTime, &TempTime)) {

        LongDateRec LongTimeRec;
        LongDateCvt Convert;

        Convert.hl.lHigh = TempTime.highSeconds;
        Convert.hl.lLow = TempTime.lowSeconds;
        LongSecondsToDate(&Convert.c, &LongTimeRec);

        m_uYear = static_cast<uint32_t>(LongTimeRec.od.oldDate.year);
        m_usMilliseconds = static_cast<uint16_t>(
            (static_cast<uint_t>(TempTime.fraction) * 1000U) >> 16U);
        m_bMonth = static_cast<uint8_t>(LongTimeRec.od.oldDate.month);
        m_bDay = static_cast<uint8_t>(LongTimeRec.od.oldDate.day);
        m_bHour = static_cast<uint8_t>(LongTimeRec.od.oldDate.hour);
        m_bMinute = static_cast<uint8_t>(LongTimeRec.od.oldDate.minute);
        m_bSecond = static_cast<uint8_t>(LongTimeRec.od.oldDate.second);
        m_bDayOfWeek = static_cast<uint8_t>(LongTimeRec.od.oldDate.dayOfWeek);
        uResult = FALSE;
    }
#endif
    return uResult;
}

/*! ************************************

    \brief Convert a \ref TimeDate_t into a MacOS UTCDateTime

    \note This function is only available on MacOS and MacOSX

    \param pUTCDateTime Pointer to a UTCDateTime structure to fill in with the
        time/date

    \return \ref FALSE if successful, non-zero if not.

***************************************/

uint_t Burger::TimeDate_t::Store(UTCDateTime* pUTCDateTime) const BURGER_NOEXCEPT
{
    uint_t uResult = TRUE;

    LongDateRec LongTimeRec;
    LongDateCvt Convert;

    LongTimeRec.ld.era = 0; // AD
    LongTimeRec.ld.year = static_cast<short>(m_uYear);
    LongTimeRec.ld.month = static_cast<short>(m_bMonth);
    LongTimeRec.ld.day = static_cast<short>(m_bDay);
    LongTimeRec.ld.hour = static_cast<short>(m_bHour);
    LongTimeRec.ld.minute = static_cast<short>(m_bMinute);
    LongTimeRec.ld.second = static_cast<short>(m_bSecond);

    // Clear the rest
    LongTimeRec.ld.dayOfWeek = 0;
    LongTimeRec.ld.dayOfYear = 0;
    LongTimeRec.ld.weekOfYear = 0;
    LongTimeRec.ld.pm = 0;
    LongTimeRec.ld.res1 = 0;
    LongTimeRec.ld.res2 = 0;
    LongTimeRec.ld.res3 = 0;

    LongDateToSeconds(&LongTimeRec, &Convert.c);

    LocalDateTime TempTime;
    TempTime.highSeconds = static_cast<UInt16>(Convert.hl.lHigh);
    TempTime.lowSeconds = Convert.hl.lLow;
    TempTime.fraction = static_cast<UInt16>((m_usMilliseconds << 16) / 1000U);
    if (!ConvertLocalToUTCDateTime(&TempTime, pUTCDateTime)) {
        uResult = FALSE;
    }
    return uResult;
}

/*! ************************************

    \brief Convert a Mac HFS time into a \ref TimeDate_t

    Convert the timestamp value from the MacOS file system to a Burgerlib
    \ref TimeDate_t structure.

    \note This function is only available on MacOS

    \param uTime Time in seconds from midnight Jan 1st, 1904

    \sa GetFileSeconds(void) const or Load(const UTCDateTime *)

***************************************/

void Burger::TimeDate_t::LoadFileSeconds(uint32_t uTime) BURGER_NOEXCEPT
{
    DateTimeRec Temp2;
    SecondsToDate(uTime, &Temp2);              // Convert to date
    m_uYear = static_cast<uint32_t>(Temp2.year); // Get the year
    m_usMilliseconds = 0;                      // MacOS doesn't use milliseconds
    m_bSecond = static_cast<uint8_t>(Temp2.second);           // Get the seconds
    m_bMinute = static_cast<uint8_t>(Temp2.minute);           // Get the minute
    m_bHour = static_cast<uint8_t>(Temp2.hour);               // Get the hour
    m_bDay = static_cast<uint8_t>(Temp2.day);                 // Get the day
    m_bMonth = static_cast<uint8_t>(Temp2.month);             // Get the month
    m_bDayOfWeek = static_cast<uint8_t>(Temp2.dayOfWeek - 1); // Day of the week
}

/*! ************************************

    \brief Convert a \ref TimeDate_t into a Mac HFS time

    Convert the timestamp value to the MacOS file system from a Burgerlib
    \ref TimeDate_t structure.

    \note This function is only available on MacOS

    \return Time in seconds from midnight Jan 1st, 1904

    \sa LoadFileSeconds(uint32_t) or Store(UTCDateTime *)

***************************************/

uint32_t Burger::TimeDate_t::GetFileSeconds(void) const BURGER_NOEXCEPT
{
    DateTimeRec Temp2;
    Temp2.year = static_cast<short>(m_uYear);     // Get the year
    Temp2.month = static_cast<short>(m_bMonth);   // Get the month
    Temp2.day = static_cast<short>(m_bDay);       // Get the day
    Temp2.hour = static_cast<short>(m_bHour);     // Get the hour
    Temp2.minute = static_cast<short>(m_bMinute); // Get the minute
    Temp2.second = static_cast<short>(m_bSecond); // Get the seconds
    Temp2.dayOfWeek = 0;                          // Day of the week

    unsigned long uResult = 0;
    DateToSeconds(&Temp2, &uResult); // Convert to seconds
    return uResult;
}

#endif
