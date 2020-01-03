/***************************************

	Fixed point math functions

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFIXEDPOINT_H__
#define __BRFIXEDPOINT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRALGORITHM_H__
#include "bralgorithm.h"
#endif

#ifndef __BRWATCOM_H__
#include "brwatcom.h"
#endif

#ifndef __BRVISUALSTUDIO_H__
#include "brvisualstudio.h"
#endif

/* BEGIN */
#if defined(BURGER_INTEL) || defined(DOXYGEN)
extern "C" const float g_fBurgerIntMathNearesttable[2];
extern "C" const float g_fBurgerMath65536;
#endif

#define FLOATTOFIXED(x) static_cast<Fixed32>((x)*65536.0f)
#define FIXEDTOFLOAT(x) (static_cast<float>(x)*(1.0f/65536.0f))
#define INTTOFIXED(x) static_cast<Fixed32>((x)<<16)
#define FIXEDTOINT(x) static_cast<int>((x+((x>>31)&0xFFFF))>>16)

namespace Burger {
	const Word16 MaxWord16 = 0xFFFFU;					///< Maximum value for \ref Word16 (Minimum is 0)
	const Word32 MaxWord32 = 0xFFFFFFFFU;				///< Maximum value for \ref Word32 (Minimum is 0)
	const Word64 MaxWord64 = 0xFFFFFFFFFFFFFFFFULL;		///< Maximum value for \ref Word64 (Minimum is 0)
	const Int16 MinInt16 = (-0x7FFF)-1;					///< Minimum value for \ref Int16
	const Int16 MaxInt16 = 0x7FFF;						///< Maximum value for \ref Int16
	const Int32 MinInt32 = (-0x7FFFFFFF)-1;				///< Minimum value for \ref Int32
	const Int32 MaxInt32 = 0x7FFFFFFF;					///< Maximum value for \ref Int32
	const Int64 MinInt64 = (-0x7FFFFFFFFFFFFFFFLL)-1;	///< Minimum value for \ref Int64
	const Int64 MaxInt64 = 0x7FFFFFFFFFFFFFFFLL;		///< Maximum value for \ref Int64
	const Frac32 MinFrac32 = (-0x7FFFFFFF)-1;			///< Minimum value for \ref Frac32
	const Frac32 MaxFrac32 = 0x7FFFFFFF;				///< Maximum value for \ref Frac32
	const Fixed32 MinFixed32 = (-0x7FFFFFFF)-1;			///< Minimum value for \ref Fixed32
	const Fixed32 MaxFixed32 = 0x7FFFFFFF;				///< Maximum value for \ref Fixed32
	const Fixed32 PiFixed32 = 0x003243F;				///< Pi in \ref Fixed32 format (3.141... * 65536 = 205887.416)
	BURGER_INLINE Word GetLoWord(Word32 uInput)	{ return (uInput&0xFFFFU); }
	BURGER_INLINE Word GetHiWord(Word32 uInput) { return (uInput>>16); }
	BURGER_INLINE Fixed32 IntToFixed(Int32 iInput) { return static_cast<Fixed32>(iInput<<16); }
	extern Fixed32 BURGER_API IntToFixedSaturate(Int32 iInput);
	BURGER_INLINE Int32 FixedToIntFloor(Fixed32 fInput) { return static_cast<Int32>(fInput>>16); }
	BURGER_INLINE Int32 FixedToInt(Fixed32 fInput) { return static_cast<Int32>((fInput+((fInput>>31)&0xFFFF))>>16); }
	BURGER_INLINE Int32 FixedToIntCeil(Fixed32 fInput) { return static_cast<Int32>((fInput+0xFFFF)>>16); }
	BURGER_INLINE Int32 FixedToIntNearest(Fixed32 fInput) { return static_cast<Int32>((fInput+0x8000-(static_cast<Word32>(fInput)>>31U)))>>16; }

	extern Int32 BURGER_API FloatToIntFloor(float fInput);
	extern void BURGER_API FloatToIntFloor(Int32 *pOutput,float fInput);
	extern Int32 BURGER_API FloatToIntCeil(float fInput);
	extern void BURGER_API FloatToIntCeil(Int32 *pOutput,float fInput);
	extern Int32 BURGER_API FloatToIntRound(float fInput);
	extern void BURGER_API FloatToIntRound(Int32 *pOutput,float fInput);
	extern Int32 BURGER_API FloatToIntRoundToZero(float fInput);
	extern void BURGER_API FloatToIntRoundToZero(Int32 *pOutput,float fInput);

#if defined(BURGER_WATCOM)
	BURGER_INLINE Fixed32 FloatToFixedFloor(float fInput) { return BurgerFixedMathFromFloatFloor(fInput); }
	BURGER_INLINE Fixed32 FloatToFixed(float fInput) { return BurgerFixedMathFromFloat(fInput); }
	BURGER_INLINE Fixed32 FloatToFixedCeil(float fInput) { return BurgerFixedMathFromFloatCeil(fInput); }
	BURGER_INLINE Fixed32 FloatToFixedNearest(float fInput) { return BurgerFixedMathFromFloatNearest(fInput); }
	BURGER_INLINE void FloatToFixedFloor(Fixed32 *pOutput,float fInput) { BurgerFixedMathFromFloatFloor2(pOutput,fInput); }
	BURGER_INLINE void FloatToFixed(Fixed32 *pOutput,float fInput) { BurgerFixedMathFromFloat2(pOutput,fInput); }
	BURGER_INLINE void FloatToFixedCeil(Fixed32 *pOutput,float fInput) { BurgerFixedMathFromFloatCeil2(pOutput,fInput); }
	BURGER_INLINE void FloatToFixedNearest(Fixed32 *pOutput,float fInput) { BurgerFixedMathFromFloatNearest2(pOutput,fInput); }
#elif defined(BURGER_X86) && defined(BURGER_METROWERKS)

	BURGER_INLINE Fixed32 FloatToFixedFloor(float fInput) { 
		asm {
			fld DWORD PTR fInput
			fmul DWORD PTR [g_fBurgerMath65536]
			fadd DWORD PTR [g_fBurgerIntMathNearesttable]
			fistp DWORD PTR fInput
		}
		return reinterpret_cast<Fixed32 *>(&fInput)[0];
	}

	BURGER_INLINE Fixed32 FloatToFixed(float fInput) {
		asm {
			fld DWORD PTR fInput
			fmul DWORD PTR [g_fBurgerMath65536]
			mov eax,fInput
			shr eax,31
			fadd dword ptr [g_fBurgerIntMathNearesttable+eax*4]
			fistp DWORD PTR fInput
		}
		return reinterpret_cast<Fixed32 *>(&fInput)[0]; 
	}

	BURGER_INLINE Fixed32 FloatToFixedCeil(float fInput) { 
		asm {
			fld DWORD PTR fInput
			fmul DWORD PTR [g_fBurgerMath65536]
			fadd DWORD PTR [g_fBurgerIntMathNearesttable+4]
			fistp DWORD PTR fInput
		}
		return reinterpret_cast<Fixed32 *>(&fInput)[0]; 
	}

	BURGER_INLINE Fixed32 FloatToFixedNearest(float fInput) {
		asm {
			fld DWORD PTR fInput
			fmul DWORD PTR [g_fBurgerMath65536]
			fistp DWORD PTR fInput
		}
		return reinterpret_cast<Fixed32 *>(&fInput)[0]; 
	}

	BURGER_INLINE void FloatToFixedFloor(Fixed32 *pOutput,float fInput) {
		asm {
			mov ecx,pOutput
			fld DWORD PTR fInput
			fmul DWORD PTR [g_fBurgerMath65536]
			fadd DWORD PTR [g_fBurgerIntMathNearesttable]
			fistp DWORD PTR [ecx] 
		}
	}

	BURGER_INLINE void FloatToFixed(Fixed32 *pOutput,float fInput) { 
		asm {
			mov ecx,pOutput
			fld DWORD PTR fInput
			fmul DWORD PTR [g_fBurgerMath65536]
			mov eax,DWORD PTR [fInput]
			shr eax,31
			fadd dword ptr [g_fBurgerIntMathNearesttable+eax*4]
			fistp DWORD PTR [ecx] 
		}
	}

	BURGER_INLINE void FloatToFixedCeil(Fixed32 *pOutput,float fInput) {
		asm {
			mov ecx,pOutput
			fld DWORD PTR fInput
			fmul DWORD PTR [g_fBurgerMath65536]
			fadd DWORD PTR [g_fBurgerIntMathNearesttable+4]
			fistp DWORD PTR [ecx]
		}
	}

	BURGER_INLINE void FloatToFixedNearest(Fixed32 *pOutput,float fInput) {
		asm {
			mov ecx,pOutput
			fld DWORD PTR fInput
			fmul DWORD PTR [g_fBurgerMath65536]
			fistp DWORD PTR [ecx] 
		}
	}

#elif defined(BURGER_X86) && defined(BURGER_MSVC)
	BURGER_INLINE Int32 FloatToFixedFloor(float fInput) {
		__asm {
			fld DWORD PTR [fInput]
			fmul DWORD PTR [g_fBurgerMath65536]
			fadd DWORD PTR [g_fBurgerIntMathNearesttable]
			fistp DWORD PTR [fInput]
		}
		return reinterpret_cast<Fixed32 *>(&fInput)[0]; 
	}
	BURGER_INLINE Int32 FloatToFixed(float fInput) { 
		__asm {
			fld DWORD PTR [fInput]
			fmul DWORD PTR [g_fBurgerMath65536]
			mov eax,[fInput]
			shr eax,31
			fadd dword ptr [g_fBurgerIntMathNearesttable+eax*4]
			fistp DWORD PTR [fInput]
		}
		return reinterpret_cast<Fixed32 *>(&fInput)[0]; 
	}
	BURGER_INLINE Int32 FloatToFixedCeil(float fInput) { 
		__asm {
			fld DWORD PTR [fInput]
			fmul DWORD PTR [g_fBurgerMath65536]
			fadd DWORD PTR [g_fBurgerIntMathNearesttable+4]
			fistp DWORD PTR [fInput]
		}
		return reinterpret_cast<Fixed32 *>(&fInput)[0]; 
	}
	BURGER_INLINE Int32 FloatToFixedNearest(float fInput) { 
		__asm {
			fld DWORD PTR [fInput]
			fmul DWORD PTR [g_fBurgerMath65536]
			fistp DWORD PTR [fInput]
		}
		return reinterpret_cast<Fixed32 *>(&fInput)[0]; 
	}
	BURGER_INLINE void FloatToFixedFloor(Fixed32 *pOutput,float fInput) { 
		__asm {
			mov ecx,pOutput
			fld DWORD PTR [fInput]
			fmul DWORD PTR [g_fBurgerMath65536]
			fadd DWORD PTR [g_fBurgerIntMathNearesttable]
			fistp DWORD PTR [ecx] 
		}
	}
	BURGER_INLINE void FloatToFixed(Fixed32 *pOutput,float fInput) { 
		__asm {
			mov ecx,pOutput
			fld DWORD PTR [fInput]
			fmul DWORD PTR [g_fBurgerMath65536]
			mov eax,[fInput]
			shr eax,31
			fadd dword ptr [g_fBurgerIntMathNearesttable+eax*4]
			fistp DWORD PTR [ecx] 
		}
	}
	BURGER_INLINE void FloatToFixedCeil(Fixed32 *pOutput,float fInput) {
		__asm {
			mov ecx,pOutput
			fld DWORD PTR [fInput]
			fmul DWORD PTR [g_fBurgerMath65536]
			fadd DWORD PTR [g_fBurgerIntMathNearesttable+4]
			fistp DWORD PTR [ecx] 
		}
	}
	BURGER_INLINE void FloatToFixedNearest(Fixed32 *pOutput,float fInput) { 
		__asm {
			mov ecx,pOutput
			fld DWORD PTR [fInput]
			fmul DWORD PTR [g_fBurgerMath65536]
			fistp DWORD PTR [ecx] 
		}
	}

#else
	BURGER_INLINE Fixed32 FloatToFixedFloor(float fInput) { return static_cast<Fixed32>(FloatToIntFloor(fInput*65536.0f)); }
	BURGER_INLINE Fixed32 FloatToFixed(float fInput) { return static_cast<Fixed32>(FloatToIntRoundToZero(fInput*65536.0f)); }
	BURGER_INLINE Fixed32 FloatToFixedCeil(float fInput) { return static_cast<Fixed32>(FloatToIntCeil(fInput*65536.0f)); }
	BURGER_INLINE Fixed32 FloatToFixedNearest(float fInput) { return static_cast<Fixed32>(FloatToIntRound(fInput*65536.0f)); }
	BURGER_INLINE void FloatToFixedFloor(Fixed32 *pOutput,float fInput) { FloatToIntFloor(reinterpret_cast<Int32*>(pOutput),fInput*65536.0f); }
	BURGER_INLINE void FloatToFixed(Fixed32 *pOutput,float fInput) { FloatToIntRoundToZero(reinterpret_cast<Int32*>(pOutput),fInput*65536.0f); }
	BURGER_INLINE void FloatToFixedCeil(Fixed32 *pOutput,float fInput) { FloatToIntCeil(reinterpret_cast<Int32*>(pOutput),fInput*65536.0f); }
	BURGER_INLINE void FloatToFixedNearest(Fixed32 *pOutput,float fInput) { FloatToIntRound(reinterpret_cast<Int32*>(pOutput),fInput*65536.0f); }
#endif

#if defined(BURGER_ARM) || defined(BURGER_AMD64) || (defined(BURGER_X86) && !defined(BURGER_WINDOWS)) || defined(DOXYGEN)
	BURGER_INLINE Int8 Abs(Int8 iInput) { if (iInput<0) { iInput=-iInput; } return iInput; }
	BURGER_INLINE Int16 Abs(Int16 iInput) BURGER_NOEXCEPT { if (iInput<0) { iInput=-iInput; } return iInput; }
	BURGER_INLINE Int32 Abs(Int32 iInput) BURGER_NOEXCEPT { if (iInput<0) { iInput=-iInput; } return iInput; }
	BURGER_INLINE Int64 Abs(Int64 iInput) BURGER_NOEXCEPT { if (iInput<0) { iInput=-iInput; } return iInput; }
	BURGER_INLINE Int8 ClampZero(Int8 iInput) BURGER_NOEXCEPT { if (iInput<0) { iInput=0; } return iInput; }
	BURGER_INLINE Int16 ClampZero(Int16 iInput) BURGER_NOEXCEPT { if (iInput<0) { iInput=0; } return iInput; }
	BURGER_INLINE Int32 ClampZero(Int32 iInput) BURGER_NOEXCEPT { if (iInput<0) { iInput=0; } return iInput; }
	BURGER_INLINE Int64 ClampZero(Int64 iInput) BURGER_NOEXCEPT { if (iInput<0) { iInput=0; } return iInput; }
#else
	BURGER_INLINE int8_t Abs(int8_t iInput) BURGER_NOEXCEPT { const int8_t iMask = static_cast<int8_t>(iInput>>7); return static_cast<int8_t>((iInput^iMask)-iMask); }
	BURGER_INLINE int16_t Abs(int16_t iInput) BURGER_NOEXCEPT { int16_t iMask = static_cast<int16_t>(iInput>>15); return static_cast<int16_t>((iInput^iMask)-iMask); }
	BURGER_INLINE Int32 Abs(Int32 iInput) BURGER_NOEXCEPT { Int32 iMask = (iInput>>31); return (iInput^iMask)-iMask; }
	BURGER_INLINE Int64 Abs(Int64 iInput) BURGER_NOEXCEPT { Int64 iMask = (iInput>>63); return (iInput^iMask)-iMask; }
	BURGER_INLINE BURGER_CONSTEXPR int8_t ClampZero(int8_t iInput) BURGER_NOEXCEPT { return static_cast<int8_t>((~(iInput>>7))&iInput); }
	BURGER_INLINE BURGER_CONSTEXPR int16_t ClampZero(int16_t iInput) BURGER_NOEXCEPT { return static_cast<int16_t>((~(iInput>>15))&iInput); }
	BURGER_INLINE BURGER_CONSTEXPR Int32 ClampZero(Int32 iInput) BURGER_NOEXCEPT { return (~(iInput>>31))&iInput; }
	BURGER_INLINE BURGER_CONSTEXPR Int64 ClampZero(Int64 iInput) BURGER_NOEXCEPT { return (~(iInput>>63))&iInput; }
#endif

	BURGER_INLINE BURGER_CONSTEXPR int8_t Sign(int8_t iInput) BURGER_NOEXCEPT { return static_cast<int8_t>((iInput>>7) | static_cast<int8_t>(static_cast<uint8_t>(-iInput)>>7U)); }
	BURGER_INLINE BURGER_CONSTEXPR int16_t Sign(int16_t iInput) BURGER_NOEXCEPT { return static_cast<int16_t>((iInput>>15) | static_cast<int16_t>(static_cast<uint16_t>(-iInput)>>15U)); }
	BURGER_INLINE BURGER_CONSTEXPR Int32 Sign(Int32 iInput) BURGER_NOEXCEPT { return (iInput>>31) | static_cast<Int32>(static_cast<Word32>(-iInput)>>31U); }
	BURGER_INLINE BURGER_CONSTEXPR Int64 Sign(Int64 iInput) BURGER_NOEXCEPT { return (iInput>>63) | static_cast<Int64>(static_cast<Word64>(-iInput)>>63U); }

	BURGER_INLINE Int32 Clamp(Int32 iInput,Int32 iMin,Int32 iMax) BURGER_NOEXCEPT { iInput = Max(iInput,iMin); return Min(iInput,iMax); }
	BURGER_INLINE Int64 Clamp(Int64 iInput,Int64 iMin,Int64 iMax) BURGER_NOEXCEPT { iInput = Max(iInput,iMin); return Min(iInput,iMax); }
	BURGER_INLINE Word32 Clamp(Word32 uInput,Word32 uMin,Word32 uMax) BURGER_NOEXCEPT { uInput = Max(uInput,uMin); return Min(uInput,uMax); }
	BURGER_INLINE Word64 Clamp(Word64 uInput,Word64 uMin,Word64 uMax) BURGER_NOEXCEPT { uInput = Max(uInput,uMin); return Min(uInput,uMax); }

#if defined(BURGER_WATCOM)
	BURGER_INLINE Fixed32 FixedMultiply(Fixed32 fInput1,Fixed32 fInput2) { return BurgerFixedMathMultiply(fInput1,fInput2); }
	BURGER_INLINE Fixed32 FixedDivide(Fixed32 fInputNumerator,Fixed32 fInputDenominator) { return BurgerFixedMathDivide(fInputNumerator,fInputDenominator); }
	BURGER_INLINE Fixed32 FixedReciprocal(Fixed32 fInput) { return BurgerFixedMathReciprocal(fInput); }
#elif defined(BURGER_X86) && defined(BURGER_METROWERKS)
	BURGER_INLINE Fixed32 FixedMultiply(Fixed32 fInput1,Fixed32 fInput2) { asm mov eax,fInput1
		asm imul fInput2
		asm shrd eax,edx,16
		asm mov fInput1,eax
		return fInput1; }
	BURGER_INLINE Fixed32 FixedDivide(Fixed32 fInputNumerator,Fixed32 fInputDenominator) { asm mov eax,fInputNumerator
		asm mov edx,fInputNumerator
		asm shl eax,16
		asm sar edx,16
		asm idiv fInputDenominator
		asm mov fInputNumerator,eax
		return fInputNumerator; }
	BURGER_INLINE Fixed32 FixedReciprocal(Fixed32 fInput) { asm mov ecx,fInput
		asm cmp ecx,-1
		asm mov eax,080000000H
		asm je Done
		asm dec eax
		asm cmp ecx,2
		asm jb Done
		asm xor eax,eax
		asm mov edx,1
		asm idiv ecx
	Done:
		asm mov fInput,eax
		return fInput; }
#elif defined(BURGER_X86) && defined(BURGER_MSVC)
	BURGER_INLINE Fixed32 FixedMultiply(Fixed32 fInput1,Fixed32 fInput2) { __asm mov eax,fInput1
		__asm mov edx,fInput2
		__asm imul edx
		__asm shrd eax,edx,16
		__asm mov fInput1,eax
		return fInput1; }
	BURGER_INLINE Fixed32 FixedDivide(Fixed32 fInputNumerator,Fixed32 fInputDenominator) { __asm mov eax,fInputNumerator
		__asm mov ebx,fInputDenominator
		__asm mov edx,eax
		__asm shl eax,16
		__asm sar edx,16
		__asm idiv ebx
		__asm mov fInputNumerator,eax
		return fInputNumerator; }
	BURGER_INLINE Fixed32 FixedReciprocal(Fixed32 fInput) { 
		if (fInput==-1) {		// Note: Visual C does not like branches in inline asm
			fInput=0x80000000;
		} else if (static_cast<Word32>(fInput)<2) {
			fInput=0x7FFFFFFF;
		} else {
			__asm mov ecx,fInput
			__asm xor eax,eax
			__asm mov edx,1
			__asm idiv ecx
			__asm mov fInput,eax
		}
		return fInput;
	}
#elif defined(BURGER_PPC) || defined(BURGER_64BITCPU) && !defined(DOXYGEN)
	BURGER_INLINE Fixed32 FixedMultiply(Fixed32 fInput1,Fixed32 fInput2) { return static_cast<Fixed32>(((static_cast<Int64>(fInput1)*static_cast<Int64>(fInput2))>>16)&0xFFFFFFFF); }
	BURGER_INLINE Fixed32 FixedDivide(Fixed32 fInputNumerator,Fixed32 fInputDenominator) { return static_cast<Fixed32>((static_cast<Int64>(fInputNumerator)<<16)/fInputDenominator); }
	BURGER_INLINE Fixed32 FixedReciprocal(Fixed32 fInput) { 
		if (fInput==-1) {
			fInput = 0x80000000;
		} else if (static_cast<Word32>(fInput)<2) {
			fInput = 0x7FFFFFFF;
		} else {
			fInput = static_cast<Fixed32>(0x100000000LL/static_cast<Int64>(fInput));
		}
		return fInput;
	}
#else
	extern Fixed32 BURGER_API FixedMultiply(Fixed32 fInput1,Fixed32 fInput2);
	extern Fixed32 BURGER_API FixedDivide(Fixed32 fInputNumerator,Fixed32 fInputDenominator);
	extern Fixed32 BURGER_API FixedReciprocal(Fixed32 fInput);
#endif

	extern Word32 BURGER_API Sqrt(Word32 uInput);
	extern Word32 BURGER_API SqrtFixedToWord32(Fixed32 fInput);
	extern Fixed32 BURGER_API Sqrt(Fixed32 uInput);

#if defined(BURGER_MSVC) || defined(BURGER_WATCOM)
	BURGER_INLINE Word32 RotateLeft(Word32 uInput,Word uShiftCount) { return static_cast<Word32>(_rotl(uInput,uShiftCount)); }
	BURGER_INLINE Word32 RotateRight(Word32 uInput,Word uShiftCount) { return static_cast<Word32>(_rotr(uInput,uShiftCount)); }
#elif (defined(BURGER_METROWERKS) && (defined(BURGER_X86) || defined(BURGER_68K)))
	BURGER_INLINE Word32 RotateLeft(Word32 uInput,Word uShiftCount) { return static_cast<Word32>(__rol(uInput,uShiftCount)); }
	BURGER_INLINE Word32 RotateRight(Word32 uInput,Word uShiftCount) { return static_cast<Word32>(__ror(uInput,uShiftCount)); }
#elif (defined(BURGER_METROWERKS) && defined(BURGER_PPC))
	BURGER_INLINE Word32 RotateLeft(Word32 uInput,Word uShiftCount) { return static_cast<Word32>(__builtin___rotate_left32(uInput,static_cast<int>(uShiftCount))); }
	BURGER_INLINE Word32 RotateRight(Word32 uInput,Word uShiftCount) { return static_cast<Word32>(__builtin___rotate_right32(uInput,static_cast<int>(uShiftCount))); }
#elif defined(BURGER_VITA)
	BURGER_INLINE Word32 RotateLeft(Word32 uInput,Word uShiftCount) { return __builtin_rol(uInput,static_cast<Word32>(uShiftCount)); }
	BURGER_INLINE Word32 RotateRight(Word32 uInput,Word uShiftCount) { return __builtin_ror(uInput,static_cast<Word32>(uShiftCount)); }
#else
	BURGER_INLINE Word32 RotateLeft(Word32 uInput,Word uShiftCount) { return ((uInput<<uShiftCount) | (uInput>>(32-uShiftCount))); }
	BURGER_INLINE Word32 RotateRight(Word32 uInput,Word uShiftCount) { return ((uInput>>uShiftCount) | (uInput<<(32-uShiftCount))); }
#endif

#if defined(BURGER_MSVC)
	BURGER_INLINE Word64 RotateLeft(Word64 uInput,Word uShiftCount) { return _rotl64(uInput,uShiftCount); }
	BURGER_INLINE Word64 RotateRight(Word64 uInput,Word uShiftCount) { return _rotr64(uInput,uShiftCount); }
#else
	BURGER_INLINE Word64 RotateLeft(Word64 uInput,Word uShiftCount) { return ((uInput<<uShiftCount) | (uInput>>(64-uShiftCount))); }
	BURGER_INLINE Word64 RotateRight(Word64 uInput,Word uShiftCount) { return ((uInput>>uShiftCount) | (uInput<<(64-uShiftCount))); }
#endif

}
/* END */

#endif
