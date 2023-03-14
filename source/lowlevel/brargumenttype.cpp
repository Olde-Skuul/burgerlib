/***************************************

	Typed Argument class

	Copyright (c) 2022-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brargumenttype.h"
#include "brassert.h"
#include "brfixedpoint.h"
#include "brfloatingpoint.h"
#include "brmemoryfunctions.h"
#include "brnumberto.h"
#include "brpackfloat.h"
#include "brstring.h"
#include "brutf8.h"

/*! ************************************

	\enum Burger::eArgumentType
	\brief Type stored in this object.

	When a \ref ArgumentType object is created, it contains intrinsic data of a
	specific type. This enumeration lists the type it could be.

***************************************/

/*! ************************************

	\union Burger::ArgumentTypeDataUnion_t
	\brief Data stored in this object.

	Based on the type reported by the class member m_eType will determine which
	member of this union is active.

***************************************/

/*! ************************************

	\union Burger::ArgumentType_t
	\brief Base structure for ArgumentType

	Data enclosed in the class ArgumentType. It was necessary to expose it as a
	structure to allow ArgumentType_DispatchPtr and ArgumentType_Dispatch access
	to the data within without creating a circular dependency.

	\note The original implementation had the templates ArgumentType_Dispatch
		and ArgumentType_DispatchPtr contained within ArgumentType but some
		compilers such as GNUC 4.2 and CodeWarior complained.

***************************************/

/*! ************************************

	\struct Burger::ArgumentType_Dispatch
	\brief Intrinsic data type dispatcher

	This template requires the type to be pre-broken down into three components,
	sizeof(), \ref is_integral and \ref is_pointer. A series of templates will
	capture intrinsic data of 1, 2, 4, or 8 bytes in size and mark the type
	appropriately. Special cases exist for float, double, bool and pointers.

	In the case of a pointer, it will be subsequently dispatched to
	ArgumentType_DispatchPtr for further processing.

	\sa ArgumentType_DispatchPtr and ArgumentType

***************************************/

/*! ************************************

	\fn Burger::ArgumentType_Dispatch::operator()(ArgumentType_t*, T)
	\brief Intrinsic data type dispatchers

	This function will use the input type ``T`` to check if it's signed or
	unsigned. The template parameters will predetermine the width of the integer
	value so only whether it's signed or unsigned is the only test performed at
	compile time.

	\note T is assumed to be an intrinsic type. Pointers and references are not
		permitted.

	\param pThis Pointer to the ArgumentType_t structure to fill in.
	\tparam T Intrinsic value to be stored in ArgumentType_t

	\sa ArgumentType_Dispatch and ArgumentType

***************************************/

/*! ************************************

	\struct Burger::ArgumentType_DispatchPtr
	\brief Intrinsic data type dispatcher

	This template is used when a pointer to an intrinsic needs to be parsed. It
	requires the type to be pre-broken down into two components, sizeof() and,
	\ref is_integral. A series of templates will capture intrinsic data of 1, 2,
	4, or 8 bytes in size and mark the type appropriately. Special cases exist
	for float, double, bool and char.

	Pointers are upcast to const * before the template is invoked to eliminate
	the need to create templates for the const, volatile pointer variants.

	\sa ArgumentType_Dispatch and ArgumentType

***************************************/

/*! ************************************

	\fn Burger::ArgumentType_DispatchPtr::operator()(ArgumentType_t*, T)
	\brief Intrinsic data type dispatchers

	This function will use the input pointer type ``T`` to check if the data
	it's pointing to is signed or unsigned. The template parameters will
	predetermine the width of the integer value so only whether it's signed or
	unsigned is the only test performed at compile time.

	\note T is assumed to be a pointer type. Intrinsics and references are not
		permitted.

	\param pThis Pointer to the ArgumentType_t structure to fill in.
	\tparam T Pointer to an intrinsic value to be stored in ArgumentType_t
	\param pInput Pointer to the data

	\sa ArgumentType_DispatchPtr and ArgumentType

***************************************/

/*! ************************************

	\class Burger::ArgumentType
	\brief Container class for passing data to SafePrint

	Instead of relying on ... for sending parameters to a function, use
	instances of this type exclusively so that all passed types are known at
	runtime so robust error checking can be performed.

	It's currently used by the SafePrint class to ensure the passed parameters
	match the data types requested by the format string.

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(T Input)
	\brief Constructor most data types.

	This template will invoke ArgumentType_Dispatch for processing. If it's a
	pointer, ArgumentType_DispatchPtr will subsequently be invoked to properly
	handle type determination.

	\tparam T Data type to be processed
	\param Input Data to be stored in this class.

	\sa ArgumentType_Dispatch or ArgumentType_DispatchPtr

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(uint16_t fInput, eArgumentType uType)
	\brief Constructor for a 16 bit float

	Since 16 bit floats are not native to C++, it has to be manually cast by
	creating an argument manually. Example is below.

	\code
	SafePrintf("Float %hf\n",ArgumentType(
		fHalf,ArgumentType::kTypeFloat16));
	\endcode

	\param fInput uint16_t representation of a 16 bit float
	\param uType Type override for the 16 bit short

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(const String *pString)
	\brief Constructor for a pointer to a Burgerlib \ref String

	\param pString Pointer to a Burgerlib \ref String

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(const String &rString)
	\brief Constructor for a reference to a Burgerlib \ref String

	\param rString Reference to a Burgerlib \ref String

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(__m64 Input)
	\brief Constructor for an MMX 64 bit value

	\param Input A __m64 MMX 64 bit value

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(__m64 *pInput)
	\brief Constructor for a pointer to a MMX 64 bit value

	\note Only available on x86 processors

	\param pInput Pointer to a __m64 MMX 64 bit value

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(const __m64 *pInput)
	\brief Constructor for a pointer to a MMX 64 bit value

	\note Only available on x86 processors

	\param pInput Pointer to a __m64 MMX 64 bit value

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(const __m128 &rInput)
	\brief Constructor for an SSE 128 bit value (4 floats)

	\note Only available on x86 or x64 processors

	\param rInput A __m128 SSE 128 bit reference

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(const __m128d &rInput)
	\brief Constructor for an SSE 128 bit value (2 doubles)

	\note Only available on x86 or x64 processors

	\param rInput A __m128d SSE 128 bit reference

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(const __m128i &rInput)
	\brief Constructor for an SSE 128 bit value (4 integers)

	\note Only available on x86 or x64 processors

	\param rInput A __m128i SSE 128 bit reference

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(__m128 *pInput)
	\brief Constructor for a pointer to a SSE 128 bit value (4 floats)

	\note Only available on x86 or x64 processors

	\param pInput Pointer to a __m128 SSE 128 bit value

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(const __m128 *pInput)
	\brief Constructor for a pointer to a SSE 128 bit value (4 floats)

	\note Only available on x86 or x64 processors

	\param pInput Pointer to a __m128 SSE 128 bit value

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(__m128d *pInput)
	\brief Constructor for a pointer to a SSE 128 bit value (2 doubles

	\note Only available on x86 or x64 processors

	\param pInput Pointer to a __m128d SSE 128 bit value

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(const __m128d *pInput)
	\brief Constructor for a pointer to a SSE 128 bit value (2 doubles

	\note Only available on x86 or x64 processors

	\param pInput Pointer to a __m128d SSE 128 bit value

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(__m128i *pInput)
	\brief Constructor for a pointer to a SSE 128 bit value (4 integers)

	\note Only available on x86 or x64 processors

	\param pInput Pointer to a __m128 SSE 128 bit value

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(const __m128i *pInput)
	\brief Constructor for a pointer to a SSE 128 bit value (4 integers)

	\note Only available on x86 or x64 processors

	\param pInput Pointer to a __m128 SSE 128 bit value

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(void *pInput)
	\brief Constructor for a pointer

	\param pInput Pointer to data that wasn't already captured by a specific
		constructor

***************************************/

/*! ************************************

	\fn Burger::ArgumentType::ArgumentType(const void *pInput)
	\brief Constructor for a pointer

	\param pInput Pointer to data that wasn't already captured by a specific
		constructor

***************************************/

/*! ************************************

	\fn uint_t Burger::ArgumentType::IsNumeric(void) const
	\brief Test if the data is a numeric value

	If the value is a number, return \ref TRUE. This includes
	floating point, bool, and integers.

	\return \ref TRUE if the value contained is a number.

***************************************/

/*! ************************************

	\fn uint_t Burger::ArgumentType::IsInteger(void) const
	\brief Test if the data is an integer numeric value

	If the value is an integer number, return \ref TRUE. It will
	return \ref FALSE if it's floating point or bool.

	\return \ref TRUE if the value contained is an integer.

***************************************/

/*! ************************************

	\fn uint_t Burger::ArgumentType::IsSigned(void) const
	\brief Test if the data is a signed integer value

	If the value is a signed integer, return \ref TRUE. It will
	return \ref FALSE if it's an unsigned integer or bool.

	\return \ref TRUE if the value contained supported signed values.

***************************************/

/*! ************************************

	\fn uint_t Burger::ArgumentType::IsUnsigned(void) const
	\brief Test if the data is an unsigned integer value

	If the value is an unsigned integer, return \ref TRUE. It will
	return \ref FALSE if it's signed, bool, or floating point.

	\return \ref TRUE if the value contained is an unsigned data type.

***************************************/

/*! ************************************

	\fn uint_t Burger::ArgumentType::IsBoolean(void) const
	\brief Test if the data is a boolean

	If the value is a boolean, or a pointer to a boolean, return \ref TRUE. It
	will return \ref FALSE if it's anything else.

	\return \ref TRUE if the value contained is a boolean.

***************************************/

/*! ************************************

	\fn uint_t Burger::ArgumentType::IsReal(void) const
	\brief Test if the data is a floating point value

	If the value is floating pointer, return \ref TRUE. It will
	return \ref FALSE if it's anything else.

	\return \ref TRUE if the value contained is a floating point value.

***************************************/

/*! ************************************

	\brief Returns \ref TRUE if the stored value is negative.

	If the value is negative, return \ref TRUE, if the data type has no concept
	of negative, it will always return \ref FALSE. bool and vectors always
	return \ref FALSE.

	\return \ref TRUE if negative

***************************************/

uint_t BURGER_API Burger::ArgumentType::IsNegative(void) const BURGER_NOEXCEPT
{
	uint_t bResult;
	switch (m_eType) {
	case kArgumentTypeInt8:
		bResult = (m_Data.m_iInt8 < 0);
		break;
	case kArgumentTypeInt16:
		bResult = (m_Data.m_iInt16 < 0);
		break;
	case kArgumentTypeInt32:
		bResult = (m_Data.m_iInt32 < 0);
		break;
	case kArgumentTypeInt64:
		bResult = (m_Data.m_iInt64 < 0);
		break;
	case kArgumentTypeFloat16:
		// Handle 16 bit floats in a cross platform way
		bResult = (m_Data.m_fHalf & 0x8000U) >> 15U;
		break;
	case kArgumentTypeFloat32:
		bResult = (m_Data.m_fFloat < 0.0f);
		break;
	case kArgumentTypeFloat64:
		bResult = (m_Data.m_dDouble < 0.0);
		break;
	default:
		// TILT!
		bResult = FALSE;
		break;
	}
	return bResult;
}

/*! ************************************

	\brief Returns \ref TRUE if the stored value is zero.

	If the value is zero, return \ref TRUE, if the data type has no concept of
	zero, it will always return \ref FALSE.

	\return \ref TRUE if zero

***************************************/

uint_t BURGER_API Burger::ArgumentType::IsZero(void) const BURGER_NOEXCEPT
{
	uint_t bResult;
	switch (m_eType) {
	case kArgumentTypeInt8:
	case kArgumentTypeUInt8:
		bResult = !m_Data.m_uInt8;
		break;
	case kArgumentTypeInt16:
	case kArgumentTypeUInt16:
		bResult = !m_Data.m_uInt16;
		break;
	case kArgumentTypeInt32:
	case kArgumentTypeUInt32:
		bResult = !m_Data.m_uInt32;
		break;
	case kArgumentTypeInt64:
	case kArgumentTypeUInt64:
		bResult = !m_Data.m_uInt64;
		break;
	case kArgumentTypeFloat16:
		// Allow +0 and -0 to be zero
		bResult = !(m_Data.m_fHalf & 0x7FFFU);
		break;
	case kArgumentTypeFloat32:
		bResult = m_Data.m_fFloat == 0.0f;
		break;
	case kArgumentTypeFloat64:
		bResult = m_Data.m_dDouble == 0.0;
		break;
	case kArgumentTypeBool:
		bResult = !m_Data.m_bBool;
		break;
	default:
		// TILT!
		bResult = FALSE;
		break;
	}
	return bResult;
}

/*! ************************************

	\fn uint_t Burger::ArgumentType::IsCharacter(void) const
	\brief Test if the data is a character type

	If the value is single character, return \ref TRUE. It will return
	\ref FALSE if it's anything else.

	\note Characters are integers that are 8, 16, or 32 bits in size. 64 bit
		integers are not considered characters.

	\return \ref TRUE if the value contained is a character value.

***************************************/

/*! ************************************

	\fn uint_t Burger::ArgumentType::IsTextPointer(void) const
	\brief Test if the data is a pointer to a "C" string

	If the value is a pointer to a "C" string, return \ref TRUE. It will
	return \ref FALSE if it's anything else.

	\return \ref TRUE if the value contained is a pointer to a "C" string.

***************************************/

/*! ************************************

	\fn uint_t Burger::ArgumentType::IsPointer(void) const
	\brief Test if the data is a pointer

	If the value is a pointer, return \ref TRUE. It will return \ref FALSE if
	it's anything else.

	\return \ref TRUE if the value contained is a pointer.

***************************************/

/*! ************************************

	\fn uint_t Burger::ArgumentType::Is64Bit(void) const
	\brief Test if the data is a 64 bits wide

	If the value is 64 bits wide, return \ref TRUE. It will return \ref FALSE if
	it's anything else.

	\return \ref TRUE if the value contained is 64 bits wide.

***************************************/

/*! ************************************

	\fn uint_t Burger::ArgumentType::IsSIMD(void) const
	\brief Test if the data is a vector value

	If the value is a vector, return \ref TRUE. It will return \ref FALSE if
	it's anything else.

	\return \ref TRUE if the value contained is a vector.

***************************************/

/*! ************************************

	\fn uint_t Burger::ArgumentType::IsSIMDPointer(void) const
	\brief Test if the data is a pointer to a vector value

	If the value is a pointer to a vector, return \ref TRUE. It will return
	\ref FALSE if it's anything else.

	\return \ref TRUE if the value contained is a pointer to a vector.

***************************************/

/*! ************************************

	\brief Returns the size of the stored data in bytes

	\return Number of bytes the data occupies, zero if undefined

***************************************/

uintptr_t BURGER_API Burger::ArgumentType::GetDataLengthInBytes(
	void) const BURGER_NOEXCEPT
{
	uintptr_t uResult;
	switch (m_eType) {
	case kArgumentTypeInt8:
	case kArgumentTypeUInt8:
		uResult = 1;
		break;

	case kArgumentTypeInt16:
	case kArgumentTypeUInt16:
	case kArgumentTypeFloat16:
		uResult = 2;
		break;

	case kArgumentTypeInt32:
	case kArgumentTypeUInt32:
	case kArgumentTypeFloat32:
		uResult = 4;
		break;

	case kArgumentTypeInt64:
	case kArgumentTypeUInt64:
	case kArgumentTypeFloat64:
	case kArgumentTypeVector2:
		uResult = 8;
		break;

	case kArgumentTypeBool:
		// Varies with each compiler
		uResult = sizeof(bool);
		break;
	case kArgumentTypeBurgerStringPtr:
	case kArgumentTypeCharPtr:
	case kArgumentTypeInt8Ptr:
	case kArgumentTypeUInt8Ptr:
	case kArgumentTypeInt16Ptr:
	case kArgumentTypeUInt16Ptr:
	case kArgumentTypeInt32Ptr:
	case kArgumentTypeUInt32Ptr:
	case kArgumentTypeInt64Ptr:
	case kArgumentTypeUInt64Ptr:
	case kArgumentTypeFloat32Ptr:
	case kArgumentTypeFloat64Ptr:
	case kArgumentTypeBoolPtr:
	case kArgumentTypeVector2Ptr:
	case kArgumentTypeVector4Ptr:
	case kArgumentTypeVector4DblPtr:
	case kArgumentTypeVector4IntPtr:
	case kArgumentTypeVoidPtr:
		// Can vary with each compiler
		uResult = sizeof(void*);
		break;

	case kArgumentTypeVector4:
	case kArgumentTypeVector4Dbl:
	case kArgumentTypeVector4Int:
		uResult = 16;
		break;

	default:
		BURGER_ASSERT(FALSE);
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Returns the pointer the stored data

	\return Number of bytes the data occupies, zero if undefined

***************************************/

const void* BURGER_API Burger::ArgumentType::GetDataAddress(
	void) const BURGER_NOEXCEPT
{
	// Due to the fact the some compilers can move data around in
	// a union, this function is needed instead of returning
	// the base pointer of m_Data

	const void* pResult;
	switch (m_eType) {
	case kArgumentTypeInt8:
	case kArgumentTypeUInt8:
		pResult = &m_Data.m_iInt8;
		break;
	case kArgumentTypeInt16:
	case kArgumentTypeUInt16:
		pResult = &m_Data.m_iInt16;
		break;
	case kArgumentTypeInt32:
	case kArgumentTypeUInt32:
		pResult = &m_Data.m_iInt32;
		break;
	case kArgumentTypeInt64:
	case kArgumentTypeUInt64:
		pResult = &m_Data.m_iInt64;
		break;

	case kArgumentTypeFloat16:
		pResult = &m_Data.m_fHalf;
		break;
	case kArgumentTypeFloat32:
		pResult = &m_Data.m_fFloat;
		break;
	case kArgumentTypeFloat64:
		pResult = &m_Data.m_dDouble;
		break;
	case kArgumentTypeBool:
		pResult = &m_Data.m_bBool;
		break;
	case kArgumentTypeVector2:
		pResult = &m_Data.m_MMX;
		break;

	case kArgumentTypeCharPtr:
		pResult = m_Data.m_pChar;
		break;
	case kArgumentTypeInt8Ptr:
		pResult = m_Data.m_pInt8;
		break;
	case kArgumentTypeUInt8Ptr:
		pResult = m_Data.m_pUInt8;
		break;
	case kArgumentTypeInt16Ptr:
		pResult = m_Data.m_pInt16;
		break;
	case kArgumentTypeUInt16Ptr:
		pResult = m_Data.m_pUInt16;
		break;
	case kArgumentTypeInt32Ptr:
		pResult = m_Data.m_pInt32;
		break;
	case kArgumentTypeUInt32Ptr:
		pResult = m_Data.m_pUInt32;
		break;
	case kArgumentTypeInt64Ptr:
		pResult = m_Data.m_pInt64;
		break;
	case kArgumentTypeUInt64Ptr:
		pResult = m_Data.m_pUInt64;
		break;
	case kArgumentTypeFloat32Ptr:
		pResult = m_Data.m_pFloat;
		break;
	case kArgumentTypeFloat64Ptr:
		pResult = m_Data.m_pDouble;
		break;
	case kArgumentTypeBoolPtr:
		pResult = m_Data.m_pBool;
		break;
	case kArgumentTypeVoidPtr:
		pResult = m_Data.m_pVoid;
		break;

		// SIMD types are only pointed to, not stored
	case kArgumentTypeVector4:
	case kArgumentTypeVector4Dbl:
	case kArgumentTypeVector4Int:
	case kArgumentTypeVector2Ptr:
	case kArgumentTypeVector4Ptr:
	case kArgumentTypeVector4DblPtr:
	case kArgumentTypeVector4IntPtr:
		pResult = m_Data.m_pVector;
		break;
	default:
		BURGER_ASSERT(FALSE);
		pResult = &m_Data;
		break;
	}
	return pResult;
}

/*! ************************************

	\fn eType Burger::ArgumentType::GetType(void) const
	\brief Return the type of data contained in the container.

	\return \ref eArgumentType of the data contained in the class.

***************************************/

/*! ************************************

	\fn void Burger::ArgumentType::SetType(eType uType)
	\brief Set the type of data contained in the container.

	\param uType \ref eArgumentType of the new data contained in the class.

***************************************/

/*! ************************************

	\brief Returns the name of the data type.

	Return a "C" string containing a user readable name of the data type.

	Example: int8_t data, with an enumeration of \ref kArgumentTypeInt8 returns
	"int8_t"

	\param uType Enumeration of the data type to check
	\return Pointer to a "C" string with the name.

***************************************/

const char* BURGER_API Burger::ArgumentType::GetTypeName(
	eArgumentType uType) BURGER_NOEXCEPT
{
	const char* pResult;
	switch (uType) {
	case kArgumentTypeInt8:
		pResult = "int8_t";
		break;
	case kArgumentTypeInt16:
		pResult = "int16_t";
		break;
	case kArgumentTypeInt32:
		pResult = "int32_t";
		break;
	case kArgumentTypeInt64:
		pResult = "int64_t";
		break;
	case kArgumentTypeUInt8:
		pResult = "uint8_t";
		break;
	case kArgumentTypeUInt16:
		pResult = "uint16_t";
		break;
	case kArgumentTypeUInt32:
		pResult = "uint32_t";
		break;
	case kArgumentTypeUInt64:
		pResult = "uint64_t";
		break;
	case kArgumentTypeFloat16:
		pResult = "half";
		break;
	case kArgumentTypeFloat32:
		pResult = "float";
		break;
	case kArgumentTypeFloat64:
		pResult = "double";
		break;
	case kArgumentTypeBool:
		pResult = "bool";
		break;
	case kArgumentTypeBurgerStringPtr:
		pResult = "String ";
		break;
	case kArgumentTypeCharPtr:
		pResult = "char *";
		break;
	case kArgumentTypeInt8Ptr:
		pResult = "int8_t *";
		break;
	case kArgumentTypeUInt8Ptr:
		pResult = "uint8_t *";
		break;
	case kArgumentTypeInt16Ptr:
		pResult = "int16_t *";
		break;
	case kArgumentTypeUInt16Ptr:
		pResult = "uint16_t *";
		break;
	case kArgumentTypeInt32Ptr:
		pResult = "int32_t *";
		break;
	case kArgumentTypeUInt32Ptr:
		pResult = "uint32_t *";
		break;
	case kArgumentTypeInt64Ptr:
		pResult = "int64_t *";
		break;
	case kArgumentTypeUInt64Ptr:
		pResult = "uint64_t *";
		break;
	case kArgumentTypeFloat32Ptr:
		pResult = "float *";
		break;
	case kArgumentTypeFloat64Ptr:
		pResult = "double *";
		break;
	case kArgumentTypeBoolPtr:
		pResult = "bool *";
		break;
	case kArgumentTypeVoidPtr:
		pResult = "void *";
		break;
	case kArgumentTypeVector2:
		pResult = "__m64";
		break;
	case kArgumentTypeVector4:
		pResult = "__m128";
		break;
	case kArgumentTypeVector4Dbl:
		pResult = "__m128d";
		break;
	case kArgumentTypeVector4Int:
		pResult = "__m128i";
		break;
	case kArgumentTypeVector2Ptr:
		pResult = "__m64 *";
		break;
	case kArgumentTypeVector4Ptr:
		pResult = "__m128 *";
		break;
	case kArgumentTypeVector4DblPtr:
		pResult = "__m128d *";
		break;
	case kArgumentTypeVector4IntPtr:
		pResult = "__m128i *";
		break;
	default:
		BURGER_ASSERT(FALSE);
		pResult = "<unknown>";
		break;
	}
	return pResult;
}

/*! ************************************

	\fn const char* Burger::ArgumentType::GetTypeName(void) const
	\brief Returns the name of the data type contained.

	Return a "C" string containing a user readable name of the data type
	contained in this class.

	Example: int8_t data, with an enumeration of \ref kArgumentTypeInt8 returns
	"int8_t"

	\return Pointer to a "C" string with the name.

***************************************/

/*! ************************************

	\brief Returns the value as a signed 32 bit integer

	If the value is a signed integer, convert to a 32 bit signed integer and
	return the converted value.

	If it's not of this type, return 0.

	\note If the contained data is 64 bits, it will be truncated.

	\return Signed value or 0 if not compatible.

***************************************/

int32_t BURGER_API Burger::ArgumentType::GetInt32(void) const BURGER_NOEXCEPT
{
	int32_t iResult;
	switch (m_eType) {
	case kArgumentTypeInt8:
		iResult = m_Data.m_iInt8;
		break;
	case kArgumentTypeInt16:
		iResult = m_Data.m_iInt16;
		break;
	case kArgumentTypeInt32:
		iResult = m_Data.m_iInt32;
		break;
	case kArgumentTypeInt64:
		iResult = static_cast<int32_t>(m_Data.m_iInt64);
		break;

	case kArgumentTypeUInt8:
		iResult = static_cast<int32_t>(m_Data.m_uInt8);
		break;
	case kArgumentTypeUInt16:
		iResult = static_cast<int32_t>(m_Data.m_uInt16);
		break;
	case kArgumentTypeUInt32:
		iResult = static_cast<int32_t>(m_Data.m_uInt32);
		break;
	case kArgumentTypeUInt64:
		iResult = static_cast<int32_t>(m_Data.m_uInt64);
		break;

	case kArgumentTypeFloat16:
		iResult = static_cast<int32_t>(
			Unpack16ToFloat(static_cast<int16_t>(m_Data.m_fHalf)));
		break;
	case kArgumentTypeFloat32:
		iResult = static_cast<int32_t>(m_Data.m_fFloat);
		break;
	case kArgumentTypeFloat64:
		iResult = static_cast<int32_t>(m_Data.m_dDouble);
		break;

	case kArgumentTypeBool:
		iResult = static_cast<int32_t>(m_Data.m_bBool);
		break;
	default:
		iResult = 0;
		break;
	}
	return iResult;
}

/*! ************************************

	\brief Returns the value as an unsigned 32 bit integer

	If the value is an unsigned integer, convert to
	a 32 bit unsigned integer and return the converted value.

	If it's not of this type, return 0.

	\return Unsigned value or 0 if not compatible.

***************************************/

uint32_t BURGER_API Burger::ArgumentType::GetUInt32(void) const BURGER_NOEXCEPT
{
	uint32_t uResult;
	switch (m_eType) {
	case kArgumentTypeUInt8:
		uResult = m_Data.m_uInt8;
		break;
	case kArgumentTypeUInt16:
		uResult = m_Data.m_uInt16;
		break;
	case kArgumentTypeUInt32:
		uResult = m_Data.m_uInt32;
		break;
	case kArgumentTypeUInt64:
		uResult = static_cast<uint32_t>(m_Data.m_uInt64);
		break;

		// Return the absolute form of signed numbers

	case kArgumentTypeInt8:
		uResult =
			static_cast<uint32_t>(Abs(static_cast<int32_t>(m_Data.m_iInt8)));
		break;
	case kArgumentTypeInt16:
		uResult =
			static_cast<uint32_t>(Abs(static_cast<int32_t>(m_Data.m_iInt16)));
		break;
	case kArgumentTypeInt32:
		uResult = static_cast<uint32_t>(Abs(m_Data.m_iInt32));
		break;
	case kArgumentTypeInt64:
		uResult = static_cast<uint32_t>(Abs(m_Data.m_iInt64));
		break;

	case kArgumentTypeFloat16:
		uResult = static_cast<uint32_t>(
			Unpack16ToFloat(static_cast<int16_t>(m_Data.m_fHalf & 0x7FFFU)));
		break;
	case kArgumentTypeFloat32:
		uResult = static_cast<uint32_t>(Abs(m_Data.m_fFloat));
		break;
	case kArgumentTypeFloat64:
		uResult = static_cast<uint32_t>(Abs(m_Data.m_dDouble));
		break;

	case kArgumentTypeBool:
		uResult = static_cast<uint32_t>(m_Data.m_bBool);
		break;
	default:
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Returns the value as a signed 64 bit integer

	If the value is a signed integer, convert to a 64 bit signed integer and
	return the converted value.

	If it's not of this type, return 0.

	\return Signed value or 0 if not compatible.

***************************************/

int64_t BURGER_API Burger::ArgumentType::GetInt64(void) const BURGER_NOEXCEPT
{
	int64_t iResult;
	switch (m_eType) {
	case kArgumentTypeInt8:
		iResult = m_Data.m_iInt8;
		break;
	case kArgumentTypeInt16:
		iResult = m_Data.m_iInt16;
		break;
	case kArgumentTypeInt32:
		iResult = m_Data.m_iInt32;
		break;
	case kArgumentTypeInt64:
		iResult = m_Data.m_iInt64;
		break;

	case kArgumentTypeUInt8:
		iResult = static_cast<int64_t>(m_Data.m_uInt8);
		break;
	case kArgumentTypeUInt16:
		iResult = static_cast<int64_t>(m_Data.m_uInt16);
		break;
	case kArgumentTypeUInt32:
		iResult = static_cast<int64_t>(m_Data.m_uInt32);
		break;
	case kArgumentTypeUInt64:
		iResult = static_cast<int64_t>(m_Data.m_uInt64);
		break;

	case kArgumentTypeFloat16:
		iResult = static_cast<int64_t>(
			Unpack16ToFloat(static_cast<int16_t>(m_Data.m_fHalf)));
		break;
	case kArgumentTypeFloat32:
		iResult = static_cast<int64_t>(m_Data.m_fFloat);
		break;
	case kArgumentTypeFloat64:
		iResult = static_cast<int64_t>(m_Data.m_dDouble);
		break;

	case kArgumentTypeBool:
		iResult = static_cast<int64_t>(m_Data.m_bBool);
		break;
	default:
		iResult = 0;
		break;
	}
	return iResult;
}

/*! ************************************

	\brief Returns the value as an unsigned 64 bit integer

	If the value is an unsigned integer, convert to a 64 bit unsigned integer
	and return the converted value.

	If it's not of this type, return 0.

	\return Unsigned value or 0 if not compatible.

***************************************/

uint64_t BURGER_API Burger::ArgumentType::GetUInt64(void) const BURGER_NOEXCEPT
{
	uint64_t uResult;
	switch (m_eType) {
	case kArgumentTypeUInt8:
		uResult = m_Data.m_uInt8;
		break;
	case kArgumentTypeUInt16:
		uResult = m_Data.m_uInt16;
		break;
	case kArgumentTypeUInt32:
		uResult = m_Data.m_uInt32;
		break;
	case kArgumentTypeUInt64:
		uResult = m_Data.m_uInt64;
		break;

		// Return the absolute form of signed numbers

	case kArgumentTypeInt8:
		uResult =
			static_cast<uint64_t>(Abs(static_cast<int32_t>(m_Data.m_iInt8)));
		break;
	case kArgumentTypeInt16:
		uResult =
			static_cast<uint64_t>(Abs(static_cast<int32_t>(m_Data.m_iInt16)));
		break;
	case kArgumentTypeInt32:
		uResult = static_cast<uint64_t>(Abs(m_Data.m_iInt32));
		break;
	case kArgumentTypeInt64:
		uResult = static_cast<uint64_t>(Abs(m_Data.m_iInt64));
		break;

	case kArgumentTypeFloat16:
		uResult = static_cast<uint64_t>(
			Unpack16ToFloat(static_cast<int16_t>(m_Data.m_fHalf & 0x7FFFU)));
		break;
	case kArgumentTypeFloat32:
		uResult = static_cast<uint64_t>(Abs(m_Data.m_fFloat));
		break;
	case kArgumentTypeFloat64:
		uResult = static_cast<uint64_t>(Abs(m_Data.m_dDouble));
		break;

	case kArgumentTypeBool:
		uResult = static_cast<uint64_t>(m_Data.m_bBool);
		break;
	default:
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Returns the value as a single character

	If the value is a compatible to be a character, return the converted value.

	If it's not of this type, return \ref kInvalidCharConversion

	\return Character or \ref kInvalidCharConversion if not compatible.

***************************************/

char BURGER_API Burger::ArgumentType::GetChar(void) const BURGER_NOEXCEPT
{
	char iResult;
	switch (m_eType) {
	case kArgumentTypeInt8:
		iResult = m_Data.m_iInt8;
		break;
	case kArgumentTypeInt16:
		iResult = static_cast<char>(m_Data.m_iInt16);
		break;
	case kArgumentTypeInt32:
		iResult = static_cast<char>(m_Data.m_iInt32);
		break;
	case kArgumentTypeInt64:
		iResult = static_cast<char>(m_Data.m_iInt64);
		break;

	// we let the compiler handle the conversion from various int formats
	// to a single character.
	case kArgumentTypeUInt8:
		iResult = static_cast<char>(m_Data.m_uInt8);
		break;
	case kArgumentTypeUInt16:
		iResult = static_cast<char>(m_Data.m_uInt16);
		break;
	case kArgumentTypeUInt32:
		iResult = static_cast<char>(m_Data.m_uInt32);
		break;
	case kArgumentTypeUInt64:
		iResult = static_cast<char>(m_Data.m_uInt64);
		break;

	case kArgumentTypeFloat16:
		iResult = static_cast<char>(
			Unpack16ToFloat(static_cast<int16_t>(m_Data.m_fHalf)));
		break;
	case kArgumentTypeFloat32:
		iResult = static_cast<char>(m_Data.m_fFloat);
		break;
	case kArgumentTypeFloat64:
		iResult = static_cast<char>(m_Data.m_dDouble);
		break;

	case kArgumentTypeBool:
		iResult = static_cast<char>(m_Data.m_bBool);
		break;
	// we don't return zero so it will be easier to spot in the output
	default:
		iResult = kInvalidCharConversion;
		break;
	}
	return iResult;
}

/*! ************************************

	\brief Returns the value as a boolean

	If the value is a compatible to be a boolean, return the converted value.

	If it's not of this type, return 0.

	\return \ref TRUE or \ref FALSE, and \ref FALSE if not compatible.

***************************************/

uint_t BURGER_API Burger::ArgumentType::GetBool(void) const BURGER_NOEXCEPT
{
	// For non-boolean, but numeric types, return if it is zero.
	// if not a type that can compare to zero, return false;
	return (m_eType == kArgumentTypeBool) ? (m_Data.m_bBool != 0) : !IsZero();
}

/*! ************************************

	\brief Returns the value as a pointer

	If the value is a compatible to be a pointer, and return the pointer or
	\ref nullptr of not.

	\return Pointer or \ref nullptr if not a pointer

***************************************/

const void* BURGER_API Burger::ArgumentType::GetPointer(
	void) const BURGER_NOEXCEPT
{
	// m_pVoid is a union, so all pointer types are mapped into the same memory
	return ((m_eType < kArgumentTypeFirstPointer) ||
			   (m_eType > kArgumentTypeLastPointer)) ?
        nullptr :
        m_Data.m_pVoid;
}

/*! ************************************

	\brief Returns the value as a "C" string pointer

	If the value is a compatible to a "C" string, return the pointer or
	\ref nullptr of not.

	\return Pointer or \ref nullptr if not a pointer

***************************************/

const char* BURGER_API Burger::ArgumentType::GetText(void) const BURGER_NOEXCEPT
{
	const char* pResult;

	// Special case for a Burgerlib string
	if (m_eType == kArgumentTypeBurgerStringPtr) {
		if (m_Data.m_pString) {
			pResult = m_Data.m_pString->c_str();
		} else {
			pResult = nullptr;
		}
	} else if ((m_eType >= kArgumentTypeFirstCString) &&
		(m_eType <= kArgumentTypeLastCString)) {
		// Standard char pointer
		pResult = m_Data.m_pChar;
	} else {
		// None of the above
		pResult = nullptr;
	}
	return pResult;
}

/*! ************************************

	\brief Returns the length of the "C" string

	If the value is a compatible to a "C" string, return the length of the
	string or zero.

	\return Length of the string or zero.

***************************************/

uintptr_t BURGER_API Burger::ArgumentType::GetTextLength(
	void) const BURGER_NOEXCEPT
{
	uintptr_t uResult;
	if (m_eType == kArgumentTypeBurgerStringPtr) {
		if (m_Data.m_pString) {
			uResult = m_Data.m_pString->length();
		} else {
			uResult = sizeof(g_NullString) - 1;
		}
	} else if (m_eType == kArgumentTypeUInt16Ptr) {
		if (m_Data.m_pUInt16) {
			uResult = UTF8::GetUTF16Size(m_Data.m_pUInt16);
		} else {
			uResult = sizeof(g_NullString) - 1;
		}
	} else if ((m_eType >= kArgumentTypeFirstCString) &&
		(m_eType <= kArgumentTypeLastCString)) {
		// C string
		if (m_Data.m_pChar) {
			uResult = StringLength(m_Data.m_pChar);
		} else {
			uResult = sizeof(g_NullString) - 1;
		}
	} else {
		// None of the above
		uResult = 0;
	}
	return uResult;
}

/*! ************************************

	\brief Convert integer to UTF8 stream.

	If the value is an 8, 16, or 32 bit integer, convert it to UTF8 and return
	the length of the data stream. The output buffer must be 5 bytes long and
	the data will not be zero terminated.

	Incompatable data types will return a length of zero and perform no
	operation.

	\param pOutput Pointer to a buffer at least 5 bytes in length
	\return Length of the string or zero.

***************************************/

uintptr_t BURGER_API Burger::ArgumentType::GetUTF8(
	char* pOutput) const BURGER_NOEXCEPT
{
	uintptr_t uLength;
	switch (m_eType) {
	case kArgumentTypeInt8:
	case kArgumentTypeUInt8:
		pOutput[0] = static_cast<char>(m_Data.m_uInt8);
		uLength = 1;
		break;
	case kArgumentTypeInt16:
	case kArgumentTypeUInt16:
		uLength = UTF8::FromUTF16(pOutput, m_Data.m_uInt16);
		break;
	case kArgumentTypeInt32:
	case kArgumentTypeUInt32:
		uLength = UTF8::FromUTF32(pOutput, m_Data.m_uInt32);
		break;
	default:
		uLength = 0;
		break;
	}
	return uLength;
}
