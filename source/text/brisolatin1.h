/***************************************

	String handlers for ISOLatin1 support

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRISOLATIN1_H__
#define __BRISOLATIN1_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRCODEPAGE_H__
#include "brcodepage.h"
#endif

/* BEGIN */
namespace Burger {
class ISOLatin1: public CodePage {
public:
	BURGER_ALIGN(static const uint8_t, g_UpperCaseTable[256], 16);
	BURGER_ALIGN(static const uint8_t, g_LowerCaseTable[256], 16);
	BURGER_ALIGN(static const uint8_t, g_ToUTF8Table[128][4], 16);
	BURGER_ALIGN(static const uint16_t, g_ToUTF16Table[128], 16);
	static uint32_t BURGER_API translate_from_UTF8(
		const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API translate_from_UTF8(char* pOutput,
		uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API translate_from_UTF8(char* pOutput,
		uintptr_t uOutputSize, const char* pInput,
		uintptr_t uInputSize) BURGER_NOEXCEPT;
};
}
/* END */

#endif
