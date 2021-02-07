/***************************************

    Number to ASCII functions

    Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#if !defined(DOXYGEN)
template<typename T, uintptr_t size, bool is_integral>
struct NumberToAsciiHex_Dispatch {
    BURGER_INLINE char* operator()(char*, T) BURGER_NOEXCEPT
    {
        // Only integral values are valid.
        BURGER_STATIC_ASSERT(is_integral);
        return nullptr;
    }
};

template<typename T>
struct NumberToAsciiHex_Dispatch<T, 1, true> {
    BURGER_INLINE char* operator()(char* pOutput, T input) BURGER_NOEXCEPT
    {
        return NumberToAsciiHex(pOutput, static_cast<uint8_t>(input));
    }
};

template<typename T>
struct NumberToAsciiHex_Dispatch<T, 2, true> {
    BURGER_INLINE char* operator()(char* pOutput, T input) BURGER_NOEXCEPT
    {
        return NumberToAsciiHex(pOutput, static_cast<uint16_t>(input));
    }
};

template<typename T>
struct NumberToAsciiHex_Dispatch<T, 4, true> {
    BURGER_INLINE char* operator()(char* pOutput, T input) BURGER_NOEXCEPT
    {
        return NumberToAsciiHex(pOutput, static_cast<uint32_t>(input));
    }
};

template<typename T>
struct NumberToAsciiHex_Dispatch<T, 8, true> {
    BURGER_INLINE char* operator()(char* pOutput, T input) BURGER_NOEXCEPT
    {
        return NumberToAsciiHex(pOutput, static_cast<uint64_t>(input));
    }
};
#endif

template<class T>
BURGER_INLINE char* NumberToAsciiHex(char* pOutput, T input) BURGER_NOEXCEPT
{
    return NumberToAsciiHex_Dispatch<T, sizeof(T), is_integral<T>::value>()(
        pOutput, input);
}

extern char* BURGER_API NumberToAsciiHex(
    char* pOutput, uint32_t uInput, uint_t uDigits) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAsciiHex(
    char* pOutput, uint64_t uInput, uint_t uDigits) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAsciiHex(
    char* pOutput, float fInput, uint_t uDigits) BURGER_NOEXCEPT;
extern char* BURGER_API NumberToAsciiHex(
    char* pOutput, double dInput, uint_t uDigits) BURGER_NOEXCEPT;

#if !defined(DOXYGEN)
template<typename T, uintptr_t size, bool is_integral>
struct NumberToAsciiHex_Dispatch2 {
    BURGER_INLINE char* operator()(char*, T, uint_t uDigits) BURGER_NOEXCEPT
    {
        // Only integral values are valid.
        BURGER_STATIC_ASSERT(is_integral);
        return nullptr;
    }
};

template<typename T>
struct NumberToAsciiHex_Dispatch2<T, 1, true> {
    BURGER_INLINE char* operator()(
        char* pOutput, T input, uint_t uDigits) BURGER_NOEXCEPT
    {
        if (!(uDigits & 0xFFU)) {
            uDigits += 2;
        }
        return NumberToAsciiHex(pOutput, static_cast<uint32_t>(input), uDigits);
    }
};

template<typename T>
struct NumberToAsciiHex_Dispatch2<T, 2, true> {
    BURGER_INLINE char* operator()(
        char* pOutput, T input, uint_t uDigits) BURGER_NOEXCEPT
    {
        if (!(uDigits & 0xFFU)) {
            uDigits += 4;
        }
        return NumberToAsciiHex(pOutput, static_cast<uint32_t>(input), uDigits);
    }
};

template<typename T>
struct NumberToAsciiHex_Dispatch2<T, 4, true> {
    BURGER_INLINE char* operator()(
        char* pOutput, T input, uint_t uDigits) BURGER_NOEXCEPT
    {
        return NumberToAsciiHex(pOutput, static_cast<uint32_t>(input), uDigits);
    }
};

template<typename T>
struct NumberToAsciiHex_Dispatch2<T, 8, true> {
    BURGER_INLINE char* operator()(
        char* pOutput, T input, uint_t uDigits) BURGER_NOEXCEPT
    {
        return NumberToAsciiHex(pOutput, static_cast<uint64_t>(input), uDigits);
    }
};
#endif

template<class T>
BURGER_INLINE char* NumberToAsciiHex(
    char* pOutput, T input, uint_t uDigits) BURGER_NOEXCEPT
{
    return NumberToAsciiHex_Dispatch2<T, sizeof(T), is_integral<T>::value>()(
        pOutput, input, uDigits);
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

#if !defined(DOXYGEN)
template<typename T, bool is_32bit, bool is_integral>
struct NumberStringLength_Dispatch {
    BURGER_INLINE uint_t operator()(T) BURGER_NOEXCEPT
    {
        // Only integral values are valid.
        BURGER_STATIC_ASSERT(is_integral);
        return 0;
    }
};

template<typename T>
struct NumberStringLength_Dispatch<T, true, true> {
    BURGER_INLINE uint_t operator()(T input) BURGER_NOEXCEPT
    {
        return NumberStringLength(static_cast<
            typename conditional<is_signed<T>::value, int32_t, uint32_t>::type>(
            input));
    }
};

template<typename T>
struct NumberStringLength_Dispatch<T, false, true> {
    BURGER_INLINE uint_t operator()(T input) BURGER_NOEXCEPT
    {
        return NumberStringLength(static_cast<
            typename conditional<is_signed<T>::value, int64_t, uint64_t>::type>(
            input));
    }
};
#endif

template<class T>
BURGER_INLINE uint_t NumberStringLength(T input) BURGER_NOEXCEPT
{
    return NumberStringLength_Dispatch<T, sizeof(T) <= 4,
        is_integral<T>::value>()(input);
}

extern uint_t BURGER_API NumberHexStringLength(uint32_t uInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API NumberHexStringLength(uint64_t uInput) BURGER_NOEXCEPT;

#if !defined(DOXYGEN)
template<typename T, bool is_32bit, bool is_integral>
struct NumberHexStringLength_Dispatch {
    BURGER_INLINE uint_t operator()(T) BURGER_NOEXCEPT
    {
        // Only integral values are valid.
        BURGER_STATIC_ASSERT(is_integral);
        return 0;
    }
};

template<typename T>
struct NumberHexStringLength_Dispatch<T, true, true> {
    BURGER_INLINE uint_t operator()(T input) BURGER_NOEXCEPT
    {
        return NumberHexStringLength(static_cast<uint32_t>(input));
    }
};

template<typename T>
struct NumberHexStringLength_Dispatch<T, false, true> {
    BURGER_INLINE uint_t operator()(T input) BURGER_NOEXCEPT
    {
        return NumberHexStringLength(static_cast<uint64_t>(input));
    }
};
#endif

template<class T>
BURGER_INLINE uint_t NumberHexStringLength(T input) BURGER_NOEXCEPT
{
    return NumberHexStringLength_Dispatch<T, sizeof(T) <= 4,
        is_integral<T>::value>()(input);
}

extern uint_t BURGER_API NumberOctalStringLength(
    uint32_t uInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API NumberOctalStringLength(
    uint64_t uInput) BURGER_NOEXCEPT;

#if !defined(DOXYGEN)
template<typename T, bool is_32bit, bool is_integral>
struct NumberOctalStringLength_Dispatch {
    BURGER_INLINE uint_t operator()(T) BURGER_NOEXCEPT
    {
        // Only integral values are valid.
        BURGER_STATIC_ASSERT(is_integral);
        return 0;
    }
};

template<typename T>
struct NumberOctalStringLength_Dispatch<T, true, true> {
    BURGER_INLINE uint_t operator()(T input) BURGER_NOEXCEPT
    {
        return NumberOctalStringLength(static_cast<uint32_t>(input));
    }
};

template<typename T>
struct NumberOctalStringLength_Dispatch<T, false, true> {
    BURGER_INLINE uint_t operator()(T input) BURGER_NOEXCEPT
    {
        return NumberOctalStringLength(static_cast<uint64_t>(input));
    }
};
#endif

template<class T>
BURGER_INLINE uint_t NumberOctalStringLength(T input) BURGER_NOEXCEPT
{
    return NumberOctalStringLength_Dispatch<T, sizeof(T) <= 4,
        is_integral<T>::value>()(input);
}
}

/* END */

#endif
