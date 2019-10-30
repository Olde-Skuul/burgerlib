/***************************************

    Helper functions for printf and the like

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSTDOUTHELPERS_H__
#define __BRSTDOUTHELPERS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern void BURGER_API PrintHexDigit(uint_t uInput);
extern void BURGER_API PrintHex(uint8_t uInput);
extern void BURGER_API PrintHex(uint16_t uInput);
extern void BURGER_API PrintHex(uint32_t uInput);
extern void BURGER_API PrintHex(uint64_t uInput);

// clang-format off
BURGER_INLINE void PrintHex(char iInput) { PrintHex(static_cast<uint8_t>(iInput)); }
BURGER_INLINE void PrintHex(signed char iInput) { PrintHex(static_cast<uint8_t>(iInput)); }
BURGER_INLINE void PrintHex(short iInput) { PrintHex(static_cast<uint16_t>(iInput)); }
BURGER_INLINE void PrintHex(int iInput) { PrintHex(static_cast<uint2uint_t>(iInput)); }
BURGER_INLINE void PrintHex(long iInput) { PrintHex(static_cast<ulong2uint_t>(iInput)); }
BURGER_INLINE void PrintHex(long long iInput) { PrintHex(static_cast<uint64_t>(iInput)); }

#if defined(BURGER_HAS_WCHAR_T) || defined(DOXYGEN)
BURGER_INLINE void PrintHex(wchar_t uInput) { PrintHex(static_cast<uint16_t>(uInput)); }
#endif

#if defined(BURGER_INT_NOT_IN_STDINT) || defined(DOXYGEN)
BURGER_INLINE void PrintHex(unsigned int uInput) { PrintHex(static_cast<uint2uint_t>(uInput)); }
#endif

#if defined(BURGER_LONG_NOT_IN_STDINT) || defined(DOXYGEN)
BURGER_INLINE void PrintHex(unsigned long uInput) { PrintHex(static_cast<ulong2uint_t> (uInput)); }
#endif

// clang-format on

extern void BURGER_API PrintHex(float fInput);
extern void BURGER_API PrintHex(double dInput);
extern void BURGER_API ClearConsole(void);
}
/* END */

#endif
