/***************************************

	String handlers for UTF32 support

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRUTF32_H__
#define __BRUTF32_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRCODEPAGE_H__
#include "brcodepage.h"
#endif

/* BEGIN */
namespace Burger {
class UTF32: public CodePage {
public:
	static const uint32_t kEndianMark = 0x0000FEFFU;
#if defined(BURGER_LITTLEENDIAN)
	static const uint32_t kBigEndianMark = 0xFFFE0000U;
	static const uint32_t kLittleEndianMark = 0x0000FEFFU;
#else
	static const uint32_t kBigEndianMark = 0x0000FEFFU;
	static const uint32_t kLittleEndianMark = 0xFFFE0000U;
#endif
	static uint_t BURGER_API IsValid(uint32_t uInput) BURGER_NOEXCEPT;
	static uint_t BURGER_API IsValid(const uint32_t* pInput) BURGER_NOEXCEPT;
	static uint_t BURGER_API IsValid(
		const uint32_t* pInput, uintptr_t uElementCount) BURGER_NOEXCEPT;

	static uint32_t BURGER_API translate_from_UTF8(
		const char* pInput) BURGER_NOEXCEPT;
	static uint32_t BURGER_API translate_from_UTF8(
		const char** ppInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API translate_from_UTF8(uint32_t* pOutput,
		uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API translate_from_UTF8(uint32_t* pOutput,
		uintptr_t uOutputSize, const char* pInput,
		uintptr_t uInputSize) BURGER_NOEXCEPT;
};
}
/* END */

#endif
