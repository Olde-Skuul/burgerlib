/***************************************

	C++ ANSI "C" string class
	This is optimized for performance

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSTRING_H__
#define __BRSTRING_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __BRGLOBALMEMORYMANAGER_H__
#include "brglobalmemorymanager.h"
#endif

#ifndef __BRASCIITO_H__
#include "brasciito.h"
#endif

#ifndef __BRSTRINGFUNCTIONS_H__
#include "brstringfunctions.h"
#endif

#ifndef __BRPRINTF_H__
#include "brprintf.h"
#endif

/* BEGIN */
namespace Burger {

#define MAKE_BURGER_STRING_FORMATTED_CONSTRUCTOR(N) \
	String(const char* pFmt, BURGER_SP_ARG##N) : m_pData(m_Raw), m_uLength(0) \
	{ \
		m_Raw[0] = 0; \
		const Burger::SafePrintArgument* args[N] = {BURGER_SP_INITARG##N}; \
		InitFormattedString(pFmt, N, args); \
	}

#define MAKE_BURGER_STRING_PRINTF(N) \
	void BURGER_INLINE Printf(const char* pFmt, BURGER_SP_ARG##N) \
	{ \
		const Burger::SafePrintArgument* args[N] = {BURGER_SP_INITARG##N}; \
		InitFormattedString(pFmt, N, args); \
	}

#define MAKE_BURGER_STRING_PRINTF_STRREF(N) \
	void BURGER_INLINE Printf(const String& sFmt, BURGER_SP_ARG##N) \
	{ \
		const Burger::SafePrintArgument* args[N] = {BURGER_SP_INITARG##N}; \
		InitFormattedString(sFmt.GetPtr(), N, args); \
	}

class String {

public:
	/** Ensures the structure is 64 bytes in size on all platforms */
	static const Word BUFFERSIZE =
		static_cast<Word>(64 - (sizeof(char*) + sizeof(WordPtr)));

private:
	char* m_pData;			///< Pointer to the string
	WordPtr m_uLength;		///< Length of the string
	char m_Raw[BUFFERSIZE]; ///< Temp preallocated buffer for most strings

public:
	String(void) BURGER_NOEXCEPT : m_pData(m_Raw), m_uLength(0)
	{
		m_Raw[0] = 0;
	}
	String(const String& rInput) BURGER_NOEXCEPT;
	String(
		const String& rInput, WordPtr uStart, WordPtr uEnd = UINTPTR_MAX) BURGER_NOEXCEPT;
	String(const char* pInput) BURGER_NOEXCEPT;
	String(const char* pInput, WordPtr uPadding) BURGER_NOEXCEPT;
	String(const Word16* pInput) BURGER_NOEXCEPT;
	String(const Word32* pInput) BURGER_NOEXCEPT;
	String(const char* pInput, WordPtr uStart, WordPtr uEnd) BURGER_NOEXCEPT;
	String(char cInput) BURGER_NOEXCEPT;
	String(char cInput, WordPtr uFillSize) BURGER_NOEXCEPT;
	String(const char* pInput1, const char* pInput2) BURGER_NOEXCEPT;
	String(const char* pInput1, const char* pInput2, const char* pInput3) BURGER_NOEXCEPT;
	String(const char* pInput1, const char* pInput2, const char* pInput3,
		const char* pInput4) BURGER_NOEXCEPT;

	BURGER_EXPAND_FORMATTING_FUNCTION(MAKE_BURGER_STRING_FORMATTED_CONSTRUCTOR);

	BURGER_EXPAND_FORMATTING_FUNCTION(
		MAKE_BURGER_STRING_PRINTF); ///< String.Printf( const char* pFmt, .... )
	BURGER_EXPAND_FORMATTING_FUNCTION(
		MAKE_BURGER_STRING_PRINTF_STRREF); ///< String.Printf( const String&
										   ///< pFmt, .... )

	~String(void)
	{
		if (m_pData != m_Raw) {
			Free(m_pData);
		}
	}

	BURGER_INLINE operator char*() BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE operator const char*() const BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE const char* c_str(void) const BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE char* c_str(void) BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE operator char() const BURGER_NOEXCEPT
	{
		return m_pData[0];
	}
	BURGER_INLINE WordPtr length(void) const BURGER_NOEXCEPT
	{
		return m_uLength;
	}
	BURGER_INLINE WordPtr GetLength(void) const BURGER_NOEXCEPT
	{
		return m_uLength;
	}
	BURGER_INLINE char* GetPtr(void) BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE const char* GetPtr(void) const BURGER_NOEXCEPT
	{
		return m_pData;
	}
	eError BURGER_API Set(const char* pInput);
	eError BURGER_API Set(const char* pInput, WordPtr uLength);
	eError BURGER_API Set(const Word16* pInput);
	eError BURGER_API Set(const Word16* pInput, WordPtr uLength);
	eError BURGER_API SetBufferSize(WordPtr uSize) BURGER_NOEXCEPT;

	String& operator=(const String& rInput);
	String& operator=(const char* pInput) BURGER_NOEXCEPT;
	String& operator=(char cInput);
	String& operator+=(const String& rInput);
	String& operator+=(const char* pInput) BURGER_NOEXCEPT;
	String& operator+=(char cInput);
	BURGER_INLINE String operator()(WordPtr uStart, WordPtr uEnd) const BURGER_NOEXCEPT
	{
		return String(*this, uStart, uEnd);
	}
	BURGER_INLINE char& operator()(WordPtr uInput) BURGER_NOEXCEPT
	{
		if (uInput >= m_uLength) {
			m_Raw[BUFFERSIZE - 1] = 0;
			return m_Raw[BUFFERSIZE - 1];
		}
		return m_pData[uInput];
	}
	BURGER_INLINE char const& operator()(WordPtr uInput) const BURGER_NOEXCEPT
	{
		if (uInput >= m_uLength) {
			return g_EmptyString[0];
		}
		return m_pData[uInput];
	}
	BURGER_INLINE char& operator[](WordPtr uInput) BURGER_NOEXCEPT
	{
		if (uInput >= m_uLength) {
			m_Raw[BUFFERSIZE - 1] = 0;
			return m_Raw[BUFFERSIZE - 1];
		}
		return m_pData[uInput];
	}
	BURGER_INLINE char const& operator[](WordPtr uInput) const BURGER_NOEXCEPT
	{
		if (uInput >= m_uLength) {
			return g_EmptyString[0];
		}
		return m_pData[uInput];
	}
	char BURGER_API Get(WordPtr uWhere) const;
	void BURGER_API Put(WordPtr uWhere, char cInput);
	char* BURGER_API StringString(const char* pInput) const;
	BURGER_INLINE String& ToLowercase(void)
	{
		Burger::StringLowercase(this->m_pData);
		return *this;
	}
	BURGER_INLINE String& ToUppercase(void)
	{
		Burger::StringUppercase(this->m_pData);
		return *this;
	}
	BURGER_INLINE String Lowercase(void) const
	{
		return String(*this).ToLowercase();
	}
	BURGER_INLINE String Uppercase(void) const
	{
		return String(*this).ToUppercase();
	}
	BURGER_INLINE Word operator!() const BURGER_NOEXCEPT
	{
		return (m_uLength == 0);
	}
	BURGER_INLINE Word IsValid(void) const BURGER_NOEXCEPT
	{
		return (m_uLength != 0);
	}
	void BURGER_API Clear(void) BURGER_NOEXCEPT;
	WordPtr BURGER_API Copy(
		char* pOutput, WordPtr uOutputSize = UINTPTR_MAX) const;
	WordPtr BURGER_API PCopy(Word8* pOutput, WordPtr uOutputSize = 256) const;
	WordPtr BURGER_API Remove(
		WordPtr uStart, WordPtr uLength = UINTPTR_MAX);
	WordPtr BURGER_API Insert(
		WordPtr uStart, const char* pInput, WordPtr uInputSize);
	WordPtr BURGER_API Append(const char* pInput, WordPtr uInputSize);
	String& BURGER_API Left(WordPtr uNewLength, char cPad = ' ');
	String& BURGER_API Right(WordPtr uNewLength, char cPad = ' ');
	BURGER_INLINE int Compare(const String& rInput) const
	{
		return StringCompare(m_pData, rInput.GetPtr());
	}
	int BURGER_API Compare(const char* pInput) const;
	int BURGER_API Compare(char cInput) const;
	WordPtr BURGER_API RemoveChars(const char* pInput);
	WordPtr BURGER_API RemoveChar(char cInput);
	WordPtr BURGER_API NormalizeLineFeeds(void);
	BURGER_INLINE Word GetBoolean(Word bDefault) const
	{
		return AsciiToBoolean(m_pData, bDefault);
	}
	void BURGER_API SetTrueFalse(Word bInput);
	void BURGER_API SetYesNo(Word bInput);
	BURGER_INLINE Word GetWord(
		Word uDefault, Word uMin = 0, Word uMax = BURGER_MAXUINT) const
	{
		return AsciiToWord(m_pData, uDefault, uMin, uMax);
	}
	void BURGER_API SetWord(Word uInput);
	void BURGER_API SetWordHex(Word uInput);
	BURGER_INLINE Int GetInt(Int iDefault, Int iMin = (-BURGER_MAXINT) - 1,
		Int iMax = BURGER_MAXINT) const
	{
		return AsciiToInteger(m_pData, iDefault, iMin, iMax);
	}
	void BURGER_API SetInt(Int iInput);
	BURGER_INLINE float GetFloat(float fDefault) const
	{
		return AsciiToFloat(m_pData, fDefault);
	}
	BURGER_INLINE float GetFloat(float fDefault, float fMin, float fMax) const
	{
		return AsciiToFloat(m_pData, fDefault, fMin, fMax);
	}
	void BURGER_API SetFloat(float fValue);
	BURGER_INLINE double GetDouble(double dDefault) const
	{
		return AsciiToDouble(m_pData, dDefault);
	}
	BURGER_INLINE double GetDouble(
		double dDefault, double dMin, double dMax) const
	{
		return AsciiToDouble(m_pData, dDefault, dMin, dMax);
	}
	void BURGER_API SetDouble(double fValue);
	void BURGER_API SetFast(const char* pInput);
	void BURGER_API SetFast(const char* pInput, WordPtr uLength);
	BURGER_INLINE friend Word operator==(
		String const& rInput1, String const& rInput2)
	{
		return static_cast<Word>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) == 0);
	}
	BURGER_INLINE friend Word operator==(
		String const& rInput1, const char* pInput2)
	{
		return static_cast<Word>(rInput1.Compare(pInput2) == 0);
	}
	BURGER_INLINE friend Word operator==(
		const char* pInput1, String const& rInput2)
	{
		return static_cast<Word>(rInput2.Compare(pInput1) == 0);
	}
	BURGER_INLINE friend Word operator==(String const& rInput1, char cInput2)
	{
		return static_cast<Word>(rInput1.Compare(cInput2) == 0);
	}
	BURGER_INLINE friend Word operator==(char cInput1, String const& rInput2)
	{
		return static_cast<Word>(rInput2.Compare(cInput1) == 0);
	}
	BURGER_INLINE friend Word operator!=(
		String const& rInput1, String const& rInput2)
	{
		return static_cast<Word>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) != 0);
	}
	BURGER_INLINE friend Word operator!=(
		String const& rInput1, const char* pInput2)
	{
		return static_cast<Word>(rInput1.Compare(pInput2) != 0);
	}
	BURGER_INLINE friend Word operator!=(
		const char* pInput1, String const& rInput2)
	{
		return static_cast<Word>(rInput2.Compare(pInput1) != 0);
	}
	BURGER_INLINE friend Word operator!=(String const& rInput1, char cInput2)
	{
		return static_cast<Word>(rInput1.Compare(cInput2) != 0);
	}
	BURGER_INLINE friend Word operator!=(char cInput1, String const& rInput2)
	{
		return static_cast<Word>(rInput2.Compare(cInput1) != 0);
	}
	BURGER_INLINE friend Word operator<(
		String const& rInput1, String const& rInput2)
	{
		return static_cast<Word>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) < 0);
	}
	BURGER_INLINE friend Word operator<(
		String const& rInput1, const char* pInput2)
	{
		return static_cast<Word>(rInput1.Compare(pInput2) < 0);
	}
	BURGER_INLINE friend Word operator<(
		const char* pInput1, String const& rInput2)
	{
		return static_cast<Word>(rInput2.Compare(pInput1) > 0);
	}
	BURGER_INLINE friend Word operator<(String const& rInput1, char cInput2)
	{
		return static_cast<Word>(rInput1.Compare(cInput2) < 0);
	}
	BURGER_INLINE friend Word operator<(char cInput1, String const& rInput2)
	{
		return static_cast<Word>(rInput2.Compare(cInput1) > 0);
	}
	BURGER_INLINE friend Word operator<=(
		String const& rInput1, String const& rInput2)
	{
		return static_cast<Word>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) <= 0);
	}
	BURGER_INLINE friend Word operator<=(
		String const& rInput1, const char* pInput2)
	{
		return static_cast<Word>(rInput1.Compare(pInput2) <= 0);
	}
	BURGER_INLINE friend Word operator<=(
		const char* pInput1, String const& rInput2)
	{
		return static_cast<Word>(rInput2.Compare(pInput1) >= 0);
	}
	BURGER_INLINE friend Word operator<=(String const& rInput1, char cInput2)
	{
		return static_cast<Word>(rInput1.Compare(cInput2) <= 0);
	}
	BURGER_INLINE friend Word operator<=(char cInput1, String const& rInput2)
	{
		return static_cast<Word>(rInput2.Compare(cInput1) >= 0);
	}
	BURGER_INLINE friend Word operator>(
		String const& rInput1, String const& rInput2)
	{
		return static_cast<Word>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) > 0);
	}
	BURGER_INLINE friend Word operator>(
		String const& rInput1, const char* pInput2)
	{
		return static_cast<Word>(rInput1.Compare(pInput2) > 0);
	}
	BURGER_INLINE friend Word operator>(
		const char* pInput1, String const& rInput2)
	{
		return static_cast<Word>(rInput2.Compare(pInput1) < 0);
	}
	BURGER_INLINE friend Word operator>(String const& rInput1, char cInput2)
	{
		return static_cast<Word>(rInput1.Compare(cInput2) > 0);
	}
	BURGER_INLINE friend Word operator>(char cInput1, String const& rInput2)
	{
		return static_cast<Word>(rInput2.Compare(cInput1) < 0);
	}
	BURGER_INLINE friend Word operator>=(
		String const& rInput1, String const& rInput2)
	{
		return static_cast<Word>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) >= 0);
	}
	BURGER_INLINE friend Word operator>=(
		String const& rInput1, const char* pInput2)
	{
		return static_cast<Word>(rInput1.Compare(pInput2) >= 0);
	}
	BURGER_INLINE friend Word operator>=(
		const char* pInput1, String const& rInput2)
	{
		return static_cast<Word>(rInput2.Compare(pInput1) <= 0);
	}
	BURGER_INLINE friend Word operator>=(String const& rInput1, char cInput2)
	{
		return static_cast<Word>(rInput1.Compare(cInput2) >= 0);
	}
	BURGER_INLINE friend Word operator>=(char cInput1, String const& rInput2)
	{
		return static_cast<Word>(rInput2.Compare(cInput1) <= 0);
	}

private:
	void BURGER_API InitFormattedString(const char* pFormat, WordPtr uArgCount,
		const SafePrintArgument** ppArgs);
	static Word BURGER_API FormattedAllocCallback(Word bError,
		WordPtr uRequestedSize, void** ppOutputBuffer, void* pContext);
};
}
/* END */

#endif
