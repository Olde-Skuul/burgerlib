/***************************************

    Fixed point math functions

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfixedpoint.h"
#include "brfloatingpoint.h"
#include "brintrinsics.h"

/*! ************************************

	\def FLOATTOFIXED
	\brief Define to convert a float into a \ref Fixed32.
	
	Macro to convert at compile time a floating point
	value into 16.16 fixed point. Used for compile
	assignment of \ref Fixed32 constants.
	
	\param x Floating point value to convert to a \ref Fixed32.

	\note Due to the float to integer conversion, use
	of this macro at runtime may be a performance
	penalty on the Xbox 360 and PS3.
	
	\code
	// Fast use
	static const Fixed32 MyData = FLOATTOFIXED(4.503f);
	// Slow use
	void Foo(float fInput)
	{
		Fixed32 iInput = FLOATTOFIXED(fInput);
	}
	\endcode

***************************************/

/*! ************************************

	\def FIXEDTOFLOAT
	\brief Define to convert a \ref Fixed32 into a float.
	
	Macro to convert at compile time a 16.16 fixed point
	number into a floating point value
	Used for compile assignment of float constants using
	\ref Fixed32 constants..
	
	\param x 16.16 fixed point value to convert to a float.

	\note Due to the integer to float conversion, use
	of this macro at runtime on non constant values may be a performance
	penalty on the Xbox 360 and PS3.
	
	\code
	// Fast use
	static const float MyData = FIXEDTOFLOAT(Burger::PiFixed32);
	// Slow use
	void Foo(Fixed32 iInput)
	{
		float fInput = FIXEDTOFLOAT(iInput);
	}
	\endcode

***************************************/

/*! ************************************

	\def INTTOFIXED
	\brief Define to convert a integer into a \ref Fixed32.
	
	Macro to convert at compile time an integer
	value into 16.16 fixed point. Used for compile
	assignment of \ref Fixed32 constants.
	
	\param x Integer value to convert to a \ref Fixed32.

	\note There is no bounds checking performed by this
	macro. As a result, integers larger than 32767 or less
	than -32768 will cause undefined behavior due to over/underflow.
	
	\code
	static const Fixed32 MyData = INTTOFIXED(443);
	\endcode

***************************************/

/*! ************************************

	\def FIXEDTOINT
	\brief Define to convert a \ref Fixed32 value to an integer.
	
	Macro to convert at compile time a 16.16 fixed point
	value into an integer. Used for compile time
	assignment of integers using \ref Fixed32 constants.
	
	\param x \ref Fixed32 value to convert to an integer.

	\note This macro uses round to zero in the removal
	of the fraction. Negative numbers like -0.8 become 0,
	as well as 0.8 becomes 0.
	
	\code
	static const int MyData = FIXEDTOINT(Burger::PiFixed32);
	\endcode

***************************************/

/*! ************************************

	\brief Helper constants for float to integer conversions.
	
	These constants are used for the translation of 
	floats to ints using floor, ceil or zero.
	The first constant is added to yield floor()
	and the second is for getting ceil()
	(1.0f/(65536.0f*256.0f)) is the 32-bit floating point
	epsilon just under 1.0f
	
	\sa Burger::FloatToIntRoundToZero(float) or Burger::FloatToIntFloor(float)

***************************************/

#if defined(BURGER_INTEL) || defined(DOXYGEN)
extern "C" const float g_fBurgerIntMathNearesttable[2] = {-0.5f,0.5f-(1.0f/(65536.0f*256.0f))};
#endif

/*! ************************************

	\brief Floating point constant of 65536.0f

***************************************/

#if defined(BURGER_INTEL) || defined(DOXYGEN)
extern "C" const float g_fBurgerMath65536 = 65536.0f;
#endif


/*! ************************************

	\fn uint_t Burger::GetLoWord(uint32_t uInput)
	\brief Retrieves the low-order word from the specified value.
	
	Mask off the upper 16 bit of the 32 bit input and set the
	bits to zero. This is an equivalent to the function
	<a href="http://msdn.microsoft.com/en-us/library/windows/desktop/ms632659(v=vs.85).aspx">LOWORD(DWORD)</a>
	
	\note This is a direct replacement of the MacOS macro LoWord().

	\param uInput The value to be converted.
	\return The low order 16 bits of uInput.
	
	\sa GetHiWord(uint32_t)
	
***************************************/

/*! ************************************

	\fn uint_t Burger::GetHiWord(uint32_t uInput)
	\brief Retrieves the low-order word from the specified value.
	
	Shift the input 16 bits to the right and set the upper
	bits to zero. This is an equivalent to the function
	<a href="http://msdn.microsoft.com/en-us/library/windows/desktop/ms632657(v=vs.85).aspx">HIWORD(DWORD)</a>
	
	\note This is a direct replacement of the MacOS macro HiWord().

	\param uInput The value to be converted.
	\return The high order 16 bits of uInput.
	
	\sa GetLoWord(uint32_t)
	
***************************************/

/*! ************************************

	\fn Fixed32 Burger::IntToFixed(int32_t iInput)
	\brief Convert an int32_t into a \ref Fixed32 value.

	Convert an integer into a fixed point number. No bounds
	checking is performed so values that exceed 32767 or are
	less than -32768 will yield undefined results.

	\param iInput Integer to convert
	\return Result of the conversion without saturation

	\sa IntToFixedSaturate(int32_t)
		
***************************************/

/*! ************************************

	\brief Convert an int32_t into a Fixed32 value with saturation.

	Convert an integer into a fixed point number. Bounds
	checking is performed so values that exceed 32767 or are
	less than -32768 will yield max and min values..

	Examples of clamping:
	* * 0x8000 -> 0x7FFFFFFF
	* * 0xFEEDBEEF -> 0x80000000
		
	\param iInput Integer to convert
	\return Result of the conversion with saturation

	\sa IntToFixed(int32_t)

***************************************/

Fixed32 BURGER_API Burger::IntToFixedSaturate(int32_t iInput)
{
	Fixed32 iResult;
	if (iInput>=0x8000) {			// Not too big?
		iResult = MaxFixed32;		// Return maximum
	} else if (iInput<=-0x7FFF) {	// Not too small?
		iResult = MinFixed32;		// Return minimum
	} else {
		iResult = static_cast<Fixed32>(iInput<<16);	// Convert to fixed (Signed shift)
	}
	return iResult;
}

/*! ************************************

	\fn Burger::FixedToIntFloor(Fixed32 fInput)
	\brief Convert a fixed point value to an integer using round down.
	
	Convert a \ref Fixed32 into an integer using the same formula as floor().
		
	\param fInput Value to convert to an integer.
	\return The input converted to an integer using the truth table below.
	
	\code
	int iResult;
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(1.1f));		//1
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(1.95f));		//1
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(-1.1f));		//-2
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(-1.95f));	//-2
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(0.1f));		//0
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(0.95f));		//0
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(-0.1f));		//-1
	iResult = Burger::FixedToIntFloor(FLOATTOFIXED(-0.95f));	//-1
	\endcode

	\sa Floor(float), Floor(double), FixedToInt(Fixed32), FixedToIntCeil(Fixed32), or FixedToIntNearest(Fixed32)

***************************************/


/*! ************************************

	\fn Burger::FixedToInt(Fixed32 fInput)
	\brief Convert a fixed point value to an integer using round to zero.
	
	Convert a \ref Fixed32 into an integer using round to zero.
		
	\param fInput Value to convert to an integer.
	\return The input converted to an integer using the truth table below.
	
	\code
	floorint = Burger::FixedToInt(FLOATTOFIXED(1.1f));		//1
	floorint = Burger::FixedToInt(FLOATTOFIXED(1.95f));		//1
	floorint = Burger::FixedToInt(FLOATTOFIXED(-1.1f));		//-1
	floorint = Burger::FixedToInt(FLOATTOFIXED(-1.95f));	//-1
	floorint = Burger::FixedToInt(FLOATTOFIXED(0.1f));		//0
	floorint = Burger::FixedToInt(FLOATTOFIXED(0.95f));		//0
	floorint = Burger::FixedToInt(FLOATTOFIXED(-0.1f));		//0
	floorint = Burger::FixedToInt(FLOATTOFIXED(-0.95f));	//0
	\endcode

	\sa FixedToIntFloor(Fixed32), FixedToIntCeil(Fixed32), or FixedToIntNearest(Fixed32)

***************************************/

/*! ************************************

	\fn Burger::FixedToIntCeil(Fixed32 fInput)
	\brief Convert a fixed point value to an integer using round up.
	
	Convert a \ref Fixed32 into an integer using the same formula as ceil().
		
	\param fInput Value to convert to an integer.
	\return The input converted to an integer using the truth table below.
	
	\code
	int iResult;
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(1.1f));		//2
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(1.95f));		//2
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(-1.1f));		//-1
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(-1.95f));	//-1
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(0.1f));		//1
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(0.95f));		//1
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(-0.1f));		//0
	iResult = Burger::FixedToIntCeil(FLOATTOFIXED(-0.95f));	//0
	\endcode

	\sa Ceil(float), Ceil(double), FixedToIntFloor(Fixed32), FixedToInt(Fixed32), or FixedToIntNearest(Fixed32)

***************************************/

/*! ************************************

	\fn Burger::FixedToIntNearest(Fixed32 fInput)
	\brief Convert a fixed point value to an integer using round to nearest.
	
	Convert a \ref Fixed32 into an integer using round to nearest.
		
	\param fInput Value to convert to an integer.
	\return The input converted to an integer using the truth table below.
	
	\code
	int iResult;
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(1.1f));	//1
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(1.95f));	//2
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(-1.1f));	//-1
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(-1.95f));	//-2
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(0.1f));	//0
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(0.95f));	//1
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(-0.1f));	//0
	iResult = Burger::FixedToIntNearest(FLOATTOFIXED(-0.95f));	//-1
	\endcode

	\note This can be used to replace FixRound() from MacOS.

	\sa Round(float), Round(double), FixedToIntFloor(Fixed32), FixedToInt(Fixed32), or FixedToIntCeil(Fixed32)

***************************************/


/*! ************************************

	\fn int32_t Burger::FloatToIntFloor(float fInput)
	\brief Convert a 32 bit float to an integer using floor().

	Convert a single precision floating point number to an integer
	using the floor() form of fractional truncation (Round down)

	\code
	floorint = Burger::FloatToIntFloor(1.1f);	//1
	floorint = Burger::FloatToIntFloor(1.95f);	//1
	floorint = Burger::FloatToIntFloor(-1.1f);	//-2
	floorint = Burger::FloatToIntFloor(-1.95f);	//-2
	floorint = Burger::FloatToIntFloor(0.1f);	//0
	floorint = Burger::FloatToIntFloor(0.95f);	//0
	floorint = Burger::FloatToIntFloor(-0.1f);	//-1
	floorint = Burger::FloatToIntFloor(-0.95f);	//-1
	\endcode

	\param fInput A valid single precision floating point number.
	\return Signed integer equivalent value after applying floor() on the floating point number.

	\sa FloatToIntFloor(int32_t *,float), FloatToIntRoundToZero(float), FloatToIntCeil(float), or FloatToIntRound(float)

***************************************/

#if defined(BURGER_X86) && (defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || defined(BURGER_MSVC))

BURGER_DECLSPECNAKED int32_t BURGER_API Burger::FloatToIntFloor(float /* fInput */)
{
BURGER_ASM {
	fld		dword ptr [esp+4]		// Get the input value
	fld		st(0)					// Load the same value in the FPU
	frndint							// Convert the integer to float
	fist	dword ptr [esp+4]		// Store the integer (Leave a copy in the FPU)
	fcomip	st(0),st(1)				// Compare the integer to the input and pop
	mov		eax,dword ptr [esp+4]	// Get the integer
	jbe		NoExtra					// Need to update the rounding?
	dec		eax						// --1
NoExtra:
	fstp	st(0)					// Clean up the FP stack
	ret		4
}
}

BURGER_DECLSPECNAKED void BURGER_API Burger::FloatToIntFloor(int32_t * /* pOutput */,float /* fInput */)
{
BURGER_ASM {
	fld		dword ptr [esp+4]		// Get the input value
	fld		st(0)					// Load the same value in the FPU
	frndint							// Convert the integer to float
	fist	dword ptr [esp+4]		// Store the integer (Leave a copy in the FPU)
	fcomip	st(0),st(1)				// Compare the integer and pop
	fstp	st(0)					// Clean up the FP stack

#if defined(BURGER_WATCOM)
	push	ecx						// Save registers
	mov		ecx,dword ptr [esp+4+4]	// Get the integer
	jbe		NoExtra					// Overflow? (Negative)
	dec		ecx						// Round to zero
NoExtra:
	mov		dword ptr [eax],ecx		// Store in the pointer
	pop		ecx						// Restore registers

#else
	mov		eax,dword ptr [esp+4]	// Get the integer
	jbe		NoExtra					// Overflow? (Negative)
	dec		eax						// Round to zero
NoExtra:
	mov		dword ptr [ecx],eax		// Store in the pointer

#endif
	ret		4						// Clean up and exit
}
}

#elif defined(BURGER_AMD64) || (defined(BURGER_INTEL) && (defined(BURGER_MACOSX) || defined(BURGER_IOS)))

int32_t BURGER_API Burger::FloatToIntFloor(float fInput)
{
	// Convert to SSE register
	__m128 vInput = _mm_set_ss(fInput);
	// Convert to int with round to zero
	__m128i iVar = _mm_cvttps_epi32(vInput);					
	// Convert back to float
	__m128 fVar = _mm_cvtepi32_ps(iVar);
	// Did I round up?
	fVar = _mm_cmpgt_ss(fVar,vInput);
	// Add 0 or -1	
	iVar = _mm_add_epi32(iVar,_mm_castps_si128(fVar));	
	return _mm_cvtsi128_si32(iVar);
}

void BURGER_API Burger::FloatToIntFloor(int32_t *pOutput,float fInput)
{
	// Convert to SSE register
	__m128 vInput = _mm_set_ss(fInput);
	// Convert to int with round to zero
	__m128i iVar = _mm_cvttps_epi32(vInput);					
	// Convert back to float
	__m128 fVar = _mm_cvtepi32_ps(iVar);
	// Did I round up?
	fVar = _mm_cmpgt_ss(fVar,vInput);
	// Add 0 or -1	
	iVar = _mm_add_epi32(iVar,_mm_castps_si128(fVar));
	// Store the result
	_mm_store_ss(static_cast<float *>(static_cast<void *>(pOutput)),_mm_castsi128_ps(iVar));
}

#elif defined(BURGER_XBOX360) || (defined(BURGER_POWERPC64) && defined(BURGER_MACOSX))

int32_t Burger::FloatToIntFloor(float fInput) 
{ 
	// Convert to the input to an integer
	double dVar = __fcfid(__fctidz(fInput));

	// Floor the value to - infinity
	dVar = __fsel(fInput-dVar,dVar,dVar-1.0f);

	// Return as an integer (Load/Hit/Store)
	int32_t iResult;
	__stfiwx(__fctiw(dVar),0,&iResult);
	return iResult;
}

void Burger::FloatToIntFloor(int32_t *pOutput,float fInput) 
{ 
	// Convert to the input to an integer
	double dVar = __fcfid(__fctidz(fInput));

	// Floor the value to - infinity
	dVar = __fsel(fInput-dVar,dVar,dVar-1.0f);	
	// Return floored value
	__stfiwx(__fctiw(dVar),0,pOutput);
}

#elif defined(BURGER_PPC) && defined(BURGER_METROWERKS)

BURGER_ASM int32_t BURGER_API Burger::FloatToIntFloor(float /* fInput */)
{
	lwz		r3,g_fMinNoInteger
	mffs	fp4						// Save the rounding register
	lfs		fp2,0(r3)				// Load 8388608.0f
	mtfsfi	7,0x03					// Set bits 30 and 31 to Round toward -infinity
	fadds	fp3,fp1,fp2				// Push the positive number to highest value without fraction (Removes fraction)
	fsubs	fp0,fp1,fp2				// Push the negative number to the lowest value without fraction (Removes fraction)

	fsubs	fp3,fp3,fp2				// Undo the push (Fraction is gone)
	fadds	fp0,fp0,fp2				// Undo the push (Fraction is gone)

	fsel	fp1,fp1,fp3,fp0			// Which one to use? Positive or negative?
	mtfsf	255,fp4					// Restore rounding
	fctiwz	fp1,fp1					// Convert to integer
	subi	r3,r1,8					// Pointer to temp on the stack
	stfiwx	fp1,0,r3				// Store the integer
	lwz		r3,0(r3)				// Return the integer
	blr
}

BURGER_ASM void BURGER_API Burger::FloatToIntFloor(int32_t * /* pOutput */,float /* fInput */ )
{
	lwz		r4,g_fMinNoInteger
	mffs	fp4						// Save the rounding register
	lfs		fp2,0(r4)				// Load 8388608.0f
	mtfsfi	7,0x03					// Set bits 30 and 31 to Round toward -infinity
	fadds	fp3,fp1,fp2				// Push the positive number to highest value without fraction (Removes fraction)
	fsubs	fp0,fp1,fp2				// Push the negative number to the lowest value without fraction (Removes fraction)

	fsubs	fp3,fp3,fp2				// Undo the push (Fraction is gone)
	fadds	fp0,fp0,fp2				// Undo the push (Fraction is gone)

	fsel	fp1,fp1,fp3,fp0			// Which one to use? Positive or negative?
	mtfsf	255,fp4					// Restore rounding
	fctiwz	fp1,fp1					// Convert to integer
	stfiwx	fp1,0,r3				// Store the integer
	blr
}

#else

int32_t BURGER_API Burger::FloatToIntFloor(float fInput)
{
	int iVar = static_cast<int>(fInput);			// Convert to int but rounded!
	float fVar = static_cast<float>(iVar);
	if (fVar>fInput) {	// Did I round up?
		--iVar;			// Fix it
	}
	return iVar;
}

/*! ************************************

	\brief Convert a 32 bit float to an integer using floor().

	Convert a single precision floating point number to an integer
	using the floor() form of fractional truncation and store it to memory

	\code
	Burger::FloatToIntFloor(&floorint,1.1f);	//1
	Burger::FloatToIntFloor(&floorint,1.95f);	//1
	Burger::FloatToIntFloor(&floorint,-1.1f);	//-2
	Burger::FloatToIntFloor(&floorint,-1.95f);	//-2
	Burger::FloatToIntFloor(&floorint,0.1f);	//0
	Burger::FloatToIntFloor(&floorint,0.95f);	//0
	Burger::FloatToIntFloor(&floorint,-0.1f);	//-1
	Burger::FloatToIntFloor(&floorint,-0.95f);	//-1
	\endcode


	\param pOutput A valid pointer to a 32-bit integer to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToIntFloor(float), FloatToIntRoundToZero(int32_t *,float), FloatToIntCeil(int32_t *,float), or FloatToIntRound(int32_t *,float)
	
***************************************/

void BURGER_API Burger::FloatToIntFloor(int32_t *pOutput,float fInput)
{
	int iVar = static_cast<int>(fInput);			// Convert to int but rounded!
	float fVar = static_cast<float>(iVar);
	if (fVar>fInput) {	// Did I round up?
		--iVar;			// Fix it
	}
	pOutput[0] = iVar;
}

#endif

/*! ************************************

	\fn int32_t Burger::FloatToIntCeil(float fInput)
	\brief Convert a 32 bit float to an integer using ceil().

	Convert a single precision floating point number to an integer
	using the ceil() form of fractional truncation

	\code
	floorint = Burger::FloatToIntCeil(1.1f);	//2
	floorint = Burger::FloatToIntCeil(1.95f);	//2
	floorint = Burger::FloatToIntCeil(-1.1f);	//-1
	floorint = Burger::FloatToIntCeil(-1.95f);	//-1
	floorint = Burger::FloatToIntCeil(0.1f);	//1
	floorint = Burger::FloatToIntCeil(0.95f);	//1
	floorint = Burger::FloatToIntCeil(-0.1f);	//0
	floorint = Burger::FloatToIntCeil(-0.95f);	//0
	\endcode

	\param fInput A valid single precision floating point number.
	\return Signed integer equivalent value after applying ceil() on the floating point number.

	\sa FloatToIntCeil(int32_t *,float), FloatToIntFloor(float), FloatToIntRoundToZero(float), or FloatToIntRound(float)
	
***************************************/

#if defined(BURGER_X86) && (defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || defined(BURGER_MSVC))

BURGER_DECLSPECNAKED int32_t BURGER_API Burger::FloatToIntCeil(float /* fInput */)
{
BURGER_ASM {
	fld		dword ptr [esp+4]		// Get the input value
	fld		st(0)					// Load the same value in the FPU
	frndint							// Convert the integer to float
	fist	dword ptr [esp+4]		// Store the integer (Leave a copy in the FPU)
	fcomip	st(0),st(1)				// Compare the integer to the input and pop
	mov		eax,dword ptr [esp+4]	// Get the integer
	jae		NoExtra					// Need to update the rounding?
	inc		eax						// ++1
NoExtra:
	fstp	st(0)					// Clean up the FP stack
	ret		4
}
}

BURGER_DECLSPECNAKED void BURGER_API Burger::FloatToIntCeil(int32_t * /* pOutput */,float /* fInput */)
{
BURGER_ASM {
	fld		dword ptr [esp+4]		// Get the input value
	fld		st(0)					// Load the same value in the FPU
	frndint							// Convert the integer to float
	fist	dword ptr [esp+4]		// Store the integer (Leave a copy in the FPU)
	fcomip	st(0),st(1)				// Compare the integer to the input and pop
	fstp	st(0)					// Clean up the FP stack

#if defined(BURGER_WATCOM)
	push	ecx						// Save registers
	mov		ecx,dword ptr [esp+4+4]	// Get the integer
	jae		NoExtra					// Need to update the rounding?
	inc		ecx						// ++1
NoExtra:
	mov		dword ptr [eax],ecx		// Store in the pointer
	pop		ecx						// Restore registers

#else
	mov		eax,dword ptr [esp+4]	// Get the integer
	jae		NoExtra					// Need to update the rounding?
	inc		eax						// --1
NoExtra:
	mov		dword ptr [ecx],eax		// Store in the pointer
#endif
	ret		4
}
}

#elif defined(BURGER_AMD64) || (defined(BURGER_INTEL) && (defined(BURGER_MACOSX) || defined(BURGER_IOS)))

int32_t BURGER_API Burger::FloatToIntCeil(float fInput)
{
	// Convert to SSE register
	__m128 vInput = _mm_set_ss(fInput);
	// Convert to int with round to zero
	__m128i iVar = _mm_cvttps_epi32(vInput);					
	// Convert back to float
	__m128 fVar = _mm_cvtepi32_ps(iVar);
	// Did I round up?
	fVar = _mm_cmplt_ss(fVar,vInput);
	// Subtract 0 or -1	
	iVar = _mm_sub_epi32(iVar,_mm_castps_si128(fVar));	
	return _mm_cvtsi128_si32(iVar);
}

void BURGER_API Burger::FloatToIntCeil(int32_t *pOutput,float fInput)
{
	// Convert to SSE register
	__m128 vInput = _mm_set_ss(fInput);
	// Convert to int with round to zero
	__m128i iVar = _mm_cvttps_epi32(vInput);					
	// Convert back to float
	__m128 fVar = _mm_cvtepi32_ps(iVar);
	// Did I round up?
	fVar = _mm_cmplt_ss(fVar,vInput);
	// Add 0 or -1	
	iVar = _mm_sub_epi32(iVar,_mm_castps_si128(fVar));
	// Store the result
	_mm_store_ss(static_cast<float *>(static_cast<void *>(pOutput)),_mm_castsi128_ps(iVar));
}

#elif defined(BURGER_XBOX360) || (defined(BURGER_POWERPC64) && defined(BURGER_MACOSX))

int32_t Burger::FloatToIntCeil(float fInput) 
{ 
	// Convert to the input to an integer
	double dVar = __fcfid(__fctidz(fInput));

	// Floor the value to - infinity
	dVar = __fsel(dVar-fInput,dVar,dVar+1.0f);

	// Return as an integer (Load/Hit/Store)
	int32_t iResult;
	__stfiwx(__fctiw(dVar),0,&iResult);
	return iResult;
}

void Burger::FloatToIntCeil(int32_t *pOutput,float fInput) 
{ 
	// Convert to the input to an integer
	double dVar = __fcfid(__fctidz(fInput));

	// Floor the value to - infinity
	dVar = __fsel(dVar-fInput,dVar,dVar+1.0f);	
	// Return floored value
	__stfiwx(__fctiw(dVar),0,pOutput);
}

#elif defined(BURGER_PPC) && defined(BURGER_METROWERKS)

BURGER_ASM int32_t BURGER_API Burger::FloatToIntCeil(float /* fInput */)
{
	lwz		r3,g_fMinNoInteger
	mffs	fp4						// Save the rounding register
	lfs		fp2,0(r3)				// Load 8388608.0f
	mtfsfi	7,0x02					// Set bits 30 and 31 to Round toward +infinity
	fadds	fp3,fp1,fp2				// Push the positive number to highest value without fraction (Removes fraction)
	fsubs	fp0,fp1,fp2				// Push the negative number to the lowest value without fraction (Removes fraction)

	fsubs	fp3,fp3,fp2				// Undo the push (Fraction is gone)
	fadds	fp0,fp0,fp2				// Undo the push (Fraction is gone)

	fsel	fp1,fp1,fp3,fp0			// Which one to use? Positive or negative?
	mtfsf	255,fp4					// Restore rounding
	fctiwz	fp1,fp1					// Convert to integer
	subi	r3,r1,8					// Pointer to temp on the stack
	stfiwx	fp1,0,r3				// Store the integer
	lwz		r3,0(r3)				// Return the integer
	blr
}

BURGER_ASM void BURGER_API Burger::FloatToIntCeil(int32_t * /* pOutput */,float /* fInput */ )
{
	lwz		r4,g_fMinNoInteger
	mffs	fp4						// Save the rounding register
	lfs		fp2,0(r4)				// Load 8388608.0f
	mtfsfi	7,0x02					// Set bits 30 and 31 to Round toward +infinity
	fadds	fp3,fp1,fp2				// Push the positive number to highest value without fraction (Removes fraction)
	fsubs	fp0,fp1,fp2				// Push the negative number to the lowest value without fraction (Removes fraction)

	fsubs	fp3,fp3,fp2				// Undo the push (Fraction is gone)
	fadds	fp0,fp0,fp2				// Undo the push (Fraction is gone)

	fsel	fp1,fp1,fp3,fp0			// Which one to use? Positive or negative?
	mtfsf	255,fp4					// Restore rounding
	fctiwz	fp1,fp1					// Conver to integer
	stfiwx	fp1,0,r3				// Store the integer
	blr
}

#else

int32_t BURGER_API Burger::FloatToIntCeil(float fInput)
{
	int iVar = static_cast<int>(fInput);			// Convert to an int
	float fVar = static_cast<float>(iVar);
	if (fVar<fInput) {			// Was there a change?
		++iVar;					// Round up
	}
	return iVar;
}

/*! ************************************

	\brief Convert a 32 bit float to an integer using ceil().

	Convert a single precision floating point number to an integer
	using the ceil() form of fractional truncation and store it to memory
		
	\code
	Burger::FloatToIntCeil(&floorint,1.1f);		//2
	Burger::FloatToIntCeil(&floorint,1.95f);	//2
	Burger::FloatToIntCeil(&floorint,-1.1f);	//-1
	Burger::FloatToIntCeil(&floorint,-1.95f);	//-1
	Burger::FloatToIntCeil(&floorint,0.1f);		//1
	Burger::FloatToIntCeil(&floorint,0.95f);	//1
	Burger::FloatToIntCeil(&floorint,-0.1f);	//0
	Burger::FloatToIntCeil(&floorint,-0.95f);	//0
	\endcode

	\param pOutput A valid pointer to a 32-bit integer to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToIntCeil(float), FloatToIntFloor(int32_t *,float), FloatToIntRoundToZero(int32_t *,float), or FloatToIntRound(int32_t *,float)
	
***************************************/

void BURGER_API Burger::FloatToIntCeil(int32_t *pOutput,float fInput)
{
	int iVar = static_cast<int>(fInput);			// Convert to an int
	float fVar = static_cast<float>(iVar);
	if (fVar<fInput) {			// Was there a change?
		++iVar;					// Round up
	}
	pOutput[0] = iVar;
}

#endif


/*! ************************************

	\brief Convert a 32 bit float to an integer using round to nearest.

	Convert a single precision floating point number to an integer
	using the round to nearest fractional truncation
		
	\code
	floorint = Burger::FloatToIntRound(1.1f);		//1
	floorint = Burger::FloatToIntRound(1.95f);		//2
	floorint = Burger::FloatToIntRound(-1.1f);		//-1
	floorint = Burger::FloatToIntRound(-1.95f);		//-2
	floorint = Burger::FloatToIntRound(0.1f);		//0
	floorint = Burger::FloatToIntRound(0.95f);		//1
	floorint = Burger::FloatToIntRound(-0.1f);		//0
	floorint = Burger::FloatToIntRound(-0.95f);		//-1
	\endcode

	\param fInput A valid single precision floating point number.
	\return Signed integer equivalent value after applying round to nearest on the floating point number.

	\sa FloatToIntRound(int32_t *,float), FloatToIntFloor(float), FloatToIntRoundToZero(float), or FloatToIntCeil(float)

***************************************/

#if defined(BURGER_X86) && (defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || defined(BURGER_MSVC))

BURGER_DECLSPECNAKED int32_t BURGER_API Burger::FloatToIntRound(float /* fInput */)
{
BURGER_ASM {
	mov		eax,dword ptr [esp+4]	// Get the sign bit
	fld		dword ptr [esp+4]		// Load into the FPU
	shr		eax,31					// 1 for negative, 0 for positive
	fstcw	[esp+4]					// Save state																																																																																	
	fldcw	word ptr [g_X86RoundDownFlag]		// Set round down
	fadd	dword ptr [g_X86OneAndNegOne+eax*4]	// Add or subtract 0.5f
	frndint							// Round
	fldcw	[esp+4]					// Restore rounding
	fistp	dword ptr [esp+4]		// Store integer
	mov		eax,dword ptr [esp+4]	// Get the result
	ret		4
}
}

BURGER_DECLSPECNAKED void BURGER_API Burger::FloatToIntRound(int32_t * /* pOutput */,float /* fInput */)
{
BURGER_ASM {

#if defined(BURGER_WATCOM)

	push	eax						// Watcom passes pOutput in eax instead of ecx
	mov		eax,dword ptr [esp+8]	// Get the sign bit
	fld		dword ptr [esp+8]		// Load into the FPU
	shr		eax,31					// 1 for negative, 0 for positive
	fstcw	[esp+8]					// Save state
	fldcw	word ptr [g_X86RoundDownFlag]	// Set round down
	fadd	dword ptr [g_X86OneAndNegOne+eax*4]		// Add or subtract 0.5f
	frndint							// Round
	fldcw	[esp+8]					// Restore rounding
	pop		eax
	fistp	dword ptr [eax]			// Store in pOutput[0]

#else

	mov		eax,dword ptr [esp+4]	// Get the sign bit
	fld		dword ptr [esp+4]		// Load into the FPU
	shr		eax,31					// 1 for negative, 0 for positive
	fstcw	[esp+4]					// Save state
	fldcw	word ptr [g_X86RoundDownFlag]	// Set round down
	fadd	dword ptr [g_X86OneAndNegOne+eax*4]		// Add or subtract 0.5f
	frndint							// Round
	fldcw	[esp+4]					// Restore rounding
	fistp	dword ptr [ecx]			// Store in pOutput[0]

#endif

	ret		4						// Clean up and exit
}
}


#elif defined(BURGER_AMD64) || (defined(BURGER_INTEL) && (defined(BURGER_MACOSX) || defined(BURGER_IOS)))

int32_t BURGER_API Burger::FloatToIntRound(float fInput)
{
	// Convert to SSE register
	__m128 vInput = _mm_set_ss(fInput);

	// Convert to int with round to zero
	__m128i iVar = _mm_cvttps_epi32(vInput);					

	// Convert back to float
	__m128 fVar = _mm_cvtepi32_ps(iVar);

	// Get the difference
	__m128 fDiff = _mm_sub_ps(vInput,fVar);

	// Constant 0x80000000
	__m128 vZero = _mm_set_ss(-0.0f);

	// Abs value if fDiff (-0.0 becomes 0x7FFFFFFF)
	fDiff = _mm_andnot_ps(vZero,fDiff);

	// If vInput is negative, return 0xFFFFFFFF or return 0x00000001
	vInput = _mm_cmplt_ss(vInput,vZero);

	// Convert vZero to hex 0x000000001
	vZero = _mm_castsi128_ps(_mm_srli_epi32(_mm_castps_si128(vZero),31));

	// 0xFFFFFFFF if greater than or equal 0.5f
	fDiff = _mm_cmpge_ss(fDiff,_mm_set_ss(g_fHalf));

	// Turn zero to 0x00000001
	vInput = _mm_or_ps(vInput,vZero);

	// Turn the -1/1 to 0 if < 0.5f
	fVar = _mm_and_ps(vInput,fDiff);

	// Add -1, 0 or 1 for final adjustment
	iVar = _mm_add_epi32(iVar,_mm_castps_si128(fVar));
	return _mm_cvtsi128_si32(iVar);
}

void BURGER_API Burger::FloatToIntRound(int32_t *pOutput,float fInput)
{
	// Convert to SSE register
	__m128 vInput = _mm_set_ss(fInput);

	// Convert to int with round to zero
	__m128i iVar = _mm_cvttps_epi32(vInput);					

	// Convert back to float
	__m128 fVar = _mm_cvtepi32_ps(iVar);

	// Get the difference
	__m128 fDiff = _mm_sub_ps(vInput,fVar);

	// Constant 0x80000000
	__m128 vZero = _mm_set_ss(-0.0f);

	// Abs value if fDiff (-0.0 becomes 0x7FFFFFFF)
	fDiff = _mm_andnot_ps(vZero,fDiff);

	// If vInput is negative, return 0xFFFFFFFF or return 0x00000001
	vInput = _mm_cmplt_ss(vInput,vZero);

	// Convert vZero to hex 0x000000001
	vZero = _mm_castsi128_ps(_mm_srli_epi32(_mm_castps_si128(vZero),31));

	// 0xFFFFFFFF if greater than or equal 0.5f
	fDiff = _mm_cmpge_ss(fDiff,_mm_set_ss(g_fHalf));

	// Turn zero to 0x00000001
	vInput = _mm_or_ps(vInput,vZero);

	// Turn the -1/1 to 0 if < 0.5f
	fVar = _mm_and_ps(vInput,fDiff);

	// Add -1, 0 or 1 for final adjustment
	iVar = _mm_add_epi32(iVar,_mm_castps_si128(fVar));
	// Store the result
	_mm_store_ss(static_cast<float *>(static_cast<void *>(pOutput)),_mm_castsi128_ps(iVar));
}

#elif defined(BURGER_XBOX360) || (defined(BURGER_POWERPC64) && defined(BURGER_MACOSX))

int32_t BURGER_API Burger::FloatToIntRound(float fInput)
{
	// Get the absolute value
	double dAbs = fabs(fInput);

	// Convert to the input to an integer (Can fail on large numbers)
	double dVar = __fcfid(__fctidz(dAbs));

	// Get the fraction
	double dFraction = dAbs-dVar;

	// Test for rounding and add 1 if it needs to round up
	dVar += __fsel(dFraction-0.5,1.0,0.0);

	// Restore the sign
	dVar = __fsel(static_cast<double>(fInput),dVar,-dVar);

	// Return as an integer (Load/Hit/Store)
	int32_t iResult;
	__stfiwx(__fctiw(dVar),0,&iResult);
	return iResult;
}

void BURGER_API Burger::FloatToIntRound(int32_t *pOutput,float fInput)
{
	// Get the absolute value
	double dAbs = fabs(fInput);

	// Convert to the input to an integer (Can fail on large numbers)
	double dVar = __fcfid(__fctidz(dAbs));

	// Get the fraction
	double dFraction = dAbs-dVar;

	// Test for rounding and add 1 if it needs to round up
	dVar += __fsel(dFraction-0.5,1.0,0.0);

	// Restore the sign
	dVar = __fsel(static_cast<double>(fInput),dVar,-dVar);

	// Return as an integer
	__stfiwx(__fctiw(dVar),0,pOutput);
}

#elif defined(BURGER_PPC) && defined(BURGER_METROWERKS)

BURGER_ASM int32_t BURGER_API Burger::FloatToIntRound(float /* fInput */)
{
	lwz		r3,g_fMinNoInteger
	fabs	fp0,fp1					// Get the abs value to test
	lfs		fp2,0(r3)				// Load 8388608.0f

	lwz		r3,g_fHalf				// Pointer to 0.5f
	fadds	fp5,fp0,fp2				// Push the positive number to highest value without fraction (Removes fraction)
	lfs		fp3,0(r3)				// Load 0.5f
	fsubs	fp5,fp5,fp2				// Undo the push (Fraction is gone)
	fsubs	fp6,fp0,fp5				// Subtract original from rounded to get the fraction
	fsubs	fp6,fp6,fp3				// Test against 0.5
	fadds	fp3,fp3,fp3				// Set to one
	fsubs	fp4,fp5,fp5				// Set to zero
	fsel	fp0,fp6,fp3,fp4			// Set to zero or one depending on the test
	fadds	fp5,fp5,fp0				// Add 0 for no rounding, 1 for round up
	fnabs	fp2,fp5					// Get the negative value
	fsel	fp1,fp1,fp5,fp2			// Which one to use? Positive or negative?

	fctiwz	fp1,fp1					// Convert to integer
	subi	r3,r1,8					// Pointer to temp on the stack
	stfiwx	fp1,0,r3				// Store the integer
	lwz		r3,0(r3)				// Return the integer
	blr
}

BURGER_ASM void BURGER_API Burger::FloatToIntRound(int32_t * /* pOutput */,float /* fInput */ )
{
	lwz		r4,g_fMinNoInteger
	fabs	fp0,fp1					// Get the abs value to test
	lfs		fp2,0(r4)				// Load 8388608.0f

	lwz		r4,g_fHalf				// Pointer to 0.5f
	fadds	fp5,fp0,fp2				// Push the positive number to highest value without fraction (Removes fraction)
	lfs		fp3,0(r4)				// Load 0.5f
	fsubs	fp5,fp5,fp2				// Undo the push (Fraction is gone)
	fsubs	fp6,fp0,fp5				// Subtract original from rounded to get the fraction
	fsubs	fp6,fp6,fp3				// Test against 0.5
	fadds	fp3,fp3,fp3				// Set to one
	fsubs	fp4,fp5,fp5				// Set to zero
	fsel	fp0,fp6,fp3,fp4			// Set to zero or one depending on the test
	fadds	fp5,fp5,fp0				// Add 0 for no rounding, 1 for round up
	fnabs	fp2,fp5					// Get the negative value
	fsel	fp1,fp1,fp5,fp2			// Which one to use? Positive or negative?

	fctiwz	fp1,fp1					// Convert to integer
	stfiwx	fp1,0,r3				// Store the integer
	blr
}

#else

int32_t BURGER_API Burger::FloatToIntRound(float fInput)
{
	int32_t iVal = static_cast<int32_t>(fInput);
	float fVal = static_cast<float>(iVal);
	float fDiff = Abs(fInput-fVal);
	if (fDiff>=0.5f) {
		if (fInput>=0.0f) {
			iVal+=1;
		} else {
			iVal-=1;
		}
	}
	return iVal;		// Round to zero
}

/*! ************************************

	\brief Convert a 32 bit float to an integer using round to nearest.

	Convert a single precision floating point number to an integer
	using the round to nearest fractional truncation and store it to memory
		
	\code
	Burger::FloatToIntRound(&floorint,1.1f);		//1
	Burger::FloatToIntRound(&floorint,1.95f);		//2
	Burger::FloatToIntRound(&floorint,-1.1f);		//-1
	Burger::FloatToIntRound(&floorint,-1.95f);		//-2
	Burger::FloatToIntRound(&floorint,0.1f);		//0
	Burger::FloatToIntRound(&floorint,0.95f);		//1
	Burger::FloatToIntRound(&floorint,-0.1f);		//0
	Burger::FloatToIntRound(&floorint,-0.95f);		//-1
	\endcode

	\param pOutput A valid pointer to a 32-bit integer to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToIntRound(float), FloatToIntFloor(int32_t *,float), FloatToIntRoundToZero(int32_t *,float), or FloatToIntCeil(int32_t *,float)
	
***************************************/

void BURGER_API Burger::FloatToIntRound(int32_t *pOutput,float fInput)
{
	int32_t iVal = static_cast<int32_t>(fInput);
	float fVal = static_cast<float>(iVal);
	float fDiff = Abs(fInput-fVal);
	if (fDiff>=0.5f) {
		if (fInput>=0.0f) {
			iVal+=1;
		} else {
			iVal-=1;
		}
	}
	pOutput[0] = iVal;		// Round to zero
}

#endif


/*! ************************************

	\fn int32_t Burger::FloatToIntRoundToZero(float fInput)
	\brief Convert a 32 bit float to an integer using round to zero.

	Convert a single precision floating point number to an integer
	using the round to zero fractional truncation

	\code
	floorint = Burger::FloatToIntRoundToZero(1.1f);	//1
	floorint = Burger::FloatToIntRoundToZero(1.95f);	//1
	floorint = Burger::FloatToIntRoundToZero(-1.1f);	//-1
	floorint = Burger::FloatToIntRoundToZero(-1.95f);	//-1
	floorint = Burger::FloatToIntRoundToZero(0.1f);	//0
	floorint = Burger::FloatToIntRoundToZero(0.95f);	//0
	floorint = Burger::FloatToIntRoundToZero(-0.1f);	//0
	floorint = Burger::FloatToIntRoundToZero(-0.95f);	//0
	\endcode

	\param fInput A valid single precision floating point number.
	\return Signed integer equivalent value after applying round to zero on the floating point number.

	\sa FloatToIntRoundToZero(int32_t *,float), FloatToIntFloor(float), FloatToIntCeil(float), or FloatToIntRound(float)

***************************************/

int32_t BURGER_API Burger::FloatToIntRoundToZero(float fInput)
{
	return static_cast<int32_t>(fInput);		// Round to zero
}

/*! ************************************

	\fn void Burger::FloatToIntRoundToZero(int32_t *pOutput,float fInput)
	\brief Convert a 32 bit float to an integer using round to zero.

	Convert a single precision floating point number to an integer
	using the round to zero fractional truncation and store it to memory
		
	\code
	Burger::FloatToIntRoundToZero(&floorint,1.1f);		//1
	Burger::FloatToIntRoundToZero(&floorint,1.95f);	//1
	Burger::FloatToIntRoundToZero(&floorint,-1.1f);	//-1
	Burger::FloatToIntRoundToZero(&floorint,-1.95f);	//-1
	Burger::FloatToIntRoundToZero(&floorint,0.1f);		//0
	Burger::FloatToIntRoundToZero(&floorint,0.95f);	//0
	Burger::FloatToIntRoundToZero(&floorint,-0.1f);	//0
	Burger::FloatToIntRoundToZero(&floorint,-0.95f);	//0
	\endcode

	\param pOutput A valid pointer to a 32-bit integer to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToIntRoundToZero(float), FloatToIntFloor(int32_t *,float), FloatToIntCeil(int32_t *,float), or FloatToIntRound(int32_t *,float)
	
***************************************/

void BURGER_API Burger::FloatToIntRoundToZero(int32_t *pOutput,float fInput)
{
	pOutput[0] = static_cast<int32_t>(fInput);		// Round to zero
}



/*! ************************************

	\fn Fixed32 Burger::FloatToFixedFloor(float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using floor().

	Convert a single precision floating point number to a \ref Fixed32
	using the floor() form of fractional truncation
		
	\param fInput A valid single precision floating point number.
	\return a \ref Fixed32 equivalent value after applying floor() on the floating point number.
	\sa FloatToFixedFloor(Fixed32 *,float), FloatToFixed(float), FloatToFixedCeil(float), or FloatToFixedNearest(float)
	
***************************************/

/*! ************************************

	\fn Fixed32 Burger::FloatToFixed(float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using round to zero.

	Convert a single precision floating point number to a \ref Fixed32
	using the round to zero fractional truncation
		
	\param fInput A valid single precision floating point number.
	\return a \ref Fixed32 equivalent value after applying round to zero on the floating point number.
	\sa FloatToFixed(Fixed32 *,float), FloatToFixedFloor(float), FloatToFixedCeil(float), or FloatToFixedNearest(float)
	
***************************************/

/*! ************************************

	\fn Fixed32 Burger::FloatToFixedCeil(float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using ceil().

	Convert a single precision floating point number to a \ref Fixed32
	using the ceil() form of fractional truncation
		
	\param fInput A valid single precision floating point number.
	\return a \ref Fixed32 equivalent value after applying ceil() on the floating point number.
	\sa FloatToFixedCeil(Fixed32 *,float), FloatToFixedFloor(float), FloatToFixed(float), or FloatToFixedNearest(float)
	
***************************************/

/*! ************************************

	\fn Fixed32 Burger::FloatToFixedNearest(float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using round to nearest.

	Convert a single precision floating point number to a \ref Fixed32
	using the round to nearest fractional truncation
		
	\param fInput A valid single precision floating point number.
	\return a \ref Fixed32 equivalent value after applying round to nearest on the floating point number.
	\sa FloatToFixedNearest(Fixed32 *,float), FloatToFixedFloor(float), FloatToFixed(float), or FloatToFixedCeil(float)
	
***************************************/

/*! ************************************

	\fn void Burger::FloatToFixedFloor(Fixed32 *pOutput,float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using floor().

	Convert a single precision floating point number to a \ref Fixed32
	using the floor() form of fractional truncation and store it to memory
	
	\param pOutput A valid pointer to a \ref Fixed32 to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToFixedFloor(float), FloatToFixed(Fixed32 *,float), FloatToFixedCeil(Fixed32 *,float), or FloatToFixedNearest(Fixed32 *,float)
	
***************************************/

/*! ************************************

	\fn void Burger::FloatToFixed(Fixed32 *pOutput,float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using round to zero.

	Convert a single precision floating point number to a \ref Fixed32
	using the round to zero fractional truncation and store it to memory
		
	\param pOutput A valid pointer to a \ref Fixed32 to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToFixed(float), FloatToFixedFloor(Fixed32 *,float), FloatToFixedCeil(Fixed32 *,float), or FloatToFixedNearest(Fixed32 *,float)
	
***************************************/

/*! ************************************

	\fn void Burger::FloatToFixedCeil(Fixed32 *pOutput,float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using ceil().

	Convert a single precision floating point number to a \ref Fixed32
	using the ceil() form of fractional truncation and store it to memory
		
	\param pOutput A valid pointer to a \ref Fixed32 to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToFixedCeil(float), FloatToFixedFloor(Fixed32 *,float), FloatToFixed(Fixed32 *,float), or FloatToFixedNearest(Fixed32 *,float)
	
***************************************/

/*! ************************************

	\fn void Burger::FloatToFixedNearest(Fixed32 *pOutput,float fInput)
	\brief Convert a 32 bit float to a \ref Fixed32 using round to nearest.

	Convert a single precision floating point number to a \ref Fixed32
	using the round to nearest fractional truncation and store it to memory
		
	\param pOutput A valid pointer to a \ref Fixed32 to receive the result.
	\param fInput A valid single precision floating point number.
	\sa FloatToFixedNearest(float), FloatToFixedFloor(Fixed32 *,float), FloatToFixed(Fixed32 *,float), or FloatToFixedCeil(Fixed32 *,float)
	
***************************************/


/*! ************************************

	\fn int8_t Burger::Abs(int8_t iInput)
	\brief Get the absolute value of an integer.
	
	Without branching, calculate the absolute value of an integer.
		
	\param iInput Value to get the absolute value of.
	\return The absolute value of iInput.
	
	\sa Abs(int16_t), Abs(int32_t), Abs(int64_t), Abs(float) and Abs(double) 
	
***************************************/

/*! ************************************

	\fn int16_t Burger::Abs(int16_t iInput)
	\brief Get the absolute value of an integer.
	
	Without branching, calculate the absolute value of an integer.
		
	\param iInput Value to get the absolute value of.
	\return The absolute value of iInput.
	
	\sa Abs(int8_t), Abs(int32_t), Abs(int64_t), Abs(float) and Abs(double) 
	
***************************************/

/*! ************************************

	\fn int32_t Burger::Abs(int32_t iInput)
	\brief Get the absolute value of an integer.
	
	Without branching, calculate the absolute value of an integer.
		
	\param iInput Value to get the absolute value of.
	\return The absolute value of iInput.
	
	\sa Abs(int8_t), Abs(int16_t), Abs(int64_t), Abs(float) and Abs(double) 
	
***************************************/

/*! ************************************

	\fn int64_t Burger::Abs(int64_t iInput)
	\brief Get the absolute value of a 64 bit integer.
	
	Without branching, calculate the absolute value of an integer.
		
	\param iInput Value to get the absolute value of.
	\return The absolute value of iInput.
	
	\sa Abs(int8_t), Abs(int16_t), Abs(int32_t), Abs(float) and Abs(double) 
	
***************************************/



/*! ************************************

	\fn int8_t Burger::ClampZero(int8_t iInput)
	\brief Clamp an integer to 0
	
	Without branching, if the integer is less than zero, set it to zero.
		
	\param iInput Value to test for less than zero.
	\return The zero clamped value of iInput.
	
	\sa ClampZero(int16_t), ClampZero(int32_t) or ClampZero(int64_t)
	
***************************************/

/*! ************************************

	\fn int16_t Burger::ClampZero(int16_t iInput)
	\brief Clamp an integer to 0
	
	Without branching, if the integer is less than zero, set it to zero.
		
	\param iInput Value to test for less than zero.
	\return The zero clamped value of iInput.
	
	\sa ClampZero(int8_t), ClampZero(int32_t) or ClampZero(int64_t)
	
***************************************/

/*! ************************************

	\fn int32_t Burger::ClampZero(int32_t iInput)
	\brief Clamp an integer to 0
	
	Without branching, if the integer is less than zero, set it to zero.
		
	\param iInput Value to test for less than zero.
	\return The zero clamped value of iInput.
	
	\sa ClampZero(int8_t), ClampZero(int16_t) or ClampZero(int64_t)
	
***************************************/

/*! ************************************

	\fn int64_t Burger::ClampZero(int64_t iInput)
	\brief Clamp an integer to 0
	
	Without branching, if the integer is less than zero, set it to zero.
		
	\param iInput Value to test for less than zero.
	\return The zero clamped value of iInput.
	
	\sa ClampZero(int8_t), ClampZero(int16_t) or ClampZero(int32_t)
	
***************************************/



/*! ************************************

	\fn int8_t Burger::Sign(int8_t iInput)
	\brief Get the sign value of an integer.
	
	Without branching, calculate the sign value of an integer. If
	the input was zero, return zero, if it's positive, return 1 and
	if negative, return -1.
		
	\param iInput Value to get the sign value of.
	\return 0, 1 or -1.
	
	\sa Sign(int16_t), Sign(int32_t), Sign(int64_t), Sign(float) and Sign(double) 
	
***************************************/

/*! ************************************

	\fn int16_t Burger::Sign(int16_t iInput)
	\brief Get the sign value of an integer.
	
	Without branching, calculate the sign value of an integer. If
	the input was zero, return zero, if it's positive, return 1 and
	if negative, return -1.
		
	\param iInput Value to get the sign value of.
	\return 0, 1 or -1.
	
	\sa Sign(int8_t), Sign(int32_t), Sign(int64_t), Sign(float) and Sign(double) 
	
***************************************/

/*! ************************************

	\fn int32_t Burger::Sign(int32_t iInput)
	\brief Get the sign value of an integer.
	
	Without branching, calculate the sign value of an integer. If
	the input was zero, return zero, if it's positive, return 1 and
	if negative, return -1.
		
	\param iInput Value to get the sign value of.
	\return 0, 1 or -1.
	
	\sa Sign(int8_t), Sign(int16_t), Sign(int64_t), Sign(float) and Sign(double) 
	
***************************************/

/*! ************************************

	\fn int64_t Burger::Sign(int64_t iInput)
	\brief Get the sign value of a 64 bit integer.
	
	Without branching, calculate the sign value of an integer. If
	the input was zero, return zero, if it's positive, return 1 and
	if negative, return -1.
		
	\param iInput Value to get the sign value of.
	\return 0, 1 or -1.
	
	\sa Sign(int8_t), Sign(int16_t), Sign(int32_t), Sign(float) and Sign(double) 
	
***************************************/


/*! ************************************

	\fn int32_t Burger::Clamp(int32_t iInput,int32_t iMin,int32_t iMax)
	\brief Clamp the input between a bounds

	If the input value is less than the minimum, return the minimum
	or if the input value is greater than the maximum, return 
	the maximum, otherwise return the input value. No
	checking is performed to determine if the minimum is
	less than the maximum.
		
	\param iInput First value to test
	\param iMin Minimum allowed value
	\param iMax Maximum allowed value
	\return The value clamped between the bounds

	\sa Clamp(uint64_t,uint64_t,uint64_t), Clamp(int64_t,int64_t,int64_t), or Clamp(uint32_t,uint32_t,uint32_t)
	
***************************************/

/*! ************************************

	\fn int64_t Burger::Clamp(int64_t iInput,int64_t iMin,int64_t iMax)
	\brief Clamp the input between a bounds

	If the input value is less than the minimum, return the minimum
	or if the input value is greater than the maximum, return 
	the maximum, otherwise return the input value. No
	checking is performed to determine if the minimum is
	less than the maximum.
		
	\param iInput First value to test
	\param iMin Minimum allowed value
	\param iMax Maximum allowed value
	\return The value clamped between the bounds

	\sa Clamp(uint32_t,uint32_t,uint32_t), Clamp(int32_t,int32_t,int32_t), or Clamp(uint64_t,uint64_t,uint64_t)
	
***************************************/

/*! ************************************

	\fn uint32_t Burger::Clamp(uint32_t uInput,uint32_t uMin,uint32_t uMax)
	\brief Clamp the input between a bounds

	If the input value is less than the minimum, return the minimum
	or if the input value is greater than the maximum, return 
	the maximum, otherwise return the input value. No
	checking is performed to determine if the minimum is
	less than the maximum.
		
	\param uInput First value to test
	\param uMin Minimum allowed value
	\param uMax Maximum allowed value
	\return The value clamped between the bounds

	\sa Clamp(int64_t,int64_t,int64_t), Clamp(uint64_t,uint64_t,uint64_t), or Clamp(int32_t,int32_t,int32_t)
	
***************************************/

/*! ************************************

	\fn uint64_t Burger::Clamp(uint64_t uInput,uint64_t uMin,uint64_t uMax)
	\brief Clamp the input between a bounds

	If the input value is less than the minimum, return the minimum
	or if the input value is greater than the maximum, return 
	the maximum, otherwise return the input value. No
	checking is performed to determine if the minimum is
	less than the maximum.
		
	\param uInput First value to test
	\param uMin Minimum allowed value
	\param uMax Maximum allowed value
	\return The value clamped between the bounds

	\sa Clamp(int32_t,int32_t,int32_t), Clamp(uint32_t,uint32_t,uint32_t), or Clamp(int64_t,int64_t,int64_t)
	
***************************************/

/*! ************************************

	\fn Fixed32 Burger::FixedMultiply(Fixed32 fInput1,Fixed32 fInput2)
	\brief Multiply two 16.16 fixed point numbers.

	Perform a signed multiplication of two 32-bit fixed point
	numbers and return the fixed point result. No
	error checking is performed, so overflow or underflow
	conditions yield undefined results.

	\param fInput1 First signed value to multiply
	\param fInput2 Second signed value to multiply
	\return Result of fInput1 * fInput2 without bounds checking

	\sa FixedMultiplySaturate(Fixed32,Fixed32) or FixedDivide(Fixed32,Fixed32)
		
***************************************/

#if defined(BURGER_WATCOM)
#elif (defined(BURGER_X86) && defined(BURGER_METROWERKS))
#elif (defined(BURGER_X86) && defined(BURGER_MSVC))
#elif (defined(BURGER_PPC) || defined(BURGER_64BITCPU))
#else
Fixed32 BURGER_API Burger::FixedMultiply(Fixed32 fInput1,Fixed32 fInput2)
{
	int64_t iFoo = static_cast<int64_t>(fInput1)*static_cast<int64_t>(fInput2);
	return static_cast<Fixed32>(iFoo>>16);
}
#endif

/*! ************************************

	\fn Fixed32 Burger::FixedDivide(Fixed32 fInputNumerator,Fixed32 fInputDenominator)
	\brief Divide two 16.16 fixed point numbers.

	Divide a 16.16 fixed point number by another 16.16 fixed point number
	without saturation or checking for division by zero..

	\param fInputNumerator Signed value to divide
	\param fInputDenominator Signed value to divide with
	\return Result of fInputNumerator / fInputDenominator without saturation

	\note A divide by zero or an overflow can cause an exception error!

	\sa FixedDivideSaturate(Fixed32,Fixed32), FixedReciprocal(Fixed32) or FixedMultiply(Fixed32,Fixed32)

***************************************/

#if defined(BURGER_WATCOM)
#elif (defined(BURGER_X86) && defined(BURGER_METROWERKS))
#elif (defined(BURGER_X86) && defined(BURGER_MSVC))
#elif (defined(BURGER_PPC) || defined(BURGER_64BITCPU))
#else
Fixed32 BURGER_API Burger::FixedDivide(Fixed32 fInputNumerator,Fixed32 fInputDenominator)
{
	int64_t iFoo = (static_cast<int64_t>(fInputNumerator)<<16)/static_cast<int64_t>(fInputDenominator);
	return static_cast<Fixed32>(iFoo>>16);
}
#endif

/*! ************************************

	\fn Fixed32 Burger::FixedReciprocal(Fixed32 fInput)
	\brief Return the reciprocal of a fixed point number.

	Divide a 16.16 fixed point number by \ref FLOATTOFIXED(1.0f) in fixed point.
	If the input value is negative epsilon (0xFFFFFFFF), return 0x8000000. If
	the input is equal to 0 or positive epsilon (0x1), then return 0x7ffffff.
	This has the effect of saturating the output and leaving no output 
	as undefined.

	\param fInput \ref Fixed32 value to convert to a reciprocal.
	\return Result of \ref FLOATTOFIXED(1.0f) / fInput with saturation

	\note It's not recommended to input epsilon or 0 due to
	saturation.

	\sa FixedDivideSaturate(Fixed32,Fixed32) or FixedDivide(Fixed32,Fixed32)

***************************************/

#if defined(BURGER_WATCOM)
#elif (defined(BURGER_X86) && defined(BURGER_METROWERKS))
#elif (defined(BURGER_X86) && defined(BURGER_MSVC))
#elif (defined(BURGER_PPC) || defined(BURGER_64BITCPU))
#else
Fixed32 BURGER_API Burger::FixedReciprocal(Fixed32 fInput)
{
	if (fInput!=-1) {
		if (static_cast<uint32_t>(fInput)>=2) {		// Prevent a divide by zero
			uint32_t uFoo = static_cast<uint32_t>(Abs(fInput));
			uFoo = 0x80000000UL/(uFoo>>1);
			if (fInput<0) {
				return -static_cast<Fixed32>(uFoo);
			}
			return static_cast<Fixed32>(uFoo);
		}
		return MaxFixed32;
	}
	return MinFixed32;
}
#endif

/*! ************************************

	\brief Get the square root of an integer.
	
	Return the square root of an integer.
	The maximum value is 46341 for the square root
	of 0x7FFFFFFF. This routine is 100% accurate.
		
	\param uInput Value to return the square root of.
	\return Return the square root as an integer.

	\note This will perform a round to nearest
	for the fraction so if the calculated square root is 1.6, it will
	return 2.
	
	\sa Burger::Sqrt(Fixed32), Burger::Sqrt(float), and Burger::Sqrt(double)
	
***************************************/

uint32_t BURGER_API Burger::Sqrt(uint32_t uInput)
{
	// Perform the square root

#if 0
	uint_t i = 16;
	uint32_t TestValue = 0x40000000;
	uint32_t Result = 0;
	do {
		uint32_t ToTest = Result+TestValue;
		Result>>=1;
		if (uInput>=ToTest) {
			Result = Result+TestValue;
			uInput -= ToTest;
		}
		TestValue>>=2;
	} while (--i);
	if (uInput>Result) {
		Result += 1;
	}
	
#else
	// This version is the function above, but unrolled
	// for performance
	
	uint32_t Result = 0;
	if (uInput>=0x40000000) {
		Result = 0x40000000;
		uInput -= 0x40000000;
	}

	// Here is where the fun begins

	uint32_t ToTest = Result + 0x10000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x400000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x100000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x40000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x400;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x100;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x40;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1;
		uInput -= ToTest;
	}

	// Big finish!	

	if (uInput>Result) {
		Result += 1;
	}
#endif
	return Result;
}

/*! ************************************

	\brief Get the square root of a \ref Fixed32.
	
	Return the square root of a \ref Fixed32 and
	return the integer result.
	The maximum value is 255.998 (256) for the square root
	of 0x7FFFFFFF. This routine is 100% accurate.
	
	Negative numbers will return zero for the result.
		
	\param fInput Value to return the square root of.
	\return Return the square root as an integer.

	\note This will perform a round to nearest
	for the fraction so if the calculated square root is 1.6, it will
	return 2.
	
	\sa Burger::Sqrt(uint32_t)

***************************************/

uint32_t BURGER_API Burger::SqrtFixedToWord32(Fixed32 fInput)
{
	// Perform the square root
	
	if (fInput<0) {
		return 0;
	}
	uint32_t uInput = static_cast<uint32_t>(fInput);
	
	uint32_t Result = 0;
	if (uInput>=0x40000000) {
		Result = 0x40000000;
		uInput -= 0x40000000;
	}

	// Here is where the fun begins

	uint32_t ToTest = Result + 0x10000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x400000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x100000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x40000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10000;
		uInput -= ToTest;
	}

	// Get the extra bit 0.5f
	
	ToTest = Result + 0x4000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4000;
		//uInput -= ToTest;
	}

	// Add in 0.5f to round up
	Result+=0x4000;
	Result>>=15;		// Convert to int
	return Result;
}

/*! ************************************

	\brief Get the square root of a \ref Fixed32.
	
	Return the square root of a \ref Fixed32 and
	return the \ref Fixed32 result.
	The maximum value is 255.998 (256) for the square root
	of 0x7FFFFFFF. This routine is 100% accurate.
	
	Negative numbers will return zero for the result.
		
	\param fInput Value to return the square root of.
	\return Return the square root as a \ref Fixed32.
	
	\sa Burger::Sqrt(uint32_t), Burger::Sqrt(float), and Burger::Sqrt(double)

***************************************/

Fixed32 BURGER_API Burger::Sqrt(Fixed32 fInput)
{
	if (fInput<0) {
		return 0;
	}
	uint32_t uInput = static_cast<uint32_t>(fInput);

	// This is simplified since most are constants

	uint32_t Result = 0;
	if (uInput>=0x40000000) {
		Result = 0x40000000;
		uInput -= 0x40000000;
	}

	/* Here is where the fun begins */

	uint32_t ToTest = Result + 0x10000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1000000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x400000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x100000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x40000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x400;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x100;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x40;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1;
		uInput -= ToTest;
	}

	// At this point, I've got the integer square root
	// I need another 16 bits of precision

	Result <<= 16;
	uInput <<= 16;

	// Now, iterate another 8 times for the final 16 bits

	ToTest = Result + 0x4000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1000;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1000;
		uInput -= ToTest;
	}

	ToTest = Result + 0x400;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x400;
		uInput -= ToTest;
	}

	ToTest = Result + 0x100;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x100;
		uInput -= ToTest;
	}

	ToTest = Result + 0x40;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x40;
		uInput -= ToTest;
	}

	ToTest = Result + 0x10;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x10;
		uInput -= ToTest;
	}

	ToTest = Result + 0x4;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x4;
		uInput -= ToTest;
	}

	ToTest = Result + 0x1;
	Result>>= 1;
	if (uInput>=ToTest) {
		Result =Result+0x1;
		uInput -= ToTest;
	}

	/* Big finish! */

	if (uInput>Result) {
		Result += 1;
	}
	return static_cast<Fixed32>(Result);
}

/*! ************************************

	\fn uint32_t Burger::RotateLeft(uint32_t uInput,uint_t uShiftCount)
	\brief Rotate the bits left
	
	Using a compiler intrinsic where supported, rotate the bits in
	the 32 bit register to the left.
		
	\param uInput Value to rotate left
	\param uShiftCount 0 to 31, number of bits to shift.
	\return The value with the bits rotated
	
	\sa RotateLeft(uint64_t,uint_t) or RotateRight(uint32_t,uint_t)
	
***************************************/

/*! ************************************

	\fn uint32_t Burger::RotateRight(uint32_t uInput,uint_t uShiftCount)
	\brief Rotate the bits right
	
	Using a compiler intrinsic where supported, rotate the bits in
	the 32 bit register to the right.
		
	\param uInput Value to rotate right
	\param uShiftCount 0 to 31, number of bits to shift.
	\return The value with the bits rotated
	
	\sa RotateRight(uint64_t,uint_t) or RotateLeft(uint32_t,uint_t)
	
***************************************/

/*! ************************************

	\fn uint64_t Burger::RotateLeft(uint64_t uInput,uint_t uShiftCount)
	\brief Rotate the bits left
	
	Using a compiler intrinsic where supported, rotate the bits in
	the 64 bit register to the left.
		
	\param uInput Value to rotate left
	\param uShiftCount 0 to 63, number of bits to shift.
	\return The value with the bits rotated
	
	\sa RotateLeft(uint32_t,uint_t) or RotateRight(uint64_t,uint_t)
	
***************************************/

/*! ************************************

	\fn uint64_t Burger::RotateRight(uint64_t uInput,uint_t uShiftCount)
	\brief Rotate the bits right
	
	Using a compiler intrinsic where supported, rotate the bits in
	the 64 bit register to the right.
		
	\param uInput Value to rotate right
	\param uShiftCount 0 to 63, number of bits to shift.
	\return The value with the bits rotated
	
	\sa RotateRight(uint32_t,uint_t) or RotateLeft(uint64_t,uint_t)
	
***************************************/
