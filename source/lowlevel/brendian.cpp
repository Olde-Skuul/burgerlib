/***************************************

    Handle endian swapping

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brendian.h"
#include "brfloatingpoint.h"

/*! ************************************

    \struct Burger::SwapEndian
    \brief Swap the byte order of 16, 32 and 64 bit values.

    When reading data from machines that are using different microprocessors,
    the byte order could be reversed. These functions will swap the byte order.

    \note These functions will always swap the bytes, \ref LittleEndian
    or \ref BigEndian to let the compiler determine whether byte
    swapping is needed.

    \sa \ref NativeEndian, \ref LittleEndian and \ref BigEndian

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(char iInput)
    \brief Load an 8 bit signed value to memory with byte alignment.

    \note Since 8 bit values can't be byte swapped, this function only exists
    for completeness sake

    \param iInput An 8 bit value.
    \return The 8 bit signed value.

    \sa NativeEndian::Load(char)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(uint8_t uInput)
    \brief Load an 8 bit unsigned value to memory with byte alignment.

    \note Since 8 bit values can't be byte swapped, this function only exists
    for completeness sake

    \param uInput An 8 bit value.
    \return The 8 bit unsigned value.

    \sa NativeEndian::Load(uint8_t)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(int8_t iInput)
    \brief Load an 8 bit signed value to memory with byte alignment.

    \note Since 8 bit values can't be byte swapped, this function only exists
    for completeness sake

    \param iInput An 8 bit value.
    \return The 8 bit signed value.

    \sa NativeEndian::Load(int8_t)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(uint16_t uInput)
    \brief Reverse the endian of a 16 bit integer

    Given a 16 bit value in an integer register, swap the bytes so that 0x1234
    becomes 0x3412

    \param uInput Value to endian convert
    \return The input with the bytes swapped, no errors are possible.

    \sa NativeEndian::Load(uint16_t)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(int16_t iInput)
    \brief Reverse the endian of a 16 bit signed integer.

    Given a 16 bit value in an integer register, swap the bytes so that 0x1234
    becomes 0x3412.

    \param iInput = Value to endian convert

    \return The input with the bytes swapped, no errors are possible.

    \note This function is inlined to actually use SwapEndian::Load(uint16_t).

    \sa NativeEndian::Load(int16_t)

***************************************/

/*! ************************************

    \fn uint32_t Burger::SwapEndian::Load(uint32_t uInput)
    \brief Reverse the endian of a 32 bit integer

    Given a 32 bit value in an integer register, swap the bytes so that
    0x12345678 becomes 0x78563412

    \param uInput Value to endian convert
    \return The input with the bytes swapped, no errors are possible.

    \sa NativeEndian::Load(uint32_t)

***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_ARM32)) && !defined(DOXYGEN)

// Nintendo DS ARM doesn't have REV
asm uint32_t BURGER_API Burger::SwapEndian::Load(uint32_t uInput)
{
    // clang-format off
    eor r1, r0, r0, ror #16 // r1 = 12^56,34^78,12^56,34^78
    bic r1, r1, #0x00ff0000 // r1 = 12^56,0,12^56,34^78
    mov r0, r0, ror #8      // r0 = 78,12,34,56
    eor r0, r0, r1, lsr #8  // r0 = 78^0,12^12^56,34^0,56^12^56 (78563412)
    bx lr
    // clang-format on
}

#endif

/*! ************************************

    \fn Burger::SwapEndian::Load(int32_t iInput);
    \brief Reverse the endian of a 32 bit signed integer.

    Given a 32 bit value in an integer register, swap the bytes
    so that 0x12345678 becomes 0x78563412.

    \param iInput = Value to endian convert

    \return The input with the bytes swapped, no errors are possible.

    \note This function is inlined to actually use SwapEndian::Load(uint32_t).

    \sa NativeEndian::Load(int32_t)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(uint64_t uInput)
    \brief Reverse the endian of a 64 bit integer

    Given a 64 bit value, swap the bytes so that 0x123456789ABCDEF0 becomes
    0xF0DEBC9A78563412

    \param uInput Value to endian convert
    \return The value with the bytes swapped, no errors are possible.

    \sa NativeEndian::Load(uint64_t)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(int64_t iInput);
    \brief Reverse the endian of a 64 bit signed integer.

    Given a 64 bit value in an integer register, swap the bytes
    so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412.

    \param iInput = Value to endian convert

    \return The input with the bytes swapped, no errors are possible.

    \note This function is inlined to actually use SwapEndian::Load(uint64_t).

    \sa NativeEndian::Load(int64_t)

***************************************/

/*! ************************************

    \brief Reverse the endian of a 32-bit float

    Given a 32-bit floating point value, swap the bytes
    so that 0x12345678 becomes 0x78563412

    \param fInput Value to endian convert
    \return The value with the bytes swapped, no errors are possible.
    \note Use of this form is discouraged since it involves moving a floating
    point value into an integer register and back again. Use the const float *
    version instead.

    \sa NativeEndian::Load(float)

***************************************/

#if (defined(BURGER_PS4) || \
    ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
        defined(BURGER_AMD64))) && \
    !defined(DOXYGEN)

#elif defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && \
    !defined(BURGER_CLANG) && !defined(BURGER_GNUC) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED float __fastcall Burger::SwapEndian::Load(
    float /* fInput */) BURGER_NOEXCEPT
{
    // clang-format off
    BURGER_ASM
    {
        mov edx, dword ptr[esp + 4] // Get the float
        mov eax, dword ptr[esp]     // Get the return address
        bswap edx                   // Swap the float
        mov dword ptr[esp + 4], edx // Store the float
        fld dword ptr[esp + 4]      // Load into the x87 FPU
        add esp, 8                  // Fix the stack
        jmp eax                     // Exit
    }
    // clang-format on
}

#elif defined(BURGER_METROWERKS) && defined(BURGER_PPC) && !defined(DOXYGEN)

asm float BURGER_API Burger::SwapEndian::Load(
    float /* fInput */) BURGER_NOEXCEPT
{
    // clang-format off
    stfs fp1, 24(SP)    // Store the 32 bit float on the stack
    addi r4, sp, 24     // Index to the stack addresses
    lwbrx r3, r0, r4    // Load the values endian reversed
    stw r3, 24(SP)      // Store the swapped data
    lfs fp1, 24(SP)     // Load the endian reversed float
    blr
    // clang-format on
}

#else

float BURGER_API Burger::SwapEndian::Load(float fInput) BURGER_NOEXCEPT
{
    uint32_float_t Temp;
    Temp.w =
        Load(static_cast<const uint32_t*>(static_cast<const void*>(&fInput)));
    return Temp.f; // Return the float
}

#endif

/*! ************************************

    \brief Reverse the endian of a 64-bit float

    Given a 64-bit floating point value, swap the bytes so that
    0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412

    \param dInput Value to endian convert

    \return The value with the bytes swapped, no errors are possible.

    \note Use of this form is discouraged since it involves moving a floating
    point value into an integer register and back again. Use the const double *
    version instead.

    \sa NativeEndian::Load(double)

***************************************/

#if (defined(BURGER_PS4) || \
    (defined(BURGER_GNUC) && !defined(__SNC__)) && defined(BURGER_AMD64)) && \
    !defined(DOXYGEN)

#elif defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && \
    !defined(BURGER_CLANG) && !defined(BURGER_GNUC) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED double __fastcall Burger::SwapEndian::Load(
    double /* dInput */) BURGER_NOEXCEPT
{
    // clang-format off
    BURGER_ASM
    {
        mov eax, dword ptr[esp + 4] // Fetch the double
        mov edx, dword ptr[esp + 8]
        bswap eax                   // Swap the lower 32 bits
        mov ecx, dword ptr[esp]     // Get the return address
        bswap edx                   // Swap the upper 32 bits
        mov dword ptr[esp + 8], eax // Store double, swapped
        mov dword ptr[esp + 4], edx
        fld qword ptr[esp + 4]      // Load the double into the FPU
        add esp, 12                 // Fix the stack
        jmp ecx                     // Exit
    }
    // clang-format on
}

#elif defined(BURGER_METROWERKS) && defined(BURGER_PPC) && !defined(DOXYGEN)

asm double BURGER_API Burger::SwapEndian::Load(
    double /* dInput */) BURGER_NOEXCEPT
{
    // clang-format off
    stfd fp1, 24(SP)    // Store the 64 bit float on the stack
    addi r5, sp, 24     // Index to the stack addresses
    addi r6, sp, 28
    lwbrx r3, r0, r5    // Load the values endian reversed
    lwbrx r4, r0, r6
    stw r4, 24(SP)      // Store the swapped data
    stw r3, 28(SP)
    lfd fp1, 24(SP)     // Load the endian reversed float
    blr
    // clang-format on
}

#else

double BURGER_API Burger::SwapEndian::Load(double dInput) BURGER_NOEXCEPT
{
    uint64_double_t Temp;
#if defined(BURGER_64BITCPU)
    Temp.w =
        Load(static_cast<const uint64_t*>(static_cast<const void*>(&dInput)));
#else
    Temp.w32[0] = Load(
        static_cast<const uint32_t*>(static_cast<const void*>(&dInput)) + 1);
    Temp.w32[1] = Load(
        static_cast<const uint32_t*>(static_cast<const void*>(&dInput)) + 0);
#endif
    return Temp.d; // Return the double
}

#endif

/*! ************************************

    \fn Burger::SwapEndian::Load(wchar_t uInput)
    \brief Reverse the endian of a 16 bit integer

    Given a 16 bit value in an integer register, swap the bytes so that 0x1234
    becomes 0x3412

    \param uInput Value to endian convert
    \return The input with the bytes swapped, no errors are possible.

    \sa NativeEndian::Load(wchar_t)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(unsigned int uInput);
    \brief Reverse the endian of a 16-32 bit unsigned integer.

    Given a 16-32 bit value in an integer register, swap the bytes
    so that 0x12345678 becomes 0x78563412.

    \param uInput = Value to endian convert

    \return The input with the bytes swapped, no errors are possible.

    \sa NativeEndian::Load(unsigned int)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(signed int iInput);
    \brief Reverse the endian of a 16-32 bit signed integer.

    Given a 16-32 bit value in an integer register, swap the bytes
    so that 0x12345678 becomes 0x78563412.

    \param iInput = Value to endian convert

    \return The input with the bytes swapped, no errors are possible.

    \sa NativeEndian::Load(signed int)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(unsigned long uInput);
    \brief Reverse the endian of a 32-64 bit unsigned integer.

    Given a 32-64 bit value in an integer register, swap the bytes
    so that 0x12345678 becomes 0x78563412.

    \param uInput = Value to endian convert

    \return The input with the bytes swapped, no errors are possible.

    \sa NativeEndian::Load(unsigned long)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(signed long iInput);
    \brief Reverse the endian of a 32-64 bit signed integer.

    Given a 32-64 bit value in an integer register, swap the bytes
    so that 0x12345678 becomes 0x78563412.

    \param iInput = Value to endian convert

    \return The input with the bytes swapped, no errors are possible.

    \sa NativeEndian::Load(signed long)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(const char *pInput)
    \brief Load an 8 bit signed value to memory with byte alignment.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pInput Pointer to an 8 bit value.
    \return The 8 bit signed value.

    \sa NativeEndian::Load(const char*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(const uint8_t *pInput)
    \brief Load an 8 bit unsigned value to memory with byte alignment.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pInput Pointer to an 8 bit value.
    \return The 8 bit unsigned value.

    \sa NativeEndian::Load(const uint8_t*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(const int8_t *pInput)
    \brief Load an 8 bit signed value to memory with byte alignment.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pInput Pointer to an 8 bit value.
    \return The 8 bit signed value.

    \sa NativeEndian::Load(const int8_t*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(const uint16_t *pInput)
    \brief Reverse the endian of a 16 bit integer

    Given a pointer to a 16 bit value, load it and swap the bytes so that 0x1234
    becomes 0x3412

    \param pInput Pointer to a value to endian convert
    \return The value with the bytes swapped.

    \note nullptr is illegal and will page fault.

    \sa NativeEndian::Load(const uint16_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(const int16_t *pInput);
    \brief Reverse the endian of a 16 bit signed integer

    Given a pointer to a 16 bit value, load it and swap the bytes so that 0x1234
    becomes 0x3412

    \param pInput Pointer to a value to endian convert
    \return The value with the bytes swapped.

    \note nullptr is illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Load(const uint16_t *).

    \sa NativeEndian::Load(const int16_t*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(const uint32_t *pInput)
    \brief Reverse the endian of a 32 bit integer

    Given a pointer to a 32 bit value, load it and swap the bytes so that
    0x12345678 becomes 0x78563412

    \param pInput Pointer to value to endian convert
    \return The value with the bytes swapped.

    \note nullptr is illegal and will page fault.

    \sa NativeEndian::Load(const uint32_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(const int32_t *pInput);
    \brief Reverse the endian of a 32 bit signed integer

    Given a pointer to a 32 bit value, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412

    \param pInput Pointer to a value to endian convert
    \return The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Load(const uint32_t*).

    \sa NativeEndian::Load(const int32_t*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(const uint64_t *pInput)
    \brief Reverse the endian of a 64-bit integer

    Given a pointer to a 64-bit value, load it and swap the bytes
    so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412

    \param pInput Pointer to the value to endian convert
    \return The value with the bytes swapped.

    \note nullptr is illegal and will page fault.

    \sa NativeEndian::Load(const uint64_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(const int64_t *pInput);
    \brief Reverse the endian of a 64 bit signed integer

    Given a pointer to a 64 bit value, load it and swap the bytes
    so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412

    \param pInput Pointer to a value to endian convert
    \return The value with the bytes swapped.

    \note nullptr is illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Load(const uint64_t*).

    \sa NativeEndian::Load(const int64_t*)

***************************************/

/*! ************************************

    \brief Reverse the endian of a 32-bit float

    Given a pointer to a 32-bit floating point value, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412

    \param pInput Pointer to the value to endian convert
    \return The value with the bytes swapped.

    \note nullptr is illegal and will page fault.

    \sa NativeEndian::Load(const float *)

***************************************/

#if (defined(BURGER_PS4) || \
    (defined(BURGER_GNUC) && !defined(__SNC__)) && defined(BURGER_AMD64)) && \
    !defined(DOXYGEN)

#elif defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && \
    !defined(BURGER_CLANG) && !defined(BURGER_GNUC) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED float __fastcall Burger::SwapEndian::Load(
    const float* /* pInput */) BURGER_NOEXCEPT
{
    // clang-format off
    BURGER_ASM
    {
        mov edx, dword ptr[ecx] // Get the float
        mov eax, dword ptr[esp] // Get the return address
        bswap edx               // Swap the float
        mov dword ptr[esp], edx // Store the float
        fld dword ptr[esp]      // Load into the x87 FPU
        add esp, 4              // Fix the stack
        jmp eax                 // Exit
    }
    // clang-format on
}

#elif defined(BURGER_METROWERKS) && defined(BURGER_PPC) && !defined(DOXYGEN)

asm float BURGER_API Burger::SwapEndian::Load(
    const float* /* pInput */) BURGER_NOEXCEPT
{
    // clang-format off
    lwbrx r0, r0, r3    // Fetch it endian swap
    stw r0, 24(SP)      // Save it on the stack
    lfs fp1, 24(SP)     // Load it as a float
    blr
    // clang-format on
}

#else

float BURGER_API Burger::SwapEndian::Load(const float* pInput) BURGER_NOEXCEPT
{
    uint32_float_t Temp;
    Temp.w =
        Load(static_cast<const uint32_t*>(static_cast<const void*>(pInput)));
    return Temp.f; // Return the float
}

#endif

/*! ************************************

    \brief Reverse the endian of a 64-bit float

    Given a pointer to a 64-bit floating point value, load it and swap the bytes
    so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412

    \param pInput Pointer to the value to endian convert
    \return The value with the bytes swapped.

    \note nullptr is illegal and will page fault.

    \sa SwapEndian::Load(double) or NativeEndian::Load(const double *)

***************************************/

#if (defined(BURGER_PS4) || \
    (defined(BURGER_GNUC) && !defined(__SNC__)) && defined(BURGER_AMD64)) && \
    !defined(DOXYGEN)

#elif defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && \
    !defined(BURGER_CLANG) && !defined(BURGER_GNUC) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED double __fastcall Burger::SwapEndian::Load(
    const double* /* dInput */) BURGER_NOEXCEPT
{
    // clang-format off
    BURGER_ASM
    {
        mov edx, dword ptr[ecx] // Fetch the double
        mov eax, dword ptr[ecx + 4]
        mov ecx, dword ptr[esp] // Get the return address
        bswap edx               // Swap the upper 32 bits
        bswap eax               // Swap the lower 32 bits
        mov dword ptr[esp], edx // Store the upper 32 bits
        push eax                // Store the lower 32 bits by making room for it
        fld qword ptr[esp]      // Load the double into the FPU
        add esp, 8              // Fix the stack
        jmp ecx                 // Exit
    }
    // clang-format on
}

#elif defined(BURGER_METROWERKS) && defined(BURGER_PPC) && !defined(DOXYGEN)

asm double BURGER_API Burger::SwapEndian::Load(
    const double* /* pInput */) BURGER_NOEXCEPT
{
    // clang-format off
    li r5, 4
    lwbrx r0, r0, r3    // Fetch it endian swap
    lwbrx r4, r5, r3
    stw r4, 24(SP)      // Save it on the stack
    stw r0, 28(SP)
    lfd fp1, 24(SP)     // Load it as a float
    blr
    // clang-format on
}

#else

double BURGER_API Burger::SwapEndian::Load(const double* pInput) BURGER_NOEXCEPT
{
    uint64_double_t Temp;
#if defined(BURGER_64BITCPU)
    Temp.w =
        Load(static_cast<const uint64_t*>(static_cast<const void*>(pInput)));
#else
    Temp.w32[0] = Load(
        static_cast<const uint32_t*>(static_cast<const void*>(pInput)) + 1);
    Temp.w32[1] = Load(
        static_cast<const uint32_t*>(static_cast<const void*>(pInput)) + 0);
#endif
    return Temp.d; // Return the double
}

#endif

/*! ************************************

    \fn Burger::SwapEndian::Load(const wchar_t *pInput)
    \brief Reverse the endian of a 32 bit integer

    Given a pointer to a 16 bit value, load it and swap the bytes so that
    0x1234 becomes 0x3412

    \param pInput Pointer to value to endian convert
    \return The value with the bytes swapped.

    \note nullptr is illegal and will page fault.

    \sa NativeEndian::Load(const wchar_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(const unsigned int *pInput);
    \brief Reverse the endian of a 16-32 bit unsigned integer

    Given a pointer to a 16-32 bit value, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412

    \param pInput Pointer to a value to endian convert
    \return The value with the bytes swapped.

    \note nullptr is illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Load(const uint32_t*).

    \sa NativeEndian::Load(const unsigned int*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(const signed int *pInput);
    \brief Reverse the endian of a 16-32 bit signed integer

    Given a pointer to a 16-32 bit value, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412

    \param pInput Pointer to a value to endian convert
    \return The value with the bytes swapped.

    \note nullptr is illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Load(const uint32_t*).

    \sa NativeEndian::Load(const signed int*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(const unsigned long *pInput);
    \brief Reverse the endian of a 32-64 bit unsigned integer

    Given a pointer to a 32-64 bit value, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412

    \param pInput Pointer to a value to endian convert
    \return The value with the bytes swapped.

    \note nullptr is illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Load(const uint64_t*).

    \sa NativeEndian::Load(const unsigned long*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Load(const signed long *pInput);
    \brief Reverse the endian of a 32-64 bit signed integer

    Given a pointer to a 32-64 bit value, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412

    \param pInput Pointer to a value to endian convert
    \return The value with the bytes swapped.

    \note nullptr is illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Load(const uint64_t*).

    \sa NativeEndian::Load(const signed long*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Store(char *pOutput,char iInput)
    \brief Store an 8 bit signed value to memory with byte alignment.

    \note Since 8 bit values can't be endian swapped, this function only exists
    for completeness sake

    \param pOutput Pointer to an 8 bit value.
    \param iInput The 8 bit signed value.

    \sa NativeEndian::Store(char*,char)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Store(uint8_t *pOutput,uint8_t uInput)
    \brief Store an 8 bit unsigned value to memory with byte alignment.

    \note Since 8 bit values can't be endian swapped, this function only exists
    for completeness sake

    \param pOutput Pointer to an 8 bit value.
    \param uInput The 8 bit unsigned value.

    \sa NativeEndian::Store(uint8_t*,char)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Store(int8_t *pOutput,int8_t iInput)
    \brief Store an 8 bit signed value to memory with byte alignment.

    \note Since 8 bit values can't be endian swapped, this function only exists
    for completeness sake

    \param pOutput Pointer to an 8 bit value.
    \param iInput The 8 bit signed value.

    \sa NativeEndian::Store(int8_t*,int8_t)

***************************************/

/*! ************************************

    \brief Reverse the endian of a 16 bit integer

    Given a pointer to a 16 bit value, load it and swap the bytes
    so that 0x1234 becomes 0x3412

    \param pOutput Pointer to a value to endian convert
    \param uInput The value with the bytes swapped.

    \note nullptr is illegal and will page fault.

    \sa NativeEndian::Store(uint16_t *, uint16_t)

***************************************/

#if ((defined(BURGER_METROWERKS) && defined(BURGER_X86)) || \
    (defined(BURGER_METROWERKS) && defined(BURGER_68K)) || \
    (defined(BURGER_METROWERKS) && defined(BURGER_PPC)) || \
    defined(BURGER_XBOX360) || defined(BURGER_PS3) || \
    defined(BURGER_INTEL_COMPILER) || \
    (defined(BURGER_WATCOM) && defined(BURGER_X86)) || defined(BURGER_MSVC) || \
    (defined(BURGER_GNUC) && !defined(__SNC__))) && \
    !defined(DOXYGEN)
#else

void BURGER_API Burger::SwapEndian::Store(uint16_t* pOutput, uint16_t uInput)
{
    uint_t uTemp = (uInput >> 8U) & 0xFFU;
    uTemp |= (uInput << 8U) & 0xFF00U;
    pOutput[0] = static_cast<uint16_t>(uTemp);
}

#endif

/*! ************************************

    \fn Burger::SwapEndian::Store(int16_t *pOutput,int16_t iInput);
    \brief Reverse the endian of a 16 bit signed integer and store it

    Given a 16 bit value, swap the bytes
    so that 0x1234 becomes 0x3412 and store it

    \param pOutput Pointer to where to store the data.
    \param iInput The value to byte swap and store.

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Store(uint16_t*,uint16_t).

    \sa NativeEndian::Store(int16_t*,int16_t)

***************************************/

/*! ************************************

    \brief Reverse the endian of a 32 bit integer

    Given a pointer to a 32 bit value, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412

    \param pOutput Pointer to value to endian convert
    \param uInput The value with the bytes swapped.

    \note nullptr is illegal and will page fault.

    \sa NativeEndian::Load(uint32_t *, uint32_t)

***************************************/

#if ((defined(BURGER_METROWERKS) && defined(BURGER_X86)) || \
    (defined(BURGER_METROWERKS) && defined(BURGER_68K)) || \
    (defined(BURGER_METROWERKS) && defined(BURGER_PPC)) || \
    defined(BURGER_XBOX360) || defined(BURGER_PS3) || \
    defined(BURGER_INTEL_COMPILER) || \
    (defined(BURGER_WATCOM) && defined(BURGER_X86)) || defined(BURGER_MSVC) || \
    (defined(BURGER_GNUC) && !defined(__SNC__))) && \
    !defined(DOXYGEN)
#else

void BURGER_API Burger::SwapEndian::Store(uint32_t* pOutput, uint32_t uInput)
{
    uint32_t uTemp = (uInput >> 24U) & 0xFFU;
    uTemp |= (uInput >> 8U) & 0xFF00U;
    uTemp |= (uInput << 8U) & 0xFF0000U;
    uTemp |= uInput << 24U;
    pOutput[0] = uTemp;
}

#endif

/*! ************************************

    \fn Burger::SwapEndian::Store(int32_t *pOutput,int32_t iInput);
    \brief Reverse the endian of a 32 bit signed integer and store it

    Given a 32 bit value, swap the bytes
    so that 0x12345678 becomes 0x78563412 and store it

    \param pOutput Pointer to where to store the data.
    \param iInput The value to byte swap and store.

    \note nullptr is illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Store(uint32_t*,uint32_t).

    \sa NativeEndian::Store(int32_t*,int32_t)

***************************************/

/*! ************************************

    \brief Store a 64-bit integer with endian reversal

    Given a 64-bit value, swap the bytes so that 0x123456789ABCDEF0 becomes
    0xF0DEBC9A78563412  and store it.

    \param pOutput Pointer to the value to endian convert
    \param uInput The value to store

    \note nullptr is illegal and will page fault.

    \sa NativeEndian::Store(uint64_t *,uint64_t)

***************************************/

#if (defined(BURGER_XBOX360) || defined(BURGER_PS3) || \
    (defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
    (defined(BURGER_INTEL) && defined(BURGER_MSVC)) || \
    ((defined(BURGER_GNUC) && !defined(__SNC__)) && \
        (defined(BURGER_AMD64) || defined(BURGER_ARM64) || \
            defined(BURGER_PPC)))) && \
    !defined(DOXYGEN)

#else

void BURGER_API Burger::SwapEndian::Store(
    uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT
{
    uint8_t bTemp1 = reinterpret_cast<const uint8_t*>(&uInput)[7];
    uint8_t bTemp2 = reinterpret_cast<const uint8_t*>(&uInput)[6];
    reinterpret_cast<uint8_t*>(pOutput)[0] = bTemp1; // Perform the swap
    reinterpret_cast<uint8_t*>(pOutput)[1] = bTemp2;
    bTemp1 = reinterpret_cast<const uint8_t*>(&uInput)[5];
    bTemp2 = reinterpret_cast<const uint8_t*>(&uInput)[4];
    reinterpret_cast<uint8_t*>(pOutput)[2] = bTemp1;
    reinterpret_cast<uint8_t*>(pOutput)[3] = bTemp2;
    bTemp1 = reinterpret_cast<const uint8_t*>(&uInput)[3];
    bTemp2 = reinterpret_cast<const uint8_t*>(&uInput)[2];
    reinterpret_cast<uint8_t*>(pOutput)[4] = bTemp1;
    reinterpret_cast<uint8_t*>(pOutput)[5] = bTemp2;
    bTemp1 = reinterpret_cast<const uint8_t*>(&uInput)[1];
    bTemp2 = reinterpret_cast<const uint8_t*>(&uInput)[0];
    reinterpret_cast<uint8_t*>(pOutput)[6] = bTemp1;
    reinterpret_cast<uint8_t*>(pOutput)[7] = bTemp2;
}

#endif

/*! ************************************

    \fn Burger::SwapEndian::Store(int64_t *pOutput,int64_t iInput);
    \brief Reverse the endian of a 64 bit signed integer and store it

    Given a 64 bit value, swap the bytes so that 0x123456789ABCDEF0 becomes
    0xF0DEBC9A78563412 and store it

    \param pOutput Pointer to where to store the data.
    \param iInput The value to byte swap and store.

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Store(uint64_t*,uint64_t).

    \sa NativeEndian::Store(int64_t*,int64_t)

***************************************/

/*! ************************************

    \brief Store a 32-bit float with endian reversal

    Given a 32-bit float, swap the bytes and store it.

    \param pOutput Pointer to the 32 bit float to endian convert
    \param fInput The value to store

    \note \ref NULL pointers are illegal and will page fault.

    \sa SwapEndian::Store(float) or NativeEndian::Store(float *,float)

***************************************/

void BURGER_API Burger::SwapEndian::Store(
    float* pOutput, float fInput) BURGER_NOEXCEPT
{
    uint8_t bTemp1 =
        reinterpret_cast<const uint8_t*>(&fInput)[3]; // Perform the swap
    uint8_t bTemp2 = reinterpret_cast<const uint8_t*>(&fInput)[2];
    reinterpret_cast<uint8_t*>(pOutput)[0] = bTemp1;
    reinterpret_cast<uint8_t*>(pOutput)[1] = bTemp2;
    bTemp1 = reinterpret_cast<const uint8_t*>(&fInput)[1];
    bTemp2 = reinterpret_cast<const uint8_t*>(&fInput)[0];
    reinterpret_cast<uint8_t*>(pOutput)[2] = bTemp1;
    reinterpret_cast<uint8_t*>(pOutput)[3] = bTemp2;
}

/*! ************************************

    \brief Store a 64-bit float with endian reversal

    Given a 64-bit float, swap the bytes and store it.

    \param pOutput Pointer to the 64 bit float to endian convert
    \param dInput The value to store

    \note \ref NULL pointers are illegal and will page fault.

    \sa SwapEndian::Store(double) or NativeEndian::Store(double *,double)

***************************************/

void BURGER_API Burger::SwapEndian::Store(
    double* pOutput, double dInput) BURGER_NOEXCEPT
{
    uint8_t bTemp1 =
        reinterpret_cast<const uint8_t*>(&dInput)[7]; // Perform the swap
    uint8_t bTemp2 = reinterpret_cast<const uint8_t*>(&dInput)[6];
    reinterpret_cast<uint8_t*>(pOutput)[0] = bTemp1;
    reinterpret_cast<uint8_t*>(pOutput)[1] = bTemp2;
    bTemp1 = reinterpret_cast<const uint8_t*>(&dInput)[5];
    bTemp2 = reinterpret_cast<const uint8_t*>(&dInput)[4];
    reinterpret_cast<uint8_t*>(pOutput)[2] = bTemp1;
    reinterpret_cast<uint8_t*>(pOutput)[3] = bTemp2;
    bTemp1 = reinterpret_cast<const uint8_t*>(&dInput)[3];
    bTemp2 = reinterpret_cast<const uint8_t*>(&dInput)[2];
    reinterpret_cast<uint8_t*>(pOutput)[4] = bTemp1;
    reinterpret_cast<uint8_t*>(pOutput)[5] = bTemp2;
    bTemp1 = reinterpret_cast<const uint8_t*>(&dInput)[1];
    bTemp2 = reinterpret_cast<const uint8_t*>(&dInput)[0];
    reinterpret_cast<uint8_t*>(pOutput)[6] = bTemp1;
    reinterpret_cast<uint8_t*>(pOutput)[7] = bTemp2;
}

/*! ************************************

    \fn Burger::SwapEndian::Store(unsigned int *pOutput,unsigned int uInput);
    \brief Reverse the endian of a 32 bit unsigned integer

    Given a 32 bit value, swap the bytes
    so that 0x12345678 becomes 0x78563412 and store it

    \param pOutput Pointer to a value to store
    \param uInput The value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Store(uint32_t*,uint32_t).

    \sa SwapEndian::Store(uint32_t*,uint32_t)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Store(signed int *pOutput,signed int iInput);
    \brief Reverse the endian of a 32 bit signed integer

    Given a 32 bit value, swap the bytes
    so that 0x12345678 becomes 0x78563412 and store it

    \param pOutput Pointer to a value to store
    \param iInput The value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Store(uint32_t*,uint32_t).

    \sa SwapEndian::Store(uint32_t*,uint32_t)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::LoadAny(const char *pInput)
    \brief Load an 8 bit signed value to memory with byte alignment.

    It assumes that the input pointer is unaligned.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pInput Pointer to an 8 bit value.
    \return The 8 bit signed value.

    \sa NativeEndian::LoadAny(const char*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::LoadAny(const uint8_t *pInput)
    \brief Load an 8 bit unsigned value to memory with byte alignment.

    It assumes that the input pointer is unaligned.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pInput Pointer to an 8 bit value.
    \return The 8 bit unsigned value.

    \sa NativeEndian::LoadAny(const uint8_t*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::LoadAny(const int8_t *pInput)
    \brief Load an 8 bit signed value to memory with byte alignment.

    It assumes that the input pointer is unaligned.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pInput Pointer to an 8 bit value.
    \return The 8 bit signed value.

    \sa NativeEndian::LoadAny(const int8_t*)

***************************************/

/*! ************************************

    \brief Fetch a 16 bit unsigned reverse endian integer from memory with byte
        alignment.

    Given a pointer to a 16 bit value, load it and swap the bytes
    so that 0x1234 becomes 0x3412. The pointer does not have
    to be 16-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to a value to endian convert
    \return The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.

    \sa SwapEndian::Load(const uint16_t *)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

#elif defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && \
    !defined(BURGER_CLANG) && !defined(BURGER_GNUC) && !defined(DOXYGEN)
BURGER_DECLSPECNAKED uint16_t __fastcall Burger::SwapEndian::LoadAny(
    const uint16_t* /* pInput */) BURGER_NOEXCEPT
{
    // clang-format off
    BURGER_ASM
    {
        mov ah, byte ptr[ecx]       // Grab the short
        mov al, byte ptr[ecx + 1]
        ret
    }
    // clang-format on
}
#else

uint16_t BURGER_API Burger::SwapEndian::LoadAny(
    const uint16_t* pInput) BURGER_NOEXCEPT
{
    unsigned int Low =
        reinterpret_cast<const uint8_t*>(pInput)[0]; // Grab 16 bits
    unsigned int High = reinterpret_cast<const uint8_t*>(pInput)[1];
#if defined(BURGER_LITTLEENDIAN)
    uint16_t Result =
        static_cast<uint16_t>((Low << 8U) + High); // Intel likes +
#else
    uint16_t Result = static_cast<uint16_t>(Low | (High << 8U)); // PPC likes |
#endif
    return Result;
}
#endif

/*! ************************************

    \brief Fetch a 32 bit unsigned reverse endian integer from memory with byte
        alignment.

    Given a pointer to a 32 bit value, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412. The pointer does not have
    to be 32-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to value to endian convert
    \return The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.

    \sa SwapEndian::Load(const uint32_t *)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

#elif defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && \
    !defined(BURGER_CLANG) && !defined(BURGER_GNUC) && !defined(DOXYGEN)
BURGER_DECLSPECNAKED uint32_t __fastcall Burger::SwapEndian::LoadAny(
    const uint32_t* /* pInput */) BURGER_NOEXCEPT
{
    // clang-format off
    BURGER_ASM
    {
        mov al, byte ptr[ecx] // Grab the long in a virtual memory friendly way
        mov ah, byte ptr[ecx + 1]
        bswap eax
        mov ah, byte ptr[ecx + 2]
        mov al, byte ptr[ecx + 3]
        ret
    }
    // clang-format on
}
#else
uint32_t BURGER_API Burger::SwapEndian::LoadAny(
    const uint32_t* pInput) BURGER_NOEXCEPT
{
    unsigned int Low1 =
        reinterpret_cast<const uint8_t*>(pInput)[0]; // Grab 32 bits
    unsigned int Low2 = reinterpret_cast<const uint8_t*>(pInput)[1];
    unsigned int Low3 = reinterpret_cast<const uint8_t*>(pInput)[2];
    unsigned int Low4 = reinterpret_cast<const uint8_t*>(pInput)[3];
#if defined(BURGER_LITTLEENDIAN)
    uint32_t Result = static_cast<uint32_t>(
        (Low1 << 24U) + (Low2 << 16U) + (Low3 << 8U) + Low4); // Intel likes +
#else
    uint32_t Result = static_cast<uint32_t>(
        Low1 | (Low2 << 8U) | (Low3 << 16U) | (Low4 << 24U)); // PPC likes |
#endif
    return Result;
}
#endif

/*! ************************************

    \brief Fetch a 64 bit unsigned reverse endian integer from memory with byte
        alignment.

    Assuming the data is unaligned, it will grab data a byte at a time
    and reconstruct it into a 64 bit value in reverse endian. The pointer does
    not have to be 64-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to a 64 bit value.
    \return The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.

    \sa NativeEndian::LoadAny(const uint64_t*)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

#elif defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && \
    !defined(BURGER_CLANG) && !defined(BURGER_GNUC) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED uint64_t __fastcall Burger::SwapEndian::LoadAny(
    const uint64_t* /* pInput */) BURGER_NOEXCEPT
{
    // Intel returns 64 bit values in EAX:EDX
    // clang-format off
    BURGER_ASM
    {
        mov dl, byte ptr[ecx] // Grab the long in a virtual memory friendly way
        mov dh, byte ptr[ecx + 1]
        bswap edx
        mov dh, byte ptr[ecx + 2]
        mov dl, byte ptr[ecx + 3]
        mov al, byte ptr[ecx + 4] // Grab the long in a virtual memory friendly way
        mov ah, byte ptr[ecx + 5]
        bswap eax
        mov ah, byte ptr[ecx + 6]
        mov al, byte ptr[ecx + 7]
        ret
    }
    // clang-format on
}

#else
uint64_t BURGER_API Burger::SwapEndian::LoadAny(
    const uint64_t* pInput) BURGER_NOEXCEPT
{
    uint64_t Result; // Make sure it's aligned

    reinterpret_cast<uint8_t*>(&Result)[7] =
        reinterpret_cast<const uint8_t*>(pInput)[0];
    reinterpret_cast<uint8_t*>(&Result)[6] =
        reinterpret_cast<const uint8_t*>(pInput)[1];
    reinterpret_cast<uint8_t*>(&Result)[5] =
        reinterpret_cast<const uint8_t*>(pInput)[2];
    reinterpret_cast<uint8_t*>(&Result)[4] =
        reinterpret_cast<const uint8_t*>(pInput)[3];
    reinterpret_cast<uint8_t*>(&Result)[3] =
        reinterpret_cast<const uint8_t*>(pInput)[4];
    reinterpret_cast<uint8_t*>(&Result)[2] =
        reinterpret_cast<const uint8_t*>(pInput)[5];
    reinterpret_cast<uint8_t*>(&Result)[1] =
        reinterpret_cast<const uint8_t*>(pInput)[6];
    reinterpret_cast<uint8_t*>(&Result)[0] =
        reinterpret_cast<const uint8_t*>(pInput)[7];
    return Result; // Return the 64 bit value in a register
}

#endif

/*! ************************************

    \brief Fetch a 32 bit reverse endian float from memory with byte alignment.

    Given a pointer to a 32-bit floating point value, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412. The pointer does not have
    to be 32-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to the value to endian convert
    \return The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.

    \sa SwapEndian::Load(float) or NativeEndian::Load(const float *)

***************************************/

#if defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && \
    !defined(BURGER_CLANG) && !defined(BURGER_GNUC) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED float __fastcall Burger::SwapEndian::LoadAny(
    const float* /* pInput */) BURGER_NOEXCEPT
{
    // clang-format off
    BURGER_ASM
    {
        mov ah, byte ptr[ecx]   // Grab the long in a virtual memory friendly way
        mov al, byte ptr[ecx + 1]
        mov dh, byte ptr[ecx + 2]
        mov dl, byte ptr[ecx + 3]
        mov ecx, [esp]          // Get the return address in the dead cycle
        mov[esp], dx            // Store in memory
        mov[esp + 2], ax
        fld dword ptr[esp]      // Get the float
        add esp, 4              // Restore the stack (And the return)
        jmp ecx                 // Faster than RET
    }
    // clang-format on
}

#else
float BURGER_API Burger::SwapEndian::LoadAny(
    const float* pInput) BURGER_NOEXCEPT
{
    float fInput; // This MUST be cast as a float to be float aligned!

    uint8_t bTemp1 = reinterpret_cast<const uint8_t*>(pInput)[0];
    uint8_t bTemp2 = reinterpret_cast<const uint8_t*>(pInput)[1];
    reinterpret_cast<uint8_t*>(&fInput)[3] = bTemp1; // Perform the swap
    reinterpret_cast<uint8_t*>(&fInput)[2] = bTemp2;
    bTemp1 = reinterpret_cast<const uint8_t*>(pInput)[2];
    bTemp2 = reinterpret_cast<const uint8_t*>(pInput)[3];
    reinterpret_cast<uint8_t*>(&fInput)[1] = bTemp1;
    reinterpret_cast<uint8_t*>(&fInput)[0] = bTemp2;
    return fInput; // Return the float
}
#endif

/*! ************************************

    \brief Fetch a 64 bit reverse endian float from memory with byte alignment.

    Given a pointer to a 64-bit floating point value, load it and swap the bytes
    so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412. The pointer does not
    have to be 64-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to the value to endian convert
    \return The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.

    \sa SwapEndian::Load(double) or NativeEndian::Load(const double *)

***************************************/

#if defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && \
    !defined(BURGER_CLANG) && !defined(BURGER_GNUC) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED double BURGER_API Burger::SwapEndian::LoadAny(
    const double* /* pInput */) BURGER_NOEXCEPT
{
    // Intel returns 64 bit values in EAX:EDX
    // clang-format off
    BURGER_ASM
    {
        sub esp, 8              // Make space on the stack (Intel is okay for 4 byte alignment)
        mov ah, byte ptr[ecx]   // Grab the long in a virtual memory friendly way
        mov al, byte ptr[ecx + 1]
        mov dh, byte ptr[ecx + 2]
        mov dl, byte ptr[ecx + 3]
        mov[esp + 6], ax        // Store in memory
        mov[esp + 4], dx
        mov ah, byte ptr[ecx + 4] // Grab the long in a virtual memory friendly way
        mov al, byte ptr[ecx + 5]
        mov dh, byte ptr[ecx + 6]
        mov dl, byte ptr[ecx + 7]
        mov[esp + 2], ax        // Store in memory
        mov[esp], dx
        mov eax, [esp + 8]      // Get the return address in the dead cycle
        fld qword ptr[esp]      // Get the double
        add esp, 12             // Restore the stack (And the return)
        jmp eax                 // Faster than RET
    }
    // clang-format on
}

#else

double BURGER_API Burger::SwapEndian::LoadAny(
    const double* pInput) BURGER_NOEXCEPT
{
    double dTemp; // This MUST be cast as a double to be double aligned!

    uint8_t bTemp1 = reinterpret_cast<const uint8_t*>(pInput)[0];
    uint8_t bTemp2 = reinterpret_cast<const uint8_t*>(pInput)[1];
    reinterpret_cast<uint8_t*>(&dTemp)[7] = bTemp1; // Perform the swap
    reinterpret_cast<uint8_t*>(&dTemp)[6] = bTemp2;
    bTemp1 = reinterpret_cast<const uint8_t*>(pInput)[2];
    bTemp2 = reinterpret_cast<const uint8_t*>(pInput)[3];
    reinterpret_cast<uint8_t*>(&dTemp)[5] = bTemp1;
    reinterpret_cast<uint8_t*>(&dTemp)[4] = bTemp2;
    bTemp1 = reinterpret_cast<const uint8_t*>(pInput)[4];
    bTemp2 = reinterpret_cast<const uint8_t*>(pInput)[5];
    reinterpret_cast<uint8_t*>(&dTemp)[3] = bTemp1;
    reinterpret_cast<uint8_t*>(&dTemp)[2] = bTemp2;
    bTemp1 = reinterpret_cast<const uint8_t*>(pInput)[6];
    bTemp2 = reinterpret_cast<const uint8_t*>(pInput)[7];
    reinterpret_cast<uint8_t*>(&dTemp)[1] = bTemp1;
    reinterpret_cast<uint8_t*>(&dTemp)[0] = bTemp2;
    return dTemp; // Return the double
}

#endif

/*! ************************************

    \fn Burger::SwapEndian::StoreAny(char *pOutput,char iInput)
    \brief Store an 8 bit signed value to memory with byte alignment.

    It assumes that the output pointer is unaligned.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pOutput Pointer to an 8 bit value.
    \param iInput The 8 bit signed value.

    \sa NativeEndian::StoreAny(char*,char)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::StoreAny(uint8_t *pOutput,uint8_t uInput)
    \brief Store an 8 bit unsigned value to memory with byte alignment.

    It assumes that the output pointer is unaligned.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pOutput Pointer to an 8 bit value.
    \param uInput The 8 bit unsigned value.

    \sa NativeEndian::StoreAny(uint8_t*,char)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::StoreAny(int8_t *pOutput,int8_t iInput)
    \brief Store an 8 bit signed value to memory with byte alignment.

    It assumes that the output pointer is unaligned.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pOutput Pointer to an 8 bit value.
    \param iInput The 8 bit signed value.

    \sa NativeEndian::StoreAny(int8_t*,int8_t)

***************************************/

/*! ************************************

    \brief Store a 16 bit unsigned value with endian swapping to memory with
        byte alignment.

    Assuming the output pointer is unaligned, it will store data a byte at a
    time into a 16 bit value in reversed endian.

    \param pOutput Pointer to a 16 bit value.
    \param uInput The 16 bit unsigned value.

    \sa NativeEndian::Store(uint16_t*,uint16_t) or
        SwapEndian::Store(uint16_t *,uint16_t).

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

#else
void BURGER_API Burger::SwapEndian::StoreAny(
    uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT
{
#if defined(BURGER_BIGENDIAN)
    reinterpret_cast<uint8_t*>(pOutput)[0] = static_cast<uint8_t>(uInput);
    reinterpret_cast<uint8_t*>(pOutput)[1] = static_cast<uint8_t>(uInput >> 8U);
#else
    reinterpret_cast<uint8_t*>(pOutput)[0] = static_cast<uint8_t>(uInput >> 8U);
    reinterpret_cast<uint8_t*>(pOutput)[1] = static_cast<uint8_t>(uInput);
#endif
}

#endif

/*! ************************************

    \brief Store a 32 bit unsigned value with endian swapping to memory with
        byte alignment.

    Assuming the output pointer is unaligned, it will store data a byte at a
    time into a 32 bit value in reversed endian.

    \param pOutput Pointer to a 32 bit value.
    \param uInput The 32 bit unsigned value.

    \sa NativeEndian::Store(uint32_t*,uint32_t)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

#else
void BURGER_API Burger::SwapEndian::StoreAny(
    uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT
{
#if defined(BURGER_BIGENDIAN)
    reinterpret_cast<uint8_t*>(pOutput)[0] = static_cast<uint8_t>(uInput);
    reinterpret_cast<uint8_t*>(pOutput)[1] = static_cast<uint8_t>(uInput >> 8U);
    reinterpret_cast<uint8_t*>(pOutput)[2] =
        static_cast<uint8_t>(uInput >> 16U);
    reinterpret_cast<uint8_t*>(pOutput)[3] =
        static_cast<uint8_t>(uInput >> 24U);
#else
    reinterpret_cast<uint8_t*>(pOutput)[0] =
        static_cast<uint8_t>(uInput >> 24U);
    reinterpret_cast<uint8_t*>(pOutput)[1] =
        static_cast<uint8_t>(uInput >> 16U);
    reinterpret_cast<uint8_t*>(pOutput)[2] = static_cast<uint8_t>(uInput >> 8U);
    reinterpret_cast<uint8_t*>(pOutput)[3] = static_cast<uint8_t>(uInput);
#endif
}

#endif

/*! ************************************

    \brief Store a 64 bit unsigned value with endian swapping to memory with
        byte alignment.

    Assuming the output pointer is unaligned, it will store data a byte at a
    time into a 64 bit value in reversed endian.

    \param pOutput Pointer to a 64 bit value.
    \param uInput The 64 bit unsigned value.

    \sa NativeEndian::Store(uint64_t*,uint64_t)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

#else
void BURGER_API Burger::SwapEndian::StoreAny(
    uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT
{
    reinterpret_cast<uint8_t*>(pOutput)[0] =
        reinterpret_cast<const uint8_t*>(&uInput)[7];
    reinterpret_cast<uint8_t*>(pOutput)[1] =
        reinterpret_cast<const uint8_t*>(&uInput)[6];
    reinterpret_cast<uint8_t*>(pOutput)[2] =
        reinterpret_cast<const uint8_t*>(&uInput)[5];
    reinterpret_cast<uint8_t*>(pOutput)[3] =
        reinterpret_cast<const uint8_t*>(&uInput)[4];
    reinterpret_cast<uint8_t*>(pOutput)[4] =
        reinterpret_cast<const uint8_t*>(&uInput)[3];
    reinterpret_cast<uint8_t*>(pOutput)[5] =
        reinterpret_cast<const uint8_t*>(&uInput)[2];
    reinterpret_cast<uint8_t*>(pOutput)[6] =
        reinterpret_cast<const uint8_t*>(&uInput)[1];
    reinterpret_cast<uint8_t*>(pOutput)[7] =
        reinterpret_cast<const uint8_t*>(&uInput)[0];
}
#endif

/*! ************************************

    \brief Store a float value to memory with endian swapping with byte
        alignment.

    Assuming the output pointer is unaligned, it will store data a byte at a
    time into a 32 bit float value in reversed endian.

    \param pOutput Pointer to a float value.
    \param fInput The float value.

    \sa NativeEndian::Store(float*,float) or
        SwapEndian::Store(float *,float).

***************************************/

void BURGER_API Burger::SwapEndian::StoreAny(
    float* pOutput, float fInput) BURGER_NOEXCEPT
{
    reinterpret_cast<uint8_t*>(pOutput)[0] =
        reinterpret_cast<const uint8_t*>(&fInput)[3];
    reinterpret_cast<uint8_t*>(pOutput)[1] =
        reinterpret_cast<const uint8_t*>(&fInput)[2];
    reinterpret_cast<uint8_t*>(pOutput)[2] =
        reinterpret_cast<const uint8_t*>(&fInput)[1];
    reinterpret_cast<uint8_t*>(pOutput)[3] =
        reinterpret_cast<const uint8_t*>(&fInput)[0];
}

/*! ************************************

    \brief Store a double value to memory with endian swapping with byte
        alignment.

    Assuming the output pointer is unaligned, it will store data a byte at a
    time into a 64 bit float value in reversed endian.

    \param pOutput Pointer to a double value.
    \param dInput The double value.

    \sa NativeEndian::Store(double*,double)

***************************************/

void BURGER_API Burger::SwapEndian::StoreAny(
    double* pOutput, double dInput) BURGER_NOEXCEPT
{
    reinterpret_cast<uint8_t*>(pOutput)[0] =
        reinterpret_cast<const uint8_t*>(&dInput)[7];
    reinterpret_cast<uint8_t*>(pOutput)[1] =
        reinterpret_cast<const uint8_t*>(&dInput)[6];
    reinterpret_cast<uint8_t*>(pOutput)[2] =
        reinterpret_cast<const uint8_t*>(&dInput)[5];
    reinterpret_cast<uint8_t*>(pOutput)[3] =
        reinterpret_cast<const uint8_t*>(&dInput)[4];
    reinterpret_cast<uint8_t*>(pOutput)[4] =
        reinterpret_cast<const uint8_t*>(&dInput)[3];
    reinterpret_cast<uint8_t*>(pOutput)[5] =
        reinterpret_cast<const uint8_t*>(&dInput)[2];
    reinterpret_cast<uint8_t*>(pOutput)[6] =
        reinterpret_cast<const uint8_t*>(&dInput)[1];
    reinterpret_cast<uint8_t*>(pOutput)[7] =
        reinterpret_cast<const uint8_t*>(&dInput)[0];
}

/*! ************************************

    \fn Burger::SwapEndian::Fixup(char *pInput)
    \brief Does nothing.

    Single byte variables can't be endian swapped.

    \param pInput Pointer to an 8 bit value.

    \sa NativeEndian::Fixup(char*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Fixup(uint8_t *pInput)
    \brief Does nothing.

    Single byte variables can't be endian swapped.

    \param pInput Pointer to an 8 bit value.

    \sa NativeEndian::Fixup(uint8_t*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Fixup(int8_t *pInput)
    \brief Does nothing.

    Single byte variables can't be endian swapped.

    \param pInput Pointer to an 8 bit value.

    \sa NativeEndian::Fixup(int8_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Fixup(uint16_t *pInput);
    \brief Reverse the endian of a 16-bit value

    Given a pointer to a 16-bit value in memory, load it and swap the bytes
    so that 0x1234 becomes 0x3412.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.

    \sa NativeEndian::Fixup(uint16_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Fixup(uint32_t *pInput);
    \brief Reverse the endian of a 32-bit value

    Given a pointer to a 32-bit value in memory, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.

    \sa SwapEndian::FixupAny(uint32_t *) or NativeEndian::Fixup(uint32_t *)

***************************************/

/*! ************************************

    \brief Reverse the endian of a 64-bit value

    Given a pointer to a 64-bit value in memory, load it and swap the bytes
    so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.

    \sa SwapEndian::FixupAny(uint64_t *) or NativeEndian::Fixup(uint64_t *)

***************************************/

#if defined(BURGER_64BITCPU) && !defined(DOXYGEN)

#elif defined(BURGER_PPC) && defined(BURGER_METROWERKS)

#else
void BURGER_API Burger::SwapEndian::Fixup(uint64_t* pInput) BURGER_NOEXCEPT
{
    uint32_t uLow = SwapEndian::Load(&reinterpret_cast<uint32_t*>(pInput)[0]);
    uint32_t uHigh = SwapEndian::Load(&reinterpret_cast<uint32_t*>(pInput)[1]);
    reinterpret_cast<uint32_t*>(pInput)[0] = uHigh;
    reinterpret_cast<uint32_t*>(pInput)[1] = uLow;
}
#endif

/*! ************************************

    \fn Burger::SwapEndian::FixupAny(char *pInput)
    \brief Does nothing.

    Single byte variables can't be endian swapped.

    \param pInput Pointer to an 8 bit value.

    \sa NativeEndian::FixupAny(char*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::FixupAny(uint8_t *pInput)
    \brief Does nothing.

    Single byte variables can't be endian swapped.

    \param pInput Pointer to an 8 bit value.

    \sa NativeEndian::FixupAny(uint8_t*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::FixupAny(int8_t *pInput)
    \brief Does nothing.

    Single byte variables can't be endian swapped.

    \param pInput Pointer to an 8 bit value.

    \sa NativeEndian::FixupAny(int8_t *)

***************************************/

/*! ************************************

    \brief Reverse the endian of a 16-bit value

    Given a pointer to a 16-bit value in memory, load it and swap the bytes
    so that 0x1234 becomes 0x3412. The pointer does not have
    to be 16-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.

    \sa NativeEndian::FixupAny(uint16_t *)

***************************************/

void BURGER_API Burger::SwapEndian::FixupAny(uint16_t* pInput) BURGER_NOEXCEPT
{
    uint8_t bTemp1 = reinterpret_cast<uint8_t*>(pInput)[0];
    uint8_t bTemp2 = reinterpret_cast<uint8_t*>(pInput)[1];
    reinterpret_cast<uint8_t*>(pInput)[1] = bTemp1;
    reinterpret_cast<uint8_t*>(pInput)[0] = bTemp2;
}

/*! ************************************

    \brief Reverse the endian of a 32-bit value

    Given a pointer to a 32-bit value in memory, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412. The pointer does not have
    to be 32-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.

    \sa SwapEndian::Fixup(uint32_t *) or NativeEndian::FixupAny(uint32_t *)

***************************************/

void BURGER_API Burger::SwapEndian::FixupAny(uint32_t* pInput) BURGER_NOEXCEPT
{
    uint8_t bTemp1 = reinterpret_cast<uint8_t*>(pInput)[0];
    uint8_t bTemp2 = reinterpret_cast<uint8_t*>(pInput)[3];
    reinterpret_cast<uint8_t*>(pInput)[3] = bTemp1;
    reinterpret_cast<uint8_t*>(pInput)[0] = bTemp2;
    bTemp1 = reinterpret_cast<uint8_t*>(pInput)[1];
    bTemp2 = reinterpret_cast<uint8_t*>(pInput)[2];
    reinterpret_cast<uint8_t*>(pInput)[2] = bTemp1;
    reinterpret_cast<uint8_t*>(pInput)[1] = bTemp2;
}

/*! ************************************

    \brief Reverse the endian of a 64-bit value

    Given a pointer to a 64-bit value in memory, load it and swap the bytes
    so that 0x123456789aBCDEF0 becomes 0xF0DEBC9A78563412. The pointer does not
    have to be 64-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.

    \sa SwapEndian::Fixup(uint64_t *) or NativeEndian::FixupAny(uint64_t *)

***************************************/

void BURGER_API Burger::SwapEndian::FixupAny(uint64_t* pInput) BURGER_NOEXCEPT
{
    uint8_t bTemp1 = reinterpret_cast<uint8_t*>(pInput)[0];
    uint8_t bTemp2 = reinterpret_cast<uint8_t*>(pInput)[7];
    reinterpret_cast<uint8_t*>(pInput)[7] = bTemp1;
    reinterpret_cast<uint8_t*>(pInput)[0] = bTemp2;
    bTemp1 = reinterpret_cast<uint8_t*>(pInput)[1];
    bTemp2 = reinterpret_cast<uint8_t*>(pInput)[6];
    reinterpret_cast<uint8_t*>(pInput)[6] = bTemp1;
    reinterpret_cast<uint8_t*>(pInput)[1] = bTemp2;
    bTemp1 = reinterpret_cast<uint8_t*>(pInput)[2];
    bTemp2 = reinterpret_cast<uint8_t*>(pInput)[5];
    reinterpret_cast<uint8_t*>(pInput)[5] = bTemp1;
    reinterpret_cast<uint8_t*>(pInput)[2] = bTemp2;
    bTemp1 = reinterpret_cast<uint8_t*>(pInput)[3];
    bTemp2 = reinterpret_cast<uint8_t*>(pInput)[4];
    reinterpret_cast<uint8_t*>(pInput)[4] = bTemp1;
    reinterpret_cast<uint8_t*>(pInput)[3] = bTemp2;
}

/*! ************************************

    \fn Burger::SwapEndian::LoadAny(const int16_t *pInput);
    \brief Reverse the endian of a 16 bit signed integer with byte alignment.

    Given a pointer to a 16 bit value, load it and swap the bytes
    so that 0x1234 becomes 0x3412. The pointer does not have
    to be 16-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to a value to endian convert
    \return The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::LoadAny(const uint16_t *).

    \sa SwapEndian::LoadAny(const uint16_t*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::LoadAny(const int32_t *pInput);
    \brief Reverse the endian of a 32 bit signed integer with byte alignment.

    Given a pointer to a 32 bit value, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412. The pointer does not have
    to be 32-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to a value to endian convert
    \return The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::LoadAny(const uint32_t *).

    \sa SwapEndian::LoadAny(const uint32_t*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::LoadAny(const int64_t *pInput);
    \brief Reverse the endian of a 64 bit signed integer with byte alignment.

    Given a pointer to a 64 bit value, load it and swap the bytes
    so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412. The pointer does not
    have to be 64-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to a value to endian convert
    \return The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::LoadAny(const uint64_t *).

    \sa SwapEndian::LoadAny(const uint64_t*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::StoreAny(int16_t *pOutput,int16_t iInput);
    \brief Reverse the endian of a 16 bit signed integer with byte alignment.

    Given a 16 bit value, swap the bytes
    so that 0x1234 becomes 0x3412 and store it. The pointer does not have
    to be 16-bit aligned. uint8_t alignment is acceptable.

    \param pOutput Pointer to a value to endian convert
    \param iInput The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::StoreAny(uint16_t*,uint16_t).

    \sa SwapEndian::StoreAny(uint16_t*,uint16_t)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::StoreAny(int32_t *pOutput,int32_t iInput);
    \brief Reverse the endian of a 32 bit signed integer with byte alignment.

    Given a 32 bit value, swap the bytes so that 0x12345678 becomes 0x78563412
    and store it. The pointer does not have to be 32-bit aligned. uint8_t
    alignment is acceptable.

    \param pOutput Pointer to a value to endian convert
    \param iInput The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::StoreAny(uint32_t*,uint32_t).

    \sa SwapEndian::StoreAny(uint32_t*,uint32_t)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::StoreAny(int64_t *pOutput,int64_t iInput);
    \brief Reverse the endian of a 64 bit signed integer with byte alignment.

    Given a 64 bit value, swap the bytes
    so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412 and store it. The
    pointer does not have to be 64-bit aligned. uint8_t alignment is
    acceptable.

    \param pOutput Pointer to a value to endian convert
    \param iInput The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::StoreAny(uint64_t*,uint64_t).

    \sa SwapEndian::StoreAny(uint64_t*,uint64_t)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::LoadAny(const unsigned int *pInput);
    \brief Reverse the endian of a 32 bit unsigned integer with byte alignment.

    Given a pointer to a 32 bit value, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412. The pointer does not have
    to be 32-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to a value to endian convert
    \return The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::LoadAny(const uint32_t *).

    \sa SwapEndian::LoadAny(const uint32_t*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::LoadAny(const signed int *pInput);
    \brief Reverse the endian of a 32 bit signed integer with byte alignment.

    Given a pointer to a 32 bit value, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412. The pointer does not have
    to be 32-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to a value to endian convert
    \return The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::LoadAny(const uint32_t *).

    \sa SwapEndian::LoadAny(const uint32_t*)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::StoreAny(unsigned int *pOutput,unsigned int uInput);
    \brief Reverse the endian of a 32 bit unsigned integer with byte alignment.

    Given a 32 bit value, swap the bytes so that 0x12345678 becomes 0x78563412
    and store it. The pointer does not have to be 32-bit aligned. uint8_t
    alignment is acceptable.

    \param pOutput Pointer to a value to endian convert
    \param uInput The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::StoreAny(uint32_t*,uint32_t).

    \sa SwapEndian::StoreAny(uint32_t*,uint32_t)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::StoreAny(signed int *pOutput,signed int iInput);
    \brief Reverse the endian of a 32 bit signed integer with byte alignment.

    Given a 32 bit value, swap the bytes so that 0x12345678 becomes 0x78563412
    and store it. The pointer does not have to be 32-bit aligned. uint8_t
    alignment is acceptable.

    \param pOutput Pointer to a value to endian convert
    \param iInput The value with the bytes swapped.

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::StoreAny(Woruint32_td32*,uint32_t).

    \sa SwapEndian::StoreAny(uint32_t*,uint32_t)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Fixup(int16_t *pInput);
    \brief Reverse the endian of a 16-bit value

    Given a pointer to a 16-bit value in memory, load it and swap the bytes
    so that 0x1234 becomes 0x3412.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Fixup(uint16_t *).

    \sa SwapEndian::Fixup(uint16_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Fixup(int32_t *pInput);
    \brief Reverse the endian of a 32-bit value

    Given a pointer to a 32-bit value in memory, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Fixup(uint32_t *).

    \sa SwapEndian::Fixup(uint32_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Fixup(int64_t *pInput);
    \brief Reverse the endian of a 64-bit value

    Given a pointer to a 64-bit value in memory, load it and swap the bytes
    so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Fixup(uint64_t*).

    \sa SwapEndian::Fixup(uint64_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Fixup(unsigned int *pInput);
    \brief Reverse the endian of a 32-bit value

    Given a pointer to a 32-bit value in memory, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Fixup(uint32_t*).

    \sa SwapEndian::Fixup(uint32_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Fixup(signed int *pInput);
    \brief Reverse the endian of a 32-bit value

    Given a pointer to a 32-bit value in memory, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Fixup(uint32_t *).

    \sa SwapEndian::Fixup(uint32_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Fixup(float *pInput);
    \brief Reverse the endian of a 32-bit float value

    Given a pointer to a 32-bit float value in memory, load it and swap the
    bytes so that 0x12345678 becomes 0x78563412.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Fixup(uint32_t *).

    \sa SwapEndian::Fixup(uint32_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::Fixup(double *pInput);
    \brief Reverse the endian of a 64-bit float value

    Given a pointer to a 64-bit value in memory, load it and swap the bytes
    so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::Fixup(uint64_t *).

    \sa SwapEndian::Fixup(uint64_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::FixupAny(int16_t *pInput);
    \brief Reverse the endian of a 16-bit value with byte alignment.

    Given a pointer to a 16-bit value in memory, load it and swap the bytes
    so that 0x1234 becomes 0x3412. The pointer does not have
    to be 16-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::FixupAny(uint16_t*).

    \sa SwapEndian::FixupAny(uint16_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::FixupAny(int32_t *pInput);
    \brief Reverse the endian of a 32-bit value with byte alignment.

    Given a pointer to a 32-bit value in memory, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412. The pointer does not have
    to be 16-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::FixupAny(uint32_t*).

    \sa SwapEndian::FixupAny(uint32_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::FixupAny(int64_t *pInput);
    \brief Reverse the endian of a 64-bit value with byte alignment.

    Given a pointer to a 64-bit value in memory, load it and swap the bytes
    so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412. The pointer does not
    have to be 16-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::FixupAny(uint64_t*).

    \sa SwapEndian::FixupAny(uint64_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::FixupAny(unsigned int *pInput);
    \brief Reverse the endian of a 32-bit value with byte alignment.

    Given a pointer to a 32-bit value in memory, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412. The pointer does not have
    to be 16-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::FixupAny(uint32_t*).

    \sa SwapEndian::FixupAny(uint32_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::FixupAny(signed int *pInput);
    \brief Reverse the endian of a 32-bit value with byte alignment.

    Given a pointer to a 32-bit value in memory, load it and swap the bytes
    so that 0x12345678 becomes 0x78563412. The pointer does not have
    to be 16-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::FixupAny(uint32_t*).

    \sa SwapEndian::FixupAny(uint32_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::FixupAny(float *pInput);
    \brief Reverse the endian of a 32-bit float value with byte alignment.

    Given a pointer to a 32-bit float value in memory, load it and swap the
    bytes so that 0x12345678 becomes 0x78563412. The pointer does not have to be
    16-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
    SwapEndian::FixupAny(uint32_t *).

    \sa SwapEndian::FixupAny(uint32_t *)

***************************************/

/*! ************************************

    \fn Burger::SwapEndian::FixupAny(double *pInput);
    \brief Reverse the endian of a 64-bit float value with byte alignment.

    Given a pointer to a 64-bit value in memory, load it and swap the bytes
    so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412. The pointer does not
    have to be 16-bit aligned. uint8_t alignment is acceptable.

    \param pInput Pointer to the value to endian convert

    \note \ref NULL pointers are illegal and will page fault.
    \note This function is inlined to actually use
        SwapEndian::FixupAny(uint64_t *).

    \sa SwapEndian::FixupAny(uint64_t *)

***************************************/

/*! ************************************

    \struct Burger::NativeEndian
    \brief Loads a 16, 32 or 64 bit value with no byte swapping.

    The classes Burger::LittleEndian and Burger::BigEndian either map to \ref
    NativeEndian or \ref SwapEndian. If the machine's endian matches the class,
    then it maps to this class.

    This class does nothing for most functions by design. It is meant to vanish
    when the program is asking for no endian swapping since the data being read
    is the same endian as the machine.

    The only functions that do not disappear are the LoadAny(??? *) group of
    calls since they have the ability to fetch a 16, 32 or 64 bit value
    regardless of the alignment of the data pointer. These are useful in
    grabbing data from a byte stream and won't trigger an alignment access
    fault.

    Under most circumstances, you will not call this class directly.

    \sa \ref SwapEndian, \ref LittleEndian and \ref BigEndian

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(char iInput)
    \brief Load an 8 bit signed value to memory with byte alignment.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param iInput An 8 bit value.
    \return The 8 bit signed value.

    \sa SwapEndian::Load(char)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(uint8_t uInput)
    \brief Load an 8 bit unsigned value to memory with byte alignment.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param uInput An 8 bit value.
    \return The 8 bit unsigned value.

    \sa SwapEndian::Load(uint8_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(int8_t iInput)
    \brief Load an 8 bit signed value to memory with byte alignment.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param iInput An 8 bit value.
    \return The 8 bit signed value.

    \sa SwapEndian::Load(int8_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(uint16_t uInput);
    \brief Fetch a 16 bit value.

    Pass a 16 bit value through with no change. This function is usually
    optimized into oblivion.

    \param uInput Unsigned 16 bit input.
    \return \a uInput untouched.

    \sa SwapEndian::Load(uint16_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(int16_t iInput);
    \brief Fetch a 16 bit value.

    Pass a 16 bit value through with no change. This function is usually
    optimized into oblivion.

    \param iInput Signed 16 bit input.
    \return \a iInput untouched.

    \sa SwapEndian::Load(int16_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(uint32_t uInput);
    \brief Fetch a 32 bit value.

    Pass a 32 bit value through with no change. This function is usually
    optimized into oblivion.

    \param uInput Unsigned 32 bit input.
    \return \a uInput untouched.

    \sa SwapEndian::Load(uint32_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(int32_t iInput);
    \brief Fetch a 32 bit value.

    Pass a 32 bit value through with no change. This function is usually
    optimized into oblivion.

    \param iInput Signed 32 bit input.
    \return \a iInput untouched.

    \sa SwapEndian::Load(int32_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(uint64_t uInput);
    \brief Fetch a 64 bit value.

    Pass a 64 bit value through with no change. This function is usually
    optimized into oblivion.

    \param uInput Unsigned 64 bit input.
    \return \a uInput untouched.

    \sa SwapEndian::Load(uint64_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(int64_t iInput);
    \brief Fetch a 64 bit value.

    Pass a 64 bit value through with no change. This function is usually
    optimized into oblivion.

    \param iInput Signed 64 bit input.
    \return \a iInput untouched.

    \sa SwapEndian::Load(int64_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(float fInput);
    \brief Fetch a float value.

    Pass a float value through with no change. This function is usually
    optimized into oblivion.

    \param fInput A float input.
    \return \a fInput untouched.

    \sa SwapEndian::Load(float)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(double dInput);
    \brief Fetch a double value.

    Pass a double value through with no change. This function is usually
    optimized into oblivion.

    \param dInput A double input.
    \return \a dInput untouched.

    \sa SwapEndian::Load(double)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(wchar_t uInput);
    \brief Fetch a 16 bit value.

    Pass a 16 bit value through with no change. This function is usually
    optimized into oblivion.

    \param uInput Unsigned 16 bit input.
    \return \a uInput untouched.

    \sa SwapEndian::Load(wchar_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(unsigned int uInput);
    \brief Fetch an unsigned integer value.

    Pass a 16-32 bit integer value through with no change. This function is
    usually optimized into oblivion.

    \param uInput Unsigned integer input.
    \return \a uInput untouched.

    \sa SwapEndian::Load(unsigned int)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(signed int iInput);
    \brief Fetch a signed integer value.

    Pass a 16-32 bit signed integer value through with no change. This function
    is usually optimized into oblivion.

    \param iInput Signed integer input.
    \return \a iInput untouched.

    \sa SwapEndian::Load(signed int)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(unsigned long uInput);
    \brief Fetch an unsigned long integer value.

    Pass a 32-64 bit integer value through with no change. This function is
    usually optimized into oblivion.

    \param uInput Unsigned integer input.
    \return \a uInput untouched.

    \sa SwapEndian::Load(unsigned long)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(signed long iInput);
    \brief Fetch a signed long integer value.

    Pass a 32-64 bit signed integer value through with no change. This function
    is usually optimized into oblivion.

    \param iInput Signed integer input.
    \return \a iInput untouched.

    \sa SwapEndian::Load(signed long)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const char *pInput)
    \brief Load an 8 bit signed value to memory with byte alignment.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pInput Pointer to an 8 bit value.
    \return The 8 bit signed value.

    \sa SwapEndian::Load(const char*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const uint8_t *pInput)
    \brief Load an 8 bit unsigned value to memory with byte alignment.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pInput Pointer to an 8 bit value.
    \return The 8 bit unsigned value.

    \sa SwapEndian::Load(const uint8_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const int8_t *pInput)
    \brief Load an 8 bit signed value to memory with byte alignment.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pInput Pointer to an 8 bit value.
    \return The 8 bit signed value.

    \sa SwapEndian::Load(const int8_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const uint16_t *pInput);
    \brief Fetch a 16 bit value from memory.

    This function is usually optimized into a simple load operation.
    It assumes that the data is 16 bit aligned.

    \param pInput Pointer to an unsigned 16 bit value.
    \return The 16 bit unsigned value.

    \sa SwapEndian::Load(const uint16_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const int16_t *pInput);
    \brief Fetch a 16 bit value from memory.

    This function is usually optimized into a simple load operation.
    It assumes that the data is 16 bit aligned.

    \param pInput Pointer to an signed 16 bit value.
    \return The 16 bit signed value.

    \sa SwapEndian::Load(const int16_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const uint32_t *pInput);
    \brief Fetch a 32 bit value from memory.

    This function is usually optimized into a simple load operation.
    It assumes that the data is 32 bit aligned.

    \param pInput Pointer to an unsigned 32 bit value.
    \return The 32 bit unsigned value.

    \sa SwapEndian::Load(const uint32_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const int32_t *pInput);
    \brief Fetch a 32 bit value from memory.

    This function is usually optimized into a simple load operation.
    It assumes that the data is 32 bit aligned.

    \param pInput Pointer to an signed 32 bit value.
    \return The 32 bit signed value.

    \sa SwapEndian::Load(const int32_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const uint64_t *pInput);
    \brief Fetch a 64 bit value from memory.

    This function is usually optimized into a simple load operation.
    It assumes that the data is 64 bit aligned.

    \param pInput Pointer to an unsigned 64 bit value.
    \return The 64 bit unsigned value.

    \sa SwapEndian::Load(const uint64_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const int64_t *pInput);
    \brief Fetch a 64 bit value from memory.

    This function is usually optimized into a simple load operation.
    It assumes that the data is 64 bit aligned.

    \param pInput Pointer to an signed 64 bit value.
    \return The 64 bit signed value.

    \sa SwapEndian::Load(const int64_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const float *pInput);
    \brief Fetch a float value from memory.

    This function is usually optimized into a simple load operation.
    It assumes that the data is 32 bit aligned.

    \param pInput Pointer to a float value.
    \return The float value.

    \sa SwapEndian::Load(const float*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const double *pInput);
    \brief Fetch a double value from memory.

    This function is usually optimized into a simple load operation.
    It assumes that the data is 64 bit aligned.

    \param pInput Pointer to a double value.
    \return The double value.

    \sa SwapEndian::Load(const double*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const wchar_t *pInput);
    \brief Fetch a 16 bit value from memory.

    This function is usually optimized into a simple load operation.
    It assumes that the data is 16 bit aligned.

    \param pInput Pointer to an unsigned 16 bit value.
    \return The 16 bit unsigned value.

    \sa SwapEndian::Load(const wchar_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const unsigned int *pInput);
    \brief Fetch an unsigned integer value from memory.

    This function is usually optimized into a simple load operation.
    It assumes that the data is aligned to the size of a native integer (Usually
    16-32 bits).

    \param pInput Pointer to an unsigned integer value.
    \return The unsigned integer value.

    \sa SwapEndian::Load(const unsigned int*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const signed int *pInput);
    \brief Fetch a signed integer value from memory.

    This function is usually optimized into a simple load operation.
    It assumes that the data is aligned to the size of a native integer (Usually
    16-32 bits).

    \param pInput Pointer to a signed integer value.
    \return The signed integer value.

    \sa SwapEndian::Load(const signed int*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const unsigned long *pInput);
    \brief Fetch an unsigned integer value from memory.

    This function is usually optimized into a simple load operation.
    It assumes that the data is aligned to the size of a native integer (Usually
    32-64 bits).

    \param pInput Pointer to an unsigned integer value.
    \return The unsigned integer value.

    \sa SwapEndian::Load(const unsigned long*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Load(const signed long *pInput);
    \brief Fetch a signed integer value from memory.

    This function is usually optimized into a simple load operation.
    It assumes that the data is aligned to the size of a native integer (Usually
    32-64 bits).

    \param pInput Pointer to a signed integer value.
    \return The signed integer value.

    \sa SwapEndian::Load(const signed long*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(char *pOutput,char iInput)
    \brief Store an 8 bit signed value to memory with byte alignment.

    \note Since 8 bit values can't be endian swapped, this function only exists
    for completeness sake

    \param pOutput Pointer to an 8 bit value.
    \param iInput The 8 bit signed value.

    \sa SwapEndian::Store(char*,char)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(uint8_t *pOutput,uint8_t uInput)
    \brief Store an 8 bit unsigned value to memory with byte alignment.

    \note Since 8 bit values can't be endian swapped, this function only exists
    for completeness sake

    \param pOutput Pointer to an 8 bit value.
    \param uInput The 8 bit unsigned value.

    \sa SwapEndian::Store(uint8_t*,char)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(int8_t *pOutput,int8_t iInput)
    \brief Store an 8 bit signed value to memory with byte alignment.

    \note Since 8 bit values can't be endian swapped, this function only exists
    for completeness sake

    \param pOutput Pointer to an 8 bit value.
    \param iInput The 8 bit signed value.

    \sa SwapEndian::Store(int8_t*,int8_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(uint16_t *pOutput,uint16_t uInput);
    \brief Store a 16 bit value to memory.

    This function is usually optimized into a simple store operation.
    It assumes that the data is 16 bit aligned.

    \param pOutput Pointer to an unsigned 16 bit value.
    \param uInput The 16 bit unsigned value.

    \sa SwapEndian::Store(uint16_t*,uint16_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(int16_t *pOutput,int16_t iInput);
    \brief Store a 16 bit value to memory.

    This function is usually optimized into a simple store operation.
    It assumes that the data is 16 bit aligned.

    \param pOutput Pointer to an signed 16 bit value.
    \param iInput The 16 bit signed value.

    \sa SwapEndian::Store(int16_t*,int16_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(uint32_t *pOutput,uint32_t uInput);
    \brief Store a 32 bit value to memory.

    This function is usually optimized into a simple store operation.
    It assumes that the data is 32 bit aligned.

    \param pOutput Pointer to an unsigned 32 bit value.
    \param uInput The 32 bit unsigned value.

    \sa SwapEndian::Store(uint32_t*,uint32_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(int32_t *pOutput,int32_t iInput);
    \brief Store a 32 bit value to memory.

    This function is usually optimized into a simple store operation.
    It assumes that the data is 32 bit aligned.

    \param pOutput Pointer to an signed 32 bit value.
    \param iInput The 32 bit signed value.

    \sa SwapEndian::Store(int32_t*,int32_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(uint64_t *pOutput,uint64_t uInput);
    \brief Store a 64 bit value to memory.

    This function is usually optimized into a simple store operation.
    It assumes that the data is 64 bit aligned.

    \param pOutput Pointer to an unsigned 64 bit value.
    \param uInput The 64 bit unsigned value.

    \sa SwapEndian::Store(uint64_t*,uint64_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(int64_t *pOutput,int64_t iInput);
    \brief Store a 64 bit value to memory.

    This function is usually optimized into a simple store operation.
    It assumes that the data is 64 bit aligned.

    \param pOutput Pointer to an signed 64 bit value.
    \param iInput The 64 bit signed value.

    \sa SwapEndian::Store(int64_t*,int64_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(float *pOutput,float fInput);
    \brief Store a float value to memory.

    This function is usually optimized into a simple store operation.
    It assumes that the data is 32 bit aligned.

    \param pOutput Pointer to a float value.
    \param fInput The float value.

    \sa SwapEndian::Store(float*,float)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(double *pOutput,double dInput);
    \brief Store a double value to memory.

    This function is usually optimized into a simple store operation.
    It assumes that the data is 64 bit aligned.

    \param pOutput Pointer to a double value.
    \param dInput The double value.

    \sa SwapEndian::Store(double*,double)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(wchar_t *pOutput,wchar_t uInput);
    \brief Store a wchar_t value to memory.

    This function is usually optimized into a simple store operation.
    It assumes that the data is aligned to the size of a native integer (Usually
    16-32 bits).

    \param pOutput Pointer to an unsigned integer value.
    \param uInput The unsigned integer value.

    \sa SwapEndian::Store(wchar_t*,wchar_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(unsigned int *pOutput,unsigned int uInput);
    \brief Store an unsigned integer value to memory.

    This function is usually optimized into a simple store operation.
    It assumes that the data is aligned to the size of a native integer (Usually
    16-32 bits).

    \param pOutput Pointer to an unsigned integer value.
    \param uInput The unsigned integer value.

    \sa SwapEndian::Store(unsigned int*,unsigned int)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(signed int *pOutput,signed int iInput);
    \brief Store a signed integer value to memory.

    This function is usually optimized into a simple store operation.
    It assumes that the data is aligned to the size of a native integer (Usually
    16-32 bits).

    \param pOutput Pointer to a signed integer value.
    \param iInput The signed integer value.

    \sa SwapEndian::Store(signed int*,signed int)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(unsigned long *pOutput,unsigned long
        uInput);
    \brief Store an unsigned integer value to memory.

    This function is usually optimized into a simple store operation.
    It assumes that the data is aligned to the size of a native integer (Usually
    32-64 bits).

    \param pOutput Pointer to an unsigned integer value.
    \param uInput The unsigned integer value.

    \sa SwapEndian::Store(unsigned long*,unsigned long)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Store(signed long *pOutput,signed long iInput);
    \brief Store a signed integer value to memory.

    This function is usually optimized into a simple store operation.
    It assumes that the data is aligned to the size of a native integer (Usually
    32-64 bits).

    \param pOutput Pointer to a signed integer value.
    \param iInput The signed integer value.

    \sa SwapEndian::Store(signed long*,signed long)

***************************************/

/*! ************************************

    \brief Fetch a 16 bit unsigned value from memory with byte alignment.

    Assuming the data is unaligned, it will grab data a byte at a time and
    reconstruct it into a 16 bit value in native endian.

    \param pInput Pointer to a 16 bit value.
    \return The 16 bit unsigned value.

    \sa SwapEndian::LoadAny(const uint16_t*)

***************************************/

#if (defined(BURGER_INTEL) || defined(BURGER_ARM64)) && \
    !defined(DOXYGEN) // Inlined

#else
uint16_t BURGER_API Burger::NativeEndian::LoadAny(
    const uint16_t* pInput) BURGER_NOEXCEPT
{
    uint_t uLow = reinterpret_cast<const uint8_t*>(pInput)[0]; // Grab 16 bits
    uint_t uHigh = reinterpret_cast<const uint8_t*>(pInput)[1];
#if defined(BURGER_BIGENDIAN)
    uint16_t uResult =
        static_cast<uint16_t>((uLow << 8) | uHigh); // PPC likes |
#else
    uint16_t uResult =
        static_cast<uint16_t>(uLow + (uHigh << 8)); // Intel likes +
#endif
    return uResult;
}
#endif

/*! ************************************

    \brief Fetch a 32 bit unsigned value from memory with byte alignment.

    Assuming the data is unaligned, it will grab data a byte at a time
    and reconstruct it into a 32 bit value in native endian.

    \param pInput Pointer to a 32 bit value.
    \return The 32 bit unsigned value.

    \sa SwapEndian::LoadAny(const uint32_t*)

***************************************/

#if (defined(BURGER_INTEL) || defined(BURGER_ARM64)) && \
    !defined(DOXYGEN) // Inlined

#else
uint32_t BURGER_API Burger::NativeEndian::LoadAny(
    const uint32_t* pInput) BURGER_NOEXCEPT
{
    uint_t uLow1 = reinterpret_cast<const uint8_t*>(pInput)[0]; // Grab 32 bits
    uint_t uLow2 = reinterpret_cast<const uint8_t*>(pInput)[1];
    uint_t uLow3 = reinterpret_cast<const uint8_t*>(pInput)[2];
    uint_t uLow4 = reinterpret_cast<const uint8_t*>(pInput)[3];
#if defined(BURGER_BIGENDIAN)
    uint32_t uResult = static_cast<uint32_t>(
        (uLow1 << 24) | (uLow2 << 16) | (uLow3 << 8) | uLow4); // PPC likes |
#else
    uint32_t uResult = static_cast<uint32_t>(
        uLow1 + (uLow2 << 8) + (uLow3 << 16) + (uLow4 << 24)); // Intel likes +
#endif
    return uResult;
}
#endif

/*! ************************************

    \brief Fetch a 64 bit unsigned value from memory with byte alignment.

    Assuming the data is unaligned, it will grab data a byte at a time
    and reconstruct it into a 64 bit value in native endian.

    \param pInput Pointer to a 64 bit value.
    \return The 64 bit unsigned value.

    \sa SwapEndian::LoadAny(const uint64_t*)

***************************************/

#if (defined(BURGER_INTEL) || defined(BURGER_ARM64)) && !defined(DOXYGEN)

#else
uint64_t BURGER_API Burger::NativeEndian::LoadAny(
    const uint64_t* pInput) BURGER_NOEXCEPT
{
    uint64_t uResult;

    reinterpret_cast<uint8_t*>(&uResult)[0] =
        reinterpret_cast<const uint8_t*>(pInput)[0];
    reinterpret_cast<uint8_t*>(&uResult)[1] =
        reinterpret_cast<const uint8_t*>(pInput)[1];
    reinterpret_cast<uint8_t*>(&uResult)[2] =
        reinterpret_cast<const uint8_t*>(pInput)[2];
    reinterpret_cast<uint8_t*>(&uResult)[3] =
        reinterpret_cast<const uint8_t*>(pInput)[3];
    reinterpret_cast<uint8_t*>(&uResult)[4] =
        reinterpret_cast<const uint8_t*>(pInput)[4];
    reinterpret_cast<uint8_t*>(&uResult)[5] =
        reinterpret_cast<const uint8_t*>(pInput)[5];
    reinterpret_cast<uint8_t*>(&uResult)[6] =
        reinterpret_cast<const uint8_t*>(pInput)[6];
    reinterpret_cast<uint8_t*>(&uResult)[7] =
        reinterpret_cast<const uint8_t*>(pInput)[7];
    // Return the 64 bit value in a register
    return uResult;
}
#endif

/*! ************************************

    \brief Fetch a float value from memory with byte alignment.

    Assuming the data is unaligned, it will grab data a byte at a time
    and reconstruct it into a float value in native endian.

    \param pInput Pointer to a float value.
    \return The float value.

    \sa SwapEndian::LoadAny(const float*)

***************************************/

#if (defined(BURGER_INTEL) || defined(BURGER_ARM64)) && \
    !defined(DOXYGEN) // Inlined

#else
float BURGER_API Burger::NativeEndian::LoadAny(
    const float* pInput) BURGER_NOEXCEPT
{
    uint32_float_t uTemp;
    uTemp.w =
        LoadAny(static_cast<const uint32_t*>(static_cast<const void*>(pInput)));
    return uTemp.f;
}

#endif

/*! ************************************

    \brief Fetch a double value from memory with byte alignment.

    Assuming the data is unaligned, it will grab data a byte at a time
    and reconstruct it into a double value in native endian.

    \param pInput Pointer to a double value.
    \return The double value.

    \sa SwapEndian::LoadAny(const double*)

***************************************/

#if (defined(BURGER_INTEL) || defined(BURGER_ARM64)) && \
    !defined(DOXYGEN) // Inlined

#else
double BURGER_API Burger::NativeEndian::LoadAny(
    const double* pInput) BURGER_NOEXCEPT
{
    uint64_double_t uTemp;
    uTemp.w =
        LoadAny(static_cast<const uint64_t*>(static_cast<const void*>(pInput)));
    return uTemp.d;
}

#endif

/*! ************************************

    \fn Burger::NativeEndian::LoadAny(const char *pInput)
    \brief Load an 8 bit signed value to memory with byte alignment.

    It assumes that the input pointer is unaligned.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pInput Pointer to an 8 bit value.
    \return The 8 bit signed value.

    \sa SwapEndian::LoadAny(const char*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::LoadAny(const uint8_t *pInput)
    \brief Load an 8 bit unsigned value to memory with byte alignment.

    It assumes that the input pointer is unaligned.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pInput Pointer to an 8 bit value.
    \return The 8 bit unsigned value.

    \sa SwapEndian::LoadAny(const uint8_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::LoadAny(const int8_t *pInput)
    \brief Load an 8 bit signed value to memory with byte alignment.

    It assumes that the input pointer is unaligned.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pInput Pointer to an 8 bit value.
    \return The 8 bit signed value.

    \sa SwapEndian::LoadAny(const int8_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::LoadAny(const int16_t *pInput)
    \brief Fetch a 16 bit signed value from memory with byte alignment.

    Assuming the data is unaligned, it will grab data a byte at a time
    and reconstruct it into a 16 bit value in native endian.

    \param pInput Pointer to a 16 bit value.
    \return The 16 bit signed value.

    \sa SwapEndian::LoadAny(const int16_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::LoadAny(const int32_t *pInput)
    \brief Fetch a 32 bit signed value from memory with byte alignment.

    Assuming the data is unaligned, it will grab data a byte at a time
    and reconstruct it into a 32 bit value in native endian.

    \param pInput Pointer to a 32 bit value.
    \return The 32 bit signed value.

    \sa SwapEndian::LoadAny(const int32_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::LoadAny(const int64_t *pInput)
    \brief Fetch a 64 bit signed value from memory with byte alignment.

    Assuming the data is unaligned, it will grab data a byte at a time
    and reconstruct it into a 64 bit value in native endian.

    \param pInput Pointer to a 64 bit value.
    \return The 64 bit signed value.

    \sa SwapEndian::LoadAny(const int64_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::LoadAny(const wchar_t *pInput)
    \brief Fetch a 16 bit unsigned value from memory with byte alignment.

    Assuming the data is unaligned, it will grab data a byte at a time and
    reconstruct it into a 16 bit value in native endian.

    \param pInput Pointer to a 16 bit value.
    \return The 16 bit unsigned value.

    \sa SwapEndian::LoadAny(const wchar_t*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::LoadAny(const unsigned int *pInput)
    \brief Fetch a 16-32 bit unsigned value from memory with byte alignment.

    Assuming the data is unaligned, it will grab data a byte at a time
    and reconstruct it into a 16-32 bit value in native endian.

    \param pInput Pointer to a 16-32 bit value.
    \return The 16-32 bit unsigned value.

    \sa SwapEndian::LoadAny(const unsigned int*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::LoadAny(const signed int *pInput)
    \brief Fetch a 16-32 bit signed value from memory with byte alignment.

    Assuming the data is unaligned, it will grab data a byte at a time
    and reconstruct it into a 16-32 bit value in native endian.

    \param pInput Pointer to a 16-32 bit value.
    \return The 16-32 bit signed value.

    \sa SwapEndian::LoadAny(const signed int*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::LoadAny(const unsigned long *pInput)
    \brief Fetch a 32-64 bit unsigned value from memory with byte alignment.

    Assuming the data is unaligned, it will grab data a byte at a time
    and reconstruct it into a 32-64 bit value in native endian.

    \param pInput Pointer to a 32-64 bit value.
    \return The 32-64 bit unsigned value.

    \sa SwapEndian::LoadAny(const unsigned long*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::LoadAny(const signed long *pInput)
    \brief Fetch a 32-64 bit signed value from memory with byte alignment.

    Assuming the data is unaligned, it will grab data a byte at a time
    and reconstruct it into a 32-64 bit value in native endian.

    \param pInput Pointer to a 32-64 bit value.
    \return The 32-64 bit signed value.

    \sa SwapEndian::LoadAny(const signed long*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::StoreAny(char *pOutput,char iInput)
    \brief Store an 8 bit signed value to memory with byte alignment.

    It assumes that the output pointer is unaligned.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pOutput Pointer to an 8 bit value.
    \param iInput The 8 bit signed value.

    \sa SwapEndian::StoreAny(char*,char)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::StoreAny(uint8_t *pOutput,uint8_t uInput)
    \brief Store an 8 bit unsigned value to memory with byte alignment.

    It assumes that the output pointer is unaligned.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pOutput Pointer to an 8 bit value.
    \param uInput The 8 bit unsigned value.

    \sa SwapEndian::StoreAny(uint8_t*,char)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::StoreAny(int8_t *pOutput,int8_t iInput)
    \brief Store an 8 bit signed value to memory with byte alignment.

    It assumes that the output pointer is unaligned.

    \note Since 8 bit values can't be unaligned, this function only exists for
    completeness sake

    \param pOutput Pointer to an 8 bit value.
    \param iInput The 8 bit signed value.

    \sa SwapEndian::StoreAny(int8_t*,int8_t)

***************************************/

/*! ************************************

    \brief Store a 16 bit unsigned value to memory with byte alignment.

    Assuming the output pointer is unaligned, it will store data a byte at a
    time into a 16 bit value in native endian.

    \param pOutput Pointer to a 16 bit value.
    \param uInput The 16 bit unsigned value.

    \sa SwapEndian::StoreAny(uint16_t*,uint16_t)

***************************************/

#if (defined(BURGER_INTEL) || defined(BURGER_ARM64)) && !defined(DOXYGEN)

#else
void BURGER_API Burger::NativeEndian::StoreAny(
    uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT
{
#if defined(BURGER_BIGENDIAN)
    reinterpret_cast<uint8_t*>(pOutput)[0] = static_cast<uint8_t>(uInput >> 8U);
    reinterpret_cast<uint8_t*>(pOutput)[1] = static_cast<uint8_t>(uInput);
#else
    reinterpret_cast<uint8_t*>(pOutput)[0] = static_cast<uint8_t>(uInput);
    reinterpret_cast<uint8_t*>(pOutput)[1] = static_cast<uint8_t>(uInput >> 8U);
#endif
}
#endif

/*! ************************************

    \brief Store a 32 bit unsigned value to memory with byte alignment.

    Assuming the output pointer is unaligned, it will store data a byte at a
    time into a 32 bit value in native endian.

    \param pOutput Pointer to a 32 bit value.
    \param uInput The 32 bit unsigned value.

    \sa SwapEndian::StoreAny(uint32_t*,uint32_t)

***************************************/

#if (defined(BURGER_INTEL) || defined(BURGER_ARM64)) && !defined(DOXYGEN)

#else
void BURGER_API Burger::NativeEndian::StoreAny(
    uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT
{
#if defined(BURGER_BIGENDIAN)
    reinterpret_cast<uint8_t*>(pOutput)[0] =
        static_cast<uint8_t>(uInput >> 24U);
    reinterpret_cast<uint8_t*>(pOutput)[1] =
        static_cast<uint8_t>(uInput >> 16U);
    reinterpret_cast<uint8_t*>(pOutput)[2] = static_cast<uint8_t>(uInput >> 8U);
    reinterpret_cast<uint8_t*>(pOutput)[3] = static_cast<uint8_t>(uInput);
#else
    reinterpret_cast<uint8_t*>(pOutput)[0] = static_cast<uint8_t>(uInput);
    reinterpret_cast<uint8_t*>(pOutput)[1] = static_cast<uint8_t>(uInput >> 8U);
    reinterpret_cast<uint8_t*>(pOutput)[2] =
        static_cast<uint8_t>(uInput >> 16U);
    reinterpret_cast<uint8_t*>(pOutput)[3] =
        static_cast<uint8_t>(uInput >> 24U);
#endif
}
#endif

/*! ************************************

    \brief Store a 64 bit unsigned value to memory with byte alignment.

    Assuming the output pointer is unaligned, it will store data a byte at a
    time into a 64 bit value in native endian.

    \param pOutput Pointer to a 64 bit value.
    \param uInput The 64 bit unsigned value.

    \sa SwapEndian::StoreAny(uint64_t*,uint64_t)

***************************************/

#if (defined(BURGER_INTEL) || defined(BURGER_ARM64)) && !defined(DOXYGEN)

#else
void BURGER_API Burger::NativeEndian::StoreAny(
    uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT
{
    reinterpret_cast<uint8_t*>(pOutput)[0] =
        reinterpret_cast<const uint8_t*>(&uInput)[0];
    reinterpret_cast<uint8_t*>(pOutput)[1] =
        reinterpret_cast<const uint8_t*>(&uInput)[1];
    reinterpret_cast<uint8_t*>(pOutput)[2] =
        reinterpret_cast<const uint8_t*>(&uInput)[2];
    reinterpret_cast<uint8_t*>(pOutput)[3] =
        reinterpret_cast<const uint8_t*>(&uInput)[3];
    reinterpret_cast<uint8_t*>(pOutput)[4] =
        reinterpret_cast<const uint8_t*>(&uInput)[4];
    reinterpret_cast<uint8_t*>(pOutput)[5] =
        reinterpret_cast<const uint8_t*>(&uInput)[5];
    reinterpret_cast<uint8_t*>(pOutput)[6] =
        reinterpret_cast<const uint8_t*>(&uInput)[6];
    reinterpret_cast<uint8_t*>(pOutput)[7] =
        reinterpret_cast<const uint8_t*>(&uInput)[7];
}
#endif

/*! ************************************

    \brief Store a float value to memory with byte alignment.

    Assuming the output pointer is unaligned, it will store data a byte at a
    time into a 32 bit float value in native endian.

    \param pOutput Pointer to a float value.
    \param fInput The float value.

    \sa SwapEndian::StoreAny(float*,float)

***************************************/

#if (defined(BURGER_INTEL) || defined(BURGER_ARM64)) && !defined(DOXYGEN)

#else
void BURGER_API Burger::NativeEndian::StoreAny(
    float* pOutput, float fInput) BURGER_NOEXCEPT
{
    reinterpret_cast<uint8_t*>(pOutput)[0] =
        reinterpret_cast<const uint8_t*>(&fInput)[0];
    reinterpret_cast<uint8_t*>(pOutput)[1] =
        reinterpret_cast<const uint8_t*>(&fInput)[1];
    reinterpret_cast<uint8_t*>(pOutput)[2] =
        reinterpret_cast<const uint8_t*>(&fInput)[2];
    reinterpret_cast<uint8_t*>(pOutput)[3] =
        reinterpret_cast<const uint8_t*>(&fInput)[3];
}
#endif

/*! ************************************

    \brief Store a double value to memory with byte alignment.

    Assuming the output pointer is unaligned, it will store data a byte at a
    time into a 64 bit float value in native endian.

    \param pOutput Pointer to a double value.
    \param dInput The double value.

    \sa SwapEndian::StoreAny(double*,double).

***************************************/

#if (defined(BURGER_INTEL) || defined(BURGER_ARM64)) && !defined(DOXYGEN)

#else
void BURGER_API Burger::NativeEndian::StoreAny(
    double* pOutput, double dInput) BURGER_NOEXCEPT
{
    reinterpret_cast<uint8_t*>(pOutput)[0] =
        reinterpret_cast<const uint8_t*>(&dInput)[0];
    reinterpret_cast<uint8_t*>(pOutput)[1] =
        reinterpret_cast<const uint8_t*>(&dInput)[1];
    reinterpret_cast<uint8_t*>(pOutput)[2] =
        reinterpret_cast<const uint8_t*>(&dInput)[2];
    reinterpret_cast<uint8_t*>(pOutput)[3] =
        reinterpret_cast<const uint8_t*>(&dInput)[3];
    reinterpret_cast<uint8_t*>(pOutput)[4] =
        reinterpret_cast<const uint8_t*>(&dInput)[4];
    reinterpret_cast<uint8_t*>(pOutput)[5] =
        reinterpret_cast<const uint8_t*>(&dInput)[5];
    reinterpret_cast<uint8_t*>(pOutput)[6] =
        reinterpret_cast<const uint8_t*>(&dInput)[6];
    reinterpret_cast<uint8_t*>(pOutput)[7] =
        reinterpret_cast<const uint8_t*>(&dInput)[7];
}
#endif

/*! ************************************

    \fn Burger::NativeEndian::StoreAny(int16_t *pOutput,int16_t iInput)
    \brief Store a 16 bit signed value to memory with byte alignment.

    It assumes that the output pointer is unaligned.

    \param pOutput Pointer to a 16 bit value.
    \param iInput The 16 bit signed value.

    \sa SwapEndian::StoreAny(int16_t*,int16_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::StoreAny(int32_t *pOutput,int32_t iInput)
    \brief Store a 32 bit signed value to memory with byte alignment.

    It assumes that the output pointer is unaligned.

    \param pOutput Pointer to a 32 bit value.
    \param iInput The 32 bit signed value.

    \sa SwapEndian::StoreAny(int32_t*,int32_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::StoreAny(int64_t *pOutput,int64_t iInput)
    \brief Store a 64 bit signed value to memory with byte alignment.

    It assumes that the output pointer is unaligned.

    \param pOutput Pointer to a 64 bit value.
    \param iInput The 64 bit signed value.

    \sa SwapEndian::StoreAny(int64_t*,int64_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::StoreAny(wchar_t *pOutput,wchar_t uInput)
    \brief Store a 16 bit value to memory with byte alignment.

    It assumes that the output pointer is unaligned.

    \param pOutput Pointer to a 16 bit value.
    \param uInput The 16 bit value.

    \sa SwapEndian::StoreAny(wchar_t*,wchar_t)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::StoreAny(unsigned int *pOutput,
        unsigned int uInput)
    \brief Store a 16-32 bit signed value to memory with byte alignment.

    It assumes that the output pointer is unaligned.

    \param pOutput Pointer to a 16-32 bit value.
    \param uInput The 16-32 bit signed value.

    \sa SwapEndian::StoreAny(unsigned int*,unsigned int)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::StoreAny(signed int *pOutput,signed int iInput)
    \brief Store a 16-32 bit signed value to memory with byte alignment.

    It assumes that the output pointer is unaligned.

    \param pOutput Pointer to a 16-32 bit value.
    \param iInput The 16-32 bit signed value.

    \sa SwapEndian::StoreAny(signed int*,signed int)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::StoreAny(unsigned long *pOutput,
        unsigned long uInput)
    \brief Store a 32-64 bit signed value to memory with byte alignment.

    It assumes that the output pointer is unaligned.

    \param pOutput Pointer to a 32-64 bit value.
    \param uInput The 32-64 bit signed value.

    \sa SwapEndian::StoreAny(unsigned long*,unsigned long)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::StoreAny(signed long *pOutput,signed long iInput)
    \brief Store a 32-64 bit signed value to memory with byte alignment.

    It assumes that the output pointer is unaligned.

    \param pOutput Pointer to a 32-64 bit value.
    \param iInput The 32-64 bit signed value.

    \sa SwapEndian::StoreAny(signed long*,signed long)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::Fixup(T)
    \brief Does nothing.

    Native endian value do not need to be endian swapped, so this function will
    do nothing.

    \tparam T Pointer to a value to not endian swap.

    \sa SwapEndian::Fixup(char*)

***************************************/

/*! ************************************

    \fn Burger::NativeEndian::FixupAny(T)
    \brief Does nothing.

    The \ref SwapEndian class would swap the endian of a non aligned variable,
    but this class performs no operation since the endian is already a match for
    what the machine expects.

    \tparam T Pointer to a value to not endian swap.

    \sa SwapEndian::FixupAny(char*)

***************************************/

/*! ************************************

    \struct Burger::BigEndian
    \brief Loads a 16, 32 or 64 bit value with byte swapping if needed.

    This class will map to either to \ref NativeEndian or \ref SwapEndian
    depending on if this is a big endian machine or not. Use of this class will
    allow the programmer to write code that is endian neutral since the compiler
    will perform the proper mapping depending on the target's settings..

    Big endian is considered true if the uint32_t value 0x12345678 is stored
    in memory as 0x12, 0x34, 0x56, 0x78.

    Examples of use:
    \code
    uint32_t LoadedInt;

    // Load 4 bytes from a file
    fread(fp,1,4,&LoadedInt);

    // Fetch the big endian data
    uint32_t foo = Burger::BigEndian::Load(&LoadedInt);
    \endcode

    \note The documentation will describe the behavior of \ref SwapEndian, be
        warned that it will map to \ref NativeEndian on a big endian machine.

    \sa \ref NativeEndian, \ref LittleEndian and \ref SwapEndian

***************************************/

/*! ************************************

    \class Burger::LittleEndian
    \brief Loads a 16, 32 or 64 bit value with byte swapping if needed.

    This class will map to either to \ref NativeEndian or \ref SwapEndian
    depending on if this is a little endian machine or not. Use of this class
    will allow the programmer to write code that is endian neutral since the
    compiler will perform the proper mapping depending on the target's settings.

    Little endian is considered true if the uint32_t value 0x12345678 is
    stored in memory as 0x78, 0x56, 0x34, 0x12.

    Examples of use:
    \code
    uint32_t LoadedInt;

    // Load 4 bytes from a file
    fread(fp,1,4,&LoadedInt);

    // Fetch the little endian data
    uint32_t foo = Burger::LittleEndian::Load(&LoadedInt);
    \endcode

    \note The documentation will describe the behavior of \ref NativeEndian, be
        warned that it will map to \ref SwapEndian on a big endian machine.

    \sa \ref NativeEndian, \ref BigEndian and \ref SwapEndian

***************************************/

/*! ************************************

    \def BURGER_BIGENDIAN16(x)
    \brief Endian swaps a 16 bit constant on little endian machines.

    If a constant is required to exist as big endian, this macro will perform
    the endian swap at compile time if the machine is little endian.

    \param x 16 bit constant
    \return Constant in big endian format.
    \sa BURGER_BIGENDIAN32() or BURGER_LITTLEENDIAN16()

***************************************/

/*! ************************************

    \def BURGER_BIGENDIAN32(x)
    \brief Endian swaps a 32 bit constant on little endian machines.

    If a constant is required to exist as big endian, this macro will perform
    the endian swap at compile time if the machine is little endian.

    \param x 32 bit constant
    \return Constant in big endian format.
    \sa BURGER_BIGENDIAN16() or BURGER_LITTLEENDIAN32()

***************************************/

/*! ************************************

    \def BURGER_LITTLEENDIAN16(x)
    \brief Endian swaps a 16 bit constant on big endian machines.

    If a constant is required to exist as little endian, this macro will perform
    the endian swap at compile time if the machine is big endian.

    \param x 16 bit constant
    \return Constant in little endian format.
    \sa BURGER_LITTLEENDIAN32() or BURGER_BIGENDIAN16()

***************************************/

/*! ************************************

    \def BURGER_LITTLEENDIAN32(x)
    \brief Endian swaps a 32 bit constant on big endian machines.

    If a constant is required to exist as little endian, this macro will perform
    the endian swap at compile time if the machine is big endian.

    \param x 32 bit constant
    \return Constant in little endian format.
    \sa BURGER_LITTLEENDIAN16() or BURGER_BIGENDIAN32()

***************************************/

/*! ************************************

    \brief Reverse the endian of an array of 16-bit integers

    Given a pointer to an array of 16-bit integers, swap the endian of every
    entry

    \param pInput Pointer to the array to endian swap
    \param uCount Number of entries in the array (Not bytes)

    \sa ConvertEndian(uint16_t *,const uint16_t *,uintptr_t)

***************************************/

void BURGER_API Burger::ConvertEndian(uint16_t* pInput, uintptr_t uCount)
{
    // Any data?
    if (uCount) {
        do {
            // Swap in place
            SwapEndian::Fixup(pInput);
            ++pInput;
        } while (--uCount);
    }
}

/*! ************************************

    \brief Reverse the endian of a copied array of 16-bit integers

    Given a pointer to an array of 16-bit integers, swap the endian of every
    entry and store the result into another array of equal or greater size

    \param pOutput Pointer to the array to receive the swapped data
    \param pInput Pointer to the array to endian swap
    \param uCount Number of entries in the array (Not bytes)

    \sa ConvertEndian(uint16_t *,uintptr_t)

***************************************/

void BURGER_API Burger::ConvertEndian(
    uint16_t* pOutput, const uint16_t* pInput, uintptr_t uCount)
{
    // Any data?
    if (uCount) {
        do {
            // Swap and copy
            pOutput[0] = SwapEndian::Load(pInput);
            ++pInput;
            ++pOutput;
        } while (--uCount);
    }
}

/*! ************************************

    \brief Reverse the endian of an array of 32-bit integers

    Given a pointer to an array of 32-bit integers, swap the endian of every
    entry

    \param pInput Pointer to the array to endian swap
    \param uCount Number of entries in the array (Not bytes)

    \sa ConvertEndian(uint32_t *,const uint32_t *,uintptr_t)

***************************************/

void BURGER_API Burger::ConvertEndian(uint32_t* pInput, uintptr_t uCount)
{
    // Any data?
    if (uCount) {
        do {
            // Swap in place
            SwapEndian::Fixup(pInput);
            ++pInput;
        } while (--uCount);
    }
}

/*! ************************************

    \brief Reverse the endian of a copied array of 32-bit integers

    Given a pointer to an array of 32-bit integers, swap the endian of every
    entry and store the result into another array of equal or greater size

    \param pOutput Pointer to the array to receive the swapped data
    \param pInput Pointer to the array to endian swap
    \param uCount Number of entries in the array (Not bytes)

    \sa ConvertEndian(uint32_t *,uintptr_t)

***************************************/

void BURGER_API Burger::ConvertEndian(
    uint32_t* pOutput, const uint32_t* pInput, uintptr_t uCount)
{
    // Any data?
    if (uCount) {
        do {
            // Swap and copy
            pOutput[0] = SwapEndian::Load(pInput);
            ++pInput;
            ++pOutput;
        } while (--uCount);
    }
}

/*! ************************************

    \brief Reverse the endian of an array of 64-bit integers

    Given a pointer to an array of 64-bit integers, swap the endian of every
    entry

    \param pInput Pointer to the array to endian swap
    \param uCount Number of entries in the array (Not bytes)

    \sa ConvertEndian(uint64_t *,const uint64_t *,uintptr_t)

***************************************/

void BURGER_API Burger::ConvertEndian(uint64_t* pInput, uintptr_t uCount)
{
    // Any data?
    if (uCount) {
        do {
            // Swap in place
            SwapEndian::Fixup(pInput);
            ++pInput;
        } while (--uCount);
    }
}

/*! ************************************

    \brief Reverse the endian of a copied array of 64-bit integers

    Given a pointer to an array of 64-bit integers, swap the endian of every
    entry and store the result into another array of equal or greater size

    \param pOutput Pointer to the array to receive the swapped data
    \param pInput Pointer to the array to endian swap
    \param uCount Number of entries in the array (Not bytes)

    \sa ConvertEndian(uint64_t *,uintptr_t)

***************************************/

void BURGER_API Burger::ConvertEndian(
    uint64_t* pOutput, const uint64_t* pInput, uintptr_t uCount)
{
    // Any data?
    if (uCount) {
        do {
            // Swap and copy
#if defined(BURGER_64BITCPU)
            pOutput[0] = SwapEndian::Load(pInput);
#else
            // For non 64 bit CPUs, do the swap in 32 bit chunks
            uint32_t uLow = SwapEndian::Load(&static_cast<const uint32_t*>(
                static_cast<const void*>(pInput))[0]);
            uint32_t uHigh = SwapEndian::Load(&static_cast<const uint32_t*>(
                static_cast<const void*>(pInput))[1]);
            static_cast<uint32_t*>(static_cast<void*>(pOutput))[0] = uHigh;
            static_cast<uint32_t*>(static_cast<void*>(pOutput))[1] = uLow;
#endif
            ++pInput;
            ++pOutput;
        } while (--uCount);
    }
}

/*! ************************************

    \brief Add 128 to every byte to convert a char to a byte or vice versa

    Given a pointer to an array of bytes, add 128 to every entry

    \param pInput Pointer to the array to bytes
    \param uLength Number of bytes in the array

    \sa SwapCharsToBytes(void *,const void *,uintptr_t)

***************************************/

void BURGER_API Burger::SwapCharsToBytes(void* pInput, uintptr_t uLength)
{
    if (uLength) {
        do {
            static_cast<uint8_t*>(pInput)[0] =
                static_cast<uint8_t>(static_cast<uint8_t*>(pInput)[0] ^ 0x80U);
            pInput = static_cast<uint8_t*>(pInput) + 1;
        } while (--uLength);
    }
}

/*! ************************************

    \brief Add 128 to every byte to convert a char to a byte or vice versa

    Given a pointer to an array of bytes, add 128 to every entry and store the
    result into another array of equal or greater size

    \param pOutput Pointer to the array to receive the converted data
    \param pInput Pointer to the array to bytes
    \param uLength Number of bytes in the array

    \sa SwapCharsToBytes(void *,uintptr_t)

***************************************/

void BURGER_API Burger::SwapCharsToBytes(
    void* pOutput, const void* pInput, uintptr_t uLength)
{
    if (uLength) {
        do {
            static_cast<uint8_t*>(pOutput)[0] = static_cast<uint8_t>(
                static_cast<const uint8_t*>(pInput)[0] ^ 0x80U);
            pInput = static_cast<const uint8_t*>(pInput) + 1;
            pOutput = static_cast<uint8_t*>(pOutput) + 1;
        } while (--uLength);
    }
}
