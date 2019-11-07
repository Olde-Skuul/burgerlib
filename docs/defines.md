Platform defines {#page_defines}
================

This is the mainstay of all of Burgerlib. You include the file burger.h in your project which will determine the compiler, target, CPU, and system specific features so you can write platform neutral code.

Debug defines, one is required MUST be present and is supplied by the build project. This is not created by default by Burgerlib. If both exist or both are missing, burger.h will force a compile error.

* \ref _DEBUG Debugging asserts are enabled.
* \ref NDEBUG Debugging asserts are disabled.

Compiler level {#sect_cpp_level}
--------------

Compiler C++ level, all are defined up to the feature level present.

* \ref BURGER_CPP89 Compiler is at least C++89 level of features
* \ref BURGER_CPP98 Compiler is at least C++98 level of features
* \ref BURGER_CPP11 Compiler is at least C++11 level of features
* \ref BURGER_CPP14 Compiler is at least C++14 level of features
* \ref BURGER_CPP17 Compiler is at least C++17 level of features
* \ref BURGER_CPP20 Compiler is at least C++20 level of features
* \ref BURGER_STDCPP_NAME String of maximum C++ level

Compiler name {#sect_compiler_name}
-------------

Compiler defines, to determine any compiler specific features are available. Only one macro is defined. All are defined as the compiler version number.

* \ref BURGER_WATCOM Open Watcom
* \ref BURGER_MRC Apple's PowerPC MPW compiler
* \ref BURGER_APPLE_SC Apple's 68000 MPW compiler
* \ref BURGER_DJGPP DJ's GNU compiler
* \ref BURGER_SNSYSTEMS SN System's compiler
* \ref BURGER_GHS Green Hills compiler
* \ref BURGER_INTEL_COMPILER Intel compiler
* \ref BURGER_MINGW MinGW compiler
* \ref BURGER_CLANG clang compiler
* \ref BURGER_GNUC GNU C, found on Linux and other platforms
* \ref BURGER_METROWERKS Metrowerks Codewarrior (PowerPC, Intel, 68000)
* \ref BURGER_ARM_COMPILER Advanced RISC Machines ARM compiler
* \ref BURGER_MSVC Microsoft Visual Studio
* \ref BURGER_COMPILER_NAME String of the compiler's name
* \ref BURGER_COMPILER_VERSION Integer of the version of the compiler

Feature Detectors {#sect_detectors}
-----------------

* \ref __has_builtin Macro for non-clang compilers
* \ref __has_feature Macro for non-clang compilers
* \ref __has_extension Macro for non-clang compilers
* \ref __has_cpp_attribute Macro for non-clang compilers
* \ref __has_declspec_attribute Macro for non-clang compilers

CPU defines {#sect_cpu_defines}
-----------

CPU defines, only one is enabled on each compiler.

* \ref BURGER_X86 compiled for 32 bit Intel CPUs.
* \ref BURGER_AMD64 compiled for 64 bit AMD64/Intel CPUs.

* \ref BURGER_ITANIUM compiled for 64 bit Intel Itanium.

* \ref BURGER_ARM32 compiled for 32 bit ARM CPUs.
* \ref BURGER_ARM64 compiled for 64 bit ARM CPUs.

* \ref BURGER_MIPS32 compiled for 32 bit MIPS CPUs.
* \ref BURGER_MIPS64 compiled for 64 bit MIPS CPUs.

* \ref BURGER_POWERPC compiled for 32 bit PowerPC CPUs.
* \ref BURGER_POWERPC64 compiled for 64 bit PowerPC CPUs.

* \ref BURGER_68K compiled for 32 bit 68000 CPUs.

* \ref BURGER_RISCV compiled for 32 bit RISC-V CPUs.

* \ref BURGER_AVR compiled for 32 bit Atmel AVR CPUS.

* \ref BURGER_MSP430 compiled for 32 bit Texas Instruments MSP430 CPUs.

* \ref BURGER_SPARC32 compiled for 32 bit SPARC CPUs.
* \ref BURGER_SPARC64 compiled for 64 bit SPARC CPUs.

* \ref BURGER_CPU_NAME String of the CPU's name.

"Type of CPU" defines, they are defined to help determine the type of CPU is running.

* \ref BURGER_INTEL The CPU is either AMD64 or X86.

* \ref BURGER_PPC CPU is part of the PowerPC family.

* \ref BURGER_ARM CPU is part of the Advanced RISC Machines family.

* \ref BURGER_MIPS CPU is part of the MIPS family.

* \ref BURGER_SPARC CPU is part of the SPARC family.

* \ref BURGER_64BITCPU The CPU has native 64 bit registers (AMD64, PPC64, ARM64, ITANIUM, SPARC64).

CPU special feature macros.

* \ref BURGER_NEON ARM NEON features are enabled.

* \ref BURGER_68881 68000 math co-processor features are enabled.

* \ref BURGER_ALTIVEC PowerPC Altivec features are enabled.

* \ref BURGER_3DNOW AMD 3DNow! features are enabled.
* \ref BURGER_SSE Intel SSE features are enabled.
* \ref BURGER_SSE2 Intel SSE features are enabled.
* \ref BURGER_AVX Intel SSE features are enabled.
* \ref BURGER_AVX2 Intel SSE features are enabled.

Endian defines {#sect_endian_defines}
--------------

Endian defines, only one is enabled on each compile. Do not use the CPU to determine the endian, because it may change on some platforms.

* \ref BURGER_BIGENDIAN Defined on big endian machines
* \ref BURGER_LITTLEENDIAN Defined on little endian machine
* \ref BURGER_ENDIANINDEX_LOW Index for low byte for 16 bit value (0 or 1)
* \ref BURGER_ENDIANINDEX_HIGH Index for high byte for 16 bit value (0 or 1)

Platform defines {#sect_platforms}
----------------

Platform defines, only one is enabled on each compile.

* \ref BURGER_PLATFORM_NAME String of the operating system's name.
* \ref BURGER_WIN32 The underlying OS is 32 bit Windows (XP/NT/7/8 Etc)
* \ref BURGER_WIN64 The underlying OS is 64 bit Windows
* \ref BURGER_WINDOWS The underlying OS is any flavor of Microsoft Windows
* \ref BURGER_MSDOS The underlying OS is MSDOS
* \ref BURGER_MAC The underlying OS is MacOS (Pre-X)
* \ref BURGER_MACOSX The underlying OS is a version of MacOSX
* \ref BURGER_MACCLASSIC macOS 7-9 using class API
* \ref BURGER_MACCARBON macOS7-9 using CarbonLib
* \ref BURGER_MACOS The underlying OS is ANY version of MacOS
* \ref BURGER_IOS Build for iOS
* \ref BURGER_NGAGE Build for Nokia NGage
* \ref BURGER_SYMBIAN Build for Symbian OS
* \ref BURGER_PS1 Build for Playstation classic
* \ref BURGER_PS2 Build for Playstation 2
* \ref BURGER_PS3 Build for Playstation 3
* \ref BURGER_PS4 Build for Playstation 4
* \ref BURGER_PSP Build for Playstation Portable
* \ref BURGER_VITA Build for Playstation Portable 2
* \ref BURGER_GAMECUBE Build for Nintendo Gamecube
* \ref BURGER_WII Build for Nintendo Wii
* \ref BURGER_WIIU Build for Nintendo WiiU
* \ref BURGER_SWITCH Build for Nintendo Switch
* \ref BURGER_GBA Gameboy Advanced Build for Nintendo Gameboy Advance
* \ref BURGER_DS Build for Nintendo DS and DSi
* \ref BURGER_3DS Build for Nintendo 3DS
* \ref BURGER_XBOX Build for the original XBox
* \ref BURGER_XBOX360 Build for the Xbox 360
* \ref BURGER_XBOXONE Build for the Xbox ONE
* \ref BURGER_ANDROID Build for Android
* \ref BURGER_SHIELD Build for nVidia Shield
* \ref BURGER_OUYA Build for Ouya
* \ref BURGER_BEOS Build for BeOS and Haiku.
* \ref BURGER_AMIGA Build for Commodore Amiga.
* BURGER_APPLEIIGS has been removed.
* BURGER_3DO has been removed.

Global defines {#sect_global_defines}
--------------

Standard defines present on all build targets

* \ref TRUE 1
* \ref FALSE 0
* \ref NULL 0
* \ref BURGER_MAXINT
* \ref BURGER_MININT
* \ref BURGER_MAXUINT
* \ref BURGER_INLINE Force inlining this function.
* \ref BURGER_NO_INLINE Disable inlining this function.
* \ref BURGER_API Burgerlib function call API type (Usually fastcall)
* \ref BURGER_ANSIAPI API native to the compiler.
* \ref BURGER_ASM Define a function as pure assembly.
* \ref BURGER_CFM macOS 7-9 Code Fragment Manager
* \ref BURGER_FASTCALLENABLED Used for Intel assembly functions to use the __fastcall ABI
* \ref BURGER_DECLSPECNAKED Used to create a function without a prologue or epilogue
* \ref BURGER_ALIGN
* \ref BURGER_PREALIGN
* \ref BURGER_POSTALIGN
* \ref BURGER_STRUCT_ALIGN
* \ref BURGER_STRUCT_PACK
* \ref BURGER_STRUCT_PACKPUSH

API detection {#sect_api_detect}
-------------

Defines to denote if features, compilation switches or high level APIs are present

* \ref BURGER_OPENGL OpenGL is available
* \ref BURGER_OPENGLES OpenGL ES is available
* \ref BURGER_VULKAN Vulkan is available
* \ref BURGER_TCPIP Is TCP/IP available on this platform
* \ref BURGER_APPLETALK Is AppleTalk supported on this platform
* \ref BURGER_IPX Is IPX/SPX available on this platform
* \ref BURGER_STEAM Is Steam is available on this platform
* \ref BURGER_XBOXLIVE Is Xbox Live available on this platform

Handy macros {#sect_handy_macros}
------------

* \ref BURGER_MACRO_TO_STRING Convert a macro's definition into a string
* \ref BURGER_JOIN Join two macros together.
* \ref BURGER_LEFT_PARENTHESIS (
* \ref BURGER_RIGHT_PARENTHESIS )
* \ref BURGER_OFFSETOF Get offset of a structure member
* \ref BURGER_GET_BASE_PTR
* \ref BURGER_CONST_GET_BASE_PTR
* \ref BURGER_ARRAYSIZE Get number of members in an array
* \ref BURGER_UNUSED Mark variables as unused
* \ref BURGER_EQUALS_DELETE Insert '= delete' on compilers that support it
* \ref BURGER_EQUALS_DEFAULT Insert '= default' on compilers that support it
* \ref BURGER_DISABLE_COPY Disable the ability to copy the class
* \ref BURGER_STATIC_ASSERT `static_assert`
* \ref BURGER_CONSTEXPR `constexpr`
* \ref BURGER_OVERRIDE `override`
* \ref BURGER_FINAL `final`
* \ref nullptr 0 on compilers that don't support this keyword.
* \ref __underlying_type `int` on compilers that don't support this keyword.
* \ref BURGER_RVALUE_REFERENCES Test for && operations.
