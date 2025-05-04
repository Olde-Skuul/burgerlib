/***************************************

	Common code for code pages

	Copyright (c) 2021-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCODEPAGE_H__
#define __BRCODEPAGE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

struct CodePage {
	static const uint32_t kInvalid = UINT32_MAX;
};

extern uint32_t BURGER_API translate_from_UTF8(
	const char* pInput, const uint8_t pTranslateTable[128][4]) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API translate_from_UTF8(char* pOutput,
	uintptr_t uOutputSize, const char* pInput,
	const uint8_t pTranslateTable[128][4]) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API translate_from_UTF8(char* pOutput,
	uintptr_t uOutputSize, const char* pInput, uintptr_t uInputSize,
	const uint8_t pTranslateTable[128][4]) BURGER_NOEXCEPT;
}
/* END */

#endif
