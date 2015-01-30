/***************************************

	Handle endian swapping

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRENDIAN_H__
#define __BRENDIAN_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRWATCOM_H__
#include "brwatcom.h"
#endif

#ifndef __BRMETROWERKS_H__
#include "brmetrowerks.h"
#endif

#ifndef __BRVISUALSTUDIO_H__
#include "brvisualstudio.h"
#endif

/* BEGIN */
namespace Burger {
class SwapEndian {
public:
#if defined(BURGER_WATCOM) || (defined(BURGER_METROWERKS) && defined(BURGER_68K))
static Word16 BURGER_INLINE Load(Word16 uInput) { return _bswap16(uInput); }
static Word32 BURGER_INLINE Load(Word32 uInput) { return _bswap(uInput); }
static Word16 BURGER_INLINE Load(const Word16 *pInput) { return _bswap16(pInput[0]); }
static Word32 BURGER_INLINE Load(const Word32 *pInput) { return _bswap(pInput[0]); }
static void BURGER_INLINE Store(Word16 *pOutput,Word16 uInput) { pOutput[0] = _bswap16(uInput); }
static void BURGER_INLINE Store(Word32 *pOutput,Word32 uInput) { pOutput[0] = _bswap(uInput); }
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
static Word16 BURGER_INLINE Load(Word16 uInput) { return _byteswap_ushort(uInput); }
static Word32 BURGER_INLINE Load(Word32 uInput) { return _byteswap_ulong(uInput); }
static Word16 BURGER_INLINE Load(const Word16 *pInput) { return __loadshortbytereverse(0,pInput); }
static Word32 BURGER_INLINE Load(const Word32 *pInput) { return __loadwordbytereverse(0,pInput); }
static void BURGER_INLINE Store(Word16 *pOutput,Word16 uInput) { __storeshortbytereverse(uInput,0,pOutput); }
static void BURGER_INLINE Store(Word32 *pOutput,Word32 uInput) { __storewordbytereverse(uInput,0,pOutput); }
#elif defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
static Word16 BURGER_INLINE Load(Word16 uInput) { int iResult = __rlwinm(static_cast<int>(uInput),24,24,31);
	iResult = __rlwimi(iResult,static_cast<int>(uInput),8,16,23);
	return static_cast<Word16>(iResult); }
static Word32 BURGER_INLINE Load(Word32 uInput) { int iResult = __rlwinm(static_cast<int>(uInput),8,24,31);
	iResult = __rlwimi(iResult,static_cast<int>(uInput),24,16,23);
	iResult = __rlwimi(iResult,static_cast<int>(uInput),8,8,15);
	iResult = __rlwimi(iResult,static_cast<int>(uInput),24,0,7);
	return static_cast<Word32>(iResult); }
static Word16 BURGER_INLINE Load(const Word16 *pInput) { return static_cast<Word16>(__lhbrx(const_cast<Word16 *>(pInput),0)); }
static Word32 BURGER_INLINE Load(const Word32 *pInput) { return __lwbrx(const_cast<Word32 *>(pInput),0); }
#elif defined(BURGER_X86) && defined(BURGER_METROWERKS)
static Word16 BURGER_INLINE Load(Word16 uInput) {
	return __ror(uInput,8);
}
static Word32 BURGER_INLINE Load(Word32 uInput) {
	asm bswap uInput
	return uInput; 
}
static Word16 BURGER_INLINE Load(const Word16 *pInput) {
	return __ror(pInput[0],8);
}
static Word32 BURGER_INLINE Load(const Word32 *pInput) {
	register int uTemp;
	uTemp = static_cast<int>(pInput[0]);
	asm bswap uTemp
	return static_cast<Word32>(uTemp);
}
static void BURGER_INLINE Store(Word16 *pOutput,Word16 uInput) {
	pOutput[0] = __ror(uInput,8);
}
static void BURGER_INLINE Store(Word32 *pOutput,Word32 uInput) {
	asm bswap uInput
	pOutput[0] = uInput;
}
#elif defined(BURGER_INTELCOMPILER)
static Word16 BURGER_INLINE Load(Word16 uInput) { return static_cast<Word16>(_bswap(uInput)>>16); }
static Word32 BURGER_INLINE Load(Word32 uInput) { return _bswap(uInput); }
static Word16 BURGER_INLINE Load(const Word16 *pInput) { return static_cast<Word16>(_bswap(pInput[0])>>16); }
static Word32 BURGER_INLINE Load(const Word32 *pInput) { return _bswap(pInput[0]);}
static void BURGER_INLINE Store(Word16 *pOutput,Word16 uInput) { pOutput[0] = static_cast<Word16>(_bswap(uInput)>>16); }
static void BURGER_INLINE Store(Word32 *pOutput,Word32 uInput) { pOutput[0] = _bswap(uInput); }
#elif (defined(BURGER_X86) || defined(BURGER_AMD64)) && defined(BURGER_MSVC)
static Word16 BURGER_INLINE Load(Word16 uInput) { return _byteswap_ushort(uInput); }
static Word32 BURGER_INLINE Load(Word32 uInput) { return _byteswap_ulong(uInput); }
static Word16 BURGER_INLINE Load(const Word16 *pInput) { return _byteswap_ushort(pInput[0]); }
static Word32 BURGER_INLINE Load(const Word32 *pInput) { return _byteswap_ulong(pInput[0]); }
static void BURGER_INLINE Store(Word16 *pOutput,Word16 uInput) { pOutput[0] = _byteswap_ushort(uInput); }
static void BURGER_INLINE Store(Word32 *pOutput,Word32 uInput) { pOutput[0] = _byteswap_ulong(uInput); }
#else
static Word16 BURGER_API Load(Word16 uInput);
static Word32 BURGER_API Load(Word32 uInput);
static Word16 BURGER_API Load(const Word16 *pInput);
static Word32 BURGER_API Load(const Word32 *pInput);
static void BURGER_API Store(Word16 *pOutput,Word16 uInput);
static void BURGER_API Store(Word32 *pOutput,Word32 uInput);
#endif
#if (defined(BURGER_XBOX360) || ((defined(BURGER_X86) || defined(BURGER_AMD64)) && defined(BURGER_MSVC))) && !defined(DOXYGEN)
static Word64 BURGER_INLINE Load(Word64 uInput) { return _byteswap_uint64(uInput); }
#else
static Word64 BURGER_API Load(Word64 uInput);
#endif
#if defined(BURGER_XBOX360) && !defined(DOXYGEN)
static Word64 BURGER_INLINE Load(const Word64 *pInput) { return __loaddoublewordbytereverse(0,pInput); }
static void BURGER_INLINE Store(Word64 *pOutput,Word64 uInput) { return __storedoublewordbytereverse(uInput,0,pOutput); }
#elif defined(BURGER_AMD64) && defined(BURGER_MSVC)
static Word64 BURGER_INLINE Load(const Word64 *pInput) { return _byteswap_uint64(pInput[0]); }
static void BURGER_INLINE Store(Word64 *pOutput,Word64 uInput) { pOutput[0] = _byteswap_uint64(uInput); }
#else
static Word64 BURGER_API Load(const Word64 *pInput);
static void BURGER_API Store(Word64 *pOutput,Word64 uInput);
#endif
static float BURGER_API Load(float fInput);
static double BURGER_API Load(double dInput);
static float BURGER_API Load(const float *pInput);
static double BURGER_API Load(const double *pInput);
static void BURGER_API Store(float *pOutput,float fInput);
static void BURGER_API Store(double *pOutput,double dInput);
static Word16 BURGER_API LoadAny(const Word16 *pInput);
static Word32 BURGER_API LoadAny(const Word32 *pInput);
static Word64 BURGER_API LoadAny(const Word64 *pInput);
static float BURGER_API LoadAny(const float *pInput);
static double BURGER_API LoadAny(const double *pInput);
static void BURGER_API StoreAny(Word16 *pOutput,Word16 uInput);
static void BURGER_API StoreAny(Word32 *pOutput,Word32 uInput);
static void BURGER_API StoreAny(Word64 *pOutput,Word64 uInput);
static void BURGER_API StoreAny(float *pOutput,float fInput);
static void BURGER_API StoreAny(double *pOutput,double dInput);
static void BURGER_INLINE Fixup(Word16 *pInput) { pInput[0] = Load(pInput); }
static void BURGER_INLINE Fixup(Word32 *pInput) { pInput[0] = Load(pInput); }
#if (defined(BURGER_XBOX360) || defined(BURGER_AMD64)) && !defined(DOXYGEN)
static void BURGER_INLINE Fixup(Word64 *pInput) { pInput[0] = Load(pInput); }
#elif defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
static void BURGER_INLINE Fixup(Word64 *pInput) {
	Word32 uLow = __lwbrx(pInput,0);
	Word32 uHigh = __lwbrx(pInput,4);
	reinterpret_cast<Word32 *>(pInput)[0] = uHigh;
	reinterpret_cast<Word32 *>(pInput)[1] = uLow;
}
#else
static void BURGER_API Fixup(Word64 *pInput);
#endif
static void BURGER_API FixupAny(Word16 *pInput);
static void BURGER_API FixupAny(Word32 *pInput);
static void BURGER_API FixupAny(Word64 *pInput);

static BURGER_INLINE Int16 Load(Int16 iInput) { return static_cast<Int16>(Load(static_cast<Word16>(iInput))); }
static BURGER_INLINE Int32 Load(Int32 iInput) { return static_cast<Int32>(Load(static_cast<Word32>(iInput))); }
static BURGER_INLINE Int64 Load(Int64 iInput) { return static_cast<Int64>(Load(static_cast<Word64>(iInput))); }
#if (!defined(BURGER_PS2) && !defined(BURGER_64BITCPU)) || defined(DOXYGEN)
static BURGER_INLINE Word Load(Word uInput) { return static_cast<Word>(Load(static_cast<Word32>(uInput))); }
static BURGER_INLINE Int Load(Int iInput) { return static_cast<Int>(Load(static_cast<Word32>(iInput))); }
static BURGER_INLINE Word Load(const Word *pInput) { return static_cast<Word>(Load(reinterpret_cast<const Word32*>(pInput))); }
static BURGER_INLINE Int Load(const Int *pInput) { return static_cast<Int>(Load(reinterpret_cast<const Word32*>(pInput))); }
static BURGER_INLINE void Store(Word *pOutput,Word uInput) { Store(reinterpret_cast<Word32*>(pOutput),static_cast<Word32>(uInput)); }
static BURGER_INLINE void Store(Int *pOutput,Int iInput) { Store(reinterpret_cast<Word32*>(pOutput),static_cast<Word32>(iInput)); }
static BURGER_INLINE Word LoadAny(const Word *pInput) { return static_cast<Word>(LoadAny(reinterpret_cast<const Word32*>(pInput))); }
static BURGER_INLINE Int LoadAny(const Int *pInput) { return static_cast<Int>(LoadAny(reinterpret_cast<const Word32*>(pInput))); }
static BURGER_INLINE void StoreAny(Word *pOutput,Word uInput) { StoreAny(reinterpret_cast<Word32*>(pOutput),static_cast<Word32>(uInput)); }
static BURGER_INLINE void StoreAny(Int *pOutput,Int iInput) { StoreAny(reinterpret_cast<Word32*>(pOutput),static_cast<Word32>(iInput)); }
static BURGER_INLINE void Fixup(Word *pInput) { Fixup(reinterpret_cast<Word32*>(pInput)); }
static BURGER_INLINE void Fixup(Int *pInput) { Fixup(reinterpret_cast<Word32*>(pInput)); }
static BURGER_INLINE void FixupAny(Word *pInput) { FixupAny(reinterpret_cast<Word32*>(pInput)); }
static BURGER_INLINE void FixupAny(Int *pInput) { FixupAny(reinterpret_cast<Word32*>(pInput)); }
#endif
static BURGER_INLINE Int16 Load(const Int16 *pInput) { return static_cast<Int16>(Load(reinterpret_cast<const Word16*>(pInput))); }
static BURGER_INLINE Int32 Load(const Int32 *pInput) { return static_cast<Int32>(Load(reinterpret_cast<const Word32*>(pInput))); }
static BURGER_INLINE Int64 Load(const Int64 *pInput) { return static_cast<Int64>(Load(reinterpret_cast<const Word64*>(pInput))); }
static BURGER_INLINE void Store(Int16 *pOutput,Int16 iInput) { Store(reinterpret_cast<Word16*>(pOutput),static_cast<Word16>(iInput)); }
static BURGER_INLINE void Store(Int32 *pOutput,Int32 iInput) { Store(reinterpret_cast<Word32*>(pOutput),static_cast<Word32>(iInput)); }
static BURGER_INLINE void Store(Int64 *pOutput,Int64 iInput) { Store(reinterpret_cast<Word64*>(pOutput),static_cast<Word64>(iInput)); }
static BURGER_INLINE Int16 LoadAny(const Int16 *pInput) { return static_cast<Int16>(LoadAny(reinterpret_cast<const Word16*>(pInput))); }
static BURGER_INLINE Int32 LoadAny(const Int32 *pInput) { return static_cast<Int32>(LoadAny(reinterpret_cast<const Word32*>(pInput))); }
static BURGER_INLINE Int64 LoadAny(const Int64 *pInput) { return static_cast<Int64>(LoadAny(reinterpret_cast<const Word64*>(pInput))); }
static BURGER_INLINE void StoreAny(Int16 *pOutput,Int16 iInput) { StoreAny(reinterpret_cast<Word16*>(pOutput),static_cast<Word16>(iInput)); }
static BURGER_INLINE void StoreAny(Int32 *pOutput,Int32 iInput) { StoreAny(reinterpret_cast<Word32*>(pOutput),static_cast<Word32>(iInput)); }
static BURGER_INLINE void StoreAny(Int64 *pOutput,Int64 iInput) { StoreAny(reinterpret_cast<Word64*>(pOutput),static_cast<Word64>(iInput)); }
static BURGER_INLINE void Fixup(Int16 *pInput) { Fixup(reinterpret_cast<Word16*>(pInput)); }
static BURGER_INLINE void Fixup(Int32 *pInput) { Fixup(reinterpret_cast<Word32*>(pInput)); }
static BURGER_INLINE void Fixup(Int64 *pInput) { Fixup(reinterpret_cast<Word64*>(pInput)); }
static BURGER_INLINE void Fixup(float *pInput) { Fixup(reinterpret_cast<Word32*>(pInput)); }
static BURGER_INLINE void Fixup(double *pInput) { Fixup(reinterpret_cast<Word64*>(pInput)); }
static BURGER_INLINE void FixupAny(Int16 *pInput) { FixupAny(reinterpret_cast<Word16*>(pInput)); }
static BURGER_INLINE void FixupAny(Int32 *pInput) { FixupAny(reinterpret_cast<Word32*>(pInput)); }
static BURGER_INLINE void FixupAny(Int64 *pInput) { FixupAny(reinterpret_cast<Word64*>(pInput)); }
static BURGER_INLINE void FixupAny(float *pInput) { FixupAny(reinterpret_cast<Word32*>(pInput)); }
static BURGER_INLINE void FixupAny(double *pInput) { FixupAny(reinterpret_cast<Word64*>(pInput)); }
};

class NativeEndian {
public:
static Word16 BURGER_INLINE Load(Word16 uInput) { return uInput; }
static Word32 BURGER_INLINE Load(Word32 uInput) { return uInput; }
static Word64 BURGER_INLINE Load(Word64 uInput) { return uInput; }
static float BURGER_INLINE Load(float fInput) { return fInput; }
static double BURGER_INLINE Load(double dInput) { return dInput; }
static Word16 BURGER_INLINE Load(const Word16 *pInput) { return pInput[0]; }
static Word32 BURGER_INLINE Load(const Word32 *pInput) { return pInput[0]; }
static Word64 BURGER_INLINE Load(const Word64 *pInput) { return pInput[0]; }
static float BURGER_INLINE Load(const float *pInput) { return pInput[0]; }
static double BURGER_INLINE Load(const double *pInput) { return pInput[0]; }
static void BURGER_INLINE Store(Word16 *pOutput,Word16 uInput) { pOutput[0] = uInput; }
static void BURGER_INLINE Store(Word32 *pOutput,Word32 uInput) { pOutput[0] = uInput; }
static void BURGER_INLINE Store(Word64 *pOutput,Word64 uInput) { pOutput[0] = uInput; }
static void BURGER_INLINE Store(float *pOutput,float fInput) { pOutput[0] = fInput; }
static void BURGER_INLINE Store(double *pOutput,double dInput) { pOutput[0] = dInput; }
#if defined(BURGER_WATCOM)
static Word16 BURGER_INLINE LoadAny(const Word16 *pInput) { return BurgerNativeEndianLoadAny16(pInput); }
static Word32 BURGER_INLINE LoadAny(const Word32 *pInput) { return BurgerNativeEndianLoadAny32(pInput); }
#else
static Word16 BURGER_API LoadAny(const Word16 *pInput);
static Word32 BURGER_API LoadAny(const Word32 *pInput);
#endif
static Word64 BURGER_API LoadAny(const Word64 *pInput);
static float BURGER_API LoadAny(const float *pInput);
static double BURGER_API LoadAny(const double *pInput);
static void BURGER_API StoreAny(Word16 *pOutput,Word16 uInput);
static void BURGER_API StoreAny(Word32 *pOutput,Word32 uInput);
static void BURGER_API StoreAny(Word64 *pOutput,Word64 uInput);
static void BURGER_API StoreAny(float *pOutput,float fInput);
static void BURGER_API StoreAny(double *pOutput,double dInput);
static void BURGER_INLINE Fixup(Word16 *) {}
static void BURGER_INLINE Fixup(Word32 *) {}
static void BURGER_INLINE Fixup(Word64 *) {}
static void BURGER_INLINE FixupAny(Word16 *) {}
static void BURGER_INLINE FixupAny(Word32 *) {}
static void BURGER_INLINE FixupAny(Word64 *) {}

static Int16 BURGER_INLINE Load(Int16 iInput) { return iInput; }
static Int32 BURGER_INLINE Load(Int32 iInput) { return iInput; }
static Int64 BURGER_INLINE Load(Int64 iInput) { return iInput; }
#if (!defined(BURGER_PS2) && !defined(BURGER_64BITCPU)) || defined(DOXYGEN)
static Word BURGER_INLINE Load(Word uInput) { return uInput; }
static Int BURGER_INLINE Load(Int iInput) { return iInput; }
static Word BURGER_INLINE Load(const Word *pInput) { return pInput[0]; }
static Int BURGER_INLINE Load(const Int *pInput) { return pInput[0]; }
static void BURGER_INLINE Store(Word *pOutput,Word uInput) { pOutput[0] = uInput; }
static void BURGER_INLINE Store(Int *pOutput,Int iInput) { pOutput[0] = iInput; }
static Word BURGER_INLINE LoadAny(const Word *pInput) { return static_cast<Word>(LoadAny(reinterpret_cast<const Word32*>(pInput))); }
static Int BURGER_INLINE LoadAny(const Int *pInput) { return static_cast<Int>(LoadAny(reinterpret_cast<const Word32*>(pInput))); }
static void BURGER_INLINE StoreAny(Word *pOutput,Word uInput) { StoreAny(reinterpret_cast<Word32*>(pOutput),static_cast<Word32>(uInput)); }
static void BURGER_INLINE StoreAny(Int *pOutput,Int iInput) { StoreAny(reinterpret_cast<Word32*>(pOutput),static_cast<Word32>(iInput)); }
static void BURGER_INLINE Fixup(Word *) {}
static void BURGER_INLINE Fixup(Int *) {}
static void BURGER_INLINE FixupAny(Word *) {}
static void BURGER_INLINE FixupAny(Int *) {}
#endif
static Int16 BURGER_INLINE Load(const Int16 *pInput) { return pInput[0]; }
static Int32 BURGER_INLINE Load(const Int32 *pInput) { return pInput[0]; }
static Int64 BURGER_INLINE Load(const Int64 *pInput) { return pInput[0]; }
static void BURGER_INLINE Store(Int16 *pOutput,Int16 iInput) { pOutput[0] = iInput; }
static void BURGER_INLINE Store(Int32 *pOutput,Int32 iInput) { pOutput[0] = iInput; }
static void BURGER_INLINE Store(Int64 *pOutput,Int64 iInput) { pOutput[0] = iInput; }
static Int16 BURGER_INLINE LoadAny(const Int16 *pInput) { return static_cast<Int16>(LoadAny(reinterpret_cast<const Word16*>(pInput))); }
static Int32 BURGER_INLINE LoadAny(const Int32 *pInput) { return static_cast<Int32>(LoadAny(reinterpret_cast<const Word32*>(pInput))); }
static Int64 BURGER_INLINE LoadAny(const Int64 *pInput) { return static_cast<Int64>(LoadAny(reinterpret_cast<const Word64*>(pInput))); }
static void BURGER_INLINE StoreAny(Int16 *pOutput,Int16 iInput) { StoreAny(reinterpret_cast<Word16*>(pOutput),static_cast<Word16>(iInput)); }
static void BURGER_INLINE StoreAny(Int32 *pOutput,Int32 iInput) { StoreAny(reinterpret_cast<Word32*>(pOutput),static_cast<Word32>(iInput)); }
static void BURGER_INLINE StoreAny(Int64 *pOutput,Int64 iInput) { StoreAny(reinterpret_cast<Word64*>(pOutput),static_cast<Word64>(iInput)); }
static void BURGER_INLINE Fixup(Int16 *) {}
static void BURGER_INLINE Fixup(Int32 *) {}
static void BURGER_INLINE Fixup(Int64 *) {}
static void BURGER_INLINE Fixup(float *) {}
static void BURGER_INLINE Fixup(double *) {}
static void BURGER_INLINE FixupAny(Int16 *) {}
static void BURGER_INLINE FixupAny(Int32 *) {}
static void BURGER_INLINE FixupAny(Int64 *) {}
static void BURGER_INLINE FixupAny(float *) {}
static void BURGER_INLINE FixupAny(double *) {}
};

#if defined(BURGER_LITTLEENDIAN)
class LittleEndian : public NativeEndian {};
class BigEndian : public SwapEndian {};
#else
class LittleEndian : public SwapEndian {};
class BigEndian : public NativeEndian {};
#endif
}

/* END */

#endif
