/***************************************

	Time Manager Class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRTIMEDATE_H__
#define __BRTIMEDATE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
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

/* BEGIN */
struct timespec;
namespace Burger {
struct TimeDate_t {
	Word32 m_uYear;				///< Year "2009"
	Word16 m_usMilliseconds;	///< 0-999
	Word8 m_bMonth;				///< 1-12
	Word8 m_bDay;				///< 1-31
	Word8 m_bDayOfWeek;			///< 0-6
	Word8 m_bHour;				///< 0-23
	Word8 m_bMinute;			///< 0-59
	Word8 m_bSecond;			///< 0-59
	void Clear(void);
	void GetTime(void);
	void TimeToString(char *pOutput) const;
	void TimeToStringPM(char *pOutput) const;
	void DateToString(char *pOutput) const;
	void DateToStringVerbose(char *pOutput) const;
	Word Read(Burger::InputMemoryStream *pInput);
	Word Write(Burger::OutputMemoryStream *pOutput) const;
	int Compare(const TimeDate_t *pInput) const;
	BURGER_INLINE friend Word operator == (const TimeDate_t &rInput1,const TimeDate_t &rInput2) { return rInput1.Compare(&rInput2)==0; }
	BURGER_INLINE friend Word operator != (const TimeDate_t &rInput1,const TimeDate_t &rInput2) { return rInput1.Compare(&rInput2)!=0; }
	BURGER_INLINE friend Word operator > (const TimeDate_t &rInput1,const TimeDate_t &rInput2) { return rInput1.Compare(&rInput2)>0; }
	BURGER_INLINE friend Word operator >= (const TimeDate_t &rInput1,const TimeDate_t &rInput2) { return rInput1.Compare(&rInput2)>=0; }
	BURGER_INLINE friend Word operator < (const TimeDate_t &rInput1,const TimeDate_t &rInput2) { return rInput1.Compare(&rInput2)<0; }
	BURGER_INLINE friend Word operator <= (const TimeDate_t &rInput1,const TimeDate_t &rInput2) { return rInput1.Compare(&rInput2)<=0; }
	Word LoadTimeT(WordPtr uTimeT);
	Word StoreTimeT(WordPtr *pTimeT) const;
	Word Load(const timespec *pTimeSpec);
	Word Store(timespec *pTimeSpec) const;
#if defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(DOXYGEN)
	Word Load(const _FILETIME *pFileTime);
	Word Store(_FILETIME *pFileTime) const;
#endif
#if defined(BURGER_MSDOS) || defined(DOXYGEN)
	void LoadMSDOS(Word32 uMSDOSTime);
	Word32 StoreMSDOS(void) const;
#endif
#if defined(BURGER_MACOS) || defined(DOXYGEN)
	Word Load(const UTCDateTime *pUTCDateTime);
	Word Store(UTCDateTime *pUTCDateTime) const;
#endif
#if defined(BURGER_MACOSX) || defined(BURGER_IOS) || defined(DOXYGEN)
	Word Load(double dNSTimeInterval);
	Word Store(double *pNSTimeInterval) const;
#endif
};
}
/* END */

#endif
