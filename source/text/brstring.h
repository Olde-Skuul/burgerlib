/***************************************

	C++ ANSI "C" string class
	This is optimized for performance

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
	String(const char* pFmt, BURGER_SP_ARG##N): m_pData(m_Raw), m_uLength(0) \
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
	static const uint_t kBufferSize =
		static_cast<uint_t>(64 - (sizeof(char*) + sizeof(uintptr_t)));

private:
	/** Pointer to the string */
	char* m_pData;
	/** Length of the string */
	uintptr_t m_uLength;
	/** Temp preallocated buffer for most strings */
	char m_Raw[kBufferSize];

public:
	String(void) BURGER_NOEXCEPT: m_pData(m_Raw), m_uLength(0)
	{
		m_Raw[0] = 0;
	}
	String(const String& rInput) BURGER_NOEXCEPT;
	String(const String& rInput, uintptr_t uStart,
		uintptr_t uEnd = UINTPTR_MAX) BURGER_NOEXCEPT;

#if defined(BURGER_RVALUE_REFERENCES)
	String(String&& rInput) BURGER_NOEXCEPT
	{
		m_uLength = rInput.m_uLength;
		if (rInput.m_pData != rInput.m_Raw) {
			m_pData = rInput.m_pData;
			rInput.m_pData = rInput.m_Raw;
			rInput.m_Raw[0] = 0;
			rInput.m_uLength = 0;
		} else {
			m_pData = m_Raw;
			StringCopy(m_Raw, sizeof(m_Raw), rInput.m_pData);
		}
	}

	String& operator=(String&& rInput) BURGER_NOEXCEPT
	{
		if (this != &rInput) {
			if (m_pData != m_Raw) {
				Free(m_pData);
			}
			m_uLength = rInput.m_uLength;
			if (rInput.m_pData != rInput.m_Raw) {
				m_pData = rInput.m_pData;
				rInput.m_pData = rInput.m_Raw;
				rInput.m_Raw[0] = 0;
				rInput.m_uLength = 0;
			} else {
				m_pData = m_Raw;
				StringCopy(m_Raw, sizeof(m_Raw), rInput.m_pData);
			}
		}
		return *this;
	}
#endif

	String(const char* pInput) BURGER_NOEXCEPT;
	String(const char* pInput, uintptr_t uPadding) BURGER_NOEXCEPT;
	String(const uint16_t* pInput) BURGER_NOEXCEPT;
	String(const uint32_t* pInput) BURGER_NOEXCEPT;
	String(
		const char* pInput, uintptr_t uStart, uintptr_t uEnd) BURGER_NOEXCEPT;
	String(char cInput) BURGER_NOEXCEPT;
	String(char cInput, uintptr_t uFillSize) BURGER_NOEXCEPT;
	String(const char* pInput1, const char* pInput2) BURGER_NOEXCEPT;
	String(const char* pInput1, const char* pInput2,
		const char* pInput3) BURGER_NOEXCEPT;
	String(const char* pInput1, const char* pInput2, const char* pInput3,
		const char* pInput4) BURGER_NOEXCEPT;

	BURGER_EXPAND_FORMATTING_FUNCTION(MAKE_BURGER_STRING_FORMATTED_CONSTRUCTOR);

	/** String.Printf(const char* pFmt, .... ) */
	BURGER_EXPAND_FORMATTING_FUNCTION(MAKE_BURGER_STRING_PRINTF);

	/** String.Printf(const String& pFmt, .... ) */
	BURGER_EXPAND_FORMATTING_FUNCTION(MAKE_BURGER_STRING_PRINTF_STRREF);

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
	BURGER_INLINE const char* data(void) const BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE operator char() const BURGER_NOEXCEPT
	{
		return m_pData[0];
	}
	BURGER_INLINE uintptr_t length(void) const BURGER_NOEXCEPT
	{
		return m_uLength;
	}
	BURGER_INLINE uintptr_t GetLength(void) const BURGER_NOEXCEPT
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
	eError BURGER_API Set(const char* pInput) BURGER_NOEXCEPT;
	eError BURGER_API Set(
		const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
	eError BURGER_API Set(const uint16_t* pInput) BURGER_NOEXCEPT;
	eError BURGER_API Set(
		const uint16_t* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
	eError BURGER_API SetBufferSize(uintptr_t uSize) BURGER_NOEXCEPT;

	String& operator=(const String& rInput);
	String& operator=(const char* pInput) BURGER_NOEXCEPT;
	String& operator=(char cInput);
	String& operator+=(const String& rInput);
	String& operator+=(const char* pInput) BURGER_NOEXCEPT;
	String& operator+=(char cInput);
	BURGER_INLINE String operator()(
		uintptr_t uStart, uintptr_t uEnd) const BURGER_NOEXCEPT
	{
		return String(*this, uStart, uEnd);
	}
	BURGER_INLINE char& operator()(uintptr_t uInput) BURGER_NOEXCEPT
	{
		if (uInput >= m_uLength) {
			m_Raw[kBufferSize - 1] = 0;
			return m_Raw[kBufferSize - 1];
		}
		return m_pData[uInput];
	}
	BURGER_INLINE char const& operator()(uintptr_t uInput) const BURGER_NOEXCEPT
	{
		if (uInput >= m_uLength) {
			return g_EmptyString[0];
		}
		return m_pData[uInput];
	}
	BURGER_INLINE char& operator[](uintptr_t uInput) BURGER_NOEXCEPT
	{
		if (uInput >= m_uLength) {
			m_Raw[kBufferSize - 1] = 0;
			return m_Raw[kBufferSize - 1];
		}
		return m_pData[uInput];
	}
	BURGER_INLINE char const& operator[](uintptr_t uInput) const BURGER_NOEXCEPT
	{
		if (uInput >= m_uLength) {
			return g_EmptyString[0];
		}
		return m_pData[uInput];
	}
	char BURGER_API Get(uintptr_t uWhere) const;
	void BURGER_API Put(uintptr_t uWhere, char cInput);
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
	BURGER_INLINE uint_t operator!() const BURGER_NOEXCEPT
	{
		return (m_uLength == 0);
	}
	BURGER_INLINE uint_t IsValid(void) const BURGER_NOEXCEPT
	{
		return (m_uLength != 0);
	}
	void BURGER_API Clear(void) BURGER_NOEXCEPT;
	uintptr_t BURGER_API Copy(
		char* pOutput, uintptr_t uOutputSize = UINTPTR_MAX) const;
	uintptr_t BURGER_API PCopy(
		uint8_t* pOutput, uintptr_t uOutputSize = 256) const;
	uintptr_t BURGER_API Remove(
		uintptr_t uStart, uintptr_t uLength = UINTPTR_MAX);
	uintptr_t BURGER_API Insert(
		uintptr_t uStart, const char* pInput, uintptr_t uInputSize);
	uintptr_t BURGER_API Append(const char* pInput, uintptr_t uInputSize);
	String& BURGER_API Left(uintptr_t uNewLength, char cPad = ' ');
	String& BURGER_API Right(uintptr_t uNewLength, char cPad = ' ');
	BURGER_INLINE int Compare(const String& rInput) const
	{
		return StringCompare(m_pData, rInput.GetPtr());
	}
	int BURGER_API Compare(const char* pInput) const;
	int BURGER_API Compare(char cInput) const;
	uintptr_t BURGER_API RemoveChars(const char* pInput);
	uintptr_t BURGER_API RemoveChar(char cInput);
	uintptr_t BURGER_API NormalizeLineFeeds(void);
	BURGER_INLINE uint_t GetBoolean(uint_t bDefault) const
	{
		return AsciiToBoolean(m_pData, bDefault);
	}
	void BURGER_API SetTrueFalse(uint_t bInput);
	void BURGER_API SetYesNo(uint_t bInput);
	BURGER_INLINE uint_t GetWord(
		uint_t uDefault, uint_t uMin = 0, uint_t uMax = BURGER_MAXUINT) const
	{
		return AsciiToWord(m_pData, uDefault, uMin, uMax);
	}
	void BURGER_API SetWord(uint_t uInput);
	void BURGER_API SetWordHex(uint_t uInput);
	BURGER_INLINE int_t GetInt(int_t iDefault,
		int_t iMin = (-BURGER_MAXINT) - 1, int_t iMax = BURGER_MAXINT) const
	{
		return AsciiToInteger(m_pData, iDefault, iMin, iMax);
	}
	void BURGER_API SetInt(int_t iInput);
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
	void BURGER_API SetFast(const char* pInput, uintptr_t uLength);
	BURGER_INLINE friend uint_t operator==(
		String const& rInput1, String const& rInput2)
	{
		return static_cast<uint_t>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) == 0);
	}
	BURGER_INLINE friend uint_t operator==(
		String const& rInput1, const char* pInput2)
	{
		return static_cast<uint_t>(rInput1.Compare(pInput2) == 0);
	}
	BURGER_INLINE friend uint_t operator==(
		const char* pInput1, String const& rInput2)
	{
		return static_cast<uint_t>(rInput2.Compare(pInput1) == 0);
	}
	BURGER_INLINE friend uint_t operator==(String const& rInput1, char cInput2)
	{
		return static_cast<uint_t>(rInput1.Compare(cInput2) == 0);
	}
	BURGER_INLINE friend uint_t operator==(char cInput1, String const& rInput2)
	{
		return static_cast<uint_t>(rInput2.Compare(cInput1) == 0);
	}
	BURGER_INLINE friend uint_t operator!=(
		String const& rInput1, String const& rInput2)
	{
		return static_cast<uint_t>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) != 0);
	}
	BURGER_INLINE friend uint_t operator!=(
		String const& rInput1, const char* pInput2)
	{
		return static_cast<uint_t>(rInput1.Compare(pInput2) != 0);
	}
	BURGER_INLINE friend uint_t operator!=(
		const char* pInput1, String const& rInput2)
	{
		return static_cast<uint_t>(rInput2.Compare(pInput1) != 0);
	}
	BURGER_INLINE friend uint_t operator!=(String const& rInput1, char cInput2)
	{
		return static_cast<uint_t>(rInput1.Compare(cInput2) != 0);
	}
	BURGER_INLINE friend uint_t operator!=(char cInput1, String const& rInput2)
	{
		return static_cast<uint_t>(rInput2.Compare(cInput1) != 0);
	}
	BURGER_INLINE friend uint_t operator<(
		String const& rInput1, String const& rInput2)
	{
		return static_cast<uint_t>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) < 0);
	}
	BURGER_INLINE friend uint_t operator<(
		String const& rInput1, const char* pInput2)
	{
		return static_cast<uint_t>(rInput1.Compare(pInput2) < 0);
	}
	BURGER_INLINE friend uint_t operator<(
		const char* pInput1, String const& rInput2)
	{
		return static_cast<uint_t>(rInput2.Compare(pInput1) > 0);
	}
	BURGER_INLINE friend uint_t operator<(String const& rInput1, char cInput2)
	{
		return static_cast<uint_t>(rInput1.Compare(cInput2) < 0);
	}
	BURGER_INLINE friend uint_t operator<(char cInput1, String const& rInput2)
	{
		return static_cast<uint_t>(rInput2.Compare(cInput1) > 0);
	}
	BURGER_INLINE friend uint_t operator<=(
		String const& rInput1, String const& rInput2)
	{
		return static_cast<uint_t>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) <= 0);
	}
	BURGER_INLINE friend uint_t operator<=(
		String const& rInput1, const char* pInput2)
	{
		return static_cast<uint_t>(rInput1.Compare(pInput2) <= 0);
	}
	BURGER_INLINE friend uint_t operator<=(
		const char* pInput1, String const& rInput2)
	{
		return static_cast<uint_t>(rInput2.Compare(pInput1) >= 0);
	}
	BURGER_INLINE friend uint_t operator<=(String const& rInput1, char cInput2)
	{
		return static_cast<uint_t>(rInput1.Compare(cInput2) <= 0);
	}
	BURGER_INLINE friend uint_t operator<=(char cInput1, String const& rInput2)
	{
		return static_cast<uint_t>(rInput2.Compare(cInput1) >= 0);
	}
	BURGER_INLINE friend uint_t operator>(
		String const& rInput1, String const& rInput2)
	{
		return static_cast<uint_t>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) > 0);
	}
	BURGER_INLINE friend uint_t operator>(
		String const& rInput1, const char* pInput2)
	{
		return static_cast<uint_t>(rInput1.Compare(pInput2) > 0);
	}
	BURGER_INLINE friend uint_t operator>(
		const char* pInput1, String const& rInput2)
	{
		return static_cast<uint_t>(rInput2.Compare(pInput1) < 0);
	}
	BURGER_INLINE friend uint_t operator>(String const& rInput1, char cInput2)
	{
		return static_cast<uint_t>(rInput1.Compare(cInput2) > 0);
	}
	BURGER_INLINE friend uint_t operator>(char cInput1, String const& rInput2)
	{
		return static_cast<uint_t>(rInput2.Compare(cInput1) < 0);
	}
	BURGER_INLINE friend uint_t operator>=(
		String const& rInput1, String const& rInput2)
	{
		return static_cast<uint_t>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) >= 0);
	}
	BURGER_INLINE friend uint_t operator>=(
		String const& rInput1, const char* pInput2)
	{
		return static_cast<uint_t>(rInput1.Compare(pInput2) >= 0);
	}
	BURGER_INLINE friend uint_t operator>=(
		const char* pInput1, String const& rInput2)
	{
		return static_cast<uint_t>(rInput2.Compare(pInput1) <= 0);
	}
	BURGER_INLINE friend uint_t operator>=(String const& rInput1, char cInput2)
	{
		return static_cast<uint_t>(rInput1.Compare(cInput2) >= 0);
	}
	BURGER_INLINE friend uint_t operator>=(char cInput1, String const& rInput2)
	{
		return static_cast<uint_t>(rInput2.Compare(cInput1) <= 0);
	}

private:
	void BURGER_API InitFormattedString(const char* pFormat,
		uintptr_t uArgCount, const SafePrintArgument** ppArgs);
	static uint_t BURGER_API FormattedAllocCallback(uint_t bError,
		uintptr_t uRequestedSize, void** ppOutputBuffer, void* pContext);
};
}
/* END */

#endif
