/***************************************

	This is the Burgerlib C++ string class. I will not trigger exceptions on
	memory errors. However, I do take great care in making sure that the class
	structure is in a valid state at all times

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
	String(const char* pFmt, BURGER_SP_ARG##N) BURGER_NOEXCEPT \
		: m_pData(m_Raw), \
		  m_uLength(0), \
		  m_uBufferSize(kBufferSize - 1) \
	{ \
		m_Raw[0] = 0; \
		const Burger::ArgumentType* args[N] = {BURGER_SP_INITARG##N}; \
		InitFormattedString(pFmt, N, args); \
	}

#define MAKE_BURGER_STRING_PRINTF(N) \
	void BURGER_INLINE Printf(const char* pFmt, BURGER_SP_ARG##N) \
		BURGER_NOEXCEPT \
	{ \
		const Burger::ArgumentType* args[N] = {BURGER_SP_INITARG##N}; \
		InitFormattedString(pFmt, N, args); \
	}

#define MAKE_BURGER_STRING_PRINTF_STRREF(N) \
	void BURGER_INLINE Printf(const String& sFmt, BURGER_SP_ARG##N) \
		BURGER_NOEXCEPT \
	{ \
		const Burger::ArgumentType* args[N] = {BURGER_SP_INITARG##N}; \
		InitFormattedString(sFmt.c_str(), N, args); \
	}

class String {

public:
	/** Ensures the structure is 96 bytes in size on all platforms */
	static const uint_t kBufferSize = static_cast<uint_t>(
		96 - (sizeof(char*) + sizeof(uintptr_t) + sizeof(uintptr_t)));

	/** Value returned when find() doesn't return a match. */
	static const uintptr_t npos = UINTPTR_MAX;

protected:
	/** Pointer to the string */
	char* m_pData;

	/** Length of the string */
	uintptr_t m_uLength;

	/** Size of the buffer, minimum kBufferSize */
	uintptr_t m_uBufferSize;

	/** Temp preallocated buffer for most strings */
	char m_Raw[kBufferSize];

	char* BURGER_API constructor_reserve(uintptr_t uBufferSize) BURGER_NOEXCEPT;
	void BURGER_API assign_small(const char* pInput) BURGER_NOEXCEPT;
	void BURGER_API assign_small(
		const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT;

public:
	String(void) BURGER_NOEXCEPT: m_pData(m_Raw),
								  m_uLength(0),
								  m_uBufferSize(kBufferSize - 1)
	{
		m_Raw[0] = 0;
	}

	String(const String& rInput) BURGER_NOEXCEPT;

#if defined(BURGER_RVALUE_REFERENCES) || defined(DOXYGEN)
	String(String&& rInput) BURGER_NOEXCEPT;
	String& operator=(String&& rInput) BURGER_NOEXCEPT;
#endif

	String(const String& rInput, uintptr_t uStart,
		uintptr_t uLength) BURGER_NOEXCEPT;

	String(const char* pInput) BURGER_NOEXCEPT;
	String(const char* pInput, uintptr_t uPadding) BURGER_NOEXCEPT;
	String(const uint16_t* pInput) BURGER_NOEXCEPT;
	String(const uint32_t* pInput) BURGER_NOEXCEPT;
	String(
		const char* pInput, uintptr_t uStart, uintptr_t uEnd) BURGER_NOEXCEPT;
	String(char iInput) BURGER_NOEXCEPT;
	String(char iInput, uintptr_t uFillSize) BURGER_NOEXCEPT;
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
	BURGER_INLINE char* data(void) BURGER_NOEXCEPT
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
	BURGER_INLINE uintptr_t size(void) const BURGER_NOEXCEPT
	{
		return m_uLength;
	}
	BURGER_INLINE uintptr_t capacity(void) const BURGER_NOEXCEPT
	{
		return m_uBufferSize;
	}
	BURGER_INLINE uint_t empty(void) const BURGER_NOEXCEPT
	{
		return !m_uLength;
	}

	eError BURGER_API assign(uintptr_t uLength, char iInput) BURGER_NOEXCEPT;
	eError BURGER_API assign(const String& rInput) BURGER_NOEXCEPT;
#if defined(BURGER_RVALUE_REFERENCES) || defined(DOXYGEN)
	void BURGER_API assign(String&& rInput) BURGER_NOEXCEPT;
#endif
	eError BURGER_API assign(const char* pInput) BURGER_NOEXCEPT;
	eError BURGER_API assign(
		const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
	eError BURGER_API assign(const uint16_t* pInput) BURGER_NOEXCEPT;
	eError BURGER_API assign(
		const uint16_t* pInput, uintptr_t uLength) BURGER_NOEXCEPT;

	eError BURGER_API assign_win437(const char* pInput) BURGER_NOEXCEPT;
	eError BURGER_API assign_win437(
		const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
	eError BURGER_API assign_win1252(const char* pInput) BURGER_NOEXCEPT;
	eError BURGER_API assign_win1252(
		const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
	eError BURGER_API assign_mac_roman_us(const char* pInput) BURGER_NOEXCEPT;
	eError BURGER_API assign_mac_roman_us(
		const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT;

	eError BURGER_API resize(uintptr_t uSize) BURGER_NOEXCEPT;
	eError BURGER_API reserve(uintptr_t uNewBufferSize) BURGER_NOEXCEPT;

	uintptr_t BURGER_API find(
		const String& rInput, uintptr_t uPosition = 0) const BURGER_NOEXCEPT;
	uintptr_t BURGER_API find(
		const char* pInput, uintptr_t uPosition = 0) const BURGER_NOEXCEPT;
	uintptr_t BURGER_API find(const char* pInput, uintptr_t uPosition,
		uintptr_t uInputSize) const BURGER_NOEXCEPT;
	uintptr_t BURGER_API find(
		char iInput, uintptr_t uPosition = 0) const BURGER_NOEXCEPT;

	String& operator=(char iInput) BURGER_NOEXCEPT;
	String& operator=(const char* pInput) BURGER_NOEXCEPT;
	String& operator=(const uint16_t* pInput) BURGER_NOEXCEPT;
	String& operator=(const String& rInput) BURGER_NOEXCEPT;

	String& operator+=(char iInput) BURGER_NOEXCEPT;
	String& operator+=(const char* pInput) BURGER_NOEXCEPT;
	String& operator+=(const uint16_t* pInput) BURGER_NOEXCEPT;
	String& operator+=(const String& rInput) BURGER_NOEXCEPT;

	BURGER_INLINE String operator()(
		uintptr_t uStart, uintptr_t uLength) const BURGER_NOEXCEPT
	{
		return String(*this, uStart, uLength);
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
	char BURGER_API Get(uintptr_t uWhere) const BURGER_NOEXCEPT;
	void BURGER_API Put(uintptr_t uWhere, char cInput) BURGER_NOEXCEPT;
	BURGER_INLINE String& ToLowercase(void) BURGER_NOEXCEPT
	{
		Burger::StringLowercase(this->m_pData);
		return *this;
	}
	BURGER_INLINE String& ToUppercase(void) BURGER_NOEXCEPT
	{
		Burger::StringUppercase(this->m_pData);
		return *this;
	}
	BURGER_INLINE String Lowercase(void) const BURGER_NOEXCEPT
	{
		return String(*this).ToLowercase();
	}
	BURGER_INLINE String Uppercase(void) const BURGER_NOEXCEPT
	{
		return String(*this).ToUppercase();
	}
	BURGER_INLINE uint_t operator!() const BURGER_NOEXCEPT
	{
		return (m_uLength == 0);
	}
	BURGER_INLINE uint_t is_valid(void) const BURGER_NOEXCEPT
	{
		return (m_uLength != 0);
	}
	void BURGER_API clear(void) BURGER_NOEXCEPT;
	uintptr_t BURGER_API copy(char* pOutput,
		uintptr_t uOutputSize = UINTPTR_MAX) const BURGER_NOEXCEPT;
	uintptr_t BURGER_API PCopy(
		uint8_t* pOutput, uintptr_t uOutputSize = 256) const BURGER_NOEXCEPT;
	uintptr_t BURGER_API Remove(
		uintptr_t uStart, uintptr_t uLength = UINTPTR_MAX) BURGER_NOEXCEPT;
	eError BURGER_API insert(uintptr_t uStart, const char* pInput,
		uintptr_t uInputSize) BURGER_NOEXCEPT;
	eError BURGER_API push_back(char iInput) BURGER_NOEXCEPT;
	char BURGER_API pop_back(void) BURGER_NOEXCEPT;
	eError BURGER_API append(const char* pInput) BURGER_NOEXCEPT;
	eError BURGER_API append(const uint16_t* pInput) BURGER_NOEXCEPT;
	eError BURGER_API append(const String& rInput) BURGER_NOEXCEPT;
	eError BURGER_API append(
		const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;
	String& BURGER_API Left(
		uintptr_t uNewLength, char iInput = ' ') BURGER_NOEXCEPT;
	String& BURGER_API Right(
		uintptr_t uNewLength, char iInput = ' ') BURGER_NOEXCEPT;
	uint_t BURGER_API starts_with(char iInput) BURGER_NOEXCEPT;
	uint_t BURGER_API starts_with(const char* pInput) BURGER_NOEXCEPT;
	uint_t BURGER_API starts_with(
		const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
	uint_t BURGER_API ends_with(char iInput) BURGER_NOEXCEPT;
	uint_t BURGER_API ends_with(const char* pInput) BURGER_NOEXCEPT;
	uint_t BURGER_API ends_with(
		const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT;

	BURGER_INLINE int Compare(const String& rInput) const BURGER_NOEXCEPT
	{
		return StringCompare(m_pData, rInput.c_str());
	}
	int BURGER_API Compare(const char* pInput) const BURGER_NOEXCEPT;
	int BURGER_API Compare(char iInput) const BURGER_NOEXCEPT;
	uintptr_t BURGER_API RemoveChars(const char* pInput) BURGER_NOEXCEPT;
	uintptr_t BURGER_API RemoveChar(char iInput) BURGER_NOEXCEPT;
	uintptr_t BURGER_API NormalizeLineFeeds(void) BURGER_NOEXCEPT;
	BURGER_INLINE uint_t GetBoolean(uint_t bDefault) const BURGER_NOEXCEPT
	{
		return AsciiToBoolean(m_pData, bDefault);
	}
	void BURGER_API SetTrueFalse(uint_t bInput) BURGER_NOEXCEPT;
	void BURGER_API SetYesNo(uint_t bInput) BURGER_NOEXCEPT;
	BURGER_INLINE uint_t GetWord(uint_t uDefault, uint_t uMin = 0,
		uint_t uMax = UINT32_MAX) const BURGER_NOEXCEPT
	{
		return AsciiToWord(m_pData, uDefault, uMin, uMax);
	}
	void BURGER_API SetWord(uint_t uInput) BURGER_NOEXCEPT;
	void BURGER_API SetWordHex(uint_t uInput) BURGER_NOEXCEPT;
	BURGER_INLINE int_t GetInt(int_t iDefault,
		int_t iMin = INT32_MIN,
		int_t iMax = INT32_MAX) const BURGER_NOEXCEPT
	{
		return AsciiToInteger(m_pData, iDefault, iMin, iMax);
	}
	void BURGER_API SetInt(int_t iInput) BURGER_NOEXCEPT;
	BURGER_INLINE float GetFloat(float fDefault) const BURGER_NOEXCEPT
	{
		return AsciiToFloat(m_pData, fDefault);
	}
	BURGER_INLINE float GetFloat(
		float fDefault, float fMin, float fMax) const BURGER_NOEXCEPT
	{
		return AsciiToFloat(m_pData, fDefault, fMin, fMax);
	}
	void BURGER_API SetFloat(float fValue) BURGER_NOEXCEPT;
	BURGER_INLINE double GetDouble(double dDefault) const BURGER_NOEXCEPT
	{
		return AsciiToDouble(m_pData, dDefault);
	}
	BURGER_INLINE double GetDouble(
		double dDefault, double dMin, double dMax) const BURGER_NOEXCEPT
	{
		return AsciiToDouble(m_pData, dDefault, dMin, dMax);
	}
	void BURGER_API SetDouble(double dValue) BURGER_NOEXCEPT;

	BURGER_INLINE friend uint_t operator==(
		String const& rInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) == 0);
	}
	BURGER_INLINE friend uint_t operator==(
		String const& rInput1, const char* pInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput1.Compare(pInput2) == 0);
	}
	BURGER_INLINE friend uint_t operator==(
		const char* pInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput2.Compare(pInput1) == 0);
	}
	BURGER_INLINE friend uint_t operator==(
		String const& rInput1, char cInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput1.Compare(cInput2) == 0);
	}
	BURGER_INLINE friend uint_t operator==(
		char cInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput2.Compare(cInput1) == 0);
	}
	BURGER_INLINE friend uint_t operator!=(
		String const& rInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) != 0);
	}
	BURGER_INLINE friend uint_t operator!=(
		String const& rInput1, const char* pInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput1.Compare(pInput2) != 0);
	}
	BURGER_INLINE friend uint_t operator!=(
		const char* pInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput2.Compare(pInput1) != 0);
	}
	BURGER_INLINE friend uint_t operator!=(
		String const& rInput1, char cInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput1.Compare(cInput2) != 0);
	}
	BURGER_INLINE friend uint_t operator!=(
		char cInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput2.Compare(cInput1) != 0);
	}
	BURGER_INLINE friend uint_t operator<(
		String const& rInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) < 0);
	}
	BURGER_INLINE friend uint_t operator<(
		String const& rInput1, const char* pInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput1.Compare(pInput2) < 0);
	}
	BURGER_INLINE friend uint_t operator<(
		const char* pInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput2.Compare(pInput1) > 0);
	}
	BURGER_INLINE friend uint_t operator<(
		String const& rInput1, char cInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput1.Compare(cInput2) < 0);
	}
	BURGER_INLINE friend uint_t operator<(
		char cInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput2.Compare(cInput1) > 0);
	}
	BURGER_INLINE friend uint_t operator<=(
		String const& rInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) <= 0);
	}
	BURGER_INLINE friend uint_t operator<=(
		String const& rInput1, const char* pInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput1.Compare(pInput2) <= 0);
	}
	BURGER_INLINE friend uint_t operator<=(
		const char* pInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput2.Compare(pInput1) >= 0);
	}
	BURGER_INLINE friend uint_t operator<=(
		String const& rInput1, char cInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput1.Compare(cInput2) <= 0);
	}
	BURGER_INLINE friend uint_t operator<=(
		char cInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput2.Compare(cInput1) >= 0);
	}
	BURGER_INLINE friend uint_t operator>(
		String const& rInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) > 0);
	}
	BURGER_INLINE friend uint_t operator>(
		String const& rInput1, const char* pInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput1.Compare(pInput2) > 0);
	}
	BURGER_INLINE friend uint_t operator>(
		const char* pInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput2.Compare(pInput1) < 0);
	}
	BURGER_INLINE friend uint_t operator>(
		String const& rInput1, char cInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput1.Compare(cInput2) > 0);
	}
	BURGER_INLINE friend uint_t operator>(
		char cInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput2.Compare(cInput1) < 0);
	}
	BURGER_INLINE friend uint_t operator>=(
		String const& rInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(
			Burger::StringCompare(rInput1.c_str(), rInput2.c_str()) >= 0);
	}
	BURGER_INLINE friend uint_t operator>=(
		String const& rInput1, const char* pInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput1.Compare(pInput2) >= 0);
	}
	BURGER_INLINE friend uint_t operator>=(
		const char* pInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput2.Compare(pInput1) <= 0);
	}
	BURGER_INLINE friend uint_t operator>=(
		String const& rInput1, char cInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput1.Compare(cInput2) >= 0);
	}
	BURGER_INLINE friend uint_t operator>=(
		char cInput1, String const& rInput2) BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(rInput2.Compare(cInput1) <= 0);
	}

private:
	void BURGER_API InitFormattedString(const char* pFormat,
		uintptr_t uArgCount, const ArgumentType** ppArgs) BURGER_NOEXCEPT;
	static uint_t BURGER_API FormattedAllocCallback(uint_t bError,
		uintptr_t uRequestedSize, void** ppOutputBuffer,
		void* pContext) BURGER_NOEXCEPT;
};
}
/* END */

#endif
