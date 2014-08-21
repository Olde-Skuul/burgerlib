/***************************************

	Intrinsics and subroutines exclusive to the Microsoft Visual Studio compilers

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRVISUALSTUDIO_H__
#define __BRVISUALSTUDIO_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_MSVC) && !defined(DOXYGEN)
extern "C" {
extern double __cdecl fabs(double);
extern double __cdecl sqrt(double);
#pragma intrinsic(fabs,sqrt)

extern Word16 __cdecl _byteswap_ushort(Word16);
extern unsigned long __cdecl _byteswap_ulong(unsigned long);
extern Word64 __cdecl _byteswap_uint64(Word64);
#pragma intrinsic(_byteswap_ushort,_byteswap_ulong,_byteswap_uint64)

extern unsigned int __cdecl _rotl(unsigned int,int);
extern Word64 __cdecl _rotl64(Word64,int);
extern unsigned int __cdecl _rotr(unsigned int,int);
extern Word64 __cdecl _rotr64(Word64,int);
#pragma intrinsic(_rotl,_rotl64,_rotr,_rotr64)

long _InterlockedExchange(long volatile*,long);
long __cdecl _InterlockedIncrement(long volatile*);
long __cdecl _InterlockedDecrement(long volatile*);
long _InterlockedExchangeAdd(long volatile*,long);
long _InterlockedCompareExchange(long volatile*,long,long);
#pragma intrinsic(_InterlockedExchange,_InterlockedIncrement,_InterlockedDecrement,_InterlockedExchangeAdd,_InterlockedCompareExchange)

#if defined(BURGER_64BITCPU)
__int64 _InterlockedExchange64(__int64 volatile*,__int64);
__int64 _InterlockedIncrement64(__int64 volatile*);
__int64 _InterlockedDecrement64(__int64 volatile*);
__int64 _InterlockedExchangeAdd64(__int64 volatile*,__int64);
__int64 _InterlockedCompareExchange64(__int64 volatile*,__int64,__int64);
#pragma intrinsic(_InterlockedExchange64,_InterlockedIncrement64,_InterlockedDecrement64,_InterlockedExchangeAdd64,_InterlockedCompareExchange64)
#endif
}
#endif
/* END */

#endif
