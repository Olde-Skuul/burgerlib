/***************************************

    Determine which compiler is being used and create standardized typedefs
    and macros so generic code can be created cross platform

    Copyright (c) 1995-2019 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRTYPES_H__
#define __BRTYPES_H__

/* BEGIN */

// Require C++
#if !defined(__cplusplus)
#error C++ compiler is required
#endif

// Generic simple macros
#define BURGER_HASHMACRO(x) #x
#define BURGER_MACRO_TO_STRING(x) BURGER_HASHMACRO(x)

// Test for the flavor of C++, assume it's at least C++89
#define BURGER_CPP89 __cplusplus

// Try C++98
#if (__cplusplus >= 199711L)
#define BURGER_CPP98 __cplusplus
#endif

// At C++11 or later, things get odd.
#if (__cplusplus >= 201103L)
#define BURGER_CPP11 __cplusplus
// SN Systems using Edison Design Group front end
#elif defined(__SNC__) && (defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403))
#if __option(cpp11)
#define BURGER_CPP11 201103L
#endif
// Because, Microsoft
#elif (defined(_MSVC_LANG) && (_MSVC_LANG >= 201103L))
#define BURGER_CPP11 _MSVC_LANG
#endif

// Test for C++14
#if (__cplusplus >= 201402L)
#define BURGER_CPP14 __cplusplus
#elif (defined(_MSVC_LANG) && (_MSVC_LANG >= 201402L))
#define BURGER_CPP14 _MSVC_LANG
#endif

// Test for C++17
#if (__cplusplus >= 201700L)
#define BURGER_CPP17 __cplusplus
#elif (defined(_MSVC_LANG) && (_MSVC_LANG >= 201700L))
#define BURGER_CPP17 _MSVC_LANG
#endif

// To C++Infinity and beyond
#if (__cplusplus > 201703L)
#define BURGER_CPP20 __cplusplus
#elif (defined(_MSVC_LANG) && (_MSVC_LANG > 201703L))
#define BURGER_CPP20 _MSVC_LANG
#endif

// Create the flavor of C++ as a string
#if defined(BURGER_CPP20)
#define BURGER_STDCPP_TYPE "C++20"
#elif defined(BURGER_CPP17)
#define BURGER_STDCPP_TYPE "C++17"
#elif defined(BURGER_CPP14)
#define BURGER_STDCPP_TYPE "C++14"
#elif defined(BURGER_CPP11)
#define BURGER_STDCPP_TYPE "C++11"
#elif defined(BURGER_CPP98)
#define BURGER_STDCPP_TYPE "C++98"
#else
#define BURGER_STDCPP_TYPE "C++89"
#endif

// Get the compiler's name and version
#if defined(__WATCOMC__)
#define BURGER_COMPILER_NAME "Open Watcom Compiler"
#define BURGER_WATCOM __WATCOMC__
#define BURGER_COMPILER_VERSION __WATCOMC__

#elif defined(__MRC__)
#define BURGER_COMPILER_NAME "Apple PowerPC MrCPP Compiler"
#if __MRC__ < 0x500
#error MPW MrCpp 5.0 or better compiler required
#endif

#if !__option(longlong)
#error 64 bit integers required, use '-longlong on'
#endif

#if !__option(bool)
#error bool required, use '-bool on'
#endif

#define BURGER_MRC __MRC__
#define BURGER_COMPILER_VERSION __MRC__

#elif defined(__SC__)
#define BURGER_COMPILER_NAME "Symantec C++ Compiler"
#if __SC__ < 0x890
#error MPW SCpp 8.9 or better compiler required
#endif

#if !__option(bool)
#error bool required, use '-bool on'
#endif

#define BURGER_APPLE_SC __SC__
#define BURGER_COMPILER_VERSION __SC__

#elif defined(__DJGPP__)
#define BURGER_COMPILER_NAME "DJ's GNU Programming Platform"
#define BURGER_DJGPP __DJGPP__
#define BURGER_COMPILER_VERSION ((__DJGPP__ * 100) + __DJGPP_MINOR__)

#elif defined(__SN__) || defined(__SNC__)
#define BURGER_COMPILER_NAME "SN Systems Compiler"
#define BURGER_SNSYSTEMS __SN_VER__
#define BURGER_COMPILER_VERSION __SN_VER__

#elif defined(__ghs__)
#define BURGER_COMPILER_NAME "Green Hills Compiler"
#define BURGER_GHS __GHS_VERSION_NUMBER
#define BURGER_COMPILER_VERSION __GHS_VERSION_NUMBER

#elif defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || \
    defined(__ECC)
#define BURGER_COMPILER_NAME "Intel Compiler"
#if defined(__INTEL_COMPILER_UPDATE)
#define BURGER_INTEL_COMPILER \
    ((__INTEL_COMPILER * 10) + __INTEL_COMPILER_UPDATE)
#else
#define BURGER_INTEL_COMPILER (__INTEL_COMPILER * 10)
#endif
#define BURGER_COMPILER_VERSION BURGER_INTEL_COMPILER

#elif defined(__clang__)
#define BURGER_COMPILER_NAME "clang Compiler"
#if defined(__clang_patchlevel__)
#define BURGER_CLANG \
    ((__clang_major__ * 10000) + (__clang_minor__ * 100) + __clang_patchlevel__)
#else
#define BURGER_CLANG ((__clang_major__ * 10000) + (__clang_minor__ * 100))
#endif
#define BURGER_COMPILER_VERSION BURGER_CLANG

#elif defined(__GNUC__)
#define BURGER_COMPILER_NAME "GNUC Compiler"
#if defined(__GNUC_PATCHLEVEL__)
#define BURGER_GNUC \
    ((__GNUC__ * 10000) + (__GNUC_MINOR__ * 100) + __GNUC_PATCHLEVEL__)
#else
#define BURGER_GNUC ((__GNUC__ * 10000) + (__GNUC_MINOR__ * 100))
#endif
#define BURGER_COMPILER_VERSION BURGER_GNUC

#elif defined(__MWERKS__) || defined(__CWCC__)
#define BURGER_COMPILER_NAME "Metrowerks Codewarrior"
#if !__option(bool)
#error bool required, check the "Enable bool support" box
#endif

#define BURGER_METROWERKS __MWERKS__
#define BURGER_COMPILER_VERSION __MWERKS__

#elif defined(__CC_ARM)
#define BURGER_COMPILER_NAME "ARM RealView C++ Compiler"
#define BURGER_ARM_COMPILER __ARMCC_VERSION
#define BURGER_COMPILER_VERSION __ARMCC_VERSION

#elif defined(_MSC_VER)
#define BURGER_COMPILER_NAME "Microsoft Visual C++"
// Ensure it's a 9 digit version number
#if defined(_MSC_FULL_VER)
#if _MSC_FULL_VER < 100000000
#define BURGER_MSVC (_MSC_FULL_VER * 10)
#else
#define BURGER_MSVC _MSC_FULL_VER
#endif
#else
#define BURGER_MSVC (_MSC_VER * 100000)
#endif
#define BURGER_COMPILER_VERSION BURGER_MSVC
#else

#define BURGER_COMPILER_NAME "Unknown Compiler"
#error Unknown compiler
#endif

// Detect the CPU being compiled for
#if defined(__386__) || defined(_M_IX86) || defined(__i386__) || \
    defined(__X86__) || defined(_X86_) || defined(__INTEL__)
#define BURGER_X86
#define BURGER_CPU_TYPE "Intel 32 bit 386"

#elif defined(_M_AMD64) || defined(_M_X64) || defined(__x86_64__) || \
    defined(__amd64__)
#define BURGER_AMD64
#define BURGER_CPU_TYPE "AMD64 64 bit CPU"

#elif defined(_IA64_) || defined(__ia64__) || defined(_M_IA64) || \
    defined(__itanium__)
#define BURGER_ITANIUM
#define BURGER_CPU_TYPE "Intel Itanium 64 bit"

#elif defined(_M_ARM) || defined(__arm__) || defined(__arm) || \
    defined(_ARM) || defined(__TARGET_ARCH_ARM)
#define BURGER_ARM32
#define BURGER_CPU_TYPE "ARM 32 bit"

#elif defined(_M_ARM64) || defined(__arm64__) || defined(__aarch64__)
#define BURGER_ARM64
#define BURGER_CPU_TYPE "ARM 64 bit"

#elif defined(__mips__) || defined(__R5900) || defined(__R5900__) || \
    defined(__MIPS__)
#if (_MIPS_SIM == _ABIO32) || (_MIPS_SIM == _ABIN32) || defined(__R5900) || \
    defined(__R5900__) || defined(__MIPS__)
#define BURGER_MIPS32
#define BURGER_CPU_TYPE "Mips 32 bit"
#elif _MIPS_SIM == _ABI64
#define BURGER_MIPS64
#define BURGER_CPU_TYPE "Mips 64 bit"
#else
#error Unknown MIPS CPU
#endif

#elif (defined(_XBOX) && defined(_M_PPCBE)) || defined(__ppc64__) || \
    defined(__powerpc64__) || defined(_ARCH_PPC64)
#define BURGER_POWERPC64
#define BURGER_CPU_TYPE "PowerPC 64 bit"

#elif defined(__POWERPC__) || defined(__ppc__) || defined(__powerpc) || \
    defined(_MPPC_) || defined(_M_PPC) || defined(__PPCGECKO__) || \
    defined(__PPCBROADWAY__)
#define BURGER_POWERPC
#define BURGER_CPU_TYPE "PowerPC 32 bit"

#elif defined(__MC68K__) || defined(_68K_) || defined(__m68k__)
#define BURGER_68K
#define BURGER_CPU_TYPE "Motorola 68000"

#elif defined(__riscv)
#define BURGER_RISCV
#define BURGER_CPU_TYPE "RISC-V"

#elif defined(__AVR_ARCH__)
#define BURGER_AVR
#define BURGER_CPU_TYPE "Atmel AVR"

#elif defined(__MSP430__)
#define BURGER_MSP430
#define BURGER_CPU_TYPE "Texas Instruments MSP430"

#elif defined(__sparc64__)
#define BURGER_SPARC64
#define BURGER_CPU_TYPE "Sun SPARC 64 bit"

#elif defined(__sparc__) || defined(__sparc)
#define BURGER_SPARC32
#define BURGER_CPU_TYPE "Sun SPARC 32 bit"

#else
#error Unknown CPU
#define BURGER_CPU_TYPE "Unknown CPU"
#endif

// Generic CPU class macros

// Intel x86 or AMD 64 CPU?
#if defined(BURGER_X86) || defined(BURGER_AMD64)
#define BURGER_INTEL
#endif

// PowerPC cpus
#if defined(BURGER_POWERPC) || defined(BURGER_POWERPC64)
#define BURGER_PPC
#endif

// ARM CPUs
#if defined(BURGER_ARM32) || defined(BURGER_ARM64)
#define BURGER_ARM
#endif

// MIPS CPUs
#if defined(BURGER_MIPS32) || defined(BURGER_MIPS64)
#define BURGER_MIPS
#endif

// SPARC CPUs
#if defined(BURGER_SPARC32) || defined(BURGER_SPARC64)
#define BURGER_SPARC
#endif

// 64 bit support?
#if defined(BURGER_ARM64) || defined(BURGER_POWERPC64) || \
    defined(BURGER_AMD64) || defined(BURGER_MIPS64) || \
    defined(BURGER_SPARC64) || defined(BURGER_ITANIUM)
#define BURGER_64BITCPU
#endif

// Special CPU extensions
#if defined(__ARM_NEON__)
#define BURGER_NEON
#endif

#if defined(mc68881) || defined(__MC68881__)
#define BURGER_68881
#endif

#if defined(__ALTIVEC__)
#define BURGER_ALTIVEC
#endif

#if defined(BURGER_METROWERKS)
#if defined(BURGER_INTEL) && __option(k63d)
#define BURGER_3DNOW
#endif
#endif

#if defined(BURGER_AMD64) || (defined(BURGER_MSVC) && (_M_IX86_FP >= 1)) || \
    defined(__SSE__)
#define BURGER_SSE
#elif defined(BURGER_METROWERKS)
#if defined(BURGER_INTEL) && __option(sse)
#define BURGER_SSE
#endif
#endif

#if defined(BURGER_AMD64) || (defined(BURGER_MSVC) && (_M_IX86_FP >= 2)) || \
    defined(__SSE2__)
#define BURGER_SSE2
#elif defined(BURGER_METROWERKS)
#if defined(BURGER_INTEL) && __option(sse2)
#define BURGER_SSE2
#endif
#endif

#if defined(__AVX__)
#define BURGER_AVX
#endif

#if defined(__AVX2__)
#define BURGER_AVX2
#endif

// Platform
#if defined(_XBOX) || defined(XBOX)
#if defined(BURGER_X86)
#define BURGER_XBOX
#elif defined(BURGER_POWERPC64)
#define BURGER_XBOX360
#else
#define BURGER_XBOXONE
#endif

#elif defined(__DOS__) || defined(__MSDOS__) || defined(_MSDOS)
#define BURGER_MSDOS
#if defined(__X32__)
#define BURGER_X32
#elif defined(__DOS4G__) || defined(__DOS4GW__) || defined(__GO32__)
#define BURGER_DOS4G
#else
#error Unknown MSDOS extender, try using Dos4g
#endif

#elif defined(__NT__) || defined(__WIN32__) || defined(__WINDOWS__) || \
    defined(__TOS_WIN__) || defined(_WIN32) || defined(_WIN64)
#if defined(BURGER_64BITCPU)
#define BURGER_WIN64
#else
#define BURGER_WIN32
#endif

#elif defined(SN_TARGET_PS2)
#define BURGER_PS2

#elif defined(__CELLOS_LV2__)
#define BURGER_PS3

#elif defined(__ORBIS__)
#define BURGER_PS4

#elif defined(SN_TARGET_PSP) || defined(SN_TARGET_PSP_PRX)
#define BURGER_PSP

#elif defined(__psp2__)
#define BURGER_VITA

#elif defined(__ANDROID__) || defined(__ANDROID_API__)
#define BURGER_ANDROID
#define BURGER_SHIELD

#elif defined(SDK_ARM9) && defined(SDK_TS)
#define BURGER_DS

#elif defined(NN_COMPILER_RVCT) || defined(NN_PLATFORM_CTR)
#define BURGER_3DS

#elif defined(__PPCGECKO__)
#define BURGER_GAMECUBE

#elif defined(RVL) || defined(RVL_SDK) || defined(RVL_OS) || \
    defined(__PPCBROADWAY__)
#define BURGER_WII

#elif defined(__espresso__)
#define BURGER_WIIU

#elif defined(NN_NINTENDO_SDK)
#define BURGER_SWITCH

#elif defined(__BEOS__)
#define BURGER_BEOS

#elif defined(__SYMBIAN32__)
#define BURGER_SYMBIAN

#elif defined(AMIGA) || defined(__amigaos__)
#define BURGER_AMIGA

#elif defined(macintosh) && macintosh
#define BURGER_MAC
#if defined(__CFM68K) || defined(__CFM68K__) || defined(BURGER_POWERPC)
#define BURGER_CFM
#endif
#if !defined(TARGET_API_MAC_CARBON) || (TARGET_API_MAC_CARBON == 0)
#define BURGER_MACCLASSIC
#else
#define BURGER_MACCARBON
#endif

#elif defined(__APPLE__) && defined(__MACH__) && \
    defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
#define BURGER_IOS

#elif defined(__APPLE__) && defined(__MACH__)
#define BURGER_MACOSX

#elif defined(__LINUX__) || defined(__linux__) || defined(__gnu_linux__)
#define BURGER_LINUX

#else
#error Unknown Platform
#endif

// Generic windows platform?
#if defined(BURGER_WIN32) || defined(BURGER_WIN64)
#define BURGER_WINDOWS
#endif

// Any form of MacOS
#if defined(BURGER_MAC) || defined(BURGER_MACOSX)
#define BURGER_MACOS
#endif

// Determine the endian by testing the platform / CPU
#if defined(BURGER_68K) || defined(BURGER_PPC) || defined(BURGER_SPARC) || \
    (defined(BURGER_MIPS) && _MIPSEL)
#define BURGER_BIGENDIAN
#else
#define BURGER_LITTLEENDIAN
#endif

// Create the endian indexes (0,1 for little, 1,0 for big)

#if defined(BURGER_LITTLEENDIAN)
#define BURGER_ENDIANINDEX_LOW 0
#else
#define BURGER_ENDIANINDEX_LOW 1
#endif

#define BURGER_ENDIANINDEX_HIGH (1 - BURGER_ENDIANINDEX_LOW)

#if defined(BURGER_INTEL_COMPILER) || defined(BURGER_CLANG) || \
    defined(BURGER_GNUC) || defined(_NATIVE_WCHAR_T_DEFINED) || \
    defined(BURGER_MINGW)
#define BURGER_HAS_WCHAR_T
#elif defined(BURGER_METROWERKS)
#if __option(wchar_type)
#define BURGER_HAS_WCHAR_T
#endif
#endif

// API to use for Burgerlib
#if defined(BURGER_X86)
#if defined(BURGER_WATCOM)
#define BURGER_API __watcall
#elif defined(BURGER_MSVC) || defined(BURGER_INTEL_COMPILER) || \
    defined(BURGER_MINGW)
#define BURGER_API __fastcall
#define BURGER_FASTCALLENABLED
#elif defined(BURGER_DJGPP) || defined(BURGER_CLANG) || defined(BURGER_GNUC)
#define BURGER_API __attribute__((fastcall))
#define BURGER_FASTCALLENABLED
#elif defined(BURGER_METROWERKS) && (BURGER_METROWERKS >= 0x3200)
#define BURGER_API __fastcall
#define BURGER_FASTCALLENABLED
#endif
#endif

#if !defined(BURGER_API)
#define BURGER_API
#endif

// API override for stdlib calls
#if defined(BURGER_X86)
#if defined(BURGER_WATCOM)
#define BURGER_ANSIAPI __watcall
#elif defined(BURGER_MSVC) || defined(BURGER_INTEL_COMPILER) || \
    defined(BURGER_MINGW)
#define BURGER_ANSIAPI __cdecl
#elif defined(BURGER_DJGPP) || defined(BURGER_CLANG) || defined(BURGER_GNUC)
#define BURGER_ANSIAPI __attribute((cdecl))
#elif defined(BURGER_METROWERKS) && (BURGER_METROWERKS >= 0x3200)
#define BURGER_ANSIAPI __cdecl
#endif
#endif

#if !defined(BURGER_ANSIAPI)
#define BURGER_ANSIAPI
#endif

// Set the inline keyword
#if defined(BURGER_WATCOM)
#define BURGER_INLINE __inline
#elif defined(BURGER_MSVC) || defined(BURGER_INTEL_COMPILER) || \
    defined(BURGER_MINGW)
#define BURGER_INLINE __forceinline
#elif defined(BURGER_DJGPP) || defined(BURGER_CLANG) || \
    defined(BURGER_GNUC) || defined(BURGER_GHS)
#define BURGER_INLINE __inline__ __attribute__((always_inline))
#elif defined(BURGER_METROWERKS) && (BURGER_METROWERKS >= 0x3200)
#define BURGER_INLINE __inline
#endif

#if !defined(BURGER_INLINE)
#define BURGER_INLINE inline
#endif

// Asm keyword for X86 instructions
#if defined(BURGER_WATCOM) || defined(BURGER_INTEL_COMPILER) || \
    defined(BURGER_MINGW)
#define BURGER_ASM _asm
#elif defined(BURGER_MSVC)
#define BURGER_ASM __asm
#else
#define BURGER_ASM asm
#endif

// Pure assembly functions

#if defined(BURGER_METROWERKS) || \
    ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC))
#define BURGER_DECLSPECNAKED asm
#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_INTEL))
#define BURGER_DECLSPECNAKED int int not supported
#else
#define BURGER_DECLSPECNAKED __declspec(naked)
#endif

// Alignment commands
#if defined(BURGER_INTEL_COMPILER) || defined(BURGER_MSVC) || \
    defined(BURGER_PS4) || (defined(BURGER_METROWERKS) && !defined(BURGER_68K))
#define BURGER_ALIGN(x, s) __declspec(align(s))(x)
#define BURGER_PREALIGN(s) __declspec(align(s))
#define BURGER_POSTALIGN(s)
#elif defined(BURGER_GNUC) || defined(BURGER_CLANG) || \
    defined(BURGER_SNSYSTEMS) || defined(BURGER_ARM_COMPILER) || \
    defined(BURGER_GHS)
#define BURGER_ALIGN(x, s) (x) __attribute__((aligned(s)))
#define BURGER_PREALIGN(s)
#define BURGER_POSTALIGN(s) __attribute__((aligned(s)))
#else
#define BURGER_ALIGN(x, s) (x)
#define BURGER_PREALIGN(s)
#define BURGER_POSTALIGN(s)
#endif

// Structure packing macro switches
#if defined(BURGER_PS2) || defined(BURGER_ANDROID) || defined(BURGER_DS)
#define BURGER_STRUCT_PACK
#elif defined(BURGER_WATCOM) || defined(BURGER_INTEL_COMPILER) || \
    defined(BURGER_ARM_COMPILER) || defined(BURGER_MSVC) || \
    defined(BURGER_PS3) || defined(BURGER_PS4) || defined(BURGER_MINGW) || \
    (defined(BURGER_METROWERKS) && defined(BURGER_INTEL))
#define BURGER_STRUCT_PACKPUSH
#elif defined(BURGER_MRC) || defined(BURGER_APPLE_SC) || \
    defined(BURGER_METROWERKS) || defined(BURGER_MACOSX) || \
    defined(BURGER_IOS) || defined(BURGER_GNUC) || defined(BURGER_VITA)
#define BURGER_STRUCT_ALIGN
#endif

// Hack for remapping uint to a uintptr_t for some compilers
#if defined(BURGER_WATCOM)
#define BURGER_NEED_UINTPTR_REMAP uint32_t
#elif defined(BURGER_MACOSX) || defined(BURGER_IOS)
#if defined(__LP64__)
#define BURGER_NEED_UINTPTR_REMAP uint64_t
#else
#define BURGER_NEED_UINTPTR_REMAP uint32_t
#endif
#endif

// Special cases for int64 support
#if defined(BURGER_APPLE_SC)
class longlong_t;
class ulonglong_t;
#define BURGER_LONGLONG longlong_t
#define BURGER_ULONGLONG ulonglong_t
#elif defined(BURGER_INTEL_COMPILER) || defined(BURGER_MSVC)
#define BURGER_LONGLONG __int64
#define BURGER_ULONGLONG unsigned __int64
#elif defined(BURGER_PS2) || defined(__LP64__)
#define BURGER_LONGLONG long
#define BURGER_ULONGLONG unsigned long
#define BURGER_LONGIS64BIT
#define BURGER_SIZEOF_LONG 8
#else
#define BURGER_LONGLONG long long
#define BURGER_ULONGLONG unsigned long long
#endif

#if !defined(BURGER_SIZEOF_INT) && !defined(DOXYGEN)
#define BURGER_SIZEOF_INT 4
#endif

#if !defined(BURGER_SIZEOF_LONG) && !defined(DOXYGEN)
#define BURGER_SIZEOF_LONG 4
#endif

// Clang support
#if !defined(__has_builtin)
#define __has_builtin(x) 0
#endif

#if !defined(__has_feature) && !defined(BURGER_SNSYSTEMS)
#define __has_feature(x) 0
#endif

#if !defined(__has_extension)
#define __has_extension __has_feature
#endif

#if !defined(__has_cpp_attribute)
#define __has_cpp_attribute(x) 0
#endif

#if !defined(__has_declspec_attribute)
#define __has_declspec_attribute(x) 0
#endif

// Set up generic defines

// Set up null defines if not already set up
#if (!defined(BURGER_UNDERLYING_TYPE) && !defined(BURGER_CPP11)) || \
    defined(DOXYGEN)
#define __underlying_type(x) int
#endif

// Standard defines (Can be overridden)
#if !defined(BURGER_MININT)
#define BURGER_MININT ((-0x7FFFFFFF) - 1)
#endif

#if !defined(BURGER_MAXINT)
#define BURGER_MAXINT 0x7FFFFFFF
#endif

#if !defined(BURGER_MAXUINT)
#define BURGER_MAXUINT 0xFFFFFFFFU
#endif

#if !defined(BURGER_MININT64)
#define BURGER_MININT64 ((-0x7FFFFFFFFFFFFFFFLL) - 1)
#endif

#if !defined(BURGER_MAXINT64)
#define BURGER_MAXINT64 0x7FFFFFFFFFFFFFFFLL
#endif

#if !defined(BURGER_MAXUINT64)
#define BURGER_MAXUINT64 0xFFFFFFFFFFFFFFFFULL
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

// Test for static_assert

#if defined(BURGER_CPP11) || \
    (defined(__clang__) && __has_feature(cxx_static_assert)) || \
    (defined(__GXX_EXPERIMENTAL_CXX0X__) && (BURGER_GNUC >= 40300)) || \
    (BURGER_MSVC >= 160000000) || defined(DOXYGEN)
#define BURGER_COMPILE_TIME_ASSERT(x) static_assert(x, #x)
#else
#define BURGER_COMPILE_TIME_ASSERT(x) \
    typedef int ThisIsABogusTypeDef##__LINE__[(x)*2 - 1]
#endif

// Test for constexpr support
#if defined(__cpp_constexpr) || __has_feature(cxx_constexpr) || \
    (defined(__ICL) && (__INTEL_COMPILER >= 1300) && \
        defined(__GXX_EXPERIMENTAL_CXX0X__)) || \
    (!defined(__EDG__) && (BURGER_GNUC >= 40600) && \
        defined(__GXX_EXPERIMENTAL_CXX0X__)) || \
    defined(__IBMCPP_CONSTEXPR) || (BURGER_MSVC >= 190000000) || \
    defined(DOXYGEN)
#define BURGER_CONSTEXPR constexpr
#else
#define BURGER_CONSTEXPR
#endif

// Test for nullptr support
#if defined(BURGER_CPP11) || (BURGER_CLANG >= 20900) || \
    (BURGER_GNUC >= 40600) || (BURGER_MSVC >= 160000000)
#else
#define nullptr 0
#endif

// Test for override support
#if (BURGER_CLANG >= 20900) || (BURGER_GNUC >= 40700) || \
    (BURGER_MSVC >= 140000000) || __has_feature(cxx_override_control) || \
    defined(DOXYGEN)
#define BURGER_OVERRIDE override
#else
#define BURGER_OVERRIDE
#endif

// Test for = delete support
#if defined(BURGER_CPP11) || __has_feature(cxx_deleted_functions) || \
    (defined(__GXX_EXPERIMENTAL_CXX0X__) && (BURGER_GNUC >= 40400)) || \
    defined(DOXYGEN)
#define BURGER_EQUALS_DELETE = delete
#define BURGER_EQUALS_DEFAULT = default
#else
#define BURGER_EQUALS_DELETE
#define BURGER_EQUALS_DEFAULT
#endif

// Test for final support
#if defined(BURGER_CPP11) || __has_feature(cxx_override_control) || \
    (defined(__GXX_EXPERIMENTAL_CXX0X__) && (BURGER_GNUC >= 40700)) || \
    (BURGER_MSVC >= 170000000) || defined(DOXYGEN)
#define BURGER_FINAL final
#elif (BURGER_MSVC >= 140000000)
#define BURGER_FINAL sealed
#elif (BURGER_GNUC >= 40700)
#define BURGER_FINAL __final
#else
#define BURGER_FINAL
#endif

// Test for && rvalue support
#if defined(BURGER_CPP11) || (BURGER_MSVC >= 160000000) || \
    __has_feature(cxx_rvalue_references) || \
    (defined(__GXX_EXPERIMENTAL_CXX0X__) && (BURGER_GNUC >= 40300))
#define BURGER_RVALUE_REFERENCES
#endif

// Use enum class when available
#if __has_feature(cxx_strong_enums) || __has_extension(cxx_strong_enums) || \
    __has_feature(objc_fixed_enum) || defined(BURGER_CPP11) || \
    (BURGER_MSVC >= 170000000) || defined(DOXYGEN)
#define BURGER_ENUMSTART(x, y) enum class x : y
#define BURGER_ENUMEND(x) \
    ; \
    typedef x x##Type;
#else
#define BURGER_ENUMSTART(x, y) \
    struct x { \
        enum Type
#define BURGER_ENUMEND(x) \
    ; \
    } \
    ; \
    typedef x::Type x##Type;
#endif
#define BURGER_ENUMFLAGSEND(x, y) \
    BURGER_ENUMEND \
    (x) BURGER_INLINE x##Type& operator|=(x##Type& uInput1, x##Type uInput2) \
    { \
        return uInput1 = static_cast<x##Type>( \
                   static_cast<y>(uInput1) | static_cast<y>(uInput2)); \
    } \
    BURGER_INLINE x##Type& operator&=(x##Type& uInput1, x##Type uInput2) \
    { \
        return uInput1 = static_cast<x##Type>( \
                   static_cast<y>(uInput1) & static_cast<y>(uInput2)); \
    } \
    BURGER_INLINE x##Type& operator^=(x##Type& uInput1, x##Type uInput2) \
    { \
        return uInput1 = static_cast<x##Type>( \
                   static_cast<y>(uInput1) ^ static_cast<y>(uInput2)); \
    } \
    BURGER_INLINE constexpr x##Type operator|( \
        x##Type uInput1, x##Type uInput2) \
    { \
        return static_cast<x##Type>( \
            static_cast<y>(uInput1) | static_cast<y>(uInput2)); \
    } \
    BURGER_INLINE constexpr x##Type operator&( \
        x##Type uInput1, x##Type uInput2) \
    { \
        return static_cast<x##Type>( \
            static_cast<y>(uInput1) & static_cast<y>(uInput2)); \
    } \
    BURGER_INLINE constexpr x##Type operator^( \
        x##Type uInput1, x##Type uInput2) \
    { \
        return static_cast<x##Type>( \
            static_cast<y>(uInput1) ^ static_cast<y>(uInput2)); \
    } \
    BURGER_INLINE constexpr Word operator!(x##Type uInput) \
    { \
        return !static_cast<y>(uInput); \
    } \
    BURGER_INLINE constexpr x##Type operator~(x##Type uInput) \
    { \
        return static_cast<x##Type>(~static_cast<y>(uInput)); \
    }

// Sanity check for debug defines
#if defined(NDEBUG) && defined(_DEBUG)
#error NDEBUG and _DEBUG are both defined. Choose one or the other
#endif

#if !defined(NDEBUG) && !defined(_DEBUG)
#if defined(BURGER_METROWERKS)
#if __option(sym)
#define _DEBUG
#else
#define NDEBUG
#endif
#else
#error NDEBUG nor _DEBUG were defined. Choose one or the other.
#endif
#endif

// Is OpenGL supported on this platform?
#if (!(defined(BURGER_VITA)) && \
    ((defined(BURGER_WINDOWS) && defined(BURGER_INTEL)) || \
        defined(BURGER_MACOS) || defined(BURGER_IOS) || \
        defined(BURGER_ANDROID) || defined(BURGER_LINUX))) || \
    defined(DOXYGEN)
#define BURGER_OPENGL_SUPPORTED
#endif

// Is OpenGLES supported on this platform?
#if (!(defined(BURGER_VITA)) && \
    (defined(BURGER_IOS) || defined(BURGER_ANDROID))) || \
    defined(DOXYGEN)
#define BURGER_OPENGLES
#endif

// Is Vulkan supported on this platform?
#if (defined(BURGER_ANDROID) || defined(BURGER_WINDOWS) || \
    defined(BURGER_LINUX))
#define BURGER_VULKAN
#endif

// Networking protocol support
#if (defined(BURGER_WINDOWS) || defined(BURGER_MACOS) || \
    defined(BURGER_LINUX)) || \
    defined(DOXYGEN)
#define BURGER_TCPIP
#endif

#if (defined(BURGER_MAC)) || defined(DOXYGEN)
#define BURGER_APPLETALK
#endif

#if (defined(BURGER_WINDOWS)) || defined(DOXYGEN)
#define BURGER_IPX
#endif

#if (defined(BURGER_WINDOWS) || defined(BURGER_MACOSX) || \
    defined(BURGER_LINUX)) || \
    defined(DOXYGEN)
#define BURGER_STEAM
#endif

#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
    defined(BURGER_XBOXONE)) || \
    defined(DOXYGEN)
#define BURGER_XBOXLIVE
#endif

// Helper macros

#define BURGER_OFFSETOF(__type, member) \
    (reinterpret_cast<intptr_t>(&reinterpret_cast<const __type*>(1)->member) - 1)
#define BURGER_GET_BASE_PTR(x, __type, member) \
    reinterpret_cast<__type*>( \
        reinterpret_cast<uint8_t*>(x) - BURGER_OFFSETOF(__type, member))
#define BURGER_CONST_GET_BASE_PTR(x, __type, member) \
    reinterpret_cast<const __type*>( \
        reinterpret_cast<const uint8_t*>(x) - BURGER_OFFSETOF(__type, member))

// Unused parameters or variables
#define BURGER_UNUSED(x) (void)(x)

// Macro to create copy constructors to disable the feature
#if defined(DOXYGEN)
#define BURGER_DISABLE_COPY(x) private:
#else
#define BURGER_DISABLE_COPY(x) \
private: \
    x(x const&) BURGER_EQUALS_DELETE; \
    x& operator=(x const&) BURGER_EQUALS_DELETE
#endif

// Global Data types

#if defined(BURGER_MRC) || defined(BURGER_APPLE_SC) || \
    (defined(BURGER_MSVC) && (BURGER_MSVC < 160000000))
#include <brstdint.h>
#else

// PS3 requires these
#if !defined(__STDC_CONSTANT_MACROS)
#define __STDC_CONSTANT_MACROS 1
#endif
#if !defined(__STDC_LIMIT_MACROS)
#define __STDC_LIMIT_MACROS 1
#endif

// Visual Studio 2010 has a bug where _INTPTR is not defined
#if defined(BURGER_MSVC) && !defined(_INTPTR) && (BURGER_MSVC < 170000000)
#if defined(BURGER_64BITCPU) && !defined(BURGER_XBOX360)
#define _INTPTR 2
#else
#define _INTPTR 0
#endif
#endif

#include <stdint.h>
#endif

typedef int8_t Int8;
typedef uint8_t Word8;
typedef int16_t Int16;
typedef uint16_t Word16;
typedef int32_t Int32;
typedef uint32_t Word32;
typedef int64_t Int64;
typedef uint64_t Word64;
typedef unsigned int uint_t;
typedef signed int int_t;

typedef uint8_t Bool;
typedef int32_t Frac32;
typedef int32_t Fixed32;
typedef unsigned int Word;
typedef signed int Int;

// 128 bit types and other intrinsics

#if defined(BURGER_PS2) || defined(BURGER_PSP)
typedef unsigned int Vector_128 __attribute__((mode(TI)));

#elif defined(BURGER_PS3)
#include <altivec.h>
#include <ppu_intrinsics.h>
#ifndef __VEC_TYPES_H__
#include <vec_types.h>
#endif
typedef vec_float4 Vector_128;

#elif defined(BURGER_PS4)
#ifndef __XMMINTRIN_H
#include <xmmintrin.h>
#endif
typedef __m128 Vector_128;

#elif defined(BURGER_INTEL) && \
    (defined(BURGER_MSVC) || defined(BURGER_LINUX) || defined(BURGER_ANDROID))
#ifndef _INCLUDED_EMM
#include <emmintrin.h>
#endif
typedef __m128 Vector_128;

#elif defined(BURGER_XBOX360)
#ifndef __PPCINTRINSICS_H__
#include <ppcintrinsics.h>
#endif
#ifndef __VECTORINTRINSICS_H__
#include <vectorintrinsics.h>
#endif
typedef __vector4 Vector_128;

#elif defined(BURGER_METROWERKS) && defined(BURGER_X86)
#ifndef _XMMINTRIN_H
#include <xmmintrin.h>
#endif
typedef __m128 Vector_128;

#elif (defined(BURGER_MACOSX) || defined(BURGER_IOS)) && defined(BURGER_INTEL)
#ifndef __EMMINTRIN_H
#include <emmintrin.h>
#endif
typedef __m128 Vector_128;

#elif defined(BURGER_ANDROID)
#ifndef __ARM_NEON_H
#include <arm_neon.h>
#endif
typedef float32x4_t Vector_128;

#elif defined(BURGER_WIIU)
#include <ppc_ghs.h>
struct Vector_128 {
    float BURGER_ALIGN(
        m128_f32[4], 16); ///< Opaque contents to the 128 bit vector register
};

#elif defined(BURGER_MINGW)
#ifndef _XMMINTRIN_H_INCLUDED
#include <xmmintrin.h>
#endif
typedef __m128 Vector_128;

#else

#if defined(BURGER_WATCOM)
#ifndef _MMINTRIN_H_INCLUDED
#include <mmintrin.h>
#endif
#endif

struct Vector_128 {
    float BURGER_ALIGN(
        m128_f32[4], 16); ///< Opaque contents to the 128 bit vector register
};
#endif

// Pointer types cast to an integer

typedef uintptr_t WordPtr;
typedef intptr_t IntPtr;
#define BURGER_MAXWORDPTR UINTPTR_MAX
#define BURGER_MAXINTPTR INTPTR_MAX

// Array size

#if defined(BURGER_WATCOM)
#define BURGER_ARRAYSIZE(x) static_cast<uintptr_t>(sizeof(x) / sizeof(x[0]))
#else
template<typename T, uintptr_t N>
const char (*_BurgerArraySize(T (&)[N]))[N];

#define BURGER_ARRAYSIZE(x) static_cast<uintptr_t>(sizeof(*_BurgerArraySize(x)))
#endif

// new override support
#if !defined(BURGER_NONEW)

#if defined(BURGER_METROWERKS)
#ifndef _NEW
#define _NEW
BURGER_INLINE void* operator new(__typeof__(sizeof(0)), void* x)
{
    return x;
}
#endif

#elif defined(BURGER_MSVC)
#define __PLACEMENT_NEW_INLINE
BURGER_INLINE void* BURGER_ANSIAPI operator new(size_t, void* x)
{
    return x;
}

#elif defined(__DJGPP__) || defined(BURGER_INTEL_COMPILER) || \
    (defined(__GNUC__) && defined(__MACH__) && \
        (defined(__APPLE_CPP__) || defined(__APPLE_CC__) || \
            defined(__NEXT_CPP__)))
BURGER_INLINE void* operator new(unsigned long int, void* x)
{
    return x;
}

#elif defined(BURGER_PS4) || \
    (defined(BURGER_ANDROID) && defined(BURGER_64BITCPU))
BURGER_INLINE void* operator new(unsigned long, void* x)
{
    return x;
}

#elif defined(BURGER_ANDROID) || defined(BURGER_SNSYSTEMS) || \
    defined(BURGER_GHS) || defined(BURGER_ARM_COMPILER)
BURGER_INLINE void* operator new(unsigned int, void* x)
{
    return x;
}

#else
BURGER_INLINE void* operator new(WordPtr, void* x)
{
    return x;
}
#endif
#endif

// Simple global templates

namespace Burger {

// Swap the contents of two memory variables
template<class T>
BURGER_INLINE void SwapVariables(T* pA, T* pB)
{
    T tTemp(*pA);
    *pA = *pB;
    *pB = tTemp;
}

// Global error types

BURGER_ENUMSTART(eError, Word)
{/** No error (Always zero) */
    None,
    /** General error */
    Generic = 0xF000,
    /** Input parameter is invalid */
    InvalidParameter,
    /** Class or subsystem not initialized */
    NotInitialized,
    /** Function not supported on this platform */
    NotSupportOnThisPlatform,
    /** Not enough memory to perform operation */
    OutOfMemory,
    /** Data isn't valid or memory overrun detected */
    DataCorruption,
    /** Data wasn't aligned on a proper byte boundary */
    BadAlignment,
    /** File not found */
    FileNotFound,
    /** Directory to file not found */
    PathNotFound,
    /** Write access was not granted */
    WriteProtected,
    /** No more file descriptors are available */
    NoMoreFileDescriptors,
    /** System file table overflow */
    FileTableOverflow,
    /** Files are present in the directory */
    DirectoryNotEmpty,
    /** Operation attempted on an object that wasn't a file */
    NotAFile,
    /** Operation attempted on an object that wasn't a directory */
    NotADirectory,
    /** File already exists */
    FileExists,
    /** Failed on reading data */
    ReadFailure,
    /** Failed on opening */
    OpenFailure,
    /** Failed on writing data */
    WriteFailure,
    /** File or stream seek operation failed */
    SeekFailure,
    /** Permission not granted to perform operation */
    AccessDenied,
    /** No data remaining */
    NoData,
    /** Buffer not large enough for operation */
    BufferTooSmall,
    /** Buffer is too large for this operation */
    BufferTooLarge,
    /** Operation timed out */
    Timeout,
    /** Data resource not found */
    ResourceNotFound,
    /** Version not supported */
    VersionMismatch,
    /** Fixed array is out of entries */
    OutOfEntries,
    /** Index is beyond the allowed range */
    OutOfBounds,
    /** Item searched for was not found */
    ItemNotFound,
    /** Failed on accepting a connection */
    AcceptFailure,
    /** Not ready yet */
    NotReady,
    /** Enumeration operation is in progress */
    EnumerationInProgress,
    /** Enumeration operation isn't running */
    NotEnumerating,
    /** Network address not found */
    AddressNotFound,
    /** Network socket failure */
    SocketFailure,
    /** Network flow interrupted (Or blocked) */
    FlowInterrupted,
    /** Color depth requested is not available on current hardware */
    ColorDepthNotSupported,
    /** Requested display resolution is not available on current hardware */
    ResolutionNotSupported,
    /** Audio driver failed */
    AudioDriverFailure,
    /** Data required for stream was not available in time */
    DataStarvation} BURGER_ENUMEND(eError)

}
/* END */

#endif
