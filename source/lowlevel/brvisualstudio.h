/***************************************

	Intrinsics and subroutines exclusive to the Microsoft Visual Studio compilers

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

//
// Note: _MSC_VER values for determining specific version.
//
// _MSC_VER == 1900 (Visual Studio 2015)
// _MSC_VER == 1800 (Visual Studio 2013)
// _MSC_VER == 1700 (Visual Studio 2012)
// _MSC_VER == 1600 (Visual Studio 2010)
// _MSC_VER == 1500 (Visual Studio 2008)
// _MSC_VER == 1400 (Visual Studio 2005)
// _MSC_VER == 1310 (Visual Studio 2003)
// _MSC_VER == 1300 (Visual Studio 2002)
// _MSC_VER == 1200 (Visual Studio 6)
// _MSC_VER == 1100 (Visual studio 5)
//

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

#if defined(BURGER_INTELARCHITECTURE)
extern void __cpuid(int [4],int);
#pragma intrinsic(__cpuid)
#if _MSC_VER>=1600
extern void __cpuidex(int [4],int,int);
#pragma intrinsic(__cpuidex)
#endif
#endif

long __cdecl _InterlockedExchange(long volatile*,long);
long __cdecl _InterlockedIncrement(long volatile*);
long __cdecl _InterlockedDecrement(long volatile*);
long __cdecl _InterlockedExchangeAdd(long volatile*,long);
long __cdecl _InterlockedCompareExchange(long volatile*,long,long);
#pragma intrinsic(_InterlockedExchange,_InterlockedIncrement,_InterlockedDecrement,_InterlockedExchangeAdd,_InterlockedCompareExchange)

#if defined(BURGER_64BITCPU)
__int64 _InterlockedExchange64(__int64 volatile*,__int64);
__int64 _InterlockedIncrement64(__int64 volatile*);
__int64 _InterlockedDecrement64(__int64 volatile*);
__int64 _InterlockedExchangeAdd64(__int64 volatile*,__int64);
__int64 _InterlockedCompareExchange64(__int64 volatile*,__int64,__int64);
#pragma intrinsic(_InterlockedExchange64,_InterlockedIncrement64,_InterlockedDecrement64,_InterlockedExchangeAdd64,_InterlockedCompareExchange64)
#if _MSC_VER<1500		// Visual studio 2005 and earlier don't have these SSE type conversions
BURGER_INLINE float _mm_cvtss_f32(__m128 vInput) { return vInput.m128_f32[0]; }
BURGER_INLINE double _mm_cvtsd_f64(__m128d vInput) { return vInput.m128d_f64[0]; }
#endif
#endif
}
#endif
#if defined(BURGER_INTELARCHITECTURE) && (defined(BURGER_LLVM) || defined(BURGER_GNUC))
BURGER_INLINE void __cpuid(int a[4],int b) {
	__asm__ __volatile__("cpuid"
	: "=a" ((a)[0]), "=b" ((a)[1]), "=c" ((a)[2]), "=d" ((a)[3])
	: "0" (b), "2" (0));
}
BURGER_INLINE void __cpuidex(int a[4],int b,int c) {
	__asm__ __volatile__("cpuid"
		: "=a" ((a)[0]), "=b" ((a)[1]), "=c" ((a)[2]), "=d" ((a)[3])
		: "0" (b), "2" (c));
}
#elif defined(BURGER_X86)
#if defined(BURGER_METROWERKS)
BURGER_INLINE void __cpuid(int a[4],int b) {
	asm {
		mov	esi,a		// Get the pointer to the destination buffer
		mov eax,b		// Command byte
		xor ecx,ecx		// Set to zero (Duplicates behavior in Visual C)
		cpuid			// Invoke CPUID
		mov [esi],eax	// Store the result in the same order as Visual C
		mov [esi+4],ebx
		mov [esi+8],ecx
		mov [esi+12],edx
	}
}
BURGER_INLINE void __cpuidex(int a[4],int b,int c) {
	asm {
		mov	esi,a		// Get the pointer to the destination buffer
		mov eax,b		// Command byte
		mov ecx,c		// Get the sub command
		cpuid			// Invoke CPUID
		mov [esi],eax	// Store the result in the same order as Visual C
		mov [esi+4],ebx
		mov [esi+8],ecx
		mov [esi+12],edx
	}
}
#elif defined(BURGER_WATCOM)
extern void __cpuid(int a[4],int b);
#pragma aux __cpuid = \
	"xor ecx,ecx" \
	"cpuid" \
	"mov [esi+0],eax" \
	"mov [esi+4],ebx" \
	"mov [esi+8],ecx" \
	"mov [esi+12],edx" \
	parm [esi] [eax] modify [ebx ecx edx];

extern void __cpuidex(int a[4],int b,int c);
#pragma aux __cpuidex = \
	"cpuid" \
	"mov [esi+0],eax" \
	"mov [esi+4],ebx" \
	"mov [esi+8],ecx" \
	"mov [esi+12],edx" \
	parm [esi] [eax] [ecx] modify [ebx ecx edx];
#endif
#endif
/* END */

#endif
