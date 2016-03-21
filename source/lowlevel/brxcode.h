/***************************************

	Intrinsics and subroutines exclusive to the Xcode compilers

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRXCODE_H__
#define __BRXCODE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if (defined(BURGER_MACOSX) || defined(BURGER_IOS)) && defined(BURGER_GNUC)
extern "C" float sqrtf(float);
extern "C" double sqrt(double);
extern "C" float fabsf(float);
extern "C" double fabs(double);

#if defined(BURGER_POWERPC)
BURGER_INLINE float __fsel(float fA,float fB,float fC) { float fD; __asm__("fsel %0,%1,%2,%3":"=f" (fD) : "f" (fA),"f"(fB),"f"(fC)); return fD; }
BURGER_INLINE double __fsel(double dA,double dB,double dC) { double dD; __asm__("fsel %0,%1,%2,%3":"=f" (dD) : "f" (dA),"f"(dB),"f"(dC)); return dD; }
#define __rlwimi(rA,rS,cnt,mb,me) ({ __asm__("rlwimi %0,%2,%3,%4,%5":"=r" (rA) : "0"(rA), "r"(rS),"n"(cnt),"n"(mb),"n"(me)); rA; })
#define __rlwinm(rS,cnt,mb,me) ({ int rA; __asm__("rlwinm %0,%1,%2,%3,%4":"=r" (rA) : "r"(rS),"n"(cnt),"n"(mb),"n"(me)); rA; } )
BURGER_INLINE int __lhbrx(void *pInput,int offset) { Word16 uResult; __asm__ volatile ("lhbrx %0,%1,%2":"=r"(uResult) : "b%"(offset),"r"(pInput) : "memory"); return uResult; }
BURGER_INLINE int __lwbrx(void *pInput,int offset) { Word32 uResult; __asm__ volatile ("lwbrx %0,%1,%2":"=r"(uResult) : "b%"(offset),"r"(pInput) : "memory"); return uResult; }
BURGER_INLINE void __sthbrx(Word16 uValue,void *pInput,int offset) { __asm__("sthbrx %0,%1,%2": : "r"(uValue),"b%"(offset),"r"(pInput) : "memory"); }
BURGER_INLINE void __stwbrx(Word32 uValue,void *pInput,int offset) { __asm__("stwbrx %0,%1,%2": : "r"(uValue),"b%"(offset),"r"(pInput) : "memory"); }
#endif

#if defined(BURGER_INTELARCHITECTURE)
BURGER_INLINE Word16 _byteswap_ushort(Word16 uInput) { __asm__("rorw $8,%0":"+r"(uInput)); return uInput; }
BURGER_INLINE Word32 _byteswap_ulong(Word32 uInput) { __asm__("bswap %0":"+r"(uInput)); return uInput; }
#if defined(BURGER_AMD64)
BURGER_INLINE Word64 _byteswap_uint64(Word64 uInput) { __asm__("bswap %0":"+r"(uInput)); return uInput; }
#endif
#endif

#if defined(BURGER_ARM)
BURGER_INLINE Word16 _byteswap_ushort(Word16 uInput) { Word16 uResult; __asm__("rev16 %0,%1":"=r"(uResult):"r"(uInput)); return uResult; }
BURGER_INLINE Word32 _byteswap_ulong(Word32 uInput) { Word32 uResult; __asm__("rev %0,%1":"=r"(uResult):"r"(uInput)); return uResult; }
#endif

#if defined(BURGER_ARM64)
BURGER_INLINE Word16 _byteswap_ushort(Word16 uInput) { Word16 uResult; __asm__("rev16 %w0,%w1":"=r"(uResult):"r"(uInput)); return uResult; }
BURGER_INLINE Word32 _byteswap_ulong(Word32 uInput) { Word32 uResult; __asm__("rev %w0,%w1":"=r"(uResult):"r"(uInput)); return uResult; }
BURGER_INLINE Word64 _byteswap_uint64(Word64 uInput) { Word64 uResult; __asm__("rev %x0,%x1":"=r"(uResult):"r"(uInput)); return uResult; }
#endif

#endif
/* END */

#endif
