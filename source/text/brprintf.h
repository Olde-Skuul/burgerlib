/***************************************

	Matt Pritchard's typesafe & enhanced c++ formatted output function replacements

	Note from Matt Pritchard: mprintf is dedicated to the ones I love: Jennifer, Nathan and Lauren.

	Copyright (c) 2012-2013 by Matt Pritchard <mattp_2006@yahoo.com>

	Merged into Burgerlib 2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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

#ifndef _INC_STDIO
#include <stdio.h>
#endif

/* BEGIN */
namespace Burger {

// Forward declarations of objects we will use
class String;

class GiantWord {
    BURGER_DISABLE_COPY(GiantWord);
private:
	static const Word cBitsPerElement = 32;			///< Number of bits per Word32
	static const Word cTotalBitsInTable = 32*34;	///< Number of bits in the table
	static const Word cMaxElements = cTotalBitsInTable / cBitsPerElement;	///< Number of elements in the data table

	Word m_uEntryCount;						///< Number of entries in the data table
	int m_iHighNonZeroElement;				///< Highest index with a non-zero entry in the data table
	int m_iLowNonZeroElement;				///< Lowest index with a non-zero entry in the data table
	Word m_bIsZero;							///< \ref TRUE if a zero length buffer
	Word32 m_uDataTable[cMaxElements];		///< Data table of bits for locating digits

public:
	GiantWord(void);
	explicit GiantWord(Word uBitsNeeded);
	BURGER_INLINE Word IsZero(void) const { return m_bIsZero; }
	BURGER_INLINE Word IsNotZero(void) const { return !m_bIsZero; }
	void BURGER_API Init(Word uBitsNeeded);
	void BURGER_API InsertLowBits(Word32 uBits,Word uShiftAmount);
	void BURGER_API InsertTopBits(Word32 uBits,Word uShiftAmount);
	void BURGER_API InsertBits(Word32 uBits,Word uEntryIndex);
	Word32 BURGER_API DivideAndReturnRemainder(Word32 uDivisor);
	Word32 BURGER_API MulReturnOverflow(Word32 uMultiplier);
};

extern void BURGER_API LoadHighPrecisionFromFloat(const FPInfo *pFPInfo,GiantWord *pIntegerPart,GiantWord *pFractionalPart);

class FPPrintInfo {
	enum eTextIndex {
		cPosInfinity,	///< Index to "inf"
		cNegInfinity,	///< Index to "-inf"
		cPosNan,		///< Index to "nan"
		cNegNan,		///< Index to "-nan"
		cIndNan,		///< Index to "-nan"
		cTextCount		///< Number of string indexes
	};
public:
	enum eResult {
		cResultZero,					///< Print a zero
		cResultNormalFloat,				///< Print a normal number
		cResultFloatRoundedUpAtEnd,		///< Print a number that is rounded in the fraction
		cResultFloatRoundedUpADigit,	///< Print a number that is rounded in the integer
		cResultPositiveInfinity,		///< Print a positive infinity
		cResultNegativeInfinity,		///< Print a negative infinity
		cResultPositiveNaN,				///< Print a positive NaN
		cResultNegativeNaN,				///< Print a negative NaN
		cResultSignalingNaN,			///< Print a signaling NaN
		cResultQuietNaN,				///< Print a quiet NaN
		cResultIndefiniteNaN,			///< Print an indefinite NaN
		cResultCount,					///< End of print list
		cResultFirstSpecial = cResultPositiveInfinity,	///< First special string to print
		cResultLastSpecial = cResultIndefiniteNaN		///< Last special string to print
	};
	static const Word cConversionDigitUnlimited = 0;			///< Limit of characters to convert (Default is no limit)
	static const char *g_SpecialFloatsText[cTextCount];			///< Special float message strings
	static const char *g_SpecialFloatsTextUpper[cTextCount];	///< Special float message string in uppercase
private:
	Word m_bHasIntegerPart;					///< \ref TRUE if there is an integer component
	Word m_bHasFractionalPart;				///< \ref TRUE if there is a fractional component
	Word m_uSpecialFormDigits;				///< Number of digits for special text
	Word m_uFirstNonZeroIntDigitPos;		///< Index to the first non zero integer digit
	Word m_uLastNonZeroIntDigitPos;			///< Index to the last non zero integer digit
	Word m_uFirstNonZeroFracDigitPos;		///< Index to the first non zero fractional digit
	Word m_uLastNonZeroFracDigitPos;		///< Index to the last non zero integer digit
	Word m_uPadIntZeros;					///< Number of integer zeros
	Word m_uPadFracZeros;					///< Number of fractional zeros
	eResult m_ePrintResult;					///< Type of floating point number
	static Word g_uClipFloatDigitsAfter;	///< Number of digits to print maximum (0 = no maximum)
public:
	FPPrintInfo(void) { Reset(); }
	void BURGER_API Reset(void);
	BURGER_INLINE Word HasInteger(void) const { return m_bHasIntegerPart; }
	BURGER_INLINE Word HasFractional(void) const { return m_bHasFractionalPart; }
	BURGER_INLINE Word GetSpecialFormDigits(void) const { return m_uSpecialFormDigits; }
	BURGER_INLINE Word GetFirstNonZeroIntDigitPosition(void) const { return m_uFirstNonZeroIntDigitPos; }
	BURGER_INLINE Word GetLastNonZeroIntDigitPosition(void) const { return m_uLastNonZeroIntDigitPos; }
	BURGER_INLINE Word GetFirstNonZeroFracDigitPosition(void) const { return m_uFirstNonZeroFracDigitPos; }
	BURGER_INLINE Word GetLastNonZeroFracDigitPosition(void) const { return m_uLastNonZeroFracDigitPos; }
	BURGER_INLINE Word GetIntegerZeroCount(void) const { return m_uPadIntZeros; }
	BURGER_INLINE Word GetFracZeroCount(void) const { return m_uPadFracZeros; }
	BURGER_INLINE eResult GetResult(void) const { return m_ePrintResult; }
	BURGER_INLINE Word IsSpecial(void) const { return ((m_ePrintResult >= cResultFirstSpecial) && (m_ePrintResult <= cResultLastSpecial)); }
	BURGER_INLINE Word IsInfinity(void) const { return ((m_ePrintResult == cResultPositiveInfinity) || (m_ePrintResult == cResultNegativeInfinity)); }
	BURGER_INLINE Word IsNaN(void) const { return ((m_ePrintResult >= cResultPositiveNaN) && (m_ePrintResult <= cResultIndefiniteNaN)); }
    static BURGER_INLINE Word GetClip(void) { return g_uClipFloatDigitsAfter; }
    static BURGER_INLINE void SetClip(Word uNewClip) { g_uClipFloatDigitsAfter = uNewClip; }
	void BURGER_API AnalyzeFloat(const FPInfo *pFPInfo,Word uPrecisionDigits);
};

class SafePrintArgument {
public:
	enum eType {
		ARG_INVALID = 0,	///< Invalid argument type
		ARG_INT8,			///< Signed 1 byte integer
		ARG_INT16,			///< Signed 2 byte integer
		ARG_INT32,			///< Signed 5 byte integer
		ARG_INT64,			///< Signed 8 byte integer
		ARG_WORD8,			///< Unsigned 1 byte integer
		ARG_WORD16,			///< Unsigned 2 byte integer
		ARG_WORD32,			///< Unsigned 4 byte integer
		ARG_WORD64,			///< Unsigned 8 byte integer
		ARG_HALF,			///< 16-bit IEEE 754-2008 float, aka binary16
		ARG_FLOAT,			///< 32-bit IEEE 754 float
		ARG_DOUBLE,			///< 64-bit IEEE 754 float
		ARG_BOOL,			///< bool C++ built in boolean type
		ARG_SIMD64,			///< 64-bit SIMD structure
		ARG_SIMD128,		///< 128-bit SIMD structure
		ARG_SIMD128I,		///< 128-bit SIMD structure, integer only
		ARG_SIMD128D,		///< 128-bit SIMD structure, 2 doubles only
		ARG_PSTRING,		///< Burgerlib String*									
		ARG_PCHAR,			///< const char* for a "C" string
		ARG_PSCHAR,			///< const signed char* for a "C" string
		ARG_PUCHAR,			///< const unsigned char* for a "C" string
		ARG_PINT16,			///< Pointer to an \ref Int16
		ARG_PWORD16,		///< Pointer to a \ref Word16
		ARG_PINT32,			///< Pointer to an \ref Int32
		ARG_PWORD32,		///< Pointer to a \ref Word32
		ARG_PINT64,			///< Pointer to an \ref Int64
		ARG_PWORD64,		///< Pointer to a \ref Word64
		ARG_PFLOAT,			///< Pointer to a float
		ARG_PDOUBLE,		///< Pointer to a double
		ARG_PSIMD64,		///< Pointer to a 64-bit SIMD structure
		ARG_PSIMD128,		///< Pointer to a 128-bit SIMD structure, 4 floats only
		ARG_PSIMD128I,		///< Pointer to a 128-bit SIMD structure, 4 32-bit integers
		ARG_PSIMD128D,		///< Pointer to a 128-bit SIMD structure, 2 doubles only
		ARG_PVOID,			///< void * for a catchall
		ARG_COUNT,			///< Auto list counter, all valid types must appear above this
		ARG_ANY,			///< Used to indicate 'any valid arg'
		ARG_FIRSTINT = ARG_INT8,			///< First integer entry
		ARG_LASTINT = ARG_WORD64,			///< Last integer entry
		ARG_FIRSTSIGNED = ARG_INT8,			///< First signed integer entry
		ARG_LASTSIGNED = ARG_INT64,			///< Last signed integer entry
		ARG_FIRSTUNSIGNED = ARG_WORD8,		///< First unsigned integer entry
		ARG_LASTUNSIGNED = ARG_WORD64,		///< Last unsigned integer entry
		ARG_FIRSTREAL = ARG_HALF,			///< First floating point entry
		ARG_LASTREAL = ARG_DOUBLE,			///< Last floating point entry
		ARG_FIRSTTEXT = ARG_PSTRING,		///< First "C" string pointer entry
		ARG_LASTTEXT = ARG_PUCHAR,			///< Last "C" string pointer entry
		ARG_FIRSTPTR = ARG_PSTRING,			///< First pointer type entry
		ARG_LASTPTR = ARG_PVOID,			///< Last pointer type entry
		ARG_FIRSTSIMD = ARG_SIMD64,			///< First SIMD entry
		ARG_LASTSIMD = ARG_SIMD128D,		///< Last SIMD entry
		ARG_FIRSTSIMDPTR = ARG_PSIMD64,		///< First SIMD pointer type entry
		ARG_LASTSIMDPTR = ARG_PSIMD128D		///< Last SIMD pointer type entry
	};

	static const char cInvalidCharConversion = '~';	///< Character returned from \ref GetChar() const for invalid chararacter

	union {
		int8_t m_iInt8;				///< Signed char value
		uint8_t m_uWord8;				///< Unsigned char value
		int16_t m_iInt16;				///< Signed short value
		uint16_t m_uWord16;			///< Unsigned short value
		int32_t m_iInt32;				///< Signed 32 bit integer value
		uint32_t m_uWord32;			///< Unsigned 32 bit integer value
		int64_t m_iInt64;				///< Signed 64 bit integer value
		uint64_t m_uWord64;			///< Unsigned 64 bit integer value
		uint16_t m_fHalf;				///< 16 bit floating point value
		float m_fFloat;				///< 32 bit floating point value
		double m_dDouble;			///< 64 bit floating point value
		bool m_bBool;				///< Boolean value
		const String *m_pString;	///< Pointer to a Burgerlib \ref String class
		const char *m_pChar;		///< Generic char pointer for strings
		const int8_t*m_pInt8;		///< Pointer to a signed char	
		const uint8_t*m_pWord8;		///< Pointer to an unsigned char
		const int16_t*m_pInt16;		///< Pointer to a signed 16 bit integer
		const uint16_t*m_pWord16;	///< Pointer to an unsigned 16 bit integer
		const int32_t*m_pInt32;		///< Pointer to a signed 32 bit integer
		const uint32_t*m_pWord32;	///< Pointer to an unsigned 32 bit integer
		const int64_t*m_pInt64;		///< Pointer to a signed 64 bit integer
		const uint64_t*m_pWord64;	///< Pointer to an unsigned 64 bit integer
		const float *m_pFloat;		///< Pointer to a 32 bit floating point number
		const double *m_pDouble;	///< Pointer to a 64 bit floating point number
		const void *m_pVoid;		///< Pointer to generic data
		const void *m_pSIMD;		///< Pointer to a 128 bit vector
	} m_Data;			///< Data stored in the container

private:
	eType m_eType;		///< Type of object found

public:
	// The magic happens here - Argument constructors for all of the
	// types that get identified and wrapped up for processing
	// Note:  The order that the constructors appear in here can make a difference to
	//        to the compilers ability to correctly match the argument to the correct
	//        constructor for it's type without promoting the argument (which is why
	//        we list them in size order, though int is an exception... trial and error
	//

    BURGER_INLINE SafePrintArgument(char arg) : m_eType(ARG_INT8) { m_Data.m_iInt8 = arg; };
    BURGER_INLINE SafePrintArgument(int8_t arg) : m_eType(ARG_INT8) { m_Data.m_iInt8 = arg; };
    BURGER_INLINE SafePrintArgument(uint8_t arg) : m_eType(ARG_WORD8) { m_Data.m_uWord8 = arg; };

    BURGER_INLINE SafePrintArgument(int16_t arg) : m_eType(ARG_INT16) { m_Data.m_iInt16 = arg; };
    BURGER_INLINE SafePrintArgument(uint16_t arg) : m_eType(ARG_WORD16) { m_Data.m_uWord16 = arg; };

	BURGER_INLINE SafePrintArgument(int32_t arg) : m_eType(ARG_INT32) { m_Data.m_iInt32 = arg; };
	BURGER_INLINE SafePrintArgument(uint32_t arg): m_eType(ARG_WORD32) { m_Data.m_uWord32 = arg; };

    BURGER_INLINE SafePrintArgument(int64_t arg) : m_eType(ARG_INT64) { m_Data.m_iInt64 = arg; };
    BURGER_INLINE SafePrintArgument(uint64_t arg) : m_eType(ARG_WORD64) { m_Data.m_uWord64 = arg; };

#if defined(BURGER_HAS_WCHAR_T) || defined(DOXYGEN)
	BURGER_INLINE SafePrintArgument(wchar_t arg) : m_eType(ARG_WORD16) { m_Data.m_uWord16 = static_cast<Word16>(arg); };
#endif 

#if defined(BURGER_INT_NOT_IN_STDINT) || defined(DOXYGEN)
#if BURGER_SIZEOF_LONG == 4
    BURGER_INLINE SafePrintArgument(signed int arg) : m_eType(ARG_INT32) { m_Data.m_iInt32 = arg; };
    BURGER_INLINE SafePrintArgument(unsigned int arg) : m_eType(ARG_WORD32) { m_Data.m_uWord32 = arg; };
#else
    BURGER_INLINE SafePrintArgument(signed int arg) : m_eType(ARG_INT16) { m_Data.m_iInt16 = arg; };
    BURGER_INLINE SafePrintArgument(unsigned int arg) : m_eType(ARG_WORD16) { m_Data.m_uWord16 = arg; };
#endif
#endif

#if defined(BURGER_LONG_NOT_IN_STDINT) || defined(DOXYGEN)
#if BURGER_SIZEOF_LONG == 8
	BURGER_INLINE SafePrintArgument(signed long arg) : m_eType(ARG_INT64) { m_Data.m_iInt64 = arg; };
	BURGER_INLINE SafePrintArgument(unsigned long arg): m_eType(ARG_WORD64) { m_Data.m_iInt64 = arg; };
#else
	BURGER_INLINE SafePrintArgument(signed long arg) : m_eType(ARG_INT32) { m_Data.m_iInt32 = arg; };
	BURGER_INLINE SafePrintArgument(unsigned long arg): m_eType(ARG_WORD32) { m_Data.m_uWord32 = arg; };
#endif
#endif

	BURGER_INLINE SafePrintArgument(uint16_t arg,eType uType) : m_eType(uType) { m_Data.m_fHalf = arg; };
	BURGER_INLINE SafePrintArgument(float arg) : m_eType(ARG_FLOAT) { m_Data.m_fFloat = arg; };
	BURGER_INLINE SafePrintArgument(double arg): m_eType(ARG_DOUBLE) { m_Data.m_dDouble = arg; };

	BURGER_INLINE SafePrintArgument(bool arg) : m_eType(ARG_BOOL) { m_Data.m_bBool = arg; };

	BURGER_INLINE SafePrintArgument(const String *arg) : m_eType(ARG_PSTRING) { m_Data.m_pString = arg; };
	BURGER_INLINE SafePrintArgument(const String &arg) : m_eType(ARG_PSTRING) { m_Data.m_pString = &arg; };

	BURGER_INLINE SafePrintArgument(const char *arg) : m_eType(ARG_PCHAR) { m_Data.m_pChar = arg; };
	BURGER_INLINE SafePrintArgument(const int8_t *arg) : m_eType(ARG_PSCHAR) { m_Data.m_pInt8 = arg; };
	BURGER_INLINE SafePrintArgument(const uint8_t *arg) : m_eType(ARG_PUCHAR) { m_Data.m_pWord8 = arg; };

	BURGER_INLINE SafePrintArgument(const int16_t *arg) : m_eType(ARG_PINT16) { m_Data.m_pInt16 = arg; };
	BURGER_INLINE SafePrintArgument(const uint16_t *arg ) : m_eType(ARG_PWORD16) { m_Data.m_pWord16 = arg; };

    BURGER_INLINE SafePrintArgument(const int32_t* arg) : m_eType(ARG_PINT32) { m_Data.m_pInt32 = arg; };
    BURGER_INLINE SafePrintArgument(const uint32_t* arg) : m_eType(ARG_PWORD32) { m_Data.m_pWord32 = arg; };

    BURGER_INLINE SafePrintArgument(const int64_t* arg) : m_eType(ARG_PINT64) { m_Data.m_pInt64 = arg; };
    BURGER_INLINE SafePrintArgument(const uint64_t* arg) : m_eType(ARG_PWORD64) { m_Data.m_pWord64 = arg; };


#if defined(BURGER_HAS_WCHAR_T) || defined(DOXYGEN)
	BURGER_INLINE SafePrintArgument(const wchar_t *arg) : m_eType(ARG_PWORD16) { m_Data.m_pWord16 = static_cast<const uint16_t*>(static_cast<const void *>(arg)); };
#endif 

#if defined(BURGER_INT_NOT_IN_STDINT) || defined(DOXYGEN)
#if BURGER_SIZEOF_LONG == 4
	BURGER_INLINE SafePrintArgument(const signed int *arg) : m_eType(ARG_PINT32) { m_Data.m_pInt32 = static_cast<const int32_t *>(static_cast<const void *>(arg)); };
	BURGER_INLINE SafePrintArgument(const unsigned int *arg) : m_eType(ARG_PWORD32) { m_Data.m_pWord32 = static_cast<const uint32_t *>(static_cast<const void *>(arg)); };
#else
    BURGER_INLINE SafePrintArgument(const signed int* arg) : m_eType(ARG_PINT16) { m_Data.m_pInt32 = static_cast<const int16_t*>(static_cast<const void*>(arg)); };
    BURGER_INLINE SafePrintArgument(const unsigned int* arg) : m_eType(ARG_PWORD16) { m_Data.m_pWord32 = static_cast<const uint16_t*>(static_cast<const void*>(arg)); };
#endif
#endif

#if defined(BURGER_LONG_NOT_IN_STDINT) || defined(DOXYGEN)
#if BURGER_SIZEOF_LONG == 8
	BURGER_INLINE SafePrintArgument(const signed long *arg) : m_eType(ARG_PINT64) { m_Data.m_pInt64 = reinterpret_cast<const int64_t *>(arg); };
	BURGER_INLINE SafePrintArgument(const unsigned long *arg) : m_eType(ARG_PWORD64) { m_Data.m_pWord64 = reinterpret_cast<const uint64_t *>(arg); };
#else
	BURGER_INLINE SafePrintArgument(const signed long *arg) : m_eType(ARG_PINT32) { m_Data.m_pInt32 = static_cast<const int32_t *>(static_cast<const void *>(arg)); };
	BURGER_INLINE SafePrintArgument(const unsigned long *arg) : m_eType(ARG_PWORD32) { m_Data.m_pWord32 = static_cast<const uint32_t *>(static_cast<const void *>(arg)); };
#endif
#endif

	BURGER_INLINE SafePrintArgument(const float* arg) : m_eType(ARG_PFLOAT) { m_Data.m_pFloat = arg; };
	BURGER_INLINE SafePrintArgument(const double* arg) : m_eType(ARG_PDOUBLE) { m_Data.m_pDouble = arg; };

#if defined(BURGER_X86) || defined(DOXYGEN)
	BURGER_INLINE SafePrintArgument(const __m64 arg) : m_eType(ARG_SIMD64) { m_Data.m_pSIMD = &arg; };
	BURGER_INLINE SafePrintArgument(const __m64 *arg) : m_eType(ARG_PSIMD64) { m_Data.m_pSIMD = arg; };
#endif

#if (defined(BURGER_INTEL) && !defined(BURGER_WATCOM)) || defined(DOXYGEN)
	BURGER_INLINE SafePrintArgument(const __m128 arg) : m_eType(ARG_SIMD128) { m_Data.m_pSIMD = &arg; };
	BURGER_INLINE SafePrintArgument(const __m128 *arg) : m_eType(ARG_PSIMD128) { m_Data.m_pSIMD = arg; };
	BURGER_INLINE SafePrintArgument(const __m128d arg) : m_eType(ARG_SIMD128D) { m_Data.m_pSIMD = &arg; };
	BURGER_INLINE SafePrintArgument(const __m128d *arg) : m_eType(ARG_PSIMD128D) { m_Data.m_pSIMD = arg; };
	BURGER_INLINE SafePrintArgument(const __m128i arg) : m_eType(ARG_SIMD128I) { m_Data.m_pSIMD = &arg; };
	BURGER_INLINE SafePrintArgument(const __m128i *arg) : m_eType(ARG_PSIMD128I) { m_Data.m_pSIMD = arg; };
#endif

	// void* has to last because the above list of pointers will be checked first, and void* will catch implicit conversion
	BURGER_INLINE SafePrintArgument(const void* arg) : m_eType(ARG_PVOID) { m_Data.m_pVoid = arg; };
	BURGER_INLINE SafePrintArgument(const SafePrintArgument &rArg) : m_Data(rArg.m_Data),m_eType(rArg.m_eType) { };

	BURGER_INLINE Word IsNumeric(void) const { return ((m_eType >= ARG_INT8) && (m_eType <= ARG_BOOL)); }
	BURGER_INLINE Word IsInteger(void) const { return ((m_eType >= ARG_FIRSTINT) && (m_eType <= ARG_LASTINT)); }
	BURGER_INLINE Word IsSigned(void) const { return ((m_eType >= ARG_FIRSTSIGNED) && (m_eType <= ARG_LASTSIGNED)); }
	BURGER_INLINE Word IsUnsigned(void) const { return ((m_eType >= ARG_FIRSTUNSIGNED) && (m_eType <= ARG_LASTUNSIGNED)); }
	BURGER_INLINE Word IsBoolean(void) const { return (m_eType == ARG_BOOL); }
	BURGER_INLINE Word IsReal(void) const { return ((m_eType >= ARG_FIRSTREAL) && (m_eType <= ARG_LASTREAL)); }

	Word BURGER_API IsNegative(void) const;
	Word BURGER_API IsZero(void) const;
	BURGER_INLINE Word IsCharacter(void) const { return ((m_eType == ARG_INT8) || (m_eType == ARG_WORD8) || (m_eType == ARG_WORD16)); }
	BURGER_INLINE Word IsTextPointer(void) const { return ((m_eType >= ARG_FIRSTTEXT) && (m_eType <= ARG_LASTTEXT)) || (m_eType == ARG_PWORD16); }
	BURGER_INLINE Word IsPointer(void) const { return ((m_eType >= ARG_FIRSTPTR) && (m_eType <= ARG_LASTPTR)); } 
	BURGER_INLINE Word Is64Bit(void) const { return (GetDataLengthInBytes() == 8); }
	BURGER_INLINE Word IsSIMD(void) const { return ((m_eType >= ARG_FIRSTSIMD) && (m_eType <= ARG_LASTSIMD)); }
	BURGER_INLINE Word IsSIMDPointer(void) const { return ((m_eType >= ARG_FIRSTSIMDPTR) && (m_eType <= ARG_LASTSIMDPTR)); }
	WordPtr BURGER_API GetDataLengthInBytes(void) const;
	const void * BURGER_API GetDataAddress(void) const;
	BURGER_INLINE eType GetType(void) const { return m_eType; }
	BURGER_INLINE void SetType(eType uType) { m_eType = uType; }
	static const char * BURGER_API GetTypeName(eType uType);
	BURGER_INLINE const char * BURGER_API GetTypeName(void) const { return GetTypeName(m_eType); }
	Int32 BURGER_API GetInt32(void) const;
	Word32 BURGER_API GetUInt32(void) const;
	Int64 BURGER_API GetInt64(void) const;
	Word64 BURGER_API GetUInt64(void) const;
	char BURGER_API GetChar(void) const;
	Word BURGER_API GetBool(void) const;
	const void * BURGER_API GetPointer(void) const;
	const char * BURGER_API GetText(void) const;
	WordPtr BURGER_API GetTextLength(void) const;
};

#if !defined(DOXYGEN)
#define BURGER_SP_ARG1 Burger::SafePrintArgument a1
#define BURGER_SP_ARG2 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2
#define BURGER_SP_ARG3 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3
#define BURGER_SP_ARG4 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4

#define BURGER_SP_ARG5 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5
#define BURGER_SP_ARG6 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6
#define BURGER_SP_ARG7 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7
#define BURGER_SP_ARG8 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8

#define BURGER_SP_ARG9 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9
#define BURGER_SP_ARG10 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10
#define BURGER_SP_ARG11 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10,Burger::SafePrintArgument a11
#define BURGER_SP_ARG12 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10,Burger::SafePrintArgument a11,Burger::SafePrintArgument a12

#define BURGER_SP_ARG13 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10,Burger::SafePrintArgument a11,Burger::SafePrintArgument a12,Burger::SafePrintArgument a13
#define BURGER_SP_ARG14 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10,Burger::SafePrintArgument a11,Burger::SafePrintArgument a12,Burger::SafePrintArgument a13,Burger::SafePrintArgument a14
#define BURGER_SP_ARG15 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10,Burger::SafePrintArgument a11,Burger::SafePrintArgument a12,Burger::SafePrintArgument a13,Burger::SafePrintArgument a14,Burger::SafePrintArgument a15
#define BURGER_SP_ARG16 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10,Burger::SafePrintArgument a11,Burger::SafePrintArgument a12,Burger::SafePrintArgument a13,Burger::SafePrintArgument a14,Burger::SafePrintArgument a15,Burger::SafePrintArgument a16

#define BURGER_SP_ARG17 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10,Burger::SafePrintArgument a11,Burger::SafePrintArgument a12,Burger::SafePrintArgument a13,Burger::SafePrintArgument a14,Burger::SafePrintArgument a15,Burger::SafePrintArgument a16,Burger::SafePrintArgument a17
#define BURGER_SP_ARG18 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10,Burger::SafePrintArgument a11,Burger::SafePrintArgument a12,Burger::SafePrintArgument a13,Burger::SafePrintArgument a14,Burger::SafePrintArgument a15,Burger::SafePrintArgument a16,Burger::SafePrintArgument a17,Burger::SafePrintArgument a18
#define BURGER_SP_ARG19 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10,Burger::SafePrintArgument a11,Burger::SafePrintArgument a12,Burger::SafePrintArgument a13,Burger::SafePrintArgument a14,Burger::SafePrintArgument a15,Burger::SafePrintArgument a16,Burger::SafePrintArgument a17,Burger::SafePrintArgument a18,Burger::SafePrintArgument a19
#define BURGER_SP_ARG20 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10,Burger::SafePrintArgument a11,Burger::SafePrintArgument a12,Burger::SafePrintArgument a13,Burger::SafePrintArgument a14,Burger::SafePrintArgument a15,Burger::SafePrintArgument a16,Burger::SafePrintArgument a17,Burger::SafePrintArgument a18,Burger::SafePrintArgument a19,Burger::SafePrintArgument a20

#define BURGER_SP_ARG21 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10,Burger::SafePrintArgument a11,Burger::SafePrintArgument a12,Burger::SafePrintArgument a13,Burger::SafePrintArgument a14,Burger::SafePrintArgument a15,Burger::SafePrintArgument a16,Burger::SafePrintArgument a17,Burger::SafePrintArgument a18,Burger::SafePrintArgument a19,Burger::SafePrintArgument a20,Burger::SafePrintArgument a21
#define BURGER_SP_ARG22 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10,Burger::SafePrintArgument a11,Burger::SafePrintArgument a12,Burger::SafePrintArgument a13,Burger::SafePrintArgument a14,Burger::SafePrintArgument a15,Burger::SafePrintArgument a16,Burger::SafePrintArgument a17,Burger::SafePrintArgument a18,Burger::SafePrintArgument a19,Burger::SafePrintArgument a20,Burger::SafePrintArgument a21,Burger::SafePrintArgument a22
#define BURGER_SP_ARG23 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10,Burger::SafePrintArgument a11,Burger::SafePrintArgument a12,Burger::SafePrintArgument a13,Burger::SafePrintArgument a14,Burger::SafePrintArgument a15,Burger::SafePrintArgument a16,Burger::SafePrintArgument a17,Burger::SafePrintArgument a18,Burger::SafePrintArgument a19,Burger::SafePrintArgument a20,Burger::SafePrintArgument a21,Burger::SafePrintArgument a22,Burger::SafePrintArgument a23
#define BURGER_SP_ARG24 Burger::SafePrintArgument a1,Burger::SafePrintArgument a2,Burger::SafePrintArgument a3,Burger::SafePrintArgument a4,Burger::SafePrintArgument a5,Burger::SafePrintArgument a6,Burger::SafePrintArgument a7,Burger::SafePrintArgument a8,Burger::SafePrintArgument a9,Burger::SafePrintArgument a10,Burger::SafePrintArgument a11,Burger::SafePrintArgument a12,Burger::SafePrintArgument a13,Burger::SafePrintArgument a14,Burger::SafePrintArgument a15,Burger::SafePrintArgument a16,Burger::SafePrintArgument a17,Burger::SafePrintArgument a18,Burger::SafePrintArgument a19,Burger::SafePrintArgument a20,Burger::SafePrintArgument a21,Burger::SafePrintArgument a22,Burger::SafePrintArgument a23,Burger::SafePrintArgument a24

#define	BURGER_SP_INITARG1 &a1
#define BURGER_SP_INITARG2 &a1,&a2
#define BURGER_SP_INITARG3 &a1,&a2,&a3
#define BURGER_SP_INITARG4 &a1,&a2,&a3,&a4

#define BURGER_SP_INITARG5 &a1,&a2,&a3,&a4,&a5
#define BURGER_SP_INITARG6 &a1,&a2,&a3,&a4,&a5,&a6
#define BURGER_SP_INITARG7 &a1,&a2,&a3,&a4,&a5,&a6,&a7
#define BURGER_SP_INITARG8 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8

#define BURGER_SP_INITARG9 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9
#define BURGER_SP_INITARG10 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10
#define BURGER_SP_INITARG11 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10,&a11
#define BURGER_SP_INITARG12 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10,&a11,&a12

#define BURGER_SP_INITARG13 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10,&a11,&a12,&a13
#define BURGER_SP_INITARG14 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10,&a11,&a12,&a13,&a14
#define BURGER_SP_INITARG15 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10,&a11,&a12,&a13,&a14,&a15
#define BURGER_SP_INITARG16 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10,&a11,&a12,&a13,&a14,&a15,&a16

#define BURGER_SP_INITARG17 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10,&a11,&a12,&a13,&a14,&a15,&a16,&a17
#define BURGER_SP_INITARG18 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10,&a11,&a12,&a13,&a14,&a15,&a16,&a17,&a18
#define BURGER_SP_INITARG19 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10,&a11,&a12,&a13,&a14,&a15,&a16,&a17,&a18,&a19
#define BURGER_SP_INITARG20 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10,&a11,&a12,&a13,&a14,&a15,&a16,&a17,&a18,&a19,&a20

#define BURGER_SP_INITARG21 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10,&a11,&a12,&a13,&a14,&a15,&a16,&a17,&a18,&a19,&a20,&a21
#define BURGER_SP_INITARG22 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10,&a11,&a12,&a13,&a14,&a15,&a16,&a17,&a18,&a19,&a20,&a21,&a22
#define BURGER_SP_INITARG23 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10,&a11,&a12,&a13,&a14,&a15,&a16,&a17,&a18,&a19,&a20,&a21,&a22,&a23
#define BURGER_SP_INITARG24 &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9,&a10,&a11,&a12,&a13,&a14,&a15,&a16,&a17,&a18,&a19,&a20,&a21,&a22,&a23,&a24
#endif

#define BURGER_EXPAND_FORMATTING_FUNCTION(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9) F(10) \
	F(11) F(12) F(13) F(14) F(15) F(16) F(17) F(18) F(19) F(20) \
	F(21) F(22) F(23) F(24)

class SafePrint {
public:
	enum eError {
		ERROR_NONE = 0,									///< No error
		ERROR_BAD_INPUT_PARAMETER = -1,					///< Bad Input Parameter
		ERROR_BAD_ARGUMENT_TYPE = -2, 					///< Invalid or Corrupted Argument
		ERROR_FORMAT_STRING_TOO_LONG = -5,				///< Format String too long
		ERROR_TOO_MANY_FORMAT_SEQUENCES = -6,			///< Too many format sequences encountered
		ERROR_INSUFFICIENT_ARGUMENTS = -7,				///< Not enough Arguments provided to the format string
		ERROR_ESCAPE_IS_LAST_CHAR = -8,					///< Escape character '%' is last character in format string (undefined)
		ERROR_WIDTH_VALUE_TOO_LARGE = -10,				///< Width specified is too large
		ERROR_PRECISION_VALUE_TOO_LARGE = -11,			///< Precision specified is too large
		ERROR_ILLEGAL_CHARACTER_FORMAT_SEQUENCE = -12,	///< Illegal character encountered in format sequence
		ERROR_NUMERIC_CHAR_IN_WRONG_PLACE = -13,		///< Numeric character encountered in wrong place in format sequence
		ERROR_ARG_INDEX_INVALID = -15,					///< Argument index not in valid range
		ERROR_ARG_INDEX_IN_WRONG_PLACE = -16,			///< Argument index specified in wrong place in format sequence
		ERROR_ARG_INDEX_DOUBLY_SPECIFIED = -17,			///< Argument index specified twice
		ERROR_ARG_INDEX_NOT_SPECIFIED = -18,			///< Argument index not specified
		ERROR_OPTION_FLAG_IN_WRONG_PLACE = -20,			///< Option flag encountered in wrong place in format sequence
		ERROR_OPTION_FLAG_DOUBLY_SPECIFIED = -21,		///< Option flag specified twice
		ERROR_PRECISION_IN_WRONG_PLACE = -22,			///< Precision specifier encountered in wrong place in format sequence
		ERROR_PRECISION_DOUBLY_SPECIFIED = -23,			///< Precision specified twice
		ERROR_ARG_LENGTH_FLAG_DOUBLY_SPECIFIED = -24,	///< Multiple Argument length flags encountered
		ERROR_DEFAULT_CONVERSION_NOT_FOUND = -30,		///< No default conversion found for argument type
		ERROR_INCOMPATIBLE_ARG = -31,					///< Argument type is incompatible with the conversion
		ERROR_INVALID_WIDECHAR_IN_ARG = -32,			///< Invalid wchar_t in argument, no conversion performed
		ERROR_INVALID_CONVERSION_TYPE = -33,			///< Invalid conversion type
		ERROR_WRITING_TO_FILE = -40,					///< Error occurred in RTL function fprintf while writing formatted output to file
		ERROR_WRITING_STDOUT = -41,						///< Error occurred in RTL function printf while writing formatted output to stdout
		ERROR_RTL_TRANSMITTED_COUNT = -42,				///< The RTL function used to transmit formatted output reported a different amount of chars sent
		ERROR_INVALID_OUTPUT_CMD = -43,					///< Invalid Output Command encountered during formatting
		ERROR_CONVERSION_OUTPUT_TOO_LARGE = -44,		///< Single conversion output too large
		ERROR_UNSPECIFIED = -99							///< Unspecified error
	};
	enum eWarningFlags {
		WARN_NONE = 0,									///< No Warning
		WARN_SIGNED_UNSIGNED_MISMATCH = 1 << 0,			///< Argument signed mismatch with conversion
		WARN_LENGTH_SPECIFIER_MISMATCH = 1 << 1,		///< Length modifier doesn't match argument
		WARN_LENGTH_NOT_VALID_FOR_CONVERSION = 1 << 2,	///< Length modifier isn't valid for conversion
		WARN_FLAG_UNDEFINED_FOR_CONVERSION = 1 << 3,	///< Flag undefined for conversion (ignored)
		WARN_FLAG_SUPPRESSED = 1 << 4,					///< Flag suppressed by another flag
		WARN_FLAG_NO_EFFECT = 1 << 5,					///< Flag had no effect
		WARN_PRECISION_UNDEFINED = 1 << 6				///< Precision is undefined for conversion
	};
	enum eOpcode {
		OP_NOT_A_SYMBOL = 0,					///< Not A (Valid) Symbol
		OP_LITERAL_PERCENT,						///< '%'
		OP_NUMERIC_VALUE,						///< '0' - '9'
		OP_ARG_SPECIFIER,						///< '$'
		OP_PRECISION_SPECIFIER,					///< '.'
		OP_VALUE_FROM_ARG,						///< '*'
		OP_FLAG_LEFTJUSTIFY,					///< '-'
		OP_FLAG_FORCESIGN,						///< '+'
		OP_FLAG_LEAD_SPACE_POSITIVE,			///< ' '
		OP_FLAG_ALTERNATIVE_FORM,				///< '#'
		OP_FLAG_COMMA_SEPERATE,					///< ','
		OP_LENGTH_SHORT,						///< 'h'
		OP_LENGTH_LONG,							///< 'l'
		OP_LENGTH_INTMAX,						///< 'j'
		OP_LENGTH_SIZET,						///< 'z'
		OP_LENGTH_WORDPTR,						///< 't'
		OP_LENGTH_DOUBLE,						///< 'L'
		OP_OUTPUT_DEFAULT,						///< '@'
		OP_OUTPUT_BINARY,						///< '!' 
		OP_OUTPUT_BOOLTEXT,						///< 'b'
		OP_OUTPUT_BOOLTEXTUPPER,				///< 'B'
		OP_OUTPUT_CHARACTER,					///< 'c'
		OP_OUTPUT_WIDECHARACTER,				///< 'C'
		OP_OUTPUT_DECIMALINT,					///< 'd' or 'i'
		OP_OUTPUT_DECIMALWORD,					///< 'u'
		OP_OUTPUT_DECIMALFLOAT,					///< 'f'
		OP_OUTPUT_DECIMALFLOATUPPER,			///< 'F'
		OP_OUTPUT_HEXFLOAT,						///< 'a'
		OP_OUTPUT_HEXFLOATUPPER,				///< 'A'
		OP_OUTPUT_SCI_NOTEFLOAT,				///< 'e'
		OP_OUTPUT_SCI_NOTEFLOATUPPER,			///< 'E'	
		OP_OUTPUT_SCI_SHORTNOTE,				///< 'g'
		OP_OUTPUT_SCI_SHORTNOTEUPPER,			///< 'G'
		OP_OUTPUT_OCTAL,						///< 'o'
		OP_OUTPUT_HEX,							///< 'x' 
		OP_OUTPUT_HEXUPPER,						///< 'X'
		OP_OUTPUT_POINTER,						///< 'p'
		OP_OUTPUT_POINTERUPPER,					///< 'P'
		OP_OUTPUT_CSTRING,						///< 's'
		OP_OUTPUT_CSTRING16,					///< 'S'
		OP_OUTPUT_STORELENGTH,					///< 'n'
		OP_FLAGS_BEGIN = OP_FLAG_LEFTJUSTIFY,	///< First flag index
		OP_FLAGS_END = OP_FLAG_COMMA_SEPERATE,	///< Last flag index
		OP_LENGTH_BEGIN = OP_LENGTH_SHORT,		///< First length index
		OP_LENGTH_END = OP_LENGTH_DOUBLE,		///< Last length index
		OP_OUTPUT_BEGIN = OP_OUTPUT_BINARY,		///< First output index
		OP_OUTPUT_END = OP_OUTPUT_CSTRING16		///< Last output index
	};
	enum eParseStage {
		PARSE_START,		///< Parsing has started
		PARSE_ARGNUM,		///< Parsing argument number
		PARSE_FLAGS,		///< Parsing a flag
		PARSE_WIDTH,		///< Parsing a width value
		PARSE_PRECISION,	///< Parsing a precision value
		PARSE_LENGTH		///< Parsing a length value
	};
	enum eParseOptions {
		PARSEOPTION_STRICT_OUTPUT_TYPES_OK = 0x0001,	///< Strict typing is enforced
		PARSEOPTION_RELAXED_OUTPUT_TYPES_OK = 0x0002,	///< Relaxed typing is allowed
		PARSEOPTION_CHECK_ARG_TYPES = 0x0004,			///< Check arguments for perfect match
		PARSEOPTION_ALLOW_N_CONVERSION = 0x0008,		///< Allow conversion of data in array
		PARSEOPTION_DEFAULT = PARSEOPTION_STRICT_OUTPUT_TYPES_OK | PARSEOPTION_RELAXED_OUTPUT_TYPES_OK | PARSEOPTION_CHECK_ARG_TYPES	///< Default parsing setting
	};
	enum eConversionSpecifier {
		CONVSPEC_INVALID,			///< Not valid
		CONVSPEC_BINNUMBER,			///< Binary output
		CONVSPEC_BOOLTEXT,			///< Boolean text ("true","false")
		CONVSPEC_CHAR,				///< Single char
		CONVSPEC_DECIMAL_INT,		///< Signed integer
		CONVSPEC_DECIMAL_WORD,		///< Unsigned integer
		CONVSPEC_OCTAL,				///< Octal output
		CONVSPEC_HEX,				///< Hex output
		CONVSPEC_HEX_FLOAT,			///< Float as hex
		CONVSPEC_DECIMAL_FLOAT,		///< Standard float
		CONVSPEC_SCI_NOTE_FLOAT,	///< Scientific notation float
		CONVSPEC_SHORTER_FLOAT,		///< Short form float
		CONVSPEC_POINTER,			///< Hex pointer
		CONVSPEC_TEXT_STRING,		///< "C" string pointer
		CONVSPEC_COUNT,				///< Number of conversion specifiers
		CONVSPEC_NOT_SUPPLIED = 255	///< Uninitialized specification
	};
	enum eConversionSpecifierFlag {
		CONVSPECFLAG_NONE = 0,											///< No conversion flag
		CONVSPECFLAG_BINARY_NUMBER = 1 << CONVSPEC_BINNUMBER,			///< Binary output flag
		CONVSPECFLAG_BOOL_TEXT = 1 << CONVSPEC_BOOLTEXT,				///< Boolean text flag
		CONVSPECFLAG_CHARACTER = 1 << CONVSPEC_CHAR,					///< Single char flag
		CONVSPECFLAG_DECIMAL_INT = 1 << CONVSPEC_DECIMAL_INT,			///< Signed integer flag
		CONVSPECFLAG_DECIMAL_UNSIGNEDINT = 1 << CONVSPEC_DECIMAL_WORD,	///< Unsigned integer flag
		CONVSPECFLAG_OCTAL = 1 << CONVSPEC_OCTAL,						///< Octal output flag
		CONVSPECFLAG_HEX = 1 << CONVSPEC_HEX,							///< Hex output flag
		CONVSPECFLAG_HEX_FLOAT = 1 << CONVSPEC_HEX_FLOAT,				///< Float as hex flag
		CONVSPECFLAG_DECIMAL_FLOAT = 1 << CONVSPEC_DECIMAL_FLOAT,		///< Standard float flag
		CONVSPECFLAG_SCINOTE_FLOAT = 1 << CONVSPEC_SCI_NOTE_FLOAT,		///< Scientific notation float flag
		CONVSPECFLAG_SHORTER_FLOAT = 1 << CONVSPEC_SHORTER_FLOAT,		///< Short form float flag
		CONVSPECFLAG_POINTER = 1 << CONVSPEC_POINTER,					///< Hex pointer flag
		CONVSPECFLAG_TEXT_STRING = 1 << CONVSPEC_TEXT_STRING			///< "C" string pointer flag
	};
	enum eConversionFlags {
		CONVFLAG_NO_OPTIONS = 0,					///< No formatting options
		CONVFLAG_LEFT_JUSTIFY = 0x00000001,			///< Left justification turned on
		CONVFLAG_DISPLAY_SIGN = 0x00000002,			///< Display the - or + sign
		CONVFLAG_BLANK_SIGN = 0x00000004,			///< Don't display a sign
		CONVFLAG_ALTERNATIVE_FORM = 0x00000008,		///< Alternative form
		CONVFLAG_LEFT_PAD_ZEROS = 0x00000010,		///< Pad the left with zeros
		CONVFLAG_COMMA_SEPARATE = 0x00000020,		///< Comma separate numbers
		CONVFLAG_UPPERCASE = 0x00000040,			///< Force text to uppercase
		CONVFLAG_INTERPRET_SHORT = 0x00000100,		///< Interpret argument as a Word16
		CONVFLAG_INTERPRET_SSHORT = 0x00000200,		///< Interpret argument as a Int16
		CONVFLAG_INTERPRET_LONG = 0x00000400,		///< Interpret argument as a Word64
		CONVFLAG_INTERPRET_LLONG = 0x00000800,		///< Interpret argument as a Int64
		CONVFLAG_INTERPRET_DOUBLE = 0x00001000,		///< Interpret argument as a float
		CONVFLAG_INTERPRET_INTMAX = 0x00002000,		///< Interpret argument as an integer
		CONVFLAG_INTERPRET_SIZET = 0x00004000,		///< Interpret argument as a WordPtr (size_t)
		CONVFLAG_INTERPRET_WORDPTR = 0x00008000,	///< Interpret argument as a WordPtr
		CONVFLAG_INTERPRET_WORD = 0x00010000,		///< Interpret argument as unsigned
		CONVFLAG_INTERPRET_SPECIFIED = 0x00080000,	///< Interpretation is specified
		CONVFLAG_WIDTH_SPECIFIED = 0x00100000,		///< Width parameter was specified
		CONVFLAG_PRECISION_SPECIFIED = 0x00200000,	///< Precision parameter was specified
		CONVFLAG_PRECISION_MARKER = 0x00400000,		///< Precision marker was specified
		CONVFLAG_OUTPUT_WCHAR = 0x01000000,			///< "C" string is wide
		CONVFLAG_HAS_SEQUENCE_CHAR = 0x80000000		///< Used by COPY_TEXT Command
	};
	enum eConversionCommand {
		COMMAND_COPY_TEXT,			///< Copy text from "C" string
		COMMAND_FORMAT_ARGUMENT		///< Processing a '%' command
	};
	struct OutputTypeAndFlags_t {
		eConversionSpecifier m_uOutputDisplayType;	///< Specifier for this type
		eConversionFlags m_uOutputFlagsModifier;	///< Modifiers allowed for this type
	};

	enum {
		cWIDTH_PAD_CHARACTER = ' ',		///< Default character to pad the width with
		cNUMERIC_BLANK_SIGN = ' ',		///< Default space for sign value
		cNUMERIC_POSITIVE_SIGN = '+',	///< Default positive sign character
		cFORMAT_ESCAPE_CHAR = '%',		///< Escape character for format string. printf uses '%', that's good enough
		cNUMERIC_NEGATIVE_SIGN = '-',	///< Default negative sign character
		cNUMERIC_GROUP_SEPARATOR = ',',	///< Default numeric separator character
		cBINARY_GROUP_SEPARATOR = ' ',	///< Default separator character for binary
		cNUMERIC_DECIMAL_POINT = '.'	///< Default decimal point for floating point
	};
	static const Word cMIN_FLOAT_EXPONENT_DIGITS = 2;	///< ISO/IEC 9899:TC2 spec for minimum exponent digits
	static const Word cMAX_OUTPUT_COMMANDS = 64;		///< Maximum number of passed parameters
	static const WordPtr cMaxPrintFPrecision = 4095;	///< Largest number of character precision to print for a floating point number
	static const WordPtr cMaxPrintFWidth = 4095;		///< Largest number of digits to print for a floating point number
	static const WordPtr cMAX_CONVERSION_OUTPUT_SIZE = 4095;	///< Largest size of a single argument in bytes
	static const WordPtr cMAX_TOTAL_OUTPUT_SIZE = 128 * 1024;	///< Maximum size of the total output (Sanity check)
	struct ParamInfo_t {
		struct FloatDigitInfo_t {
			Word16 m_uIntSpecialDigits;		///< Integer digit count
			Word16 m_uFracDigits;			///< Fraction digit count
			Word16 m_uPadDigits;			///< Padding digit count
			Word8 m_uSignDecimalDigits;		///< Number of digits for sign
			Word8 m_uExponentDigits;		///< Number of exponent digits
			Word16 m_uFPInfoResult;			///< \ref FPPrintInfo::eResult Type of floating point number found
			Word16 m_uPadIntZeros;			///< Number of integer zeros
			Word16 m_uPadFracZeros;			///< Number of fraction zeros
			Word16 m_uFirstNonZeroIntPos;	///< First integer digit index
		};

	private:
		Word8 m_uCommand;				///< \ref eConversionCommand micro command (copy memory, format argument)
		Word8 m_uOutputConversion;		///< \ref eConversionSpecifier output type of the argument
		Word8 m_uArgIndex;				///< Source Argument Index
		Word8 m_uWarnings;				///< \ref eWarningFlags Accumulated warnings found (Bits are OR'd together)
		Word32 m_uFormatOptionFlags;	///< \ref eConversionFlags Output Formatting Flags
		Word16 m_uCharacterWidth;		///< Number of characters to print
		Word16 m_uPrecision;			///< Number of characters of numeric precision
		Word16 m_uFormattedLength;		///< Length of the section of formatted output in bytes
		Word16 m_uOutputLength;			///< Total Length (actual or estimated) of output (formatted + padding) in bytes
	public:
		union {
			const char *m_pText;			///< Pointer to text if "C" string
			FloatDigitInfo_t m_FloatInfo;	///< Info on the formatted float
		};

	public:
		BURGER_INLINE eConversionCommand GetCommand(void) const { return static_cast<eConversionCommand>(m_uCommand); }
		BURGER_INLINE void SetCommand(eConversionCommand uCommand) { m_uCommand = static_cast<Word8>(uCommand); }

		BURGER_INLINE eConversionSpecifier GetConversion(void) const { return static_cast<eConversionSpecifier>(m_uOutputConversion); }
		BURGER_INLINE void SetConversion(eConversionSpecifier uOutputConversion) { m_uOutputConversion = static_cast<Word8>(uOutputConversion); }
		BURGER_INLINE Word32 GetConversionAsFlag(void) const { return static_cast<Word32>(1U << m_uOutputConversion); }

		BURGER_INLINE Word GetArgIndex(void) const { return m_uArgIndex; }
		BURGER_INLINE void SetArgIndex(Word uArgIndex) { m_uArgIndex = static_cast<Word8>(uArgIndex); }

		BURGER_INLINE Word GetWarnings(void) const { return m_uWarnings; }
		BURGER_INLINE void SetWarning(eWarningFlags uWarning) { m_uWarnings |= static_cast<Word8>(uWarning); }

		BURGER_INLINE Word32 GetFlags(void) const { return m_uFormatOptionFlags; }
		BURGER_INLINE void SetFlags(Word32 uFormatOptionFlags) { m_uFormatOptionFlags = uFormatOptionFlags; }
		BURGER_INLINE void SetFlag(eConversionFlags uFormatOptionFlags) { m_uFormatOptionFlags |= static_cast<Word32>(uFormatOptionFlags); }
		BURGER_INLINE void ClearFlag(eConversionFlags uFormatOptionFlags) { m_uFormatOptionFlags &= (~static_cast<Word32>(uFormatOptionFlags)); }
		BURGER_INLINE Word IsFlagSet(eConversionFlags uFormatOptionFlags) const { return ((m_uFormatOptionFlags & static_cast<Word32>(uFormatOptionFlags)) == static_cast<Word32>(uFormatOptionFlags)); }
		BURGER_INLINE void ClearFlags(void) { m_uFormatOptionFlags = CONVFLAG_NO_OPTIONS; }

		BURGER_INLINE Word GetWidth(void) const { return m_uCharacterWidth; }
		BURGER_INLINE void SetWidth(Word uCharacterWidth) { m_uCharacterWidth = static_cast<Word16>(uCharacterWidth); }

		BURGER_INLINE Word GetPrecision(void) const { return m_uPrecision; }
		BURGER_INLINE void SetPrecision(Word uPrecision) { m_uPrecision = static_cast<Word16>(uPrecision); }

		BURGER_INLINE WordPtr GetFormattedLength(void) const { return m_uFormattedLength; }
		BURGER_INLINE void SetFormattedLength(WordPtr uFormattedLength) { m_uFormattedLength = static_cast<Word16>(uFormattedLength); }

		BURGER_INLINE WordPtr GetOutputLength(void) const { return m_uOutputLength; }
		BURGER_INLINE void SetOutputLength(WordPtr uOutputLength) { m_uOutputLength = static_cast<Word16>(uOutputLength); }

		void BURGER_API SetFloatInfo(Word uIntSpecialDigits,Word uFracDigits,Word bSign,Word uDecimalPoint,Word uPadDigits,Word uExponentDigits);
		void BURGER_API GetFloatInfo(Word *pIntSpecialDigits,Word *pFracDigits,Word *pSign,Word *pDecimalPoint,Word *pPadDigits,Word *pExponentDigits) const;

		void BURGER_API SetFloatAnalysisInfo(FPPrintInfo::eResult uFPInfoResult,Word uPadIntZeros,Word uPadFracZeros,Word uFirstNonZeroIntPos);
		void BURGER_API GetFloatAnalysisInfo(FPPrintInfo::eResult *pFPInfoResult,Word *pPadIntZeros,Word *pPadFracZeros,Word *pFirstNonZeroIntPos) const;
		void BURGER_API SetFloatSpecialResult(FPPrintInfo::eResult uFPInfoResult,Word uIntSpecialDigits,Word uPadIntZeros,Word uIsNegative);

		BURGER_INLINE Word GetFloatSpecialChars(void) const { return m_FloatInfo.m_uIntSpecialDigits; }
		BURGER_INLINE Word GetFloatSpecialLeadingZeros(void) const { return m_FloatInfo.m_uPadIntZeros; }
		BURGER_INLINE Word GetFloatSpecialIsNegative(void) const { return (m_FloatInfo.m_uExponentDigits != 0); }
		WordPtr BURGER_API PadFieldWidth(char *pOutBuffer) const;
		void BURGER_API GetFormattedOutputLength(const SafePrintArgument *pArg);
		void BURGER_API CheckConversionFlags(void);
		void BURGER_API CheckConversionForWarnings(const SafePrintArgument *pArg);
		eError BURGER_API StoreOffNumericValue(eParseStage uStage,Word32 uCurrentNumValue);
		WordPtr BURGER_API FormatInteger(char *pOutBuffer,const SafePrintArgument *pArg) const;
		WordPtr BURGER_API FormatHexOrOctal(char *pOutBuffer,const SafePrintArgument *pArg) const;
		WordPtr BURGER_API FormatPointer(char* pOutBuffer,const SafePrintArgument *pArg) const;
		WordPtr BURGER_API FormatChar(char *pOutBuffer,const SafePrintArgument *pArg) const;
		WordPtr BURGER_API FormatTextString(char *pOutBuffer,const SafePrintArgument *pArg) const;
		WordPtr BURGER_API FormatBool(char *pOutBuffer,const SafePrintArgument *pArg) const;
		WordPtr BURGER_API FormatBinary(char *pOutBuffer,const SafePrintArgument *pArg) const;
		WordPtr BURGER_API FormatSpecialReal(char *pOutBuffer,FPPrintInfo::eResult uSpecialResult) const;
		WordPtr BURGER_API FormatReal(char *pOutBuffer,const SafePrintArgument *pArg) const;
	};
	struct ProcessResults_t {
		WordPtr m_uNumOutputCommands;		///< Number of output commands created
		WordPtr m_uFormattedOutputLength;	///< Size of formatted output (no trailing null) in bytes
		WordPtr m_uErrorFormatSequencePos;	///< Position of sequence that contains error
		WordPtr m_uErrorCharPos;			///< Position of character that triggered error
		eError m_uPhaseResults;				///< Results of the parsing or output phase
		IntPtr BURGER_API ErrorHandler(const char *pFormatString,WordPtr uArgCount,const SafePrintArgument** ppArgs);
		WordPtr BURGER_API FormatArgument(char *pOutputBuffer,WordPtr uOutputBufferSize,const ParamInfo_t* pParamInfo,const SafePrintArgument *pArgument);
		WordPtr BURGER_API GenerateFormatOutputToBuffer(char *pOutputBuffer,WordPtr uOutputBufferSize,WordPtr uArgCount,const SafePrintArgument **ppArgs,WordPtr uParamInfoCount,const ParamInfo_t *pParamInfos);
		Word BURGER_API WriteBufferToFile(FILE *fp,Word bUsingSTDOUT,const char *pBuffer,WordPtr uBufferSize,WordPtr uCharsSoFar);
		WordPtr BURGER_API GenerateFormattedOutputToFile(FILE *fp,Word bUsingSTDOUT,WordPtr uArgCount,const SafePrintArgument **ppArgs,WordPtr uParamInfoCount,const ParamInfo_t* pParamInfos);
		Word BURGER_API FormatPreProcess(eParseOptions uOptions,const char *pFormat,WordPtr uFormatLength,WordPtr uArgCount,const SafePrintArgument** ppArgs,WordPtr uParamInfoCount,ParamInfo_t *pParamInfos);
	};
	typedef Word (BURGER_API *SprintfCallbackProc)(Word bNoErrors,WordPtr uRequestedSize,void **ppOutputBuffer,void *pContext);

	static const char g_NullString[];
	static const char *g_BoolText[2];
	static const eOpcode g_CodeMap[128];
	static const eConversionFlags g_PrintFlagMappings[5];
	static const eConversionFlags g_PrintfLengthMappings[6];
	static const OutputTypeAndFlags_t g_PrintfOutputType[22];
	static const WordPtr g_ConversionArgumentSizes[14];
	static const Word32 g_ValidFlagsForConversion[14];
	static const Word32 g_ValidOutputDisplayTypesStrict[SafePrintArgument::ARG_COUNT];
	static const Word32 g_ValidOutputDisplayTypesRelaxed[SafePrintArgument::ARG_COUNT];

	static const char* BURGER_API GetErrorDescription(eError uType);
	static const char* BURGER_API GetWarningDescription(eWarningFlags uFlags);
	static eOpcode BURGER_API GetDefaultArgumentType(const SafePrintArgument* pArg);
	static const char* BURGER_API GetOutputDisplayType(eConversionSpecifier uType);
	static const char* BURGER_API GetOutputCommandName(eConversionCommand uCommand);
};

extern IntPtr BURGER_API GetFormattedLength(const char *pFormat,WordPtr uArgCount,const SafePrintArgument **ppArgs);
extern IntPtr BURGER_API SprintfUserAlloc(SafePrint::SprintfCallbackProc pCallback,void *pContext,Word bAddNull,const char *pFormat,WordPtr uArgCount,const SafePrintArgument **ppArgs);
extern IntPtr BURGER_API Sprintf(char *pOutput,const char *pFormat,WordPtr uArgCount,const SafePrintArgument **ppArgs);
extern IntPtr BURGER_API Snprintf(char *pOutput,WordPtr uOutputSize,const char *pFormat,WordPtr uArgCount,const SafePrintArgument **ppArgs);
extern IntPtr BURGER_API Printf(const char *pFormat,WordPtr uArgCount,const SafePrintArgument** ppArgs);
extern IntPtr BURGER_API Fprintf(FILE *fp,const char *pFormat,WordPtr uArgCount,const SafePrintArgument **ppArgs);
extern IntPtr BURGER_API DebugSnprintf(char *pOutput,WordPtr uOutputSize,const char *pFormat,WordPtr uArgCount,const SafePrintArgument **ppArgs);

#if !defined(DOXYGEN)
BURGER_INLINE IntPtr GetFormattedLength(const char *pFormat) { return GetFormattedLength(pFormat,0,NULL); }
BURGER_INLINE IntPtr SprintfUserAlloc(SafePrint::SprintfCallbackProc pCallback,void *pContext,Word bAddNull,const char *pFormat) { return SprintfUserAlloc(pCallback,pContext,bAddNull,pFormat,0,NULL); }
BURGER_INLINE IntPtr Sprintf(char *pOutput,const char *pFormat) { return Sprintf(pOutput,pFormat,0,NULL); }
BURGER_INLINE IntPtr Snprintf(char *pOutput,WordPtr uOutputSize,const char *pFormat) { return Snprintf(pOutput,uOutputSize,pFormat,0,NULL); }
BURGER_INLINE IntPtr Printf(const char* pFormat) { return Printf(pFormat,0,NULL); }
BURGER_INLINE IntPtr Fprintf(FILE *fp,const char* pFormat) { return Fprintf(fp,pFormat,0,NULL); }
BURGER_INLINE IntPtr DebugSnprintf(char *pOutput,WordPtr uOutputSize,const char *pFormat) { return DebugSnprintf(pOutput,uOutputSize,pFormat,0,NULL); }

#define BURGER_TEMPMACRO(N) \
	BURGER_INLINE IntPtr GetFormattedLength(const char *pFormat,BURGER_SP_ARG##N) { \
	const SafePrintArgument *ArgTable[N] = { BURGER_SP_INITARG##N }; \
	return GetFormattedLength(pFormat,N,ArgTable); }
BURGER_EXPAND_FORMATTING_FUNCTION(BURGER_TEMPMACRO);
#undef BURGER_TEMPMACRO

#define BURGER_TEMPMACRO(N) \
	BURGER_INLINE IntPtr SprintfUserAlloc(SafePrint::SprintfCallbackProc pCallback,void *pContext,Word bAddNull,const char *pFormat,BURGER_SP_ARG##N) { \
	const SafePrintArgument *ArgTable[N] = { BURGER_SP_INITARG##N }; \
	return SprintfUserAlloc(pCallback,pContext,bAddNull,pFormat,N,ArgTable); }
BURGER_EXPAND_FORMATTING_FUNCTION(BURGER_TEMPMACRO);
#undef BURGER_TEMPMACRO

#define BURGER_TEMPMACRO(N) \
	BURGER_INLINE IntPtr Sprintf(char *pOutput,const char *pFormat,BURGER_SP_ARG##N) { \
	const SafePrintArgument *ArgTable[N] = { BURGER_SP_INITARG##N }; \
	return Sprintf(pOutput,pFormat,N,ArgTable); }
BURGER_EXPAND_FORMATTING_FUNCTION(BURGER_TEMPMACRO);
#undef BURGER_TEMPMACRO

#define BURGER_TEMPMACRO(N) \
BURGER_INLINE IntPtr Snprintf(char *pOutput,WordPtr uOutputSize,const char*pFormat, BURGER_SP_ARG##N) { \
	const SafePrintArgument *ArgTable[N] = { BURGER_SP_INITARG##N }; \
	return Snprintf(pOutput,uOutputSize,pFormat,N,ArgTable); }
BURGER_EXPAND_FORMATTING_FUNCTION(BURGER_TEMPMACRO);
#undef BURGER_TEMPMACRO

#define BURGER_TEMPMACRO(N) \
BURGER_INLINE IntPtr Printf(const char *pFormat,BURGER_SP_ARG##N) { \
	const SafePrintArgument* ArgTable[ N ] = { BURGER_SP_INITARG##N };	\
	return Printf(pFormat,N,ArgTable); }
BURGER_EXPAND_FORMATTING_FUNCTION(BURGER_TEMPMACRO);
#undef BURGER_TEMPMACRO

#define BURGER_TEMPMACRO(N) \
	BURGER_INLINE IntPtr Fprintf(FILE *fp,const char *pFormat,BURGER_SP_ARG##N) { \
	const SafePrintArgument* ArgTable[ N ] = { BURGER_SP_INITARG##N };	\
	return Fprintf(fp,pFormat,N,ArgTable); }
BURGER_EXPAND_FORMATTING_FUNCTION(BURGER_TEMPMACRO);
#undef BURGER_TEMPMACRO

#define BURGER_TEMPMACRO(N) \
	BURGER_INLINE IntPtr DebugSnprintf(char *pOutput,WordPtr uOutputSize,const char* pFormat,BURGER_SP_ARG##N ) { \
	const SafePrintArgument* ArgTable[N] = { BURGER_SP_INITARG##N };	\
	return DebugSnprintf(pOutput,uOutputSize,pFormat,N,ArgTable); }
BURGER_EXPAND_FORMATTING_FUNCTION(BURGER_TEMPMACRO);
#undef BURGER_TEMPMACRO

#endif

}
/* END */
#endif
