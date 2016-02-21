/***************************************

	Fixed point math functions

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRWATCOM_H__
#include "brwatcom.h"
#endif

#ifndef __BRVISUALSTUDIO_H__
#include "brvisualstudio.h"
#endif

/* BEGIN */
#if defined(BURGER_INTELARCHITECTURE) || defined(DOXYGEN)
extern "C" const float g_fBurgerIntMathNearesttable[2];
extern "C" const float g_fBurgerMath65536;
#endif

#define FLOATTOFIXED(x) static_cast<Fixed32>((x)*65536.0f)
#define FIXEDTOFLOAT(x) (static_cast<float>(x)*(1.0f/65536.0f))
#define INTTOFIXED(x) static_cast<Fixed32>((x)<<16)
#define FIXEDTOINT(x) static_cast<int>((x+((x>>31)&0xFFFF))>>16)

namespace Burger {
	const Word16 MaxWord16 = 0xFFFFU;			///< Maximum value for \ref Word16 (Minimum is 0)
	const Word32 MaxWord32 = 0xFFFFFFFFU;		///< Maximum value for \ref Word32 (Minimum is 0)
	const Word64 MaxWord64 = 0xFFFFFFFFFFFFFFFFULL;	///< Maximum value for \ref Word64 (Minimum is 0)
	const Int16 MinInt16 = (-0x7FFF)-1;			///< Minimum value for \ref Int16
	const Int16 MaxInt16 = 0x7FFF;				///< Maximum value for \ref Int16
	const Int32 MinInt32 = (-0x7FFFFFFF)-1;	///< Minimum value for \ref Int32
	const Int32 MaxInt32 = 0x7FFFFFFF;			///< Maximum value for \ref Int32
	const Int64 MinInt64 = (-0x7FFFFFFFFFFFFFFFLL)-1;	///< Minimum value for \ref Int64
	const Int64 MaxInt64 = 0x7FFFFFFFFFFFFFFFLL;		///< Maximum value for \ref Int64
	const Frac32 MinFrac32 = (-0x7FFFFFFF)-1;	///< Minimum value for \ref Frac32
	const Frac32 MaxFrac32 = 0x7FFFFFFF;		///< Maximum value for \ref Frac32
	const Fixed32 MinFixed32 = (-0x7FFFFFFF)-1;///< Minimum value for \ref Fixed32
	const Fixed32 MaxFixed32 = 0x7FFFFFFF;		///< Maximum value for \ref Fixed32
	const Fixed32 PiFixed32 = 0x003243F;		///< Pi in \ref Fixed32 format (3.141... * 65536 = 205887.416)
	BURGER_INLINE Word GetLoWord(Word32 uInput) { return (uInput&0xFFFFU); }
	BURGER_INLINE Word GetHiWord(Word32 uInput) { return (uInput>>16); }
	BURGER_INLINE Fixed32 IntToFixed(Int32 iInput) { return static_cast<Fixed32>(iInput<<16); }
	extern Fixed32 BURGER_API IntToFixedSaturate(Int32 iInput);
	BURGER_INLINE Int32 FixedToIntFloor(Fixed32 fInput) { return static_cast<Int32>(fInput>>16); }
	BURGER_INLINE Int32 FixedToInt(Fixed32 fInput) { return static_cast<Int32>((fInput+((fInput>>31)&0xFFFF))>>16); }
	BURGER_INLINE Int32 FixedToIntCeil(Fixed32 fInput) { return static_cast<Int32>((fInput+0xFFFF)>>16); }
	BURGER_INLINE Int32 FixedToIntNearest(Fixed32 fInput) { return static_cast<Int32>((fInput+0x8000)>>16); }
#if defined(BURGER_WATCOM)
	BURGER_INLINE Int32 FloatToIntFloor(float fInput) { return BurgerIntMathFromFloatFloor(fInput); }
	BURGER_INLINE Int32 FloatToInt(float fInput) { return BurgerIntMathFromFloat(fInput); }
	BURGER_INLINE Int32 FloatToIntCeil(float fInput) { return BurgerIntMathFromFloatCeil(fInput); }
	BURGER_INLINE Int32 FloatToIntNearest(float fInput) { return BurgerIntMathFromFloatNearest(fInput); }
	BURGER_INLINE void FloatToIntFloor(Int32 *pOutput,float fInput) { BurgerIntMathFromFloatFloor2(pOutput,fInput); }
	BURGER_INLINE void FloatToInt(Int32 *pOutput,float fInput) { BurgerIntMathFromFloat2(pOutput,fInput); }
	BURGER_INLINE void FloatToIntCeil(Int32 *pOutput,float fInput) { BurgerIntMathFromFloatCeil2(pOutput,fInput); }
	BURGER_INLINE void FloatToIntNearest(Int32 *pOutput,float fInput) { BurgerIntMathFromFloatNearest2(pOutput,fInput); }
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
	BURGER_INLINE Int32 FloatToIntFloor(float fInput) { Int32 iResult; __stfiwx(__fctiw(fInput-0.5f),0,&iResult); return iResult; }
	BURGER_INLINE Int32 FloatToInt(float fInput) { Int32 iResult; __stfiwx(__fctiwz(fInput),0,&iResult); return iResult; }
	BURGER_INLINE Int32 FloatToIntCeil(float fInput) { Int32 iResult; __stfiwx(__fctiw(fInput+(0.5f-(1.0f/(65536.0f*256.0f)))),0,&iResult); return iResult; }
	BURGER_INLINE Int32 FloatToIntNearest(float fInput) { Int32 iResult; __stfiwx(__fctiw(fInput),0,&iResult); return iResult; }
#if _MSC_FULL_VER!=14007151	// This compiler has a bug
	BURGER_INLINE void FloatToIntFloor(Int32 *pOutput,float fInput) { __stfiwx(__fctiw(fInput-0.5f),0,pOutput); }
	BURGER_INLINE void FloatToInt(Int32 *pOutput,float fInput) { __stfiwx(__fctiwz(fInput),0,pOutput); }
	BURGER_INLINE void FloatToIntCeil(Int32 *pOutput,float fInput) { __stfiwx(__fctiw(fInput+(0.5f-(1.0f/(65536.0f*256.0f)))),0,pOutput); }
	BURGER_INLINE void FloatToIntNearest(Int32 *pOutput,float fInput) { __stfiwx(__fctiw(fInput),0,pOutput); }
#else	// Workaround until the bug is fixed
	BURGER_INLINE void FloatToIntFloor(Int32 *pOutput,float fInput) { pOutput[0] = FloatToIntFloor(fInput); }
	BURGER_INLINE void FloatToInt(Int32 *pOutput,float fInput) { pOutput[0] = FloatToInt(fInput); }
	BURGER_INLINE void FloatToIntCeil(Int32 *pOutput,float fInput) { pOutput[0] = FloatToIntCeil(fInput); }
	BURGER_INLINE void FloatToIntNearest(Int32 *pOutput,float fInput) { pOutput[0] = FloatToIntNearest(fInput); }
#endif
#elif defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
	BURGER_INLINE Int32 FloatToIntFloor(register float fInput) { volatile Int32 iResult; fInput = fInput-0.5f; register volatile Int32 *pResult = &iResult; asm { fctiw fInput,fInput; stfiwx fInput,r0,pResult } return iResult; }
	BURGER_INLINE Int32 FloatToInt(register float fInput) { volatile Int32 iResult; register volatile Int32 *pResult = &iResult; asm { fctiwz fInput,fInput; stfiwx fInput,r0,pResult } return iResult; }
	BURGER_INLINE Int32 FloatToIntCeil(register float fInput) { volatile Int32 iResult; fInput = fInput+(0.5f-(1.0f/(65536.0f*256.0f))); register volatile Int32 *pResult = &iResult; asm { fctiw fInput,fInput; stfiwx fInput,r0,pResult } return iResult; }
	BURGER_INLINE Int32 FloatToIntNearest(register float fInput) { volatile Int32 iResult; register volatile Int32 *pResult = &iResult; asm { fctiw fInput,fInput; stfiwx fInput,r0,pResult } return iResult; }
	BURGER_INLINE void FloatToIntFloor(register Int32 *pOutput,register float fInput) { fInput = fInput-0.5f; asm { fctiw fInput,fInput; stfiwx fInput,r0,pOutput } }
	BURGER_INLINE void FloatToInt(register Int32 *pOutput,register float fInput) { asm { fctiwz fInput,fInput; stfiwx fInput,r0,pOutput } }
	BURGER_INLINE void FloatToIntCeil(register Int32 *pOutput,register float fInput) { fInput = fInput+(0.5f-(1.0f/(65536.0f*256.0f))); asm { fctiw fInput,fInput; stfiwx fInput,r0,pOutput } }
	BURGER_INLINE void FloatToIntNearest(register Int32 *pOutput,register float fInput) { asm { fctiw fInput,fInput; stfiwx fInput,r0,pOutput } }
#elif defined(BURGER_X86) && defined(BURGER_METROWERKS)
	BURGER_INLINE Int32 FloatToIntFloor(float fInput) { asm fld DWORD PTR fInput
		asm fadd DWORD PTR [g_fBurgerIntMathNearesttable]
		asm fistp DWORD PTR fInput
		return reinterpret_cast<Int32 *>(&fInput)[0];}
	BURGER_INLINE Int32 FloatToInt(float fInput) { asm fld DWORD PTR fInput
		asm mov eax,fInput
		asm shr eax,31
		asm fadd dword ptr [g_fBurgerIntMathNearesttable+eax*4]
		asm fistp DWORD PTR fInput
		return reinterpret_cast<Int32 *>(&fInput)[0]; }
	BURGER_INLINE Int32 FloatToIntCeil(float fInput) { asm fld DWORD PTR fInput
		asm fadd DWORD PTR [g_fBurgerIntMathNearesttable+4]
		asm fistp DWORD PTR fInput
		return reinterpret_cast<Int32 *>(&fInput)[0]; }
	BURGER_INLINE Int32 FloatToIntNearest(float fInput) { asm fld DWORD PTR fInput
		asm fistp DWORD PTR fInput
		return reinterpret_cast<Int32 *>(&fInput)[0]; }
	BURGER_INLINE void FloatToIntFloor(Int32 *pOutput,float fInput) { asm mov ecx,pOutput
		asm fld DWORD PTR fInput
		asm fadd DWORD PTR [g_fBurgerIntMathNearesttable]
		asm { fistp DWORD PTR [ecx] } }
	BURGER_INLINE void FloatToInt(Int32 *pOutput,float fInput) { asm mov ecx,pOutput
		asm fld DWORD PTR fInput
		asm mov eax,fInput
		asm shr eax,31
		asm fadd dword ptr [g_fBurgerIntMathNearesttable+eax*4]
		asm { fistp DWORD PTR [ecx] } }
	BURGER_INLINE void FloatToIntCeil(Int32 *pOutput,float fInput) { asm mov ecx,pOutput
		asm fld DWORD PTR fInput
		asm fadd DWORD PTR [g_fBurgerIntMathNearesttable+4]
		asm { fistp DWORD PTR [ecx] } }
	BURGER_INLINE void FloatToIntNearest(Int32 *pOutput,float fInput) { asm mov ecx,pOutput
		asm fld DWORD PTR fInput
		asm { fistp DWORD PTR [ecx] } }
#elif defined(BURGER_X86) && defined(BURGER_MSVC)
	BURGER_INLINE Int32 FloatToIntFloor(float fInput) { __asm fld DWORD PTR [fInput]
		__asm fadd DWORD PTR [g_fBurgerIntMathNearesttable]
		__asm fistp DWORD PTR [fInput]
		return reinterpret_cast<Int32 *>(&fInput)[0]; }
	BURGER_INLINE Int32 FloatToInt(float fInput) { __asm fld DWORD PTR [fInput]
		__asm mov eax,[fInput]
		__asm shr eax,31
		__asm fadd dword ptr [g_fBurgerIntMathNearesttable+eax*4]
		__asm fistp DWORD PTR [fInput]
		return reinterpret_cast<Int32 *>(&fInput)[0]; }
	BURGER_INLINE Int32 FloatToIntCeil(float fInput) { __asm fld DWORD PTR [fInput]
		__asm fadd DWORD PTR [g_fBurgerIntMathNearesttable+4]
		__asm fistp DWORD PTR [fInput]
		return reinterpret_cast<Int32 *>(&fInput)[0]; }
	BURGER_INLINE Int32 FloatToIntNearest(float fInput) { __asm fld DWORD PTR [fInput]
		__asm fistp DWORD PTR [fInput]
		return reinterpret_cast<Int32 *>(&fInput)[0]; }
	BURGER_INLINE void FloatToIntFloor(Int32 *pOutput,float fInput) { __asm mov ecx,pOutput
		__asm fld DWORD PTR [fInput]
		__asm fadd DWORD PTR [g_fBurgerIntMathNearesttable]
		__asm fistp DWORD PTR [ecx] }
	BURGER_INLINE void FloatToInt(Int32 *pOutput,float fInput) { __asm mov ecx,pOutput
		__asm fld DWORD PTR [fInput]
		__asm mov eax,[fInput]
		__asm shr eax,31
		__asm fadd dword ptr [g_fBurgerIntMathNearesttable+eax*4]
		__asm fistp DWORD PTR [ecx] }
	BURGER_INLINE void FloatToIntCeil(Int32 *pOutput,float fInput) { __asm mov ecx,pOutput
		__asm fld DWORD PTR [fInput]
		__asm fadd DWORD PTR [g_fBurgerIntMathNearesttable+4]
		__asm fistp DWORD PTR [ecx] }
	BURGER_INLINE void FloatToIntNearest(Int32 *pOutput,float fInput) { __asm mov ecx,pOutput
		__asm fld DWORD PTR [fInput]
		__asm fistp DWORD PTR [ecx] }
#elif defined(BURGER_AMD64) && defined(BURGER_MSVC)
	BURGER_INLINE Int32 FloatToIntFloor(float fInput) { return _mm_cvt_ss2si(_mm_set_ss(fInput+g_fBurgerIntMathNearesttable[0])); }
	BURGER_INLINE Int32 FloatToInt(float fInput) { return static_cast<Int32>(fInput); }
	BURGER_INLINE Int32 FloatToIntCeil(float fInput) { return _mm_cvt_ss2si(_mm_set_ss(fInput+g_fBurgerIntMathNearesttable[1])); }
	BURGER_INLINE Int32 FloatToIntNearest(float fInput) { return _mm_cvt_ss2si(_mm_set_ss(fInput));}
	BURGER_INLINE void FloatToIntFloor(Int32 *pOutput,float fInput) { pOutput[0] = _mm_cvt_ss2si(_mm_set_ss(fInput+g_fBurgerIntMathNearesttable[0])); }
	BURGER_INLINE void FloatToInt(Int32 *pOutput,float fInput) { pOutput[0] = static_cast<Int32>(fInput); }
	BURGER_INLINE void FloatToIntCeil(Int32 *pOutput,float fInput) { pOutput[0] = _mm_cvt_ss2si(_mm_set_ss(fInput+g_fBurgerIntMathNearesttable[1])); }
	BURGER_INLINE void FloatToIntNearest(Int32 *pOutput,float fInput) { pOutput[0] = _mm_cvt_ss2si(_mm_set_ss(fInput)); }
#else
	extern Int32 BURGER_API FloatToIntFloor(float fInput);
	extern Int32 BURGER_API FloatToInt(float fInput);
	extern Int32 BURGER_API FloatToIntCeil(float fInput);
	extern Int32 BURGER_API FloatToIntNearest(float fInput);
	BURGER_INLINE void FloatToIntFloor(Int32 *pOutput,float fInput) { pOutput[0] = FloatToIntFloor(fInput); }
	BURGER_INLINE void FloatToInt(Int32 *pOutput,float fInput) { pOutput[0] = FloatToInt(fInput); }
	BURGER_INLINE void FloatToIntCeil(Int32 *pOutput,float fInput) { pOutput[0] = FloatToIntCeil(fInput); }
	BURGER_INLINE void FloatToIntNearest(Int32 *pOutput,float fInput) { pOutput[0] = FloatToIntNearest(fInput); }
#endif
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
	BURGER_INLINE Fixed32 FloatToFixedFloor(float fInput) { asm fld DWORD PTR fInput
		asm fmul DWORD PTR [g_fBurgerMath65536]
		asm fadd DWORD PTR [g_fBurgerIntMathNearesttable]
		asm fistp DWORD PTR fInput
		return reinterpret_cast<Fixed32 *>(&fInput)[0];}
	BURGER_INLINE Fixed32 FloatToFixed(float fInput) { asm fld DWORD PTR fInput
		asm fmul DWORD PTR [g_fBurgerMath65536]
		asm mov eax,fInput
		asm shr eax,31
		asm fadd dword ptr [g_fBurgerIntMathNearesttable+eax*4]
		asm fistp DWORD PTR fInput
		return reinterpret_cast<Fixed32 *>(&fInput)[0]; }
	BURGER_INLINE Fixed32 FloatToFixedCeil(float fInput) { asm fld DWORD PTR fInput
		asm fmul DWORD PTR [g_fBurgerMath65536]
		asm fadd DWORD PTR [g_fBurgerIntMathNearesttable+4]
		asm fistp DWORD PTR fInput
		return reinterpret_cast<Fixed32 *>(&fInput)[0]; }
	BURGER_INLINE Fixed32 FloatToFixedNearest(float fInput) { asm fld DWORD PTR fInput
		asm fmul DWORD PTR [g_fBurgerMath65536]
		asm fistp DWORD PTR fInput
		return reinterpret_cast<Fixed32 *>(&fInput)[0]; }
	BURGER_INLINE void FloatToFixedFloor(Fixed32 *pOutput,float fInput) { asm mov ecx,pOutput
		asm fld DWORD PTR fInput
		asm fmul DWORD PTR [g_fBurgerMath65536]
		asm fadd DWORD PTR [g_fBurgerIntMathNearesttable]
		asm { fistp DWORD PTR [ecx] } }
	BURGER_INLINE void FloatToFixed(Fixed32 *pOutput,float fInput) { asm mov ecx,pOutput
		asm fld DWORD PTR fInput
		asm fmul DWORD PTR [g_fBurgerMath65536]
		asm mov eax,DWORD PTR [fInput]
		asm shr eax,31
		asm fadd dword ptr [g_fBurgerIntMathNearesttable+eax*4]
		asm { fistp DWORD PTR [ecx] } }
	BURGER_INLINE void FloatToFixedCeil(Fixed32 *pOutput,float fInput) { asm mov ecx,pOutput
		asm fld DWORD PTR fInput
		asm fmul DWORD PTR [g_fBurgerMath65536]
		asm fadd DWORD PTR [g_fBurgerIntMathNearesttable+4]
		asm { fistp DWORD PTR [ecx] } }
	BURGER_INLINE void FloatToFixedNearest(Fixed32 *pOutput,float fInput) { asm mov ecx,pOutput
		asm fld DWORD PTR fInput
		asm fmul DWORD PTR [g_fBurgerMath65536]
		asm { fistp DWORD PTR [ecx] } }
#elif defined(BURGER_X86) && defined(BURGER_MSVC)
	BURGER_INLINE Int32 FloatToFixedFloor(float fInput) { __asm fld DWORD PTR [fInput]
		__asm fmul DWORD PTR [g_fBurgerMath65536]
		__asm fadd DWORD PTR [g_fBurgerIntMathNearesttable]
		__asm fistp DWORD PTR [fInput]
		return reinterpret_cast<Fixed32 *>(&fInput)[0]; }
	BURGER_INLINE Int32 FloatToFixed(float fInput) { __asm fld DWORD PTR [fInput]
		__asm fmul DWORD PTR [g_fBurgerMath65536]
		__asm mov eax,[fInput]
		__asm shr eax,31
		__asm fadd dword ptr [g_fBurgerIntMathNearesttable+eax*4]
		__asm fistp DWORD PTR [fInput]
		return reinterpret_cast<Fixed32 *>(&fInput)[0]; }
	BURGER_INLINE Int32 FloatToFixedCeil(float fInput) { __asm fld DWORD PTR [fInput]
		__asm fmul DWORD PTR [g_fBurgerMath65536]
		__asm fadd DWORD PTR [g_fBurgerIntMathNearesttable+4]
		__asm fistp DWORD PTR [fInput]
		return reinterpret_cast<Fixed32 *>(&fInput)[0]; }
	BURGER_INLINE Int32 FloatToFixedNearest(float fInput) { __asm fld DWORD PTR [fInput]
		__asm fmul DWORD PTR [g_fBurgerMath65536]
		__asm fistp DWORD PTR [fInput]
		return reinterpret_cast<Fixed32 *>(&fInput)[0]; }
	BURGER_INLINE void FloatToFixedFloor(Fixed32 *pOutput,float fInput) { __asm mov ecx,pOutput
		__asm fld DWORD PTR [fInput]
		__asm fmul DWORD PTR [g_fBurgerMath65536]
		__asm fadd DWORD PTR [g_fBurgerIntMathNearesttable]
		__asm fistp DWORD PTR [ecx] }
	BURGER_INLINE void FloatToFixed(Fixed32 *pOutput,float fInput) { __asm mov ecx,pOutput
		__asm fld DWORD PTR [fInput]
		__asm fmul DWORD PTR [g_fBurgerMath65536]
		__asm mov eax,[fInput]
		__asm shr eax,31
		__asm fadd dword ptr [g_fBurgerIntMathNearesttable+eax*4]
		__asm fistp DWORD PTR [ecx] }
	BURGER_INLINE void FloatToFixedCeil(Fixed32 *pOutput,float fInput) { __asm mov ecx,pOutput
		__asm fld DWORD PTR [fInput]
		__asm fmul DWORD PTR [g_fBurgerMath65536]
		__asm fadd DWORD PTR [g_fBurgerIntMathNearesttable+4]
		__asm fistp DWORD PTR [ecx] }
	BURGER_INLINE void FloatToFixedNearest(Fixed32 *pOutput,float fInput) { __asm mov ecx,pOutput
		__asm fld DWORD PTR [fInput]
		__asm fmul DWORD PTR [g_fBurgerMath65536]
		__asm fistp DWORD PTR [ecx] }
#else
	BURGER_INLINE Fixed32 FloatToFixedFloor(float fInput) { return static_cast<Fixed32>(FloatToIntFloor(fInput*65536.0f)); }
	BURGER_INLINE Fixed32 FloatToFixed(float fInput) { return static_cast<Fixed32>(FloatToInt(fInput*65536.0f)); }
	BURGER_INLINE Fixed32 FloatToFixedCeil(float fInput) { return static_cast<Fixed32>(FloatToIntCeil(fInput*65536.0f)); }
	BURGER_INLINE Fixed32 FloatToFixedNearest(float fInput) { return static_cast<Fixed32>(FloatToIntNearest(fInput*65536.0f)); }
	BURGER_INLINE void FloatToFixedFloor(Fixed32 *pOutput,float fInput) { FloatToIntFloor(reinterpret_cast<Int32*>(pOutput),fInput*65536.0f); }
	BURGER_INLINE void FloatToFixed(Fixed32 *pOutput,float fInput) { FloatToInt(reinterpret_cast<Int32*>(pOutput),fInput*65536.0f); }
	BURGER_INLINE void FloatToFixedCeil(Fixed32 *pOutput,float fInput) { FloatToIntCeil(reinterpret_cast<Int32*>(pOutput),fInput*65536.0f); }
	BURGER_INLINE void FloatToFixedNearest(Fixed32 *pOutput,float fInput) { FloatToIntNearest(reinterpret_cast<Int32*>(pOutput),fInput*65536.0f); }
#endif
#if defined(BURGER_ARM) || defined(BURGER_AMD64) || (defined(BURGER_X86) && !defined(BURGER_WINDOWS)) || defined(DOXYGEN)
	BURGER_INLINE Int32 Abs(Int32 iInput) { if (iInput<0) { iInput=-iInput; } return iInput; }
	BURGER_INLINE Int64 Abs(Int64 iInput) { if (iInput<0) { iInput=-iInput; } return iInput; }
	BURGER_INLINE Int32 ClampZero(Int32 iInput) { if (iInput<0) { iInput=0; } return iInput; }
	BURGER_INLINE Int64 ClampZero(Int64 iInput) { if (iInput<0) { iInput=0; } return iInput; }
#else
	BURGER_INLINE Int32 Abs(Int32 iInput) { Int32 iMask = (iInput>>31); return (iInput^iMask)-iMask; }
	BURGER_INLINE Int64 Abs(Int64 iInput) { Int64 iMask = (iInput>>63); return (iInput^iMask)-iMask; }
	BURGER_INLINE Int32 ClampZero(Int32 iInput) { return (~(iInput>>31))&iInput; }
	BURGER_INLINE Int64 ClampZero(Int64 iInput) { return (~(iInput>>63))&iInput; }
#endif
	BURGER_INLINE Int32 Min(Int32 iA,Int32 iB) { return ((iA < iB) ? iA : iB); }
	BURGER_INLINE Int64 Min(Int64 iA,Int64 iB) { return ((iA < iB) ? iA : iB); }
	BURGER_INLINE Word32 Min(Word32 uA,Word32 uB) { return ((uA < uB) ? uA : uB); }
	BURGER_INLINE Word64 Min(Word64 uA,Word64 uB) { return ((uA < uB) ? uA : uB); }
	BURGER_INLINE Int32 Max(Int32 iA,Int32 iB) { return ((iA > iB) ? iA : iB); }
	BURGER_INLINE Int64 Max(Int64 iA,Int64 iB) { return ((iA > iB) ? iA : iB); }
	BURGER_INLINE Word32 Max(Word32 uA,Word32 uB) { return ((uA > uB) ? uA : uB); }
	BURGER_INLINE Word64 Max(Word64 uA,Word64 uB) { return ((uA > uB) ? uA : uB); }
	BURGER_INLINE Int32 Clamp(Int32 iInput,Int32 iMin,Int32 iMax) { iInput = Max(iInput,iMin); return Min(iInput,iMax); }
	BURGER_INLINE Int64 Clamp(Int64 iInput,Int64 iMin,Int64 iMax) { iInput = Max(iInput,iMin); return Min(iInput,iMax); }
	BURGER_INLINE Word32 Clamp(Word32 uInput,Word32 uMin,Word32 uMax) { uInput = Max(uInput,uMin); return Min(uInput,uMax); }
	BURGER_INLINE Word64 Clamp(Word64 uInput,Word64 uMin,Word64 uMax) { uInput = Max(uInput,uMin); return Min(uInput,uMax); }

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
#elif defined(BURGER_POWERPC) || defined(BURGER_64BITCPU) && !defined(DOXYGEN)
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
#elif (defined(BURGER_METROWERKS) && defined(BURGER_POWERPC))
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
