/***************************************

	Create and work with Windows style GUID structures

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRGUID_H__
#define __BRGUID_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

/* BEGIN */
#ifdef __cplusplus
extern "C" {
#endif

// If windows is included first, don't declare _GUID
#if !defined(GUID_DEFINED) || defined(DOXYGEN)

#if !defined(DOXYGEN)
// Define used by the windows headers to disable
// generation of the structure below
#define GUID_DEFINED
#endif

typedef struct _GUID {
	/** Initial 32 bits little endian */
	uint32_t Data1;
	/** Next 16 bits little endian */
	uint16_t Data2;
	/** Third value little endian */
	uint16_t Data3;
	/** Remaining 64 bits as 8 bytes */
	uint8_t Data4[8];
} GUID;
#endif

#ifdef __cplusplus
}
#endif

namespace Burger {

extern const GUID g_GUID_zero;

extern void BURGER_API GUID_init(GUID* pOutput) BURGER_NOEXCEPT;
extern void BURGER_API GUID_to_string(
	char* pOutput, const GUID* pInput) BURGER_NOEXCEPT;
extern eError BURGER_API GUID_from_string(
	GUID* pOutput, const char* pInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API GUID_hash(const GUID* pInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API GUID_is_equal(
	const GUID* pInput1, const GUID* pInput2) BURGER_NOEXCEPT;
extern int BURGER_API GUID_compare(
	const GUID* pInput1, const GUID* pInput2) BURGER_NOEXCEPT;
}

/* END */

#endif
