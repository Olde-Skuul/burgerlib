/***************************************

    String handlers for UTF32 support

    Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brutf32.h"
#include "brutf8.h"

/*! ************************************

    \class Burger::UTF32
    \brief Conversion routines to the UTF32 format

    UTF32 is simplest data format for [Unicode](http://www.unicode.org) data to
    be stored in a 32 bit wide "C" string. It can easily contain all of the
    characters for the worlds' languages. These functions allow conversion from
    UTF8, which Burgerlib is based on, to UTF32 which some foreign APIs require
    for internationalization. Please note that these functions operate on
    strings that are native endian.

***************************************/

/*! ************************************

    \var Burger::UTF32::BAD
    \brief Value returned if a routine failed.

    If a function doesn't return \ref TRUE or \ref FALSE for failure, it will
    return this value instead. Please see the documentation for each function
    to know which ones use true/false pairs or this value.

***************************************/

/*! ************************************

    \var Burger::UTF32::ENDIANMARK
    \brief Byte stream token for native endian.

    When writing a text file using UTF32, you may need to write this value as
    the first character to mark the endian that the data was saved at. This
    value is the correct value for the native endian of the machine. Use
    Burger::UTF32::BE or Burger::UTF32::LE to test incoming data to determine
    the endian of data that's unknown.

***************************************/

/*! ************************************

    \var Burger::UTF32::BE
    \brief 32 bit Byte Order Mark (BOM) for Big Endian UTF32 data.

    If a token was read in the matched this constant, then it is assumed that
    all of the following data is Big Endian. It adheres to the Unicode standard
    for UTF-32

***************************************/

/*! ************************************

    \var Burger::UTF32::LE
    \brief 32 bit Byte Order Mark (BOM) for Little Endian UTF32 data.

    If a token was read in the matched this constant, then it is assumed that
    all of the following data is Little Endian. It adheres to the Unicode
    standard for UTF-32

***************************************/

/*! ************************************

    \brief Validate a UTF32 value.

    Return \ref TRUE if a UTF32 character is in the valid bounds. (0-0xD7FF) or
    (0xE000-0x10FFFF).

    \param uInput UTF32 encoded character value.
    \return \ref TRUE if in bounds, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::UTF32::IsValid(uint32_t uInput) BURGER_NOEXCEPT
{
    if ((uInput < 0xD800U) || ((uInput >= 0xE000U) && (uInput < 0x10FFFFU))) {
        return TRUE;
    }
    return FALSE;
}

/*! ************************************

    \brief Check a UTF32 "C" string for validity

    Check a "C" string if it's a valid UTF32 stream. Return false if there was
    an error, or true if the bytes represent a valid UTF32 pattern.

    \param pInput Pointer to a zero terminated string, ``nullptr`` will page
        fault.

    \return \ref TRUE if the entire string is a valid UTF32 stream, \ref FALSE
        if not.

***************************************/

uint_t BURGER_API Burger::UTF32::IsValid(const uint32_t* pInput) BURGER_NOEXCEPT
{
    uint_t uFirst = pInput[0];
    if (uFirst) {
        // Accept it
        ++pInput;
        do {
            if (((uFirst >= 0xD800U) && (uFirst < 0xE000U)) ||
                (uFirst >= 0x110000U)) {
                // This is a bad UTF32 string
                return FALSE;
            }
            uFirst = pInput[0];
            // Accept it
            ++pInput;
        } while (uFirst);
    }
    // Yes! It's good
    return TRUE;
}

/*! ************************************

    \brief Check a UTF32 uint32_t array for validity.

    Check a uint32_t array and see if it's a valid UTF32 stream. Return false if
    there was an error, or true if the bytes represent a valid UTF32 pattern.

    \param pInput Pointer to UTF32 data. Can be ``nullptr`` if uInputSize is
        zero, otherwise page fault.
    \param uElementCount Size of the data in elements, if zero, then the
        function will return true.

    \return \ref TRUE if the entire string is a valid UTF8 stream, \ref FALSE if
        not.

***************************************/

uint_t BURGER_API Burger::UTF32::IsValid(
    const uint32_t* pInput, uintptr_t uElementCount) BURGER_NOEXCEPT
{
    if (uElementCount) {
        do {
            const uint32_t uFirst = pInput[0];
            // Accept it
            ++pInput;
            if (((uFirst >= 0xD800U) && (uFirst < 0xE000U)) ||
                (uFirst >= 0x110000U)) {
                // This is a bad UTF32 string
                return FALSE;
            }
        } while (--uElementCount);
    }
    // Yes! It's good
    return TRUE;
}

/*! ************************************

    \brief Return a UTF32 code from a UTF8 stream.

    Convert from a UTF8 stream into a 32 bit Unicode value (0x00 to 0x10FFFF).
    This function will perform validation on the incoming stream and will flag
    any data that's invalid.

    \note This function will not move the pointer forward, use
        UTF8::NextToken(const char *) instead.

    \param pInput Pointer to a valid UTF8 "C" string, ``nullptr`` will page
        fault.

    \return The UTF32 code or \ref BAD if invalid. 0x00 is not invalid.

    \sa FromUTF8(const char **), UTF8::GetTokenSize(const char *) or
        UTF8::NextToken(const char *).

***************************************/

uint32_t BURGER_API Burger::UTF32::FromUTF8(const char* pInput) BURGER_NOEXCEPT
{
    // Check for the simple case of 0x00-0x7F
    const uint32_t uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
    if (uFirst < 0x80U) {
        return uFirst; // Return ASCII now (0x00-0x7F)
    }

    // UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid

    if (uFirst >= 0xC2U) {
        // Trick to convert 0x80-0xBF to 0x00-0x3F
        const uint32_t uSecond =
            reinterpret_cast<const uint8_t*>(pInput)[1] ^ 0x80U;
        if (uSecond < 0x40U) {

            // Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
            if (uFirst < 0xE0U) {
                // Get the upper 5 bits
                return ((uFirst & 0x1FU) << 6U) | uSecond;
            }

            const uint32_t uThird =
                reinterpret_cast<const uint8_t*>(pInput)[2] ^ 0x80U;
            if (uThird < 0x40U) {

                // Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF /
                // 0x80-0xBF
                if (uFirst < 0xF0U) {
                    // Get the upper 4 bits
                    const uint32_t uResult =
                        ((uFirst & 0xFU) << 12U) | (uSecond << 6U) | uThird;
                    // Only 0x0800-0xFFFF are valid
                    if ((uResult >= 0x800U) &&
                        ((uResult < 0xD800U) || (uResult >= 0xE000U))) {
                        // It's good!
                        return uResult;
                    }
                } else {
                    const uint32_t uFourth =
                        reinterpret_cast<const uint8_t*>(pInput)[3] ^ 0x80U;
                    if (uFourth < 0x40U) {

                        // Check for the 21 bit code of 0xF0-0xFF / 0x80-0xBF /
                        // 0x80-0xBF / 0x80-0xBF
                        // Get the upper 4 bits
                        const uint32_t uResult = ((uFirst & 0xFU) << 18U) |
                            (uSecond << 12U) | (uThird << 6U) | uFourth;
                        // Only 0x10000-0x10FFFF are valid
                        if ((uResult >= 0x10000U) && (uResult < 0x110000U)) {
                            // It's good!
                            return uResult;
                        }
                    }
                }
            }
        }
    }
    // This is a bad stream
    return BAD;
}

/*! ************************************

    \brief Return a UTF32 code from a UTF8 stream and update the pointer

    Convert from a UTF8 stream into a 32 bit
    Unicode value (0x00 to 0x10FFFF). This function will perform validation
    on the incoming stream and will flag any data that's invalid.

    \param ppInput Pointer to a valid UTF8 "C" string pointer, ``nullptr`` will
        page fault.

    \return The UTF32 code or \ref BAD if invalid. 0x00 is not invalid.
    \sa FromUTF8(const char *), UTF8::GetTokenSize(const char *) or
        UTF8::NextToken(const char *).

***************************************/

uint32_t BURGER_API BURGER_API Burger::UTF32::FromUTF8(
    const char** ppInput) BURGER_NOEXCEPT
{
    const char* pInput = ppInput[0];
    // Decode the data
    const uint32_t uResult = FromUTF8(pInput);
    if (uResult != BAD) {
        ppInput[0] = UTF8::NextToken(pInput);
    }
    return uResult;
}

/*! ************************************

    \brief Convert a UTF8 "C" string into a UTF32 stream.

    Take a "C" string that is using UTF8 encoding and convert it
    to a UTF32 encoded "C" string. The function will return the size of the
    string after encoding. This size is valid, even if it exceeded the output
    buffer size. The output pointer and size can be null to have this routine
    calculate the size of the possible output so the application can allocate a
    buffer large enough to hold it.

    \note This function will ensure that the string is always zero terminated,
    even if truncation is necessary to get it to fit in the output buffer. Under
    no circumstances will the output buffer be overrun.

    \note If invalid UTF8 data is found, it will be skipped.

    \param pOutput Pointer to UTF8 buffer to receive the converted string,
        ``nullptr`` is okay if uOutputSize is zero, otherwise it will page
        fault.
    \param uOutputSize Size of the output buffer in bytes.
    \param pInput UTF32 encoded "C" string, ``nullptr`` will page fault.

    \return The number of bytes of the potential output without the trailing
        uint32_t zero. It is valid, even if the output buffer wasn't large
        enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF32::FromUTF8(uint32_t* pOutput,
    uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT
{
    uint32_t* pWorkPtr = pOutput;
    // Use longs
    uOutputSize >>= 2;
    // If nonzero, then I append a 0 to the string.
    const uintptr_t uAddZero = uOutputSize;
    // Valid?
    if (uOutputSize) {
        --uOutputSize;
    }
    // This is the end of the valid buffer
    uint32_t* pEndPtr = pWorkPtr + uOutputSize;

    // Let's convert the string
    uint32_t uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];

    // Accept it
    ++pInput;
    // Can I start the loop?
    if (uFirst) {
        do {
            // Check for the simple case of 0x00-0x7F
            if (uFirst < 0x80U) {
                // Can I write?
                if (pWorkPtr < pEndPtr) {
                    pWorkPtr[0] = uFirst;
                }
                ++pWorkPtr; // Increment the output

                // UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid

            } else if (uFirst >= 0xC2U) {
                // Trick to convert 0x80-0xBF to 0x00-0x3F
                const uint32_t uSecond =
                    reinterpret_cast<const uint8_t*>(pInput)[0] ^ 0x80U;
                if (uSecond < 0x40U) {

                    // Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
                    if (uFirst < 0xE0U) {
                        // Can I write?
                        if (pWorkPtr < pEndPtr) {
                            pWorkPtr[0] = ((uFirst & 0x1FU) << 6U) | uSecond;
                        }
                        ++pWorkPtr; // Increment the output
                        ++pInput;   // Accept the second byte

                        // Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF /
                        // 0x80-0xBF
                    } else {
                        const uint32_t uThird =
                            reinterpret_cast<const uint8_t*>(pInput)[1] ^ 0x80U;
                        if (uThird < 0x40U) {
                            if (uFirst < 0xF0U) {
                                // Get the upper 4 bits
                                const uint32_t uResult =
                                    ((uFirst & 0xFU) << 12U) | (uSecond << 6U) |
                                    uThird;
                                if ((uResult >= 0x800U) &&
                                    ((uResult < 0xD800U) ||
                                        // Only 0x0800-0xFFFF are valid
                                        (uResult >= 0xE000U))) {
                                    // Can I write?
                                    if (pWorkPtr < pEndPtr) {
                                        pWorkPtr[0] = uResult;
                                    }
                                    // Increment the output
                                    ++pWorkPtr;
                                }
                                // I took three chars
                                pInput += 2;

                                // Check for the 21 bit code of 0xF0-0xFF /
                                // 0x80-0xBF / 0x80-0xBF / 0x80-0xBF
                            } else {
                                const uint32_t uFourth =
                                    reinterpret_cast<const uint8_t*>(
                                        pInput)[2] ^
                                    0x80U;
                                if (uFourth < 0x40U) {
                                    // Get the upper 4 bits
                                    const uint32_t uResult =
                                        ((uFirst & 0xFU) << 18U) |
                                        (uSecond << 12U) | (uThird << 6U) |
                                        uFourth;
                                    // Only 0x10000-0x10FFFF are valid
                                    if ((uResult >= 0x10000U) &&
                                        (uResult < 0x110000U)) {
                                        // Can I write?
                                        if (pWorkPtr < pEndPtr) {
                                            pWorkPtr[0] = uResult;
                                        }
                                        // Increment the output
                                        ++pWorkPtr;
                                    }
                                    pInput += 3;
                                }
                            }
                        }
                    }
                }
            }
            uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
            // Accept it
            ++pInput;
        } while (uFirst);
    }
    // Can I add a trailing zero?
    if (uAddZero) {
        if (pWorkPtr < pEndPtr) {
            pEndPtr = pWorkPtr;
        }
        // Write it, but don't add it to the strlen()
        pEndPtr[0] = 0;
    }

    // Return the equivalent of strlen()
    return static_cast<uintptr_t>(reinterpret_cast<uint8_t*>(pWorkPtr) -
        reinterpret_cast<uint8_t*>(pOutput));
}

/*! ************************************

    \brief Convert a UTF8 stream into a UTF32 uint32_t array.

    Take a byte array that is using UTF8 encoding and convert it to a UTF32
    uint32_t encoded "C" string. The function will return the size of the string
    after encoding. This size is valid, even if it exceeded the output buffer
    size. The output pointer and size can be ``nullptr`` to have this routine
    calculate the size of the possible output so the application can allocate a
    buffer large enough to hold it.

    \note This function will ensure that the string is always zero terminated,
    even if truncation is necessary to get it to fit in the output buffer. Under
    no circumstances will the output buffer be overrun.

    \note Zeros can be encoded into the stream. This function will not early out
        if a zero was parsed. Zeros will be placed in the UTF32 stream as is.

    \param pOutput Pointer to a byte buffer to receive the UTF32 string
        ``nullptr`` is okay if uOutputSize is zero, otherwise a page fault will
        occur.
    \param uOutputSize Size of the output buffer in bytes.
    \param pInput UTF8 encoded byte array, ``nullptr`` is okay if uInputSize is
        zero.
    \param uInputSize Size of the input byte array.

    \return Byte count of the potential output. It is valid, even if the output
        buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF32::FromUTF8(uint32_t* pOutput,
    uintptr_t uOutputSize, const char* pInput,
    uintptr_t uInputSize) BURGER_NOEXCEPT
{
    uint32_t* pWorkPtr = pOutput;
    // Convert to longs
    uOutputSize >>= 2;
    // Can I save a zero?
    const uintptr_t uAddZero = uOutputSize;
    // Valid for anything?
    if (uOutputSize) {
        // Make room for the zero
        --uOutputSize;
    }
    // Can't write past this point, ever
    uint32_t* pEndPtr = pWorkPtr + uOutputSize;

    // Let's convert the string

    if (uInputSize) { // Sanity check
        do {
            const uint32_t uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
            // Accept it
            ++pInput;

            // Check for the simple case of 0x00-0x7F

            if (uFirst < 0x80U) {
                // Can I write?
                if (pWorkPtr < pEndPtr) {
                    pWorkPtr[0] = uFirst;
                }
                ++pWorkPtr; // Increment the output
                // UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid

            } else if ((uInputSize >= 2) && uFirst >= 0xC2U) {
                // Trick to convert 0x80-0xBF to 0x00-0x3F
                const uint32_t uSecond =
                    reinterpret_cast<const uint8_t*>(pInput)[0] ^ 0x80U;
                if (uSecond < 0x40U) {

                    // Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
                    if (uFirst < 0xE0U) {
                        // Can I write?
                        if (pWorkPtr < pEndPtr) {
                            pWorkPtr[0] = ((uFirst & 0x1FU) << 6U) | uSecond;
                        }
                        // Increment the output
                        ++pWorkPtr;
                        // Accept the second byte
                        ++pInput;
                        --uInputSize;

                        // Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF /
                        // 0x80-0xBF
                    } else if (uInputSize >= 3) {
                        const uint32_t uThird =
                            reinterpret_cast<const uint8_t*>(pInput)[1] ^ 0x80U;
                        if (uThird < 0x40U) {
                            if (uFirst < 0xF0U) {
                                // Get the upper 4 bits
                                const uint32_t uResult =
                                    ((uFirst & 0xFU) << 12U) | (uSecond << 6U) |
                                    uThird;
                                // Only 0x0800-0xFFFF are valid
                                if ((uResult >= 0x800U) &&
                                    ((uResult < 0xD800U) ||
                                        (uResult >= 0xE000U))) {
                                    // Can I write?
                                    if (pWorkPtr < pEndPtr) {
                                        pWorkPtr[0] = uResult;
                                    }
                                    // Increment the output
                                    ++pWorkPtr;
                                }
                                // I took three chars
                                pInput += 2;
                                uInputSize -= 2;

                                // Check for the 21 bit code of 0xF0-0xFF /
                                // 0x80-0xBF / 0x80-0xBF / 0x80-0xBF
                            } else if (uInputSize >= 4) {
                                const uint32_t uFourth =
                                    reinterpret_cast<const uint8_t*>(
                                        pInput)[2] ^
                                    0x80U;
                                if (uFourth < 0x40U) {
                                    // Get the upper 4 bits
                                    const uint32_t uResult =
                                        ((uFirst & 0xFU) << 18U) |
                                        (uSecond << 12U) | (uThird << 6U) |
                                        uFourth;
                                    if ((uResult >= 0x10000U) &&
                                        // Only 0x10000-0x10FFFF are valid
                                        (uResult < 0x110000U)) {

                                        // Can I write?
                                        if (pWorkPtr < pEndPtr) {
                                            pWorkPtr[0] = uResult;
                                        }
                                        // Increment the output
                                        ++pWorkPtr;
                                    }
                                }
                                pInput += 3;
                                uInputSize -= 3;
                            }
                        }
                    }
                }
            }
        } while (--uInputSize);
    }
    // Can I add a trailing zero?
    if (uAddZero) {
        if (pWorkPtr < pEndPtr) {
            pEndPtr = pWorkPtr;
        }
        // Write it, but don't add it to the strlen()
        pEndPtr[0] = 0;
    }

    // Return the equivalent of strlen()
    return static_cast<uint32_t>(reinterpret_cast<uint8_t*>(pWorkPtr) -
        reinterpret_cast<uint8_t*>(pOutput));
}
