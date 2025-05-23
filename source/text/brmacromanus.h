/***************************************

	String handlers for MacRomanUS support

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMACROMANUS_H__
#define __BRMACROMANUS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRCODEPAGE_H__
#include "brcodepage.h"
#endif

/* BEGIN */
namespace Burger {
class MacRomanUS: public CodePage {
public:
	BURGER_ALIGN(static const uint8_t, g_UpperCase[256], 16);
	BURGER_ALIGN(static const uint8_t, g_LowerCase[256], 16);
	BURGER_ALIGN(static const uint8_t, g_ToUTF8[128][4], 16);
	BURGER_ALIGN(static const uint16_t, g_ToUTF16[128], 16);
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
