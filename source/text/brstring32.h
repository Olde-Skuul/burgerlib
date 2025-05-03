/***************************************

	C++ ANSI "C" string class for UTF32
	This is optimized for performance

	Copyright (c) 2023-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSTRING32_H__
#define __BRSTRING32_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __BRGLOBALMEMORYMANAGER_H__
#include "brglobalmemorymanager.h"
#endif

#ifndef __BRSTRINGFUNCTIONS_H__
#include "brstringfunctions.h"
#endif

/* BEGIN */
namespace Burger {
class String32 {
public:
	/** Ensures the structure is 128 bytes in size on all platforms */
	static const uintptr_t BUFFERSIZE = static_cast<uintptr_t>(
		(128 - (sizeof(uint32_t*) + sizeof(uintptr_t))) / sizeof(uint32_t));

private:
	/** Pointer to the string */
	uint32_t* m_pData;
	/** Length of the string */
	uintptr_t m_uLength;
	/** Temp preallocated buffer for most strings Structure size is 128 bytes */
	uint32_t m_Raw[BUFFERSIZE];

public:
	String32(void) BURGER_NOEXCEPT: m_pData(m_Raw), m_uLength(0)
	{
		m_Raw[0] = 0;
	}
	String32(const String32& rInput);
	String32(
		const String32& rInput, uintptr_t uStart, uintptr_t uEnd = UINTPTR_MAX);
	String32(const char* pInput) BURGER_NOEXCEPT;
	String32(const char* pInput, uintptr_t uPadding);
	String32(const uint16_t* pInput);
	String32(const uint32_t* pInput);
	~String32()
	{
		if (m_pData != m_Raw) {
			free_memory(m_pData);
		}
	}

	BURGER_INLINE operator uint32_t*() BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE operator const uint32_t*() const BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE const uint32_t* c_str() const BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE uint32_t* c_str() BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE const uint32_t* data(void) const BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE uint32_t* data(void) BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE operator uint32_t() const BURGER_NOEXCEPT
	{
		return m_pData[0];
	}
	BURGER_INLINE uintptr_t length(void) const BURGER_NOEXCEPT
	{
		return m_uLength;
	}
	BURGER_INLINE uint_t empty(void) const BURGER_NOEXCEPT
	{
		return !m_uLength;
	}
	BURGER_INLINE uintptr_t GetLength(void) const BURGER_NOEXCEPT
	{
		return m_uLength;
	}
	BURGER_INLINE uint32_t* GetPtr() BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE const uint32_t* GetPtr() const BURGER_NOEXCEPT
	{
		return m_pData;
	}
	eError BURGER_API assign(const char* pInput) BURGER_NOEXCEPT;
	eError BURGER_API assign(const uint32_t* pInput) BURGER_NOEXCEPT;
	eError BURGER_API SetBufferSize(uintptr_t uSize) BURGER_NOEXCEPT;
	eError BURGER_API resize(uintptr_t uSize) BURGER_NOEXCEPT
	{
		return SetBufferSize(uSize);
	}

	String32& operator=(const String32& rInput);
	String32& operator=(const uint32_t* pInput);
	String32& operator=(const char* pInput);
	String32& operator=(char cInput);
	BURGER_INLINE String32 operator()(
		uintptr_t uStart, uintptr_t uEnd) const BURGER_NOEXCEPT
	{
		return String32(*this, uStart, uEnd);
	}
	BURGER_INLINE uint32_t& operator()(uintptr_t uInput) BURGER_NOEXCEPT
	{
		if (uInput >= m_uLength) {
			m_Raw[BUFFERSIZE - 1] = 0;
			return m_Raw[BUFFERSIZE - 1];
		}
		return m_pData[uInput];
	}
	BURGER_INLINE uint32_t const& operator()(
		uintptr_t uInput) const BURGER_NOEXCEPT
	{
		if (uInput >= m_uLength) {
			return g_EmptyString32[0];
		}
		return m_pData[uInput];
	}
	BURGER_INLINE uint32_t& operator[](uintptr_t uInput) BURGER_NOEXCEPT
	{
		if (uInput >= m_uLength) {
			m_Raw[BUFFERSIZE - 1] = 0;
			return m_Raw[BUFFERSIZE - 1];
		}
		return m_pData[uInput];
	}
	BURGER_INLINE uint32_t const& operator[](
		uintptr_t uInput) const BURGER_NOEXCEPT
	{
		if (uInput >= m_uLength) {
			return g_EmptyString32[0];
		}
		return m_pData[uInput];
	}
	BURGER_INLINE uint_t operator!() const BURGER_NOEXCEPT
	{
		return (m_uLength == 0);
	}
	BURGER_INLINE uint_t is_valid(void) const BURGER_NOEXCEPT
	{
		return (m_uLength != 0);
	}
	void clear(void) BURGER_NOEXCEPT;
};
}
/* END */

#endif
