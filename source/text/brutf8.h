/***************************************

	String handlers for UTF8 support

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRUTF8_H__
#define __BRUTF8_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
class UTF8 {
public:
	static const uint_t kInvalid = BURGER_MAXUINT;
	BURGER_ALIGN(static const uint8_t, TokenSizeTable[256], 16);
	static const uint8_t ByteOrderMark[3];
	static uint_t BURGER_API IsValidSingle(const char* pInput) BURGER_NOEXCEPT;
	static uint_t BURGER_API IsValid(const char* pInput) BURGER_NOEXCEPT;
	static uint_t BURGER_API IsValid(
		const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetTokenSize(
		const char* pInput) BURGER_NOEXCEPT;
	static const char* BURGER_API NextToken(const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromMacRomanUS(
		char* pOutput, uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromMacRomanUS(char* pOutput,
		uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromMacRomanUS(char* pOutput,
		uintptr_t uOutputSize, const char* pInput,
		uintptr_t uInputSize) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API FromWin1252(
		char* pOutput, uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromWin1252(char* pOutput,
		uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromWin1252(char* pOutput,
		uintptr_t uOutputSize, const char* pInput,
		uintptr_t uInputSize) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API FromWin437(
		char* pOutput, uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromWin437(char* pOutput, uintptr_t uOutputSize,
		const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromWin437(char* pOutput, uintptr_t uOutputSize,
		const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API FromISOLatin1(
		char* pOutput, uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromISOLatin1(char* pOutput,
		uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromISOLatin1(char* pOutput,
		uintptr_t uOutputSize, const char* pInput,
		uintptr_t uInputSize) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromGeneric(char* pOutput,
		const uint8_t pTranslateTable[128][4], uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromGeneric(char* pOutput,
		uintptr_t uOutputSize, const uint8_t pTranslateTable[128][4],
		const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromGeneric(char* pOutput,
		uintptr_t uOutputSize, const uint8_t pTranslateTable[128][4],
		const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromUTF16(
		char* pOutput, uint16_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromUTF16(char* pOutput, uintptr_t uOutputSize,
		const uint16_t* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromUTF16(char* pOutput, uintptr_t uOutputSize,
		const uint16_t* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;
	static char* BURGER_API FromUTF16(const uint16_t* pInput) BURGER_NOEXCEPT;

	static uint_t BURGER_API ToGeneric(const char* pInput,
		const uint8_t pTranslateTable[128][4]) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API ToGeneric(char* pOutput, uintptr_t uOutputSize,
		const uint8_t pTranslateTable[128][4],
		const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API ToGeneric(char* pOutput, uintptr_t uOutputSize,
		const uint8_t pTranslateTable[128][4], const char* pInput,
		uintptr_t uInputSize) BURGER_NOEXCEPT;
};
}
/* END */

#endif
