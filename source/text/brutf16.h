/***************************************

	String handlers for UTF16 support

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRUTF16_H__
#define __BRUTF16_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
class UTF16 {
public:
	static const uint16_t kInvalid = UINT16_MAX;
	static const uint16_t kEndianMark = 0xFEFFU;
#if defined(BURGER_LITTLEENDIAN)
	static const uint16_t kBigEndianMark = 0xFFFEU;
	static const uint16_t kLittleEndianMark = 0xFEFFU;
#else
	static const uint16_t kBigEndianMark = 0xFEFFU;
	static const uint16_t kLittleEndianMark = 0xFFFEU;
#endif

	static uint_t BURGER_API IsValid(uint16_t uInput) BURGER_NOEXCEPT;
	static uint_t BURGER_API IsValid(const uint16_t* pInput) BURGER_NOEXCEPT;
	static uint_t BURGER_API IsValid(
		const uint16_t* pInput, uintptr_t uElementCount) BURGER_NOEXCEPT;
	static uint16_t BURGER_API translate_from_UTF8(
		const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API translate_from_UTF8(uint16_t* pOutput,
		uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API translate_from_UTF8(uint16_t* pOutput,
		uintptr_t uOutputSize, const char* pInput,
		uintptr_t uInputSize) BURGER_NOEXCEPT;
};
}
/* END */

#endif
