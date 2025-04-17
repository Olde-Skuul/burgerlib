/***************************************

	Number String Manager

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRNUMBERSTRING_H__
#define __BRNUMBERSTRING_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSTRINGFUNCTIONS_H__
#include "brstringfunctions.h"
#endif

/* BEGIN */
namespace Burger {
class NumberString {
private:
	/** String buffer */
	char m_Data[32];

public:
	NumberString() BURGER_NOEXCEPT
	{
		m_Data[0] = 0;
	}
	NumberString(uint32_t uInput) BURGER_NOEXCEPT;
	NumberString(uint32_t uInput, uint_t uFormat) BURGER_NOEXCEPT;
	NumberString(int32_t iInput) BURGER_NOEXCEPT;
	NumberString(int32_t iInput, uint_t uFormat) BURGER_NOEXCEPT;
	NumberString(uint64_t uInput) BURGER_NOEXCEPT;
	NumberString(uint64_t uInput, uint_t uFormat) BURGER_NOEXCEPT;
	NumberString(int64_t iInput) BURGER_NOEXCEPT;
	NumberString(int64_t iInput, uint_t uFormat) BURGER_NOEXCEPT;
	NumberString(float fInput) BURGER_NOEXCEPT;
	NumberString(double dInput) BURGER_NOEXCEPT;

#if defined(BURGER_HAS_WCHAR_T) || defined(DOXYGEN)
	NumberString(wchar_t uInput) BURGER_NOEXCEPT;
	NumberString(wchar_t uInput, uint_t uFormat) BURGER_NOEXCEPT;
#endif

#if defined(BURGER_INT_NOT_IN_STDINT) || defined(DOXYGEN)
	NumberString(unsigned int uInput) BURGER_NOEXCEPT;
	NumberString(unsigned int uInput, uint_t uFormat) BURGER_NOEXCEPT;
#endif

#if defined(BURGER_LONG_NOT_IN_STDINT) || defined(DOXYGEN)
	NumberString(unsigned long uInput) BURGER_NOEXCEPT;
	NumberString(unsigned long uInput, uint_t uFormat) BURGER_NOEXCEPT;
#endif

	NumberString& operator=(uint32_t uInput) BURGER_NOEXCEPT;
	NumberString& operator=(int32_t iInput) BURGER_NOEXCEPT;
	NumberString& operator=(uint64_t uInput) BURGER_NOEXCEPT;
	NumberString& operator=(int64_t iInput) BURGER_NOEXCEPT;
	NumberString& operator=(float fInput) BURGER_NOEXCEPT;
	NumberString& operator=(double dInput) BURGER_NOEXCEPT;
	BURGER_INLINE operator const char*() const BURGER_NOEXCEPT
	{
		return m_Data;
	}
	BURGER_INLINE const char* c_str(void) const BURGER_NOEXCEPT
	{
		return m_Data;
	}
	BURGER_INLINE char operator[](uintptr_t uIndex) const BURGER_NOEXCEPT
	{
		return m_Data[uIndex];
	}
	BURGER_INLINE const char& operator[](uintptr_t uIndex) BURGER_NOEXCEPT
	{
		return m_Data[uIndex];
	}
	BURGER_INLINE void clear(void) BURGER_NOEXCEPT
	{
		m_Data[0] = 0;
	}
	BURGER_INLINE uint_t empty(void) const BURGER_NOEXCEPT
	{
		return !m_Data[0];
	}
	BURGER_INLINE uintptr_t size(void) const BURGER_NOEXCEPT
	{
		return string_length(m_Data);
	}
	BURGER_INLINE uintptr_t length(void) const BURGER_NOEXCEPT
	{
		return string_length(m_Data);
	}
};
}
/* END */

#endif
