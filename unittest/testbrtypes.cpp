/***************************************

    Determine which compiler is being used and create standardized typedefs and
    macros so generic code can be created cross platform

    Copyright (c) 1995-2019 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "testbrtypes.h"
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
#pragma warning( \
    disable : 4003) // Disable not enough actual parameters for macro
#pragma warning( \
    disable : 4324) // Disable structure was padded due to __declspec(align())
#endif

/***************************************

    Print the contents of these macros so they
    can be manually checked for correctness

***************************************/

static void BURGER_API ShowCompilerMacros(uint_t uVerbose)
{
    if (uVerbose & VERBOSE_MACROS) {

        Message("Compiler macros");

        // Print known C++ compiler flags
#if defined(__STDC__)
        Message(
            "__STDC__ is defined as \"" BURGER_MACRO_TO_STRING(__STDC__) "\"");
#else
        Message("__STDC__ is not defined");
#endif

#if defined(__STDC_VERSION__)
        Message("__STDC_VERSION__ is defined as \"" BURGER_MACRO_TO_STRING(
            __STDC_VERSION__) "\"");
#else
        Message("__STDC_VERSION__ is not defined");
#endif

#if defined(__cplusplus)
        Message("__cplusplus is defined as \"" BURGER_MACRO_TO_STRING(
            __cplusplus) "\"");
#else
        Message("__cplusplus is not defined");
#endif

#if defined(__cplusplus_cli)
        Message("__cplusplus_cli is defined as \"" BURGER_MACRO_TO_STRING(
            __cplusplus_cli) "\"");
#else
        Message("__cplusplus_cli is not defined");
#endif

#if defined(__embedded_cplusplus)
        Message("__embedded_cplusplus is defined as \"" BURGER_MACRO_TO_STRING(
            __embedded_cplusplus) "\"");
#else
        Message("__embedded_cplusplus is not defined");
#endif

        // Unix
#if defined(unix)
        Message("unix is defined as \"" BURGER_MACRO_TO_STRING(unix) "\"");
#else
        Message("unix is not defined");
#endif

#if defined(__unix__)
        Message(
            "__unix__ is defined as \"" BURGER_MACRO_TO_STRING(__unix__) "\"");
#else
        Message("__unix__ is not defined");
#endif

#if defined(__unix)
        Message("__unix is defined as \"" BURGER_MACRO_TO_STRING(__unix) "\"");
#else
        Message("__unix is not defined");
#endif

        // Posix
#if defined(_POSIX_VERSION)
        Message("_POSIX_VERSION is defined as \"" BURGER_MACRO_TO_STRING(
            _POSIX_VERSION) "\"");
#else
        Message("_POSIX_VERSION is not defined");
#endif

#if defined(_POSIX2_C_VERSION)
        Message("_POSIX2_C_VERSION is defined as \"" BURGER_MACRO_TO_STRING(
            _POSIX2_C_VERSION) "\"");
#else
        Message("_POSIX2_C_VERSION is not defined");
#endif

        // X/Open
#if defined(_XOPEN_VERSION)
        Message("_XOPEN_VERSION is defined as \"" BURGER_MACRO_TO_STRING(
            _XOPEN_VERSION) "\"");
#else
        Message("_XOPEN_VERSION is not defined");
#endif

#if defined(_XOPEN_UNIX)
        Message("_XOPEN_UNIX is defined as \"" BURGER_MACRO_TO_STRING(
            _XOPEN_UNIX) "\"");
#else
        Message("_XOPEN_UNIX is not defined");
#endif

        // Linux Standards base
#if defined(__LSB_VERSION__)
        Message("__LSB_VERSION__ is defined as \"" BURGER_MACRO_TO_STRING(
            __LSB_VERSION__) "\"");
#else
        Message("__LSB_VERSION__ is not defined");
#endif

        // Data models
#if defined(_ILP32)
        Message("_ILP32 is defined as \"" BURGER_MACRO_TO_STRING(_ILP32) "\"");
#else
        Message("_ILP32 is not defined");
#endif

#if defined(__ILP32__)
        Message("__ILP32__ is defined as \"" BURGER_MACRO_TO_STRING(
            __ILP32__) "\"");
#else
        Message("__ILP32__ is not defined");
#endif

#if defined(_LP64)
        Message("_LP64 is defined as \"" BURGER_MACRO_TO_STRING(_LP64) "\"");
#else
        Message("_LP64 is not defined");
#endif

#if defined(__LP64__)
        Message(
            "__LP64__ is defined as \"" BURGER_MACRO_TO_STRING(__LP64__) "\"");
#else
        Message("__LP64__ is not defined");
#endif

        // Operating system macros

        // IBM AIX
#if defined(_AIX)
        Message("_AIX is defined as \"" BURGER_MACRO_TO_STRING(_AIX) "\"");
#else
        Message("_AIX is not defined");
#endif

#if defined(__TOS_AIX__)
        Message("__TOS_AIX__ is defined as \"" BURGER_MACRO_TO_STRING(
            __TOS_AIX__) "\"");
#else
        Message("__TOS_AIX__ is not defined");
#endif

        // Android
#if defined(__ANDROID__)
        Message("__ANDROID__ is defined as \"" BURGER_MACRO_TO_STRING(
            __ANDROID__) "\"");
#else
        Message("__ANDROID__ is not defined");
#endif

#if defined(__ANDROID_API__)
        Message("__ANDROID_API__ is defined as \"" BURGER_MACRO_TO_STRING(
            __ANDROID_API__) "\"");
#else
        Message("__ANDROID_API__ is not defined");
#endif

        // Commodore Amiga
#if defined(AMIGA)
        Message("AMIGA is defined as \"" BURGER_MACRO_TO_STRING(AMIGA) "\"");
#else
        Message("AMIGA is not defined");
#endif

#if defined(__amigaos__)
        Message("__amigaos__ is defined as \"" BURGER_MACRO_TO_STRING(
            __amigaos__) "\"");
#else
        Message("__amigaos__ is not defined");
#endif

        // BeOS
#if defined(__BEOS__)
        Message(
            "__BEOS__ is defined as \"" BURGER_MACRO_TO_STRING(__BEOS__) "\"");
#else
        Message("__BEOS__ is not defined");
#endif

#if defined(__be_os)
        Message(
            "__be_os is defined as \"" BURGER_MACRO_TO_STRING(__be_os) "\"");
#else
        Message("__be_os is not defined");
#endif

        // Berkeley Software Distribution (BSD)
#if defined(BSD)
        Message("BSD is defined as \"" BURGER_MACRO_TO_STRING(BSD) "\"");
#else
        Message("BSD is not defined");
#endif

#if defined(__bsdi__)
        Message(
            "__bsdi__ is defined as \"" BURGER_MACRO_TO_STRING(__bsdi__) "\"");
#else
        Message("__bsdi__ is not defined");
#endif

#if defined(__DragonFly__)
        Message("__DragonFly__ is defined as \"" BURGER_MACRO_TO_STRING(
            __DragonFly__) "\"");
#else
        Message("__DragonFly__ is not defined");
#endif

#if defined(__FreeBSD__)
        Message("__FreeBSD__ is defined as \"" BURGER_MACRO_TO_STRING(
            __FreeBSD__) "\"");
#else
        Message("__FreeBSD__ is not defined");
#endif

#if defined(__FreeBSD_kernel__)
        Message("__FreeBSD_kernel__ is defined as \"" BURGER_MACRO_TO_STRING(
            __FreeBSD_kernel__) "\"");
#else
        Message("__FreeBSD_kernel__ is not defined");
#endif

#if defined(__NetBSD__)
        Message("__NetBSD__ is defined as \"" BURGER_MACRO_TO_STRING(
            __NetBSD__) "\"");
#else
        Message("__NetBSD__ is not defined");
#endif

#if defined(__OpenBSD__)
        Message("__OpenBSD__ is defined as \"" BURGER_MACRO_TO_STRING(
            __OpenBSD__) "\"");
#else
        Message("__OpenBSD__ is not defined");
#endif

#if defined(_SYSTYPE_BSD)
        Message("_SYSTYPE_BSD is defined as \"" BURGER_MACRO_TO_STRING(
            _SYSTYPE_BSD) "\"");
#else
        Message("_SYSTYPE_BSD is not defined");
#endif

        // Cygwin
#if defined(__CYGWIN__)
        Message("__CYGWIN__ is defined as \"" BURGER_MACRO_TO_STRING(
            __CYGWIN__) "\"");
#else
        Message("__CYGWIN__ is not defined");
#endif

        // Linux
#if defined(linux)
        Message("linux is defined as \"" BURGER_MACRO_TO_STRING(linux) "\"");
#else
        Message("linux is not defined");
#endif

#if defined(__linux)
        Message(
            "__linux is defined as \"" BURGER_MACRO_TO_STRING(__linux) "\"");
#else
        Message("__linux is not defined");
#endif

#if defined(__linux__)
        Message("__linux__ is defined as \"" BURGER_MACRO_TO_STRING(
            __linux__) "\"");
#else
        Message("__linux__ is not defined");
#endif

#if defined(__gnu_linux__)
        Message("__gnu_linux__ is defined as \"" BURGER_MACRO_TO_STRING(
            __gnu_linux__) "\"");
#else
        Message("__gnu_linux__ is not defined");
#endif

        // Macintosh
#if defined(macintosh)
        Message("macintosh is defined as \"" BURGER_MACRO_TO_STRING(
            macintosh) "\"");
#else
        Message("macintosh is not defined");
#endif

#if defined(Macintosh)
        Message("Macintosh is defined as \"" BURGER_MACRO_TO_STRING(
            Macintosh) "\"");
#else
        Message("Macintosh is not defined");
#endif

#if defined(__APPLE__)
        Message("__APPLE__ is defined as \"" BURGER_MACRO_TO_STRING(
            __APPLE__) "\"");
#else
        Message("__APPLE__ is not defined");
#endif

#if defined(__MACH__)
        Message(
            "__MACH__ is defined as \"" BURGER_MACRO_TO_STRING(__MACH__) "\"");
#else
        Message("__MACH__ is not defined");
#endif

#if defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
        Message(
            "__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ is defined as \"" BURGER_MACRO_TO_STRING(
                __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) "\"");
#else
        Message(
            "__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ is not defined");
#endif

#if defined(__CFM68K)
        Message(
            "__CFM68K is defined as \"" BURGER_MACRO_TO_STRING(__CFM68K) "\"");
#else
        Message("__CFM68K is not defined");
#endif

#if defined(__CFM68K__)
        Message("__CFM68K__ is defined as \"" BURGER_MACRO_TO_STRING(
            __CFM68K__) "\"");
#else
        Message("__CFM68K__ is not defined");
#endif

        // Morph OS
#if defined(__MORPHOS__)
        Message("__MORPHOS__ is defined as \"" BURGER_MACRO_TO_STRING(
            __MORPHOS__) "\"");
#else
        Message("__MORPHOS__ is not defined");
#endif

        // MSDOS
#if defined(MSDOS)
        Message("MSDOS is defined as \"" BURGER_MACRO_TO_STRING(MSDOS) "\"");
#else
        Message("MSDOS is not defined");
#endif

#if defined(__DOS__)
        Message(
            "__DOS__ is defined as \"" BURGER_MACRO_TO_STRING(__DOS__) "\"");
#else
        Message("__DOS__ is not defined");
#endif

#if defined(__MSDOS__)
        Message("__MSDOS__ is defined as \"" BURGER_MACRO_TO_STRING(
            __MSDOS__) "\"");
#else
        Message("__MSDOS__ is not defined");
#endif

#if defined(_MSDOS)
        Message("_MSDOS is defined as \"" BURGER_MACRO_TO_STRING(_MSDOS) "\"");
#else
        Message("_MSDOS is not defined");
#endif

#if defined(__DOS4G__)
        Message("__DOS4G__ is defined as \"" BURGER_MACRO_TO_STRING(
            __DOS4G__) "\"");
#else
        Message("__DOS4G__ is not defined");
#endif

#if defined(__X32__)
        Message(
            "__X32__ is defined as \"" BURGER_MACRO_TO_STRING(__X32__) "\"");
#else
        Message("__X32__ is not defined");
#endif

        // Solaris
#if defined(sun)
        Message("sun is defined as \"" BURGER_MACRO_TO_STRING(sun) "\"");
#else
        Message("sun is not defined");
#endif

#if defined(__sun)
        Message("__sun is defined as \"" BURGER_MACRO_TO_STRING(__sun) "\"");
#else
        Message("__sun is not defined");
#endif

        // Windows
#if defined(_WIN16)
        Message("_WIN16 is defined as \"" BURGER_MACRO_TO_STRING(_WIN16) "\"");
#else
        Message("_WIN16 is not defined");
#endif

#if defined(_WIN32)
        Message("_WIN32 is defined as \"" BURGER_MACRO_TO_STRING(_WIN32) "\"");
#else
        Message("_WIN32 is not defined");
#endif

#if defined(_WIN64)
        Message("_WIN64 is defined as \"" BURGER_MACRO_TO_STRING(_WIN64) "\"");
#else
        Message("_WIN64 is not defined");
#endif

#if defined(__WIN32__)
        Message("__WIN32__ is defined as \"" BURGER_MACRO_TO_STRING(
            __WIN32__) "\"");
#else
        Message("__WIN32__ is not defined");
#endif

#if defined(__WINDOWS__)
        Message("__WINDOWS__ is defined as \"" BURGER_MACRO_TO_STRING(
            __WINDOWS__) "\"");
#else
        Message("__WINDOWS__ is not defined");
#endif

#if defined(__NT__)
        Message("__NT__ is defined as \"" BURGER_MACRO_TO_STRING(__NT__) "\"");
#else
        Message("__NT__ is not defined");
#endif

#if defined(__TOS_WIN__)
        Message("__TOS_WIN__ is defined as \"" BURGER_MACRO_TO_STRING(
            __TOS_WIN__) "\"");
#else
        Message("__TOS_WIN__ is not defined");
#endif

#if defined(_WIN32_WCE)
        Message("_WIN32_WCE is defined as \"" BURGER_MACRO_TO_STRING(
            _WIN32_WCE) "\"");
#else
        Message("_WIN32_WCE is not defined");
#endif

        // Sony platforms
#if defined(SN_TARGET_PS2)
        Message("SN_TARGET_PS2 is defined as \"" BURGER_MACRO_TO_STRING(
            SN_TARGET_PS2) "\"");
#else
        Message("SN_TARGET_PS2 is not defined");
#endif

#if defined(__CELLOS_LV2__)
        Message("__CELLOS_LV2__ is defined as \"" BURGER_MACRO_TO_STRING(
            __CELLOS_LV2__) "\"");
#else
        Message("__CELLOS_LV2__ is not defined");
#endif

#if defined(__ORBIS__)
        Message("__ORBIS__ is defined as \"" BURGER_MACRO_TO_STRING(
            __ORBIS__) "\"");
#else
        Message("__ORBIS__ is not defined");
#endif

#if defined(SN_TARGET_PSP)
        Message("SN_TARGET_PSP is defined as \"" BURGER_MACRO_TO_STRING(
            SN_TARGET_PSP) "\"");
#else
        Message("SN_TARGET_PSP is not defined");
#endif

#if defined(__psp2__)
        Message(
            "__psp2__ is defined as \"" BURGER_MACRO_TO_STRING(__psp2__) "\"");
#else
        Message("__psp2__ is not defined");
#endif

        // Xbox
#if defined(_XBOX)
        Message("_XBOX is defined as \"" BURGER_MACRO_TO_STRING(_XBOX) "\"");
#else
        Message("_XBOX is not defined");
#endif

#if defined(XBOX)
        Message("XBOX is defined as \"" BURGER_MACRO_TO_STRING(XBOX) "\"");
#else
        Message("XBOX is not defined");
#endif

        // Nintendo
#if defined(SDK_ARM9)
        Message(
            "SDK_ARM9 is defined as \"" BURGER_MACRO_TO_STRING(SDK_ARM9) "\"");
#else
        Message("SDK_ARM9 is not defined");
#endif

#if defined(SDK_TS)
        Message("SDK_TS is defined as \"" BURGER_MACRO_TO_STRING(SDK_TS) "\"");
#else
        Message("SDK_TS is not defined");
#endif

#if defined(RVL_SDK)
        Message(
            "RVL_SDK is defined as \"" BURGER_MACRO_TO_STRING(RVL_SDK) "\"");
#else
        Message("RVL_SDK is not defined");
#endif

        // Print known C++ compiler version identifiers
#if defined(__clang__)
        Message("__clang__ is defined as \"" BURGER_MACRO_TO_STRING(
            __clang__) "\"");
#else
        Message("__clang__ is not defined");
#endif

#if defined(__ghs__)
        Message(
            "__ghs__ is defined as \"" BURGER_MACRO_TO_STRING(__ghs__) "\"");
#else
        Message("__ghs__ is not defined");
#endif

#if defined(__llvm__)
        Message(
            "__llvm__ is defined as \"" BURGER_MACRO_TO_STRING(__llvm__) "\"");
#else
        Message("__llvm__ is not defined");
#endif

#if defined(__xlc__)
        Message(
            "__xlc__ is defined as \"" BURGER_MACRO_TO_STRING(__xlc__) "\"");
#else
        Message("__xlc__ is not defined");
#endif

#if defined(__APPLE_CPP__)
        Message("__APPLE_CPP__ is defined as \"" BURGER_MACRO_TO_STRING(
            __APPLE_CPP__) "\"");
#else
        Message("__APPLE_CPP__ is not defined");
#endif

#if defined(__APPLE_CC__)
        Message("__APPLE_CC__ is defined as \"" BURGER_MACRO_TO_STRING(
            __APPLE_CC__) "\"");
#else
        Message("__APPLE_CC__ is not defined");
#endif

#if defined(__NEXT_CPP__)
        Message("__NEXT_CPP__ is defined as \"" BURGER_MACRO_TO_STRING(
            __NEXT_CPP__) "\"");
#else
        Message("__NEXT_CPP__ is not defined");
#endif

#if defined(__BORLANDC__)
        Message("__BORLANDC__ is defined as \"" BURGER_MACRO_TO_STRING(
            __BORLANDC__) "\"");
#else
        Message("__BORLANDC__ is not defined");
#endif

#if defined(__CC65__)
        Message(
            "__CC65__ is defined as \"" BURGER_MACRO_TO_STRING(__CC65__) "\"");
#else
        Message("__CC65__ is not defined");
#endif

#if defined(__DJGPP__)
        Message("__DJGPP__ is defined as \"" BURGER_MACRO_TO_STRING(
            __DJGPP__) "\"");
#else
        Message("__DJGPP__ is not defined");
#endif

#if defined(__GNUC__)
        Message(
            "__GNUC__ is defined as \"" BURGER_MACRO_TO_STRING(__GNUC__) "\"");
#else
        Message("__GNUC__ is not defined");
#endif

#if defined(__INTEL_COMPILER)
        Message("__INTEL_COMPILER is defined as \"" BURGER_MACRO_TO_STRING(
            __INTEL_COMPILER) "\"");
#else
        Message("__INTEL_COMPILER is not defined");
#endif

#if defined(__ICL)
        Message("__ICL is defined as \"" BURGER_MACRO_TO_STRING(__ICL) "\"");
#else
        Message("__ICL is not defined");
#endif

#if defined(__LCC__)
        Message(
            "__LCC__ is defined as \"" BURGER_MACRO_TO_STRING(__LCC__) "\"");
#else
        Message("__LCC__ is not defined");
#endif

#if defined(__MWERKS__)
        Message("__MWERKS__ is defined as \"" BURGER_MACRO_TO_STRING(
            __MWERKS__) "\"");
#else
        Message("__MWERKS__ is not defined");
#endif

#if defined(__CWCC__)
        Message(
            "__CWCC__ is defined as \"" BURGER_MACRO_TO_STRING(__CWCC__) "\"");
#else
        Message("__CWCC__ is not defined");
#endif

#if defined(_MSC_VER)
        Message(
            "_MSC_VER is defined as \"" BURGER_MACRO_TO_STRING(_MSC_VER) "\"");
#else
        Message("_MSC_VER is not defined");
#endif

#if defined(_MSVC_LANG)
        Message("_MSVC_LANG is defined as \"" BURGER_MACRO_TO_STRING(
            _MSVC_LANG) "\"");
#else
        Message("_MSVC_LANG is not defined");
#endif

#if defined(__MINGW32__)
        Message("__MINGW32__ is defined as \"" BURGER_MACRO_TO_STRING(
            __MINGW32__) "\"");
#else
        Message("__MINGW32__ is not defined");
#endif

#if defined(__MINGW64__)
        Message("__MINGW64__ is defined as \"" BURGER_MACRO_TO_STRING(
            __MINGW64__) "\"");
#else
        Message("__MINGW64__ is not defined");
#endif

#if defined(__MRC__)
        Message(
            "__MRC__ is defined as \"" BURGER_MACRO_TO_STRING(__MRC__) "\"");
#else
        Message("__MRC__ is not defined");
#endif

#if defined(__CC_NORCROFT)
        Message("__CC_NORCROFT is defined as \"" BURGER_MACRO_TO_STRING(
            __CC_NORCROFT) "\"");
#else
        Message("__CC_NORCROFT is not defined");
#endif

#if defined(__SC__)
        Message("__SC__ is defined as \"" BURGER_MACRO_TO_STRING(__SC__) "\"");
#else
        Message("__SC__ is not defined");
#endif

#if defined(MPW_CPLUS)
        Message("MPW_CPLUS is defined as \"" BURGER_MACRO_TO_STRING(
            MPW_CPLUS) "\"");
#else
        Message("MPW_CPLUS is not defined");
#endif

#if defined(MPW_C)
        Message("MPW_C is defined as \"" BURGER_MACRO_TO_STRING(MPW_C) "\"");
#else
        Message("MPW_C is not defined");
#endif

#if defined(__SN__)
        Message("__SN__ is defined as \"" BURGER_MACRO_TO_STRING(__SN__) "\"");
#else
        Message("__SN__ is not defined");
#endif

#if defined(__SNC__)
        Message(
            "__SNC__ is defined as \"" BURGER_MACRO_TO_STRING(__SNC__) "\"");
#else
        Message("__SNC__ is not defined");
#endif

#if defined(__TINYC__)
        Message("__TINYC__ is defined as \"" BURGER_MACRO_TO_STRING(
            __TINYC__) "\"");
#else
        Message("__TINYC__ is not defined");
#endif

#if defined(__TURBOC__)
        Message("__TURBOC__ is defined as \"" BURGER_MACRO_TO_STRING(
            __TURBOC__) "\"");
#else
        Message("__TURBOC__ is not defined");
#endif

#if defined(__WATCOMC__)
        Message("__WATCOMC__ is defined as \"" BURGER_MACRO_TO_STRING(
            __WATCOMC__) "\"");
#else
        Message("__WATCOMC__ is not defined");
#endif

#if defined(__ZTC__)
        Message(
            "__ZTC__ is defined as \"" BURGER_MACRO_TO_STRING(__ZTC__) "\"");
#else
        Message("__ZTC__ is not defined");
#endif

        // Print known CPU identifiers
#if defined(__R5900__)
        Message("__R5900__ is defined as \"" BURGER_MACRO_TO_STRING(
            __R5900__) "\"");
#else
        Message("__R5900__ is not defined");
#endif

#if defined(__R5900)
        Message(
            "__R5900 is defined as \"" BURGER_MACRO_TO_STRING(__R5900) "\"");
#else
        Message("__R5900 is not defined");
#endif

#if defined(__MIPS__)
        Message(
            "__MIPS__ is defined as \"" BURGER_MACRO_TO_STRING(__MIPS__) "\"");
#else
        Message("__MIPS__ is not defined");
#endif

#if defined(__mips__)
        Message(
            "__mips__ is defined as \"" BURGER_MACRO_TO_STRING(__mips__) "\"");
#else
        Message("__mips__ is not defined");
#endif

#if defined(__PPC__)
        Message(
            "__PPC__ is defined as \"" BURGER_MACRO_TO_STRING(__PPC__) "\"");
#else
        Message("__PPC__ is not defined");
#endif

#if defined(__PPCGEKKO__)
        Message("__PPCGEKKO__ is defined as \"" BURGER_MACRO_TO_STRING(
            __PPCGEKKO__) "\"");
#else
        Message("__PPCGEKKO__ is not defined");
#endif

#if defined(__PPCBROADWAY__)
        Message("__PPCBROADWAY__ is defined as \"" BURGER_MACRO_TO_STRING(
            __PPCBROADWAY__) "\"");
#else
        Message("__PPCBROADWAY__ is not defined");
#endif

#if defined(_M_MPPC)
        Message(
            "_M_MPPC is defined as \"" BURGER_MACRO_TO_STRING(_M_MPPC) "\"");
#else
        Message("_M_MPPC is not defined");
#endif

#if defined(__ppc__)
        Message(
            "__ppc__ is defined as \"" BURGER_MACRO_TO_STRING(__ppc__) "\"");
#else
        Message("__ppc__ is not defined");
#endif

#if defined(__ppc64__)
        Message("__ppc64__ is defined as \"" BURGER_MACRO_TO_STRING(
            __ppc64__) "\"");
#else
        Message("__ppc64__ is not defined");
#endif

#if defined(__espresso__)
        Message("__espresso__ is defined as \"" BURGER_MACRO_TO_STRING(
            __espresso__) "\"");
#else
        Message("__espresso__ is not defined");
#endif

#if defined(__ALTIVEC__)
        Message("__ALTIVEC__ is defined as \"" BURGER_MACRO_TO_STRING(
            __ALTIVEC__) "\"");
#else
        Message("__ALTIVEC__ is not defined");
#endif

#if defined(_M_IX86)
        Message(
            "_M_IX86 is defined as \"" BURGER_MACRO_TO_STRING(_M_IX86) "\"");
#else
        Message("_M_IX86 is not defined");
#endif

#if defined(__i386__)
        Message(
            "__i386__ is defined as \"" BURGER_MACRO_TO_STRING(__i386__) "\"");
#else
        Message("__i386__ is not defined");
#endif

#if defined(_M_AMD64)
        Message(
            "_M_AMD64 is defined as \"" BURGER_MACRO_TO_STRING(_M_AMD64) "\"");
#else
        Message("_M_AMD64 is not defined");
#endif

#if defined(_M_X64)
        Message("_M_X64 is defined as \"" BURGER_MACRO_TO_STRING(_M_X64) "\"");
#else
        Message("_M_X64 is not defined");
#endif

#if defined(__x86_64__)
        Message("__x86_64__ is defined as \"" BURGER_MACRO_TO_STRING(
            __x86_64__) "\"");
#else
        Message("__x86_64__ is not defined");
#endif

#if defined(__SSE__)
        Message(
            "__SSE__ is defined as \"" BURGER_MACRO_TO_STRING(__SSE__) "\"");
#else
        Message("__SSE__ is not defined");
#endif

#if defined(__SSE2__)
        Message(
            "__SSE2__ is defined as \"" BURGER_MACRO_TO_STRING(__SSE2__) "\"");
#else
        Message("__SSE2__ is not defined");
#endif

#if defined(__AVX__)
        Message(
            "__AVX__ is defined as \"" BURGER_MACRO_TO_STRING(__AVX__) "\"");
#else
        Message("__AVX__ is not defined");
#endif

#if defined(__AVX2__)
        Message(
            "__AVX2__ is defined as \"" BURGER_MACRO_TO_STRING(__AVX2__) "\"");
#else
        Message("__AVX2__ is not defined");
#endif

#if defined(_M_IA64)
        Message(
            "_M_IA64 is defined as \"" BURGER_MACRO_TO_STRING(_M_IA64) "\"");
#else
        Message("_M_IA64 is not defined");
#endif

#if defined(_M_MRX000)
        Message("_M_MRX000 is defined as \"" BURGER_MACRO_TO_STRING(
            _M_MRX000) "\"");
#else
        Message("_M_MRX000 is not defined");
#endif

#if defined(_M_M68K)
        Message(
            "_M_M68K is defined as \"" BURGER_MACRO_TO_STRING(_M_M68K) "\"");
#else
        Message("_M_M68K is not defined");
#endif

#if defined(__MC68K__)
        Message("__MC68K__ is defined as \"" BURGER_MACRO_TO_STRING(
            __MC68K__) "\"");
#else
        Message("__MC68K__ is not defined");
#endif

#if defined(_68K_)
        Message("_68K_ is defined as \"" BURGER_MACRO_TO_STRING(_68K_) "\"");
#else
        Message("_68K_ is not defined");
#endif

#if defined(__m68k__)
        Message(
            "__m68k__ is defined as \"" BURGER_MACRO_TO_STRING(__m68k__) "\"");
#else
        Message("__m68k__ is not defined");
#endif

#if defined(mc68881)
        Message(
            "mc68881 is defined as \"" BURGER_MACRO_TO_STRING(mc68881) "\"");
#else
        Message("mc68881 is not defined");
#endif

#if defined(__MC68881__)
        Message("__MC68881__ is defined as \"" BURGER_MACRO_TO_STRING(
            __MC68881__) "\"");
#else
        Message("__MC68881__ is not defined");
#endif

#if defined(_M_ARM)
        Message("_M_ARM is defined as \"" BURGER_MACRO_TO_STRING(_M_ARM) "\"");
#else
        Message("_M_ARM is not defined");
#endif

#if defined(__arm__)
        Message(
            "__arm__ is defined as \"" BURGER_MACRO_TO_STRING(__arm__) "\"");
#else
        Message("__arm__ is not defined");
#endif

#if defined(__arm64__)
        Message("__arm64__ is defined as \"" BURGER_MACRO_TO_STRING(
            __arm64__) "\"");
#else
        Message("__arm64__ is not defined");
#endif

#if defined(__ARM_NEON__)
        Message("__ARM_NEON__ is defined as \"" BURGER_MACRO_TO_STRING(
            __ARM_NEON__) "\"");
#else
        Message("__ARM_NEON__ is not defined");
#endif

#if defined(__aarch64__)
        Message("__aarch64__ is defined as \"" BURGER_MACRO_TO_STRING(
            __aarch64__) "\"");
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
        Message("NULL is defined as \"" BURGER_MACRO_TO_STRING(NULL) "\"");
        Message("TRUE is defined as \"" BURGER_MACRO_TO_STRING(TRUE) "\"");
        Message("FALSE is defined as \"" BURGER_MACRO_TO_STRING(FALSE) "\"");

        Message("BURGER_ALIGN is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_ALIGN(x, s)) "\"");
        Message("BURGER_PREALIGN is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_PREALIGN(s)) "\"");
        Message("BURGER_POSTALIGN is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_POSTALIGN(s)) "\"");

        Message("BURGER_API is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_API) "\"");
        Message("BURGER_ANSIAPI is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_ANSIAPI) "\"");
        Message("BURGER_INLINE is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_INLINE) "\"");
        Message("BURGER_DECLSPECNAKED is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_DECLSPECNAKED) "\"");
        Message("BURGER_ASM is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_ASM) "\"");
        Message("BURGER_MAXWORDPTR is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_MAXWORDPTR) "\"");
        Message("BURGER_MININT is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_MININT) "\"");
        Message("BURGER_MAXINT is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_MAXINT) "\"");
        Message("BURGER_MAXUINT is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_MAXUINT) "\"");
        Message("BURGER_MININT64 is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_MININT64) "\"");
        Message("BURGER_MAXINT64 is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_MAXINT64) "\"");
        Message("BURGER_MAXUINT64 is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_MAXUINT64) "\"");

        // 64 bit supported?

#if defined(BURGER_LONGLONG)
        Message("BURGER_LONGLONG is defined as \"" BURGER_MACRO_TO_STRING(
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

#if defined(BURGER_OPENGL_SUPPORTED)
        Message("BURGER_OPENGL_SUPPORTED is defined");
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
        Message("BURGER_CPP89 is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_CPP89) "\"");
#endif

#if defined(BURGER_CPP98)
        Message("BURGER_CPP98 is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_CPP98) "\"");
#endif

#if defined(BURGER_CPP11)
        Message("BURGER_CPP11 is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_CPP11) "\"");
#endif

#if defined(BURGER_CPP14)
        Message("BURGER_CPP14 is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_CPP14) "\"");
#endif

#if defined(BURGER_CPP17)
        Message("BURGER_CPP17 is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_CPP17) "\"");
#endif

#if defined(BURGER_CPP20)
        Message("BURGER_CPP20 is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_CPP20) "\"");
#endif

#if defined(BURGER_GNUC_VERSION)
        Message("BURGER_GNUC_VERSION is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_GNUC_VERSION) "\"");
#endif

#if defined(BURGER_CLANG_VERSION)
        Message("BURGER_CLANG_VERSION is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_CLANG_VERSION) "\"");
#endif

#if defined(BURGER_CONSTEXPR)
        Message("BURGER_CONSTEXPR is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_CONSTEXPR) "\"");
#endif

#if defined(BURGER_OVERRIDE)
        Message("BURGER_OVERRIDE is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_OVERRIDE) "\"");
#endif

#if defined(nullptr)
        Message("nullptr is defined");
#endif

#if defined(BURGER_ENUMSTART)
        Message("BURGER_ENUMSTART is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_ENUMSTART(SampleEnum, uint_t)) "\"");
#endif

#if defined(BURGER_ENUMEND)
        Message("BURGER_ENUMEND is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_ENUMEND(SampleEnum)) "\"");
#endif

        // Disable, the macro is huge
        //#if defined(BURGER_ENUMFLAGSEND)
        //        Message("BURGER_ENUMFLAGSEND is defined as \""
        //        BURGER_MACRO_TO_STRING(
        //            BURGER_ENUMFLAGSEND(SampleEnum, uint_t)) "\"");
        //#endif

#if defined(BURGER_COMPILE_TIME_ASSERT)
        Message(
            "BURGER_COMPILE_TIME_ASSERT is defined as \"" BURGER_MACRO_TO_STRING(
                BURGER_COMPILE_TIME_ASSERT(sizeof(int) == 4)) "\"");
#endif
    }

#if defined(BURGER_WATCOM)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_WATCOM is defined as \"" BURGER_MACRO_TO_STRING(
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
        Message("BURGER_METROWERKS is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_METROWERKS) "\"");
    }
    ++uTest;
#endif

#if defined(BURGER_MSVC)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_MSVC is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_MSVC) "\"");
    }
    ++uTest;
#endif

#if defined(BURGER_GNUC)
    if (uVerbose & VERBOSE_MACROS) {
        Message("BURGER_GNUC is defined");
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
        Message("BURGER_SIZEOF_INT is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_SIZEOF_INT) "\"");
#endif

#if defined(BURGER_SIZEOF_LONG)
        Message("BURGER_SIZEOF_LONG is defined as \"" BURGER_MACRO_TO_STRING(
            BURGER_SIZEOF_LONG) "\"");
#endif

#if defined(BURGER_NEED_UINTPTR_REMAP)
        Message(
            "BURGER_NEED_UINTPTR_REMAP is defined as \"" BURGER_MACRO_TO_STRING(
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

    Test default data chunk sizes

***************************************/

static uint_t BURGER_API TestDataChunkSize(uint_t uVerbose)
{
    if (uVerbose & VERBOSE_MSG) {
        Message("Running data chunk size test tests");
    }

    uint_t uTest = sizeof(uint8_t) != 1;
    uint_t uFailure = uTest;
    ReportFailure("sizeof(uint8_t) is %u instead of 1.", uTest,
        static_cast<uint_t>(sizeof(uint8_t)));

    uTest = sizeof(int8_t) != 1;
    uFailure |= uTest;
    ReportFailure("sizeof(int8_t) is %u instead of 1.", uTest,
        static_cast<uint_t>(sizeof(int8_t)));

    uTest = sizeof(uint16_t) != 2;
    uFailure |= uTest;
    ReportFailure("sizeof(uint16_t) is %u instead of 2.", uTest,
        static_cast<uint_t>(sizeof(uint16_t)));

    uTest = sizeof(int16_t) != 2;
    uFailure |= uTest;
    ReportFailure("sizeof(int16_t) is %u instead of 2.", uTest,
        static_cast<uint_t>(sizeof(int16_t)));

    uTest = sizeof(uint32_t) != 4;
    uFailure |= uTest;
    ReportFailure("sizeof(uint32_t) is %u instead of 4.", uTest,
        static_cast<uint_t>(sizeof(uint32_t)));

    uTest = sizeof(int32_t) != 4;
    uFailure |= uTest;
    ReportFailure("sizeof(int32_t) is %u instead of 4.", uTest,
        static_cast<uint_t>(sizeof(int32_t)));

    uTest = sizeof(uint64_t) != 8;
    uFailure |= uTest;
    ReportFailure("sizeof(uint64_t) is %u instead of 8.", uTest,
        static_cast<uint_t>(sizeof(uint64_t)));

    uTest = sizeof(int64_t) != 8;
    uFailure |= uTest;
    ReportFailure("sizeof(int64_t) is %u instead of 8.", uTest,
        static_cast<uint_t>(sizeof(int64_t)));

    uTest = sizeof(void*) != sizeof(uintptr_t);
    uFailure |= uTest;
    ReportFailure("sizeof(void *) is %u instead of sizeof(uintptr_t).", uTest,
        static_cast<uint_t>(sizeof(void*)));

    uTest = sizeof(Bool) != 1;
    uFailure |= uTest;
    ReportFailure("sizeof(Bool) is %u instead of 1.", uTest,
        static_cast<uint_t>(sizeof(Bool)));

    uTest = sizeof(Frac32) != 4;
    uFailure |= uTest;
    ReportFailure("sizeof(Frac32) is %u instead of 4.", uTest,
        static_cast<uint_t>(sizeof(Frac32)));

    uTest = sizeof(Fixed32) != 4;
    uFailure |= uTest;
    ReportFailure("sizeof(Fixed32) is %u instead of 4.", uTest,
        static_cast<uint_t>(sizeof(Fixed32)));

    uTest = sizeof(uint_t) != 4;
    uFailure |= uTest;
    ReportFailure("sizeof(uint_t) is %u instead of 4.", uTest,
        static_cast<uint_t>(sizeof(uint_t)));

    uTest = sizeof(int_t) != 4;
    uFailure |= uTest;
    ReportFailure("sizeof(int_t) is %u instead of 4.", uTest,
        static_cast<uint_t>(sizeof(int_t)));

    uTest = sizeof(Vector_128) != 16;
    uFailure |= uTest;
    ReportFailure("sizeof(Vector_128) is %u instead of 16.", uTest,
        static_cast<uint_t>(sizeof(Vector_128)));

    //
    // Test BURGER_LONGIS64BIT
    //

#if defined(BURGER_LONGIS64BIT)
    uTest = sizeof(long) != 8;
    uFailure |= uTest;
    ReportFailure(
        "sizeof(long) is %u instead of 8 because BURGER_LONGIS64BIT was defined.",
        uTest, static_cast<uint_t>(sizeof(long)));
#else
    uTest = sizeof(long) != 4;
    uFailure |= uTest;
    ReportFailure(
        "sizeof(long) is %u instead of 4 because BURGER_LONGIS64BIT was not defined.",
        uTest, static_cast<uint_t>(sizeof(long)));
#endif

    uTest = sizeof(long long) != 8;
    uFailure |= uTest;
    ReportFailure("sizeof(long long) is %u instead of 8.", uTest,
        static_cast<uint_t>(sizeof(long long)));

    if ((uVerbose & VERBOSE_MSG) && !uFailure) {
        Message("Data chunk size test passed.");
    }
    return uFailure;
}

/***************************************

    Test structure alignment

***************************************/

static uint_t BURGER_API TestStructureAlignment(uint_t uVerbose)
{

    //
    // Test for native entry alignment of structure elements
    //

    struct NativeAlign_t {
        int8_t m_0;   // Offset 0
        int16_t m_2;  // Offset 2 (Align to short)
        int8_t m_4;   // Offset 4
        int32_t m_8;  // Offset 8 (Align to int32)
        int8_t m_12;  // Offset 12
        int64_t m_16; // Offset 16 (Align to int64 or int32)
        int8_t m_24;  // Offset 24
        int64_t m_32; // Offset 32 (Align to int64 only)
        int8_t m_40;  // Offset 40
        float m_44;   // Offset 44 (Align to int32)
        int8_t m_48;  // Offset 48
        double m_56;  // Offset 56 (Align to int64 only)
    };

    uint_t uTest = BURGER_OFFSETOF(NativeAlign_t, m_0) != 0;
    uint_t uFailure = uTest;
    ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_0) is %u instead of 0.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_0)));

    uTest = BURGER_OFFSETOF(NativeAlign_t, m_2) != 2;
    uFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_2) is %u instead of 2.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_2)));

    uTest = BURGER_OFFSETOF(NativeAlign_t, m_4) != 4;
    uFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_4) is %u instead of 4.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_4)));

    uTest = BURGER_OFFSETOF(NativeAlign_t, m_8) != 8;
    uFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_8) is %u instead of 8.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_8)));

    uTest = BURGER_OFFSETOF(NativeAlign_t, m_12) != 12;
    uFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_12) is %u instead of 12.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_12)));

    uTest = BURGER_OFFSETOF(NativeAlign_t, m_16) != 16;
    uFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_16) is %u instead of 16.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_16)));

    uTest = BURGER_OFFSETOF(NativeAlign_t, m_24) != 24;
    uFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_24) is %u instead of 24.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_24)));

    uTest = BURGER_OFFSETOF(NativeAlign_t, m_32) != 32;
    uFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_32) is %u instead of 32.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_32)));

    uTest = BURGER_OFFSETOF(NativeAlign_t, m_40) != 40;
    uFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_40) is %u instead of 40.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_40)));

    uTest = BURGER_OFFSETOF(NativeAlign_t, m_44) != 44;
    uFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_44) is %u instead of 44.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_44)));

    uTest = BURGER_OFFSETOF(NativeAlign_t, m_48) != 48;
    uFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_48) is %u instead of 48.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_48)));

    uTest = BURGER_OFFSETOF(NativeAlign_t, m_56) != 56;
    uFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(NativeAlign_t,56) is %u instead of 56.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_56)));

    //
    // Test for pointer alignment
    // Alignment is to match the value found in sizeof(WordPtr)
    // since pointers could be 4 or 8 bytes in size depending on CPU / platform
    //

    struct PointerAlign_t {
        int8_t m_0;    // Offset 0
        int8_t* m_ptr; // Offset 4 or 8 (Align to WordPtr)
    };

    uTest = BURGER_OFFSETOF(PointerAlign_t, m_0) != 0;
    uFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(PointerAlign_t,m_0) is %u instead of 0.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(PointerAlign_t, m_0)));

    uTest = BURGER_OFFSETOF(PointerAlign_t, m_ptr) != sizeof(WordPtr);
    uFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(PointerAlign_t,m_ptr) is %u instead of %u.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(PointerAlign_t, m_ptr)),
        static_cast<uint_t>(sizeof(WordPtr)));

    //
    // Test the BURGER_ALIGN() macro for alignment overrides
    //

    struct ElementAlign_t {
        int8_t m_0;                    // Offset 0
        int8_t BURGER_ALIGN(m_2, 2);   // Offset 2
        int8_t m_3;                    // Offset 3
        int8_t m_4;                    // Offset 4
        int8_t BURGER_ALIGN(m_8, 4);   // Offset 8
        int8_t m_9;                    // Offset 9
        int8_t BURGER_ALIGN(m_16, 8);  // Offset 16
        int8_t m_17;                   // Offset 17
        int8_t BURGER_ALIGN(m_32, 16); // Offset 32
    };

    uTest = BURGER_OFFSETOF(ElementAlign_t, m_0) != 0;
    uint_t uAlignFailure = uTest;
    ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_0) is %u instead of 0.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_0)));

    uTest = BURGER_OFFSETOF(ElementAlign_t, m_2) != 2;
    uAlignFailure |= uTest;
    ReportFailure("BURGER_ALIGN(m_2,2) is %u instead of 2.", uTest,
        static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_2)));

    uTest = BURGER_OFFSETOF(ElementAlign_t, m_3) != 3;
    uAlignFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_3) is %u instead of 3.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_3)));

    uTest = BURGER_OFFSETOF(ElementAlign_t, m_4) != 4;
    uAlignFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_4) is %u instead of 4.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_4)));

    uTest = BURGER_OFFSETOF(ElementAlign_t, m_8) != 8;
    uAlignFailure |= uTest;
    ReportFailure("BURGER_ALIGN(m_8,4) is %u instead of 8.", uTest,
        static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_8)));

    uTest = BURGER_OFFSETOF(ElementAlign_t, m_9) != 9;
    uAlignFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_9) is %u instead of 9.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_9)));

    uTest = BURGER_OFFSETOF(ElementAlign_t, m_16) != 16;
    uAlignFailure |= uTest;
    ReportFailure("BURGER_ALIGN(m_16,8) is %u instead of 16.", uTest,
        static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_16)));

    uTest = BURGER_OFFSETOF(ElementAlign_t, m_17) != 17;
    uAlignFailure |= uTest;
    ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_17) is %u instead of 17.",
        uTest, static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_17)));

    uTest = BURGER_OFFSETOF(ElementAlign_t, m_32) != 32;
    uAlignFailure |= uTest;
    ReportFailure("BURGER_ALIGN(m_32,16) is %u instead of 32.", uTest,
        static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_32)));

    if (uAlignFailure) {
        Message("FAILED BURGER_ALIGN() macro is broken on this compiler.");
    }
    uFailure |= uAlignFailure;

    //
    // Wrap up
    //
    if (!uFailure && (uVerbose & VERBOSE_MSG)) {
        Message("Structure alignment test passed");
    }
    return uFailure;
}

/***************************************

    Show CPU features

***************************************/

static void BURGER_API ShowCPUFeatures(uint_t uVerbose)
{
    // Test for an FPU
    if (uVerbose & VERBOSE_MSG) {
        Message("Burger::HasFPU() = %u", Burger::HasFPU());
    }

    // Test CPUID
    uint_t uCPUIDPresent = Burger::IsCPUIDPresent();
    if (uVerbose & VERBOSE_MSG) {
        Message("Burger::IsCPUIDPresent() = %u", uCPUIDPresent);
    }

    if (uVerbose & VERBOSE_MSG) {
        if (uCPUIDPresent) {

            Burger::CPUID_t MyID;
            Burger::CPUID(&MyID);
            Message("CPUID_t.m_uHighestCPUID = 0x%08X", MyID.m_uHighestCPUID);
            Message("CPUID_t.m_uHighestCPUIDExtended = 0x%08X",
                MyID.m_uHighestCPUIDExtended);
            Message("CPUID_t.m_uHighestCPUIDHyperVisor = 0x%08X",
                MyID.m_uHighestCPUIDHyperVisor);
            Message("CPUID_t.m_uModel = 0x%08X", MyID.m_uModel);
            Message("CPUID_t.m_uBrand = 0x%08X", MyID.m_uBrand);
            Message("CPUID_t.m_uCPUID1ECX = 0x%08X", MyID.m_uCPUID1ECX);
            Message("CPUID_t.m_uCPUID1EDX = 0x%08X", MyID.m_uCPUID1EDX);
            Message("CPUID_t.m_uCPUID80000001ECX = 0x%08X",
                MyID.m_uCPUID80000001ECX);
            Message("CPUID_t.m_uCPUID80000001EDX = 0x%08X",
                MyID.m_uCPUID80000001EDX);
            Message("CPUID_t.m_uCPUID7EBX = 0x%08X", MyID.m_uCPUID7EBX);
            Message("CPUID_t.m_uCPUID7ECX = 0x%08X", MyID.m_uCPUID7ECX);
            Message("CPUID_t.m_uCPUID7EDX = 0x%08X", MyID.m_uCPUID7EDX);
            Message(
                "CPUID_t.m_uCPUType = %u", static_cast<uint_t>(MyID.m_uCPUType));
            Message("CPUID_t.m_CPUName = %s", MyID.m_CPUName);
            Message("CPUID_t.m_BrandName = %s", MyID.m_BrandName);
            Message("CPUID_t.m_HypervisorName = %s", MyID.m_HypervisorName);
            Message("CPUID_t.m_HypervisorSignature = %s",
                MyID.m_HypervisorSignature);

            // Dump the CPU flags detects
            if (MyID.HasRTSC()) {
                Message("HasRTSC");
            }

            if (MyID.HasCMOV()) {
                Message("HasCMOV");
            }

            if (MyID.HasMMX()) {
                Message("HasMMX");
            }
            if (MyID.HasSSE()) {
                Message("HasSSE");
            }
            if (MyID.HasSSE2()) {
                Message("HasSSE2");
            }
            if (MyID.HasSSE3()) {
                Message("HasSSE3");
            }
            if (MyID.HasSSSE3()) {
                Message("HasSSSE3");
            }
            if (MyID.HasSSE4a()) {
                Message("HasSSE4a");
            }
            if (MyID.HasSSE41()) {
                Message("HasSSE41");
            }
            if (MyID.HasSSE42()) {
                Message("HasSSE42");
            }
            if (MyID.HasMOVBE()) {
                Message("HasMOVBE");
            }
            if (MyID.HasAES()) {
                Message("HasAES");
            }
            if (MyID.HasAVX()) {
                Message("HasAVX");
            }
            if (MyID.HasCMPXCHG16B()) {
                Message("HasCMPXCHG16B");
            }
            if (MyID.HasF16C()) {
                Message("HasF16C");
            }
            if (MyID.HasFMA3()) {
                Message("HasFMA3");
            }
            if (MyID.HasFMA4()) {
                Message("HasFMA4");
            }
            if (MyID.HasLAHFSAHF()) {
                Message("HasLAHFSAHF");
            }
            if (MyID.HasPrefetchW()) {
                Message("HasPrefetchW");
            }
            if (MyID.Has3DNOW()) {
                Message("Has3DNOW");
            }
            if (MyID.HasExtended3DNOW()) {
                Message("HasExtended3DNOW");
            }
        } else {

            // CPUID not available on a 32 bit CPU is not a failure,
            // but it is a warning that your CPU is pretty old.

#if defined(BURGER_INTEL)
            Message("Burger::CPUID() is not available");
#endif
        }
    }

    //
    // Display 32 bit Intel special registers
    //

#if defined(BURGER_X86)
    if (uVerbose & VERBOSE_MSG) {
        Burger::e8087Precision uPrecision = Burger::Get8087Precision();
        Message(
            "Burger::Get8087Precision() = %u", static_cast<uint_t>(uPrecision));
        Burger::e8087Rounding uRounding = Burger::Get8087Rounding();
        Message("Burger::Get8087Rounding() = %u", static_cast<uint_t>(uRounding));
    }
#endif

    //
    // Handle AltiVec (MacOS / MacOSX) PowerPC
    //

    if (uVerbose & VERBOSE_MSG) {
        Message("Burger::HasAltiVec() = %u", Burger::HasAltiVec());
        Message("Burger::HasFSqrt() = %u", Burger::HasFSqrt());
    }

    //
    // Handle PowerPC special registers
    //

#if defined(BURGER_POWERPC)
    if (uVerbose & VERBOSE_MSG) {

        Burger::ePowerPCRounding uRounding = Burger::GetPowerPCRounding();
        Message(
            "Burger::GetPowerPCRounding() = %u", static_cast<uint_t>(uRounding));

        Burger::ePowerPCRounding uRounding2 =
            Burger::SetPowerPCRounding(Burger::ROUNDDOWN);
        uRounding2 = Burger::GetPowerPCRounding();
        Message(
            "Burger::GetPowerPCRounding() = %u", static_cast<uint_t>(uRounding2));
        Burger::SetPowerPCRounding(Burger::ROUNDUP);
        uRounding2 = Burger::GetPowerPCRounding();
        Message(
            "Burger::GetPowerPCRounding() = %u", static_cast<uint_t>(uRounding2));
        Burger::SetPowerPCRounding(uRounding);
    }
#endif
}

/***************************************

    Show Platform features

***************************************/

static void BURGER_API ShowPlatformFeatures(uint_t uVerbose)
{
    uint_t uTest = 0;

    BURGER_UNUSED(uVerbose);
    BURGER_UNUSED(uTest);

    //
    // Handle Windows features
    //

#if defined(BURGER_WINDOWS)
    if (uVerbose & VERBOSE_MSG) {

        // Test operating system versions

        uTest = Burger::Windows::IsWin95orWin98();
        Message("Burger::Windows::IsWin95orWin98() = %u", uTest);

        uTest = Burger::Windows::IsWinXPOrGreater();
        Message("Burger::Windows::IsWinXPOrGreater() = %u", uTest);

        uTest = Burger::Windows::IsVistaOrGreater();
        Message("Burger::Windows::IsVistaOrGreater() = %u", uTest);

        uTest = Burger::Windows::IsWin7OrGreater();
        Message("Burger::Windows::IsWin7OrGreater() = %u", uTest);

        uTest = Burger::Windows::IsWin8OrGreater();
        Message("Burger::Windows::IsWin8OrGreater() = %u", uTest);

        uTest = Burger::Windows::IsWin10OrGreater();
        Message("Burger::Windows::IsWin10OrGreater() = %u", uTest);

        // Is 32 bit code running in 64 bit windows?
        uTest = Burger::Globals::IsWindows64Bit();
        Message("Burger::Globals::IsWindows64Bit() = %u", uTest);

        // DirectX versions
        uTest = Burger::Globals::GetDirectXVersion();
        Message("Burger::Globals::GetDirectXVersion() = 0x%04X", uTest);

        uTest = Burger::Windows::IsDirectInputPresent();
        Message("Burger::Windows::IsDirectInputPresent() = %u", uTest);
        uTest = Burger::Windows::IsDirectInput8Present();
        Message("Burger::Windows::IsDirectInput8Present() = %u", uTest);
        uTest = Burger::Windows::IsXInputPresent();
        Message("Burger::Windows::IsXInputPresent() = %u", uTest);

        uTest = Burger::Windows::IsDirectDrawPresent();
        Message("Burger::Windows::IsDirectDrawPresent() = %u", uTest);

        uTest = Burger::Windows::IsD3D9Present();
        Message("Burger::Windows::IsD3D9Present() = %u", uTest);

        uTest = Burger::Windows::IsDirectSoundPresent();
        Message("Burger::Windows::IsDirectSoundPresent() = %u", uTest);

        uTest = Burger::Windows::IsDirectPlayPresent();
        Message("Burger::Windows::IsDirectPlayPresent() = %u", uTest);

        uTest = Burger::Windows::IsXAudio2Present();
        Message("Burger::Windows::IsXAudio2Present() = %u", uTest);

        char QTBuffer[300];

        uTest = Burger::Globals::GetPathToQuickTimeFolder(
            QTBuffer, sizeof(QTBuffer), nullptr);
        Message("Burger::Globals::GetPathToQuickTimeFolder() = %u, %s", uTest,
            QTBuffer);
        uTest =
            Burger::Globals::GetQTSystemDirectoryA(QTBuffer, sizeof(QTBuffer));
        Message("Burger::Globals::GetQTSystemDirectoryA() = %u, %s", uTest,
            QTBuffer);
        uTest = Burger::Globals::GetQTApplicationDirectoryA(
            QTBuffer, sizeof(QTBuffer));
        Message("Burger::Globals::GetQTApplicationDirectoryA() = %u, %s", uTest,
            QTBuffer);
        uTest = Burger::Globals::GetQTExtensionDirectoryA(
            QTBuffer, sizeof(QTBuffer));
        Message("Burger::Globals::GetQTExtensionDirectoryA() = %u, %s", uTest,
            QTBuffer);
        uTest = Burger::Globals::GetQTComponentDirectoryA(
            QTBuffer, sizeof(QTBuffer));
        Message("Burger::Globals::GetQTComponentDirectoryA() = %u, %s", uTest,
            QTBuffer);
    }

#endif

#if defined(BURGER_MACOS) || defined(BURGER_WINDOWS)
    if (uVerbose & VERBOSE_MSG) {
        uTest = Burger::Globals::GetQuickTimeVersion();
        Message("Burger::Globals::GetQuickTimeVersion() = 0x%04X", uTest);
    }
#endif

    //
    // Handle MacOS Version
    //

#if defined(BURGER_MACOS)
    if (uVerbose & VERBOSE_MSG) {
        uTest = Burger::Globals::GetMacOSVersion();
        Message("Burger::Globals::GetMacOSVersion() = %04X", uTest);
    }
#endif

    //
    // Mac OS functions

#if defined(BURGER_MAC)
    // _MemoryDispatch = 0xA05C
    if (uVerbose & VERBOSE_MSG) {
        uTest = Burger::Mac::IsTrapAvailable(0xA05C);
        Message("Burger::Mac::IsTrapAvailable(0xA05C) = %u", uTest);

        uTest = Burger::Mac::IsQuickTimePowerPlugAvailable();
        Message("Burger::Mac::IsQuickTimePowerPlugAvailable() = %u", uTest);

        uTest = Burger::Mac::GetAppleShareVersion();
        Message("Burger::Mac::GetAppleShareVersion() = %04X", uTest);

        uTest = Burger::Mac::GetInputSprocketVersion();
        Message("Burger::Mac::GetInputSprocketVersion() = %04X", uTest);

        uTest = Burger::Mac::GetDrawSprocketVersion();
        Message("Burger::Mac::GetDrawSprocketVersion() = %04X", uTest);

        uTest = Burger::Mac::GetSoundManagerVersion();
        Message("Burger::Mac::GetSoundManagerVersion() = %04X", uTest);
    }
#endif
}

/***************************************

    Perform the tests for the macros and compiler
    settings

***************************************/

int BURGER_API TestBrtypes(uint_t uVerbose)
{
    uint_t uFailure = 0;

    // Display the compiler used to build this tool
    Message("Built with " BURGER_COMPILER_NAME " / " BURGER_STDCPP_TYPE
            " / Version %d / " BURGER_CPU_TYPE,
        BURGER_COMPILER_VERSION);

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
    uFailure |= TestDataChunkSize(uVerbose);
    uFailure |= TestStructureAlignment(uVerbose);

    // Print messages about features found on the platform

    ShowCPUFeatures(uVerbose);
    ShowPlatformFeatures(uVerbose);

    return static_cast<int>(uFailure);
}