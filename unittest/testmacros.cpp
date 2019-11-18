/***************************************

    Determine which compiler is being used and create standardized typedefs and
    macros so generic code can be created cross platform

    Copyright (c) 1995-2019 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "testmacros.h"
#include "bratomic.h"
#include "brfloatingpoint.h"
#include "brglobals.h"
#include "common.h"
#include <stdio.h>

#if defined(BURGER_STRUCT_ALIGN)
#pragma options align = native
#elif defined(BURGER_STRUCT_PACKPUSH)
#pragma pack(push, 8)
#elif defined(BURGER_STRUCT_PACK)
#pragma pack(8)
#endif

#if defined(BURGER_STRUCT_ALIGN)
#pragma options align = reset
#elif defined(BURGER_STRUCT_PACKPUSH)
#pragma pack(pop)
#elif defined(BURGER_STRUCT_PACK)
#pragma pack()
#endif

#if defined(BURGER_MSVC)
// Disable not enough actual parameters for macro
#pragma warning(disable : 4003)
// Disable structure was padded due to __declspec(align())
#pragma warning(disable : 4324)
#endif

/***************************************

    Print the contents of these macros so they can be manually checked for
    correctness

***************************************/

static void BURGER_API ShowCompilerMacros(uint_t uVerbose)
{
    if (uVerbose & VERBOSE_MACROS) {

        Message("Known compiler macros found");

        // Print known C++ compiler flags
#if defined(__STDC__)
        Message("__STDC__ is defined as \"" BURGER_STRINGIZE(__STDC__) "\"");
#else
        Message("__STDC__ is not defined");
#endif

#if defined(__STDC_VERSION__)
        Message("__STDC_VERSION__ is defined as \"" BURGER_STRINGIZE(
            __STDC_VERSION__) "\"");
#else
        Message("__STDC_VERSION__ is not defined");
#endif

#if defined(__cplusplus)
        Message(
            "__cplusplus is defined as \"" BURGER_STRINGIZE(__cplusplus) "\"");
#else
        Message("__cplusplus is not defined");
#endif

#if defined(__cplusplus_cli)
        Message("__cplusplus_cli is defined as \"" BURGER_STRINGIZE(
            __cplusplus_cli) "\"");
#else
        Message("__cplusplus_cli is not defined");
#endif

#if defined(__embedded_cplusplus)
        Message("__embedded_cplusplus is defined as \"" BURGER_STRINGIZE(
            __embedded_cplusplus) "\"");
#else
        Message("__embedded_cplusplus is not defined");
#endif

        // Unix
#if defined(unix)
        Message("unix is defined as \"" BURGER_STRINGIZE(unix) "\"");
#else
        Message("unix is not defined");
#endif

#if defined(__unix__)
        Message("__unix__ is defined as \"" BURGER_STRINGIZE(__unix__) "\"");
#else
        Message("__unix__ is not defined");
#endif

#if defined(__unix)
        Message("__unix is defined as \"" BURGER_STRINGIZE(__unix) "\"");
#else
        Message("__unix is not defined");
#endif

        // Posix
#if defined(_POSIX_VERSION)
        Message("_POSIX_VERSION is defined as \"" BURGER_STRINGIZE(
            _POSIX_VERSION) "\"");
#else
        Message("_POSIX_VERSION is not defined");
#endif

#if defined(_POSIX2_C_VERSION)
        Message("_POSIX2_C_VERSION is defined as \"" BURGER_STRINGIZE(
            _POSIX2_C_VERSION) "\"");
#else
        Message("_POSIX2_C_VERSION is not defined");
#endif

        // X/Open
#if defined(_XOPEN_VERSION)
        Message("_XOPEN_VERSION is defined as \"" BURGER_STRINGIZE(
            _XOPEN_VERSION) "\"");
#else
        Message("_XOPEN_VERSION is not defined");
#endif

#if defined(_XOPEN_UNIX)
        Message(
            "_XOPEN_UNIX is defined as \"" BURGER_STRINGIZE(_XOPEN_UNIX) "\"");
#else
        Message("_XOPEN_UNIX is not defined");
#endif

        // Linux Standards base
#if defined(__LSB_VERSION__)
        Message("__LSB_VERSION__ is defined as \"" BURGER_STRINGIZE(
            __LSB_VERSION__) "\"");
#else
        Message("__LSB_VERSION__ is not defined");
#endif

        // Data models
#if defined(_ILP32)
        Message("_ILP32 is defined as \"" BURGER_STRINGIZE(_ILP32) "\"");
#else
        Message("_ILP32 is not defined");
#endif

#if defined(__ILP32__)
        Message("__ILP32__ is defined as \"" BURGER_STRINGIZE(__ILP32__) "\"");
#else
        Message("__ILP32__ is not defined");
#endif

#if defined(_LP64)
        Message("_LP64 is defined as \"" BURGER_STRINGIZE(_LP64) "\"");
#else
        Message("_LP64 is not defined");
#endif

#if defined(__LP64__)
        Message("__LP64__ is defined as \"" BURGER_STRINGIZE(__LP64__) "\"");
#else
        Message("__LP64__ is not defined");
#endif

        // Operating system macros

        // IBM AIX
#if defined(_AIX)
        Message("_AIX is defined as \"" BURGER_STRINGIZE(_AIX) "\"");
#else
        Message("_AIX is not defined");
#endif

#if defined(__TOS_AIX__)
        Message(
            "__TOS_AIX__ is defined as \"" BURGER_STRINGIZE(__TOS_AIX__) "\"");
#else
        Message("__TOS_AIX__ is not defined");
#endif

        // Android
#if defined(__ANDROID__)
        Message(
            "__ANDROID__ is defined as \"" BURGER_STRINGIZE(__ANDROID__) "\"");
#else
        Message("__ANDROID__ is not defined");
#endif

#if defined(__ANDROID_API__)
        Message("__ANDROID_API__ is defined as \"" BURGER_STRINGIZE(
            __ANDROID_API__) "\"");
#else
        Message("__ANDROID_API__ is not defined");
#endif

        // Commodore Amiga
#if defined(AMIGA)
        Message("AMIGA is defined as \"" BURGER_STRINGIZE(AMIGA) "\"");
#else
        Message("AMIGA is not defined");
#endif

#if defined(__amigaos__)
        Message(
            "__amigaos__ is defined as \"" BURGER_STRINGIZE(__amigaos__) "\"");
#else
        Message("__amigaos__ is not defined");
#endif

        // BeOS
#if defined(__BEOS__)
        Message("__BEOS__ is defined as \"" BURGER_STRINGIZE(__BEOS__) "\"");
#else
        Message("__BEOS__ is not defined");
#endif

#if defined(__be_os)
        Message("__be_os is defined as \"" BURGER_STRINGIZE(__be_os) "\"");
#else
        Message("__be_os is not defined");
#endif

        // Berkeley Software Distribution (BSD)
#if defined(BSD)
        Message("BSD is defined as \"" BURGER_STRINGIZE(BSD) "\"");
#else
        Message("BSD is not defined");
#endif

#if defined(__bsdi__)
        Message("__bsdi__ is defined as \"" BURGER_STRINGIZE(__bsdi__) "\"");
#else
        Message("__bsdi__ is not defined");
#endif

#if defined(__DragonFly__)
        Message("__DragonFly__ is defined as \"" BURGER_STRINGIZE(
            __DragonFly__) "\"");
#else
        Message("__DragonFly__ is not defined");
#endif

#if defined(__FreeBSD__)
        Message(
            "__FreeBSD__ is defined as \"" BURGER_STRINGIZE(__FreeBSD__) "\"");
#else
        Message("__FreeBSD__ is not defined");
#endif

#if defined(__FreeBSD_kernel__)
        Message("__FreeBSD_kernel__ is defined as \"" BURGER_STRINGIZE(
            __FreeBSD_kernel__) "\"");
#else
        Message("__FreeBSD_kernel__ is not defined");
#endif

#if defined(__NetBSD__)
        Message(
            "__NetBSD__ is defined as \"" BURGER_STRINGIZE(__NetBSD__) "\"");
#else
        Message("__NetBSD__ is not defined");
#endif

#if defined(__OpenBSD__)
        Message(
            "__OpenBSD__ is defined as \"" BURGER_STRINGIZE(__OpenBSD__) "\"");
#else
        Message("__OpenBSD__ is not defined");
#endif

#if defined(_SYSTYPE_BSD)
        Message("_SYSTYPE_BSD is defined as \"" BURGER_STRINGIZE(
            _SYSTYPE_BSD) "\"");
#else
        Message("_SYSTYPE_BSD is not defined");
#endif

        // Cygwin
#if defined(__CYGWIN__)
        Message(
            "__CYGWIN__ is defined as \"" BURGER_STRINGIZE(__CYGWIN__) "\"");
#else
        Message("__CYGWIN__ is not defined");
#endif

        // Linux
#if defined(linux)
        Message("linux is defined as \"" BURGER_STRINGIZE(linux) "\"");
#else
        Message("linux is not defined");
#endif

#if defined(__linux)
        Message("__linux is defined as \"" BURGER_STRINGIZE(__linux) "\"");
#else
        Message("__linux is not defined");
#endif

#if defined(__linux__)
        Message("__linux__ is defined as \"" BURGER_STRINGIZE(__linux__) "\"");
#else
        Message("__linux__ is not defined");
#endif

#if defined(__gnu_linux__)
        Message("__gnu_linux__ is defined as \"" BURGER_STRINGIZE(
            __gnu_linux__) "\"");
#else
        Message("__gnu_linux__ is not defined");
#endif

        // Macintosh
#if defined(macintosh)
        Message("macintosh is defined as \"" BURGER_STRINGIZE(macintosh) "\"");
#else
        Message("macintosh is not defined");
#endif

#if defined(Macintosh)
        Message("Macintosh is defined as \"" BURGER_STRINGIZE(Macintosh) "\"");
#else
        Message("Macintosh is not defined");
#endif

#if defined(__APPLE__)
        Message("__APPLE__ is defined as \"" BURGER_STRINGIZE(__APPLE__) "\"");
#else
        Message("__APPLE__ is not defined");
#endif

#if defined(__MACH__)
        Message("__MACH__ is defined as \"" BURGER_STRINGIZE(__MACH__) "\"");
#else
        Message("__MACH__ is not defined");
#endif

#if defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
        Message(
            "__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ is defined as \"" BURGER_STRINGIZE(
                __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) "\"");
#else
        Message(
            "__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ is not defined");
#endif

#if defined(__CFM68K)
        Message("__CFM68K is defined as \"" BURGER_STRINGIZE(__CFM68K) "\"");
#else
        Message("__CFM68K is not defined");
#endif

#if defined(__CFM68K__)
        Message(
            "__CFM68K__ is defined as \"" BURGER_STRINGIZE(__CFM68K__) "\"");
#else
        Message("__CFM68K__ is not defined");
#endif

        // Morph OS
#if defined(__MORPHOS__)
        Message(
            "__MORPHOS__ is defined as \"" BURGER_STRINGIZE(__MORPHOS__) "\"");
#else
        Message("__MORPHOS__ is not defined");
#endif

        // MSDOS
#if defined(MSDOS)
        Message("MSDOS is defined as \"" BURGER_STRINGIZE(MSDOS) "\"");
#else
        Message("MSDOS is not defined");
#endif

#if defined(__DOS__)
        Message("__DOS__ is defined as \"" BURGER_STRINGIZE(__DOS__) "\"");
#else
        Message("__DOS__ is not defined");
#endif

#if defined(__MSDOS__)
        Message("__MSDOS__ is defined as \"" BURGER_STRINGIZE(__MSDOS__) "\"");
#else
        Message("__MSDOS__ is not defined");
#endif

#if defined(_MSDOS)
        Message("_MSDOS is defined as \"" BURGER_STRINGIZE(_MSDOS) "\"");
#else
        Message("_MSDOS is not defined");
#endif

#if defined(__DOS4G__)
        Message("__DOS4G__ is defined as \"" BURGER_STRINGIZE(__DOS4G__) "\"");
#else
        Message("__DOS4G__ is not defined");
#endif

#if defined(__X32__)
        Message("__X32__ is defined as \"" BURGER_STRINGIZE(__X32__) "\"");
#else
        Message("__X32__ is not defined");
#endif

        // Solaris
#if defined(sun)
        Message("sun is defined as \"" BURGER_STRINGIZE(sun) "\"");
#else
        Message("sun is not defined");
#endif

#if defined(__sun)
        Message("__sun is defined as \"" BURGER_STRINGIZE(__sun) "\"");
#else
        Message("__sun is not defined");
#endif

        // Windows
#if defined(_WIN16)
        Message("_WIN16 is defined as \"" BURGER_STRINGIZE(_WIN16) "\"");
#else
        Message("_WIN16 is not defined");
#endif

#if defined(_WIN32)
        Message("_WIN32 is defined as \"" BURGER_STRINGIZE(_WIN32) "\"");
#else
        Message("_WIN32 is not defined");
#endif

#if defined(_WIN64)
        Message("_WIN64 is defined as \"" BURGER_STRINGIZE(_WIN64) "\"");
#else
        Message("_WIN64 is not defined");
#endif

#if defined(__WIN32__)
        Message("__WIN32__ is defined as \"" BURGER_STRINGIZE(__WIN32__) "\"");
#else
        Message("__WIN32__ is not defined");
#endif

#if defined(__WINDOWS__)
        Message(
            "__WINDOWS__ is defined as \"" BURGER_STRINGIZE(__WINDOWS__) "\"");
#else
        Message("__WINDOWS__ is not defined");
#endif

#if defined(__NT__)
        Message("__NT__ is defined as \"" BURGER_STRINGIZE(__NT__) "\"");
#else
        Message("__NT__ is not defined");
#endif

#if defined(__TOS_WIN__)
        Message(
            "__TOS_WIN__ is defined as \"" BURGER_STRINGIZE(__TOS_WIN__) "\"");
#else
        Message("__TOS_WIN__ is not defined");
#endif

#if defined(_WIN32_WCE)
        Message(
            "_WIN32_WCE is defined as \"" BURGER_STRINGIZE(_WIN32_WCE) "\"");
#else
        Message("_WIN32_WCE is not defined");
#endif

        // Sony platforms
#if defined(SN_TARGET_PS2)
        Message("SN_TARGET_PS2 is defined as \"" BURGER_STRINGIZE(
            SN_TARGET_PS2) "\"");
#else
        Message("SN_TARGET_PS2 is not defined");
#endif

#if defined(__CELLOS_LV2__)
        Message("__CELLOS_LV2__ is defined as \"" BURGER_STRINGIZE(
            __CELLOS_LV2__) "\"");
#else
        Message("__CELLOS_LV2__ is not defined");
#endif

#if defined(__ORBIS__)
        Message("__ORBIS__ is defined as \"" BURGER_STRINGIZE(__ORBIS__) "\"");
#else
        Message("__ORBIS__ is not defined");
#endif

#if defined(SN_TARGET_PSP)
        Message("SN_TARGET_PSP is defined as \"" BURGER_STRINGIZE(
            SN_TARGET_PSP) "\"");
#else
        Message("SN_TARGET_PSP is not defined");
#endif

#if defined(__psp2__)
        Message("__psp2__ is defined as \"" BURGER_STRINGIZE(__psp2__) "\"");
#else
        Message("__psp2__ is not defined");
#endif

        // Xbox
#if defined(_XBOX)
        Message("_XBOX is defined as \"" BURGER_STRINGIZE(_XBOX) "\"");
#else
        Message("_XBOX is not defined");
#endif

#if defined(XBOX)
        Message("XBOX is defined as \"" BURGER_STRINGIZE(XBOX) "\"");
#else
        Message("XBOX is not defined");
#endif

        // Nintendo
#if defined(SDK_ARM9)
        Message("SDK_ARM9 is defined as \"" BURGER_STRINGIZE(SDK_ARM9) "\"");
#else
        Message("SDK_ARM9 is not defined");
#endif

#if defined(SDK_TS)
        Message("SDK_TS is defined as \"" BURGER_STRINGIZE(SDK_TS) "\"");
#else
        Message("SDK_TS is not defined");
#endif

#if defined(RVL_SDK)
        Message("RVL_SDK is defined as \"" BURGER_STRINGIZE(RVL_SDK) "\"");
#else
        Message("RVL_SDK is not defined");
#endif

        // Print known C++ compiler version identifiers
#if defined(__clang__)
        Message("__clang__ is defined as \"" BURGER_STRINGIZE(__clang__) "\"");
#else
        Message("__clang__ is not defined");
#endif

#if defined(__ghs__)
        Message("__ghs__ is defined as \"" BURGER_STRINGIZE(__ghs__) "\"");
#else
        Message("__ghs__ is not defined");
#endif

#if defined(__llvm__)
        Message("__llvm__ is defined as \"" BURGER_STRINGIZE(__llvm__) "\"");
#else
        Message("__llvm__ is not defined");
#endif

#if defined(__xlc__)
        Message("__xlc__ is defined as \"" BURGER_STRINGIZE(__xlc__) "\"");
#else
        Message("__xlc__ is not defined");
#endif

#if defined(__APPLE_CPP__)
        Message("__APPLE_CPP__ is defined as \"" BURGER_STRINGIZE(
            __APPLE_CPP__) "\"");
#else
        Message("__APPLE_CPP__ is not defined");
#endif

#if defined(__APPLE_CC__)
        Message("__APPLE_CC__ is defined as \"" BURGER_STRINGIZE(
            __APPLE_CC__) "\"");
#else
        Message("__APPLE_CC__ is not defined");
#endif

#if defined(__NEXT_CPP__)
        Message("__NEXT_CPP__ is defined as \"" BURGER_STRINGIZE(
            __NEXT_CPP__) "\"");
#else
        Message("__NEXT_CPP__ is not defined");
#endif

#if defined(__BORLANDC__)
        Message("__BORLANDC__ is defined as \"" BURGER_STRINGIZE(
            __BORLANDC__) "\"");
#else
        Message("__BORLANDC__ is not defined");
#endif

#if defined(__CC65__)
        Message("__CC65__ is defined as \"" BURGER_STRINGIZE(__CC65__) "\"");
#else
        Message("__CC65__ is not defined");
#endif

#if defined(__DJGPP__)
        Message("__DJGPP__ is defined as \"" BURGER_STRINGIZE(__DJGPP__) "\"");
#else
        Message("__DJGPP__ is not defined");
#endif

#if defined(__GNUC__)
        Message("__GNUC__ is defined as \"" BURGER_STRINGIZE(__GNUC__) "\"");
#else
        Message("__GNUC__ is not defined");
#endif

#if defined(__INTEL_COMPILER)
        Message("__INTEL_COMPILER is defined as \"" BURGER_STRINGIZE(
            __INTEL_COMPILER) "\"");
#else
        Message("__INTEL_COMPILER is not defined");
#endif

#if defined(__ICL)
        Message("__ICL is defined as \"" BURGER_STRINGIZE(__ICL) "\"");
#else
        Message("__ICL is not defined");
#endif

#if defined(__LCC__)
        Message("__LCC__ is defined as \"" BURGER_STRINGIZE(__LCC__) "\"");
#else
        Message("__LCC__ is not defined");
#endif

#if defined(__MWERKS__)
        Message(
            "__MWERKS__ is defined as \"" BURGER_STRINGIZE(__MWERKS__) "\"");
#else
        Message("__MWERKS__ is not defined");
#endif

#if defined(__CWCC__)
        Message("__CWCC__ is defined as \"" BURGER_STRINGIZE(__CWCC__) "\"");
#else
        Message("__CWCC__ is not defined");
#endif

#if defined(__MSL__)
        Message("__MSL__ is defined as \"" BURGER_STRINGIZE(__MSL__) "\"");
#else
        Message("__MSL__ is not defined");
#endif

#if defined(_MSC_VER)
        Message("_MSC_VER is defined as \"" BURGER_STRINGIZE(_MSC_VER) "\"");
#else
        Message("_MSC_VER is not defined");
#endif

#if defined(_MSVC_LANG)
        Message(
            "_MSVC_LANG is defined as \"" BURGER_STRINGIZE(_MSVC_LANG) "\"");
#else
        Message("_MSVC_LANG is not defined");
#endif

#if defined(__MINGW32__)
        Message(
            "__MINGW32__ is defined as \"" BURGER_STRINGIZE(__MINGW32__) "\"");
#else
        Message("__MINGW32__ is not defined");
#endif

#if defined(__MINGW64__)
        Message(
            "__MINGW64__ is defined as \"" BURGER_STRINGIZE(__MINGW64__) "\"");
#else
        Message("__MINGW64__ is not defined");
#endif

#if defined(__MRC__)
        Message("__MRC__ is defined as \"" BURGER_STRINGIZE(__MRC__) "\"");
#else
        Message("__MRC__ is not defined");
#endif

#if defined(__CC_NORCROFT)
        Message("__CC_NORCROFT is defined as \"" BURGER_STRINGIZE(
            __CC_NORCROFT) "\"");
#else
        Message("__CC_NORCROFT is not defined");
#endif

#if defined(__SC__)
        Message("__SC__ is defined as \"" BURGER_STRINGIZE(__SC__) "\"");
#else
        Message("__SC__ is not defined");
#endif

#if defined(MPW_CPLUS)
        Message("MPW_CPLUS is defined as \"" BURGER_STRINGIZE(MPW_CPLUS) "\"");
#else
        Message("MPW_CPLUS is not defined");
#endif

#if defined(MPW_C)
        Message("MPW_C is defined as \"" BURGER_STRINGIZE(MPW_C) "\"");
#else
        Message("MPW_C is not defined");
#endif

#if defined(__SN__)
        Message("__SN__ is defined as \"" BURGER_STRINGIZE(__SN__) "\"");
#else
        Message("__SN__ is not defined");
#endif

#if defined(__SNC__)
        Message("__SNC__ is defined as \"" BURGER_STRINGIZE(__SNC__) "\"");
#else
        Message("__SNC__ is not defined");
#endif

#if defined(__TINYC__)
        Message("__TINYC__ is defined as \"" BURGER_STRINGIZE(__TINYC__) "\"");
#else
        Message("__TINYC__ is not defined");
#endif

#if defined(__TURBOC__)
        Message(
            "__TURBOC__ is defined as \"" BURGER_STRINGIZE(__TURBOC__) "\"");
#else
        Message("__TURBOC__ is not defined");
#endif

#if defined(__WATCOMC__)
        Message(
            "__WATCOMC__ is defined as \"" BURGER_STRINGIZE(__WATCOMC__) "\"");
#else
        Message("__WATCOMC__ is not defined");
#endif

#if defined(__ZTC__)
        Message("__ZTC__ is defined as \"" BURGER_STRINGIZE(__ZTC__) "\"");
#else
        Message("__ZTC__ is not defined");
#endif

        // Print known CPU identifiers
#if defined(__R5900__)
        Message("__R5900__ is defined as \"" BURGER_STRINGIZE(__R5900__) "\"");
#else
        Message("__R5900__ is not defined");
#endif

#if defined(__R5900)
        Message("__R5900 is defined as \"" BURGER_STRINGIZE(__R5900) "\"");
#else
        Message("__R5900 is not defined");
#endif

#if defined(__MIPS__)
        Message("__MIPS__ is defined as \"" BURGER_STRINGIZE(__MIPS__) "\"");
#else
        Message("__MIPS__ is not defined");
#endif

#if defined(__mips__)
        Message("__mips__ is defined as \"" BURGER_STRINGIZE(__mips__) "\"");
#else
        Message("__mips__ is not defined");
#endif

#if defined(__PPC__)
        Message("__PPC__ is defined as \"" BURGER_STRINGIZE(__PPC__) "\"");
#else
        Message("__PPC__ is not defined");
#endif

#if defined(__PPCGEKKO__)
        Message("__PPCGEKKO__ is defined as \"" BURGER_STRINGIZE(
            __PPCGEKKO__) "\"");
#else
        Message("__PPCGEKKO__ is not defined");
#endif

#if defined(__PPCBROADWAY__)
        Message("__PPCBROADWAY__ is defined as \"" BURGER_STRINGIZE(
            __PPCBROADWAY__) "\"");
#else
        Message("__PPCBROADWAY__ is not defined");
#endif

#if defined(_M_MPPC)
        Message("_M_MPPC is defined as \"" BURGER_STRINGIZE(_M_MPPC) "\"");
#else
        Message("_M_MPPC is not defined");
#endif

#if defined(__ppc__)
        Message("__ppc__ is defined as \"" BURGER_STRINGIZE(__ppc__) "\"");
#else
        Message("__ppc__ is not defined");
#endif

#if defined(__ppc64__)
        Message("__ppc64__ is defined as \"" BURGER_STRINGIZE(__ppc64__) "\"");
#else
        Message("__ppc64__ is not defined");
#endif

#if defined(__espresso__)
        Message("__espresso__ is defined as \"" BURGER_STRINGIZE(
            __espresso__) "\"");
#else
        Message("__espresso__ is not defined");
#endif

#if defined(__ALTIVEC__)
        Message(
            "__ALTIVEC__ is defined as \"" BURGER_STRINGIZE(__ALTIVEC__) "\"");
#else
        Message("__ALTIVEC__ is not defined");
#endif

#if defined(_M_IX86)
        Message("_M_IX86 is defined as \"" BURGER_STRINGIZE(_M_IX86) "\"");
#else
        Message("_M_IX86 is not defined");
#endif

#if defined(__i386__)
        Message("__i386__ is defined as \"" BURGER_STRINGIZE(__i386__) "\"");
#else
        Message("__i386__ is not defined");
#endif

#if defined(_M_AMD64)
        Message("_M_AMD64 is defined as \"" BURGER_STRINGIZE(_M_AMD64) "\"");
#else
        Message("_M_AMD64 is not defined");
#endif

#if defined(_M_X64)
        Message("_M_X64 is defined as \"" BURGER_STRINGIZE(_M_X64) "\"");
#else
        Message("_M_X64 is not defined");
#endif

#if defined(__x86_64__)
        Message(
            "__x86_64__ is defined as \"" BURGER_STRINGIZE(__x86_64__) "\"");
#else
        Message("__x86_64__ is not defined");
#endif

#if defined(__SSE__)
        Message("__SSE__ is defined as \"" BURGER_STRINGIZE(__SSE__) "\"");
#else
        Message("__SSE__ is not defined");
#endif

#if defined(__SSE2__)
        Message("__SSE2__ is defined as \"" BURGER_STRINGIZE(__SSE2__) "\"");
#else
        Message("__SSE2__ is not defined");
#endif

#if defined(__AVX__)
        Message("__AVX__ is defined as \"" BURGER_STRINGIZE(__AVX__) "\"");
#else
        Message("__AVX__ is not defined");
#endif

#if defined(__AVX2__)
        Message("__AVX2__ is defined as \"" BURGER_STRINGIZE(__AVX2__) "\"");
#else
        Message("__AVX2__ is not defined");
#endif

#if defined(_M_IA64)
        Message("_M_IA64 is defined as \"" BURGER_STRINGIZE(_M_IA64) "\"");
#else
        Message("_M_IA64 is not defined");
#endif

#if defined(_M_MRX000)
        Message("_M_MRX000 is defined as \"" BURGER_STRINGIZE(_M_MRX000) "\"");
#else
        Message("_M_MRX000 is not defined");
#endif

#if defined(_M_M68K)
        Message("_M_M68K is defined as \"" BURGER_STRINGIZE(_M_M68K) "\"");
#else
        Message("_M_M68K is not defined");
#endif

#if defined(__MC68K__)
        Message("__MC68K__ is defined as \"" BURGER_STRINGIZE(__MC68K__) "\"");
#else
        Message("__MC68K__ is not defined");
#endif

#if defined(_68K_)
        Message("_68K_ is defined as \"" BURGER_STRINGIZE(_68K_) "\"");
#else
        Message("_68K_ is not defined");
#endif

#if defined(__m68k__)
        Message("__m68k__ is defined as \"" BURGER_STRINGIZE(__m68k__) "\"");
#else
        Message("__m68k__ is not defined");
#endif

#if defined(mc68881)
        Message("mc68881 is defined as \"" BURGER_STRINGIZE(mc68881) "\"");
#else
        Message("mc68881 is not defined");
#endif

#if defined(__MC68881__)
        Message(
            "__MC68881__ is defined as \"" BURGER_STRINGIZE(__MC68881__) "\"");
#else
        Message("__MC68881__ is not defined");
#endif

#if defined(_M_ARM)
        Message("_M_ARM is defined as \"" BURGER_STRINGIZE(_M_ARM) "\"");
#else
        Message("_M_ARM is not defined");
#endif

#if defined(__arm__)
        Message("__arm__ is defined as \"" BURGER_STRINGIZE(__arm__) "\"");
#else
        Message("__arm__ is not defined");
#endif

#if defined(__arm64__)
        Message("__arm64__ is defined as \"" BURGER_STRINGIZE(__arm64__) "\"");
#else
        Message("__arm64__ is not defined");
#endif

#if defined(__ARM_NEON__)
        Message("__ARM_NEON__ is defined as \"" BURGER_STRINGIZE(
            __ARM_NEON__) "\"");
#else
        Message("__ARM_NEON__ is not defined");
#endif

#if defined(__aarch64__)
        Message(
            "__aarch64__ is defined as \"" BURGER_STRINGIZE(__aarch64__) "\"");
#else
        Message("__aarch64__ is not defined");
#endif
    }
}

/***************************************

    Print the contents of these macros so they
    can be manually checked for correctness

***************************************/

static void BURGER_API ShowMacros(uint_t uVerbose)
{
    if (uVerbose & VERBOSE_MACROS) {
        Message("NULL is defined as \"" BURGER_STRINGIZE(NULL) "\"");
        Message("TRUE is defined as \"" BURGER_STRINGIZE(TRUE) "\"");
        Message("FALSE is defined as \"" BURGER_STRINGIZE(FALSE) "\"");

        Message("BURGER_ALIGN is defined as \"" BURGER_STRINGIZE(
            BURGER_ALIGN(x, s)) "\"");
        Message("BURGER_PREALIGN is defined as \"" BURGER_STRINGIZE(
            BURGER_PREALIGN(s)) "\"");
        Message("BURGER_POSTALIGN is defined as \"" BURGER_STRINGIZE(
            BURGER_POSTALIGN(s)) "\"");

        Message(
            "BURGER_API is defined as \"" BURGER_STRINGIZE(BURGER_API) "\"");
        Message("BURGER_ANSIAPI is defined as \"" BURGER_STRINGIZE(
            BURGER_ANSIAPI) "\"");
        Message("BURGER_INLINE is defined as \"" BURGER_STRINGIZE(
            BURGER_INLINE) "\"");
        Message("BURGER_DECLSPECNAKED is defined as \"" BURGER_STRINGIZE(
            BURGER_DECLSPECNAKED) "\"");
        Message(
            "BURGER_ASM is defined as \"" BURGER_STRINGIZE(BURGER_ASM) "\"");
        Message("BURGER_MININT is defined as \"" BURGER_STRINGIZE(
            BURGER_MININT) "\"");
        Message("BURGER_MAXINT is defined as \"" BURGER_STRINGIZE(
            BURGER_MAXINT) "\"");
        Message("BURGER_MAXUINT is defined as \"" BURGER_STRINGIZE(
            BURGER_MAXUINT) "\"");
        Message("BURGER_MININT64 is defined as \"" BURGER_STRINGIZE(
            BURGER_MININT64) "\"");
        Message("BURGER_MAXINT64 is defined as \"" BURGER_STRINGIZE(
            BURGER_MAXINT64) "\"");
        Message("BURGER_MAXUINT64 is defined as \"" BURGER_STRINGIZE(
            BURGER_MAXUINT64) "\"");

        // 64 bit supported?

#if defined(BURGER_LONGLONG)
        Message("BURGER_LONGLONG is defined as \"" BURGER_STRINGIZE(
            BURGER_LONGLONG) "\"");
#endif

        // Display optional macros

#if defined(BURGER_STRUCT_PACKPUSH)
        Message("BURGER_STRUCT_PACKPUSH is defined");
#endif

#if defined(BURGER_STRUCT_ALIGN)
        Message("BURGER_STRUCT_ALIGN is defined");
#endif

#if defined(BURGER_STRUCT_PACK)
        Message("BURGER_STRUCT_PACK is defined");
#endif

#if defined(BURGER_FASTCALLENABLED)
        Message("BURGER_FASTCALLENABLED is defined");
#endif

#if defined(BURGER_NO_USING_NAMESPACE)
        Message("BURGER_NO_USING_NAMESPACE is defined");
#endif

#if defined(BURGER_OPENGL)
        Message("BURGER_OPENGL is defined");
#endif

#if defined(BURGER_OPENGLES)
        Message("BURGER_OPENGLES is defined");
#endif

#if defined(BURGER_VULKAN)
        Message("BURGER_VULKAN is defined");
#endif
    }
}

/***************************************

    Test the endian macros

***************************************/

static uint_t BURGER_API TestEndianMacros(void)
{
    union {
        uint32_t m_uWord32;  // 32 bit integer to store
        uint8_t m_uBytes[4]; // Byte array to test the endian with
    } EndianTest;

    uint_t uFailure = 0;

#if defined(BURGER_LITTLEENDIAN) && defined(BURGER_BIGENDIAN)
    Message(
        "Error: Both BURGER_LITTLEENDIAN and BURGER_BIGENDIAN are defined!");
    uFailure = 10;
#endif

#if !defined(BURGER_LITTLEENDIAN) && !defined(BURGER_BIGENDIAN)
    Message(
        "Error: Neither BURGER_LITTLEENDIAN or BURGER_BIGENDIAN are defined!");
    uFailure = 10;
#endif

    //
    // Test the endian ACTUALLY matches
    //

    EndianTest.m_uWord32 = 0x12345678U;
    uint_t uTest;

#if defined(BURGER_BIGENDIAN)
    uTest = (EndianTest.m_uBytes[0] == 0x78);
    uFailure |= uTest;
    ReportFailure(
        "BURGER_BIGENDIAN was defined on a Little endian machine!", uTest);
#endif

#if defined(BURGER_LITTLEENDIAN)
    uTest = (EndianTest.m_uBytes[0] == 0x12);
    uFailure |= uTest;
    ReportFailure(
        "BURGER_LITTLEENDIAN was defined on a Big endian machine!", uTest);
#endif
    return uFailure;
}

/***************************************

    Test the debug flag macros

***************************************/

static uint_t BURGER_API TestDebugMacros(uint_t uVerbose)
{
    uint_t uFailure = 0;

#if defined(NDEBUG) && defined(_DEBUG)
    Message("Error: Both NDEBUG and _DEBUG are defined!");
    uFailure = 10;
#endif

#if !defined(NDEBUG) && !defined(_DEBUG)
    Message("Error: Neither NDEBUG or _DEBUG are defined!");
    uFailure = 10;
#endif

    //
    // Display the debug/release macros
    //

    if (uVerbose & VERBOSE_MACROS) {
#if defined(NDEBUG)
        Message("NDEBUG is defined");
#endif

#if defined(_DEBUG)
        Message("_DEBUG is defined");
#endif
    }

    return uFailure;
}

/***************************************

    Test the compiler macros

***************************************/

static uint_t BURGER_API TestCompilerMacros(uint_t uVerbose)
{
    uint_t uFailure = 0;
    uint_t uTest = 0;

    if (uVerbose & VERBOSE_MACROS) {
        // Compiler level

#if defined(BURGER_CPP89)
        Message("BURGER_CPP89 is defined as \"" BURGER_STRINGIZE(
            BURGER_CPP89) "\"");
#endif

#if defined(BURGER_CPP98)
        Message("BURGER_CPP98 is defined as \"" BURGER_STRINGIZE(
            BURGER_CPP98) "\"");
#endif

#if defined(BURGER_CPP11)
        Message("BURGER_CPP11 is defined as \"" BURGER_STRINGIZE(
            BURGER_CPP11) "\"");
#endif

#if defined(BURGER_CPP14)
        Message("BURGER_CPP14 is defined as \"" BURGER_STRINGIZE(
            BURGER_CPP14) "\"");
#endif

#if defined(BURGER_CPP17)
        Message("BURGER_CPP17 is defined as \"" BURGER_STRINGIZE(
            BURGER_CPP17) "\"");
#endif

#if defined(BURGER_CPP20)
        Message("BURGER_CPP20 is defined as \"" BURGER_STRINGIZE(
            BURGER_CPP20) "\"");
#endif

#if defined(BURGER_GNUC_VERSION)
        Message("BURGER_GNUC_VERSION is defined as \"" BURGER_STRINGIZE(
            BURGER_GNUC_VERSION) "\"");
#endif

#if defined(BURGER_CLANG_VERSION)
        Message("BURGER_CLANG_VERSION is defined as \"" BURGER_STRINGIZE(
            BURGER_CLANG_VERSION) "\"");
#endif

#if defined(BURGER_CONSTEXPR)
        Message("BURGER_CONSTEXPR is defined as \"" BURGER_STRINGIZE(
            BURGER_CONSTEXPR) "\"");
#endif

#if defined(BURGER_OVERRIDE)
        Message("BURGER_OVERRIDE is defined as \"" BURGER_STRINGIZE(
            BURGER_OVERRIDE) "\"");
#endif

#if defined(nullptr)
        Message("nullptr is defined");
#endif

#if defined(BURGER_ENUMSTART)
        Message("BURGER_ENUMSTART is defined as \"" BURGER_STRINGIZE(
            BURGER_ENUMSTART(SampleEnum, uint_t)) "\"");
#endif

#if defined(BURGER_ENUMEND)
        Message("BURGER_ENUMEND is defined as \"" BURGER_STRINGIZE(
            BURGER_ENUMEND(SampleEnum)) "\"");
#endif

        // Disable, the macro is huge
        //#if defined(BURGER_ENUMFLAGSEND)
        //        Message("BURGER_ENUMFLAGSEND is defined as \""
        //        BURGER_STRINGIZE(
        //            BURGER_ENUMFLAGSEND(SampleEnum, uint_t)) "\"");
        //#endif

#if defined(BURGER_STATIC_ASSERT)
        Message("BURGER_STATIC_ASSERT is defined as \"" BURGER_STRINGIZE(
            BURGER_STATIC_ASSERT(sizeof(int) == 4)) "\"");
#endif
    }

#if defined(BURGER_WATCOM)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_WATCOM is defined as \"" BURGER_STRINGIZE(
            BURGER_WATCOM) "\"");
    }
    ++uTest;
#endif

#if defined(BURGER_MINGW)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_MINGW is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_MRC)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_MRC is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_APPLE_SC)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_APPLE_SC is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_INTEL_COMPILER)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_INTEL_COMPILER is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_SNSYSTEMS)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_SNSYSTEMS is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_METROWERKS)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_METROWERKS is defined as \"" BURGER_STRINGIZE(
            BURGER_METROWERKS) "\"");
    }
    ++uTest;
#endif

#if defined(BURGER_MSVC)
    if (uVerbose & VERBOSE_MACROS) {
        Message(
            "BURGER_MSVC is defined as \"" BURGER_STRINGIZE(BURGER_MSVC) "\"");
    }
    ++uTest;
#endif

#if defined(BURGER_GNUC)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_GNUC is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_CLANG)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_CLANG is defined");
    }
    ++uTest;
#endif

    if (uTest >= 2) {
        Message("Multiple compilers have been defined!");
        uFailure = 10;
    }
    if (!uTest) {
        Message("Unknown compiler detected!");
        uFailure = 10;
    }
    return uFailure;
}

/***************************************

    Test the CPU macros

***************************************/

static uint_t BURGER_API TestCPUMacros(uint_t uVerbose)
{
    uint_t uFailure = 0;
    uint_t uTest = 0;

#if defined(BURGER_68K)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_68K is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_POWERPC)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_POWERPC is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_POWERPC64)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_POWERPC64 is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_ARM32)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_ARM32 is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_ARM64)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_ARM64 is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_X86)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_X86 is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_AMD64)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_AMD64 is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_MIPS32)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_MIPS32 is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_MIPS64)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_MIPS64 is defined");
    }
    ++uTest;
#endif

    // This is an enhancement, not a CPU type, so it's okay to
    // be defined with a CPU type

    if (uVerbose & VERBOSE_MACROS) {

#if defined(BURGER_SIZEOF_INT)
        Message("BURGER_SIZEOF_INT is defined as \"" BURGER_STRINGIZE(
            BURGER_SIZEOF_INT) "\"");
#endif

#if defined(BURGER_SIZEOF_LONG)
        Message("BURGER_SIZEOF_LONG is defined as \"" BURGER_STRINGIZE(
            BURGER_SIZEOF_LONG) "\"");
#endif

#if defined(BURGER_NEED_UINTPTR_REMAP)
        Message("BURGER_NEED_UINTPTR_REMAP is defined as \"" BURGER_STRINGIZE(
            BURGER_NEED_UINTPTR_REMAP) "\"");
#endif

#if defined(BURGER_64BITCPU)
        Message("BURGER_64BITCPU is defined");
#endif

#if defined(BURGER_INTEL)
        Message("BURGER_INTEL is defined");
#endif

#if defined(BURGER_PPC)
        Message("BURGER_PPC is defined");
#endif

#if defined(BURGER_ARM)
        Message("BURGER_ARM is defined");
#endif

#if defined(BURGER_MIPS)
        Message("BURGER_MIPS is defined");
#endif

#if defined(BURGER_SPARC)
        Message("BURGER_SPARC is defined");
#endif

#if defined(BURGER_NEON)
        Message("BURGER_NEON is defined");
#endif

#if defined(BURGER_SSE)
        Message("BURGER_SSE is defined");
#endif

#if defined(BURGER_SSE2)
        Message("BURGER_SSE2 is defined");
#endif

#if defined(BURGER_3DNOW)
        Message("BURGER_3DNOW is defined");
#endif

#if defined(BURGER_ALTIVEC)
        Message("BURGER_ALTIVEC is defined");
#endif

#if defined(BURGER_AVX)
        Message("BURGER_AVX is defined");
#endif

#if defined(BURGER_AVX2)
        Message("BURGER_AVX2 is defined");
#endif
    }

    if (uTest != 1) {
        Message("Multiple CPUs have been defined!");
        uFailure = 10;
    }

    if (!uTest) {
        Message("Unknown CPU detected!");
        uFailure = 10;
    }
    return uFailure;
}

/***************************************

    Test the platform macros

***************************************/

static uint_t BURGER_API TestPlatformMacros(uint_t uVerbose)
{
    uint_t uFailure = 0;
    uint_t uTest = 0;

    //
    // Handle MSDOS
    //

#if defined(BURGER_MSDOS)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_MSDOS is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_DOS4G)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_DOS4G is defined");
    }
#endif

#if defined(BURGER_X32)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_X32 is defined");
    }
#endif

    //
    // Handle Windows
    //

#if defined(BURGER_WIN32)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_WIN32 is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_WIN64)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_WIN64 is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_WINDOWS)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_WINDOWS is defined");
    }
#endif

    //
    // Dead platforms
    //

#if defined(BURGER_BEOS)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_BEOS is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_NGAGE)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_NGAGE is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_SYMBIAN)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_SYMBIAN is defined");
    }
    ++uTest;
#endif

    //
    // MacOS
    //

#if defined(BURGER_MAC)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_MAC is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_MACOSX)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_MACOSX is defined");
    }
    ++uTest;
#endif

    if (uVerbose & VERBOSE_MACROS) {

#if defined(BURGER_MACOS)
        Message("BURGER_MACOS is defined");
#endif

#if defined(BURGER_MACCLASSIC)
        Message("BURGER_MACCLASSIC is defined");
#endif

#if defined(BURGER_MACCARBON)
        Message("BURGER_MACCARBON is defined");
#endif

#if defined(TARGET_API_MAC_CARBON)
        Message("TARGET_API_MAC_CARBON is defined");
#endif

#if defined(BURGER_CFM)
        Message("BURGER_CFM is defined");
#endif
    }

    //
    // iOS
    //
#if defined(BURGER_IOS)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_IOS is defined");
    }
    ++uTest;
#endif

    //
    // Android and their sub platforms
    //
#if defined(BURGER_ANDROID)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_ANDROID is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_OUYA)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_OUYA is defined");
    }
#endif

#if defined(BURGER_SHIELD)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_SHIELD is defined");
    }
#endif

#if defined(BURGER_LINUX)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_LINUX is defined");
    }
    ++uTest;
#endif

    //
    // Video game consoles
    //

#if defined(BURGER_XBOX)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_XBOX is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_XBOX360)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_XBOX360 is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_XBOXONE)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_XBOXONE is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_PS2)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_PS2 is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_PS3)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_PS3 is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_PS4)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_PS4 is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_PSP)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_PSP is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_VITA)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_VITA is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_GAMECUBE)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_GAMECUBE is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_WII)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_WII is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_WIIU)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_WII is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_SWITCH)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_SWITCH is defined");
    }
    ++uTest;
#endif

#if defined(BURGER_DS)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_DS is defined");
    }
    ++uTest;
#endif

    if (uTest >= 2) {
        Message("Multiple Platforms have been defined!");
        uFailure = 10;
    }
    if (!uTest) {
        Message("Unknown Platform detected!");
        uFailure = 10;
    }
    return uFailure;
}

/***************************************

    Perform the tests for the macros and compiler
    settings

***************************************/

int BURGER_API TestMacros(uint_t uVerbose)
{
    uint_t uFailure = 0;

    if (uVerbose & VERBOSE_MSG) {
        Message("Testing macros");
    }

    ShowCompilerMacros(uVerbose);
    ShowMacros(uVerbose);

    uFailure = TestEndianMacros();
    uFailure |= TestDebugMacros(uVerbose);
    uFailure |= TestCompilerMacros(uVerbose);
    uFailure |= TestCPUMacros(uVerbose);
    uFailure |= TestPlatformMacros(uVerbose);

    return static_cast<int>(uFailure);
}
