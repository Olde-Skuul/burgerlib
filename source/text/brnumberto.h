/***************************************

	Number to ASCII functions

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRNUMBERTO_H__
#define __BRNUMBERTO_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRALGORITHM_H__
#include "bralgorithm.h"
#endif

/* BEGIN */
namespace Burger {

extern char* BURGER_API NumberToAsciiHex(
	char* pOutput, uint8_t uInput) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAsciiHex(
	char* pOutput, uint16_t uInput) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAsciiHex(
	char* pOutput, uint32_t uInput) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAsciiHex(
	char* pOutput, uint64_t uInput) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAsciiHex(
	char* pOutput, float fInput) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAsciiHex(
	char* pOutput, double dInput) BURGER_NOEXCEPT;

template<class T>
BURGER_INLINE char* NumberToAsciiHex(char* pOutput, T input) BURGER_NOEXCEPT
{
	BURGER_STATIC_ASSERT(is_integral<T>::value);
	typedef typename conditional<sizeof(T) == 1, uint8_t,
		typename conditional<sizeof(T) == 2, uint16_t,
			typename conditional<sizeof(T) == 4, uint32_t,
				uint64_t>::type>::type>::type _tSwitch;
	return NumberToAsciiHex(pOutput, static_cast<_tSwitch>(input));
}

extern char* BURGER_API NumberToAsciiHex(
	char* pOutput, uint32_t uInput, uint_t uDigits) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAsciiHex(
	char* pOutput, uint64_t uInput, uint_t uDigits) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAsciiHex(
	char* pOutput, float fInput, uint_t uDigits) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAsciiHex(
	char* pOutput, double dInput, uint_t uDigits) BURGER_NOEXCEPT;

BURGER_INLINE char* BURGER_API NumberToAsciiHex(
	char* pOutput, uint8_t uInput, uint_t uDigits) BURGER_NOEXCEPT
{
	if (!(uDigits & 0xFFU)) {
		uDigits += 2;
	}
	return NumberToAsciiHex(pOutput, static_cast<uint32_t>(uInput), uDigits);
}

BURGER_INLINE char* BURGER_API NumberToAsciiHex(
	char* pOutput, uint16_t uInput, uint_t uDigits) BURGER_NOEXCEPT
{
	if (!(uDigits & 0xFFU)) {
		uDigits += 4;
	}
	return NumberToAsciiHex(pOutput, static_cast<uint32_t>(uInput), uDigits);
}

template<class T>
BURGER_INLINE char* NumberToAsciiHex(
	char* pOutput, T input, uint_t uDigits) BURGER_NOEXCEPT
{
	BURGER_STATIC_ASSERT(is_integral<T>::value);
	typedef typename conditional<sizeof(T) == 1, uint8_t,
		typename conditional<sizeof(T) == 2, uint16_t,
			typename conditional<sizeof(T) == 4, uint32_t,
				uint64_t>::type>::type>::type _tSwitch;
	return NumberToAsciiHex(pOutput, static_cast<_tSwitch>(input), uDigits);
}

extern char* BURGER_API NumberToAscii(
	char* pOutput, uint32_t uInput, uint_t uDigits) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAscii(
	char* pOutput, int32_t iInput, uint_t uDigits) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAscii(
	char* pOutput, uint64_t uInput, uint_t uDigits) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAscii(
	char* pOutput, int64_t iInput, uint_t uDigits) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAscii(
	char* pOutput, float fInput, uint_t uDigits = 0) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAscii(
	char* pOutput, double dInput, uint_t uDigits = 0) BURGER_NOEXCEPT;

#if !defined(DOXYGEN)
template<typename T, uintptr_t size, bool is_integral>
struct NumberToAscii_Dispatch {
	BURGER_INLINE char* operator()(char*, T, uint_t uDigits) BURGER_NOEXCEPT
	{
		// Only integral values are valid.
		BURGER_STATIC_ASSERT(is_integral);
		return nullptr;
	}
};

template<typename T>
struct NumberToAscii_Dispatch<T, 1, true> {
	BURGER_INLINE char* operator()(
		char* pOutput, T input, uint_t uDigits) BURGER_NOEXCEPT
	{
		if (!(uDigits & 0xFFU)) {
			uDigits += 3;
		}
		return NumberToAscii(pOutput,
			static_cast<typename conditional<is_signed<T>::value, int32_t,
				uint32_t>::type>(input),
			uDigits);
	}
};

template<typename T>
struct NumberToAscii_Dispatch<T, 2, true> {
	BURGER_INLINE char* operator()(
		char* pOutput, T input, uint_t uDigits) BURGER_NOEXCEPT
	{
		if (!(uDigits & 0xFFU)) {
			uDigits += 5;
		}
		return NumberToAscii(pOutput,
			static_cast<typename conditional<is_signed<T>::value, int32_t,
				uint32_t>::type>(input),
			uDigits);
	}
};

template<typename T>
struct NumberToAscii_Dispatch<T, 4, true> {
	BURGER_INLINE char* operator()(
		char* pOutput, T input, uint_t uDigits) BURGER_NOEXCEPT
	{
		return NumberToAscii(pOutput,
			static_cast<typename conditional<is_signed<T>::value, int32_t,
				uint32_t>::type>(input),
			uDigits);
	}
};

template<typename T>
struct NumberToAscii_Dispatch<T, 8, true> {
	BURGER_INLINE char* operator()(
		char* pOutput, T input, uint_t uDigits) BURGER_NOEXCEPT
	{
		return NumberToAscii(pOutput,
			static_cast<typename conditional<is_signed<T>::value, int64_t,
				uint64_t>::type>(input),
			uDigits);
	}
};
#endif

template<class T>
BURGER_INLINE char* NumberToAscii(char* pOutput, T input) BURGER_NOEXCEPT
{
	return NumberToAscii_Dispatch<T, sizeof(T), is_integral<T>::value>()(
		pOutput, input, 0);
}

template<class T>
BURGER_INLINE char* NumberToAscii(
	char* pOutput, T input, uint_t uDigits) BURGER_NOEXCEPT
{
	return NumberToAscii_Dispatch<T, sizeof(T), is_integral<T>::value>()(
		pOutput, input, uDigits);
}

extern uint_t BURGER_API NumberStringLength(uint32_t uInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API NumberStringLength(int32_t iInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API NumberStringLength(uint64_t uInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API NumberStringLength(int64_t iInput) BURGER_NOEXCEPT;

template<class T>
BURGER_INLINE uint_t NumberStringLength(T input) BURGER_NOEXCEPT
{
	BURGER_STATIC_ASSERT(is_integral<T>::value);
	typedef typename conditional<sizeof(T) <= 4,
		typename conditional<is_signed<T>::value, int32_t, uint32_t>::type,
		typename conditional<is_signed<T>::value, int64_t,
			uint64_t>::type>::type _tSwitch;
	return NumberStringLength(static_cast<_tSwitch>(input));
}

extern uint_t BURGER_API NumberHexStringLength(uint32_t uInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API NumberHexStringLength(uint64_t uInput) BURGER_NOEXCEPT;

template<class T>
BURGER_INLINE uint_t NumberHexStringLength(T input) BURGER_NOEXCEPT
{
	BURGER_STATIC_ASSERT(is_integral<T>::value);
	typedef
		typename conditional<sizeof(T) <= 4, uint32_t, uint64_t>::type _tSwitch;
	return NumberHexStringLength(static_cast<_tSwitch>(input));
}

extern uint_t BURGER_API NumberOctalStringLength(
	uint32_t uInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API NumberOctalStringLength(
	uint64_t uInput) BURGER_NOEXCEPT;

template<class T>
BURGER_INLINE uint_t NumberOctalStringLength(T input) BURGER_NOEXCEPT
{
	BURGER_STATIC_ASSERT(is_integral<T>::value);
	typedef
		typename conditional<sizeof(T) <= 4, uint32_t, uint64_t>::type _tSwitch;
	return NumberOctalStringLength(static_cast<_tSwitch>(input));
}
}

/* END */

#endif
