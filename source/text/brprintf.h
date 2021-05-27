/***************************************

    Matt Pritchard's typesafe & enhanced c++ formatted output function
    replacements

    Note from Matt Pritchard: mprintf is dedicated to the ones I love: Jennifer,
    Nathan and Lauren.

    Copyright (c) 2012-2013 by Matt Pritchard <mattp_2006@yahoo.com>

    Copyright (c) 2016-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRPRINTF_H__
#define __BRPRINTF_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRMETROWERKS_H__
#include "brmetrowerks.h"
#endif

#ifndef __BRWATCOM_H__
#include "brwatcom.h"
#endif

#ifndef __BRSTRINGFUNCTIONS_H__
#include "brstringfunctions.h"
#endif

#ifndef __BRFPINFO_H__
#include "brfpinfo.h"
#endif

#ifndef __BRFPLARGEINT_H__
#include "brfplargeint.h"
#endif

#ifndef _INC_STDIO
#include <stdio.h>
#endif

/* BEGIN */
namespace Burger {

// Forward declarations of objects we will use
class String;

extern void BURGER_API LoadHighPrecisionFromFloat(const FPInfo* pFPInfo,
    FPLargeInt* pIntegerPart, FPLargeInt* pFractionalPart) BURGER_NOEXCEPT;

class FPPrintInfo {
    enum eTextIndex {
        kPosInfinity, ///< Index to "inf"
        kNegInfinity, ///< Index to "-inf"
        kPosNan,      ///< Index to "nan"
        kNegNan,      ///< Index to "-nan"
        kIndNan,      ///< Index to "-nan"
        kTextCount    ///< Number of string indexes
    };

public:
    enum eResult {
        /** Print a zero */
        kResultZero,
        /** Print a normal number */
        kResultNormalFloat,
        /** Print a number that is rounded in the fraction */
        kResultFloatRoundedUpAtEnd,
        /** Print a number that is rounded in the integer */
        kResultFloatRoundedUpADigit,
        /** Print a positive infinity */
        kResultPositiveInfinity,
        /** Print a negative infinity */
        kResultNegativeInfinity,
        /** Print a positive NaN */
        kResultPositiveNaN,
        /** Print a negative NaN */
        kResultNegativeNaN,
        /** Print a signaling NaN */
        kResultSignalingNaN,
        /** Print a quiet NaN */
        kResultQuietNaN,
        /** Print an indefinite NaN */
        kResultIndefiniteNaN,
        /** End of print list */
        kResultCount,
        /** First special string to print */
        kResultFirstSpecial = kResultPositiveInfinity,
        /** Last special string to print */
        kResultLastSpecial = kResultIndefiniteNaN
    };

    /** Limit of characters to convert (Default is no limit) */
    static const uint_t kConversionDigitUnlimited = 0;
    /** Special float message strings */
    static const char* g_SpecialFloatsText[kTextCount];
    /** Special float message string in uppercase */
    static const char* g_SpecialFloatsTextUpper[kTextCount];

private:
    /** \ref TRUE if there is an integer component */
    uint_t m_bHasIntegerPart;
    /** \ref TRUE if there is a fractional component */
    uint_t m_bHasFractionalPart;
    /** Number of digits for special text */
    uint_t m_uSpecialFormDigits;
    /** Index to the first non zero integer digit */
    uint_t m_uFirstNonZeroIntDigitPos;
    /** Index to the last non zero integer digit */
    uint_t m_uLastNonZeroIntDigitPos;
    /** Index to the first non zero fractional digit */
    uint_t m_uFirstNonZeroFracDigitPos;
    /** Index to the last non zero integer digit */
    uint_t m_uLastNonZeroFracDigitPos;
    /** Number of integer zeros */
    uint_t m_uPadIntZeros;
    /** Number of fractional zeros */
    uint_t m_uPadFracZeros;
    /** Type of floating point number */
    eResult m_ePrintResult;
    /** Number of digits to print maximum (0 = no maximum) */
    static uint_t g_uClipFloatDigitsAfter;

public:
    FPPrintInfo(void) BURGER_NOEXCEPT
    {
        Reset();
    }
    void BURGER_API Reset(void) BURGER_NOEXCEPT;

    BURGER_INLINE uint_t HasInteger(void) const BURGER_NOEXCEPT
    {
        return m_bHasIntegerPart;
    }
    BURGER_INLINE uint_t HasFractional(void) const BURGER_NOEXCEPT
    {
        return m_bHasFractionalPart;
    }
    BURGER_INLINE uint_t GetSpecialFormDigits(void) const BURGER_NOEXCEPT
    {
        return m_uSpecialFormDigits;
    }
    BURGER_INLINE uint_t GetFirstNonZeroIntDigitPosition(
        void) const BURGER_NOEXCEPT
    {
        return m_uFirstNonZeroIntDigitPos;
    }
    BURGER_INLINE uint_t GetLastNonZeroIntDigitPosition(
        void) const BURGER_NOEXCEPT
    {
        return m_uLastNonZeroIntDigitPos;
    }
    BURGER_INLINE uint_t GetFirstNonZeroFracDigitPosition(
        void) const BURGER_NOEXCEPT
    {
        return m_uFirstNonZeroFracDigitPos;
    }
    BURGER_INLINE uint_t GetLastNonZeroFracDigitPosition(
        void) const BURGER_NOEXCEPT
    {
        return m_uLastNonZeroFracDigitPos;
    }
    BURGER_INLINE uint_t GetIntegerZeroCount(void) const BURGER_NOEXCEPT
    {
        return m_uPadIntZeros;
    }
    BURGER_INLINE uint_t GetFracZeroCount(void) const BURGER_NOEXCEPT
    {
        return m_uPadFracZeros;
    }
    BURGER_INLINE eResult GetResult(void) const BURGER_NOEXCEPT
    {
        return m_ePrintResult;
    }
    BURGER_INLINE uint_t IsSpecial(void) const BURGER_NOEXCEPT
    {
        return (m_ePrintResult >= kResultFirstSpecial) &&
            (m_ePrintResult <= kResultLastSpecial);
    }
    BURGER_INLINE uint_t IsInfinity(void) const BURGER_NOEXCEPT
    {
        return (m_ePrintResult == kResultPositiveInfinity) ||
            (m_ePrintResult == kResultNegativeInfinity);
    }
    BURGER_INLINE uint_t IsNaN(void) const BURGER_NOEXCEPT
    {
        return (m_ePrintResult >= kResultPositiveNaN) &&
            (m_ePrintResult <= kResultIndefiniteNaN);
    }
    static BURGER_INLINE uint_t GetClip(void) BURGER_NOEXCEPT
    {
        return g_uClipFloatDigitsAfter;
    }
    static BURGER_INLINE void SetClip(uint_t uNewClip) BURGER_NOEXCEPT
    {
        g_uClipFloatDigitsAfter = uNewClip;
    }
    void BURGER_API AnalyzeFloat(
        const FPInfo* pFPInfo, uint_t uPrecisionDigits) BURGER_NOEXCEPT;
};

class SafePrintArgument {
public:
    enum eType {
        /** Invalid argument type */
        ARG_INVALID = 0,
        /** Signed 1 byte integer */
        ARG_INT8,
        /** Signed 2 byte integer */
        ARG_INT16,
        /** Signed 4 byte integer */
        ARG_INT32,
        /** Signed 8 byte integer */
        ARG_INT64,
        /** Unsigned 1 byte integer */
        ARG_UINT8,
        /** Unsigned 2 byte integer */
        ARG_UINT16,
        /** Unsigned 4 byte integer */
        ARG_UINT32,
        /** Unsigned 8 byte integer */
        ARG_UINT64,
        /** 16-bit IEEE 754-2008 float, aka binary16 */
        ARG_HALF,
        /** 32-bit IEEE 754 float */
        ARG_FLOAT,
        /** 64-bit IEEE 754 float */
        ARG_DOUBLE,
        /** bool C++ built in boolean type */
        ARG_BOOL,
        /** 64-bit SIMD structure */
        ARG_SIMD64,
        /** 128-bit SIMD structure */
        ARG_SIMD128,
        /** 128-bit SIMD structure, integer only */
        ARG_SIMD128I,
        /** 128-bit SIMD structure, 2 doubles only */
        ARG_SIMD128D,
        /** Burgerlib String */
        ARG_PSTRING,
        /** const char* for a "C" string */
        ARG_PCHAR,
        /** const signed char* for a "C" string */
        ARG_PSCHAR,
        /** const unsigned char* for a "C" string */
        ARG_PUCHAR,
        /** Pointer to an int16_t */
        ARG_PINT16,
        /** Pointer to a uint16_t */
        ARG_PUINT16,
        /** Pointer to an int32_t */
        ARG_PINT32,
        /** Pointer to a uint32_t */
        ARG_PUINT32,
        /** Pointer to an int64_t */
        ARG_PINT64,
        /**Pointer to a uint64_t */
        ARG_PUINT64,
        /** Pointer to a float */
        ARG_PFLOAT,
        /** Pointer to a double */
        ARG_PDOUBLE,
        /** Pointer to a 64-bit SIMD structure */
        ARG_PSIMD64,
        /** Pointer to a 128-bit SIMD structure, 4 floats only */
        ARG_PSIMD128,
        /** Pointer to a 128-bit SIMD structure, 4 32-bit integers */
        ARG_PSIMD128I,
        /** Pointer to a 128-bit SIMD structure, 2 doubles only */
        ARG_PSIMD128D,
        /** void * for a catchall */
        ARG_PVOID,
        /** Auto list counter, all valid types must appear above this */
        ARG_COUNT,
        /** Used to indicate 'any valid arg' */
        ARG_ANY,
        /** First integer entry */
        ARG_FIRSTINT = ARG_INT8,
        /** Last integer entry */
        ARG_LASTINT = ARG_UINT64,
        /** First signed integer entry */
        ARG_FIRSTSIGNED = ARG_INT8,
        /** Last signed integer entry */
        ARG_LASTSIGNED = ARG_INT64,
        /** First unsigned integer entry */
        ARG_FIRSTUNSIGNED = ARG_UINT8,
        /** Last unsigned integer entry */
        ARG_LASTUNSIGNED = ARG_UINT64,
        /** First floating point entry */
        ARG_FIRSTREAL = ARG_HALF,
        /** Last floating point entry */
        ARG_LASTREAL = ARG_DOUBLE,
        /** First "C" string pointer entry */
        ARG_FIRSTTEXT = ARG_PSTRING,
        /** Last "C" string pointer entry */
        ARG_LASTTEXT = ARG_PUCHAR,
        /** First pointer type entry */
        ARG_FIRSTPTR = ARG_PSTRING,
        /** Last pointer type entry */
        ARG_LASTPTR = ARG_PVOID,
        /** First SIMD entry */
        ARG_FIRSTSIMD = ARG_SIMD64,
        /** Last SIMD entry */
        ARG_LASTSIMD = ARG_SIMD128D,
        /** First SIMD pointer type entry */
        ARG_FIRSTSIMDPTR = ARG_PSIMD64,
        /** Last SIMD pointer type entry */
        ARG_LASTSIMDPTR = ARG_PSIMD128D
    };

    /** Character returned from \ref GetChar() const for invalid character */
    static const char kInvalidCharConversion = '~';

    union {
        int8_t m_iInt8;            ///< Signed char value
        uint8_t m_uWord8;          ///< Unsigned char value
        int16_t m_iInt16;          ///< Signed short value
        uint16_t m_uWord16;        ///< Unsigned short value
        int32_t m_iInt32;          ///< Signed 32 bit integer value
        uint32_t m_uWord32;        ///< Unsigned 32 bit integer value
        int64_t m_iInt64;          ///< Signed 64 bit integer value
        uint64_t m_uWord64;        ///< Unsigned 64 bit integer value
        uint16_t m_fHalf;          ///< 16 bit floating point value
        float m_fFloat;            ///< 32 bit floating point value
        double m_dDouble;          ///< 64 bit floating point value
        bool m_bBool;              ///< Boolean value
        const String* m_pString;   ///< Pointer to a Burgerlib \ref String class
        const char* m_pChar;       ///< Generic char pointer for strings
        const int8_t* m_pInt8;     ///< Pointer to a signed char
        const uint8_t* m_pWord8;   ///< Pointer to an unsigned char
        const int16_t* m_pInt16;   ///< Pointer to a signed 16 bit integer
        const uint16_t* m_pWord16; ///< Pointer to an unsigned 16 bit integer
        const int32_t* m_pInt32;   ///< Pointer to a signed 32 bit integer
        const uint32_t* m_pWord32; ///< Pointer to an unsigned 32 bit integer
        const int64_t* m_pInt64;   ///< Pointer to a signed 64 bit integer
        const uint64_t* m_pWord64; ///< Pointer to an unsigned 64 bit integer
        const float* m_pFloat;   ///< Pointer to a 32 bit floating point number
        const double* m_pDouble; ///< Pointer to a 64 bit floating point number
        const void* m_pVoid;     ///< Pointer to generic data
        const void* m_pSIMD;     ///< Pointer to a 128 bit vector
    } m_Data;                    ///< Data stored in the container

private:
    eType m_eType; ///< Type of object found

public:
    // The magic happens here - Argument constructors for all of the
    // types that get identified and wrapped up for processing
    // Note:  The order that the constructors appear in here can make a
    // difference to
    //        to the compilers ability to correctly match the argument to the
    //        correct constructor for it's type without promoting the argument
    //        (which is why we list them in size order, though int is an
    //        exception... trial and error
    //

    BURGER_INLINE SafePrintArgument(char arg) BURGER_NOEXCEPT: m_eType(ARG_INT8)
    {
        m_Data.m_iInt8 = arg;
    };
    BURGER_INLINE SafePrintArgument(int8_t arg) BURGER_NOEXCEPT
        : m_eType(ARG_INT8)
    {
        m_Data.m_iInt8 = arg;
    };
    BURGER_INLINE SafePrintArgument(uint8_t arg) BURGER_NOEXCEPT
        : m_eType(ARG_UINT8)
    {
        m_Data.m_uWord8 = arg;
    };

    BURGER_INLINE SafePrintArgument(int16_t arg) BURGER_NOEXCEPT
        : m_eType(ARG_INT16)
    {
        m_Data.m_iInt16 = arg;
    };
    BURGER_INLINE SafePrintArgument(uint16_t arg) BURGER_NOEXCEPT
        : m_eType(ARG_UINT16)
    {
        m_Data.m_uWord16 = arg;
    };

    BURGER_INLINE SafePrintArgument(int32_t arg) BURGER_NOEXCEPT
        : m_eType(ARG_INT32)
    {
        m_Data.m_iInt32 = arg;
    };
    BURGER_INLINE SafePrintArgument(uint32_t arg) BURGER_NOEXCEPT
        : m_eType(ARG_UINT32)
    {
        m_Data.m_uWord32 = arg;
    };

    BURGER_INLINE SafePrintArgument(int64_t arg) BURGER_NOEXCEPT
        : m_eType(ARG_INT64)
    {
        m_Data.m_iInt64 = arg;
    };
    BURGER_INLINE SafePrintArgument(uint64_t arg) BURGER_NOEXCEPT
        : m_eType(ARG_UINT64)
    {
        m_Data.m_uWord64 = arg;
    };

#if defined(BURGER_HAS_WCHAR_T) || defined(DOXYGEN)
    BURGER_INLINE SafePrintArgument(wchar_t arg) BURGER_NOEXCEPT
        : m_eType(ARG_UINT16)
    {
        m_Data.m_uWord16 = static_cast<uint16_t>(arg);
    };
#endif

#if defined(BURGER_HAS_CHAR8_T) || defined(DOXYGEN)
    BURGER_INLINE SafePrintArgument(char8_t arg) BURGER_NOEXCEPT
        : m_eType(ARG_UINT8) {
        m_Data.m_uWord8 = static_cast<uint8_t>(arg);
    };
#endif

#if defined(BURGER_HAS_CHAR16_T) || defined(DOXYGEN)
	BURGER_INLINE SafePrintArgument(char16_t arg) BURGER_NOEXCEPT
		: m_eType(ARG_UINT16) {
		m_Data.m_uWord16 = static_cast<uint16_t>(arg);
	};

	BURGER_INLINE SafePrintArgument(char32_t arg) BURGER_NOEXCEPT
		: m_eType(ARG_UINT32) {
		m_Data.m_uWord32 = static_cast<uint32_t>(arg);
	};
#endif

#if defined(BURGER_INT_NOT_IN_STDINT) || defined(DOXYGEN)
#if BURGER_SIZEOF_LONG == 4
    BURGER_INLINE SafePrintArgument(signed int arg) BURGER_NOEXCEPT
        : m_eType(ARG_INT32)
    {
        m_Data.m_iInt32 = arg;
    };
    BURGER_INLINE SafePrintArgument(unsigned int arg) BURGER_NOEXCEPT
        : m_eType(ARG_UINT32)
    {
        m_Data.m_uWord32 = arg;
    };
#else
    BURGER_INLINE SafePrintArgument(signed int arg) BURGER_NOEXCEPT
        : m_eType(ARG_INT16)
    {
        m_Data.m_iInt16 = arg;
    };
    BURGER_INLINE SafePrintArgument(unsigned int arg) BURGER_NOEXCEPT
        : m_eType(ARG_WORD16)
    {
        m_Data.m_uWord16 = arg;
    };
#endif
#endif

#if defined(BURGER_LONG_NOT_IN_STDINT) || defined(DOXYGEN)
#if BURGER_SIZEOF_LONG == 8
    BURGER_INLINE SafePrintArgument(signed long arg) BURGER_NOEXCEPT
        : m_eType(ARG_INT64)
    {
        m_Data.m_iInt64 = arg;
    };
    BURGER_INLINE SafePrintArgument(unsigned long arg) BURGER_NOEXCEPT
        : m_eType(ARG_UINT64)
    {
        m_Data.m_iInt64 = arg;
    };
#else
    BURGER_INLINE SafePrintArgument(signed long arg) BURGER_NOEXCEPT
        : m_eType(ARG_INT32)
    {
        m_Data.m_iInt32 = arg;
    };
    BURGER_INLINE SafePrintArgument(unsigned long arg) BURGER_NOEXCEPT
        : m_eType(ARG_UINT32)
    {
        m_Data.m_uWord32 = arg;
    };
#endif
#endif

    BURGER_INLINE SafePrintArgument(uint16_t arg, eType uType) BURGER_NOEXCEPT
        : m_eType(uType)
    {
        m_Data.m_fHalf = arg;
    };
    BURGER_INLINE SafePrintArgument(float arg) BURGER_NOEXCEPT
        : m_eType(ARG_FLOAT)
    {
        m_Data.m_fFloat = arg;
    };
    BURGER_INLINE SafePrintArgument(double arg) BURGER_NOEXCEPT
        : m_eType(ARG_DOUBLE)
    {
        m_Data.m_dDouble = arg;
    };

    BURGER_INLINE SafePrintArgument(bool arg) BURGER_NOEXCEPT: m_eType(ARG_BOOL)
    {
        m_Data.m_bBool = arg;
    };

    BURGER_INLINE SafePrintArgument(const String* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PSTRING)
    {
        m_Data.m_pString = arg;
    };
    BURGER_INLINE SafePrintArgument(const String& arg) BURGER_NOEXCEPT
        : m_eType(ARG_PSTRING)
    {
        m_Data.m_pString = &arg;
    };

    BURGER_INLINE SafePrintArgument(const char* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PCHAR)
    {
        m_Data.m_pChar = arg;
    };
    BURGER_INLINE SafePrintArgument(const int8_t* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PSCHAR)
    {
        m_Data.m_pInt8 = arg;
    };
    BURGER_INLINE SafePrintArgument(const uint8_t* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PUCHAR)
    {
        m_Data.m_pWord8 = arg;
    };

    BURGER_INLINE SafePrintArgument(const int16_t* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PINT16)
    {
        m_Data.m_pInt16 = arg;
    };
    BURGER_INLINE SafePrintArgument(const uint16_t* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PUINT16)
    {
        m_Data.m_pWord16 = arg;
    };

    BURGER_INLINE SafePrintArgument(const int32_t* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PINT32)
    {
        m_Data.m_pInt32 = arg;
    };
    BURGER_INLINE SafePrintArgument(const uint32_t* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PUINT32)
    {
        m_Data.m_pWord32 = arg;
    };

    BURGER_INLINE SafePrintArgument(const int64_t* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PINT64)
    {
        m_Data.m_pInt64 = arg;
    };
    BURGER_INLINE SafePrintArgument(const uint64_t* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PUINT64)
    {
        m_Data.m_pWord64 = arg;
    };

#if defined(BURGER_HAS_WCHAR_T) || defined(DOXYGEN)
    BURGER_INLINE SafePrintArgument(const wchar_t* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PUINT16)
    {
        m_Data.m_pWord16 =
            static_cast<const uint16_t*>(static_cast<const void*>(arg));
    };
#endif

#if defined(BURGER_INT_NOT_IN_STDINT) || defined(DOXYGEN)
#if BURGER_SIZEOF_LONG == 4
    BURGER_INLINE SafePrintArgument(const signed int* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PINT32)
    {
        m_Data.m_pInt32 =
            static_cast<const int32_t*>(static_cast<const void*>(arg));
    };
    BURGER_INLINE SafePrintArgument(const unsigned int* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PUINT32)
    {
        m_Data.m_pWord32 =
            static_cast<const uint32_t*>(static_cast<const void*>(arg));
    };
#else
    BURGER_INLINE SafePrintArgument(const signed int* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PINT16)
    {
        m_Data.m_pInt32 =
            static_cast<const int16_t*>(static_cast<const void*>(arg));
    };
    BURGER_INLINE SafePrintArgument(const unsigned int* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PWORD16)
    {
        m_Data.m_pWord32 =
            static_cast<const uint16_t*>(static_cast<const void*>(arg));
    };
#endif
#endif

#if defined(BURGER_LONG_NOT_IN_STDINT) || defined(DOXYGEN)
#if BURGER_SIZEOF_LONG == 8
    BURGER_INLINE SafePrintArgument(const signed long* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PINT64)
    {
        m_Data.m_pInt64 = reinterpret_cast<const int64_t*>(arg);
    };
    BURGER_INLINE SafePrintArgument(const unsigned long* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PUINT64)
    {
        m_Data.m_pWord64 = reinterpret_cast<const uint64_t*>(arg);
    };
#else
    BURGER_INLINE SafePrintArgument(const signed long* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PINT32)
    {
        m_Data.m_pInt32 =
            static_cast<const int32_t*>(static_cast<const void*>(arg));
    };
    BURGER_INLINE SafePrintArgument(const unsigned long* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PUINT32)
    {
        m_Data.m_pWord32 =
            static_cast<const uint32_t*>(static_cast<const void*>(arg));
    };
#endif
#endif

    BURGER_INLINE SafePrintArgument(const float* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PFLOAT)
    {
        m_Data.m_pFloat = arg;
    };
    BURGER_INLINE SafePrintArgument(const double* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PDOUBLE)
    {
        m_Data.m_pDouble = arg;
    };

#if defined(BURGER_X86) || defined(DOXYGEN)
    BURGER_INLINE SafePrintArgument(const __m64 arg) BURGER_NOEXCEPT
        : m_eType(ARG_SIMD64)
    {
        m_Data.m_pSIMD = &arg;
    };
    BURGER_INLINE SafePrintArgument(const __m64* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PSIMD64)
    {
        m_Data.m_pSIMD = arg;
    };
#endif

#if (defined(BURGER_INTEL) && !defined(BURGER_WATCOM)) || defined(DOXYGEN)
    BURGER_INLINE SafePrintArgument(const __m128 arg) BURGER_NOEXCEPT
        : m_eType(ARG_SIMD128)
    {
        m_Data.m_pSIMD = &arg;
    };
    BURGER_INLINE SafePrintArgument(const __m128* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PSIMD128)
    {
        m_Data.m_pSIMD = arg;
    };
    BURGER_INLINE SafePrintArgument(const __m128d arg) BURGER_NOEXCEPT
        : m_eType(ARG_SIMD128D)
    {
        m_Data.m_pSIMD = &arg;
    };
    BURGER_INLINE SafePrintArgument(const __m128d* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PSIMD128D)
    {
        m_Data.m_pSIMD = arg;
    };
    BURGER_INLINE SafePrintArgument(const __m128i arg) BURGER_NOEXCEPT
        : m_eType(ARG_SIMD128I)
    {
        m_Data.m_pSIMD = &arg;
    };
    BURGER_INLINE SafePrintArgument(const __m128i* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PSIMD128I)
    {
        m_Data.m_pSIMD = arg;
    };
#endif

    // void* has to last because the above list of pointers will be checked
    // first, and void* will catch implicit conversion
    BURGER_INLINE SafePrintArgument(const void* arg) BURGER_NOEXCEPT
        : m_eType(ARG_PVOID)
    {
        m_Data.m_pVoid = arg;
    };

    BURGER_INLINE uint_t IsNumeric(void) const BURGER_NOEXCEPT
    {
        return ((m_eType >= ARG_INT8) && (m_eType <= ARG_BOOL));
    }
    BURGER_INLINE uint_t IsInteger(void) const BURGER_NOEXCEPT
    {
        return ((m_eType >= ARG_FIRSTINT) && (m_eType <= ARG_LASTINT));
    }
    BURGER_INLINE uint_t IsSigned(void) const BURGER_NOEXCEPT
    {
        return ((m_eType >= ARG_FIRSTSIGNED) && (m_eType <= ARG_LASTSIGNED));
    }
    BURGER_INLINE uint_t IsUnsigned(void) const BURGER_NOEXCEPT
    {
        return (
            (m_eType >= ARG_FIRSTUNSIGNED) && (m_eType <= ARG_LASTUNSIGNED));
    }
    BURGER_INLINE uint_t IsBoolean(void) const BURGER_NOEXCEPT
    {
        return (m_eType == ARG_BOOL);
    }
    BURGER_INLINE uint_t IsReal(void) const BURGER_NOEXCEPT
    {
        return ((m_eType >= ARG_FIRSTREAL) && (m_eType <= ARG_LASTREAL));
    }

    uint_t BURGER_API IsNegative(void) const BURGER_NOEXCEPT;
    uint_t BURGER_API IsZero(void) const BURGER_NOEXCEPT;
    BURGER_INLINE uint_t IsCharacter(void) const BURGER_NOEXCEPT
    {
        return ((m_eType == ARG_INT8) || (m_eType == ARG_UINT8) ||
            (m_eType == ARG_UINT16));
    }
    BURGER_INLINE uint_t IsTextPointer(void) const BURGER_NOEXCEPT
    {
        return ((m_eType >= ARG_FIRSTTEXT) && (m_eType <= ARG_LASTTEXT)) ||
            (m_eType == ARG_PUINT16);
    }
    BURGER_INLINE uint_t IsPointer(void) const BURGER_NOEXCEPT
    {
        return ((m_eType >= ARG_FIRSTPTR) && (m_eType <= ARG_LASTPTR));
    }
    BURGER_INLINE uint_t Is64Bit(void) const BURGER_NOEXCEPT
    {
        return (GetDataLengthInBytes() == 8);
    }
    BURGER_INLINE uint_t IsSIMD(void) const BURGER_NOEXCEPT
    {
        return ((m_eType >= ARG_FIRSTSIMD) && (m_eType <= ARG_LASTSIMD));
    }
    BURGER_INLINE uint_t IsSIMDPointer(void) const BURGER_NOEXCEPT
    {
        return ((m_eType >= ARG_FIRSTSIMDPTR) && (m_eType <= ARG_LASTSIMDPTR));
    }
    uintptr_t BURGER_API GetDataLengthInBytes(void) const BURGER_NOEXCEPT;
    const void* BURGER_API GetDataAddress(void) const BURGER_NOEXCEPT;
    BURGER_INLINE eType GetType(void) const BURGER_NOEXCEPT
    {
        return m_eType;
    }
    BURGER_INLINE void SetType(eType uType) BURGER_NOEXCEPT
    {
        m_eType = uType;
    }
    static const char* BURGER_API GetTypeName(eType uType) BURGER_NOEXCEPT;
    BURGER_INLINE const char* BURGER_API GetTypeName(void) const BURGER_NOEXCEPT
    {
        return GetTypeName(m_eType);
    }
    int32_t BURGER_API GetInt32(void) const BURGER_NOEXCEPT;
    uint32_t BURGER_API GetUInt32(void) const BURGER_NOEXCEPT;
    int64_t BURGER_API GetInt64(void) const BURGER_NOEXCEPT;
    uint64_t BURGER_API GetUInt64(void) const BURGER_NOEXCEPT;
    char BURGER_API GetChar(void) const BURGER_NOEXCEPT;
    uint_t BURGER_API GetBool(void) const BURGER_NOEXCEPT;
    const void* BURGER_API GetPointer(void) const BURGER_NOEXCEPT;
    const char* BURGER_API GetText(void) const BURGER_NOEXCEPT;
    uintptr_t BURGER_API GetTextLength(void) const BURGER_NOEXCEPT;
};

#if !defined(DOXYGEN)
#define BURGER_SP_ARG1 Burger::SafePrintArgument a1
#define BURGER_SP_ARG2 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2
#define BURGER_SP_ARG3 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3
#define BURGER_SP_ARG4 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4

#define BURGER_SP_ARG5 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5
#define BURGER_SP_ARG6 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6
#define BURGER_SP_ARG7 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7
#define BURGER_SP_ARG8 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8

#define BURGER_SP_ARG9 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9
#define BURGER_SP_ARG10 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10
#define BURGER_SP_ARG11 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10, \
        Burger::SafePrintArgument a11
#define BURGER_SP_ARG12 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10, \
        Burger::SafePrintArgument a11, Burger::SafePrintArgument a12

#define BURGER_SP_ARG13 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10, \
        Burger::SafePrintArgument a11, Burger::SafePrintArgument a12, \
        Burger::SafePrintArgument a13
#define BURGER_SP_ARG14 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10, \
        Burger::SafePrintArgument a11, Burger::SafePrintArgument a12, \
        Burger::SafePrintArgument a13, Burger::SafePrintArgument a14
#define BURGER_SP_ARG15 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10, \
        Burger::SafePrintArgument a11, Burger::SafePrintArgument a12, \
        Burger::SafePrintArgument a13, Burger::SafePrintArgument a14, \
        Burger::SafePrintArgument a15
#define BURGER_SP_ARG16 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10, \
        Burger::SafePrintArgument a11, Burger::SafePrintArgument a12, \
        Burger::SafePrintArgument a13, Burger::SafePrintArgument a14, \
        Burger::SafePrintArgument a15, Burger::SafePrintArgument a16

#define BURGER_SP_ARG17 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10, \
        Burger::SafePrintArgument a11, Burger::SafePrintArgument a12, \
        Burger::SafePrintArgument a13, Burger::SafePrintArgument a14, \
        Burger::SafePrintArgument a15, Burger::SafePrintArgument a16, \
        Burger::SafePrintArgument a17
#define BURGER_SP_ARG18 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10, \
        Burger::SafePrintArgument a11, Burger::SafePrintArgument a12, \
        Burger::SafePrintArgument a13, Burger::SafePrintArgument a14, \
        Burger::SafePrintArgument a15, Burger::SafePrintArgument a16, \
        Burger::SafePrintArgument a17, Burger::SafePrintArgument a18
#define BURGER_SP_ARG19 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10, \
        Burger::SafePrintArgument a11, Burger::SafePrintArgument a12, \
        Burger::SafePrintArgument a13, Burger::SafePrintArgument a14, \
        Burger::SafePrintArgument a15, Burger::SafePrintArgument a16, \
        Burger::SafePrintArgument a17, Burger::SafePrintArgument a18, \
        Burger::SafePrintArgument a19
#define BURGER_SP_ARG20 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10, \
        Burger::SafePrintArgument a11, Burger::SafePrintArgument a12, \
        Burger::SafePrintArgument a13, Burger::SafePrintArgument a14, \
        Burger::SafePrintArgument a15, Burger::SafePrintArgument a16, \
        Burger::SafePrintArgument a17, Burger::SafePrintArgument a18, \
        Burger::SafePrintArgument a19, Burger::SafePrintArgument a20

#define BURGER_SP_ARG21 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10, \
        Burger::SafePrintArgument a11, Burger::SafePrintArgument a12, \
        Burger::SafePrintArgument a13, Burger::SafePrintArgument a14, \
        Burger::SafePrintArgument a15, Burger::SafePrintArgument a16, \
        Burger::SafePrintArgument a17, Burger::SafePrintArgument a18, \
        Burger::SafePrintArgument a19, Burger::SafePrintArgument a20, \
        Burger::SafePrintArgument a21
#define BURGER_SP_ARG22 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10, \
        Burger::SafePrintArgument a11, Burger::SafePrintArgument a12, \
        Burger::SafePrintArgument a13, Burger::SafePrintArgument a14, \
        Burger::SafePrintArgument a15, Burger::SafePrintArgument a16, \
        Burger::SafePrintArgument a17, Burger::SafePrintArgument a18, \
        Burger::SafePrintArgument a19, Burger::SafePrintArgument a20, \
        Burger::SafePrintArgument a21, Burger::SafePrintArgument a22
#define BURGER_SP_ARG23 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10, \
        Burger::SafePrintArgument a11, Burger::SafePrintArgument a12, \
        Burger::SafePrintArgument a13, Burger::SafePrintArgument a14, \
        Burger::SafePrintArgument a15, Burger::SafePrintArgument a16, \
        Burger::SafePrintArgument a17, Burger::SafePrintArgument a18, \
        Burger::SafePrintArgument a19, Burger::SafePrintArgument a20, \
        Burger::SafePrintArgument a21, Burger::SafePrintArgument a22, \
        Burger::SafePrintArgument a23
#define BURGER_SP_ARG24 \
    Burger::SafePrintArgument a1, Burger::SafePrintArgument a2, \
        Burger::SafePrintArgument a3, Burger::SafePrintArgument a4, \
        Burger::SafePrintArgument a5, Burger::SafePrintArgument a6, \
        Burger::SafePrintArgument a7, Burger::SafePrintArgument a8, \
        Burger::SafePrintArgument a9, Burger::SafePrintArgument a10, \
        Burger::SafePrintArgument a11, Burger::SafePrintArgument a12, \
        Burger::SafePrintArgument a13, Burger::SafePrintArgument a14, \
        Burger::SafePrintArgument a15, Burger::SafePrintArgument a16, \
        Burger::SafePrintArgument a17, Burger::SafePrintArgument a18, \
        Burger::SafePrintArgument a19, Burger::SafePrintArgument a20, \
        Burger::SafePrintArgument a21, Burger::SafePrintArgument a22, \
        Burger::SafePrintArgument a23, Burger::SafePrintArgument a24

#define BURGER_SP_INITARG1 &a1
#define BURGER_SP_INITARG2 &a1, &a2
#define BURGER_SP_INITARG3 &a1, &a2, &a3
#define BURGER_SP_INITARG4 &a1, &a2, &a3, &a4

#define BURGER_SP_INITARG5 &a1, &a2, &a3, &a4, &a5
#define BURGER_SP_INITARG6 &a1, &a2, &a3, &a4, &a5, &a6
#define BURGER_SP_INITARG7 &a1, &a2, &a3, &a4, &a5, &a6, &a7
#define BURGER_SP_INITARG8 &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8

#define BURGER_SP_INITARG9 &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9
#define BURGER_SP_INITARG10 &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10
#define BURGER_SP_INITARG11 \
    &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11
#define BURGER_SP_INITARG12 \
    &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12

#define BURGER_SP_INITARG13 \
    &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13
#define BURGER_SP_INITARG14 \
    &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14
#define BURGER_SP_INITARG15 \
    &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, \
        &a15
#define BURGER_SP_INITARG16 \
    &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, \
        &a15, &a16

#define BURGER_SP_INITARG17 \
    &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, \
        &a15, &a16, &a17
#define BURGER_SP_INITARG18 \
    &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, \
        &a15, &a16, &a17, &a18
#define BURGER_SP_INITARG19 \
    &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, \
        &a15, &a16, &a17, &a18, &a19
#define BURGER_SP_INITARG20 \
    &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, \
        &a15, &a16, &a17, &a18, &a19, &a20

#define BURGER_SP_INITARG21 \
    &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, \
        &a15, &a16, &a17, &a18, &a19, &a20, &a21
#define BURGER_SP_INITARG22 \
    &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, \
        &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22
#define BURGER_SP_INITARG23 \
    &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, \
        &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23
#define BURGER_SP_INITARG24 \
    &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, \
        &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24
#endif

#define BURGER_EXPAND_FORMATTING_FUNCTION(F) \
    F(1) \
    F(2) \
    F(3) \
    F(4) \
    F(5) \
    F(6) \
    F(7) \
    F(8) \
    F(9) \
    F(10) \
    F(11) \
    F(12) \
    F(13) F(14) F(15) F(16) F(17) F(18) F(19) F(20) F(21) F(22) F(23) F(24)

class SafePrint {
public:
    enum eError {
        /** No error */
        ERROR_NONE = 0,
        /** Bad Input Parameter */
        ERROR_BAD_INPUT_PARAMETER = -1,
        /** Invalid or Corrupted Argument */
        ERROR_BAD_ARGUMENT_TYPE = -2,
        /** Format String too long */
        ERROR_FORMAT_STRING_TOO_LONG = -5,
        /** Too many format sequences encountered */
        ERROR_TOO_MANY_FORMAT_SEQUENCES = -6,
        /** Not enough Arguments provided to the format string */
        ERROR_INSUFFICIENT_ARGUMENTS = -7,
        /** Escape character '%' is last character in format string (undefined)
         */
        ERROR_ESCAPE_IS_LAST_CHAR = -8,
        /** Width specified is too large */
        ERROR_WIDTH_VALUE_TOO_LARGE = -10,
        /** Precision specified is too large */
        ERROR_PRECISION_VALUE_TOO_LARGE = -11,
        /** Illegal character encountered in format sequence */
        ERROR_ILLEGAL_CHARACTER_FORMAT_SEQUENCE = -12,
        /** Numeric character encountered in wrong place in format sequence */
        ERROR_NUMERIC_CHAR_IN_WRONG_PLACE = -13,
        /** Argument index not in valid range */
        ERROR_ARG_INDEX_INVALID = -15,
        /** Argument index specified in wrong place in format sequence */
        ERROR_ARG_INDEX_IN_WRONG_PLACE = -16,
        /** Argument index specified twice */
        ERROR_ARG_INDEX_DOUBLY_SPECIFIED = -17,
        /** Argument index not specified */
        ERROR_ARG_INDEX_NOT_SPECIFIED = -18,
        /** Option flag encountered in wrong place in format sequence */
        ERROR_OPTION_FLAG_IN_WRONG_PLACE = -20,
        /** Option flag specified twice */
        ERROR_OPTION_FLAG_DOUBLY_SPECIFIED = -21,
        /** Precision specifier encountered in wrong place in format sequence */
        ERROR_PRECISION_IN_WRONG_PLACE = -22,
        /** Precision specified twice */
        ERROR_PRECISION_DOUBLY_SPECIFIED = -23,
        /** Multiple Argument length flags encountered */
        ERROR_ARG_LENGTH_FLAG_DOUBLY_SPECIFIED = -24,
        /** No default conversion found for argument type */
        ERROR_DEFAULT_CONVERSION_NOT_FOUND = -30,
        /** Argument type is incompatible with the conversion */
        ERROR_INCOMPATIBLE_ARG = -31,
        /** Invalid wchar_t in argument, no conversion performed */
        ERROR_INVALID_WIDECHAR_IN_ARG = -32,
        /** Invalid conversion type */
        ERROR_INVALID_CONVERSION_TYPE = -33,
        /** Error occurred in RTL function fprintf while writing formatted
           output to file */
        ERROR_WRITING_TO_FILE = -40,
        /** Error occurred in RTL function printf while writing formatted output
           to stdout */
        ERROR_WRITING_STDOUT = -41,
        /** The RTL function used to transmit formatted output reported a
           different amount of chars sent */
        ERROR_RTL_TRANSMITTED_COUNT = -42,
        /** Invalid Output Command encountered during formatting */
        ERROR_INVALID_OUTPUT_CMD = -43,
        /** Single conversion output too large */
        ERROR_CONVERSION_OUTPUT_TOO_LARGE = -44,
        /** Unspecified error */
        ERROR_UNSPECIFIED = -99
    };
    enum eWarningFlags {
        /** No Warning */
        WARN_NONE = 0,
        /** Argument signed mismatch with conversion */
        WARN_SIGNED_UNSIGNED_MISMATCH = 1 << 0,
        /** Length modifier doesn't match argument */
        WARN_LENGTH_SPECIFIER_MISMATCH = 1 << 1,
        /** Length modifier isn't valid for conversion */
        WARN_LENGTH_NOT_VALID_FOR_CONVERSION = 1 << 2,
        /** Flag undefined for conversion (ignored) */
        WARN_FLAG_UNDEFINED_FOR_CONVERSION = 1 << 3,
        /** Flag suppressed by another flag */
        WARN_FLAG_SUPPRESSED = 1 << 4,
        /** Flag had no effect */
        WARN_FLAG_NO_EFFECT = 1 << 5,
        /** Precision is undefined for conversion */
        WARN_PRECISION_UNDEFINED = 1 << 6
    };
    enum eOpcode {
        OP_NOT_A_SYMBOL = 0,                   ///< Not A (Valid) Symbol
        OP_LITERAL_PERCENT,                    ///< '%'
        OP_NUMERIC_VALUE,                      ///< '0' - '9'
        OP_ARG_SPECIFIER,                      ///< '$'
        OP_PRECISION_SPECIFIER,                ///< '.'
        OP_VALUE_FROM_ARG,                     ///< '*'
        OP_FLAG_LEFTJUSTIFY,                   ///< '-'
        OP_FLAG_FORCESIGN,                     ///< '+'
        OP_FLAG_LEAD_SPACE_POSITIVE,           ///< ' '
        OP_FLAG_ALTERNATIVE_FORM,              ///< '#'
        OP_FLAG_COMMA_SEPERATE,                ///< ','
        OP_LENGTH_SHORT,                       ///< 'h'
        OP_LENGTH_LONG,                        ///< 'l'
        OP_LENGTH_INTMAX,                      ///< 'j'
        OP_LENGTH_SIZET,                       ///< 'z'
        OP_LENGTH_WORDPTR,                     ///< 't'
        OP_LENGTH_DOUBLE,                      ///< 'L'
        OP_OUTPUT_DEFAULT,                     ///< '@'
        OP_OUTPUT_BINARY,                      ///< '!'
        OP_OUTPUT_BOOLTEXT,                    ///< 'b'
        OP_OUTPUT_BOOLTEXTUPPER,               ///< 'B'
        OP_OUTPUT_CHARACTER,                   ///< 'c'
        OP_OUTPUT_WIDECHARACTER,               ///< 'C'
        OP_OUTPUT_DECIMALINT,                  ///< 'd' or 'i'
        OP_OUTPUT_DECIMALWORD,                 ///< 'u'
        OP_OUTPUT_DECIMALFLOAT,                ///< 'f'
        OP_OUTPUT_DECIMALFLOATUPPER,           ///< 'F'
        OP_OUTPUT_HEXFLOAT,                    ///< 'a'
        OP_OUTPUT_HEXFLOATUPPER,               ///< 'A'
        OP_OUTPUT_SCI_NOTEFLOAT,               ///< 'e'
        OP_OUTPUT_SCI_NOTEFLOATUPPER,          ///< 'E'
        OP_OUTPUT_SCI_SHORTNOTE,               ///< 'g'
        OP_OUTPUT_SCI_SHORTNOTEUPPER,          ///< 'G'
        OP_OUTPUT_OCTAL,                       ///< 'o'
        OP_OUTPUT_HEX,                         ///< 'x'
        OP_OUTPUT_HEXUPPER,                    ///< 'X'
        OP_OUTPUT_POINTER,                     ///< 'p'
        OP_OUTPUT_POINTERUPPER,                ///< 'P'
        OP_OUTPUT_CSTRING,                     ///< 's'
        OP_OUTPUT_CSTRING16,                   ///< 'S'
        OP_OUTPUT_STORELENGTH,                 ///< 'n'
        OP_FLAGS_BEGIN = OP_FLAG_LEFTJUSTIFY,  ///< First flag index
        OP_FLAGS_END = OP_FLAG_COMMA_SEPERATE, ///< Last flag index
        OP_LENGTH_BEGIN = OP_LENGTH_SHORT,     ///< First length index
        OP_LENGTH_END = OP_LENGTH_DOUBLE,      ///< Last length index
        OP_OUTPUT_BEGIN = OP_OUTPUT_BINARY,    ///< First output index
        OP_OUTPUT_END = OP_OUTPUT_CSTRING16    ///< Last output index
    };
    enum eParseStage {
        PARSE_START,     ///< Parsing has started
        PARSE_ARGNUM,    ///< Parsing argument number
        PARSE_FLAGS,     ///< Parsing a flag
        PARSE_WIDTH,     ///< Parsing a width value
        PARSE_PRECISION, ///< Parsing a precision value
        PARSE_LENGTH     ///< Parsing a length value
    };
    enum eParseOptions {
        /** Strict typing is enforced */
        PARSEOPTION_STRICT_OUTPUT_TYPES_OK = 0x0001,
        /** Relaxed typing is allowed */
        PARSEOPTION_RELAXED_OUTPUT_TYPES_OK = 0x0002,
        /** Check arguments for perfect match */
        PARSEOPTION_CHECK_ARG_TYPES = 0x0004,
        /** Allow conversion of data in array */
        PARSEOPTION_ALLOW_N_CONVERSION = 0x0008,
        /** Default parsing setting */
        PARSEOPTION_DEFAULT = PARSEOPTION_STRICT_OUTPUT_TYPES_OK |
            PARSEOPTION_RELAXED_OUTPUT_TYPES_OK | PARSEOPTION_CHECK_ARG_TYPES
    };

    enum eConversionSpecifier {
        CONVSPEC_INVALID,           ///< Not valid
        CONVSPEC_BINNUMBER,         ///< Binary output
        CONVSPEC_BOOLTEXT,          ///< Boolean text ("true","false")
        CONVSPEC_CHAR,              ///< Single char
        CONVSPEC_DECIMAL_INT,       ///< Signed integer
        CONVSPEC_DECIMAL_WORD,      ///< Unsigned integer
        CONVSPEC_OCTAL,             ///< Octal output
        CONVSPEC_HEX,               ///< Hex output
        CONVSPEC_HEX_FLOAT,         ///< Float as hex
        CONVSPEC_DECIMAL_FLOAT,     ///< Standard float
        CONVSPEC_SCI_NOTE_FLOAT,    ///< Scientific notation float
        CONVSPEC_SHORTER_FLOAT,     ///< Short form float
        CONVSPEC_POINTER,           ///< Hex pointer
        CONVSPEC_TEXT_STRING,       ///< "C" string pointer
        CONVSPEC_COUNT,             ///< Number of conversion specifiers
        CONVSPEC_NOT_SUPPLIED = 255 ///< Uninitialized specification
    };

    enum eConversionSpecifierFlag {
        /** No conversion flag */
        CONVSPECFLAG_NONE = 0,
        /** Binary output flag */
        CONVSPECFLAG_BINARY_NUMBER = 1 << CONVSPEC_BINNUMBER,
        /** Boolean text flag */
        CONVSPECFLAG_BOOL_TEXT = 1 << CONVSPEC_BOOLTEXT,
        /** Single char flag */
        CONVSPECFLAG_CHARACTER = 1 << CONVSPEC_CHAR,
        /** Signed integer flag */
        CONVSPECFLAG_DECIMAL_INT = 1 << CONVSPEC_DECIMAL_INT,
        /** Unsigned integer flag */
        CONVSPECFLAG_DECIMAL_UNSIGNEDINT = 1 << CONVSPEC_DECIMAL_WORD,
        /** Octal output flag */
        CONVSPECFLAG_OCTAL = 1 << CONVSPEC_OCTAL,
        /** Hex output flag */
        CONVSPECFLAG_HEX = 1 << CONVSPEC_HEX,
        /** Float as hex flag */
        CONVSPECFLAG_HEX_FLOAT = 1 << CONVSPEC_HEX_FLOAT,
        /** Standard float flag */
        CONVSPECFLAG_DECIMAL_FLOAT = 1 << CONVSPEC_DECIMAL_FLOAT,
        /** Scientific notation float flag */
        CONVSPECFLAG_SCINOTE_FLOAT = 1 << CONVSPEC_SCI_NOTE_FLOAT,
        /** Short form float flag */
        CONVSPECFLAG_SHORTER_FLOAT = 1 << CONVSPEC_SHORTER_FLOAT,
        /** Hex pointer flag */
        CONVSPECFLAG_POINTER = 1 << CONVSPEC_POINTER,
        /** "C" string pointer flag */
        CONVSPECFLAG_TEXT_STRING = 1 << CONVSPEC_TEXT_STRING
    };

    enum eConversionFlags {
        /** No formatting options */
        CONVFLAG_NO_OPTIONS = 0,
        /** Left justification turned on */
        CONVFLAG_LEFT_JUSTIFY = 0x00000001,
        /** Display the - or + sign */
        CONVFLAG_DISPLAY_SIGN = 0x00000002,
        /** Don't display a sign */
        CONVFLAG_BLANK_SIGN = 0x00000004,
        /** Alternative form */
        CONVFLAG_ALTERNATIVE_FORM = 0x00000008,
        /** Pad the left with zeros */
        CONVFLAG_LEFT_PAD_ZEROS = 0x00000010,
        /** Comma separate numbers */
        CONVFLAG_COMMA_SEPARATE = 0x00000020,
        /** Force text to uppercase */
        CONVFLAG_UPPERCASE = 0x00000040,
        /** Interpret argument as a uint16_t */
        CONVFLAG_INTERPRET_SHORT = 0x00000100,
        /** Interpret argument as a int16_t */
        CONVFLAG_INTERPRET_SSHORT = 0x00000200,
        /** Interpret argument as a uint64_t */
        CONVFLAG_INTERPRET_LONG = 0x00000400,
        /** Interpret argument as a int64_t */
        CONVFLAG_INTERPRET_LLONG = 0x00000800,
        /** Interpret argument as a float */
        CONVFLAG_INTERPRET_DOUBLE = 0x00001000,
        /** Interpret argument as an integer */
        CONVFLAG_INTERPRET_INTMAX = 0x00002000,
        /** Interpret argument as a uintptr_t (size_t) */
        CONVFLAG_INTERPRET_SIZET = 0x00004000,
        /** Interpret argument as a uintptr_t */
        CONVFLAG_INTERPRET_WORDPTR = 0x00008000,
        /** Interpret argument as unsigned */
        CONVFLAG_INTERPRET_WORD = 0x00010000,
        /** Interpretation is specified */
        CONVFLAG_INTERPRET_SPECIFIED = 0x00080000,
        /** Width parameter was specified */
        CONVFLAG_WIDTH_SPECIFIED = 0x00100000,
        /** Precision parameter was specified */
        CONVFLAG_PRECISION_SPECIFIED = 0x00200000,
        /** Precision marker was specified */
        CONVFLAG_PRECISION_MARKER = 0x00400000,
        /** "C" string is wide */
        CONVFLAG_OUTPUT_WCHAR = 0x01000000,
        /** Used by COPY_TEXT Command */
        CONVFLAG_HAS_SEQUENCE_CHAR = 0x80000000
    };

    enum eConversionCommand {
        COMMAND_COPY_TEXT,      ///< Copy text from "C" string
        COMMAND_FORMAT_ARGUMENT ///< Processing a '%' command
    };

    struct OutputTypeAndFlags_t {
        /** Specifier for this type */
        eConversionSpecifier m_uOutputDisplayType;
        /** Modifiers allowed for this type */
        eConversionFlags m_uOutputFlagsModifier;
    };

    enum {
        /** Default character to pad the width with */
        kWIDTH_PAD_CHARACTER = ' ',
        /** Default space for sign value */
        kNUMERIC_BLANK_SIGN = ' ',
        /** Default positive sign character */
        kNUMERIC_POSITIVE_SIGN = '+',
        /** Escape character for format string. printf uses '%', that's good
           enough */
        kFORMAT_ESCAPE_CHAR = '%',
        /** Default negative sign character */
        kNUMERIC_NEGATIVE_SIGN = '-',
        /** Default numeric separator character */
        kNUMERIC_GROUP_SEPARATOR = ',',
        /** Default separator character for binary */
        kBINARY_GROUP_SEPARATOR = ' ',
        /** Default decimal point for floating point */
        kNUMERIC_DECIMAL_POINT = '.'
    };

    /** ISO/IEC 9899:TC2 spec for minimum exponent digits */
    static const uint_t kMIN_FLOAT_EXPONENT_DIGITS = 2;
    /** Maximum number of passed parameters */
    static const uint_t kMAX_OUTPUT_COMMANDS = 64;
    /** Largest number of character precision to print for a floating point
     * number */
    static const uintptr_t kMaxPrintFPrecision = 4095;
    /** Largest number of digits to print for a floating point number */
    static const uintptr_t kMaxPrintFWidth = 4095;
    /** Largest size of a single argument in bytes */
    static const uintptr_t kMAX_CONVERSION_OUTPUT_SIZE = 4095;
    /** Maximum size of the total output (Sanity check) */
    static const uintptr_t kMAX_TOTAL_OUTPUT_SIZE = 128 * 1024;

    struct ParamInfo_t {
        struct FloatDigitInfo_t {
            /** Integer digit count */
            uint16_t m_uIntSpecialDigits;
            /** Fraction digit count */
            uint16_t m_uFracDigits;
            /** Padding digit count */
            uint16_t m_uPadDigits;
            /** Number of digits for sign */
            uint8_t m_uSignDecimalDigits;
            /** Number of exponent digits */
            uint8_t m_uExponentDigits;
            /** \ref FPPrintInfo::eResult Type of floating point number found */
            uint16_t m_uFPInfoResult;
            /** Number of integer zeros */
            uint16_t m_uPadIntZeros;
            /** Number of fraction zeros */
            uint16_t m_uPadFracZeros;
            /** First integer digit index */
            uint16_t m_uFirstNonZeroIntPos;
        };

    private:
        /** \ref eConversionCommand micro command (copy memory, format argument)
         */
        uint8_t m_uCommand;
        /** \ref eConversionSpecifier output type of the argument */
        uint8_t m_uOutputConversion;
        /** Source Argument Index */
        uint8_t m_uArgIndex;
        /** \ref eWarningFlags Accumulated warnings found (Bits are OR'd
         * together) */
        uint8_t m_uWarnings;
        /** \ref eConversionFlags Output Formatting Flags */
        uint32_t m_uFormatOptionFlags;
        /** Number of characters to print */
        uint16_t m_uCharacterWidth;
        /** Number of characters of numeric precision */
        uint16_t m_uPrecision;
        /** Length of the section of formatted output in bytes */
        uint16_t m_uFormattedLength;
        /** Total Length (actual or estimated) of output (formatted + padding)
         * in bytes */
        uint16_t m_uOutputLength;

    public:
        union {
            const char* m_pText;          ///< Pointer to text if "C" string
            FloatDigitInfo_t m_FloatInfo; ///< Info on the formatted float
        };

    public:
        BURGER_INLINE eConversionCommand GetCommand(void) const BURGER_NOEXCEPT
        {
            return static_cast<eConversionCommand>(m_uCommand);
        }
        BURGER_INLINE void SetCommand(
            eConversionCommand uCommand) BURGER_NOEXCEPT
        {
            m_uCommand = static_cast<uint8_t>(uCommand);
        }

        BURGER_INLINE eConversionSpecifier GetConversion(
            void) const BURGER_NOEXCEPT
        {
            return static_cast<eConversionSpecifier>(m_uOutputConversion);
        }
        BURGER_INLINE void SetConversion(
            eConversionSpecifier uOutputConversion) BURGER_NOEXCEPT
        {
            m_uOutputConversion = static_cast<uint8_t>(uOutputConversion);
        }
        BURGER_INLINE uint32_t GetConversionAsFlag(void) const BURGER_NOEXCEPT
        {
            return static_cast<uint32_t>(1U << m_uOutputConversion);
        }

        BURGER_INLINE uint_t GetArgIndex(void) const BURGER_NOEXCEPT
        {
            return m_uArgIndex;
        }
        BURGER_INLINE void SetArgIndex(uint_t uArgIndex) BURGER_NOEXCEPT
        {
            m_uArgIndex = static_cast<uint8_t>(uArgIndex);
        }

        BURGER_INLINE uint_t GetWarnings(void) const BURGER_NOEXCEPT
        {
            return m_uWarnings;
        }
        BURGER_INLINE void SetWarning(eWarningFlags uWarning) BURGER_NOEXCEPT
        {
            m_uWarnings |= static_cast<uint8_t>(uWarning);
        }

        BURGER_INLINE uint32_t GetFlags(void) const BURGER_NOEXCEPT
        {
            return m_uFormatOptionFlags;
        }
        BURGER_INLINE void SetFlags(uint32_t uFormatOptionFlags) BURGER_NOEXCEPT
        {
            m_uFormatOptionFlags = uFormatOptionFlags;
        }
        BURGER_INLINE void SetFlag(
            eConversionFlags uFormatOptionFlags) BURGER_NOEXCEPT
        {
            m_uFormatOptionFlags |= static_cast<uint32_t>(uFormatOptionFlags);
        }
        BURGER_INLINE void ClearFlag(
            eConversionFlags uFormatOptionFlags) BURGER_NOEXCEPT
        {
            m_uFormatOptionFlags &=
                (~static_cast<uint32_t>(uFormatOptionFlags));
        }
        BURGER_INLINE uint_t IsFlagSet(
            eConversionFlags uFormatOptionFlags) const BURGER_NOEXCEPT
        {
            return ((m_uFormatOptionFlags &
                        static_cast<uint32_t>(uFormatOptionFlags)) ==
                static_cast<uint32_t>(uFormatOptionFlags));
        }
        BURGER_INLINE void ClearFlags(void) BURGER_NOEXCEPT
        {
            m_uFormatOptionFlags = CONVFLAG_NO_OPTIONS;
        }

        BURGER_INLINE uint_t GetWidth(void) const BURGER_NOEXCEPT
        {
            return m_uCharacterWidth;
        }
        BURGER_INLINE void SetWidth(uint_t uCharacterWidth) BURGER_NOEXCEPT
        {
            m_uCharacterWidth = static_cast<uint16_t>(uCharacterWidth);
        }

        BURGER_INLINE uint_t GetPrecision(void) const BURGER_NOEXCEPT
        {
            return m_uPrecision;
        }
        BURGER_INLINE void SetPrecision(uint_t uPrecision) BURGER_NOEXCEPT
        {
            m_uPrecision = static_cast<uint16_t>(uPrecision);
        }

        BURGER_INLINE uintptr_t GetFormattedLength(void) const BURGER_NOEXCEPT
        {
            return m_uFormattedLength;
        }
        BURGER_INLINE void SetFormattedLength(
            uintptr_t uFormattedLength) BURGER_NOEXCEPT
        {
            m_uFormattedLength = static_cast<uint16_t>(uFormattedLength);
        }

        BURGER_INLINE uintptr_t GetOutputLength(void) const BURGER_NOEXCEPT
        {
            return m_uOutputLength;
        }
        BURGER_INLINE void SetOutputLength(
            uintptr_t uOutputLength) BURGER_NOEXCEPT
        {
            m_uOutputLength = static_cast<uint16_t>(uOutputLength);
        }

        void BURGER_API SetFloatInfo(uint_t uIntSpecialDigits,
            uint_t uFracDigits, uint_t bSign, uint_t uDecimalPoint,
            uint_t uPadDigits, uint_t uExponentDigits) BURGER_NOEXCEPT;
        void BURGER_API GetFloatInfo(uint_t* pIntSpecialDigits,
            uint_t* pFracDigits, uint_t* pSign, uint_t* pDecimalPoint,
            uint_t* pPadDigits, uint_t* pExponentDigits) const BURGER_NOEXCEPT;

        void BURGER_API SetFloatAnalysisInfo(FPPrintInfo::eResult uFPInfoResult,
            uint_t uPadIntZeros, uint_t uPadFracZeros,
            uint_t uFirstNonZeroIntPos) BURGER_NOEXCEPT;
        void BURGER_API GetFloatAnalysisInfo(
            FPPrintInfo::eResult* pFPInfoResult, uint_t* pPadIntZeros,
            uint_t* pPadFracZeros,
            uint_t* pFirstNonZeroIntPos) const BURGER_NOEXCEPT;
        void BURGER_API SetFloatSpecialResult(
            FPPrintInfo::eResult uFPInfoResult, uint_t uIntSpecialDigits,
            uint_t uPadIntZeros, uint_t uIsNegative) BURGER_NOEXCEPT;

        BURGER_INLINE uint_t GetFloatSpecialChars(void) const BURGER_NOEXCEPT
        {
            return m_FloatInfo.m_uIntSpecialDigits;
        }
        BURGER_INLINE uint_t GetFloatSpecialLeadingZeros(
            void) const BURGER_NOEXCEPT
        {
            return m_FloatInfo.m_uPadIntZeros;
        }
        BURGER_INLINE uint_t GetFloatSpecialIsNegative(
            void) const BURGER_NOEXCEPT
        {
            return (m_FloatInfo.m_uExponentDigits != 0);
        }
        uintptr_t BURGER_API PadFieldWidth(
            char* pOutBuffer) const BURGER_NOEXCEPT;
        void BURGER_API GetFormattedOutputLength(
            const SafePrintArgument* pArg) BURGER_NOEXCEPT;
        void BURGER_API CheckConversionFlags(void) BURGER_NOEXCEPT;
        void BURGER_API CheckConversionForWarnings(
            const SafePrintArgument* pArg) BURGER_NOEXCEPT;
        eError BURGER_API StoreOffNumericValue(
            eParseStage uStage, uint32_t uCurrentNumValue) BURGER_NOEXCEPT;
        uintptr_t BURGER_API FormatInteger(char* pOutBuffer,
            const SafePrintArgument* pArg) const BURGER_NOEXCEPT;
        uintptr_t BURGER_API FormatHexOrOctal(char* pOutBuffer,
            const SafePrintArgument* pArg) const BURGER_NOEXCEPT;
        uintptr_t BURGER_API FormatPointer(char* pOutBuffer,
            const SafePrintArgument* pArg) const BURGER_NOEXCEPT;
        uintptr_t BURGER_API FormatChar(char* pOutBuffer,
            const SafePrintArgument* pArg) const BURGER_NOEXCEPT;
        uintptr_t BURGER_API FormatTextString(char* pOutBuffer,
            const SafePrintArgument* pArg) const BURGER_NOEXCEPT;
        uintptr_t BURGER_API FormatBool(char* pOutBuffer,
            const SafePrintArgument* pArg) const BURGER_NOEXCEPT;
        uintptr_t BURGER_API FormatBinary(char* pOutBuffer,
            const SafePrintArgument* pArg) const BURGER_NOEXCEPT;
        uintptr_t BURGER_API FormatSpecialReal(char* pOutBuffer,
            FPPrintInfo::eResult uSpecialResult) const BURGER_NOEXCEPT;
        uintptr_t BURGER_API FormatReal(char* pOutBuffer,
            const SafePrintArgument* pArg) const BURGER_NOEXCEPT;
    };
    struct ProcessResults_t {
        /** Number of output commands created */
        uintptr_t m_uNumOutputCommands;
        /** Size of formatted output (no trailing null) in bytes */
        uintptr_t m_uFormattedOutputLength;
        /** Position of sequence that contains error */
        uintptr_t m_uErrorFormatSequencePos;
        /** Position of character that triggered error */
        uintptr_t m_uErrorCharPos;
        /** Results of the parsing or output phase */
        eError m_uPhaseResults;
        intptr_t BURGER_API ErrorHandler(const char* pFormatString,
            uintptr_t uArgCount,
            const SafePrintArgument** ppArgs) BURGER_NOEXCEPT;
        uintptr_t BURGER_API FormatArgument(char* pOutputBuffer,
            uintptr_t uOutputBufferSize, const ParamInfo_t* pParamInfo,
            const SafePrintArgument* pArgument) BURGER_NOEXCEPT;
        uintptr_t BURGER_API GenerateFormatOutputToBuffer(char* pOutputBuffer,
            uintptr_t uOutputBufferSize, uintptr_t uArgCount,
            const SafePrintArgument** ppArgs, uintptr_t uParamInfoCount,
            const ParamInfo_t* pParamInfos) BURGER_NOEXCEPT;
        uint_t BURGER_API WriteBufferToFile(FILE* fp, uint_t bUsingSTDOUT,
            const char* pBuffer, uintptr_t uBufferSize,
            uintptr_t uCharsSoFar) BURGER_NOEXCEPT;
        uintptr_t BURGER_API GenerateFormattedOutputToFile(FILE* fp,
            uint_t bUsingSTDOUT, uintptr_t uArgCount,
            const SafePrintArgument** ppArgs, uintptr_t uParamInfoCount,
            const ParamInfo_t* pParamInfos) BURGER_NOEXCEPT;
        uint_t BURGER_API FormatPreProcess(eParseOptions uOptions,
            const char* pFormat, uintptr_t uFormatLength, uintptr_t uArgCount,
            const SafePrintArgument** ppArgs, uintptr_t uParamInfoCount,
            ParamInfo_t* pParamInfos) BURGER_NOEXCEPT;
    };
    typedef uint_t(BURGER_API* SprintfCallbackProc)(uint_t bNoErrors,
        uintptr_t uRequestedSize, void** ppOutputBuffer, void* pContext);

    static const char g_NullString[];
    static const char* g_BoolText[2];
    static const eOpcode g_CodeMap[128];
    static const eConversionFlags g_PrintFlagMappings[5];
    static const eConversionFlags g_PrintfLengthMappings[6];
    static const OutputTypeAndFlags_t g_PrintfOutputType[22];
    static const uintptr_t g_ConversionArgumentSizes[14];
    static const uint32_t g_ValidFlagsForConversion[14];
    static const uint32_t
        g_ValidOutputDisplayTypesStrict[SafePrintArgument::ARG_COUNT];
    static const uint32_t
        g_ValidOutputDisplayTypesRelaxed[SafePrintArgument::ARG_COUNT];

    static const char* BURGER_API GetErrorDescription(
        eError uType) BURGER_NOEXCEPT;
    static const char* BURGER_API GetWarningDescription(
        eWarningFlags uFlags) BURGER_NOEXCEPT;
    static eOpcode BURGER_API GetDefaultArgumentType(
        const SafePrintArgument* pArg) BURGER_NOEXCEPT;
    static const char* BURGER_API GetOutputDisplayType(
        eConversionSpecifier uType) BURGER_NOEXCEPT;
    static const char* BURGER_API GetOutputCommandName(
        eConversionCommand uCommand) BURGER_NOEXCEPT;
};

extern intptr_t BURGER_API GetFormattedLength(const char* pFormat,
    uintptr_t uArgCount, const SafePrintArgument** ppArgs) BURGER_NOEXCEPT;
extern intptr_t BURGER_API SprintfUserAlloc(
    SafePrint::SprintfCallbackProc pCallback, void* pContext, uint_t bAddNull,
    const char* pFormat, uintptr_t uArgCount,
    const SafePrintArgument** ppArgs) BURGER_NOEXCEPT;
extern intptr_t BURGER_API Sprintf(char* pOutput, const char* pFormat,
    uintptr_t uArgCount, const SafePrintArgument** ppArgs) BURGER_NOEXCEPT;
extern intptr_t BURGER_API Snprintf(char* pOutput, uintptr_t uOutputSize,
    const char* pFormat, uintptr_t uArgCount,
    const SafePrintArgument** ppArgs) BURGER_NOEXCEPT;
extern intptr_t BURGER_API Printf(const char* pFormat, uintptr_t uArgCount,
    const SafePrintArgument** ppArgs) BURGER_NOEXCEPT;
extern intptr_t BURGER_API Fprintf(FILE* fp, const char* pFormat,
    uintptr_t uArgCount, const SafePrintArgument** ppArgs) BURGER_NOEXCEPT;
extern intptr_t BURGER_API DebugSnprintf(char* pOutput, uintptr_t uOutputSize,
    const char* pFormat, uintptr_t uArgCount,
    const SafePrintArgument** ppArgs) BURGER_NOEXCEPT;

#if !defined(DOXYGEN)
BURGER_INLINE intptr_t GetFormattedLength(const char* pFormat) BURGER_NOEXCEPT
{
    return GetFormattedLength(pFormat, 0, nullptr);
}
BURGER_INLINE intptr_t SprintfUserAlloc(
    SafePrint::SprintfCallbackProc pCallback, void* pContext, uint_t bAddNull,
    const char* pFormat) BURGER_NOEXCEPT
{
    return SprintfUserAlloc(pCallback, pContext, bAddNull, pFormat, 0, nullptr);
}
BURGER_INLINE intptr_t Sprintf(
    char* pOutput, const char* pFormat) BURGER_NOEXCEPT
{
    return Sprintf(pOutput, pFormat, 0, nullptr);
}
BURGER_INLINE intptr_t Snprintf(
    char* pOutput, uintptr_t uOutputSize, const char* pFormat) BURGER_NOEXCEPT
{
    return Snprintf(pOutput, uOutputSize, pFormat, 0, nullptr);
}
BURGER_INLINE intptr_t Printf(const char* pFormat) BURGER_NOEXCEPT
{
    return Printf(pFormat, 0, nullptr);
}
BURGER_INLINE intptr_t Fprintf(FILE* fp, const char* pFormat) BURGER_NOEXCEPT
{
    return Fprintf(fp, pFormat, 0, nullptr);
}
BURGER_INLINE intptr_t DebugSnprintf(
    char* pOutput, uintptr_t uOutputSize, const char* pFormat)
{
    return DebugSnprintf(pOutput, uOutputSize, pFormat, 0, nullptr);
}

#define BURGER_TEMPMACRO(N) \
    BURGER_INLINE intptr_t GetFormattedLength( \
        const char* pFormat, BURGER_SP_ARG##N) \
    { \
        const SafePrintArgument* ArgTable[N] = {BURGER_SP_INITARG##N}; \
        return GetFormattedLength(pFormat, N, ArgTable); \
    }
BURGER_EXPAND_FORMATTING_FUNCTION(BURGER_TEMPMACRO);
#undef BURGER_TEMPMACRO

#define BURGER_TEMPMACRO(N) \
    BURGER_INLINE intptr_t SprintfUserAlloc( \
        SafePrint::SprintfCallbackProc pCallback, void* pContext, \
        uint_t bAddNull, const char* pFormat, BURGER_SP_ARG##N) \
    { \
        const SafePrintArgument* ArgTable[N] = {BURGER_SP_INITARG##N}; \
        return SprintfUserAlloc( \
            pCallback, pContext, bAddNull, pFormat, N, ArgTable); \
    }
BURGER_EXPAND_FORMATTING_FUNCTION(BURGER_TEMPMACRO);
#undef BURGER_TEMPMACRO

#define BURGER_TEMPMACRO(N) \
    BURGER_INLINE intptr_t Sprintf( \
        char* pOutput, const char* pFormat, BURGER_SP_ARG##N) \
    { \
        const SafePrintArgument* ArgTable[N] = {BURGER_SP_INITARG##N}; \
        return Sprintf(pOutput, pFormat, N, ArgTable); \
    }
BURGER_EXPAND_FORMATTING_FUNCTION(BURGER_TEMPMACRO);
#undef BURGER_TEMPMACRO

#define BURGER_TEMPMACRO(N) \
    BURGER_INLINE intptr_t Snprintf(char* pOutput, uintptr_t uOutputSize, \
        const char* pFormat, BURGER_SP_ARG##N) \
    { \
        const SafePrintArgument* ArgTable[N] = {BURGER_SP_INITARG##N}; \
        return Snprintf(pOutput, uOutputSize, pFormat, N, ArgTable); \
    }
BURGER_EXPAND_FORMATTING_FUNCTION(BURGER_TEMPMACRO);
#undef BURGER_TEMPMACRO

#define BURGER_TEMPMACRO(N) \
    BURGER_INLINE intptr_t Printf(const char* pFormat, BURGER_SP_ARG##N) \
    { \
        const SafePrintArgument* ArgTable[N] = {BURGER_SP_INITARG##N}; \
        return Printf(pFormat, N, ArgTable); \
    }
BURGER_EXPAND_FORMATTING_FUNCTION(BURGER_TEMPMACRO);
#undef BURGER_TEMPMACRO

#define BURGER_TEMPMACRO(N) \
    BURGER_INLINE intptr_t Fprintf( \
        FILE* fp, const char* pFormat, BURGER_SP_ARG##N) \
    { \
        const SafePrintArgument* ArgTable[N] = {BURGER_SP_INITARG##N}; \
        return Fprintf(fp, pFormat, N, ArgTable); \
    }
BURGER_EXPAND_FORMATTING_FUNCTION(BURGER_TEMPMACRO);
#undef BURGER_TEMPMACRO

#define BURGER_TEMPMACRO(N) \
    BURGER_INLINE intptr_t DebugSnprintf(char* pOutput, uintptr_t uOutputSize, \
        const char* pFormat, BURGER_SP_ARG##N) \
    { \
        const SafePrintArgument* ArgTable[N] = {BURGER_SP_INITARG##N}; \
        return DebugSnprintf(pOutput, uOutputSize, pFormat, N, ArgTable); \
    }
BURGER_EXPAND_FORMATTING_FUNCTION(BURGER_TEMPMACRO);
#undef BURGER_TEMPMACRO

#endif

}
/* END */
#endif
