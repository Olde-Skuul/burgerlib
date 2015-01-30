/***************************************

	Determine which compiler is being used and
	create standardized typedefs and macros
	so generic code can be created cross platform

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRTYPES_H__
#define __BRTYPES_H__

/* BEGIN */
// Open Watcom C++ for Win32 and MS-DOS
#if defined(__WATCOMC__)
#define BURGER_WATCOM
#define BURGER_ALIGN(x,s) (x)
#define BURGER_PREALIGN(s)
#define BURGER_POSTALIGN(s)
#define BURGER_STRUCT_PACKPUSH
#define BURGER_X86
#define BURGER_LITTLEENDIAN
#define BURGER_ASM _asm
#define BURGER_INLINE inline
#define BURGER_LONGLONG __int64
#if defined(__DOS__)
#define BURGER_MSDOS
#elif defined(__NT__)
#define BURGER_WIN32
#else
#error Watcom is not set with the proper bt=??? parameter. Try WPP386 bt=NT
#endif

// MRC for MacOS Classic and Carbon
#elif defined(__MRC__)
#define BURGER_MRC
#define BURGER_ALIGN(x,s) __align(s) (x)
#define BURGER_PREALIGN(s) __align(s)
#define BURGER_POSTALIGN(s)
#define BURGER_STRUCT_ALIGN
#define BURGER_MAC
#define BURGER_POWERPC
#define BURGER_BIGENDIAN
#define BURGER_CFM
#if (__MRC__ > 0x0300) && (__MRC__ < 0x0700) && __option(longlong)
#define BURGER_LONGLONG long long
#endif

// SC Compiler for 680x0 for Mac OS Classic
#elif defined(__SC__) && (defined(MPW_CPLUS) || defined(MPW_C))
#define BURGER_APPLESC
#define BURGER_ALIGN(x,s) __align(s) (x)
#define BURGER_PREALIGN(s) __align(s)
#define BURGER_POSTALIGN(s)
#if (__SC__ >= 0x0801)
#define BURGER_STRUCT_ALIGN
#endif
#define BURGER_MAC
#define BURGER_68K
#define BURGER_BIGENDIAN
#if defined(__CFM68K__)
#define BURGER_CFM
#endif

// Intel Compiler for Win32
#elif defined(__ICL)
#define BURGER_INTELCOMPILER
#define BURGER_ALIGN(x,s) __declspec(align(s)) (x)
#define BURGER_PREALIGN(s) __declspec(align(s))
#define BURGER_POSTALIGN(s)
#define BURGER_STRUCT_PACKPUSH
#define BURGER_X86
#define BURGER_WIN32
#define BURGER_LITTLEENDIAN
#define BURGER_ANSIAPI __cdecl
#define BURGER_API __fastcall
#define BURGER_FASTCALLENABLED
#define BURGER_INLINE __forceinline
#define BURGER_ASM _asm
#define BURGER_LONGLONG __int64

// SN Systems MIPS Compiler for PS2
#elif (defined(__R5900) || defined(__R5900__)) || (defined(__MWERKS__) && defined(__MIPS__))
#define BURGER_SNSYSTEMS
#define BURGER_ALIGN(x,s) (x) __attribute__((aligned(s)))
#define BURGER_PREALIGN(s)
#define BURGER_POSTALIGN(s) __attribute__((aligned(s)))
#define BURGER_STRUCT_PACK
#define BURGER_MIPS
#define BURGER_PS2
#define BURGER_LITTLEENDIAN
#define BURGER_LONGLONG long

// SN Systems PPC Compiler for GameCube
#elif (defined(__SN__) && defined(__PPC__)) || (defined(__MWERKS__) && defined(__PPCGEKKO__))
#define BURGER_SNSYSTEMS
#define BURGER_ALIGN(x,s) (x) __attribute__((aligned(s)))
#define BURGER_PREALIGN(s)
#define BURGER_POSTALIGN(s) __attribute__((aligned(s)))
#define BURGER_STRUCT_PACK
#define BURGER_POWERPC
#define BURGER_GEKKO
#if defined(RVL_SDK)
#define BURGER_WII
#else
#define BURGER_GAMECUBE
#endif
#define BURGER_BIGENDIAN

// CodeWarrior for ARM for Nintendo DS
#elif defined(SDK_ARM9) && defined(SDK_TS)
#define BURGER_METROWERKS
#define BURGER_ALIGN(x,s) (x) __attribute__((aligned(s)))
#define BURGER_PREALIGN(s)
#define BURGER_POSTALIGN(s) __attribute__((aligned(s)))
#define BURGER_STRUCT_PACK
#define BURGER_ARM
#define BURGER_DS
#define BURGER_LITTLEENDIAN

// Android (Shield)
#elif defined(ANDROID)
#define BURGER_GNUC
#define BURGER_ARM
#define BURGER_SHIELD
#define BURGER_ANDROID
#define BURGER_LITTLEENDIAN
#define BURGER_INLINE __inline__ __attribute__((always_inline))
#define BURGER_ALIGN(x,s) (x) __attribute__((aligned(s)))
#define BURGER_PREALIGN(s)
#define BURGER_POSTALIGN(s) __attribute__((aligned(s)))
#define BURGER_STRUCT_PACK

// Visual Studio 2003/2005/2008 for Xbox and Xbox 360
#elif defined(_XBOX) || defined(XBOX)
#define BURGER_MSVC
#define BURGER_ALIGN(x,s) __declspec(align(s)) (x)
#define BURGER_PREALIGN(s) __declspec(align(s))
#define BURGER_POSTALIGN(s)
#define BURGER_STRUCT_PACKPUSH
#if !defined(_M_IX86)
#define BURGER_POWERPC
#define BURGER_BIGENDIAN
#define BURGER_XBOX360
#define BURGER_64BITCPU
#else
#define BURGER_X86
#define BURGER_LITTLEENDIAN
#define BURGER_XBOX
#define BURGER_API __fastcall
#define BURGER_FASTCALLENABLED
#endif
#define BURGER_ANSIAPI __cdecl
#define BURGER_INLINE __forceinline
#define BURGER_ASM _asm
#define BURGER_LONGLONG __int64

// Detect PS3
#elif defined(__CELLOS_LV2__)
#define BURGER_GNUC
#define BURGER_POWERPC
#define BURGER_BIGENDIAN
#define BURGER_PS3
#define BURGER_64BITCPU
#define BURGER_INLINE inline __attribute__((always_inline))
#define BURGER_ALIGN(x,s) __declspec(align(s)) (x)
#define BURGER_PREALIGN(s) __declspec(align(s))
#define BURGER_POSTALIGN(s)
#define BURGER_STRUCT_PACKPUSH
#define BURGER_LONGLONG long long

// PS4 Orbis
#elif defined(__ORBIS__)
#define BURGER_LLVM
#define BURGER_AMD64
#define BURGER_LITTLEENDIAN
#define BURGER_PS4
#define BURGER_64BITCPU
#define BURGER_ALIGN(x,s) __declspec(align(s)) (x)
#define BURGER_PREALIGN(s) __declspec(align(s))
#define BURGER_POSTALIGN(s)
#define BURGER_STRUCT_PACKPUSH
#define BURGER_LONGLONG long long

// Metrowerks CodeWarrior for MacOS Classic, OSX, Win32
#elif defined(__MWERKS__)
#define BURGER_METROWERKS
#if defined(macintosh) && macintosh
#define BURGER_BIGENDIAN
#define BURGER_STRUCT_ALIGN
#define BURGER_MAC
#if !defined(__POWERPC__)
#define BURGER_68K
#if !defined(NDEBUG) && !defined(_DEBUG)
#if __option(sym)
#define _DEBUG
#else
#define NDEBUG
#endif
#endif
#if defined(__CFM68K) || defined(__CFM68K__)
#define BURGER_CFM
#endif
#else
#define BURGER_POWERPC
#define BURGER_CFM
#define BURGER_ALIGN(x,s) __declspec(align(s)) (x)
#define BURGER_PREALIGN(s) __declspec(align(s))
#define BURGER_POSTALIGN(s)
#endif
#elif defined(__MACH__)
#define BURGER_STRUCT_ALIGN
#define BURGER_MACOSX
#define BURGER_POWERPC
#define BURGER_BIGENDIAN
#elif !defined(__be_os) || (__be_os != __dest_os)
#define BURGER_STRUCT_PACKPUSH
#define BURGER_WIN32
#define BURGER_X86
#define BURGER_LITTLEENDIAN
#if __MWERKS__>=0x3200
#define BURGER_ANSIAPI __cdecl
#define BURGER_API __fastcall
#define BURGER_FASTCALLENABLED
#else
#define BURGER_DECLSPECNAKED asm
#endif
#define BURGER_ALIGN(x,s) __declspec(align(s)) (x)
#define BURGER_PREALIGN(s) __declspec(align(s))
#define BURGER_POSTALIGN(s)
#define BURGER_LONGLONG long long
#else
#define BURGER_BEOS
#define BURGER_STRUCT_ALIGN
#if !defined(__POWERPC__)
#define BURGER_X86
#define BURGER_LITTLEENDIAN
#else
#define BURGER_POWERPC
#define BURGER_BIGENDIAN
#endif
#endif

// XCode for MacOS X / iOS PPC, ARM and Intel
#elif defined(__GNUC__) && defined(__MACH__) && (defined(__APPLE_CPP__) || defined(__APPLE_CC__) || defined(__NEXT_CPP__))
#define BURGER_GNUC
#define BURGER_ALIGN(x,s) (x) __attribute__((aligned(s)))
#define BURGER_PREALIGN(s)
#define BURGER_POSTALIGN(s) __attribute__((aligned(s)))
#define BURGER_STRUCT_ALIGN
#if defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
#define BURGER_IOS
#else
#define BURGER_MACOSX
#endif
#define BURGER_INLINE __inline__ __attribute__((always_inline))
#if defined(__i386__)
#define BURGER_X86
#define BURGER_LITTLEENDIAN
#define BURGER_API __attribute__((regparm(3)))
#define BURGER_ANSIAPI __attribute((cdecl))
#define BURGER_DECLSPECNAKED __attribute__((naked))
#elif defined(__x86_64__)
#define BURGER_AMD64
#define BURGER_LITTLEENDIAN
#define BURGER_64BITCPU
#define BURGER_DECLSPECNAKED __attribute__((naked))
#elif defined(__ppc64__)
#define BURGER_POWERPC
#define BURGER_BIGENDIAN
#define BURGER_64BITCPU
#elif defined(__ppc__)
#define BURGER_POWERPC
#define BURGER_BIGENDIAN
#elif defined(__arm__)
#define BURGER_ARM
#if defined(__ARM_NEON__)
#define BURGER_NEON
#endif
#define BURGER_LITTLEENDIAN
#elif defined(__arm64__)
#define BURGER_ARM64
#if defined(__ARM_NEON__)
#define BURGER_NEON
#endif
#define BURGER_LITTLEENDIAN
#define BURGER_64BITCPU
#else
#error Unknown CPU
#endif

// Visual Studio 2003/2005/2008 for Win32
#elif defined(_MSC_VER) && defined(_M_IX86)
#define BURGER_MSVC
#define BURGER_ALIGN(x,s) __declspec(align(s)) (x)
#define BURGER_PREALIGN(s) __declspec(align(s))
#define BURGER_POSTALIGN(s)
#define BURGER_STRUCT_PACKPUSH
#define BURGER_X86
#define BURGER_LITTLEENDIAN
#define BURGER_WIN32
#define BURGER_ANSIAPI __cdecl
#define BURGER_API __fastcall
#define BURGER_FASTCALLENABLED
#define BURGER_INLINE __forceinline
#define BURGER_ASM _asm
#define BURGER_LONGLONG __int64

// Visual Studio 2005/2008 for Win64
#elif defined(_MSC_VER) && defined(_M_AMD64)
#define BURGER_MSVC
#define BURGER_ALIGN(x,s) __declspec(align(s)) (x)
#define BURGER_PREALIGN(s) __declspec(align(s))
#define BURGER_POSTALIGN(s)
#define BURGER_STRUCT_PACKPUSH
#define BURGER_AMD64
#define BURGER_LITTLEENDIAN
#define BURGER_WIN64
#define BURGER_ANSIAPI __cdecl
#define BURGER_API __fastcall
#define BURGER_FASTCALLENABLED
#define BURGER_INLINE __forceinline
#define BURGER_ASM _asm
#define BURGER_LONGLONG __int64
#define BURGER_64BITCPU

// Sony Playstation VITA
#elif defined(__psp2__)
#if defined(__GNUC__)
#define BURGER_GNUC
#endif
#if defined(__SNC__)
#define BURGER_SNSYSTEMS
#endif
#define BURGER_VITA
#define BURGER_ANDROID
#define BURGER_INLINE __inline__ __attribute__((always_inline))
#define BURGER_ALIGN(x,s) (x) __attribute__((aligned(s)))
#define BURGER_PREALIGN(s)
#define BURGER_POSTALIGN(s) __attribute__((aligned(s)))
#define BURGER_STRUCT_ALIGN
#define BURGER_ARM
#define BURGER_NEON
#define BURGER_LITTLEENDIAN
#else
#error Unknown compiler / platform
#endif

#if defined(BURGER_WIN32) || defined(BURGER_WIN64)
#define BURGER_WINDOWS
#endif

#if defined(BURGER_MAC)
#if !defined(TARGET_API_MAC_CARBON) || (TARGET_API_MAC_CARBON==0)
#define BURGER_MACCLASSIC
#else
#define BURGER_MACCARBON
#endif
#endif

#if defined(BURGER_MAC) || defined(BURGER_MACOSX)
#define BURGER_MACOS
#endif

#if !defined(BURGER_API)
#define BURGER_API
#endif

#if !defined(BURGER_ANSIAPI)
#define BURGER_ANSIAPI
#endif

#if !defined(BURGER_MAXINT)
#define BURGER_MAXINT 0x7FFFFFFF
#endif

#if !defined(BURGER_MAXUINT)
#define BURGER_MAXUINT 0xFFFFFFFFU
#endif

#if !defined(BURGER_ASM)
#define BURGER_ASM asm
#endif

#if !defined(BURGER_DECLSPECNAKED)
#define BURGER_DECLSPECNAKED __declspec(naked)
#endif

#if !defined(BURGER_LONGLONG) && !defined(DOXYGEN)
#define BURGER_LONGLONG long long
#endif

#ifndef BURGER_INLINE
#if defined(__cplusplus) || defined(__GNUC__) || defined(__MWERKS__) || defined(DOXYGEN)
#define BURGER_INLINE inline
#elif defined(BURGER_WINDOWS)
#define BURGER_INLINE __inline
#else
#define BURGER_INLINE
#endif
#endif

#ifndef BURGER_ALIGN
#define BURGER_ALIGN(x,s) x
#endif

#ifndef BURGER_PREALIGN
#define BURGER_PREALIGN(s)
#endif

#ifndef BURGER_POSTALIGN
#define BURGER_POSTALIGN(s)
#endif

#if !defined(BURGER_LITTLEENDIAN) && !defined(BURGER_BIGENDIAN)
#if defined(BURGER_X86) || defined(BURGER_AMD64) || defined(BURGER_PS2) || defined(BURGER_IOS) || defined(BURGER_DS)
#define BURGER_LITTLEENDIAN
#else
#define BURGER_BIGENDIAN
#endif
#endif

#if !defined(NULL)
#define NULL 0
#endif

#if !defined(TRUE)
#define TRUE 1
#endif

#if !defined(FALSE)
#define FALSE 0
#endif

#if defined(NDEBUG) && defined(_DEBUG)
#error NDEBUG and _DEBUG are both defined. Choose one or the other
#endif

#if !defined(NDEBUG) && !defined(_DEBUG)
#error NDEBUG nor _DEBUG were defined. Choose one or the other.
#endif

#if defined(BURGER_WINDOWS) || defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_SHIELD) || defined(BURGER_LINUX) || defined(DOXYGEN)
#define BURGER_OPENGL_SUPPORTED
#endif

#if defined(BURGER_IOS) || defined(BURGER_SHIELD) || defined(DOXYGEN)
#define BURGER_OPENGLES
#endif

#define BURGER_OFFSETOF(__type,member) (reinterpret_cast<WordPtr>(&(reinterpret_cast<const __type *>(1)->member))-1)
#define BURGER_ARRAYSIZE(x) static_cast<WordPtr>(sizeof(x)/sizeof(x[0]))
#define BURGER_UNUSED(x) (void)(x)
#define BURGER_HASHMACRO(x) #x
#define BURGER_MACRO_TO_STRING(x) BURGER_HASHMACRO(x)
#if defined(DOXYGEN)
#define BURGER_DISABLECOPYCONSTRUCTORS(x) private:
#else
#define BURGER_DISABLECOPYCONSTRUCTORS(x) private: x(x const &); x &operator = (x const &)
#endif

typedef signed char Int8;
typedef unsigned char Word8;
typedef signed short Int16;
typedef unsigned short Word16;
#if defined(BURGER_PS2) || defined(BURGER_64BITCPU)
typedef signed int Int32;
typedef unsigned int Word32;
#else
typedef signed long Int32;
typedef unsigned long Word32;
#endif
typedef signed BURGER_LONGLONG Int64;
typedef unsigned BURGER_LONGLONG Word64;
typedef Word8 Bool;
typedef Int32 Frac32;
typedef Int32 Fixed32;
typedef unsigned int Word;
typedef signed int Int;

#if defined(BURGER_PS2) || defined(BURGER_PSP)
typedef unsigned int Vector_128 __attribute__((mode (TI)));
#elif defined(BURGER_PS3)
#ifndef __VEC_TYPES_H__
#include <vec_types.h>
#endif
typedef vec_float4 Vector_128;
#elif defined(BURGER_PS4)
typedef float Vector_128 __attribute__((__vector_size__(16)));
#elif defined(BURGER_XBOX360)
#ifndef __PPCINTRINSICS_H__
#include <ppcintrinsics.h>
#endif
#ifndef __VECTORINTRINSICS_H__
#include <vectorintrinsics.h>
#endif
typedef __vector4 Vector_128;
#elif defined(BURGER_MSVC) || defined(BURGER_XBOX)
#ifndef _INCLUDED_EMM
#include <emmintrin.h>
#endif
typedef __m128 Vector_128;
#elif defined(BURGER_METROWERKS) && defined(BURGER_X86)
#ifndef _XMMINTRIN_H
#include <xmmintrin.h>
#endif
typedef __m128 Vector_128;
#elif defined(BURGER_ANDROID)
#ifndef __ARM_NEON_H
#include <arm_neon.h>
#endif
typedef float32x4_t Vector_128;
#else
struct Vector_128 {
	float BURGER_ALIGN(m128_f32[4],16);			///< Opaque contents to the 128 bit vector register
};
#endif

#if defined(BURGER_64BITCPU) && !defined(BURGER_XBOX360) && !defined(BURGER_PS3)
typedef Word64 WordPtr;
typedef Int64 IntPtr;
#define BURGER_MAXWORDPTR 0xFFFFFFFFFFFFFFFFULL
#define BURGER_MAXINTPTR 0x7FFFFFFFFFFFFFFFLL
#else
typedef Word32 WordPtr;
typedef Int32 IntPtr;
#define BURGER_MAXWORDPTR 0xFFFFFFFFU
#define BURGER_MAXINTPTR 0x7FFFFFFF
#endif

#if !defined(BURGER_NONEW)
#if defined(BURGER_METROWERKS)
BURGER_INLINE void *operator new(__typeof__(sizeof(0)), void*x) { return x; }
#elif defined(BURGER_MSVC)
#define __PLACEMENT_NEW_INLINE
BURGER_INLINE void* BURGER_ANSIAPI operator new(size_t, void*x) {return x;}
#elif defined(__GNUC__) && defined(__MACH__) && (defined(__APPLE_CPP__) || defined(__APPLE_CC__) || defined(__NEXT_CPP__)) && defined(BURGER_64BITCPU)
BURGER_INLINE void* operator new(unsigned long int,void *x) {return x;}
#elif defined(BURGER_PS4)
BURGER_INLINE void* operator new(unsigned long, void*x) {return x;}
#elif defined(BURGER_ANDROID) || defined(BURGER_SNSYSTEMS)
BURGER_INLINE void* operator new(unsigned int, void*x) {return x;}
#else
BURGER_INLINE void* operator new(WordPtr, void*x) {return x;}
#endif
#endif

namespace Burger {
template<class T>
BURGER_INLINE void SwapVariables(T *pA,T *pB) {
	T tTemp(*pA);
	*pA = *pB;
	*pB = tTemp;
}
}
/* END */

#endif
