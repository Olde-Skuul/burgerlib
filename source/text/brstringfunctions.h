/***************************************

	Stand alone string functions

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSTRINGFUNCTIONS_H__
#define __BRSTRINGFUNCTIONS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

enum { NOENDINGNULL = 0x4000, LEADINGZEROS = 0x8000 };

enum {
	ASCII_LOWER = 0x01,
	ASCII_UPPER = 0x02,
	ASCII_CONTROL = 0x04,
	ASCII_SPACE = 0x08,
	ASCII_WHITESPACE = 0x10,
	ASCII_PUNCTUATION = 0x20,
	ASCII_HEX = 0x40,
	ASCII_DIGIT = 0x80
};

struct FourCharacterCode_t {
	union {
		char c[4]; ///< Array of 4 characters for the code
		Word32 w;  ///< Value in binary
	};
	BURGER_INLINE operator const char*() const
	{
		return c;
	}
	BURGER_INLINE operator char*()
	{
		return c;
	}
	BURGER_INLINE operator Word32() const
	{
		return w;
	}
	BURGER_INLINE FourCharacterCode_t& operator=(Word32 uInput)
	{
		w = uInput;
		return *this;
	}
	BURGER_INLINE Word32 GetCode(void) const
	{
		return w;
	}
	BURGER_INLINE void SetCode(Word32 uInput)
	{
		w = uInput;
	}
};

extern const char g_EmptyString[1];
extern const char g_CRLFString[3];
extern const char g_CRString[2];
extern const char g_LFString[2];
extern const char g_TrueString[5];
extern const char g_FalseString[6];
extern const char g_YesString[4];
extern const char g_NoString[3];
extern const char* g_WeekDays[7];
extern const char* g_Months[12];
extern const Word16 g_EmptyString16[1];
extern const Word16 g_CRLFString16[3];
extern const Word16 g_CRString16[2];
extern const Word16 g_LFString16[2];
extern const Word16 g_TrueString16[5];
extern const Word16 g_FalseString16[6];
extern const Word16 g_YesString16[4];
extern const Word16 g_NoString16[3];

extern const Word8 BURGER_ALIGN(g_AsciiTestTable[256], 16);
extern const char BURGER_ALIGN(g_NibbleToAsciiUppercase[16], 16);
extern const char BURGER_ALIGN(g_NibbleToAsciiLowercase[16], 16);
extern const Word8 BURGER_ALIGN(g_AsciiToWord8Table[256], 16);
extern const Word8 BURGER_ALIGN(g_ReverseBits[256], 16);

extern const Word32 g_TensTable32[10];
extern const Word64 g_TensTable64[20];
extern Word32 BURGER_API BCDToWord(Word32 uInput);
extern Word32 BURGER_API WordToBCD(Word32 uInput);
extern Word32 BURGER_API PowerOf2(Word32 uInput);
extern Word64 BURGER_API PowerOf2(Word64 uInput);
#if defined(BURGER_NEED_UINTPTR_REMAP)
BURGER_INLINE uintptr_t PowerOf2(uintptr_t uInput) { return PowerOf2(static_cast<BURGER_NEED_UINTPTR_REMAP>(uInput)); }
#endif
BURGER_INLINE Word32 ToLower(Word32 uInput)
{
	if (static_cast<Word32>(uInput - 'A') < 26U) {
		uInput += 32;
	}
	return uInput;
}
BURGER_INLINE Word32 ToUpper(Word32 uInput)
{
	if (static_cast<Word32>(uInput - 'a') < 26U) {
		uInput -= 32;
	}
	return uInput;
}

#if defined(BURGER_WINDOWS)
BURGER_INLINE Word IsPointerInvalid(const void* pInput)
{
	return (reinterpret_cast<WordPtr>(pInput) < 0x10000U);
}
BURGER_INLINE Word IsPointerValid(const void* pInput)
{
	return (reinterpret_cast<WordPtr>(pInput) >= 0x10000U);
}
#else
BURGER_INLINE Word IsPointerInvalid(const void* pInput)
{
	return (pInput == nullptr);
}
BURGER_INLINE Word IsPointerValid(const void* pInput)
{
	return (pInput != nullptr);
}
#endif

BURGER_INLINE Word IsStringEmpty(const char* pInput)
{
	return (IsPointerInvalid(pInput) || (!pInput[0]));
}
BURGER_INLINE Word IsStringEmpty(const Word16* pInput)
{
	return (IsPointerInvalid(pInput) || (!pInput[0]));
}
BURGER_INLINE Word IsDigit(char iInput)
{
	return static_cast<Word>(
		g_AsciiTestTable[static_cast<Word8>(iInput)] & ASCII_DIGIT);
}
BURGER_INLINE Word IsHex(char iInput)
{
	return static_cast<Word>(
		g_AsciiTestTable[static_cast<Word8>(iInput)] & ASCII_HEX);
}
BURGER_INLINE Word IsWhitespace(char iInput)
{
	return static_cast<Word>(
		g_AsciiTestTable[static_cast<Word8>(iInput)] & ASCII_WHITESPACE);
}
BURGER_INLINE Word IsLowercase(char iInput)
{
	return static_cast<Word>(
		g_AsciiTestTable[static_cast<Word8>(iInput)] & ASCII_LOWER);
}
BURGER_INLINE Word IsUppercase(char iInput)
{
	return static_cast<Word>(
		g_AsciiTestTable[static_cast<Word8>(iInput)] & ASCII_UPPER);
}

extern Word32 BURGER_API BitReverse(Word32 uInput, Word uBitLength);
extern Word64 BURGER_API BitReverse(Word64 uInput, Word uBitLength);
extern Word BURGER_API BitSetCount(Word32 uInput);
extern Word BURGER_API BitSetCount(Word64 uInput);

extern void BURGER_API CStringToPString(Word8* pOutput, const char* pInput);
extern void BURGER_API PStringToCString(char* pOutput, const Word8* pInput);

extern char* BURGER_API ParseBeyondWhiteSpace(const char* pInput);
extern char* BURGER_API ParseToDelimiter(const char* pInput);
extern char* BURGER_API ParseBeyondEOL(const char* pInput);
extern char* BURGER_API ParseBeyondEOL(const char* pInput, WordPtr uLength);
extern char* BURGER_API ParseQuotedString(
	char* pOutput, WordPtr uOutputSize, const char* pInput);
extern WordPtr BURGER_API CopyUpToEOL(
	char* pOutput, WordPtr uOutputSize, const char* pInput, WordPtr uInputSize);

extern void BURGER_API StripLeadingSpaces(char* pInput);
extern void BURGER_API StripTrailingSpaces(char* pInput);
extern void BURGER_API StripLeadingWhiteSpace(char* pInput);
extern void BURGER_API StripTrailingWhiteSpace(char* pInput);
extern void BURGER_API StripLeadingAndTrailingSpaces(char* pInput);
extern void BURGER_API StripLeadingAndTrailingWhiteSpace(char* pInput);
extern void BURGER_API StripAllFromList(char* pInput, const char* pList);
extern void BURGER_API StripAllButList(char* pInput, const char* pList);
extern void BURGER_API StripTrailing(char* pInput, const char* pList);
extern void BURGER_API StripLeading(char* pInput, const char* pList);

extern void BURGER_API RemoveTrailingChar(char* pInput, Word uRemove);
extern void BURGER_API ForceTrailingChar(char* pInput, Word uLast);

extern void BURGER_API SlashesToColons(char* pInput);
extern void BURGER_API SlashesToColons(char* pOutput, const char* pInput);
extern void BURGER_API SlashesToWindowsSlashes(char* pInput);
extern void BURGER_API SlashesToWindowsSlashes(
	char* pOutput, const char* pInput);
extern void BURGER_API EndWithWindowsSlashes(char* pInput);
extern void BURGER_API EndWithWindowsSlashes(char* pOutput, const char* pInput);
extern void BURGER_API SlashesToLinuxSlashes(char* pInput);
extern void BURGER_API SlashesToLinuxSlashes(char* pOutput, const char* pInput);
extern void BURGER_API EndWithLinuxSlashes(char* pInput);
extern void BURGER_API EndWithLinuxSlashes(char* pOutput, const char* pInput);

extern void BURGER_API Replace(char* pInput, Word uFrom, Word uTo);
extern void BURGER_API Replace(
	char* pOutput, const char* pInput, Word uFrom, Word uTo);

extern char* BURGER_API GetFileExtension(const char* pInput);
extern void BURGER_API SetFileExtension(
	char* pInput, const char* pNewExtension);

extern WordPtr BURGER_API StringLength(const char* pInput) BURGER_NOEXCEPT;
extern WordPtr BURGER_API StringLength(const Word16* pInput);

extern void BURGER_API StringCopy(char* pOutput, const char* pInput);
extern void BURGER_API StringCopy(
	char* pOutput, WordPtr uOutputSize, const char* pInput);
extern void BURGER_API StringCopy(
	char* pOutput, WordPtr uOutputSize, const char* pInput, WordPtr uInputSize);
extern void BURGER_API StringCopy(Word16* pOutput, const Word16* pInput);
extern void BURGER_API StringCopy(
	Word16* pOutput, WordPtr uOutputSize, const Word16* pInput);
extern void BURGER_API StringCopy(Word16* pOutput, WordPtr uOutputSize,
	const Word16* pInput, WordPtr uInputSize);

extern char* BURGER_API StringDuplicate(const char* pInput);
extern char* BURGER_API StringDuplicate(const char* pInput, WordPtr uPadding);

extern void BURGER_API StringDelete(const char* pInput);

extern void BURGER_API StringConcatenate(char* pOutput, const char* pInput);
extern void BURGER_API StringConcatenate(
	char* pOutput, WordPtr uOutputSize, const char* pInput);
extern void BURGER_API StringConcatenate(
	char* pOutput, WordPtr uOutputSize, const char* pInput, WordPtr uInputSize);
extern void BURGER_API StringConcatenate(Word16* pOutput, const Word16* pInput);
extern void BURGER_API StringConcatenate(
	Word16* pOutput, WordPtr uOutputSize, const Word16* pInput);

extern int BURGER_API StringCompare(const char* pInput1, const char* pInput2);
extern int BURGER_API StringCompare(
	const char* pInput1, const char* pInput2, WordPtr uMaxLength);
extern int BURGER_API StringCompare(
	const Word16* pInput1, const Word16* pInput2);
extern int BURGER_API StringCompare(
	const Word16* pInput1, const Word16* pInput2, WordPtr uMaxLength);
extern int BURGER_API StringCaseCompare(
	const char* pInput1, const char* pInput2) BURGER_NOEXCEPT;
extern int BURGER_API StringCaseCompare(
	const char* pInput1, const char* pInput2, WordPtr uMaxLength);

extern Word BURGER_API Wildcardcmp(const char* pInput, const char* pWildcard);
extern Word BURGER_API HasWildcard(const char* pInput);

extern void BURGER_API StringUppercase(char* pInput);
extern void BURGER_API StringUppercase(char* pOutput, const char* pInput);
extern void BURGER_API StringLowercase(char* pInput);
extern void BURGER_API StringLowercase(char* pOutput, const char* pInput);

extern char* BURGER_API StringCharacter(const char* pInput, int iChar);
extern Word16* BURGER_API StringCharacter(const Word16* pInput, Word uChar);
extern char* BURGER_API StringCharacterReverse(const char* pInput, int iChar);
extern Word16* BURGER_API StringCharacterReverse(
	const Word16* pInput, Word uChar);

extern WordPtr BURGER_API StringSkipOver(
	const char* pInput,const char* pDelimiters);
extern WordPtr BURGER_API StringSkipOver(
	const Word16* pInput,const Word16* pDelimiters);
extern WordPtr BURGER_API StringStopAt(
	const char* pInput, const char* pDelimiters);
extern WordPtr BURGER_API StringStopAt(
	const Word16* pInput, const Word16* pDelimiters);

extern char* BURGER_API StringString(const char* pInput, const char* pTest);
extern Word16* BURGER_API StringString(
	const Word16* pInput, const Word16* pTest);
extern char* BURGER_API StringCaseString(const char* pInput, const char* pTest);
extern Word16* BURGER_API StringCaseString(
	const Word16* pInput, const Word16* pTest);

extern char* BURGER_API StringToken(
	char* pInput, const char* pDelimiters, char** ppSave);
extern Word16* BURGER_API StringToken(
	Word16* pInput, const Word16* pDelimiters, Word16** ppSave);

}

/* END */

#endif
