/***************************************

    Time Manager Class

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRTIMEDATE_H__
#define __BRTIMEDATE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __BROUTPUTMEMORYSTREAM_H__
#include "broutputmemorystream.h"
#endif

#ifndef __BRINPUTMEMORYSTREAM_H__
#include "brinputmemorystream.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_MAC) && !defined(__BRMACTYPES_H__)
#include "brmactypes.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

#if defined(BURGER_IOS) && !defined(__BRIOSTYPES_H__)
#include "briostypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

#if defined(BURGER_VITA) && !defined(__BRVITATYPES_H__)
#include "brvitatypes.h"
#endif

/* BEGIN */
struct timespec;
namespace Burger {

struct TimeDate_t {
    uint32_t m_uYear;          ///< Year "2009"
    uint16_t m_usMilliseconds; ///< 0-999
    uint8_t m_bMonth;          ///< 1-12
    uint8_t m_bDay;            ///< 1-31
    uint8_t m_bDayOfWeek;      ///< 0-6
    uint8_t m_bHour;           ///< 0-23
    uint8_t m_bMinute;         ///< 0-59
    uint8_t m_bSecond;         ///< 0-59

    void Clear(void) BURGER_NOEXCEPT;
    void GetTime(void) BURGER_NOEXCEPT;
    void TimeToString(char* pOutput) const BURGER_NOEXCEPT;
    void TimeToStringPM(char* pOutput) const BURGER_NOEXCEPT;
    void DateToString(char* pOutput) const BURGER_NOEXCEPT;
    void DateToStringVerbose(char* pOutput) const BURGER_NOEXCEPT;
    eError Read(InputMemoryStream* pInput) BURGER_NOEXCEPT;
    eError Write(OutputMemoryStream* pOutput) const BURGER_NOEXCEPT;
    int Compare(const TimeDate_t* pInput) const BURGER_NOEXCEPT;

    BURGER_INLINE friend uint_t operator==(
        const TimeDate_t& rInput1, const TimeDate_t& rInput2) BURGER_NOEXCEPT
    {
        return rInput1.Compare(&rInput2) == 0;
    }
    BURGER_INLINE friend uint_t operator!=(
        const TimeDate_t& rInput1, const TimeDate_t& rInput2) BURGER_NOEXCEPT
    {
        return rInput1.Compare(&rInput2) != 0;
    }
    BURGER_INLINE friend uint_t operator>(
        const TimeDate_t& rInput1, const TimeDate_t& rInput2) BURGER_NOEXCEPT
    {
        return rInput1.Compare(&rInput2) > 0;
    }
    BURGER_INLINE friend uint_t operator>=(
        const TimeDate_t& rInput1, const TimeDate_t& rInput2) BURGER_NOEXCEPT
    {
        return rInput1.Compare(&rInput2) >= 0;
    }
    BURGER_INLINE friend uint_t operator<(
        const TimeDate_t& rInput1, const TimeDate_t& rInput2) BURGER_NOEXCEPT
    {
        return rInput1.Compare(&rInput2) < 0;
    }
    BURGER_INLINE friend uint_t operator<=(
        const TimeDate_t& rInput1, const TimeDate_t& rInput2) BURGER_NOEXCEPT
    {
        return rInput1.Compare(&rInput2) <= 0;
    }

    eError LoadTimeT(uintptr_t uTimeT) BURGER_NOEXCEPT;
    eError StoreTimeT(uintptr_t* pTimeT) const BURGER_NOEXCEPT;
    eError Load(const timespec* pTimeSpec) BURGER_NOEXCEPT;
    eError Store(timespec* pTimeSpec) const BURGER_NOEXCEPT;

#if defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(DOXYGEN)
    uint_t Load(const _SYSTEMTIME* pSystemTime);
    uint_t Store(_SYSTEMTIME* pSystemTime) const;
    uint_t Load(const _FILETIME* pFileTime);
    uint_t Store(_FILETIME* pFileTime) const;
#endif

#if defined(BURGER_MSDOS) || defined(DOXYGEN)
    void LoadMSDOS(uint32_t uMSDOSTime);
    uint32_t StoreMSDOS(void) const;
#endif

#if defined(BURGER_MACOS) || defined(DOXYGEN)
    uint_t Load(const UTCDateTime* pUTCDateTime);
    uint_t Store(UTCDateTime* pUTCDateTime) const;
    void LoadFileSeconds(uint32_t uTime);
    uint32_t GetFileSeconds(void) const;
#endif

#if defined(BURGER_MACOSX) || defined(BURGER_IOS) || defined(DOXYGEN)
    uint_t Load(double dNSTimeInterval);
    uint_t Store(double* pNSTimeInterval) const;
#endif

#if defined(BURGER_VITA) || defined(DOXYGEN)
    uint_t Load(const SceDateTime* pSceDateTime);
    uint_t Store(SceDateTime* pSceDateTime) const;
#endif
};
}
/* END */

#endif
