/***************************************

	Number String Manager in hexadecimal

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRNUMBERSTRINGHEX_H__
#define __BRNUMBERSTRINGHEX_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSTRINGFUNCTIONS_H__
#include "brstringfunctions.h"
#endif

#ifndef __BRNUMBERTO_H__
#include "brnumberto.h"
#endif

/* BEGIN */
namespace Burger {
class NumberStringHex {
private:
	char m_Data[24]; ///< String buffer
public:
	BURGER_INLINE NumberStringHex() BURGER_NOEXCEPT
	{
		m_Data[0] = 0;
	}

	template<class T>
	BURGER_INLINE NumberStringHex(T input) BURGER_NOEXCEPT
	{
		NumberToAsciiHex(m_Data, input);
	}

	template<class T>
	BURGER_INLINE NumberStringHex(T uInput, uint_t uFormat) BURGER_NOEXCEPT
	{
		NumberToAsciiHex(m_Data, uInput, uFormat);
	}

	template<class T>
	BURGER_INLINE NumberStringHex& operator=(T input) BURGER_NOEXCEPT
	{
		NumberToAsciiHex(m_Data, input);
		return *this;
	}

	BURGER_INLINE operator const char*() const BURGER_NOEXCEPT
	{
		return m_Data;
	}
	BURGER_NODISCARD BURGER_INLINE const char* data(void) const BURGER_NOEXCEPT
	{
		return m_Data;
	}
	BURGER_NODISCARD BURGER_INLINE const char* c_str(void) const BURGER_NOEXCEPT
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
	BURGER_NODISCARD BURGER_INLINE uint_t empty(void) const BURGER_NOEXCEPT
	{
		return !m_Data[0];
	}
	BURGER_NODISCARD BURGER_INLINE uintptr_t length(void) const BURGER_NOEXCEPT
	{
		return StringLength(m_Data);
	}
	BURGER_NODISCARD BURGER_INLINE uintptr_t capacity(
		void) const BURGER_NOEXCEPT
	{
		return sizeof(m_Data);
	}
};

}
/* END */

#endif
