/***************************************

    Determine which compiler is being used and create standardized typedefs
    and macros so generic code can be created cross platform

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

/***************************************

    Generic simple macros

***************************************/

#define BURGER_STRINGIZE2(x) #x
#define BURGER_STRINGIZE(x) BURGER_STRINGIZE2(x)

#define BURGER_JOIN3(x, y) x##y
#define BURGER_JOIN2(x, y) BURGER_JOIN3(x, y)
#define BURGER_JOIN(x, y) BURGER_JOIN2(x, y)

#define BURGER_LEFT_PARENTHESIS (
#define BURGER_RIGHT_PARENTHESIS )
#define BURGER_NULL_MACRO_PARAM /**/

#if !defined(NULL)
#define NULL 0
#endif

#if !defined(TRUE)
#define TRUE 1
#endif

#if !defined(FALSE)
#define FALSE 0
#endif

/***************************************

    Test for the flavor of C++, assume it's at least C++89

***************************************/

#define BURGER_CPP89 __cplusplus

// Try C++98
#if (__cplusplus >= 199711L) || defined(DOXYGEN)
#define BURGER_CPP98 __cplusplus
#endif

// At C++11 or later, things get odd.
#if (__cplusplus >= 201103L) || defined(DOXYGEN)
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
#if (__cplusplus >= 201402L) || defined(DOXYGEN)
#define BURGER_CPP14 __cplusplus
#elif (defined(_MSVC_LANG) && (_MSVC_LANG >= 201402L))
#define BURGER_CPP14 _MSVC_LANG
#endif

// Test for C++17
#if (__cplusplus >= 201700L) || defined(DOXYGEN)
#define BURGER_CPP17 __cplusplus
#elif (defined(_MSVC_LANG) && (_MSVC_LANG >= 201700L))
#define BURGER_CPP17 _MSVC_LANG
#endif

// To C++Infinity and beyond
#if (__cplusplus > 201703L) || defined(DOXYGEN)
#define BURGER_CPP20 __cplusplus
#elif (defined(_MSVC_LANG) && (_MSVC_LANG > 201703L))
#define BURGER_CPP20 _MSVC_LANG
#endif

// Create the flavor of C++ as a string
#if defined(BURGER_CPP20)
#define BURGER_STDCPP_NAME "C++20"
#elif defined(BURGER_CPP17)
#define BURGER_STDCPP_NAME "C++17"
#elif defined(BURGER_CPP14)
#define BURGER_STDCPP_NAME "C++14"
#elif defined(BURGER_CPP11)
#define BURGER_STDCPP_NAME "C++11"
#elif defined(BURGER_CPP98)
#define BURGER_STDCPP_NAME "C++98"
#else
#define BURGER_STDCPP_NAME "C++89"
#endif

/***************************************

    Get the compiler's name and version

***************************************/

#define BURGER_HAS_64_BIT_SUPPORT
#define BURGER_HAS_SFINAE

#if defined(__WATCOMC__)
#define BURGER_COMPILER_NAME "Open Watcom Compiler"
#define BURGER_WATCOM __WATCOMC__
#define BURGER_COMPILER_VERSION BURGER_WATCOM

#elif defined(__MRC__)
#define BURGER_COMPILER_NAME "Apple PowerPC MrCPP Compiler"
#define BURGER_MRC __MRC__
#define BURGER_COMPILER_VERSION BURGER_MRC

#if __MRC__ < 0x500
#error MPW MrCpp 5.0 or better compiler required
#endif

#if !__option(longlong)
#undef BURGER_HAS_64_BIT_SUPPORT
#endif

#if !__option(bool)
#error bool required, use '-bool on'
#endif

#undef BURGER_HAS_SFINAE

#elif defined(__SC__)
#define BURGER_COMPILER_NAME "Symantec C++ Compiler"
#define BURGER_APPLE_SC __SC__
#define BURGER_COMPILER_VERSION BURGER_APPLE_SC

#if __SC__ < 0x890
#error MPW SCpp 8.9 or better compiler required
#endif

#if !__option(bool)
#error bool required, use '-bool on'
#endif
#undef BURGER_HAS_64_BIT_SUPPORT
#undef BURGER_HAS_SFINAE

#elif defined(__DJGPP__)
#define BURGER_COMPILER_NAME "DJ's GNU Programming Platform"
#define BURGER_DJGPP ((__DJGPP__ * 100) + __DJGPP_MINOR__)
#define BURGER_COMPILER_VERSION BURGER_DJGPP

#elif defined(__SN__) || defined(__SNC__)
#define BURGER_COMPILER_NAME "SN Systems Compiler"
#define BURGER_SNSYSTEMS __SN_VER__
#define BURGER_COMPILER_VERSION BURGER_SNSYSTEMS

#elif defined(__ghs__)
#define BURGER_COMPILER_NAME "Green Hills Compiler"
#define BURGER_GHS __GHS_VERSION_NUMBER
#define BURGER_COMPILER_VERSION BURGER_GHS

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
#define BURGER_METROWERKS __MWERKS__
#define BURGER_COMPILER_VERSION BURGER_METROWERKS

#if !__option(bool)
#error bool required, check the "Enable bool support" box
#endif

#elif defined(__CC_ARM)
#define BURGER_COMPILER_NAME "ARM RealView C++ Compiler"
#define BURGER_ARM_COMPILER __ARMCC_VERSION
#define BURGER_COMPILER_VERSION BURGER_ARM_COMPILER

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

#elif !defined(DOXYGEN)
#define BURGER_COMPILER_NAME "Unknown Compiler"
#error Unknown compiler

#endif

// Disable ``noexcept`` used with no exception handling mode specified
#if (BURGER_MSVC >= 190000000)
#pragma warning(disable : 4577)
#endif

#if defined(BURGER_WATCOM)
// Conditional expression is always true
#pragma warning 367 9
// Conditional expression is always false
#pragma warning 368 9
// Conditional expression is always false (zero)
#pragma warning 690 9
#endif

/***************************************

    Feature detection macros, using the clang paradigm.

***************************************/

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

#if !defined(__has_attribute)
#define __has_attribute(x) 0
#endif

#if !defined(__has_warning)
#define __has_warning(x) 0
#endif

#if !defined(__has_include)
#define __has_include(x) 0
#endif

#if !defined(__has_include_next)
#define __has_include_next(x) 0
#endif

/***************************************

    Detect the CPU being compiled for

***************************************/

#if defined(__386__) || defined(_M_IX86) || defined(__i386__) || \
    defined(__X86__) || defined(_X86_) || defined(__INTEL__)
#define BURGER_X86
#define BURGER_CPU_NAME "Intel 32 bit 386"

#elif defined(_M_AMD64) || defined(_M_X64) || defined(__x86_64__) || \
    defined(__amd64__)
#define BURGER_AMD64
#define BURGER_CPU_NAME "AMD64 64 bit CPU"

#elif defined(_IA64_) || defined(__ia64__) || defined(_M_IA64) || \
    defined(__itanium__)
#define BURGER_ITANIUM
#define BURGER_CPU_NAME "Intel Itanium 64 bit"

#elif defined(_M_ARM) || defined(__arm__) || defined(__arm) || \
    defined(_ARM) || defined(__TARGET_ARCH_ARM)
#define BURGER_ARM32
#define BURGER_CPU_NAME "ARM 32 bit"

#elif defined(_M_ARM64) || defined(__arm64__) || defined(__aarch64__)
#define BURGER_ARM64
#define BURGER_CPU_NAME "ARM 64 bit"

#elif defined(__mips__) || defined(__R5900) || defined(__R5900__) || \
    defined(__MIPS__)
#if (_MIPS_SIM == _ABIO32) || (_MIPS_SIM == _ABIN32) || defined(__R5900) || \
    defined(__R5900__) || defined(__MIPS__)
#define BURGER_MIPS32
#define BURGER_CPU_NAME "Mips 32 bit"
#elif _MIPS_SIM == _ABI64
#define BURGER_MIPS64
#define BURGER_CPU_NAME "Mips 64 bit"
#else
#error Unknown MIPS CPU
#endif

#elif (defined(_XBOX) && defined(_M_PPCBE)) || defined(__ppc64__) || \
    defined(__powerpc64__) || defined(_ARCH_PPC64)
#define BURGER_POWERPC64
#define BURGER_CPU_NAME "PowerPC 64 bit"

#elif defined(__POWERPC__) || defined(__ppc__) || defined(__powerpc) || \
    defined(_MPPC_) || defined(_M_PPC) || defined(__PPCGECKO__) || \
    defined(__PPCBROADWAY__)
#define BURGER_POWERPC
#define BURGER_CPU_NAME "PowerPC 32 bit"

#elif defined(__MC68K__) || defined(_68K_) || defined(__m68k__) || \
    defined(mc68000)
#define BURGER_68K
#define BURGER_CPU_NAME "Motorola 68000"

#elif defined(__riscv)
#define BURGER_RISCV
#define BURGER_CPU_NAME "RISC-V"

#elif defined(__AVR_ARCH__)
#define BURGER_AVR
#define BURGER_CPU_NAME "Atmel AVR"

#elif defined(__MSP430__)
#define BURGER_MSP430
#define BURGER_CPU_NAME "Texas Instruments MSP430"

#elif defined(__sparc64__)
#define BURGER_SPARC64
#define BURGER_CPU_NAME "Sun SPARC 64 bit"

#elif defined(__sparc__) || defined(__sparc)
#define BURGER_SPARC32
#define BURGER_CPU_NAME "Sun SPARC 32 bit"

#elif !defined(DOXYGEN)
#error Unknown CPU
#define BURGER_CPU_NAME "Unknown CPU"
#endif

/***************************************

    Generic CPU class macros

***************************************/

// Intel x86 or AMD 64 CPU?
#if defined(BURGER_X86) || defined(BURGER_AMD64) || defined(DOXYGEN)
#define BURGER_INTEL
#endif

// PowerPC cpus
#if defined(BURGER_POWERPC) || defined(BURGER_POWERPC64) || defined(DOXYGEN)
#define BURGER_PPC
#endif

// ARM CPUs
#if defined(BURGER_ARM32) || defined(BURGER_ARM64) || defined(DOXYGEN)
#define BURGER_ARM
#endif

// MIPS CPUs
#if defined(BURGER_MIPS32) || defined(BURGER_MIPS64) || defined(DOXYGEN)
#define BURGER_MIPS
#endif

// SPARC CPUs
#if defined(BURGER_SPARC32) || defined(BURGER_SPARC64) || defined(DOXYGEN)
#define BURGER_SPARC
#endif

// 64 bit support?
#if defined(BURGER_ARM64) || defined(BURGER_POWERPC64) || \
    defined(BURGER_AMD64) || defined(BURGER_MIPS64) || \
    defined(BURGER_SPARC64) || defined(BURGER_ITANIUM) || defined(DOXYGEN)
#define BURGER_64BITCPU
#endif

/***************************************

    Special CPU extensions

***************************************/

#if defined(__ARM_NEON__) || defined(DOXYGEN)
#define BURGER_NEON
#endif

#if defined(mc68881) || defined(__MC68881__) || defined(DOXYGEN)
#define BURGER_68881
#endif

#if defined(__ALTIVEC__) || defined(DOXYGEN)
#define BURGER_ALTIVEC
#endif

#if defined(BURGER_METROWERKS)
#if defined(BURGER_INTEL) && __option(k63d)
#define BURGER_3DNOW
#endif
#endif

#if defined(BURGER_AMD64) || (defined(BURGER_MSVC) && (_M_IX86_FP >= 1)) || \
    defined(__SSE__) || defined(DOXYGEN)
#define BURGER_SSE
#elif defined(BURGER_METROWERKS)
#if defined(BURGER_INTEL) && __option(sse)
#define BURGER_SSE
#endif
#endif

#if defined(BURGER_AMD64) || (defined(BURGER_MSVC) && (_M_IX86_FP >= 2)) || \
    defined(__SSE2__) || defined(DOXYGEN)
#define BURGER_SSE2
#elif defined(BURGER_METROWERKS)
#if defined(BURGER_INTEL) && __option(sse2)
#define BURGER_SSE2
#endif
#endif

#if defined(__AVX__) || defined(DOXYGEN)
#define BURGER_AVX
#endif

#if defined(__AVX2__) || defined(DOXYGEN)
#define BURGER_AVX2
#endif

/***************************************

    Determine the operating system

***************************************/

// Microsoft XBox
#if defined(_XBOX) || defined(XBOX)
#if defined(BURGER_X86)
#define BURGER_XBOX
#define BURGER_PLATFORM_NAME "Microsoft XBox Classic"
#elif defined(BURGER_POWERPC64)
#define BURGER_XBOX360
#define BURGER_PLATFORM_NAME "Microsoft XBox 360"
#else
#define BURGER_XBOXONE
#define BURGER_PLATFORM_NAME "Microsoft XBox ONE"
#endif

#elif defined(__DOS__) || defined(__MSDOS__) || defined(_MSDOS)
#define BURGER_MSDOS
#if defined(__X32__)
#define BURGER_X32
#define BURGER_PLATFORM_NAME "Microsoft DOS X32"
#elif defined(__DOS4G__) || defined(__DOS4GW__) || defined(__GO32__)
#define BURGER_DOS4G
#define BURGER_PLATFORM_NAME "Microsoft DOS DOS4G"
#else
#error Unknown MSDOS extender, try using Dos4g
#endif

#elif defined(__NT__) || defined(__WIN32__) || defined(__WINDOWS__) || \
    defined(__TOS_WIN__) || defined(_WIN32) || defined(_WIN64)
#if defined(BURGER_64BITCPU)
#define BURGER_WIN64
#define BURGER_PLATFORM_NAME "Microsoft Windows 64 bit"
#else
#define BURGER_WIN32
#define BURGER_PLATFORM_NAME "Microsoft Windows 32 bit"
#endif

// Sony platforms
#elif defined(SN_TARGET_PS2)
#define BURGER_PS2
#define BURGER_PLATFORM_NAME "Sony Playstation 2"

#elif defined(__CELLOS_LV2__)
#define BURGER_PS3
#define BURGER_PLATFORM_NAME "Sony Playstation 3"

#elif defined(__ORBIS__)
#define BURGER_PS4
#define BURGER_PLATFORM_NAME "Sony Playstation 4"

#elif defined(SN_TARGET_PSP) || defined(SN_TARGET_PSP_PRX)
#define BURGER_PSP
#define BURGER_PLATFORM_NAME "Sony Playstation Portable"

#elif defined(__psp2__)
#define BURGER_VITA
#define BURGER_PLATFORM_NAME "Sony Playstation Vita"

// Android
#elif defined(__ANDROID__) || defined(__ANDROID_API__)
#define BURGER_ANDROID
#define BURGER_PLATFORM_NAME "Google Android"
#define BURGER_SHIELD

// Nintendo platforms
#elif defined(SDK_ARM9) && defined(SDK_TS)
#define BURGER_DS
#define BURGER_PLATFORM_NAME "Nintendo DS"

#elif defined(NN_COMPILER_RVCT) || defined(NN_PLATFORM_CTR)
#define BURGER_3DS
#define BURGER_PLATFORM_NAME "Nintendo 3DS"

#elif defined(__PPCGECKO__)
#define BURGER_GAMECUBE
#define BURGER_PLATFORM_NAME "Nintendo Gamecube"

#elif defined(RVL) || defined(RVL_SDK) || defined(RVL_OS) || \
    defined(__PPCBROADWAY__)
#define BURGER_WII
#define BURGER_PLATFORM_NAME "Nintendo Wii"

#elif defined(__espresso__)
#define BURGER_WIIU
#define BURGER_PLATFORM_NAME "Nintendo WiiU"

#elif defined(NN_NINTENDO_SDK)
#define BURGER_SWITCH
#define BURGER_PLATFORM_NAME "Nintendo Switch"

#elif defined(__BEOS__)
#define BURGER_BEOS
#define BURGER_PLATFORM_NAME "BeOS / Haiku"

#elif defined(__SYMBIAN32__)
#define BURGER_SYMBIAN
#define BURGER_PLATFORM_NAME "Nokia Symbian"

#elif defined(AMIGA) || defined(__amigaos__)
#define BURGER_AMIGA
#define BURGER_PLATFORM_NAME "Commodore Amiga"

#elif defined(macintosh) && macintosh
#define BURGER_MAC
#if defined(__CFM68K) || defined(__CFM68K__) || defined(BURGER_POWERPC)
#define BURGER_CFM
#endif
#if !defined(TARGET_API_MAC_CARBON) || (TARGET_API_MAC_CARBON == 0)
#define BURGER_MACCLASSIC
#define BURGER_PLATFORM_NAME "Apple macOS Classic"
#else
#define BURGER_MACCARBON
#define BURGER_PLATFORM_NAME "Apple macOS Carbon"
#endif

#elif defined(__APPLE__) && defined(__MACH__) && \
    defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
#define BURGER_IOS
#define BURGER_PLATFORM_NAME "Apple iOS"

#elif defined(__APPLE__) && defined(__MACH__)
#define BURGER_MACOSX
#define BURGER_PLATFORM_NAME "Apple macOS X"

#elif defined(__LINUX__) || defined(__linux__) || defined(__gnu_linux__)
#define BURGER_LINUX
#define BURGER_PLATFORM_NAME "Linux"

#elif defined(ARDUINO)
#define BURGER_ARDUINO
#define BURGER_PLATFORM_NAME "Arduino"

#elif !defined(DOXYGEN)
#error Unknown Platform
#endif

// Generic windows platform?
#if defined(BURGER_WIN32) || defined(BURGER_WIN64) || defined(DOXYGEN)
#define BURGER_WINDOWS
#endif

// Any form of MacOS
#if defined(BURGER_MAC) || defined(BURGER_MACOSX) || defined(DOXYGEN)
#define BURGER_MACOS
#endif

/***************************************

    Determine available APIs from the operating system.

***************************************/

// Is OpenGL supported on this platform?

#if (!defined(BURGER_VITA) && \
    ((defined(BURGER_WINDOWS) && defined(BURGER_INTEL)) || \
        defined(BURGER_MACOS) || defined(BURGER_IOS) || \
        defined(BURGER_ANDROID) || defined(BURGER_LINUX))) || \
    defined(DOXYGEN)
#define BURGER_OPENGL
#endif

// Is OpenGLES supported on this platform?

#if (!defined(BURGER_VITA) && \
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

#if (defined(BURGER_MACOSX) || defined(BURGER_MSDOS) || \
    defined(BURGER_LINUX) || defined(BURGER_WINDOWS)) || \
    defined(DOXYGEN)
#define BURGER_CONSOLE
#endif

/***************************************

    Determine the endian by testing the platform / CPU

***************************************/

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

/***************************************

    API macros

***************************************/

// Burgerlib's primary ABI
#if defined(BURGER_X86) && defined(BURGER_WATCOM)
#define BURGER_API __watcall
#define BURGER_ANSIAPI __watcall

#elif defined(BURGER_X86) && \
    (defined(BURGER_MSVC) || defined(BURGER_INTEL_COMPILER) || \
        defined(BURGER_MINGW) || (BURGER_METROWERKS >= 0x3200))
#define BURGER_API __fastcall
#define BURGER_ANSIAPI __cdecl
#define BURGER_FASTCALLENABLED

#elif defined(BURGER_X86) && \
    (defined(BURGER_DJGPP) || defined(BURGER_CLANG) || defined(BURGER_GNUC))
#define BURGER_API __attribute__((regparm(3)))
#define BURGER_ANSIAPI __attribute((cdecl))
#define BURGER_FASTCALLENABLED

#else
#define BURGER_API
#define BURGER_ANSIAPI
#endif

// Set the inline keyword

#if (BURGER_MSVC >= 120000000) || defined(BURGER_INTEL_COMPILER) || \
    defined(BURGER_MINGW)
#define BURGER_INLINE __forceinline

#elif defined(BURGER_WATCOM) || (BURGER_METROWERKS >= 0x3200) || \
    defined(BURGER_MSVC)
#define BURGER_INLINE __inline

#elif defined(BURGER_DJGPP) || defined(BURGER_CLANG) || \
    defined(BURGER_GNUC) || defined(BURGER_GHS) || defined(BURGER_SNSYSTEMS)
#define BURGER_INLINE __inline__ __attribute__((always_inline))

#else
#define BURGER_INLINE inline
#endif

// Set the no inline keyword
#if (BURGER_MSVC >= 130000000) || (BURGER_METROWERKS >= 0x3100) || \
    defined(BURGER_INTEL_COMPILER) || defined(BURGER_MINGW)
#define BURGER_NO_INLINE __declspec(noinline)
#elif defined(BURGER_DJGPP) || defined(BURGER_CLANG) || \
    defined(BURGER_GNUC) || defined(BURGER_GHS)
#define BURGER_NO_INLINE __attribute__((noinline))
#else
#define BURGER_NO_INLINE
#endif

// Noreturn
#if (BURGER_MSVC >= 120000000)
#define BURGER_NORETURN __declspec(noreturn)
#elif (BURGER_GNUC >= 20800) || __has_attribute(__noreturn__)
#define BURGER_NORETURN __attribute__((__noreturn__))
#elif defined(BURGER_CPP11)
#define BURGER_NORETURN [[noreturn]]
#else
#define BURGER_NORETURN
#endif

// Attribute printf format
#if (BURGER_GNUC >= 20300) || __has_attribute(format) || defined(DOXYGEN)
#define BURGER_PRINTF_ATTRIBUTE(_index, _check) \
    __attribute__((format(printf, _index, _check)))
#else
#define BURGER_PRINTF_ATTRIBUTE(_index, _check)
#endif

// Asm keyword for inline assembly (If supported)
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

/***************************************

    Alignment and packing commands

***************************************/

#if defined(BURGER_CPP11) || __has_feature(cxx_alignas) || \
    __has_extension(cxx_alignas) || \
    ((BURGER_GNUC >= 40800) && defined(__GXX_EXPERIMENTAL_CXX0X__)) || \
    defined(DOXYGEN)
#define BURGER_ALIGN(__type, __name, __a) alignas(__a) __type __name
#define BURGER_PREALIGN(__a) alignas(__a)
#define BURGER_POSTALIGN(__a)
#elif defined(BURGER_INTEL_COMPILER) || defined(BURGER_MSVC) || \
    defined(BURGER_PS4) || \
    (defined(BURGER_METROWERKS) && !defined(BURGER_68K))
#define BURGER_ALIGN(__type, __name, __a) __type __declspec(align(__a)) __name
#define BURGER_PREALIGN(__a) __declspec(align(__a))
#define BURGER_POSTALIGN(__a)
#elif defined(BURGER_GNUC) || defined(BURGER_CLANG) || \
    defined(BURGER_SNSYSTEMS) || defined(BURGER_ARM_COMPILER) || \
    defined(BURGER_GHS)
#define BURGER_ALIGN(__type, __name, __a) \
    __type __name __attribute__((aligned(__a)))
#define BURGER_PREALIGN(__a)
#define BURGER_POSTALIGN(__a) __attribute__((aligned(__a)))
#else
#define BURGER_ALIGN(__type, __name, __a) __type __name
#define BURGER_PREALIGN(__a)
#define BURGER_POSTALIGN(__a)
#define BURGER_NO_ALIGN
#endif

// Structure packing macro switches
#if defined(BURGER_MRC) || defined(BURGER_APPLE_SC) || \
    (defined(BURGER_METROWERKS) && \
        (defined(BURGER_PPC) || defined(BURGER_68K)))
#define BURGER_STRUCT_ALIGN
#elif defined(BURGER_PS2) || defined(BURGER_DS)
#define BURGER_STRUCT_PACK
#else
#define BURGER_STRUCT_PACKPUSH
#endif

/***************************************

    C++11 or higher features

***************************************/

// Test for nullptr support

#if defined(BURGER_CPP11) || __has_feature(cxx_nullptr) || \
    ((BURGER_GNUC >= 40600) && \
        (defined(__GXX_EXPERIMENTAL_CXX0X__) && \
            __GXX_EXPERIMENTAL_CXX0X__)) || \
    (BURGER_MSVC >= 160000000)
#else
#define nullptr 0
#endif

// Test for __underlying_type() support

#if defined(BURGER_CPP11) || __has_feature(underlying_type) || \
    (BURGER_MSVC >= 170000000) || (BURGER_GNUC >= 40700)
#else
#define __underlying_type(x) int
#endif

// Test for static_assert

#if defined(BURGER_CPP11) || __has_feature(cxx_static_assert) || \
    defined(__cpp_static_assert) || \
    ((BURGER_GNUC >= 40300) && defined(__GXX_EXPERIMENTAL_CXX0X__)) || \
    (BURGER_MSVC >= 160000000) || defined(DOXYGEN)
#define BURGER_STATIC_ASSERT(x) static_assert(x, #x)
#else
#define BURGER_STATIC_ASSERT(x) \
    typedef int ThisIsABogusTypeDef##__LINE__[(x) ? 1 : -1]
#endif

// Test for constexpr support

#if defined(BURGER_CPP11) || __has_feature(cxx_constexpr) || \
    defined(__cpp_constexpr) || \
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

// Test for noexcept support

#if defined(BURGER_CPP11) || __has_feature(cxx_noexcept) || \
    ((BURGER_GNUC >= 40600) && defined(__GXX_EXPERIMENTAL_CXX0X__)) || \
    (BURGER_MSVC >= 190023026) || defined(DOXYGEN)
#define BURGER_NOEXCEPT noexcept
#else
#define BURGER_NOEXCEPT
#endif

// Test for override support

#if defined(BURGER_CPP11) || __has_feature(cxx_override_control) || \
    (BURGER_CLANG >= 20900) || (BURGER_GNUC >= 40700) || \
    (BURGER_MSVC >= 140000000) || defined(DOXYGEN)
#define BURGER_OVERRIDE override
#if (BURGER_MSVC >= 140000000) && (BURGER_MSVC < 170000000)
// Disable warning for override in VS 2005-2010
#pragma warning(disable : 4481)
#endif
#else
#define BURGER_OVERRIDE
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

// Test for "= delete" support

#if defined(BURGER_CPP11) || __has_feature(cxx_deleted_functions) || \
    ((BURGER_GNUC >= 40400) && defined(__GXX_EXPERIMENTAL_CXX0X__)) || \
    defined(DOXYGEN)
#define BURGER_EQUALS_DELETE = delete
#define BURGER_EQUALS_DEFAULT = default
#define BURGER_DEFAULT_DESTRUCTOR = default
#else
#define BURGER_EQUALS_DELETE
#define BURGER_EQUALS_DEFAULT
#define BURGER_DEFAULT_DESTRUCTOR \
    { \
    }
#endif

// Test for && rvalue support
#if defined(BURGER_CPP11) || __has_feature(cxx_rvalue_references) || \
    ((BURGER_GNUC >= 40300) && defined(__GXX_EXPERIMENTAL_CXX0X__)) || \
    (BURGER_MSVC >= 160000000) || defined(DOXYGEN)
#define BURGER_RVALUE_REFERENCES
#endif

// Test for address sanitization
#if __has_feature(address_sanitizer) || (BURGER_GNUC >= 40800) || \
    defined(DOXYGEN)
#define BURGER_ADDRESS_SANITIZER
#define BURGER_DISABLE_ASAN __attribute__((no_sanitize_address))
#else
#define BURGER_DISABLE_ASAN
#endif

// Test for memory sanitization
#if __has_feature(memory_sanitizer) || defined(DOXYGEN)
#define BURGER_MEMORY_SANITIZER
#define BURGER_DISABLE_MSAN __attribute__((no_sanitize("memory")))
#else
#define BURGER_DISABLE_MSAN
#endif

#if defined(BURGER_CPP17) || __has_cpp_attribute(maybe_unused) || \
    defined(DOXYGEN)
#define BURGER_MAYBE_UNUSED [[maybe_unused]]
#elif __has_cpp_attribute(gnu::unused)
#define BURGER_MAYBE_UNUSED [[gnu::unused]]
#elif __has_cpp_attribute(unused) && defined(BURGER_CLANG)
#define BURGER_MAYBE_UNUSED __attribute__((unused))
#else
#define BURGER_MAYBE_UNUSED
#endif

#if defined(BURGER_CPP17) || __has_cpp_attribute(nodiscard) || defined(DOXYGEN)
#define BURGER_NODISCARD [[nodiscard]]
#elif __has_cpp_attribute(clang::warn_unused_result)
#define BURGER_NODISCARD [[clang::warn_unused_result]]
#elif __has_cpp_attribute(gnu::warn_unused_result)
#define BURGER_NODISCARD [[gnu::warn_unused_result]]
#elif __has_cpp_attribute(warn_unused_result) && defined(BURGER_CLANG)
#define BURGER_NODISCARD __attribute__((warn_unused_result))
#else
#define BURGER_NODISCARD
#endif

#if defined(BURGER_CPP17) || __has_cpp_attribute(fallthrough) || \
    defined(DOXYGEN)
#define BURGER_FALLTHROUGH [[fallthrough]]
#elif __has_cpp_attribute(clang::fallthrough)
#define BURGER_FALLTHROUGH [[clang::fallthrough]]
#elif __has_cpp_attribute(gnu::fallthrough)
#define BURGER_FALLTHROUGH [[gnu::fallthrough]]
#elif (BURGER_GNUC >= 70000)
#define BURGER_FALLTHROUGH __attribute__((fallthrough))
#else
#define BURGER_FALLTHROUGH ((void)0)
#endif

#if __has_cpp_attribute(used) || __has_attribute(used) || (BURGER_GNUC >= 30100)
#define BURGER_USED __attribute__((used))
#elif __has_attribute(__used__)
#define BURGER_USED __attribute__((__used__))
#else
#define BURGER_USED
#endif

/***************************************

    Helper macros

***************************************/

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

#define BURGER_OFFSETOF(__type, __member) \
    (reinterpret_cast<intptr_t>( \
         &reinterpret_cast<const __type*>(1)->__member) - \
        1)
#define BURGER_GET_BASE_PTR(x, __type, __member) \
    reinterpret_cast<__type*>( \
        reinterpret_cast<uint8_t*>(x) - BURGER_OFFSETOF(__type, __member))
#define BURGER_CONST_GET_BASE_PTR(x, __type, __member) \
    reinterpret_cast<const __type*>(reinterpret_cast<const uint8_t*>(x) - \
        BURGER_OFFSETOF(__type, __member))

// Unused parameters or variables
#define BURGER_UNUSED(x) (void)(x)

// Macro to create copy constructors to disable the feature
#if defined(BURGER_RVALUE_REFERENCES) || defined(DOXYGEN)
#define BURGER_DISABLE_COPY(x) \
private: \
    x(const x&) BURGER_EQUALS_DELETE; \
    x& operator=(const x&) BURGER_EQUALS_DELETE; \
    x(x&&) BURGER_EQUALS_DELETE; \
    x& operator=(x&&) BURGER_EQUALS_DELETE
#else
#define BURGER_DISABLE_COPY(x) \
private: \
    x(const x&) BURGER_EQUALS_DELETE; \
    x& operator=(const x&) BURGER_EQUALS_DELETE
#endif

// clang-format off
// Use enum class when available
#if defined(BURGER_CPP11) || __has_feature(cxx_strong_enums) || \
    __has_extension(cxx_strong_enums) || __has_feature(objc_fixed_enum) || \
    (BURGER_MSVC >= 170000000) || defined(DOXYGEN)
#define BURGER_ENUMSTART(x, y) enum class x : y
#define BURGER_ENUMEND(x) ; typedef x x##Type;

#else
#define BURGER_ENUMSTART(x, y) struct x { enum Type
#define BURGER_ENUMEND(x) ; }; typedef x::Type x##Type;
#endif

#define BURGER_ENUMFLAGSEND(x, y) \
    BURGER_ENUMEND(x) \
    BURGER_INLINE x##Type& operator|=(x##Type& uInput1, x##Type uInput2) { \
        return uInput1 = static_cast<x##Type>(static_cast<y>(uInput1) | static_cast<y>(uInput2)); } \
    BURGER_INLINE x##Type& operator&=(x##Type& uInput1, x##Type uInput2) { \
        return uInput1 = static_cast<x##Type>(static_cast<y>(uInput1) & static_cast<y>(uInput2)); } \
    BURGER_INLINE x##Type& operator^=(x##Type& uInput1, x##Type uInput2) { \
        return uInput1 = static_cast<x##Type>(static_cast<y>(uInput1) ^ static_cast<y>(uInput2)); } \
    BURGER_INLINE BURGER_CONSTEXPR x##Type operator|(x##Type uInput1, x##Type uInput2) { \
        return static_cast<x##Type>(static_cast<y>(uInput1) | static_cast<y>(uInput2)); } \
    BURGER_INLINE BURGER_CONSTEXPR x##Type operator&(x##Type uInput1, x##Type uInput2) { \
        return static_cast<x##Type>(static_cast<y>(uInput1) & static_cast<y>(uInput2)); } \
    BURGER_INLINE BURGER_CONSTEXPR x##Type operator^(x##Type uInput1, x##Type uInput2) { \
        return static_cast<x##Type>(static_cast<y>(uInput1) ^ static_cast<y>(uInput2)); } \
    BURGER_INLINE BURGER_CONSTEXPR uint_t operator!(x##Type uInput) { \
        return !static_cast<y>(uInput); } \
    BURGER_INLINE constexpr x##Type operator~(x##Type uInput) { \
        return static_cast<x##Type>(~static_cast<y>(uInput)); }
// clang-format on

/***************************************

    Data types

***************************************/

// Special case for 16 bit int

#if defined(BURGER_68K) && defined(BURGER_METROWERKS)
#if !__option(fourbyteints)
#define BURGER_SIZEOF_INT 2
#endif
#endif

#if !defined(BURGER_SIZEOF_INT) || defined(DOXYGEN)
#define BURGER_SIZEOF_INT 4
#endif

// Special case for 64 bit long

#if !defined(BURGER_HAS_64_BIT_SUPPORT) || defined(DOXYGEN)
// 64 bit integer support is performed by a 64 bit container structure
namespace Burger {
struct longlong_t;
struct ulonglong_t;
}
#define BURGER_LONGLONG Burger::longlong_t
#define BURGER_ULONGLONG Burger::ulonglong_t

#elif defined(BURGER_INTEL_COMPILER) || defined(BURGER_MSVC)
#define BURGER_LONGLONG __int64
#define BURGER_ULONGLONG unsigned __int64

#elif defined(BURGER_PS2) || defined(__LP64__)
#define BURGER_LONGLONG long
#define BURGER_ULONGLONG unsigned long
#define BURGER_SIZEOF_LONG 8

#else
#define BURGER_LONGLONG long long
#define BURGER_ULONGLONG unsigned long long
#endif

#if !defined(BURGER_SIZEOF_LONG) || defined(DOXYGEN)
#define BURGER_SIZEOF_LONG 4
#endif

// stdint.h types

#if defined(BURGER_MRC) || defined(BURGER_APPLE_SC) || \
    (defined(BURGER_MSVC) && (BURGER_MSVC < 160000000))
#include <brstdint.h>

#if (BURGER_SIZEOF_INT != 4)
#define BURGER_LONG_NOT_IN_STDINT
#endif

#if (BURGER_SIZEOF_LONG == 4)
#define BURGER_LONG_NOT_IN_STDINT
#endif

#else

// PS3 requires these
#if !defined(__STDC_CONSTANT_MACROS) && !defined(DOXYGEN)
#define __STDC_CONSTANT_MACROS 1
#endif
#if !defined(__STDC_LIMIT_MACROS) && !defined(DOXYGEN)
#define __STDC_LIMIT_MACROS 1
#endif

// Visual Studio 2010 has a bug where _INTPTR is not defined
#if !defined(_INTPTR) && defined(BURGER_MSVC) && (BURGER_MSVC < 170000000)
#if defined(BURGER_64BITCPU) && !defined(BURGER_XBOX360)
#define _INTPTR 2
#else
#define _INTPTR 0
#endif
#endif

#include <stdint.h>

#if defined(BURGER_METROWERKS) || (defined(BURGER_WATCOM) && defined(_M_I86))
#define BURGER_INT_NOT_IN_STDINT
#endif

#if !defined(BURGER_METROWERKS) && \
    ((BURGER_SIZEOF_LONG == 4) || defined(BURGER_MACOSX) || \
        defined(BURGER_IOS))
#define BURGER_LONG_NOT_IN_STDINT
#endif

#endif

// Helper types to convert native int to uint??_t
#if (BURGER_SIZEOF_INT == 4)
typedef int32_t int2int_t;
typedef uint32_t uint2uint_t;
#else
typedef int16_t int2int_t;
typedef uint16_t uint2uint_t;
#endif

#if (BURGER_SIZEOF_LONG == 4)
typedef int32_t long2int_t;
typedef uint32_t ulong2uint_t;
#else
typedef int64_t long2int_t;
typedef uint64_t ulong2uint_t;
#endif

// Are wchar_t, char8_t, char16_t and char32_t native types?

#if defined(BURGER_INTEL_COMPILER) || defined(BURGER_CLANG) || \
    defined(BURGER_GNUC) || defined(_NATIVE_WCHAR_T_DEFINED) || \
    defined(BURGER_MINGW) || defined(DOXYGEN)
#define BURGER_HAS_WCHAR_T
#elif defined(BURGER_METROWERKS)
#if __option(wchar_type)
#define BURGER_HAS_WCHAR_T
#endif
#endif

#if defined(__cpp_char8_t) || defined(DOXYGEN)
#define BURGER_HAS_CHAR8_T
#endif

#if defined(BURGER_CPP11) || (__cpp_unicode_characters >= 200704) || \
    (_HAS_CHAR16_T_LANGUAGE_SUPPORT > 0) || \
    ((BURGER_GNUC >= 40400) && defined(__GXX_EXPERIMENTAL_CXX0X__)) || \
    defined(DOXYGEN)
#define BURGER_HAS_CHAR16_T
#endif

// 64 bit types for 32 bit compilers
namespace Burger {
struct ulonglong_t;
struct longlong_t {
#if defined(BURGER_BIGENDIAN)
    int32_t hi;  ///< High 32 bits of the 64 bit integer
    uint32_t lo; ///< Low 32 bits of the 64 bit integer
#else
    uint32_t lo; ///< Low 32 bits of the 64 bit integer
    int32_t hi;  ///< High 32 bits of the 64 bit integer
#endif
    BURGER_INLINE operator ulonglong_t&() BURGER_NOEXCEPT
    {
        return *reinterpret_cast<ulonglong_t*>(this);
    }
};

struct ulonglong_t {
#if defined(BURGER_BIGENDIAN)
    uint32_t hi; ///< High 32 bits of the 64 bit integer
    uint32_t lo; ///< Low 32 bits of the 64 bit integer
#else
    uint32_t lo; ///< Low 32 bits of the 64 bit integer
    uint32_t hi; ///< High 32 bits of the 64 bit integer
#endif
    BURGER_INLINE operator longlong_t&() BURGER_NOEXCEPT
    {
        return *reinterpret_cast<longlong_t*>(this);
    }
};
}

/***************************************

    Vector data types and vector intrinsics

***************************************/

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
    (defined(BURGER_MSVC) || defined(BURGER_INTEL_COMPILER) || \
        defined(BURGER_GNUC) || defined(BURGER_CLANG))
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
    /** Opaque contents to the 128 bit vector register */
    BURGER_ALIGN(float, m128_f32[4], 16);
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
    /** Opaque contents to the 128 bit vector register */
    BURGER_ALIGN(float, m128_f32[4], 16);
};
#endif

/***************************************

    Elements in an array template

***************************************/

#if defined(BURGER_WATCOM) || defined(BURGER_APPLE_SC) || defined(BURGER_MRC)
#define BURGER_ARRAYSIZE(x) static_cast<uintptr_t>(sizeof(x) / sizeof(x[0]))
#else
template<typename T, uintptr_t N>
extern const char (*_BurgerArraySize(T (&)[N]))[N];

#define BURGER_ARRAYSIZE(x) static_cast<uintptr_t>(sizeof(*_BurgerArraySize(x)))
#endif

/***************************************

    In place new

***************************************/

// clang-format off
#if !defined(BURGER_NONEW)

#if defined(BURGER_METROWERKS)
#ifndef _NEW
#define _NEW
BURGER_INLINE void* operator new(__typeof__(sizeof(0)), void* x) BURGER_NOEXCEPT { return x; }
#endif

#elif defined(BURGER_MSVC) || defined(BURGER_INTEL_COMPILER)
#define __PLACEMENT_NEW_INLINE
BURGER_INLINE void* BURGER_ANSIAPI operator new(size_t, void* x) BURGER_NOEXCEPT { return x;}

#elif defined(BURGER_DJGPP) || defined(BURGER_INTEL_COMPILER) || \
    (defined(BURGER_GNUC) && defined(__MACH__) && \
        (defined(__APPLE_CPP__) || defined(__APPLE_CC__) || \
            defined(__NEXT_CPP__)))
BURGER_INLINE void* operator new(unsigned long int, void* x) BURGER_NOEXCEPT { return x; }

#elif defined(BURGER_PS4) || \
    (defined(BURGER_ANDROID) && defined(BURGER_64BITCPU))
BURGER_INLINE void* operator new(unsigned long, void* x) BURGER_NOEXCEPT { return x; }

#elif defined(BURGER_ANDROID) || defined(BURGER_SNSYSTEMS) || \
    defined(BURGER_GHS) || defined(BURGER_ARM_COMPILER)
BURGER_INLINE void* operator new(unsigned int, void* x) BURGER_NOEXCEPT { return x; }

#else
BURGER_INLINE void* operator new(uintptr_t, void* x) BURGER_NOEXCEPT { return x; }
#endif
#endif
// clang-format on

// Set up generic defines

// Hack for remapping uint_t to a uintptr_t for some compilers
#if defined(BURGER_WATCOM)
#define BURGER_NEED_UINTPTR_REMAP uint32_t
#elif defined(BURGER_MACOSX) || defined(BURGER_IOS)
#if defined(__LP64__)
#define BURGER_NEED_UINTPTR_REMAP uint64_t
#else
#define BURGER_NEED_UINTPTR_REMAP uint32_t
#endif
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

// Pointer types cast to an integer

typedef uintptr_t WordPtr;
typedef intptr_t IntPtr;
#define BURGER_MAXWORDPTR UINTPTR_MAX
#define BURGER_MAXINTPTR INTPTR_MAX

/* END */

#if (BURGER_MSVC >= 192000000)
#pragma warning(disable : 26429 26446 26472 26481 26482 26485 26490)
#endif

#endif
