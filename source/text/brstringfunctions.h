/***************************************

	Stand alone string functions

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSTRINGFUNCTIONS_H__
#define __BRSTRINGFUNCTIONS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRALGORITHM_H__
#include "bralgorithm.h"
#endif

/* BEGIN */
namespace Burger {

BURGER_ENUM_TYPE(eNumericConversionFlags, uint_t) {
	/** Pass this flag to disable storing a terminating zero. */
	kDisableEndingNull = 0x4000,
	/** Pass this flag to enable leading ASCII zeros output. */
	kEnableLeadingZeros = 0x8000
};

BURGER_ENUM_TYPE(eASCIITypeFlag, uint8_t) {
	/** Lower case character */
	ASCII_LOWER = 0x01,
	/** Upper case character */
		ASCII_UPPER = 0x02,
	/** Control character */
		ASCII_CONTROL = 0x04,
	/** Space character (Tab/CR/LF/FF) */
		ASCII_SPACE = 0x08,
	/** Whitespace (Space/Tab only) */
		ASCII_WHITESPACE = 0x10,
	/** Punctuation character */
		ASCII_PUNCTUATION = 0x20,
	/** Base 16 numeric character */
		kASCIITypeFlagHex = 0x40,
	/** Base 10 numeric character */
		ASCII_DIGIT = 0x80
};

struct FourCharacterCode_t {
	union {
		/** Array of 4 characters for the code */
		char c[4];
		/** Value in binary */
		uint32_t w;
	};
	BURGER_INLINE operator const char*() const BURGER_NOEXCEPT
	{
		return c;
	}
	BURGER_INLINE operator char*() BURGER_NOEXCEPT
	{
		return c;
	}
	BURGER_INLINE operator uint32_t() const BURGER_NOEXCEPT
	{
		return w;
	}
	BURGER_INLINE FourCharacterCode_t& operator=(
		uint32_t uInput) BURGER_NOEXCEPT
	{
		w = uInput;
		return *this;
	}
	BURGER_INLINE uint32_t GetCode(void) const BURGER_NOEXCEPT
	{
		return w;
	}
	BURGER_INLINE void SetCode(uint32_t uInput) BURGER_NOEXCEPT
	{
		w = uInput;
	}
};

extern const char g_EmptyString[1];
extern const char g_CRLFString[3];
extern const char g_CRString[2];
extern const char g_LFString[2];
extern const char g_TokenDelimiters[5];
extern const char g_TrueString[5];
extern const char g_FalseString[6];
extern const char g_YesString[4];
extern const char g_NoString[3];
extern const char g_NullString[7];
extern const char* g_WeekDays[7];
extern const char* g_Months[12];
extern const char* g_pBoolStrings[2];
extern const uint16_t g_EmptyString16[1];
extern const uint16_t g_CRLFString16[3];
extern const uint16_t g_CRString16[2];
extern const uint16_t g_LFString16[2];
extern const uint16_t g_TokenDelimiters16[5];
extern const uint16_t g_TrueString16[5];
extern const uint16_t g_FalseString16[6];
extern const uint16_t g_YesString16[4];
extern const uint16_t g_NoString16[3];
extern const uint32_t g_EmptyString32[1];
extern const uint32_t g_CRLFString32[3];
extern const uint32_t g_CRString32[2];
extern const uint32_t g_LFString32[2];
extern const uint32_t g_TokenDelimiters32[5];
extern const uint32_t g_TrueString32[5];
extern const uint32_t g_FalseString32[6];
extern const uint32_t g_YesString32[4];
extern const uint32_t g_NoString32[3];

BURGER_ALIGN(extern const uint8_t, g_AsciiTestTable[256], 16);
BURGER_ALIGN(extern const char, g_NibbleToAsciiUppercase[16], 16);
BURGER_ALIGN(extern const char, g_NibbleToAsciiLowercase[16], 16);
BURGER_ALIGN(extern const uint8_t, g_AsciiToWord8Table[256], 16);
BURGER_ALIGN(extern const uint8_t, g_reverse_bits[256], 16);

extern const uint32_t g_TensTable32[10];
extern const uint64_t g_TensTable64[20];

BURGER_INLINE BURGER_CONSTEXPR uint32_t ToLower(uint32_t uInput) BURGER_NOEXCEPT
{
	return (static_cast<uint32_t>(uInput - 'A') < 26U) ? uInput + 32 : uInput;
}
BURGER_INLINE BURGER_CONSTEXPR uint32_t ToUpper(uint32_t uInput) BURGER_NOEXCEPT
{
	return (static_cast<uint32_t>(uInput - 'a') < 26U) ? uInput - 32 : uInput;
}

#if defined(BURGER_WINDOWS)
BURGER_INLINE uint_t IsPointerInvalid(const void* pInput) BURGER_NOEXCEPT
{
	return (reinterpret_cast<uintptr_t>(pInput) < 0x10000U);
}
BURGER_INLINE uint_t IsPointerValid(const void* pInput) BURGER_NOEXCEPT
{
	return (reinterpret_cast<uintptr_t>(pInput) >= 0x10000U);
}
#else
BURGER_INLINE uint_t IsPointerInvalid(const void* pInput) BURGER_NOEXCEPT
{
	return (pInput == nullptr);
}
BURGER_INLINE uint_t IsPointerValid(const void* pInput) BURGER_NOEXCEPT
{
	return (pInput != nullptr);
}
#endif

BURGER_INLINE uint_t IsStringEmpty(const char* pInput) BURGER_NOEXCEPT
{
	return (IsPointerInvalid(pInput) || (!pInput[0]));
}
BURGER_INLINE uint_t IsStringEmpty(const uint16_t* pInput) BURGER_NOEXCEPT
{
	return (IsPointerInvalid(pInput) || (!pInput[0]));
}
BURGER_INLINE uint_t IsDigit(char iInput) BURGER_NOEXCEPT
{
	return static_cast<uint_t>(
		g_AsciiTestTable[static_cast<uint8_t>(iInput)] & ASCII_DIGIT);
}
BURGER_INLINE uint_t is_hex(char iInput) BURGER_NOEXCEPT
{
	return static_cast<uint_t>(
		g_AsciiTestTable[static_cast<uint8_t>(iInput)] & kASCIITypeFlagHex);
}
BURGER_INLINE uint_t IsWhitespace(char iInput) BURGER_NOEXCEPT
{
	return static_cast<uint_t>(
		g_AsciiTestTable[static_cast<uint8_t>(iInput)] & ASCII_WHITESPACE);
}
BURGER_INLINE uint_t IsLowercase(char iInput) BURGER_NOEXCEPT
{
	return static_cast<uint_t>(
		g_AsciiTestTable[static_cast<uint8_t>(iInput)] & ASCII_LOWER);
}
BURGER_INLINE uint_t IsUppercase(char iInput) BURGER_NOEXCEPT
{
	return static_cast<uint_t>(
		g_AsciiTestTable[static_cast<uint8_t>(iInput)] & ASCII_UPPER);
}
BURGER_INLINE uint_t IsSpace(int iInput) BURGER_NOEXCEPT
{
	return static_cast<uint_t>(
		(static_cast<uint_t>(iInput) < BURGER_ARRAYSIZE(g_AsciiTestTable)) &&
		(g_AsciiTestTable[static_cast<uint_t>(iInput)] & ASCII_SPACE));
}

extern void BURGER_API CStringToPString(
	uint8_t* pOutput, const char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API PStringToCString(
	char* pOutput, const uint8_t* pInput) BURGER_NOEXCEPT;

extern char* BURGER_API ParseBeyondWhiteSpace(
	const char* pInput) BURGER_NOEXCEPT;
extern char* BURGER_API ParseToDelimiter(const char* pInput) BURGER_NOEXCEPT;
extern char* BURGER_API ParseBeyondEOL(const char* pInput) BURGER_NOEXCEPT;
extern char* BURGER_API ParseBeyondEOL(
	const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
extern char* BURGER_API ParseQuotedString(
	char* pOutput, uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API CopyUpToEOL(char* pOutput, uintptr_t uOutputSize,
	const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;

extern void BURGER_API StripLeadingSpaces(char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API StripTrailingSpaces(char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API StripLeadingWhiteSpace(char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API StripTrailingWhiteSpace(char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API StripLeadingAndTrailingSpaces(
	char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API StripLeadingAndTrailingWhiteSpace(
	char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API StripAllFromList(
	char* pInput, const char* pList) BURGER_NOEXCEPT;
extern void BURGER_API StripAllButList(
	char* pInput, const char* pList) BURGER_NOEXCEPT;
extern void BURGER_API StripTrailing(
	char* pInput, const char* pList) BURGER_NOEXCEPT;
extern void BURGER_API StripLeading(
	char* pInput, const char* pList) BURGER_NOEXCEPT;

extern void BURGER_API RemoveTrailingChar(
	char* pInput, uint_t uRemove) BURGER_NOEXCEPT;
extern void BURGER_API ForceTrailingChar(
	char* pInput, uint_t uLast) BURGER_NOEXCEPT;

extern void BURGER_API SlashesToColons(char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API SlashesToColons(
	char* pOutput, const char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API SlashesToWindowsSlashes(char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API SlashesToWindowsSlashes(
	char* pOutput, const char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API EndWithWindowsSlashes(char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API EndWithWindowsSlashes(
	char* pOutput, const char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API SlashesToLinuxSlashes(char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API SlashesToLinuxSlashes(
	char* pOutput, const char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API EndWithLinuxSlashes(char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API EndWithLinuxSlashes(
	char* pOutput, const char* pInput) BURGER_NOEXCEPT;

extern void BURGER_API Replace(
	char* pInput, uint_t uFrom, uint_t uTo) BURGER_NOEXCEPT;
extern void BURGER_API Replace(char* pOutput, const char* pInput, uint_t uFrom,
	uint_t uTo) BURGER_NOEXCEPT;

extern char* BURGER_API GetFileExtension(const char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API SetFileExtension(
	char* pInput, const char* pNewExtension) BURGER_NOEXCEPT;

extern uintptr_t BURGER_API string_length(const char* pInput) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API string_length(
	const uint16_t* pInput) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API string_length(
	const uint32_t* pInput) BURGER_NOEXCEPT;

extern void BURGER_API string_copy(
	char* pOutput, const char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API string_copy(
	char* pOutput, uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API string_copy(char* pOutput, uintptr_t uOutputSize,
	const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;
extern void BURGER_API string_copy(
	uint16_t* pOutput, const uint16_t* pInput) BURGER_NOEXCEPT;
extern void BURGER_API string_copy(uint16_t* pOutput, uintptr_t uOutputSize,
	const uint16_t* pInput) BURGER_NOEXCEPT;
extern void BURGER_API string_copy(uint16_t* pOutput, uintptr_t uOutputSize,
	const uint16_t* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;

extern char* BURGER_API StringDuplicate(const char* pInput) BURGER_NOEXCEPT;
extern char* BURGER_API StringDuplicate(
	const char* pInput, uintptr_t uPadding) BURGER_NOEXCEPT;

extern void BURGER_API StringDelete(const char* pInput) BURGER_NOEXCEPT;

extern void BURGER_API StringConcatenate(
	char* pOutput, const char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API StringConcatenate(
	char* pOutput, uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API StringConcatenate(char* pOutput, uintptr_t uOutputSize,
	const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;
extern void BURGER_API StringConcatenate(
	uint16_t* pOutput, const uint16_t* pInput) BURGER_NOEXCEPT;
extern void BURGER_API StringConcatenate(uint16_t* pOutput,
	uintptr_t uOutputSize, const uint16_t* pInput) BURGER_NOEXCEPT;

extern int BURGER_API string_compare(
	const char* pInput1, const char* pInput2) BURGER_NOEXCEPT;
extern int BURGER_API string_compare(const char* pInput1, const char* pInput2,
	uintptr_t uMaxLength) BURGER_NOEXCEPT;
extern int BURGER_API string_compare(
	const uint16_t* pInput1, const uint16_t* pInput2) BURGER_NOEXCEPT;
extern int BURGER_API string_compare(const uint16_t* pInput1,
	const uint16_t* pInput2, uintptr_t uMaxLength) BURGER_NOEXCEPT;
extern int BURGER_API StringCaseCompare(
	const char* pInput1, const char* pInput2) BURGER_NOEXCEPT;
extern int BURGER_API StringCaseCompare(const char* pInput1,
	const char* pInput2, uintptr_t uMaxLength) BURGER_NOEXCEPT;

extern uint_t BURGER_API Wildcardcmp(
	const char* pInput, const char* pWildcard) BURGER_NOEXCEPT;
extern uint_t BURGER_API HasWildcard(const char* pInput) BURGER_NOEXCEPT;

extern void BURGER_API StringUppercase(char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API StringUppercase(
	char* pOutput, const char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API StringLowercase(char* pInput) BURGER_NOEXCEPT;
extern void BURGER_API StringLowercase(
	char* pOutput, const char* pInput) BURGER_NOEXCEPT;

extern char* BURGER_API StringCharacter(
	const char* pInput, int iChar) BURGER_NOEXCEPT;
extern uint16_t* BURGER_API StringCharacter(
	const uint16_t* pInput, uint_t uChar) BURGER_NOEXCEPT;
extern char* BURGER_API StringCharacterReverse(
	const char* pInput, int iChar) BURGER_NOEXCEPT;
extern uint16_t* BURGER_API StringCharacterReverse(
	const uint16_t* pInput, uint_t uChar) BURGER_NOEXCEPT;
extern uint_t BURGER_API StringEndsWith(
	char* pInput, int iChar) BURGER_NOEXCEPT;
extern uint_t BURGER_API StringEndsWith(
	uint16_t* pInput, uint_t uChar) BURGER_NOEXCEPT;

extern uintptr_t BURGER_API StringSkipOver(
	const char* pInput, const char* pDelimiters) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API StringSkipOver(
	const uint16_t* pInput, const uint16_t* pDelimiters) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API StringStopAt(
	const char* pInput, const char* pDelimiters) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API StringStopAt(
	const uint16_t* pInput, const uint16_t* pDelimiters) BURGER_NOEXCEPT;

extern char* BURGER_API StringString(
	const char* pInput, const char* pTest) BURGER_NOEXCEPT;
extern char* BURGER_API StringString(const char* pInput, const char* pTest,
	uintptr_t uTestLength) BURGER_NOEXCEPT;
extern uint16_t* BURGER_API StringString(
	const uint16_t* pInput, const uint16_t* pTest) BURGER_NOEXCEPT;
extern char* BURGER_API StringCaseString(
	const char* pInput, const char* pTest) BURGER_NOEXCEPT;
extern uint16_t* BURGER_API StringCaseString(
	const uint16_t* pInput, const uint16_t* pTest) BURGER_NOEXCEPT;

extern char* BURGER_API StringToken(
	char* pInput, const char* pDelimiters, char** ppSave) BURGER_NOEXCEPT;
extern uint16_t* BURGER_API StringToken(uint16_t* pInput,
	const uint16_t* pDelimiters, uint16_t** ppSave) BURGER_NOEXCEPT;

}

/* END */

#endif
