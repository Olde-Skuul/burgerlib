/***************************************

    Simple structures

    Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSTRUCTS_H__
#define __BRSTRUCTS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

typedef uint8_t Float80Bit[10]; ///< 80 bit float (Extended)

struct uint32_float_t {
    union {
        uint32_t w; ///< Value in binary
        float f;    ///< Value as a 32 bit float
    };
    BURGER_INLINE operator float() const BURGER_NOEXCEPT
    {
        return f;
    }
    BURGER_INLINE uint32_float_t & operator=(float fInput) BURGER_NOEXCEPT
    {
        f = fInput;
        return *this;
    }
    BURGER_INLINE uint32_t GetWord(void) const BURGER_NOEXCEPT
    {
        return w;
    }
    BURGER_INLINE void SetWord(uint32_t uInput) BURGER_NOEXCEPT
    {
        w = uInput;
    }
};

struct uint64_double_t {
    union {
        /** Value in binary */
        uint64_t w;
        /** Value as a 64 bit float */
        double d;
        /** Array of 2 32 bit words that map on the 64 bit value */
        uint32_t w32[2];
    };
    BURGER_INLINE operator double() const BURGER_NOEXCEPT
    {
        return d;
    }
    BURGER_INLINE uint64_double_t & operator=(double dInput) BURGER_NOEXCEPT
    {
        d = dInput;
        return *this;
    }
    BURGER_INLINE uint64_t GetWord(void) const BURGER_NOEXCEPT
    {
        return w;
    }
    BURGER_INLINE void SetWord(uint64_t uInput) BURGER_NOEXCEPT
    {
        w = uInput;
    }
    BURGER_INLINE uint32_t GetLowWord(void) const BURGER_NOEXCEPT
    {
        return w32[BURGER_ENDIANINDEX_LOW];
    }
    BURGER_INLINE uint32_t GetHiWord(void) const BURGER_NOEXCEPT
    {
        return w32[BURGER_ENDIANINDEX_HIGH];
    }
    BURGER_INLINE void SetLowWord(uint32_t uInput) BURGER_NOEXCEPT
    {
        w32[BURGER_ENDIANINDEX_LOW] = uInput;
    }
    BURGER_INLINE void SetHiWord(uint32_t uInput) BURGER_NOEXCEPT
    {
        w32[BURGER_ENDIANINDEX_HIGH] = uInput;
    }
};
}
/* END */

#endif
