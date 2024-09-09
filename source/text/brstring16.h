/***************************************

    C++ ANSI "C" string class for UTF16
    This is optimized for performance

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSTRING16_H__
#define __BRSTRING16_H__

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
class String16 {
public:
    /** Ensures the structure is 128 bytes in size on all platforms */
    static const uintptr_t BUFFERSIZE = static_cast<uintptr_t>(
        (128 - (sizeof(uint16_t*) + sizeof(uintptr_t))) / sizeof(uint16_t));

private:
    /** Pointer to the string */
    uint16_t* m_pData;
    /** Length of the string */
    uintptr_t m_uLength;
    /** Temp preallocated buffer for most strings Structure size is 128 bytes */
    uint16_t m_Raw[BUFFERSIZE];

public:
    String16(void) BURGER_NOEXCEPT: m_pData(m_Raw), m_uLength(0)
    {
        m_Raw[0] = 0;
    }
    String16(const String16& rInput);
    String16(
        const String16& rInput, uintptr_t uStart, uintptr_t uEnd = UINTPTR_MAX);
    String16(const char* pInput) BURGER_NOEXCEPT;
    String16(const char* pInput, uintptr_t uPadding);
    String16(const uint16_t* pInput);
    String16(const uint32_t* pInput);
    ~String16()
    {
        if (m_pData != m_Raw) {
            Free(m_pData);
        }
    }

    BURGER_INLINE operator uint16_t*() BURGER_NOEXCEPT
    {
        return m_pData;
    }
    BURGER_INLINE operator const uint16_t *() const BURGER_NOEXCEPT
    {
        return m_pData;
    }
    BURGER_INLINE const uint16_t* c_str() const BURGER_NOEXCEPT
    {
        return m_pData;
    }
    BURGER_INLINE uint16_t* c_str() BURGER_NOEXCEPT
    {
        return m_pData;
    }
	BURGER_INLINE const uint16_t *data(void) const BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE uint16_t *data(void) BURGER_NOEXCEPT
	{
		return m_pData;
	}
    BURGER_INLINE operator uint16_t() const BURGER_NOEXCEPT
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
    BURGER_INLINE uint16_t* GetPtr() BURGER_NOEXCEPT
    {
        return m_pData;
    }
    BURGER_INLINE const uint16_t* GetPtr() const BURGER_NOEXCEPT
    {
        return m_pData;
    }
    eError BURGER_API assign(const char* pInput) BURGER_NOEXCEPT;
    eError BURGER_API assign(const uint16_t* pInput) BURGER_NOEXCEPT;
    eError BURGER_API SetBufferSize(uintptr_t uSize) BURGER_NOEXCEPT;
	eError BURGER_API resize(uintptr_t uSize) BURGER_NOEXCEPT { return SetBufferSize(uSize); }

    String16& operator=(const String16& rInput);
    String16& operator=(const uint16_t* pInput);
    String16& operator=(const char* pInput);
    String16& operator=(char cInput);
    BURGER_INLINE String16 operator()(
        uintptr_t uStart, uintptr_t uEnd) const BURGER_NOEXCEPT
    {
        return String16(*this, uStart, uEnd);
    }
    BURGER_INLINE uint16_t& operator()(uintptr_t uInput) BURGER_NOEXCEPT
    {
        if (uInput >= m_uLength) {
            m_Raw[BUFFERSIZE - 1] = 0;
            return m_Raw[BUFFERSIZE - 1];
        }
        return m_pData[uInput];
    }
    BURGER_INLINE uint16_t const& operator()(
        uintptr_t uInput) const BURGER_NOEXCEPT
    {
        if (uInput >= m_uLength) {
            return g_EmptyString16[0];
        }
        return m_pData[uInput];
    }
    BURGER_INLINE uint16_t& operator[](uintptr_t uInput) BURGER_NOEXCEPT
    {
        if (uInput >= m_uLength) {
            m_Raw[BUFFERSIZE - 1] = 0;
            return m_Raw[BUFFERSIZE - 1];
        }
        return m_pData[uInput];
    }
    BURGER_INLINE uint16_t const& operator[](
        uintptr_t uInput) const BURGER_NOEXCEPT
    {
        if (uInput >= m_uLength) {
            return g_EmptyString16[0];
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
