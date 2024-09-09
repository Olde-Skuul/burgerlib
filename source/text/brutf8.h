/***************************************

	String handlers for UTF8 support

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
	static const uint_t kInvalid = UINT32_MAX;
	BURGER_ALIGN(static const uint8_t, TokenSizeTable[256], 16);
	static const uint8_t ByteOrderMark[3];

	static uint_t BURGER_API is_valid_single(const char* pInput) BURGER_NOEXCEPT;
	static uint_t BURGER_API is_valid(const char* pInput) BURGER_NOEXCEPT;
	static uint_t BURGER_API is_valid(
		const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetTokenSize(
		const char* pInput) BURGER_NOEXCEPT;
	static const char* BURGER_API NextToken(const char* pInput) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API GetMacRomanUSSize(
		uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetMacRomanUSSize(
		const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetMacRomanUSSize(
		const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API FromMacRomanUS(
		char* pOutput, uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromMacRomanUS(char* pOutput,
		uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromMacRomanUS(char* pOutput,
		uintptr_t uOutputSize, const char* pInput,
		uintptr_t uInputSize) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API GetWin1252Size(uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetWin1252Size(
		const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetWin1252Size(
		const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API FromWin1252(
		char* pOutput, uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromWin1252(char* pOutput,
		uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromWin1252(char* pOutput,
		uintptr_t uOutputSize, const char* pInput,
		uintptr_t uInputSize) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API GetWin437Size(uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetWin437Size(
		const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetWin437Size(
		const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API FromWin437(
		char* pOutput, uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromWin437(char* pOutput, uintptr_t uOutputSize,
		const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromWin437(char* pOutput, uintptr_t uOutputSize,
		const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API GetISOLatin1Size(uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetISOLatin1Size(
		const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetISOLatin1Size(
		const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API FromISOLatin1(
		char* pOutput, uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromISOLatin1(char* pOutput,
		uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromISOLatin1(char* pOutput,
		uintptr_t uOutputSize, const char* pInput,
		uintptr_t uInputSize) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API GetGenericSize(
		const uint8_t pTranslateTable[128][4], uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetGenericSize(
		const uint8_t pTranslateTable[128][4],
		const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetGenericSize(
		const uint8_t pTranslateTable[128][4], const char* pInput,
		uintptr_t uInputSize) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API FromGeneric(char* pOutput,
		const uint8_t pTranslateTable[128][4], uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromGeneric(char* pOutput,
		uintptr_t uOutputSize, const uint8_t pTranslateTable[128][4],
		const char* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API FromGeneric(char* pOutput,
		uintptr_t uOutputSize, const uint8_t pTranslateTable[128][4],
		const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API GetUTF16Size(uint_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetUTF16Size(
		const uint16_t* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetUTF16Size(
		const uint16_t* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API from_UTF16(
		char* pOutput, uint16_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API from_UTF16(char* pOutput, uintptr_t uOutputSize,
		const uint16_t* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API from_UTF16(char* pOutput, uintptr_t uOutputSize,
		const uint16_t* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;
	static char* BURGER_API from_UTF16(const uint16_t* pInput) BURGER_NOEXCEPT;

	static uintptr_t BURGER_API from_UTF32(
		char* pOutput, uint32_t uInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API from_UTF32(char* pOutput, uintptr_t uOutputSize,
		const uint32_t* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API from_UTF32(char* pOutput, uintptr_t uOutputSize,
		const uint32_t* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;
	static char* BURGER_API from_UTF32(const uint32_t* pInput) BURGER_NOEXCEPT;

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
