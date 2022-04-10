/***************************************

	Typed Argument class

	Copyright (c) 2022-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRARGUMENTTYPE_H__
#define __BRARGUMENTTYPE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRALGORITHM_H__
#include "bralgorithm.h"
#endif

/* BEGIN */
namespace Burger {

// Forward declarations of objects we will use
class String;

enum eArgumentType {
	/** Invalid argument type */
	kArgumentTypeInvalid,

	/** Signed 1 byte integer */
	kArgumentTypeInt8,
	/** Signed 2 byte integer */
	kArgumentTypeInt16,
	/** Signed 4 byte integer */
	kArgumentTypeInt32,
	/** Signed 8 byte integer */
	kArgumentTypeInt64,

	/** Unsigned 1 byte integer */
	kArgumentTypeUInt8,
	/** Unsigned 2 byte integer */
	kArgumentTypeUInt16,
	/** Unsigned 4 byte integer */
	kArgumentTypeUInt32,
	/** Unsigned 8 byte integer */
	kArgumentTypeUInt64,

	/** 16-bit IEEE 754-2008 float, aka binary16 */
	kArgumentTypeFloat16,
	/** 32-bit IEEE 754 float */
	kArgumentTypeFloat32,
	/** 64-bit IEEE 754 float */
	kArgumentTypeFloat64,

	/** bool C++ built in boolean type */
	kArgumentTypeBool,

	/** 64-bit SIMD structure */
	kArgumentTypeVector2,
	/** 128-bit SIMD structure */
	kArgumentTypeVector4,
	/** 128-bit SIMD structure, integer only */
	kArgumentTypeVector4Int,
	/** 128-bit SIMD structure, 2 doubles only */
	kArgumentTypeVector4Dbl,

	/** Burgerlib String */
	kArgumentTypeBurgerStringPtr,
	/** const char* for a "C" string */
	kArgumentTypeCharPtr,
	/** const signed char* for a "C" string */
	kArgumentTypeInt8Ptr,
	/** const unsigned char* for a "C" string */
	kArgumentTypeUInt8Ptr,
	/** Pointer to an int16_t */
	kArgumentTypeInt16Ptr,
	/** Pointer to a uint16_t */
	kArgumentTypeUInt16Ptr,
	/** Pointer to an int32_t */
	kArgumentTypeInt32Ptr,
	/** Pointer to a uint32_t */
	kArgumentTypeUInt32Ptr,
	/** Pointer to an int64_t */
	kArgumentTypeInt64Ptr,
	/**Pointer to a uint64_t */
	kArgumentTypeUInt64Ptr,
	/** Pointer to a float */
	kArgumentTypeFloat32Ptr,
	/** Pointer to a double */
	kArgumentTypeFloat64Ptr,
	/** Pointer to a bool */
	kArgumentTypeBoolPtr,
	/** Pointer to a 64-bit SIMD structure */
	kArgumentTypeVector2Ptr,
	/** Pointer to a 128-bit SIMD structure, 4 floats only */
	kArgumentTypeVector4Ptr,
	/** Pointer to a 128-bit SIMD structure, 4 32-bit integers */
	kArgumentTypeVector4IntPtr,
	/** Pointer to a 128-bit SIMD structure, 2 doubles only */
	kArgumentTypeVector4DblPtr,
	/** void * for a catchall */
	kArgumentTypeVoidPtr,

	/** Auto list counter, all valid types must appear above this */
	kArgumentTypeCount,

	/** Used to indicate 'any valid arg' */
	kArgumentTypeAny,

	/** First integer entry */
	kArgumentTypeFirstInt = kArgumentTypeInt8,
	/** Last integer entry */
	kArgumentTypeLastInt = kArgumentTypeUInt64,

	/** First signed integer entry */
	kArgumentTypeFirstSigned = kArgumentTypeInt8,
	/** Last signed integer entry */
	kArgumentTypeLastSigned = kArgumentTypeInt64,

	/** First unsigned integer entry */
	kArgumentTypeFirstUnsigned = kArgumentTypeUInt8,
	/** Last unsigned integer entry */
	kArgumentTypeLastUnsigned = kArgumentTypeUInt64,

	/** First floating point entry */
	kArgumentTypeFirstFloat = kArgumentTypeFloat16,
	/** Last floating point entry */
	kArgumentTypeLastFloat = kArgumentTypeFloat64,

	/** First "C" string pointer entry */
	kArgumentTypeFirstCString = kArgumentTypeBurgerStringPtr,
	/** Last "C" string pointer entry */
	kArgumentTypeLastCString = kArgumentTypeUInt8Ptr,

	/** First pointer type entry */
	kArgumentTypeFirstPointer = kArgumentTypeBurgerStringPtr,
	/** Last pointer type entry */
	kArgumentTypeLastPointer = kArgumentTypeVoidPtr,

	/** First SIMD entry */
	kArgumentTypeFirstVector = kArgumentTypeVector2,
	/** Last SIMD entry */
	kArgumentTypeLastVector = kArgumentTypeVector4Dbl,

	/** First SIMD pointer type entry */
	kArgumentTypeFirstVectorPointer = kArgumentTypeVector2Ptr,
	/** Last SIMD pointer type entry */
	kArgumentTypeLastVectorPointer = kArgumentTypeVector4DblPtr
};

union ArgumentTypeDataUnion_t {
	/** Signed char value */
	int8_t m_iInt8;
	/** Unsigned char value */
	uint8_t m_uInt8;

	/** Signed short value */
	int16_t m_iInt16;
	/** Unsigned short value */
	uint16_t m_uInt16;

	/** Signed 32 bit integer value */
	int32_t m_iInt32;
	/** Unsigned 32 bit integer value */
	uint32_t m_uInt32;

	/** Signed 64 bit integer value */
	int64_t m_iInt64;
	/** Unsigned 64 bit integer value */
	uint64_t m_uInt64;

	/** 16 bit floating point value */
	uint16_t m_fHalf;
	/** 32 bit floating point value */
	float m_fFloat;
	/** 64 bit floating point value */
	double m_dDouble;

	/** 64 bit MMX register */
	uint64_t m_MMX; // Allow 32 bit alignment

	/** Boolean value */
	bool m_bBool;

	/** Pointer to a Burgerlib \ref String class */
	const String* m_pString;
	/** Generic char pointer for strings */
	const char* m_pChar;
	/** Pointer to a signed char */
	const int8_t* m_pInt8;
	/** Pointer to an unsigned char */
	const uint8_t* m_pUInt8;
	/** Pointer to a signed 16 bit integer */
	const int16_t* m_pInt16;
	/** Pointer to an unsigned 16 bit integer */
	const uint16_t* m_pUInt16;
	/** Pointer to a signed 32 bit integer */
	const int32_t* m_pInt32;
	/** Pointer to an unsigned 32 bit integer */
	const uint32_t* m_pUInt32;
	/** Pointer to a signed 64 bit integer */
	const int64_t* m_pInt64;
	/** Pointer to an unsigned 64 bit integer */
	const uint64_t* m_pUInt64;
	/** Pointer to bool */
	const bool* m_pBool;

	/** Pointer to a 32 bit floating point number */
	const float* m_pFloat;
	/** Pointer to a 64 bit floating point number */
	const double* m_pDouble;
	/** Pointer to generic data */
	const void* m_pVoid;
	/** Pointer to a 128 bit vector */
	const void* m_pVector;
};

struct ArgumentType_t {
	/** Data stored in the container */
	ArgumentTypeDataUnion_t m_Data;

	/** Type of object found */
	eArgumentType m_eType;
};

// Catchall for all unsupported types

template<typename T, uintptr_t size, bool bIsIntegral, bool bIsPointer>
struct ArgumentType_Dispatch {
	BURGER_INLINE void operator()(ArgumentType_t* pThis, T) BURGER_NOEXCEPT
	{
		// If this triggers, the data type is not supported yet.
		// BURGER_STATIC_ASSERT(bIsIntegral);
		// BURGER_STATIC_ASSERT(bIsPointer);
		pThis->m_Data.m_pVoid = nullptr;
		pThis->m_eType = kArgumentTypeVoidPtr;
	}
};

#if !defined(DOXYGEN)
// Handlers for all intrinsic integer types based on supported sizes
template<typename T>
struct ArgumentType_Dispatch<T, 1, true, false> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, T Input) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_uInt8 = static_cast<uint8_t>(Input);
		pThis->m_eType = select_value<is_signed<T>::value, eArgumentType,
			kArgumentTypeInt8, kArgumentTypeUInt8>::value;
	}
};

template<typename T>
struct ArgumentType_Dispatch<T, 2, true, false> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, T Input) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_uInt16 = static_cast<uint16_t>(Input);
		pThis->m_eType = select_value<is_signed<T>::value, eArgumentType,
			kArgumentTypeInt16, kArgumentTypeUInt16>::value;
	}
};

template<typename T>
struct ArgumentType_Dispatch<T, 4, true, false> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, T Input) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_uInt32 = static_cast<uint32_t>(Input);
		pThis->m_eType = select_value<is_signed<T>::value, eArgumentType,
			kArgumentTypeInt32, kArgumentTypeUInt32>::value;
	}
};

template<typename T>
struct ArgumentType_Dispatch<T, 8, true, false> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, T Input) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_uInt64 = static_cast<uint64_t>(Input);
		pThis->m_eType = select_value<is_signed<T>::value, eArgumentType,
			kArgumentTypeInt64, kArgumentTypeUInt64>::value;
	}
};

// Special cases for integrals and floats
template<>
struct ArgumentType_Dispatch<float, sizeof(float), false, false> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, float fInput) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_fFloat = fInput;
		pThis->m_eType = kArgumentTypeFloat32;
	}
};

template<>
struct ArgumentType_Dispatch<double, sizeof(double), false, false> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, double dInput) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_dDouble = dInput;
		pThis->m_eType = kArgumentTypeFloat64;
	}
};

template<>
struct ArgumentType_Dispatch<bool, sizeof(bool), true, false> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, bool bInput) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_bBool = bInput;
		pThis->m_eType = kArgumentTypeBool;
	}
};
#endif

// Handlers for all intrinsic integer types based on supported sizes using a
// pointer

// Catchall for all pointer types
template<typename T, uintptr_t size, bool bIsIntegral>
struct ArgumentType_DispatchPtr {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, T pInput) BURGER_NOEXCEPT
	{
		// If this is triggered, a pointer was passed to an unsupported data
		// type
		BURGER_STATIC_ASSERT(bIsIntegral);
		pThis->m_Data.m_pVoid = pInput;
		pThis->m_eType = kArgumentTypeVoidPtr;
	}
};

#if !defined(DOXYGEN)
template<typename T>
struct ArgumentType_DispatchPtr<T, 1, true> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, T pInput) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_pInt8 = (const int8_t*)pInput;
		pThis->m_eType =
			select_value<is_signed<typename remove_pointer<T>::type>::value,
				eArgumentType, kArgumentTypeInt8Ptr,
				kArgumentTypeUInt8Ptr>::value;
	}
};

template<typename T>
struct ArgumentType_DispatchPtr<T, 2, true> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, T pInput) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_pInt16 = (const int16_t*)pInput;
		pThis->m_eType =
			select_value<is_signed<typename remove_pointer<T>::type>::value,
				eArgumentType, kArgumentTypeInt16Ptr,
				kArgumentTypeUInt16Ptr>::value;
	}
};

template<typename T>
struct ArgumentType_DispatchPtr<T, 4, true> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, T pInput) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_pInt32 = (const int32_t*)pInput;
		pThis->m_eType =
			select_value<is_signed<typename remove_pointer<T>::type>::value,
				eArgumentType, kArgumentTypeInt32Ptr,
				kArgumentTypeUInt32Ptr>::value;
	}
};

template<typename T>
struct ArgumentType_DispatchPtr<T, 8, true> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, T pInput) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_pInt64 = (const int64_t*)pInput;
		pThis->m_eType =
			select_value<is_signed<typename remove_pointer<T>::type>::value,
				eArgumentType, kArgumentTypeInt64Ptr,
				kArgumentTypeUInt64Ptr>::value;
	}
};

// Special cases for integrals and floats
template<>
struct ArgumentType_DispatchPtr<const char*, sizeof(char), true> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, const char* pInput) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_pChar = pInput;
		pThis->m_eType = kArgumentTypeCharPtr;
	}
};

template<>
struct ArgumentType_DispatchPtr<const float*, sizeof(float), false> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, const float* pInput) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_pFloat = pInput;
		pThis->m_eType = kArgumentTypeFloat32Ptr;
	}
};

template<>
struct ArgumentType_DispatchPtr<const double*, sizeof(double), false> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, const double* pInput) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_pDouble = pInput;
		pThis->m_eType = kArgumentTypeFloat64Ptr;
	}
};

template<>
struct ArgumentType_DispatchPtr<const bool*, sizeof(bool), true> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, const bool* pInput) BURGER_NOEXCEPT
	{
		pThis->m_Data.m_pBool = pInput;
		pThis->m_eType = kArgumentTypeBoolPtr;
	}
};

// Forward to the pointer type handler
template<typename T>
struct ArgumentType_Dispatch<T, sizeof(void*), false, true> {
	BURGER_INLINE void operator()(
		ArgumentType_t* pThis, T pInput) BURGER_NOEXCEPT
	{
		// Use the underlying data type for proper dispatching
		typedef typename remove_pointer<T>::type real_type;

		// Use the remove_cv to convert all pointers to const T * for
		// dispatching so the only overrides needed are const char *, const
		// float *, etc
		ArgumentType_DispatchPtr<const typename remove_cv<real_type>::type*,
			sizeof(real_type), is_integral<real_type>::value>()(
			pThis, (const typename remove_cv<real_type>::type*)pInput);
	}
};
#endif

class ArgumentType: public ArgumentType_t {
public:
	/** Character returned from \ref GetChar() const for invalid character */
	static const char kInvalidCharConversion = '~';

public:
	// Generic data type constructors
	template<class T>
	BURGER_INLINE ArgumentType(T Input) BURGER_NOEXCEPT
	{
		ArgumentType_Dispatch<T, sizeof(T), is_integral<T>::value,
			is_pointer<T>::value>()(this, Input);
	}

	// Specialized constructors for all supported non integral types
	BURGER_INLINE ArgumentType(
		uint16_t fInput, eArgumentType uType) BURGER_NOEXCEPT
	{
		m_Data.m_fHalf = fInput;
		m_eType = uType;
	}

	// Special case for Burgerlib strings
	BURGER_INLINE ArgumentType(const String* pString) BURGER_NOEXCEPT
	{
		m_Data.m_pString = pString;
		m_eType = kArgumentTypeBurgerStringPtr;
	}
	BURGER_INLINE ArgumentType(const String& rString) BURGER_NOEXCEPT
	{
		m_Data.m_pString = &rString;
		m_eType = kArgumentTypeBurgerStringPtr;
	}

// MMX registers, these functions must be literal constructors because
// if they were done as templates, CodeWarrior for Intel will crash.
#if defined(BURGER_X86) || defined(DOXYGEN)
	BURGER_INLINE ArgumentType(__m64 Input) BURGER_NOEXCEPT
	{
		m_Data.m_MMX = ((const uint64_t*)&Input)[0];
		m_eType = kArgumentTypeVector2;
	}
	BURGER_INLINE ArgumentType(__m64* pInput) BURGER_NOEXCEPT
	{
		m_Data.m_pVector = pInput;
		m_eType = kArgumentTypeVector2Ptr;
	}
	BURGER_INLINE ArgumentType(const __m64* pInput) BURGER_NOEXCEPT
	{
		m_Data.m_pVector = pInput;
		m_eType = kArgumentTypeVector2Ptr;
	}
#endif

		// SSE registers
		// These functions pass by reference so the value stays in scope
#if (defined(BURGER_INTEL) && !defined(BURGER_WATCOM)) || defined(DOXYGEN)
	BURGER_INLINE ArgumentType(const __m128& rInput) BURGER_NOEXCEPT
	{
		m_Data.m_pVector = &rInput;
		m_eType = kArgumentTypeVector4;
	}
	BURGER_INLINE ArgumentType(const __m128d& rInput) BURGER_NOEXCEPT
	{
		m_Data.m_pVector = &rInput;
		m_eType = kArgumentTypeVector4Dbl;
	}
	BURGER_INLINE ArgumentType(const __m128i& rInput) BURGER_NOEXCEPT
	{
		m_Data.m_pVector = &rInput;
		m_eType = kArgumentTypeVector4Int;
	}

	BURGER_INLINE ArgumentType(__m128* pInput) BURGER_NOEXCEPT
	{
		m_Data.m_pVector = pInput;
		m_eType = kArgumentTypeVector4Ptr;
	}
	BURGER_INLINE ArgumentType(const __m128* pInput) BURGER_NOEXCEPT
	{
		m_Data.m_pVector = pInput;
		m_eType = kArgumentTypeVector4Ptr;
	}
	BURGER_INLINE ArgumentType(__m128d* pInput) BURGER_NOEXCEPT
	{
		m_Data.m_pVector = pInput;
		m_eType = kArgumentTypeVector4DblPtr;
	}
	BURGER_INLINE ArgumentType(const __m128d* pInput) BURGER_NOEXCEPT
	{
		m_Data.m_pVector = pInput;
		m_eType = kArgumentTypeVector4DblPtr;
	}
	BURGER_INLINE ArgumentType(__m128i* pInput) BURGER_NOEXCEPT
	{
		m_Data.m_pVector = pInput;
		m_eType = kArgumentTypeVector4IntPtr;
	}
	BURGER_INLINE ArgumentType(const __m128i* pInput) BURGER_NOEXCEPT
	{
		m_Data.m_pVector = pInput;
		m_eType = kArgumentTypeVector4IntPtr;
	}
#endif

	// void* has to last because the above list of pointers will be checked
	// first, and void* will catch implicit conversion
	BURGER_INLINE ArgumentType(void* pInput) BURGER_NOEXCEPT
	{
		m_Data.m_pVoid = pInput;
		m_eType = kArgumentTypeVoidPtr;
	};
	BURGER_INLINE ArgumentType(const void* pInput) BURGER_NOEXCEPT
	{
		m_Data.m_pVoid = pInput;
		m_eType = kArgumentTypeVoidPtr;
	};

	BURGER_INLINE uint_t IsNumeric(void) const BURGER_NOEXCEPT
	{
		return (m_eType >= kArgumentTypeInt8) && (m_eType <= kArgumentTypeBool);
	}
	BURGER_INLINE uint_t IsInteger(void) const BURGER_NOEXCEPT
	{
		return (m_eType >= kArgumentTypeFirstInt) &&
			(m_eType <= kArgumentTypeLastInt);
	}
	BURGER_INLINE uint_t IsSigned(void) const BURGER_NOEXCEPT
	{
		return (m_eType >= kArgumentTypeFirstSigned) &&
			(m_eType <= kArgumentTypeLastSigned);
	}
	BURGER_INLINE uint_t IsUnsigned(void) const BURGER_NOEXCEPT
	{
		return (m_eType >= kArgumentTypeFirstUnsigned) &&
			(m_eType <= kArgumentTypeLastUnsigned);
	}
	BURGER_INLINE uint_t IsBoolean(void) const BURGER_NOEXCEPT
	{
		return (m_eType == kArgumentTypeBool) ||
			(m_eType == kArgumentTypeBoolPtr);
	}
	BURGER_INLINE uint_t IsReal(void) const BURGER_NOEXCEPT
	{
		return (m_eType >= kArgumentTypeFirstFloat) &&
			(m_eType <= kArgumentTypeLastFloat);
	}

	uint_t BURGER_API IsNegative(void) const BURGER_NOEXCEPT;
	uint_t BURGER_API IsZero(void) const BURGER_NOEXCEPT;
	BURGER_INLINE uint_t IsCharacter(void) const BURGER_NOEXCEPT
	{
		return (m_eType >= kArgumentTypeInt8) &&
			(m_eType <= kArgumentTypeUInt32) && (m_eType != kArgumentTypeInt64);
	}
	BURGER_INLINE uint_t IsTextPointer(void) const BURGER_NOEXCEPT
	{
		return ((m_eType >= kArgumentTypeFirstCString) &&
				   (m_eType <= kArgumentTypeLastCString)) ||
			(m_eType == kArgumentTypeUInt16Ptr);
	}
	BURGER_INLINE uint_t IsPointer(void) const BURGER_NOEXCEPT
	{
		return (m_eType >= kArgumentTypeFirstPointer) &&
			(m_eType <= kArgumentTypeLastPointer);
	}
	BURGER_INLINE uint_t Is64Bit(void) const BURGER_NOEXCEPT
	{
		return GetDataLengthInBytes() == 8;
	}
	BURGER_INLINE uint_t IsSIMD(void) const BURGER_NOEXCEPT
	{
		return (m_eType >= kArgumentTypeFirstVector) &&
			(m_eType <= kArgumentTypeLastVector);
	}
	BURGER_INLINE uint_t IsSIMDPointer(void) const BURGER_NOEXCEPT
	{
		return (m_eType >= kArgumentTypeFirstVectorPointer) &&
			(m_eType <= kArgumentTypeLastVectorPointer);
	}
	uintptr_t BURGER_API GetDataLengthInBytes(void) const BURGER_NOEXCEPT;
	const void* BURGER_API GetDataAddress(void) const BURGER_NOEXCEPT;
	BURGER_INLINE eArgumentType GetType(void) const BURGER_NOEXCEPT
	{
		return m_eType;
	}
	BURGER_INLINE void SetType(eArgumentType uType) BURGER_NOEXCEPT
	{
		m_eType = uType;
	}
	static const char* BURGER_API GetTypeName(
		eArgumentType uType) BURGER_NOEXCEPT;
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
	uintptr_t BURGER_API GetUTF8(char* pOutput) const BURGER_NOEXCEPT;
};
}
/* END */
#endif
