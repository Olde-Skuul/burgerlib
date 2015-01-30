/***************************************

	Fixed point math functions

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfloatingpoint.h"
#include <math.h>

//
// Constant used by Intel assembly for the 8087 specific code
//

#if defined(BURGER_X86)
#if defined(BURGER_MSVC)
#pragma warning(disable:4725)	// Disable Intel instruction not accurate on some CPUs (FPREM1)
#endif

struct Word80ToLongDouble {
	union {
		Word8 w[10];	///< Value in binary
		long double l;	///< Value as a 80 bit float
	};
	BURGER_INLINE operator long double() const { return l; }
};

// Pi * 2 in 80 bit format
extern const Burger::Float80Bit Pi280bit = {0x35,0xc2,0x68,0x21,0xa2,0xda,0x0f,0xc9,0x00,0x40};
#endif

/*! ************************************

	\typedef Burger::Float80Bit

	This is a type to abstract the 80-bit floating
	point data type found in the x8087 and 680x0 
	math co-processors. Currently, this type is
	hardly used anymore with the exception of the
	audio frame rate used in AIFF files.
	
	Burgerlib only has functions to read and 
	write this format into double and float.
	
	\note Use of this data type for runtime math is
	discouraged and only used for x86 code and AIFF
	audio file support.
	
***************************************/

/*! ************************************

	\struct Burger::Word32ToFloat
	\brief Class to create float values with integer constants
	
	Some "C" compilers are not 100% accurate in creating
	floating point constants. To get around this limitation,
	once the binary representation of a floating point number is
	determined, it will be used to assign the value so the
	binary representation does not change from compiler
	to compiler.
	
	\code
	// Binary representation for Single Epsilon
	Word32ToFloat Epsilon = {{0x34000000U}};
	
	void code(void)
	{
		float fEpsilon = Epsilon;
	}
	\endcode
	
	\sa Burger::Word64ToDouble

***************************************/

/*! ************************************

	\fn Burger::Word32ToFloat::operator float() const
	\brief Accessor to load the floating point value
	
	\sa Burger::Word32ToFloat or Burger::Word64ToDouble

***************************************/


/*! ************************************

	\struct Burger::Word64ToDouble
	\brief Class to create double values with integer constants
	
	Some "C" compilers are not 100% accurate in creating
	floating point constants. To get around this limitation,
	once the binary representation of a floating point number is
	determined, it will be used to assign the value so the
	binary representation does not change from compiler
	to compiler.
	
	\code
	// Binary representation for Double Epsilon
	Word64ToDouble Epsilon = {{0x3CB0000000000000ULL}};
	
	void code(void)
	{
		double dEpsilon = Epsilon;
	}
	\endcode
	
	\sa Burger::Word32ToFloat

***************************************/

/*! ************************************

	\fn Burger::Word64ToDouble::operator double() const
	\brief Accessor to load the double precision floating point value
	
	\sa Burger::Word64ToDouble or Burger::Word32ToFloat

***************************************/

/*! ************************************

	\struct Burger::Vector_128Float64
	\brief Structure to create a 128 vector with 64 bit floating point constants
	
	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit
	value and an array of data that maps directly over it.
	
	\code
	// Initialize with 2 64 bit floats
	const Burger::Vector_128Float64 One_Vector = {{{1.0,-55.0}}};

	\endcode

	\sa Vector_128Word64, Vector_128Float32, Vector_128Word32, Vector_128Word16, Vector_128Word8, Vector_128Int32

***************************************/

/*! ************************************

	\fn Burger::Vector_128Float64::operator Vector_128() const
	\brief Accessor to load the 128 bit vector value
	
	\sa Vector_128Float64 or Vector_128

***************************************/

/*! ************************************

	\struct Burger::Vector_128Float32
	\brief Structure to create a 128 vector with 32 bit floating point constants
	
	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit
	value and an array of data that maps directly over it.
	
	\code
	// Initialize with 4 32 bit floats
	const Burger::Vector_128Float32 One_Vector = {{1.0f,1.0f,1.0f,1.0f}};

	\endcode

	\sa Vector_128Word32, Vector_128Word16, Vector_128Word8, Vector_128Int32

***************************************/

/*! ************************************

	\fn Burger::Vector_128Float32::operator Vector_128() const
	\brief Accessor to load the 128 bit vector value
	
	\sa Vector_128Float32 or Vector_128

***************************************/

/*! ************************************

	\struct Burger::Vector_128Word64
	\brief Structure to create a 128 vector with 64 bit unsigned integer constants
	
	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit
	value and an array of data that maps directly over it.
	
	\code
	// Initialize with 2 64 bit unsigned integers
	const Burger::Vector_128Word64 One_Vector = {{1,0x123456789ABCDEFULL}};

	\endcode

	\sa Vector_128Float64, Vector_128Word32, Vector_128Word16, Vector_128Word8, Vector_128Int32

***************************************/

/*! ************************************

	\fn Burger::Vector_128Word64::operator Vector_128() const
	\brief Accessor to load the 128 bit vector value
	
	\sa Vector_128Word64 or Vector_128

***************************************/

/*! ************************************

	\struct Burger::Vector_128Word32
	\brief Structure to create a 128 vector with 32 bit unsigned integer constants
	
	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit
	value and an array of data that maps directly over it.
	
	\code
	// Initialize with 4 32 bit unsigned integers
	const Burger::Vector_128Word32 One_Vector = {{1,1,1,0x12345678}};

	\endcode

	\sa Vector_128Float32, Vector_128Word16, Vector_128Word8, Vector_128Int32

***************************************/

/*! ************************************

	\fn Burger::Vector_128Word32::operator Vector_128() const
	\brief Accessor to load the 128 bit vector value
	
	\sa Vector_128Word32 or Vector_128

***************************************/

/*! ************************************

	\struct Burger::Vector_128Word16
	\brief Structure to create a 128 vector with 16 bit unsigned integer constants
	
	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit
	value and an array of data that maps directly over it.
	
	\code
	// Initialize with 8 16 bit unsigned integers
	const Burger::Vector_128Word16 One_Vector = {{1,2,3,4,5,6,7,0x1234}};

	\endcode

	\sa Vector_128Float32, Vector_128Word32, Vector_128Word8, Vector_128Int32

***************************************/

/*! ************************************

	\fn Burger::Vector_128Word16::operator Vector_128() const
	\brief Accessor to load the 128 bit vector value
	
	\sa Vector_128Word16 or Vector_128

***************************************/


/*! ************************************

	\struct Burger::Vector_128Word8
	\brief Structure to create a 128 vector with 8 bit unsigned integer constants
	
	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit
	value and an array of data that maps directly over it.
	
	\code
	// Initialize with 16 8 bit unsigned integers
	const Burger::Vector_128Word8 One_Vector = {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}};

	\endcode

	\sa Vector_128Float32, Vector_128Word32, Vector_128Word16, Vector_128Int32

***************************************/

/*! ************************************

	\fn Burger::Vector_128Word8::operator Vector_128() const
	\brief Accessor to load the 128 bit vector value
	
	\sa Vector_128Word8 or Vector_128

***************************************/


/*! ************************************

	\struct Burger::Vector_128Int32
	\brief Structure to create a 128 vector with 32 bit signed integer constants
	
	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit
	value and an array of data that maps directly over it.
	
	\code
	// Initialize with 4 32 bit signed integers
	const Burger::Vector_128Int32 One_Vector = {{-1,1,-1,0x12345678}};

	\endcode

	\sa Vector_128Float32, Vector_128Word32, Vector_128Word16, Vector_128Word8

***************************************/

/*! ************************************

	\fn Burger::Vector_128Int32::operator Vector_128() const
	\brief Accessor to load the 128 bit vector value
	
	\sa Vector_128Int32 or Vector_128

***************************************/

const Burger::Word64ToDouble Burger::g_dOne = {{0x3FF0000000000000ULL}};
const Burger::Word64ToDouble Burger::g_dHalf = {{0x3FE0000000000000ULL}};
const Burger::Word64ToDouble Burger::g_dInf = {{0x7FF0000000000000ULL}};
const Burger::Word64ToDouble Burger::g_dNan = {{0x7FF7FFFFFFFFFFFFULL}};
const Burger::Word64ToDouble Burger::g_dQNan = {{0x7FFFFFFFFFFFFFFFULL}};
const Burger::Word64ToDouble Burger::g_dMin = {{0x0010000000000000ULL}};
const Burger::Word64ToDouble Burger::g_dMax = {{0x7FEFFFFFFFFFFFFFULL}};
const Burger::Word64ToDouble Burger::g_dEpsilon = {{0x3CB0000000000000ULL}};
const Burger::Word64ToDouble Burger::g_dQuarterPi = {{0x3FE921FB54442D18ULL}};
const Burger::Word64ToDouble Burger::g_dHalfPi = {{0x3FF921FB54442D18ULL}};
const Burger::Word64ToDouble Burger::g_dPi = {{0x400921FB54442D18ULL}};
const Burger::Word64ToDouble Burger::g_dPi2 = {{0x401921FB54442D18ULL}};
const Burger::Word64ToDouble Burger::g_dPi4 = {{0x402921FB54442D18ULL}};
const Burger::Word64ToDouble Burger::g_dReciprocalHalfPi = {{0x3FE45F306DC9C883ULL}};
const Burger::Word64ToDouble Burger::g_dReciprocalPi = {{0x3FD45F306DC9C883ULL}};
const Burger::Word64ToDouble Burger::g_dReciprocalPi2 = {{0x3FC45F306DC9C883ULL}};
const Burger::Word64ToDouble Burger::g_dRadiansToDegrees = {{0x404CA5DC1A63C1F8ULL}};
const Burger::Word64ToDouble Burger::g_dDegreesToRadians = {{0x3F91DF46A2529D39ULL}};

const Burger::Word32ToFloat Burger::g_fOne = {{0x3F800000U}};
const Burger::Word32ToFloat Burger::g_fHalf = {{0x3F000000U}};
const Burger::Word32ToFloat Burger::g_fInf = {{0x7F800000U}};
const Burger::Word32ToFloat Burger::g_fNan = {{0x7FBFFFFFU}};
const Burger::Word32ToFloat Burger::g_fQNan = {{0x7FFFFFFFU}};
const Burger::Word32ToFloat Burger::g_fMin = {{0x00800000U}};
const Burger::Word32ToFloat Burger::g_fMax = {{0x7F7FFFFFU}};
const Burger::Word32ToFloat Burger::g_fEpsilon = {{0x34000000U}};
const Burger::Word32ToFloat Burger::g_fQuarterPi = {{0x3F490FDB}}; 
const Burger::Word32ToFloat Burger::g_fHalfPi = {{0x3FC90FDB}}; 
const Burger::Word32ToFloat Burger::g_fPi = {{0x40490FDB}}; 
const Burger::Word32ToFloat Burger::g_fPi2 = {{0x40C90FDB}}; 
const Burger::Word32ToFloat Burger::g_fPi4 = {{0x41490FDB}}; 
const Burger::Word32ToFloat Burger::g_fReciprocalHalfPi = {{0x3F22F983}}; 
const Burger::Word32ToFloat Burger::g_fReciprocalPi = {{0x3EA2F983}}; 
const Burger::Word32ToFloat Burger::g_fReciprocalPi2 = {{0x3E22F983}}; 
const Burger::Word32ToFloat Burger::g_fRadiansToDegrees = {{0x42652EE1}}; 
const Burger::Word32ToFloat Burger::g_fDegreesToRadians = {{0x3C8EFA35}}; 

/*! ************************************

	\enum Burger::e8087Precision
	\brief 32 bit Intel floating point precision
	
	On Intel 32 bit processors, there's a
	special floating point register to control the precision
	of the 80387 compatible FPU

	\note This enumeration only exists on 32 bit Intel compatible
	CPU targets

	\sa Set8087Precision(e8087Precision)

***************************************/

/*! ************************************

	\fn Burger::e8087Precision BURGER_API Burger::Set8087Precision(e8087Precision eInput)
	\brief Change the floating point precision
	
	On Intel 32 bit processors, there's a
	special floating point register to control the precision
	of the 80387 compatible FPU. This function allows the
	modification of this register for FPU precision. Lower
	precision yields more speed.

	\note This function only exists on 32 bit Intel compatible
		CPU targets.
	\note If Direct3D is enabled, Windows sets the precision to 24 bit
		on 32 bit platforms. This function will undo the change if
		that's not desired.

	\param eInput New enumeration state
	\return Previous precision state
	\sa e8087Precision

***************************************/

#if defined(BURGER_X86) && (defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || defined(BURGER_MSVC))
BURGER_DECLSPECNAKED Burger::e8087Precision BURGER_API Burger::Set8087Precision(e8087Precision /* eInput */)
{
	BURGER_ASM {
#if defined(BURGER_WATCOM)
	sub		esp,12				// Space for the status word
	mov		[esp+8],ecx			// ecx must be preserved for Watcom
	mov		ecx,eax
#else
	sub		esp,8				// Space for the status word
#endif
	fnstcw	[esp]				// Get the current status word
	mov		[esp+4],edx
	mov		eax,dword ptr [esp]	// Get the current value for return
	shl		ecx,8				// Move to the Pentium bits area
	mov		edx,eax				// Get the bits to retain for write
	and		eax,0x0300			// Mask for return value
	and		edx,0xFCFF			// Mask off unused bits
	shr		eax,8				// Convert to enum
	or		edx,ecx 			// Blend in the bits
	mov		dword ptr [esp],edx	// Store in memory
	fldcw	[esp]				// Save the new status register
	mov		edx,[esp+4]
#if defined(BURGER_WATCOM)
	mov		ecx,[esp+8]
	add		esp,12				// Fix stack
#else
	add		esp,8				// Fix stack
#endif
	ret
	}
}
#elif defined(BURGER_X86) && defined(BURGER_MACOSX)
// __ZN6Burger16Set8087PrecisionENS_14e8087PrecisionE = Burger::e8087Precision BURGER_API Burger::Set8087Precision(e8087Precision /* eInput */)
__asm__(
"	.align	4,0x90\n"
"	.globl __ZN6Burger16Set8087PrecisionENS_14e8087PrecisionE\n"
"__ZN6Burger16Set8087PrecisionENS_14e8087PrecisionE:\n"
"	subl	%esp,12\n"			// Space for the status word
"	movl	%ecx,8(%esp)\n"		// ecx must be preserved for Watcom
"	movl	%eax,%ecx\n"
"	fnstcw	(%esp)\n"			// Get the current status word
"	movl	%edx,4(%esp)\n"
"	movl	(%esp),%eax\n"		// Get the current value for return
"	shll	$8,%ecx\n"			// Move to the Pentium bits area
"	movl	%eax,%edx\n"		// Get the bits to retain for write
"	andl	$0x0300,%eax\n"		// Mask for return value
"	andl	$0xFCFF,%edx\n"		// Mask off unused bits
"	shrl	$8,%eax\n"			// Convert to enum
"	orl		%ecx,%edx\n" 		// Blend in the bits
"	movl	%edx,(%esp)\n"		// Store in memory
"	fldcw	(%esp)\n"			// Save the new status register
"	movl	4(%esp),%edx\n"
"	movl	8(%esp),%ecx\n"
"	addl	%esp,12\n"			// Fix stack
"	ret\n"
);
#endif

/*! ************************************

	\enum Burger::e8087Rounding
	\brief 32 bit Intel floating point rounding mode
	
	On Intel 32 bit processors, there's a
	special floating point register to control the rounding
	mode from float to integer conversions in the 80387 compatible FPU

	\note This enumeration only exists on 32 bit Intel compatible
	CPU targets

	\sa Set8087Rounding(e8087Rounding)

***************************************/

/*! ************************************

	\fn Burger::e8087Rounding BURGER_API Burger::Set8087Rounding(e8087Rounding eInput)
	\brief Change the floating point precision
	
	On Intel 32 bit processors, there's a
	special floating point register to control the rounding
	of the 80387 compatible FPU. This function allows the
	modification of this register for FPU rounding.

	\note This function only exists on 32 bit Intel compatible
		CPU targets.

	\param eInput New enumeration state
	\return Previous rounding state
	\sa e8087Rounding

***************************************/

#if defined(BURGER_X86) && (defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || defined(BURGER_MSVC))
BURGER_DECLSPECNAKED Burger::e8087Rounding BURGER_API Burger::Set8087Rounding(e8087Rounding /* eInput */)
{
	BURGER_ASM {
#if defined(BURGER_WATCOM)
	sub		esp,12				// Space for the status word
	mov		[esp+8],ecx			// ecx must be preserved for Watcom
	mov		ecx,eax
#else
	sub		esp,8				// Space for the status word
#endif
	fnstcw	[esp]				// Get the current status word
	mov		[esp+4],edx
	mov		eax,dword ptr [esp]	// Get the current value for return
	shl		ecx,10				// Move to the Pentium bits area
	mov		edx,eax				// Get the bits to retain for write
	and		eax,0x0C00			// Mask for return value
	and		edx,0xF3FF			// Mask off unused bits
	shr		eax,10				// Convert to enum
	or		edx,ecx 			// Blend in the bits
	mov		dword ptr [esp],edx	// Store in memory
	fldcw	[esp]				// Save the new status register
	mov		edx,[esp+4]
#if defined(BURGER_WATCOM)
	mov		ecx,[esp+8]
	add		esp,12				// Fix stack
#else
	add		esp,8				// Fix stack
#endif
	ret
	}
}
#elif defined(BURGER_X86) && defined(BURGER_MACOSX)
// __ZN6Burger15Set8087RoundingENS_13e8087RoundingE = Burger::e8087Rounding BURGER_API Burger::Set8087Rounding(e8087Rounding /* eInput */)
__asm__(
"	.align	4,0x90\n"
"	.globl __ZN6Burger15Set8087RoundingENS_13e8087RoundingE\n"
"__ZN6Burger15Set8087RoundingENS_13e8087RoundingE:\n"
"	subl	%esp,12\n"			// Space for the status word
"	movl	%ecx,8(%esp)\n"		// ecx must be preserved for Watcom
"	movl	%eax,%ecx\n"
"	fnstcw	(%esp)\n"			// Get the current status word
"	movl	%edx,4(%esp)\n"
"	movl	(%esp),%eax\n"		// Get the current value for return
"	shll	$10,%ecx\n"			// Move to the Pentium bits area
"	movl	%eax,%edx\n"		// Get the bits to retain for write
"	andl	$0x0C00,%eax\n"		// Mask for return value
"	andl	$0xF3FF,%edx\n"		// Mask off unused bits
"	shrl	$10,%eax\n"			// Convert to enum
"	orl		%ecx,%edx\n" 		// Blend in the bits
"	movl	%edx,(%esp)\n"		// Store in memory
"	fldcw	(%esp)\n"			// Save the new status register
"	movl	4(%esp),%edx\n"
"	movl	8(%esp),%ecx\n"
"	addl	%esp,12\n"			// Fix stack
"	ret\n"
);
#endif

/*! ************************************

	\fn Burger::Sqr(float fInput);
	\brief Get the square value of a float
	
	Return the squared value of the floating point input. 

	\note -Inf will return +Inf. NaN is undefined
	
	\param fInput A valid single precision floating point number.
	\return The squared value of the input.
	\sa Sqr(double) 

***************************************/

/*! ************************************

	\fn Burger::Sqr(double dInput);
	\brief Get the square value of a double
	
	Return the squared value of the 64 bit floating point input. 

	\note -Inf will return +Inf. NaN is undefined
	
	\param dInput A valid double precision floating point number.
	\return The squared value of the input.
	\sa Sqr(float) 

***************************************/

/*! ************************************

	\fn Burger::Abs(float fInput);
	\brief Get the absolute value of a float
	
	Return the absolute value of the floating point input. 
	This is usually done by inline assembly.

	\note -Inf will return +Inf. NaN is undefined
	
	\param fInput A valid single precision floating point number.
	\return The absolute value of the input.
	\sa Abs(Int32), Abs(Int64) and Abs(double) 

***************************************/

#if defined(BURGER_WATCOM)
#elif defined(BURGER_METROWERKS) && defined(BURGER_POWERPC)
#elif defined(BURGER_METROWERKS) && defined(BURGER_X86)
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
#elif (defined(BURGER_X86) || defined(BURGER_AMD64) || (defined(BURGER_MACOSX) && !defined(BURGER_METROWERKS))) && !defined(DOXYGEN)
#elif defined(BURGER_PS4) || defined(BURGER_VITA)
#else
float BURGER_API Burger::Abs(float fInput)
{
	return fInput>=0 ? fInput : -fInput;
}
#endif

/*! ************************************

	\fn Burger::Abs(double dInput);
	\brief Get the absolute value of a double
	
	Return the absolute value of the floating point input. 
	This is usually done by inline assembly.
	
	\note -Inf will return +Inf. NaN is undefined

	\param dInput A valid double precision floating point number.
	\return The absolute value of the input.
	\sa Abs(Int32), Abs(Int64) and Abs(float) 

***************************************/

#if defined(BURGER_WATCOM)
#elif defined(BURGER_METROWERKS) && defined(BURGER_POWERPC)
#elif defined(BURGER_METROWERKS) && defined(BURGER_X86)
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
#elif (defined(BURGER_X86) || defined(BURGER_AMD64) || (defined(BURGER_MACOSX) && !defined(BURGER_METROWERKS))) && !defined(DOXYGEN)
#elif defined(BURGER_PS4) || defined(BURGER_VITA)
#else
double BURGER_API Burger::Abs(double dInput)
{
	return dInput>=0 ? dInput : -dInput;
}
#endif

/*! ************************************

	\fn Burger::Sqrt(float fInput);
	\brief Get the square root value of a float
	
	Return the square root of the floating point input. 
	This is usually done by inline assembly.
	
	\param fInput A valid single precision floating point number.
	\return The square root of the input.
	\sa Sqrt(double), Sqrt(Word32), and Sqrt(Fixed32)

***************************************/

#if defined(BURGER_WATCOM)
#elif defined(BURGER_METROWERKS) && defined(BURGER_POWERPC)
#elif defined(BURGER_METROWERKS) && defined(BURGER_X86)
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
#elif (defined(BURGER_X86) || defined(BURGER_AMD64) || (defined(BURGER_MACOSX) && !defined(BURGER_METROWERKS))) && !defined(DOXYGEN)
#elif defined(BURGER_PS4) || defined(BURGER_VITA)
#else
float BURGER_API Burger::Sqrt(float fInput)
{
	return static_cast<float>(sqrt(fInput));
}
#endif

/*! ************************************

	\fn Burger::Sqrt(double dInput);
	\brief Get the square root value of a double
	
	Return the square root of the floating point input. 
	This is usually done by inline assembly.
	
	\param dInput A valid double precision floating point number.
	\return The square root of the input.
	\sa Sqrt(float), Sqrt(Word32), and Sqrt(Fixed32)

***************************************/

#if defined(BURGER_WATCOM)
#elif defined(BURGER_METROWERKS) && defined(BURGER_POWERPC)
#elif defined(BURGER_METROWERKS) && defined(BURGER_X86)
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
#elif (defined(BURGER_X86) || defined(BURGER_AMD64) || (defined(BURGER_MACOSX) && !defined(BURGER_METROWERKS))) && !defined(DOXYGEN)
#elif defined(BURGER_PS4) || defined(BURGER_VITA)
#else
double BURGER_API Burger::Sqrt(double dInput)
{
	return sqrt(dInput);
}
#endif

/*! ************************************

	\fn Burger::IntToFloat(Int32 iInput)
	\brief 32 bit integer to floating point conversion

	\param iInput 32 bit integer to convert
	\return Floating point representation of the integer input
	
	\sa IntToFloat(const Int32 *), IntToFloat(float *,Int32) or IntToFloat(float *,const Int32 *)

***************************************/

/*! ************************************

	\fn Burger::IntToFloat(const Int32 *pInput)
	\brief 32 bit integer to floating point conversion

	\param pInput Pointer to a 32 bit integer to convert
	\return Floating point representation of the integer input
	
	\sa IntToFloat(Int32), IntToFloat(float *,Int32) or IntToFloat(float *,const Int32 *)

***************************************/

/*! ************************************

	\fn Burger::IntToFloat(float *pOutput,Int32 iInput)
	\brief 32 bit integer to floating point conversion

	\param iInput 32 bit integer to convert
	\param pOutput Pointer to a 32 bit float to receive the converted integer
	
	\sa IntToFloat(Int32), IntToFloat(const Int32 *) or IntToFloat(float *,const Int32 *)

***************************************/

/*! ************************************

	\fn Burger::IntToFloat(float *pOutput,const Int32 *pInput)
	\brief 32 bit integer to floating point conversion
	
	\note This function is fast on the Xbox 360 due to direct use of VMX128 instructions

	\param pInput Pointer to a 32 bit integer to convert
	\param pOutput Pointer to a 32 bit float to receive the converted integer
	
	\sa IntToFloat(Int32), IntToFloat(const Int32 *) or IntToFloat(float *,Int32)

***************************************/




/*! ************************************

	\fn Burger::FixedToFloat(Fixed32 iInput)
	\brief 32 bit 16.16 fixed point integer to floating point conversion

	\param iInput 32 bit 16.16 fixed point integer to convert
	\return Floating point representation of the integer input
	
	\sa FixedToFloat(const Fixed32 *), FixedToFloat(float *,Fixed32) or FixedToFloat(float *,const Fixed32 *)

***************************************/

/*! ************************************

	\fn Burger::FixedToFloat(const Fixed32 *pInput)
	\brief 32 bit 16.16 fixed point integer to floating point conversion

	\param pInput Pointer to a 32 bit 16.16 fixed point integer to convert
	\return Floating point representation of the integer input
	
	\sa FixedToFloat(Fixed32), FixedToFloat(float *,Fixed32) or FixedToFloat(float *,const Fixed32 *)

***************************************/

/*! ************************************

	\fn Burger::FixedToFloat(float *pOutput,Fixed32 iInput)
	\brief 32 bit 16.16 fixed point integer to floating point conversion

	\param iInput 32 bit 16.16 fixed point integer to convert
	\param pOutput Pointer to a 32 bit float to receive the converted integer
	
	\sa FixedToFloat(Fixed32), FixedToFloat(const Fixed32 *) or FixedToFloat(float *,const Fixed32 *)

***************************************/

/*! ************************************

	\fn Burger::FixedToFloat(float *pOutput,const Fixed32 *pInput)
	\brief 32 bit 16.16 fixed point integer to floating point conversion
	
	\note This function is fast on the Xbox 360 due to direct use of VMX128 instructions

	\param pInput Pointer to a 32 bit 16.16 fixed point integer to convert
	\param pOutput Pointer to a 32 bit float to receive the converted integer
	
	\sa FixedToFloat(Fixed32), FixedToFloat(const Fixed32 *) or FixedToFloat(float *,Fixed32)

***************************************/

/*! ************************************

	\fn Burger::Interpolate(float fFrom,float fTo,float fFactor)
	\brief Interpolate between two floating point values

	Using a factor that's clamped from 0.0f to 1.0f, return the value
	between fFrom and fTo inclusive along a linear scale where
	0.0f returns fFrom and 1.0f returns fTo. Order of the fFrom and
	fTo values are unimportant.

	\note NaN and Inf as input will yield undefined behavior
	
	\param fFrom Value to return if the interpolation factor is 0.0f
	\param fTo Value to return if the interpolation factor is 1.0f
	\param fFactor Interpolation between 0.0f and 1.0f inclusive
	\return Value between fFrom and fTo
	
	\sa Interpolate(double,double,double)

***************************************/

/*! ************************************

	\fn Burger::Interpolate(double dFrom,double dTo,double dFactor)
	\brief Interpolate between two floating point values

	Using a factor that's clamped from 0.0 to 1.0, return the value
	between dFrom and dTo inclusive along a linear scale where
	0.0 returns dFrom and 1.0 returns dTo. Order of the dFrom and
	dTo values are unimportant.
	
	\note NaN and Inf as input will yield undefined behavior

	\param dFrom Value to return if the interpolation factor is 0.0
	\param dTo Value to return if the interpolation factor is 1.0
	\param dFactor Interpolation between 0.0 and 1.0 inclusive
	\return Value between dFrom and dTo
	
	\sa Interpolate(float,float,float)

***************************************/

/*! ************************************

	\fn float Burger::Square(float fInput)
	\brief Return the square of the input
	
	Square the number and return the result.
		
	\param fInput Value to square
	\return The square of the input
	
	\sa Square(double)
	
***************************************/

/*! ************************************

	\fn double Burger::Square(double dInput)
	\brief Return the square of the input
	
	Square the number and return the result.
		
	\param dInput Value to square
	\return The square of the input
	
	\sa Square(float)
	
***************************************/

/*! ************************************

	\fn float Burger::Sign(float fInput)
	\brief Return sign constant
	
	If the input is less than zero, return
	negative one, if the input is greater than zero,
	return one, otherwise return zero.
		
	\param fInput Value to test
	\return The sign constant of 1, -1 or 0
	
	\sa Sign(double)
	
***************************************/

/*! ************************************

	\fn double Burger::Sign(double dInput)
	\brief Return sign constant
	
	If the input is less than zero, return
	negative one, if the input is greater than zero,
	return one, otherwise return zero.
		
	\param dInput Value to test
	\return The sign constant of 1, -1 or 0
	
	\sa Sign(float)
	
***************************************/

/*! ************************************

	\fn float Burger::Min(float fA,float fB)
	\brief Return the lesser of two numbers

	Compare the two input values and return the lesser of the two.
		
	\param fA First value to test
	\param fB Second value to test
	\return The lesser of the two inputs
	
	\sa Min(double,double) or Max(float,float)
	
***************************************/

/*! ************************************

	\fn double Burger::Min(double dA,double dB)
	\brief Return the lesser of two numbers

	Compare the two input values and return the lesser of the two.
		
	\param dA First value to test
	\param dB Second value to test
	\return The lesser of the two inputs
	
	\sa Min(float,float) or Max(double,double)
	
***************************************/

/*! ************************************

	\fn float Burger::Max(float fA,float fB)
	\brief Return the greater of two numbers

	Compare the two input values and return the greater of the two.
		
	\param fA First value to test
	\param fB Second value to test
	\return The greater of the two inputs
	
	\sa Max(double,double), or Min(float,float)
	
***************************************/

/*! ************************************

	\fn double Burger::Max(double dA,double dB)
	\brief Return the greater of two numbers

	Compare the two input values and return the greater of the two.
		
	\param dA First value to test
	\param dB Second value to test
	\return The greater of the two inputs
	
	\sa Max(float,float), or Min(double,double)
	
***************************************/

/*! ************************************

	\fn float Burger::Clamp(float fInput,float fMin,float fMax)
	\brief Clamp the input between a bounds

	If the input value is less than the minimum, return the minimum
	or if the input value is greater than the maximum, return 
	the maximum, otherwise return the input value. No
	checking is performed to determine if the minimum is
	less than the maximum.
		
	\param fInput First value to test
	\param fMin Minimum allowed value
	\param fMax Maximum allowed value
	\return The value clamped between the bounds

	\sa Clamp(double,double,double)
	
***************************************/

/*! ************************************

	\fn double Burger::Clamp(double dInput,double dMin,double dMax)
	\brief Clamp the input between a bounds

	If the input value is less than the minimum, return the minimum
	or if the input value is greater than the maximum, return 
	the maximum, otherwise return the input value. No
	checking is performed to determine if the minimum is
	less than the maximum.
		
	\param dInput First value to test
	\param dMin Minimum allowed value
	\param dMax Maximum allowed value
	\return The value clamped between the bounds

	\sa Clamp(float,float,float)
	
***************************************/

/*! ************************************

	\brief Return \ref TRUE if the value is a NaN
	
	Test for QNan and SNan and return \ref TRUE if so.
	
	\param fInput A 32 bit floating point number.
	\return \ref TRUE if Nan, \ref FALSE if not.
	\sa IsNan(double), IsInf(float), IsFinite(float), IsNormal(float) and SignBit(float)

***************************************/

#if defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
BURGER_ASM Word BURGER_API Burger::IsNan(float fInput)
{
	fcmpu	cr0,fp1,fp1		// Perform a NaN test
	mfcr	r0				// Move cr0 into r0
	extrwi	r3,r0,1,3		// (Flags>>28)&1 Grab the "Unordered" flag
	blr						// Exit
}
#else
Word BURGER_API Burger::IsNan(float fInput)
{
	Word32 uInput = reinterpret_cast<Word32 *>(&fInput)[0];
	// Kill off the high bit
	uInput &= 0x7FFFFFFF;
	// Set the high bit if 0x7F800001-0x7FFFFFFF
	uInput = 0x7F800000-uInput;
	// Return TRUE or FALSE depending on the test
	return (uInput>>31U);
}
#endif

/*! ************************************

	\brief Return \ref TRUE if the value is a NaN
	
	Test for QNan and SNan and return \ref TRUE if so.
	
	\param dInput A 64 bit floating point number.
	\return \ref TRUE if Nan, \ref FALSE if not.
	\sa IsNan(float), IsInf(double), IsFinite(double), IsNormal(double) and SignBit(double)

***************************************/

#if defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
BURGER_ASM Word BURGER_API Burger::IsNan(double dInput)
{
	fcmpu	cr0,fp1,fp1		// Perform a NaN test
	mfcr	r0				// Move cr0 into r0
	extrwi	r3,r0,1,3		// (Flags>>28)&1 Grab the "Unordered" flag
	blr						// Exit
}
#else
Word BURGER_API Burger::IsNan(double dInput)
{
#if defined(BURGER_64BITCPU)
	Word64 uInput = reinterpret_cast<Word64 *>(&dInput)[0];
	// Kill off the high bit
	uInput &= 0x7FFFFFFFFFFFFFFFULL;
	// Set the high bit if 0x7F800001-0x7FFFFFFF
	uInput = 0x7FF0000000000000ULL-uInput;
	// Return TRUE or FALSE depending on the test
	return static_cast<Word>(uInput>>63U);
#else
	return (reinterpret_cast<const Word64 *>(&dInput)[0] << 1U) > (0x7ff0000000000000ULL<<1U);
#endif
}
#endif

/*! ************************************

	\brief Return \ref TRUE if the value is infinity
	
	Test for infinity and return \ref TRUE if so.
	
	\param fInput A 32 bit floating point number.
	\return \ref TRUE if Infinity, \ref FALSE if not.
	\sa IsInf(double), IsNan(float), IsFinite(float), IsNormal(float) and SignBit(float)

***************************************/

#if defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
BURGER_ASM Word BURGER_API Burger::IsInf(float fInput)
{
	lwz		r3,g_fInf(RTOC)	// Get the address of g_fInf
	fabs	fp1,fp1			// Strip the sign
	lfs		fp0,0(r3)		// Fetch fInf
	fcmpu	cr0,fp1,fp0		// Compare for equality
	mfcr	r0				// Move cr0 into r0
	extrwi	r3,r0,1,2		// (Flags>>29)&1 Grab the "Equal" flag
	blr						// Exit
}
#else
Word BURGER_API Burger::IsInf(float fInput)
{
	Word32 uInput = reinterpret_cast<Word32 *>(&fInput)[0];
	Word32 uTemp = uInput & 0x7FFFFFFF;
	return uTemp == 0x7F800000;
}
#endif

/*! ************************************

	\brief Return \ref TRUE if the value is infinity
	
	Test for infinity and return \ref TRUE if so.
	
	\param dInput A 64 bit floating point number.
	\return \ref TRUE if Infinity, \ref FALSE if not.
	\sa IsInf(float), IsNan(double), IsFinite(double), IsNormal(double) and SignBit(double)

***************************************/

#if defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
BURGER_ASM Word BURGER_API Burger::IsInf(double dInput)
{
	lwz		r3,g_dInf(RTOC)	// Get the address of g_dInf
	fabs	fp1,fp1			// Strip the sign
	lfd		fp0,0(r3)		// Fetch dInf
	fcmpu	cr0,fp1,fp0		// Compare for equality
	mfcr	r0				// Move cr0 into r0
	extrwi	r3,r0,1,2		// (Flags>>29)&1 Grab the "Equal" flag
	blr						// Exit
}
#else
Word BURGER_API Burger::IsInf(double dInput)
{
	return (reinterpret_cast<const Word64 *>(&dInput)[0] << 1U) == (0x7ff0000000000000ULL<<1U);
}
#endif

/*! ************************************

	\brief Return \ref TRUE if the value is finite
	
	Test for a finite number (Not Nan, Inf) and return \ref TRUE if so.
	
	\param fInput A 32 bit floating point number.
	\return \ref TRUE if finite, \ref FALSE if not.
	\sa IsFinite(double), IsNan(float), IsInf(float), IsNormal(float) and SignBit(float)

***************************************/

#if defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
BURGER_ASM Word BURGER_API Burger::IsFinite(float fInput)
{
	lwz		r3,g_fInf(RTOC)	// Get the address of g_fInf
	fabs	fp1,fp1			// Strip the sign
	lfs		fp0,0(r3)		// Fetch fInf
	fcmpu	cr0,fp1,fp0		// Compare if less than infinity
	mfcr	r0				// Move cr0 into r0
	extrwi	r3,r0,1,0		// (Flags>>31)&1 Grab the "Less than" flag
	blr						// Exit
}
#else
Word BURGER_API Burger::IsFinite(float fInput)
{
	Word32 uInput = reinterpret_cast<Word32 *>(&fInput)[0];
	Word32 uTemp = uInput & 0x7FFFFFFF;
	uTemp = uTemp-0x7F800000;
	return (uTemp>>31);
}
#endif

/*! ************************************

	\brief Return \ref TRUE if the value is finite
	
	Test for a finite number (Not Nan, Inf) and return \ref TRUE if so.
	
	\param dInput A 64 bit floating point number.
	\return \ref TRUE if finite, \ref FALSE if not.
	\sa IsFinite(float), IsNan(double), IsInf(double), IsNormal(double) and SignBit(double)

***************************************/

#if defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
BURGER_ASM Word BURGER_API Burger::IsFinite(double dInput)
{
	lwz		r3,g_dInf(RTOC)	// Get the address of g_dInf
	fabs	fp1,fp1			// Strip the sign
	lfd		fp0,0(r3)		// Fetch dInf
	fcmpu	cr0,fp1,fp0		// Compare if less than infinity
	mfcr	r0				// Move cr0 into r0
	extrwi	r3,r0,1,0		// (Flags>>31)&1 Grab the "Less than" flag
	blr						// Exit
}
#else
Word BURGER_API Burger::IsFinite(double dInput)
{
	Word32 uInput;
#if defined(BURGER_BIGENDIAN)
	uInput = reinterpret_cast<const Word32 *>(&dInput)[0];
#else
	uInput = reinterpret_cast<const Word32 *>(&dInput)[1];
#endif
	Word32 uTemp = uInput & 0x7FFFFFFF;
	uTemp = uTemp-0x7FF00000;
	return (uTemp>>31);
}
#endif

/*! ************************************

	\brief Return \ref TRUE if the value is normal
	
	Test for a finite normal number (Not Zero, NaN, Inf or denormalized) and return \ref TRUE if so.
	
	\param fInput A 32 bit floating point number.
	\return \ref TRUE if normal, \ref FALSE if not.
	\sa IsNormal(double), IsNan(float), IsInf(float), IsFinite(float) and SignBit(float)

***************************************/

Word BURGER_API Burger::IsNormal(float fInput)
{
	Word32 uInput = reinterpret_cast<Word32 *>(&fInput)[0];
	Word32 uTemp = (uInput-0x00800000U) & 0x7FFFFFFF;
	uTemp = uTemp-(0x7F800000U-0x00800000U);
	return (uTemp>>31);
}

/*! ************************************

	\brief Return \ref TRUE if the value is normal
	
	Test for a finite normal number (Not Zero, NaN, Inf or denormalized) and return \ref TRUE if so.
	
	\param dInput A 64 bit floating point number.
	\return \ref TRUE if normal, \ref FALSE if not.
	\sa IsNormal(float), IsNan(double), IsInf(double), IsFinite(double) and SignBit(double)

***************************************/

Word BURGER_API Burger::IsNormal(double dInput)
{
	Word32 uInput;
#if defined(BURGER_BIGENDIAN)
	uInput = reinterpret_cast<const Word32 *>(&dInput)[0];
#else
	uInput = reinterpret_cast<const Word32 *>(&dInput)[1];
#endif
	Word32 uTemp = (uInput-0x00100000U) & 0x7FFFFFFF;
	uTemp = uTemp-(0x7FF00000-0x00100000U);
	return (uTemp>>31);
}

/*! ************************************

	\brief Return \ref TRUE if the value is negative
	
	Test for a negative number and return \ref TRUE if so.
	
	\param fInput A 32 bit floating point number.
	\return \ref TRUE if negative, \ref FALSE if not.
	\sa SignBit(double), IsNan(float), IsInf(float), IsFinite(float) and IsNormal(float)

***************************************/

Word BURGER_API Burger::SignBit(float fInput)
{
	return reinterpret_cast<const Word32 *>(&fInput)[0]>>31;
}

/*! ************************************

	\brief Return \ref TRUE if the value is negative
	
	Test for a negative number and return \ref TRUE if so.
	
	\param dInput A 64 bit floating point number.
	\return \ref TRUE if negative, \ref FALSE if not.
	\sa SignBit(float), IsNan(double), IsInf(double), IsFinite(double) and IsNormal(double)

***************************************/

Word BURGER_API Burger::SignBit(double dInput)
{
#if defined(BURGER_BIGENDIAN)
	return reinterpret_cast<const Word32 *>(&dInput)[0]>>31;
#else
	return reinterpret_cast<const Word32 *>(&dInput)[1]>>31;
#endif
}


/*! ************************************

	\brief Return a floating point number with the fraction removed

	If there is any fraction, round the value down to the 
	whole integer.

	\param fInput 32 bit floating point value to round down to the nearest integer.
	\return The input converted to a 32 bit floating point value using the truth table below.

	\code
	float fResult;
	fResult = Burger::Floor(1.1f);		//1
	fResult = Burger::Floor(1.95f);		//1
	fResult = Burger::Floor(-1.1f);		//-2
	fResult = Burger::Floor(-1.95f);	//-2
	fResult = Burger::Floor(0.1f);		//0
	fResult = Burger::Floor(0.95f);		//0
	fResult = Burger::Floor(-0.1f);		//-1
	fResult = Burger::Floor(-0.95f);	//-1
	\endcode

	\sa Floor(double), Ceil(float), Round(float), RoundToZero(float), or FixedToIntFloor(Fixed32)

***************************************/

#if defined(BURGER_X86) && (defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || defined(BURGER_MSVC))
BURGER_DECLSPECNAKED float BURGER_API Burger::Floor(float /* fInput */)
{
	BURGER_ASM {
	mov		eax,[esp+4]				// Load the value in an integer register
	fld		dword ptr [esp+4]		// Load the same value in the FPU
	and		eax,0x7FFFFFFF			// Mask off the sign
	cmp		eax,0x4B000000			// Compare to 8388608.0f
	jae		FloorExit				// Out of range, return original value
	frndint							// Convert the integer to float (It's in range)
	xor		eax,eax					// Clear ax for flags
	fcom	dword ptr [esp+4]		// Compare values for difference (Pop stack)
	fnstsw	ax						
	and		eax,0x100				// Was the new greater than the original?
	jnz		FloorExit
	fsub	dword ptr [g_fOne]		// Subtract 1.0f to round down
FloorExit:
	ret		4						// Clean up and exit
	}
}
#elif defined(BURGER_X86) && defined(BURGER_MACOSX)
// __ZN6Burger5FloorEf = float BURGER_API Burger::Floor(float /* fInput */)
__asm__(
"	.align	4,0x90\n"
"	.globl __ZN6Burger5FloorEf\n"
"__ZN6Burger5FloorEf:\n"
"	movl	4(%esp),%eax\n"			// Load the value in an integer register
"	flds	4(%esp)\n"				// Load the same value in the FPU
"	andl	$0x7FFFFFFF,%eax\n"		// Mask off the sign
"	cmpl	$0x4B000000,%eax\n"		// Compare to 8388608.0f
"	jae		1f\n"					// Out of range, return original value
"	frndint\n"						// Convert the integer to float (It's in range)
"	xorl	%eax,%eax\n"			// Clear ax for flags
"	fcoms	4(%esp)\n"				// Compare values for difference (Pop stack)
"	fnstsw	%ax\n"
"	andl	$0x100,%eax\n"			// Was the new greater than the original?
"	jnz		1f\n"
"	fsubs	(__ZN6Burger6g_fOneE)\n"	// Subtract 1.0f to round down
"1:	ret\n"							// Clean up and exit
);
#else
float BURGER_API Burger::Floor(float fInput)
{
	// Note : 8388608 is the first floating point number
	// that cannot have a fraction. Therefore this routine can't
	// affect it. I need to handle it special case since I need to
	// convert the float to an int and it's possible to have a float out
	// of bounds. This is a bad thing.
	if (fInput<8388608.0f && fInput>-8388608.0f) {		// Within bounds?
		int iVar = static_cast<int>(fInput);			// Convert to int but rounded!
		float fVar = static_cast<float>(iVar);
		if (fVar>fInput) {	// Did I round up?
			--fVar;			// Fix it
		}
		fInput = fVar;		// Return floored value
	}
	return fInput;
}
#endif

/*! ************************************

	\brief Return a floating point number with the fraction removed

	If there is any fraction, round the value down to the 
	whole integer.

	\param dInput 64 bit floating point value to round down to the nearest integer.
	\return The input converted to a 64 bit floating point value using the truth table below.

	\code
	double dResult;
	dResult = Burger::Floor(1.1);	//1
	dResult = Burger::Floor(1.95);	//1
	dResult = Burger::Floor(-1.1);	//-2
	dResult = Burger::Floor(-1.95);	//-2
	dResult = Burger::Floor(0.1);	//0
	dResult = Burger::Floor(0.95);	//0
	dResult = Burger::Floor(-0.1);	//-1
	dResult = Burger::Floor(-0.95);	//-1
	\endcode

	\sa Floor(float), Ceil(double), Round(double), RoundToZero(double), or FixedToIntFloor(Fixed32)

***************************************/

double BURGER_API Burger::Floor(double dInput)
{
	// Note : 4503599627370496.0 is the first floating point number
	// that cannot have a fraction. Therefore this routine can't
	// affect it. I need to handle it special case since I need to
	// convert the double to an int and it's possible to have a double out
	// of bounds. This is a bad thing.
	if (dInput<4503599627370496.0 && dInput>-4503599627370496.0) {		// Within bounds?
		Int64 iVar = static_cast<Int64>(dInput);			// Convert to int but rounded!
		double dVar = static_cast<double>(iVar);
		if (dVar>dInput) {	// Did I round up?
			--dVar;			// Fix it
		}
		dInput = dVar;		// Return floored value
	}
	return dInput;
}

/*! ************************************

	\brief Return a floating point number rounded up

	If there is any fraction, round the value up to the 
	next whole integer.

	\param fInput 32 bit floating point value to round up to the nearest integer.
	\return The input converted to a 32 bit floating point value using the truth table below.

	\code
	float fResult;
	fResult = Burger::Ceil(1.1f);	//2
	fResult = Burger::Ceil(1.95f);	//2
	fResult = Burger::Ceil(-1.1f);	//-1
	fResult = Burger::Ceil(-1.95f);	//-1
	fResult = Burger::Ceil(0.1f);	//1
	fResult = Burger::Ceil(0.95f);	//1
	fResult = Burger::Ceil(-0.1f);	//0
	fResult = Burger::Ceil(-0.95f);	//0
	\endcode

	\sa Ceil(double), Floor(float), Round(float), RoundToZero(float), or FixedToIntCeil(Fixed32)

***************************************/

#if defined(BURGER_X86) && (defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || defined(BURGER_MSVC))
BURGER_DECLSPECNAKED float BURGER_API Burger::Ceil(float /* fInput */)
{
	BURGER_ASM {
	mov		eax,[esp+4]				// Load the value in an integer register
	fld		dword ptr [esp+4]		// Load the same value in the FPU
	and		eax,0x7FFFFFFF			// Mask off the sign
	cmp		eax,0x4B000000			// Compare to 8388608.0f
	jae		CeilExit				// Out of range, return original value
	frndint							// Convert the integer to float (It's in range)
	xor		eax,eax					// Clear ax for flags
	fcom	dword ptr [esp+4]		// Compare values for difference (Pop stack)
	fnstsw	ax
	and		eax,0x4100				// Was the new less than the original?
	jz		CeilExit
	fadd	dword ptr [g_fOne]		// Add 1.0f to round up
CeilExit:
	ret		4						// Clean up and exit
	}
}
#elif defined(BURGER_X86) && defined(BURGER_MACOSX)
// __ZN6Burger4CeilEf = float BURGER_API Burger::Ceil(float /* fInput */)
__asm__(
"	.align	4,0x90\n"
".globl __ZN6Burger4CeilEf\n"
"__ZN6Burger4CeilEf:\n"
"	movl	4(%esp),%eax\n"			// Load the value in an integer register
"	flds	4(%esp)\n"				// Load the same value in the FPU
"	andl	$0x7FFFFFFF,%eax\n"		// Mask off the sign
"	cmpl	$0x4B000000,%eax\n"		// Compare to 8388608.0f
"	jae		1f\n"					// Out of range, return original value
"	frndint\n"						// Convert the integer to float (It's in range)
"	xorl	%eax,%eax\n"			// Clear ax for flags
"	fcoms	4(%esp)\n"				// Compare values for difference (Pop stack)
"	fnstsw	%ax\n"
"	andl	$0x4100,%eax\n"			// Was the new less than the original?
"	jz		1f\n"
"	fadds	(__ZN6Burger6g_fOneE)\n"	// Add 1.0f to round up
"1:	ret\n"
);

#else
float BURGER_API Burger::Ceil(float fInput)
{
	// Note : 8388608 is the first floating point number
	// that cannot have a fraction. Therefore this routine can't
	// affect it. I need to handle it special case since I need to
	// convert the float to an int and it's possible to have a float out
	// of bounds. This is a bad thing.

	if (fInput<8388608.0f && fInput>-8388608.0f) {		// Within bounds?
		int iVar = static_cast<int>(fInput);			// Convert to an int
		float fVar = static_cast<float>(iVar);
		if (fVar<fInput) {			// Was there a change?
			++fVar;					// Round up
		}
		fInput = fVar;				// Return the result
	}
	return fInput;					// Return the input (Can't change it)
}
#endif

/*! ************************************

	\brief Return a floating point number rounded up

	If there is any fraction, round the value up to the 
	next whole integer.

	\param dInput 64 bit floating point value to round up to the nearest integer.
	\return The input converted to a 64 bit floating point value using the truth table below.

	\code
	double dResult;
	dResult = Burger::Ceil(1.1);	//2
	dResult = Burger::Ceil(1.95);	//2
	dResult = Burger::Ceil(-1.1);	//-1
	dResult = Burger::Ceil(-1.95);	//-1
	dResult = Burger::Ceil(0.1);	//1
	dResult = Burger::Ceil(0.95);	//1
	dResult = Burger::Ceil(-0.1);	//0
	dResult = Burger::Ceil(-0.95);	//0
	\endcode

	\sa Ceil(float), Floor(double), Round(double), RoundToZero(double), or FixedToIntCeil(Fixed32)

***************************************/

double BURGER_API Burger::Ceil(double dInput)
{
	// Note : 4503599627370496.0 is the first floating point number
	// that cannot have a fraction. Therefore this routine can't
	// affect it. I need to handle it special case since I need to
	// convert the double to an int and it's possible to have a double out
	// of bounds. This is a bad thing.

	if (dInput<4503599627370496.0 && dInput>-4503599627370496.0) {		// Within bounds?
		Int64 iVar = static_cast<Int64>(dInput);			// Convert to an int
		double dVar = static_cast<double>(iVar);
		if (dVar<dInput) {			// Was there a change?
			++dVar;					// Round up
		}
		dInput = dVar;				// Return the result
	}
	return dInput;					// Return the input (Can't change it)
}


/*! ************************************

	\brief Return a floating point number rounded to the nearest integer

	If there is any fraction, round the value to the 
	whole integer.

	\param fInput 32 bit floating point value to round to the nearest integer.
	\return The input converted to a 32 bit floating point value using the truth table below.

	\code
	float fResult;
	fResult = Burger::Round(1.1f);		//1
	fResult = Burger::Round(1.95f);		//2
	fResult = Burger::Round(-1.1f);		//-1
	fResult = Burger::Round(-1.95f);	//-2
	fResult = Burger::Round(0.1f);		//0
	fResult = Burger::Round(0.95f);		//1
	fResult = Burger::Round(-0.1f);		//0
	fResult = Burger::Round(-0.95f);	//-1
	\endcode

	\sa Round(double), Floor(float), Ceil(float), RoundToZero(float), or FixedToIntNearest(Fixed32)

***************************************/

float BURGER_API Burger::Round(float fInput)
{
	// Note : 8388608 is the first floating point number
	// that cannot have a fraction. Therefore this routine can't
	// affect it. I need to handle it special case since I need to
	// convert the float to an int and it's possible to have a float out
	// of bounds. This is a bad thing.
	if (fInput<8388608.0f && fInput>-8388608.0f) {		// Within bounds?
		fInput+=0.5f;
		int iVar = static_cast<int>(fInput);			// Convert to int but rounded!
		float fVar = static_cast<float>(iVar);
		if (fVar>fInput) {	// Did I round up?
			--fVar;			// Fix it
		}
		fInput = fVar;		// Return floored value
	}
	return fInput;
}

/*! ************************************

	\brief Return a floating point number rounded to the nearest integer

	If there is any fraction, round the value to the 
	whole integer.

	\param dInput 64 bit floating point value to round to the nearest integer.
	\return The input converted to a 64 bit floating point value using the truth table below.

	\code
	double dResult;
	dResult = Burger::Round(1.1);	//1
	dResult = Burger::Round(1.95);	//2
	dResult = Burger::Round(-1.1);	//-1
	dResult = Burger::Round(-1.95);	//-2
	dResult = Burger::Round(0.1);	//0
	dResult = Burger::Round(0.95);	//1
	dResult = Burger::Round(-0.1);	//0
	dResult = Burger::Round(-0.95);	//-1
	\endcode

	\sa Round(float), Floor(double), Ceil(double), RoundToZero(double), or FixedToIntNearest(Fixed32)

***************************************/

double BURGER_API Burger::Round(double dInput)
{
	// Note : 4503599627370496.0 is the first floating point number
	// that cannot have a fraction. Therefore this routine can't
	// affect it. I need to handle it special case since I need to
	// convert the double to an int and it's possible to have a double out
	// of bounds. This is a bad thing.
	if (dInput<4503599627370496.0 && dInput>-4503599627370496.0) {		// Within bounds?
		dInput+=0.5;
		Int64 iVar = static_cast<Int64>(dInput);			// Convert to int but rounded!
		double dVar = static_cast<double>(iVar);
		if (dVar>dInput) {	// Did I round up?
			--dVar;			// Fix it
		}
		dInput = dVar;		// Return floored value
	}
	return dInput;
}

/*! ************************************

	\brief Return a floating point number rounded to zero

	If there is any fraction, round the value to the 
	whole integer towards zero

	\param fInput 32 bit floating point value to round to zero.
	\return The input converted to a 32 bit floating point value using the truth table below.

	\code
	float fResult;
	fResult = Burger::RoundToZero(1.1f);	//1
	fResult = Burger::RoundToZero(1.95f);	//1
	fResult = Burger::RoundToZero(-1.1f);	//-1
	fResult = Burger::RoundToZero(-1.95f);	//-1
	fResult = Burger::RoundToZero(0.1f);	//0
	fResult = Burger::RoundToZero(0.95f);	//0
	fResult = Burger::RoundToZero(-0.1f);	//0
	fResult = Burger::RoundToZero(-0.95f);	//0
	\endcode

	\sa RoundToZero(double), Floor(float), Ceil(float), Round(float), or FixedToInt(Fixed32)

***************************************/

float BURGER_API Burger::RoundToZero(float fInput)
{
	if (fInput<0.0f) {
		return Ceil(fInput);
	} else {
		return Floor(fInput);
	}
}

/*! ************************************

	\brief Return a floating point number rounded to zero

	If there is any fraction, round the value to the 
	whole integer towards zero

	\param dInput 64 bit floating point value to round to zero.
	\return The input converted to a 64 bit floating point value using the truth table below.

	\code
	double dResult;
	dResult = Burger::RoundToZero(1.1);		//1
	dResult = Burger::RoundToZero(1.95);	//1
	dResult = Burger::RoundToZero(-1.1);	//-1
	dResult = Burger::RoundToZero(-1.95);	//-1
	dResult = Burger::RoundToZero(0.1);		//0
	dResult = Burger::RoundToZero(0.95);	//0
	dResult = Burger::RoundToZero(-0.1);	//0
	dResult = Burger::RoundToZero(-0.95);	//0
	\endcode

	\sa RoundToZero(float), Floor(double), Ceil(double), Round(double), or FixedToInt(Fixed32)

***************************************/

double BURGER_API Burger::RoundToZero(double dInput)
{
	if (dInput<0.0) {
		return Ceil(dInput);
	} else {
		return Floor(dInput);
	}
}

/*! ************************************

	\brief Modulo the range of the input so that -BURGER_PI <= fInput < BURGER_PI

	Wrap around a value in radians into remain in the valid range of radians.

	\note Due to 32 bit precision, numbers larger than 16*pi will likely contain rounding errors

	\param fInput 32 bit floating point value in radians
	\return The input converted to a 32 bit floating point value wrapped to fit within.

	\sa ModuloRadians(double), Sin(float), Cos(float)

***************************************/

#if defined(BURGER_X86) && (defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || defined(BURGER_MSVC))
BURGER_DECLSPECNAKED float BURGER_API Burger::ModuloRadians(float /* fInput */)
{
	BURGER_ASM {
	fld		dword ptr [esp+4]		// Load into the FPU
	fld		dword ptr [g_fReciprocalPi2]	// Load in 1/ 2Pi
	fmul	st(0),st(1)				// Multiply (Really fInput/2Pi)
	fadd	dword ptr [g_fHalf]		// Add half for rounding
	fst		dword ptr [esp+4]		// Round to nearest in 24 bit to force consistent precision
	frndint							// Convert to integer
	fcom	dword ptr [esp+4]		// Compare the two and get rid of the prerounded
	fnstsw	ax
	test	ah,0x41					// ble
	jne		ModuloRadiansFExit		// Did it round up?
	fsub	dword ptr [g_fOne]		// Fixup
ModuloRadiansFExit:
	fmul	dword ptr [g_fPi2]		// Mul by 2 pi
	fsubp	st(1),st(0)				// Subtract and clean up
	ret		4						// Clean up and exit
	}
}
#elif defined(BURGER_X86) && defined(BURGER_MACOSX)
// __ZN6Burger13ModuloRadiansEf = float BURGER_API Burger::ModuloRadians(float /* fInput */)
__asm__(
"	.align	4,0x90\n"
"	.globl __ZN6Burger13ModuloRadiansEf\n"
"__ZN6Burger13ModuloRadiansEf:\n"
"	movss	4(%esp),%xmm0\n"				// Load into the FPU
"	movss	(__ZN6Burger16g_fReciprocalPi2E),%xmm2\n"	// Load in 1/ 2Pi
"	mulss	%xmm0,%xmm2\n"					// Multiply (Really fInput/2Pi)
"	addss	(__ZN6Burger7g_fHalfE),%xmm2\n"	// Add half for rounding
"	cvttss2si	%xmm2,%eax\n"				// Convert to integer
"	cvtsi2ss	%eax,%xmm1\n"
"	ucomiss	%xmm2,%xmm1\n"					// Compare the two and get rid of the prerounded
"	jbe		1f\n"							// Did it round up?
"	subss	(__ZN6Burger6g_fOneE),%xmm1\n"	// Fixup
"1:\n"
"	mulss	(__ZN6Burger6g_fPi2E),%xmm1\n"	// Mul by 2 pi
"	subss	%xmm1,%xmm0\n"					// Subtract and clean up
"	movss	%xmm0,4(%esp)\n"				// Return in st(0)
"	flds	4(%esp)\n"
"	ret\n"
);
#elif defined(BURGER_POWERPC) && defined(BURGER_MACOSX)
//
// Due to the inaccuracy of fnmsubs, the function is hand coded
// to pass all unit tests
//
// __ZN6Burger13ModuloRadiansEf = float BURGER_API Burger::ModuloRadians(float /* fInput */)
__asm__(
"	.align	2,0\n"
"	.globl __ZN6Burger13ModuloRadiansEf\n"
"__ZN6Burger13ModuloRadiansEf:\n"
"	lis		r3,ha16(__ZN6Burger16g_fReciprocalPi2E)\n"	// Fetch the pointers
"	lis		r6,0x4330\n"			// Create 0x4330000080000000 for integer to float conversions
"	lis		r4,ha16(__ZN6Burger7g_fHalfE)\n"
"	lis		r7,0x8000\n"
"	lis		r5,ha16(__ZN6Burger6g_fPi2E)\n"
"	lfs		f3,lo16(__ZN6Burger16g_fReciprocalPi2E)(r3)\n"			// Load in 1/ 2Pi
"	lfs		f4,lo16(__ZN6Burger7g_fHalfE)(r4)\n"			// Load in 0.5f
"	lfs		f5,lo16(__ZN6Burger6g_fPi2E)(r5)\n"			// Load in Pi2
"	fmadds	f3,f1,f3,f4\n"			// (fInput*g_fReciprocalPi2)+g_fHalf			
"	stw		r6,-16(r1)\n"				// Store 0x4330000080000000
"	stw		r7,-12(r1)\n"
"	lfd		f2,-16(r1)\n"				// Load 0x4330000080000000 in FPU
"	fctiwz	f0,f3\n"					// Convert to integer
"	stfd	f0,-8(r1)\n"				// Store the integer in memory (64 bit)
"	fneg	f5,f5\n"					// Negate Pi2
"	lwz		r0,-4(r1)\n"				// Extract the low word
"	xor		r0,r0,r7\n"				// Flip the bit
"	stw		r6,-8(r1)\n"			// Create a fake double
"	stw		r0,-4(r1)\n"				// Store the integer
"	lfd		f0,-8(r1)\n"				// Load the rounded double
"	fsubs	f2,f0,f2\n"				// Complete the int to float conversion
"	fcmpu	cr0,f2,f3\n"				// Compare the two and get rid of the prerounded
"	ble		1f\n"
"	lis		r3,ha16(__ZN6Burger6g_fOneE)\n"			// Load 1.0f
"	lfs		f0,lo16(__ZN6Burger6g_fOneE)(r3)\n"
"	fsubs	f2,f2,f0\n"				// Fixup
"1:\n"
// Don't use fnmsubs, it doesn't handle the sign bit properly
"	fmadds	f1,f2,f5,f1\n"			//  (fVar*-g_fPi2) + fInput		
"	blr\n"
);
#elif defined(BURGER_MAC) && defined(BURGER_POWERPC)
//
// Due to the inaccuracy of fnmsubs, the function is hand coded
// to pass all unit tests
//
BURGER_ASM float BURGER_API Burger::ModuloRadians(float /* fInput */)
{
	lwz		r3,g_fReciprocalPi2(RTOC)	// Fetch the pointers
	lis		r6,0x4330				// Create 0x4330000080000000 for integer to float conversions
	lwz		r4,g_fHalf(RTOC)
	lis		r7,0x8000
	lwz		r5,g_fPi2(RTOC)
	lfs		fp3,0(r3)				// Load in 1/ 2Pi
	lfs		fp4,0(r4)				// Load in 0.5f
	lfs		fp5,0(r5)				// Load in Pi2
	fmadds	fp3,fp1,fp3,fp4			// (fInput*g_fReciprocalPi2)+g_fHalf			
	stw		r6,-16(SP)				// Store 0x4330000080000000
	stw		r7,-12(SP)
	lfd		fp2,-16(SP)				// Load 0x4330000080000000 in FPU
	fctiwz	fp0,fp3					// Convert to integer
	stfd	fp0,-8(SP)				// Store the integer in memory (64 bit)
	fneg	fp5,fp5					// Negate Pi2
	lwz		r0,-4(SP)				// Extract the low word
	xor		r0,r0,r7				// Flip the bit
	stw		r6,-8(SP)				// Create a fake double
	stw		r0,-4(SP)				// Store the integer
	lfd		fp0,-8(SP)				// Load the rounded double
	fsubs	fp2,fp0,fp2				// Complete the int to float conversion
	fcmpu	cr0,fp2,fp3				// Compare the two and get rid of the prerounded
	ble		FModule1
	lwz		r3,g_fOne(RTOC)			// Load 1.0f
	lfs		fp0,0(r3)
	fsubs	fp2,fp2,fp0				// Fixup
FModule1:
	// Don't use fnmsubs, it doesn't handle the sign bit properly
	fmadds	fp1,fp2,fp5,fp1			//  (fVar*-g_fPi2) + fInput
	blr
}
#else
float BURGER_API Burger::ModuloRadians(float fInput)
{
	// Produce Input/(BURGER_PI*2)
	float fTemp = (fInput*g_fReciprocalPi2)+0.5f;
	// Convert to int but rounded!
	int iVar = static_cast<int>(fTemp);
	float fVar = static_cast<float>(iVar);
	if (fVar>fTemp) {	// Did I round up?
		--fVar;			// Fix it
	}
	// Get the whole number and remove it
	return fInput - (fVar*g_fPi2);
}
#endif

/*! ************************************

	\brief Modulo the range of the input so that -BURGER_PI <= fInput < BURGER_PI

	Wrap around a value in radians into remain in the valid range of radians.

	\note Due to 64 bit precision, numbers larger than 1024*pi will likely contain rounding errors

	\param dInput 64 bit floating point value in radians
	\return The input converted to a 64 bit floating point value wrapped to fit within.

	\sa ModuloRadians(float), Sin(double), Cos(double)

***************************************/

#if defined(BURGER_X86) && (defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || defined(BURGER_MSVC))
BURGER_DECLSPECNAKED double BURGER_API Burger::ModuloRadians(double /* dInput */)
{
	BURGER_ASM {
	fld		qword ptr [esp+4]		// Load into the FPU
	fld		qword ptr [g_dReciprocalPi2]	// Load in 1/2Pi
	fmul	st(0),st(1)				// Multiply
	fadd	qword ptr [g_dHalf]		// Add half for rounding
	fst		qword ptr [esp+4]		// Store at double precision
	frndint							// Remove fraction
	fcom	qword ptr [esp+4]		// Compare the two and get rid of the prerounded
	fnstsw	ax
	test	ah,0x41 
	jnz		ModuloRadiansFExit		// Did it round up?
	fsub	qword ptr [g_dOne]		// Fixup
ModuloRadiansFExit:
	fmul	qword ptr [g_dPi2]		// Mul by 2 pi
	fsubp	st(1),st(0)				// Subtract and clean up
	ret		8						// Clean up and exit
	}
}
#elif defined(BURGER_X86) && defined(BURGER_MACOSX)
// __ZN6Burger13ModuloRadiansEd = double BURGER_API Burger::ModuloRadians(double /* fInput */)
__asm__(
"	.align	4,0x90\n"
"	.globl __ZN6Burger13ModuloRadiansEd\n"
"__ZN6Burger13ModuloRadiansEd:\n"
"	movsd	4(%esp),%xmm0\n"					// Load into the FPU
"	movsd	(__ZN6Burger16g_dReciprocalPi2E),%xmm2\n"	// Load in 1/2Pi
"	mulsd	%xmm0,%xmm2\n"						// Multiply
"	addsd	(__ZN6Burger7g_dHalfE),%xmm2\n"		// Add half for rounding
"	cvttsd2si	%xmm2,%eax\n"					// Remove fraction
"	cvtsi2sdl	%eax,%xmm1\n"
"	ucomisd	%xmm2,%xmm1\n"						// Compare the two and get rid of the prerounded
"	jbe		1f\n"								// Did it round up?
"	subsd	(__ZN6Burger6g_dOneE),%xmm1\n"		// Fixup
"1:\n"
"	mulsd	(__ZN6Burger6g_dPi2E),%xmm1\n"		// Mul by 2 pi
"	subsd	%xmm1,%xmm0\n"						// Subtract and clean up
"	movsd	%xmm0,4(%esp)\n"
"	fldl	4(%esp)\n"							// Return in st(0)
"	ret\n"
);
#elif defined(BURGER_POWERPC) && defined(BURGER_MACOSX)
//
// Due to the inaccuracy of fnmsubs, the function is hand coded
// to pass all unit tests
//
// __ZN6Burger13ModuloRadiansEd = double BURGER_API Burger::ModuloRadians(double /* fInput */)
__asm__(
"	.align	2,0\n"
"	.globl __ZN6Burger13ModuloRadiansEd\n"
"__ZN6Burger13ModuloRadiansEd:\n"
"	lis		r3,ha16(__ZN6Burger16g_dReciprocalPi2E)\n"	// Fetch the pointers
"	lis		r6,0x4330\n"			// Create 0x4330000080000000 for integer to float conversions
"	lis		r4,ha16(__ZN6Burger7g_dHalfE)\n"
"	lis		r7,0x8000\n"
"	lis		r5,ha16(__ZN6Burger6g_dPi2E)\n"
"	lfd		f3,lo16(__ZN6Burger16g_dReciprocalPi2E)(r3)\n"	// Load in 1/ 2Pi
"	lfd		f4,lo16(__ZN6Burger7g_dHalfE)(r4)\n"		// Load in 0.5f
"	fmul	f3,f1,f3\n"				// (fInput*g_fReciprocalPi2)
"	lfd		f5,lo16(__ZN6Burger6g_dPi2E)(r5)\n"			// Load in Pi2
"	fadd	f3,f3,f4\n"				// (fInput*g_fReciprocalPi2)+g_fHalf
		
// fmadd doesn't handle sign properly so it failed the unit test
// Used explicit fmul and fadd to get the accuracy
//"	fmadd	f3,f1,f3,f4\n"			// (fInput*g_fReciprocalPi2)+g_fHalf			
"	stw		r6,-16(r1)\n"				// Store 0x4330000080000000
"	stw		r7,-12(r1)\n"
"	lfd		f2,-16(r1)\n"				// Load 0x4330000080000000 in FPU
"	fctiwz	f0,f3\n"					// Convert to integer
"	stfd	f0,-8(r1)\n"				// Store the integer in memory (64 bit)
"	fneg	f5,f5\n"					// Negate Pi2
"	lwz		r0,-4(r1)\n"				// Extract the low word
"	xor		r0,r0,r7\n"				// Flip the bit
"	stw		r6,-8(r1)\n"			// Create a fake double
"	stw		r0,-4(r1)\n"				// Store the integer
"	lfd		f0,-8(r1)\n"				// Load the rounded double
"	fsub	f2,f0,f2\n"				// Complete the int to float conversion
"	fcmpu	cr0,f2,f3\n"				// Compare the two and get rid of the prerounded
"	ble		1f\n"
"	lis		r3,ha16(__ZN6Burger6g_dOneE)\n"			// Load 1.0f
"	lfd		f0,lo16(__ZN6Burger6g_dOneE)(r3)\n"
"	fsub	f2,f2,f0\n"				// Fixup
"1:\n"
// Don't use fnmsubs, it doesn't handle the sign bit properly
"	fmadd	f1,f2,f5,f1\n"			//  (fVar*-g_fPi2) + fInput		
"	blr\n"
);
#elif defined(BURGER_METROWERKS) && defined(BURGER_POWERPC)
//
// Due to the inaccuracy of fnmsubs, the function is hand coded
// to pass all unit tests
//
BURGER_ASM double BURGER_API Burger::ModuloRadians(double /* dInput */)
{
	lwz		r3,g_dReciprocalPi2(RTOC)	// Fetch the pointers
	lis		r6,0x4330				// Create 0x4330000080000000 for integer to float conversions
	lwz		r4,g_dHalf(RTOC)
	lis		r7,0x8000
	lwz		r5,g_dPi2(RTOC)
	lfd		fp3,0(r3)				// Load in 1/ 2Pi
	lfd		fp4,0(r4)				// Load in 0.5f
	fmul	fp3,fp1,fp3				// (fInput*g_fReciprocalPi2)
	lfd		fp5,0(r5)				// Load in Pi2
	fadd	fp3,fp3,fp4				// (fInput*g_fReciprocalPi2)+g_fHalf

// fmadd doesn't handle sign properly so it failed the unit test
// Used explicit fmul and fadd to get the accuracy
//	fmadd	fp3,fp1,fp3,fp4			// (fInput*g_fReciprocalPi2)+g_fHalf
	stw		r6,-16(SP)				// Store 0x4330000080000000
	stw		r7,-12(SP)
	lfd		fp2,-16(SP)				// Load 0x4330000080000000 in FPU
	fctiwz	fp0,fp3					// Convert to integer
	stfd	fp0,-8(SP)				// Store the integer in memory (64 bit)
	fneg	fp5,fp5					// Negate Pi2
	lwz		r0,-4(SP)				// Extract the low word
	xor		r0,r0,r7				// Flip the bit
	stw		r6,-8(SP)				// Create a fake double
	stw		r0,-4(SP)				// Store the integer
	lfd		fp0,-8(SP)				// Load the rounded double
	fsub	fp2,fp0,fp2				// Complete the int to float conversion
	fcmpu	cr0,fp2,fp3				// Compare the two and get rid of the prerounded
	ble		FModule1
	lwz		r3,g_dOne(RTOC)			// Load 1.0f
	lfd		fp0,0(r3)
	fsub	fp2,fp2,fp0				// Fixup
FModule1:
	// Don't use fnmsubs, it doesn't handle the sign bit properly
	fmadd	fp1,fp2,fp5,fp1			//  (fVar*-g_fPi2) + fInput
	blr
}
#else
double BURGER_API Burger::ModuloRadians(double dInput)
{
	// Produce Input/(BURGER_PI*2)
	double dTemp = (dInput*g_dReciprocalPi2)+0.5;
	// 
	Int64 iVar = static_cast<Int64>(dTemp);			// Convert to int but rounded!
	double dVar = static_cast<double>(iVar);
	if (dVar>dTemp) {	// Did I round up?
		--dVar;			// Fix it
	}
	// Get the whole number and remove it
	return dInput - (dVar*g_dPi2);
}
#endif

/*! ************************************

	\fn float BURGER_API Burger::Sin(float fInput)
	\brief Return the sine from radians

	This is a replacement of sinf() from the C++ libraries

	To calculate sine...
	sin(x) = x - ((x^3)/ 3!) + ((x^5)/ 5!) - ((x^7)/ 7!) + ((x^9)/ 9!) ...
	Repeat the pattern by reversing the sign of the addition and adding
	2 to the factors for every iteration

	This function will perform 10 iterations from 3,5,7,...,23

	\note ! is factoral so 3! = 1*2*3, and 5! = 1*2*3*4*5

	\param fInput Value in Radians
	\return Sine of fInput

	\sa Sin(double) or Cos(float)

***************************************/

// 3!, 5!, to 23!
//static const Burger::Word32ToFloat g_fSineFactors[11] = {
//	{{0xC0C00000}},{{0x42F00000}},{{0xC59D8000}},{{0x48B13000}},{{0xCC184540}},
//	{{0x4FB99466}},{{0xD3983BBC}},{{0x57A1BF77}},{{0xDBD815CA}},{{0x603141DF}},
//	{{0xE4AF2E1A}}
//};

// 1/3!, 1/5!, to 1/23!
#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)
static const
#endif
Burger::Word32ToFloat g_fInverseSineFactors[11] = {
	{{0xBE2AAAAB}},{{0x3C088889}},{{0xB9500D01}},{{0x3638EF1D}},{{0xB2D7322B}},
	{{0x2F309231}},{{0xAB573F9F}},{{0x274A963C}},{{0xA317A4DA}},{{0x1EB8DC78}},
	{{0x9A3B0DA1}}
};

#if defined(BURGER_X86) && (defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || defined(BURGER_MSVC)) 
BURGER_DECLSPECNAKED float BURGER_API Burger::Sin(float /* fInput */)
{
	BURGER_ASM {	
	fld		dword ptr [esp+4]		// Load into the FPU
	fld		dword ptr [g_fReciprocalPi2]	// Load in 1/2Pi
	fmul	st(0),st(1)				// Multiply
	fadd	dword ptr [g_fHalf]		// Add half for rounding
	fst		dword ptr [esp+4]		// Round to nearest in 24 bit to force consistent precision
	frndint							// Convert to integer
	fcom	dword ptr [esp+4]		// Compare the two and get rid of the prerounded
#if defined(BURGER_WATCOM)
	nop								// There is an alignment bug in Watcom C. Remove this and [g_fInverseSineFactors+8] vanishes
#endif
	fnstsw	ax
	test	ah,0x41					// ble
	jne		SinFExit				// Did it round up?
	fsub	dword ptr [g_fOne]		// Fixup
SinFExit:
	fmul	dword ptr [g_fPi2]		// Mul by 2 pi
	fsubp	st(1),st(0)				// Subtract and clean up

	fld		st(0)					// Make a copy for squaring
	fmul	st(0),st(1)				// Square the input
	fld		st(0)
	fmul	st(0),st(2)				// Create the Power of 3
	fld		dword ptr [g_fInverseSineFactors]	// Start iterating 
	fmul	st(0),st(1)				// fInput*(1/3!)
	faddp	st(3),st(0)				// fResult + fInput*(1/3!)
	fmul	st(0),st(1)				// Up the power by 2
	fld		dword ptr [g_fInverseSineFactors+4] 
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseSineFactors+8]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseSineFactors+12]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseSineFactors+16]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseSineFactors+20]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseSineFactors+24]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseSineFactors+28]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseSineFactors+32]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseSineFactors+36]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmulp	st(1),st(0)
	fmul	dword ptr [g_fInverseSineFactors+40]
	faddp	st(1),st(0)
	ret		4						// Clean up and exit
	}
}
#elif defined(BURGER_X86) && defined(BURGER_MACOSX)
// __ZN6Burger3SinEf = float BURGER_API Burger::Sin(float fInput)
__asm__(".globl __ZN6Burger3SinEf\n"
"__ZN6Burger3SinEf:\n"
"	flds	4(%esp)\n"				// Load into the FPU
"	flds	(__ZN6Burger16g_fReciprocalPi2E)\n"	// Load in 1/2Pi
"	fmul	%st(1),%st(0)\n"			// Multiply (Really fInput/2Pi)
"	fadds	(__ZN6Burger7g_fHalfE)\n"	// Add half for rounding
"	fsts	4(%esp)\n"					// Round to nearest in 24 bit to force consistent precision
"	frndint\n"							// Convert to integer
"	fcoms	4(%esp)\n"					// Compare the two and get rid of the prerounded
"	fnstsw	%ax\n"
"	testb	$0x41,%ah\n"				// ble
"	jne		1f\n"						// Did it round up?
"	fsubs	(__ZN6Burger6g_fOneE)\n"	// Fixup
"1:\n"
"	fmuls	(__ZN6Burger6g_fPi2E)\n"	// Mul by 2 pi
"	fsubrp	%st(1),%st(0)\n"			// Subtract and clean up
"	fld		%st(0)\n"					// Make a copy for squaring
"	fmul	%st(1),%st(0)\n"			// Square the input
"	fld		%st(0)\n"
"	fmul	%st(2),%st(0)\n"			// Create the Power of 3
"	flds	(_g_fInverseSineFactors)\n"	// Start iterating
"	fmul	%st(1),%st(0)\n"			// fInput*(1/3!)
"	faddp	%st(0),%st(3)\n"			// fResult + fInput*(1/3!)
"	fmul	%st(1),%st(0)\n"			// Up the power by 2
"	flds	(_g_fInverseSineFactors+4)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseSineFactors+8)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseSineFactors+12)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseSineFactors+16)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseSineFactors+20)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseSineFactors+24)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseSineFactors+28)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseSineFactors+32)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseSineFactors+36)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmulp	%st(0),%st(1)\n"
"	fmuls	(_g_fInverseSineFactors+40)\n"
"	faddp	%st(0),%st(1)\n"
"	ret\n"						// Clean up and exit
);

#else
float BURGER_API Burger::Sin(float fInput)
{
	// Start by rounding the radians to reduce the chance
	// of floating point rounding errors
	fInput = ModuloRadians(fInput);

	// To calculate sine...
	// Note: ! is factoral so 3! = 1*2*3, and 5! = 1*2*3*4*5
	// sin(x) = x - ((x^3)/ 3!) + ((x^5)/ 5!) - ((x^7)/ 7!) + ((x^9)/ 9!)
	// Repeat the pattern by reversing the sign of the addition and adding
	// 2 for the square for every iteration

	float fInput2 = (fInput*fInput);

	float fInputFactorial = (fInput2*fInput);

	//- ((x^3)/ 3!)
	float fResult = (fInputFactorial*g_fInverseSineFactors[0])+fInput;
	//+ ((x^5)/ 5!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseSineFactors[1])+fResult;
	//- ((x^7)/ 7!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseSineFactors[2])+fResult;
	//+ ((x^9)/ 9!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseSineFactors[3])+fResult;
	//- ((x^11)/ 11!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseSineFactors[4])+fResult;
	//+ ((x^13)/ 13!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseSineFactors[5])+fResult;
	//- ((x^15)/ 15!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseSineFactors[6])+fResult;
	//+ ((x^17)/ 17!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseSineFactors[7])+fResult;
	//- ((x^19)/ 19!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseSineFactors[8])+fResult;
	//+ ((x^21)/ 21!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseSineFactors[9])+fResult;
	//- ((x^23)/ 23!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseSineFactors[10])+fResult;
	return fResult;
}
#endif

/*! ************************************

	\fn double BURGER_API Burger::Sin(double dInput)
	\brief Return the sine from radians

	This is a replacement of sin() from the C++ libraries

	\param dInput Value in Radians
	\return Sine of dInput

	\sa Sin(float) or Cos(double)

***************************************/

// 3!, 5!, to 23!
//static const Burger::Word64ToDouble g_dSineFactors[11] = {
//	{{0xC018000000000000ULL}},{{0x405E000000000000ULL}},{{0xC0B3B00000000000ULL}},{{0x4116260000000000ULL}},{{0xC18308A800000000ULL}},
//	{{0x41F7328CC0000000ULL}},{{0xC273077775800000ULL}},{{0x42F437EEECD80000ULL}},{{0xC37B02B930689000ULL}},{{0x4406283BE9B5C620ULL}},
//	{{0xC495E5C335F8A4CEULL}}
//};

// 1/3!, 1/5!, to 1/23!
static const Burger::Word64ToDouble g_dInverseSineFactors[11] = {
	{{0xBFC5555555555555ULL}},{{0x3F81111111111111ULL}},{{0xBF2A01A01A01A01AULL}},{{0x3EC71DE3A556C734ULL}},{{0xBE5AE64567F544E4ULL}},
	{{0x3DE6124613A86D09ULL}},{{0xBD6AE7F3E733B81FULL}},{{0x3CE952C77030AD4AULL}},{{0xBC62F49B46814157ULL}},{{0x3BD71B8EF6DCF572ULL}},
	{{0xBB4761B413163819ULL}}
};

#if defined(BURGER_X86) && (defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || defined(BURGER_MSVC)) 
BURGER_DECLSPECNAKED double BURGER_API Burger::Sin(double /* dInput */)
{
	BURGER_ASM {
	fld		qword ptr [esp+4]		// Load into the FPU
	fld		qword ptr [g_dReciprocalPi2]	// Load in 1/2Pi
	fmul	st(0),st(1)				// Multiply
	fadd	qword ptr [g_dHalf]		// Add half for rounding
	fst		qword ptr [esp+4]		// Store at double precision
	frndint							// Remove fraction
	fcom	qword ptr [esp+4]		// Compare the two and get rid of the prerounded
#if defined(BURGER_WATCOM)
	nop								// There is an alignment bug in Watcom C. Remove this and [g_dInverseSineFactors+16] vanishes
#endif
	fnstsw	ax
	test	ah,0x41 
	jnz		SinFExit				// Did it round up?
	fsub	qword ptr [g_dOne]		// Fixup
SinFExit:
	fmul	qword ptr [g_dPi2]		// Mul by 2 pi
	fsubp	st(1),st(0)				// Subtract and clean up

	fld		st(0)					// Make a copy for squaring
	fmul	st(0),st(1)				// Square the input
	fld		st(0)
	fmul	st(0),st(2)				// Create the Power of 3
	fld		qword ptr [g_dInverseSineFactors]	// Start iterating 
	fmul	st(0),st(1)				// fInput*(1/3!)
	faddp	st(3),st(0)				// fResult + fInput*(1/3!)
	fmul	st(0),st(1)				// Up the power by 2
	fld		qword ptr [g_dInverseSineFactors+8] 
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseSineFactors+16]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseSineFactors+24]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseSineFactors+32]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseSineFactors+40]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseSineFactors+48]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseSineFactors+56]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseSineFactors+64]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseSineFactors+72]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmulp	st(1),st(0)
	fmul	qword ptr [g_dInverseSineFactors+80]
	faddp	st(1),st(0)
	ret		8						// Clean up and exit
	}
}
// No MacOSX 32 bit intel version assembly version, since SSE builds quite nicely
#else
double BURGER_API Burger::Sin(double dInput)
{
	// Start by rounding the radians to reduce the chance
	// of floating point rounding errors
	dInput = ModuloRadians(dInput);

	// To calculate sine...
	// Note: ! is factoral so 3! = 1*2*3, and 5! = 1*2*3*4*5
	// sin(x) = x - ((x^3)/ 3!) + ((x^5)/ 5!) - ((x^7)/ 7!) + ((x^9)/ 9!)
	// Repeat the pattern by reversing the sign of the addition and adding
	// 2 for the square for every iteration

	double dInput2 = (dInput*dInput);

	double dInputFactorial = (dInput2*dInput);

	//- ((x^3)/ 3!)
	double dResult = (dInputFactorial*g_dInverseSineFactors[0])+dInput;
	//+ ((x^5)/ 5!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseSineFactors[1])+dResult;
	//- ((x^7)/ 7!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseSineFactors[2])+dResult;
	//+ ((x^9)/ 9!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseSineFactors[3])+dResult;
	//- ((x^11)/ 11!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseSineFactors[4])+dResult;
	//+ ((x^13)/ 13!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseSineFactors[5])+dResult;
	//- ((x^15)/ 15!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseSineFactors[6])+dResult;
	//+ ((x^17)/ 17!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseSineFactors[7])+dResult;
	//- ((x^19)/ 19!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseSineFactors[8])+dResult;
	//+ ((x^21)/ 21!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseSineFactors[9])+dResult;
	//- ((x^23)/ 23!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseSineFactors[10])+dResult;
	return dResult;
}
#endif

/*! ************************************

	\fn float BURGER_API Burger::Cos(float fInput)
	\brief Return the cosine from radians

	This is a replacement of cosf() from the C++ libraries

	\param fInput Value in Radians
	\return Cosine of fInput

	\sa Cos(double) or Sin(float)

***************************************/

// 2!, 4!, to 22!
//static const Burger::Word32ToFloat g_fCosineFactors[11] = {
//	{{0xC0000000}},{{0x41C00000}},{{0xC4340000}},{{0x471D8000}},{{0xCA5D7C00}},
//	{{0x4DE467E0}},{{0xD1A261D9}},{{0x55983BBC}},{{0xD9B5F766}},{{0x5E070D9E}},
//	{{0xE273BA93}}
//};

// 1/2!, 1/4!, to 1/22!
#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)
static const
#endif
Burger::Word32ToFloat g_fInverseCosineFactors[11] = {
	{{0xBF000000}},{{0x3D2AAAAB}},{{0xBAB60B61}},{{0x37D00D01}},{{0xB493F27E}},
	{{0x310F76C7}},{{0xAD49CBA5}},{{0x29573F9F}},{{0xA53413C3}},{{0x20F2A15D}},
	{{0x9C8671CB}}
};

#if defined(BURGER_X86) && (defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || defined(BURGER_MSVC)) 
BURGER_DECLSPECNAKED float BURGER_API Burger::Cos(float /* fInput */)
{
	BURGER_ASM {
	fld1
	fld		dword ptr [esp+4]		// Load into the FPU
	fld		dword ptr [g_fReciprocalPi2]	// Load in 1/2Pi
	fmul	st(0),st(1)				// Multiply
	fadd	dword ptr [g_fHalf]		// Add half for rounding
	fst		dword ptr [esp+4]		// Round to nearest in 24 bit to force consistent precision
	frndint							// Convert to integer
	fcom	dword ptr [esp+4]		// Compare the two and get rid of the prerounded
	fnstsw	ax
	test	ah,0x41					// ble
	jne		CosinFExit				// Did it round up?
	fsub	dword ptr [g_fOne]		// Fixup
CosinFExit:
	fmul	dword ptr [g_fPi2]		// Mul by 2 pi
	fsubp	st(1),st(0)				// Subtract and clean up

	fmul	st(0),st(0)				// Square the input
	fld		st(0)					// Copy the Power of 2			
	fld		dword ptr [g_fInverseCosineFactors]	// Start iterating 
	fmul	st(0),st(1)				// fInput*(1/2!)
	faddp	st(3),st(0)				// fResult + fInput*(1/2!)
	fmul	st(0),st(1)				// Up the power by 2
	fld		dword ptr [g_fInverseCosineFactors+4] 
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseCosineFactors+8]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseCosineFactors+12]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseCosineFactors+16]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseCosineFactors+20]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseCosineFactors+24]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseCosineFactors+28]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseCosineFactors+32]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		dword ptr [g_fInverseCosineFactors+36]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmulp	st(1),st(0)
	fmul	dword ptr [g_fInverseCosineFactors+40]
	faddp	st(1),st(0)
	ret		4						// Clean up and exit
	}
}
#elif defined(BURGER_X86) && defined(BURGER_MACOSX)
// __ZN6Burger3CosEf = float BURGER_API Burger::Cos(float fInput)
__asm__(".globl __ZN6Burger3CosEf\n"
"__ZN6Burger3CosEf:\n"
"	flds	(__ZN6Burger6g_fOneE)\n"	// Initial 1
"	flds	4(%esp)\n"					// Load into the FPU
"	flds	(__ZN6Burger16g_fReciprocalPi2E)\n"	// Load in 1/2Pi
"	fmul	%st(1),%st(0)\n"			// Multiply (Really fInput/2Pi)
"	fadds	(__ZN6Burger7g_fHalfE)\n"	// Add half for rounding
"	fsts	4(%esp)\n"					// Round to nearest in 24 bit to force consistent precision
"	frndint\n"							// Convert to integer
"	fcoms	4(%esp)\n"					// Compare the two and get rid of the prerounded
"	fnstsw	%ax\n"
"	testb	$0x41,%ah\n"				// ble
"	jne		1f\n"						// Did it round up?
"	fsubs	(__ZN6Burger6g_fOneE)\n"	// Fixup
"1:\n"
"	fmuls	(__ZN6Burger6g_fPi2E)\n"	// Mul by 2 pi
"	fsubrp	%st(1),%st(0)\n"			// Subtract and clean up
"	fmul	%st(0),%st(0)\n"			// Square the input
"	fld		%st(0)\n"					// Copy the Power of 2
"	flds	(_g_fInverseCosineFactors)\n"	// Start iterating
"	fmul	%st(1),%st(0)\n"			// fInput*(1/2!)
"	faddp	%st(0),%st(3)\n"			// fResult + fInput*(1/2!)
"	fmul	%st(1),%st(0)\n"			// Up the power by 2
"	flds	(_g_fInverseCosineFactors+4)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseCosineFactors+8)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseCosineFactors+12)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseCosineFactors+16)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseCosineFactors+20)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseCosineFactors+24)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseCosineFactors+28)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseCosineFactors+32)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmul	%st(1),%st(0)\n"
"	flds	(_g_fInverseCosineFactors+36)\n"
"	fmul	%st(1),%st(0)\n"
"	faddp	%st(0),%st(3)\n"
"	fmulp	%st(0),%st(1)\n"
"	fmuls	(_g_fInverseCosineFactors+40)\n"
"	faddp	%st(0),%st(1)\n"
"	ret\n"						// Clean up and exit
);

#else
float BURGER_API Burger::Cos(float fInput)
{
	// Start by rounding the radians to reduce the chance
	// of floating point rounding errors
	fInput = ModuloRadians(fInput);

	// To calculate cosine...
	// Note: ! is factoral so 2! = 1*2, and 4! = 1*2*3*4
	// cos(x) = 1 - ((x^2)/ 2!) + ((x^4)/ 4!) - ((x^6)/ 6!) + ((x^8)/ 8!)
	// Repeat the pattern by reversing the sign of the addition and adding
	// 2 for the square for every iteration

	float fInput2 = (fInput*fInput);

	//- ((x^2)/ 2!)
	float fResult = (fInput2*g_fInverseCosineFactors[0])+1.0f;
	//+ ((x^4)/ 4!)
	float fInputFactorial = fInput2*fInput2;
	fResult = (fInputFactorial*g_fInverseCosineFactors[1])+fResult;
	//- ((x^6)/ 6!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseCosineFactors[2])+fResult;
	//+ ((x^8)/ 8!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseCosineFactors[3])+fResult;
	//- ((x^10)/ 10!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseCosineFactors[4])+fResult;
	//+ ((x^12)/ 12!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseCosineFactors[5])+fResult;
	//- ((x^14)/ 14!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseCosineFactors[6])+fResult;
	//+ ((x^16)/ 16!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseCosineFactors[7])+fResult;
	//- ((x^18)/ 18!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseCosineFactors[8])+fResult;
	//+ ((x^20)/ 20!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseCosineFactors[9])+fResult;
	//- ((x^22)/ 22!)
	fInputFactorial = fInputFactorial*fInput2;
	fResult = (fInputFactorial*g_fInverseCosineFactors[10])+fResult;
	return fResult;
}
#endif

/*! ************************************

	\fn double BURGER_API Burger::Cos(double dInput)
	\brief Return the cosine from radians

	This is a replacement of cos() from the C++ libraries

	\param dInput Value in Radians
	\return Cosine of dInput

	\sa Cos(float) or Sin(double)

***************************************/

// 2!, 4!, to 22!
//#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)
//static const
//#endif
//Burger::Word64ToDouble g_dCosineFactors[11] = {
//	{{0xC000000000000000ULL}},{{0x4038000000000000ULL}},{{0xC086800000000000ULL}},{{0x40E3B00000000000ULL}},{{0xC14BAF8000000000ULL}},
//	{{0x41BC8CFC00000000ULL}},{{0xC2344C3B28000000ULL}},{{0x42B3077775800000ULL}},{{0xC336BEECCA730000ULL}},{{0x43C0E1B3BE415A00ULL}},
//	{{0xC44E77526159F06CULL}}
//};
// 1/2!, 1/4!, to 1/22!
#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)
static const
#endif
Burger::Word64ToDouble g_dInverseCosineFactors[11] = {
	{{0xBFE0000000000000ULL}},{{0x3FA5555555555555ULL}},{{0xBF56C16C16C16C17ULL}},{{0x3EFA01A01A01A01AULL}},{{0xBE927E4FB7789F5CULL}},
	{{0x3E21EED8EFF8D898ULL}},{{0xBDA93974A8C07C9DULL}},{{0x3D2AE7F3E733B81FULL}},{{0xBCA6827863B97D97ULL}},{{0x3C1E542BA4020225ULL}},
	{{0xBB90CE396DB7F853ULL}}
};

#if defined(BURGER_X86) && (defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || defined(BURGER_MSVC)) 
BURGER_DECLSPECNAKED double BURGER_API Burger::Cos(double /* dInput */)
{
	BURGER_ASM {
	fld1
	fld		qword ptr [esp+4]		// Load into the FPU
	fld		qword ptr [g_dReciprocalPi2]	// Load in 1/2Pi
	fmul	st(0),st(1)				// Multiply
	fadd	qword ptr [g_dHalf]		// Add half for rounding
	fst		qword ptr [esp+4]		// Store at double precision
	frndint							// Remove fraction
	fcom	qword ptr [esp+4]		// Compare the two and get rid of the prerounded
	fnstsw	ax
	test	ah,0x41 
	jnz		SinFExit				// Did it round up?
	fsub	qword ptr [g_dOne]		// Fixup
SinFExit:
	fmul	qword ptr [g_dPi2]		// Mul by 2 pi
	fsubp	st(1),st(0)				// Subtract and clean up

	fmul	st(0),st(0)				// Square the input
	fld		st(0)					// Copy the Power of 2			
	fld		qword ptr [g_dInverseCosineFactors]	// Start iterating 
	fmul	st(0),st(1)				// fInput*(1/2!)
	faddp	st(3),st(0)				// fResult + fInput*(1/2!)
	fmul	st(0),st(1)				// Up the power by 2
	fld		qword ptr [g_dInverseCosineFactors+8] 
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseCosineFactors+16]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseCosineFactors+24]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseCosineFactors+32]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseCosineFactors+40]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseCosineFactors+48]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseCosineFactors+56]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseCosineFactors+64]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmul	st(0),st(1)
	fld		qword ptr [g_dInverseCosineFactors+72]
	fmul	st(0),st(1)
	faddp	st(3),st(0)
	fmulp	st(1),st(0)
	fmul	qword ptr [g_dInverseCosineFactors+80]
	faddp	st(1),st(0)
	ret		8						// Clean up and exit
	}
}
// No MacOSX 32 bit intel version assembly version, since SSE builds quite nicely
#else
double BURGER_API Burger::Cos(double dInput)
{
	// Start by rounding the radians to reduce the chance
	// of floating point rounding errors
	dInput = ModuloRadians(dInput);

	// To calculate cosine...
	// Note: ! is factoral so 2! = 1*2, and 4! = 1*2*3*4
	// cos(x) = 1 - ((x^2)/ 2!) + ((x^4)/ 4!) - ((x^6)/ 6!) + ((x^8)/ 8!)
	// Repeat the pattern by reversing the sign of the addition and adding
	// 2 for the square for every iteration

	double dInput2 = (dInput*dInput);

	//- ((x^2)/ 2!)
	double dResult = (dInput2*g_dInverseCosineFactors[0])+1.0;
	//+ ((x^4)/ 4!)
	double dInputFactorial = dInput2*dInput2;
	dResult = (dInputFactorial*g_dInverseCosineFactors[1])+dResult;
	//- ((x^6)/ 6!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseCosineFactors[2])+dResult;
	//+ ((x^8)/ 8!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseCosineFactors[3])+dResult;
	//- ((x^10)/ 10!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseCosineFactors[4])+dResult;
	//+ ((x^12)/ 12!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseCosineFactors[5])+dResult;
	//- ((x^14)/ 14!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseCosineFactors[6])+dResult;
	//+ ((x^16)/ 16!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseCosineFactors[7])+dResult;
	//- ((x^18)/ 18!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseCosineFactors[8])+dResult;
	//+ ((x^20)/ 20!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseCosineFactors[9])+dResult;
	//- ((x^22)/ 22!)
	dInputFactorial = dInputFactorial*dInput2;
	dResult = (dInputFactorial*g_dInverseCosineFactors[10])+dResult;
	return dResult;
}
#endif


float BURGER_API Burger::Tan(float fInput)
{
// Watcom and Mac Class StdC do not have the fast version
#if defined(BURGER_WATCOM) || (defined(BURGER_MAC) && defined(__MATH_H__))
	return static_cast<float>(tan(fInput));
#else
	return tanf(fInput);
#endif
}

double BURGER_API Burger::Tan(double dInput)
{
	return tan(dInput);
}

float BURGER_API Burger::ASin(float fInput)
{
// Watcom and Mac Class StdC do not have the fast version
#if defined(BURGER_WATCOM) || (defined(BURGER_MAC) && defined(__MATH_H__))
	return static_cast<float>(asin(fInput));
#else
	return asinf(fInput);
#endif
}

double BURGER_API Burger::ASin(double dInput)
{
	return asin(dInput);
}

float BURGER_API Burger::ACos(float fInput)
{
// Watcom and Mac Class StdC do not have the fast version
#if defined(BURGER_WATCOM) || (defined(BURGER_MAC) && defined(__MATH_H__))
	return static_cast<float>(acos(fInput));
#else
	return acosf(fInput);
#endif
}

double BURGER_API Burger::ACos(double dInput)
{
	return acos(dInput);
}

float BURGER_API Burger::ATan(float fInput)
{
// Watcom and Mac Class StdC do not have the fast version
#if defined(BURGER_WATCOM) || (defined(BURGER_MAC) && defined(__MATH_H__))
	return static_cast<float>(atan(fInput));
#else
	return atanf(fInput);
#endif
}

double BURGER_API Burger::ATan(double dInput)
{
	return atan(dInput);
}

float BURGER_API Burger::ATan2(float fSin,float fCos)
{
// Watcom and Mac Class StdC do not have the fast version
#if defined(BURGER_WATCOM) || (defined(BURGER_MAC) && defined(__MATH_H__))
	return static_cast<float>(atan2(fSin,fCos));
#else
	return atan2f(fSin,fCos);
#endif
}

double BURGER_API Burger::ATan2(double dSin,double dCos)
{
	return atan2(dSin,dCos);
}

float BURGER_API Burger::Exp(float fInput)
{
// Watcom and Mac Class StdC do not have the fast version
#if defined(BURGER_WATCOM) || (defined(BURGER_MAC) && defined(__MATH_H__))
	return static_cast<float>(exp(fInput));
#else
	return expf(fInput);
#endif
}

double BURGER_API Burger::Exp(double dInput)
{
	return exp(dInput);
}

float BURGER_API Burger::Log(float fInput)
{
// Watcom and Mac Class StdC do not have the fast version
#if defined(BURGER_WATCOM) || (defined(BURGER_MAC) && defined(__MATH_H__))
	return static_cast<float>(log(fInput));
#else
	return logf(fInput);
#endif
}

double BURGER_API Burger::Log(double dInput)
{
	return log(dInput);
}

static const float LN_2 = 1.44269504088896340736f;

float BURGER_API Burger::Log2(float fInput)
{
	return Log(fInput)*LN_2;
}

double BURGER_API Burger::Log2(double dInput)
{
	return Log(dInput)*1.44269504088896340736;
}

float BURGER_API Burger::Log10(float fInput)
{
// Watcom and Mac Class StdC do not have the fast version
#if defined(BURGER_WATCOM) || (defined(BURGER_MAC) && defined(__MATH_H__))
	return static_cast<float>(log10(fInput));
#else
	return log10f(fInput);
#endif
}

double BURGER_API Burger::Log10(double dInput)
{
	return log10(dInput);
}

float BURGER_API Burger::Modf(float fInput,float *pInteger)
{
// Watcom and Mac Class StdC do not have the fast version
#if defined(BURGER_WATCOM) || (defined(BURGER_MAC) && defined(__MATH_H__))
	double dInteger;
	float fResult = static_cast<float>(modf(fInput,&dInteger));
	if (pInteger) {
		pInteger[0] = static_cast<float>(dInteger);
	}
	return fResult;
#else
	return modff(fInput,pInteger);
#endif
}

double BURGER_API Burger::Modf(double dInput,double *pInteger)
{
	return modf(dInput,pInteger);
}

float BURGER_API Burger::Fmod(float fInput,float fDivisor)
{
// Watcom and Mac Class StdC do not have the fast version
#if defined(BURGER_WATCOM) || (defined(BURGER_MAC) && defined(__MATH_H__))
	return static_cast<float>(fmod(fInput,fDivisor));
#else
	return fmodf(fInput,fDivisor);
#endif
}

double BURGER_API Burger::Fmod(double dInput,double dDivisor)
{
	return fmod(dInput,dDivisor);
}