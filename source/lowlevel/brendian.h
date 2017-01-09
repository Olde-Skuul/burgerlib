/***************************************

	Handle endian swapping

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRXCODE_H__
#include "brxcode.h"
#endif

/* BEGIN */
namespace Burger {
class SwapEndian {
public:

#if defined(BURGER_XBOX360) && !defined(DOXYGEN)
static BURGER_INLINE Word16 Load(Word16 uInput) { return _byteswap_ushort(uInput); }
static BURGER_INLINE Word32 Load(Word32 uInput) { return _byteswap_ulong(uInput); }
static BURGER_INLINE Word16 Load(const Word16 *pInput) { return __loadshortbytereverse(0,pInput); }
static BURGER_INLINE Word32 Load(const Word32 *pInput) { return __loadwordbytereverse(0,pInput); }
static BURGER_INLINE void Store(Word16 *pOutput,Word16 uInput) { __storeshortbytereverse(uInput,0,pOutput); }
static BURGER_INLINE void Store(Word32 *pOutput,Word32 uInput) { __storewordbytereverse(uInput,0,pOutput); }

#elif defined(BURGER_PS3) && !defined(DOXYGEN)
static BURGER_INLINE Word16 Load(Word16 uInput) {
	int iResult = (uInput>>8U)&0xFF;				// __rlwinm(static_cast<int>(uInput),24,24,31);
	iResult = (iResult&0xFFFF00FF)|((uInput<<8)&0xFF00);	// __rlwimi(iResult,static_cast<int>(uInput),8,16,23);
	return static_cast<Word16>(iResult);
}
static BURGER_INLINE Word32 Load(Word32 uInput) {
	int iResult = (uInput>>24U)&0xFF;		// __rlwinm(static_cast<int>(uInput),8,24,31);
	iResult = (iResult&0xFFFF00FF)|((uInput>>8)&0xFF00);	// __rlwimi(iResult,static_cast<int>(uInput),8,16,23);
	iResult = (iResult&0xFF00FFFF)|((uInput<<8)&0xFF0000);	// __rlwimi(iResult,static_cast<int>(uInput),8,8,15);
	iResult = (iResult&0x00FFFFFF)|((uInput<<24)&0xFF000000);	// __rlwimi(iResult,static_cast<int>(uInput),24,0,7);
	return static_cast<Word32>(iResult);
}
static BURGER_INLINE Word16 Load(const Word16 *pInput) { return static_cast<Word16>(__builtin_lhbrx(const_cast<Word16 *>(pInput),0)); }
static BURGER_INLINE Word32 Load(const Word32 *pInput) { return __builtin_lwbrx(const_cast<Word32 *>(pInput),0); }
static BURGER_INLINE void Store(Word16 *pOutput,Word16 uInput) { __builtin_sthbrx(uInput,pOutput,0); }
static BURGER_INLINE void Store(Word32 *pOutput,Word32 uInput) { __builtin_stwbrx(uInput,pOutput,0); }

#elif (defined(BURGER_POWERPC) && (defined(BURGER_MACOS))) && !defined(DOXYGEN)
static BURGER_INLINE Word16 Load(Word16 uInput) {
	int iResult = __rlwinm(static_cast<int>(uInput),24,24,31);
	iResult = __rlwimi(iResult,static_cast<int>(uInput),8,16,23);
	return static_cast<Word16>(iResult);
}
static BURGER_INLINE Word32 Load(Word32 uInput) {
	int iResult = __rlwinm(static_cast<int>(uInput),8,24,31);
	iResult = __rlwimi(iResult,static_cast<int>(uInput),24,16,23);
	iResult = __rlwimi(iResult,static_cast<int>(uInput),8,8,15);
	iResult = __rlwimi(iResult,static_cast<int>(uInput),24,0,7);
	return static_cast<Word32>(iResult);
}
static BURGER_INLINE Word16 Load(const Word16 *pInput) { return static_cast<Word16>(__lhbrx(const_cast<Word16 *>(pInput),0)); }
static BURGER_INLINE Word32 Load(const Word32 *pInput) { return __lwbrx(const_cast<Word32 *>(pInput),0); }
static BURGER_INLINE void Store(Word16 *pOutput,Word16 uInput) { __sthbrx(uInput,pOutput,0); }
static BURGER_INLINE void Store(Word32 *pOutput,Word32 uInput) { __stwbrx(uInput,pOutput,0); }

#elif defined(BURGER_INTELCOMPILER) && !defined(DOXYGEN)
static BURGER_INLINE Word16 Load(Word16 uInput) { return static_cast<Word16>(_bswap(uInput)>>16); }
static BURGER_INLINE Word32 Load(Word32 uInput) { return _bswap(uInput); }
static BURGER_INLINE Word16 Load(const Word16 *pInput) { return static_cast<Word16>(_bswap(pInput[0])>>16); }
static BURGER_INLINE Word32 Load(const Word32 *pInput) { return _bswap(pInput[0]);}
static BURGER_INLINE void Store(Word16 *pOutput,Word16 uInput) { pOutput[0] = static_cast<Word16>(_bswap(uInput)>>16); }
static BURGER_INLINE void Store(Word32 *pOutput,Word32 uInput) { pOutput[0] = _bswap(uInput); }

#elif (defined(BURGER_X86) && defined(BURGER_METROWERKS)) && !defined(DOXYGEN)
static BURGER_INLINE Word16 Load(Word16 uInput) { return __ror(uInput,8); }
static BURGER_INLINE Word32 Load(Word32 uInput) { return _byteswap_ulong(uInput); }
static BURGER_INLINE Word16 Load(const Word16 *pInput) { return __ror(pInput[0],8); }
static BURGER_INLINE Word32 Load(const Word32 *pInput) { return _byteswap_ulong(pInput[0]); }
static BURGER_INLINE void Store(Word16 *pOutput,Word16 uInput) { pOutput[0] = __ror(uInput,8); }
static BURGER_INLINE void Store(Word32 *pOutput,Word32 uInput) { pOutput[0] = _byteswap_ulong(uInput); }
	
#elif (defined(BURGER_WATCOM) || \
	(defined(BURGER_METROWERKS) && defined(BURGER_68K)) || \
	(defined(BURGER_INTELARCHITECTURE) && defined(BURGER_MSVC)) || \
	((defined(BURGER_INTELARCHITECTURE) || defined(BURGER_ARM) || defined(BURGER_ARM64)) && (defined(BURGER_MACOSX) || defined(BURGER_IOS)))) && \
	!defined(DOXYGEN)
static BURGER_INLINE Word16 Load(Word16 uInput) { return _byteswap_ushort(uInput); }
static BURGER_INLINE Word32 Load(Word32 uInput) { return _byteswap_ulong(uInput); }
static BURGER_INLINE Word16 Load(const Word16 *pInput) { return _byteswap_ushort(pInput[0]); }
static BURGER_INLINE Word32 Load(const Word32 *pInput) { return _byteswap_ulong(pInput[0]); }
static BURGER_INLINE void Store(Word16 *pOutput,Word16 uInput) { pOutput[0] = _byteswap_ushort(uInput); }
static BURGER_INLINE void Store(Word32 *pOutput,Word32 uInput) { pOutput[0] = _byteswap_ulong(uInput); }

#else
static Word16 BURGER_API Load(Word16 uInput);
static Word32 BURGER_API Load(Word32 uInput);
static Word16 BURGER_API Load(const Word16 *pInput);
static Word32 BURGER_API Load(const Word32 *pInput);
static void BURGER_API Store(Word16 *pOutput,Word16 uInput);
static void BURGER_API Store(Word32 *pOutput,Word32 uInput);
#endif


#if (defined(BURGER_XBOX360) || \
	(defined(BURGER_INTELARCHITECTURE) && defined(BURGER_MSVC)) || \
	((defined(BURGER_MACOS) || defined(BURGER_IOS)) && (defined(BURGER_AMD64) || defined(BURGER_ARM64)))) && \
	!defined(DOXYGEN)
static BURGER_INLINE Word64 Load(Word64 uInput) { return _byteswap_uint64(uInput); }
#else
static Word64 BURGER_API Load(Word64 uInput);
#endif


#if defined(BURGER_XBOX360) && !defined(DOXYGEN)
static BURGER_INLINE Word64 Load(const Word64 *pInput) { return __loaddoublewordbytereverse(0,pInput); }
static BURGER_INLINE void Store(Word64 *pOutput,Word64 uInput) { return __storedoublewordbytereverse(uInput,0,pOutput); }
#elif defined(BURGER_PS3) && !defined(DOXYGEN)
static BURGER_INLINE Word64 Load(const Word64 *pInput) { return __builtin_ldbrx(pInput,0); }
static BURGER_INLINE void Store(Word64 *pOutput,Word64 uInput) { return __builtin_stdbrx(uInput,pOutput,0); }
#elif ((defined(BURGER_AMD64) && defined(BURGER_MSVC)) || \
	((defined(BURGER_MACOS) || defined(BURGER_IOS)) && (defined(BURGER_AMD64) || defined(BURGER_ARM64)))) && \
	!defined(DOXYGEN)
static BURGER_INLINE Word64 Load(const Word64 *pInput) { return _byteswap_uint64(pInput[0]); }
static BURGER_INLINE void Store(Word64 *pOutput,Word64 uInput) { pOutput[0] = _byteswap_uint64(uInput); }
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
static BURGER_INLINE void Fixup(Word16 *pInput) { pInput[0] = Load(pInput); }
static BURGER_INLINE void Fixup(Word32 *pInput) { pInput[0] = Load(pInput); }
	
#if (defined(BURGER_XBOX360) || defined(BURGER_PS3) || \
	defined(BURGER_AMD64) || \
	((defined(BURGER_MACOS) || defined(BURGER_IOS)) && (defined(BURGER_AMD64) || defined(BURGER_ARM64)))) && \
	!defined(DOXYGEN)
static BURGER_INLINE void Fixup(Word64 *pInput) { pInput[0] = Load(pInput); }
#elif defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
static BURGER_INLINE void Fixup(Word64 *pInput) {
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
static BURGER_INLINE void Fixup(float *pInput) { Fixup(static_cast<Word32*>(static_cast<void *>(pInput))); }
static BURGER_INLINE void Fixup(double *pInput) { Fixup(static_cast<Word64*>(static_cast<void *>(pInput))); }
static BURGER_INLINE void FixupAny(Int16 *pInput) { FixupAny(reinterpret_cast<Word16*>(pInput)); }
static BURGER_INLINE void FixupAny(Int32 *pInput) { FixupAny(reinterpret_cast<Word32*>(pInput)); }
static BURGER_INLINE void FixupAny(Int64 *pInput) { FixupAny(reinterpret_cast<Word64*>(pInput)); }
static BURGER_INLINE void FixupAny(float *pInput) { FixupAny(static_cast<Word32*>(static_cast<void *>(pInput))); }
static BURGER_INLINE void FixupAny(double *pInput) { FixupAny(static_cast<Word64*>(static_cast<void *>(pInput))); }
};

class NativeEndian {
public:
static BURGER_INLINE Word16 Load(Word16 uInput) { return uInput; }
static BURGER_INLINE Word32 Load(Word32 uInput) { return uInput; }
static BURGER_INLINE Word64 Load(Word64 uInput) { return uInput; }
static BURGER_INLINE float Load(float fInput) { return fInput; }
static BURGER_INLINE double Load(double dInput) { return dInput; }
static BURGER_INLINE Word16 Load(const Word16 *pInput) { return pInput[0]; }
static BURGER_INLINE Word32 Load(const Word32 *pInput) { return pInput[0]; }
static BURGER_INLINE Word64 Load(const Word64 *pInput) { return pInput[0]; }
static BURGER_INLINE float Load(const float *pInput) { return pInput[0]; }
static BURGER_INLINE double Load(const double *pInput) { return pInput[0]; }
static BURGER_INLINE void Store(Word16 *pOutput,Word16 uInput) { pOutput[0] = uInput; }
static BURGER_INLINE void Store(Word32 *pOutput,Word32 uInput) { pOutput[0] = uInput; }
static BURGER_INLINE void Store(Word64 *pOutput,Word64 uInput) { pOutput[0] = uInput; }
static BURGER_INLINE void Store(float *pOutput,float fInput) { pOutput[0] = fInput; }
static BURGER_INLINE void Store(double *pOutput,double dInput) { pOutput[0] = dInput; }
#if defined(BURGER_WATCOM)
static BURGER_INLINE Word16 LoadAny(const Word16 *pInput) { return BurgerNativeEndianLoadAny16(pInput); }
static BURGER_INLINE Word32 LoadAny(const Word32 *pInput) { return BurgerNativeEndianLoadAny32(pInput); }
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
static BURGER_INLINE void Fixup(Word16 *) {}
static BURGER_INLINE void Fixup(Word32 *) {}
static BURGER_INLINE void Fixup(Word64 *) {}
static BURGER_INLINE void FixupAny(Word16 *) {}
static BURGER_INLINE void FixupAny(Word32 *) {}
static BURGER_INLINE void FixupAny(Word64 *) {}

static BURGER_INLINE Int16 Load(Int16 iInput) { return iInput; }
static BURGER_INLINE Int32 Load(Int32 iInput) { return iInput; }
static BURGER_INLINE Int64 Load(Int64 iInput) { return iInput; }
	
static BURGER_INLINE Int16 Load(const Int16 *pInput) { return pInput[0]; }
static BURGER_INLINE Int32 Load(const Int32 *pInput) { return pInput[0]; }
static BURGER_INLINE Int64 Load(const Int64 *pInput) { return pInput[0]; }
static BURGER_INLINE void Store(Int16 *pOutput,Int16 iInput) { pOutput[0] = iInput; }
static BURGER_INLINE void Store(Int32 *pOutput,Int32 iInput) { pOutput[0] = iInput; }
static BURGER_INLINE void Store(Int64 *pOutput,Int64 iInput) { pOutput[0] = iInput; }
static BURGER_INLINE Int16 LoadAny(const Int16 *pInput) { return static_cast<Int16>(LoadAny(reinterpret_cast<const Word16*>(pInput))); }
static BURGER_INLINE Int32 LoadAny(const Int32 *pInput) { return static_cast<Int32>(LoadAny(reinterpret_cast<const Word32*>(pInput))); }
static BURGER_INLINE Int64 LoadAny(const Int64 *pInput) { return static_cast<Int64>(LoadAny(reinterpret_cast<const Word64*>(pInput))); }
static BURGER_INLINE void StoreAny(Int16 *pOutput,Int16 iInput) { StoreAny(reinterpret_cast<Word16*>(pOutput),static_cast<Word16>(iInput)); }
static BURGER_INLINE void StoreAny(Int32 *pOutput,Int32 iInput) { StoreAny(reinterpret_cast<Word32*>(pOutput),static_cast<Word32>(iInput)); }
static BURGER_INLINE void StoreAny(Int64 *pOutput,Int64 iInput) { StoreAny(reinterpret_cast<Word64*>(pOutput),static_cast<Word64>(iInput)); }
static BURGER_INLINE void Fixup(Int16 *) {}
static BURGER_INLINE void Fixup(Int32 *) {}
static BURGER_INLINE void Fixup(Int64 *) {}
static BURGER_INLINE void Fixup(float *) {}
static BURGER_INLINE void Fixup(double *) {}
static BURGER_INLINE void FixupAny(Int16 *) {}
static BURGER_INLINE void FixupAny(Int32 *) {}
static BURGER_INLINE void FixupAny(Int64 *) {}
static BURGER_INLINE void FixupAny(float *) {}
static BURGER_INLINE void FixupAny(double *) {}
};

#if defined(BURGER_LITTLEENDIAN)
class LittleEndian : public NativeEndian {};
class BigEndian : public SwapEndian {};
#else
class LittleEndian : public SwapEndian {};
class BigEndian : public NativeEndian {};
#endif
extern void BURGER_API ConvertEndian(Word16 *pInput,WordPtr uCount);
extern void BURGER_API ConvertEndian(Word16 *pOutput,const Word16 *pInput,WordPtr uCount);
extern void BURGER_API ConvertEndian(Word32 *pInput,WordPtr uCount);
extern void BURGER_API ConvertEndian(Word32 *pOutput,const Word32 *pInput,WordPtr uCount);
extern void BURGER_API ConvertEndian(Word64 *pInput,WordPtr uCount);
extern void BURGER_API ConvertEndian(Word64 *pOutput,const Word64 *pInput,WordPtr uCount);
extern void BURGER_API SwapCharsToBytes(void *pInput,WordPtr uLength);
extern void BURGER_API SwapCharsToBytes(void *pOutput,const void *pInput,WordPtr uLength);

}

/* END */

#endif
