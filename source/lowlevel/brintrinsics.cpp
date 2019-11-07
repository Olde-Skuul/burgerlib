/***************************************

    Compiler intrinsics.

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brintrinsics.h"

/*! ************************************

    \brief Swap endian of a 64 bit integer.

    64 bit operations for endian swap are specialized on different platforms
    since some forms require the return value to be in a structure.

    \param uInput 64 integer to swap endian.
    \return Input with all 8 bytes reversed.

***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_68K)) || defined(DOXYGEN)

// clang-format off
BURGER_ASM uint64_t _swapendian64(uint64_t uInput)
{
    // Get the pointer for the return value
    movea.l 4(a7), a0
    // Get the input value
    move.l  8(a7), d0
    move.l  12(a7), d1
    // Swap the endian
    ror.w #8, d0
    ror.w #8, d1
    swap d0
    swap d1
    ror.w #8, d0
    ror.w #8, d1
    // Save the result, swapping the halves.
    move.l d1, 0(a0)
    move.l d0, 4(a0)
    rts
}
// clang-format on

#elif defined(BURGER_APPLE_SC) && defined(BURGER_68K)

// The SC assembler doesn't allow for "inline assembly", so
// we fake it by declaring the byte stream and then call it with
// a C stub.

// clang-format off
#pragma parameter _swapendian64_asm(__A0, __A1)
extern "C" void _swapendian64_asm(uint64_t* pDest, const uint64_t* pInput) = {
    0x2029, 0x0000, // move.l 0(a1),d0
    0x2229, 0x0004, // move.l 4(a1),d1
    0xE058,         // ror.w #8,d0
    0xE059,         // ror.w #8,d1
    0x4840,         // swap d0
    0x4841,         // swap d1
    0xE058,         // ror.w #8,d0
    0xE059,         // ror.w #8,d1
    0x2141, 0x0000, // move.l d1,0(a0)
    0x2140, 0x0004  // move.l d0,4(a0)
};
// clang-format on

uint64_t _swapendian64(const uint64_t& rInput)
{
    // Call the assembly fragment
    uint64_t temp;
    _swapendian64_asm(&temp, &rInput);
    return temp;
}
#endif
