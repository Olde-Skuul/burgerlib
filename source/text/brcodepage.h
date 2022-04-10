/***************************************

	Common code for code pages

	Copyright (c) 2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
class CodePage {
public:
	static const uint_t kInvalid = BURGER_MAXUINT;

	static uint_t BURGER_API TranslateFromUTF8(const char* pInput,
		const uint8_t pTranslateTable[128][4]) BURGER_NOEXCEPT;
	static uint_t BURGER_API TranslateFromUTF8(char* pOutput,
		uintptr_t uOutputSize, const char* pInput,
		const uint8_t pTranslateTable[128][4]) BURGER_NOEXCEPT;
	static uint_t BURGER_API TranslateFromUTF8(char* pOutput,
		uintptr_t uOutputSize, const char* pInput, uintptr_t uInputSize,
		const uint8_t pTranslateTable[128][4]) BURGER_NOEXCEPT;
};
}
/* END */

#endif
