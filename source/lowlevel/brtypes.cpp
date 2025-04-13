/***************************************

	Determine which compiler is being used and create standardized typedefs and
	macros so generic code can be created cross platform

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#if defined(DOXYGEN)
#define _DEBUG
#define NDEBUG

#define BURGER_WATCOM __WATCOMC__
#define BURGER_MRC __MRC__
#define BURGER_APPLE_SC __SC__
#define BURGER_BORLAND __BORLANDC__
#define BURGER_DJGPP ((__DJGPP__ * 100) + __DJGPP_MINOR__)
#define BURGER_SNSYSTEMS __SN_VER__
#define BURGER_GHS __GHS_VERSION_NUMBER
#define BURGER_INTEL_COMPILER \
	((__INTEL_COMPILER * 10) + __INTEL_COMPILER_UPDATE)
#define BURGER_MINGW
#define BURGER_CLANG \
	((__clang_major__ * 10000) + (__clang_minor__ * 100) + __clang_patchlevel__)
#define BURGER_GNUC \
	((__GNUC__ * 10000) + (__GNUC_MINOR__ * 100) + __GNUC_PATCHLEVEL__)
#define BURGER_METROWERKS __MWERKS__
#define BURGER_ARM_COMPILER __ARMCC_VERSION
#define BURGER_MSVC _MSC_FULL_VER
#define BURGER_COMPILER_NAME "Open Watcom Compiler"
#define BURGER_COMPILER_VERSION 1234

#define BURGER_X86
#define BURGER_AMD64
#define BURGER_ITANIUM
#define BURGER_ARM32
#define BURGER_ARM64
#define BURGER_MIPS32
#define BURGER_MIPS64
#define BURGER_POWERPC
#define BURGER_POWERPC64
#define BURGER_68K
#define BURGER_RISCV
#define BURGER_AVR
#define BURGER_MSP430
#define BURGER_SPARC32
#define BURGER_SPARC64
#define BURGER_NANOMIPS32
#define BURGER_SH32
#define BURGER_S390
#define BURGER_KVX
#define BURGER_BPF
#define BURGER_MRISC32
#define BURGER_CPU_NAME "The name of the CPU"

#define BURGER_3DNOW

#define BURGER_XBOX
#define BURGER_XBOX360
#define BURGER_XBOXONE
#define BURGER_MSDOS
#define BURGER_DOS4G
#define BURGER_X32
#define BURGER_WINDOWS
#define BURGER_WIN32
#define BURGER_WIN64
#define BURGER_PS1
#define BURGER_PS2
#define BURGER_PS3
#define BURGER_PS4
#define BURGER_PS5
#define BURGER_PSP
#define BURGER_VITA
#define BURGER_ANDROID
#define BURGER_STADIA
#define BURGER_SHIELD
#define BURGER_OUYA
#define BURGER_GBA
#define BURGER_DS
#define BURGER_3DS
#define BURGER_GAMECUBE
#define BURGER_WII
#define BURGER_WIIU
#define BURGER_SWITCH
#define BURGER_BEOS
#define BURGER_SYMBIAN
#define BURGER_NGAGE
#define BURGER_AMIGA
#define BURGER_MAC
#define BURGER_CFM
#define BURGER_MACCLASSIC
#define BURGER_MACCARBON
#define TARGET_API_MAC_CARBON 1
#define BURGER_IOS
#define BURGER_MACOSX
#define BURGER_LINUX
#define BURGER_ARDUINO
#define BURGER_STEAM
#define BURGER_PLATFORM_NAME "Microsoft XBox 360"

#define BURGER_LITTLEENDIAN

#define BURGER_FASTCALLENABLED
#define BURGER_STRUCT_ALIGN
#define BURGER_STRUCT_PACK
#define BURGER_STRUCT_PACKPUSH
#define BURGER_NO_ALIGN

#define nullptr 0
#define __underlying_type(x) int

#define BURGER_INT_NOT_IN_STDINT

/*! ************************************

	\page pageintroduction Introduction to Burgerlib.

	\section intro_sec Introduction

	BurgerLib is a set of subroutines that are designed to work the same across
	multiple target platforms. Proper use of this library will allow rapid
	development and deployment of game and application software across many
	different operating systems and computers.

	All aspects of the virtual OS are standardized so most interaction with the
	host OS an be done transparently so the application needs minimal work for
	platform localization.

	The library was not designed to be a game engine. It was designed to be a
	set of foundation functions that will allow the creation of game engines
	that are cross platform and will take advantage of most features of the
	underlying hardware without being aware that the features are even present.

	Compilers supported under a Win32 host:

	* * <a href="http://www.openwatcom.com"
		target="_blank">Open Watcom 1.9</a> for <a
		href="http://www.dosextender.com/" target="_blank">FlashTek X-32</a>
		Intel.

	* * <a href="http://www.openwatcom.com" target="_blank">Open Watcom 1.9</a>
		for <a href="http://www.tenberry.com/dos4g/watcom/"
		target="_blank">DOS/4GW</a> Intel.

	* * <a href="http://www.openwatcom.com" target="_blank">Open
		Watcom 1.9</a> for <a href="http://msdn.microsoft.com"
		target="_blank">Win32</a> Intel.

	* * <a href="http://www.intel.com/software/products/compilers/cwin/"
		target="_blank">Intel C++ 10.0</a> for <a
		href="http://msdn.microsoft.com" target="_blank">Win32</a> Intel.

	* * <a href="http://msdn2.microsoft.com/en-us/vstudio/aa973782.aspx"
		target="_blank">Visual Studio .NET 2005</a> for <a
		href="http://msdn.microsoft.com" target="_blank">Win32</a> Intel.

	* * <a href="http://msdn2.microsoft.com/en-us/vstudio/aa973782.aspx"
		target="_blank">Visual Studio .NET 2005</a> for <a
		href="http://msdn.microsoft.com" target="_blank">Win64</a> AMD64.

	* * <a href="http://msdn2.microsoft.com/en-us/vstudio/aa700867.aspx"
		target="_blank">Visual Studio .NET 2003</a> for <a
		href="http://www.xbox.com" target="_blank">XBox</a> Intel Celeron.

	* * <a href="http://msdn2.microsoft.com/en-us/vstudio/aa973782.aspx"
		target="_blank">Visual Studio .NET 2005</a> for <a
		href="http://www.xbox.com" target="_blank">XBox 360</a> PowerPC.

	* * <a
		href="http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=CW-SONY-PS2"
		target="_blank">Metrowerks CodeWarrior 3.8.1</a> for <a
		href="http://www.playstation.com/" target="_blank">Sony Playstation
		2</a> MIPS R5900.

	* * <a href="http://www.snsys.com/PlayStation2/ProDG.htm"
		target="_blank">SNSystems GCC</a> for <a
		href="http://www.playstation.com/" target="_blank">Sony Playstation
		2</a> MIPS R5900.

	* * <a href="http://www.lukasz.dk/ps2toolchain.html"
		target="_blank">PS2Dev</a> for <a href="http://www.playstation.com/"
		target="_blank">Sony Playstation 2</a> MIPS R5900.

	* * <a
		href="http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=CW-NINTENDO-GC"
		target="_blank">Metrowerks CodeWarrior 2.7</a> for <a
		href="http://www.nintendo.com/systemsgcn" target="_blank">Nintendo
		GameCube</a> PowerPC Gekko.

	* * <a href="http://www.snsys.com/GameCube/ProDG.htm"
		target="_blank">SNSystems GCC</a> for <a
		href="http://www.nintendo.com/systemsgcn" target="_blank">Nintendo
		GameCube</a> PowerPC Gekko.

	* * <a
		href="http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=CW-NINTENDODS"
		target="_blank">Metrowerks CodeWarrior 2.0</a> for <a
		href="http://www.nintendo.com/systemsds" target="_blank">Nintendo DS</a>
		ARM 9.

	* * <a href="http://www.warioworld.com" target="_blank">Metrowerks
		CodeWarrior 3.0</a> for <a href="http://www.nintendo.com/systemswii"
		target="_blank">Nintendo Wii</a> PowerPC Gekko 2.

	Compilers supported under a MacOSX host:

	* * <a href="http://developer.apple.com/tools/mpw-tools/"
		target="_blank">MrC 5.0d3 MPW</a> for <a
		href="https://en.wikipedia.org/wiki/Mac_OS_9" target="_blank">Mac OS
		Classic</a> PowerPC.

	* * <a
		href="http://www.metrowerks.com/MW/Develop/Desktop/Windows/Professional/Default.htm"
		target="_blank">Metrowerks CodeWarrior 9.6</a> for <a
		href="http://msdn.microsoft.com" target="_blank">Win32</a> Intel.

	* * <a
		href="http://www.metrowerks.com/MW/Develop/Desktop/Macintosh/Professional/Mac9.htm"
		target="_blank">Metrowerks CodeWarrior 6.0</a> for <a
		href="https://en.wikipedia.org/wiki/Mac_OS_9" target="_blank">MacOS
		Classic 8.6/9.2</a> Motorola 68K.

	* * <a
		href="http://www.metrowerks.com/MW/Develop/Desktop/Macintosh/Professional/Mac9.htm"
		target="_blank">Metrowerks CodeWarrior 10.0</a> for <a
		href="https://en.wikipedia.org/wiki/Mac_OS_9" target="_blank">MacOS
		Classic 8.6/9.2</a> PowerPC.

	* * <a
		href="http://www.metrowerks.com/MW/Develop/Desktop/Macintosh/Professional/Mac9.htm"
		target="_blank">Metrowerks CodeWarrior 10.0</a> for <a
		href="https://www.apple.com/macos/high-sierra/" target="_blank">MacOS
		Carbon 8.6/9.2/X</a> PowerPC.

	* * <a
		href="http://www.metrowerks.com/MW/Develop/Desktop/Macintosh/Professional/Mac9.htm"
		target="_blank">Metrowerks CodeWarrior 10.0</a> for <a
		href="https://www.apple.com/macos/high-sierra/" target="_blank">MacOS
		X</a> Mach PowerPC.

	* * <a href="https://developer.apple.com/xcode/" target="_blank">Apple
		XCode 3.0</a> for <a href="http://www.apple.com/macosx/"
		target="_blank">MacOS X</a> Mach PowerPC.

	* * <a href="https://developer.apple.com/xcode/" target="_blank">Apple
		XCode 10.0</a> for <a href="https://www.apple.com/macos/high-sierra/"
		target="_blank">MacOS X</a> Mach Intel.

***************************************/

/*! ************************************

	\def _DEBUG
	\brief Defined to enable the creation of debug code.

	If this define exists, debugging information is included while compiling.
	This is used to flag whether debug code is to be included in the build
	and/or \ref BURGER_ASSERT is to operate. Only \ref _DEBUG or \ref NDEBUG can
	exist at a time. Specifying both or neither is considered a fatal compile
	error. This define is not supplied by Burgerlib, it must be defined by the
	build project.

	\note This flag is not used to determine if optimizations have been
		disabled. It's only used to enable the generation of debugging code.

	\code
	#if defined(_DEBUG)
		CheckMemoryHeap();
	#endif
	\endcode

***************************************/

/*! ************************************

	\def NDEBUG
	\brief Defined to disable the creation of debug code.

	If this define exists, debugging information is not included while
	compiling. This is only present to force \ref BURGER_ASSERT to vanish. Only
	\ref _DEBUG or \ref NDEBUG can exist at a time. Specifying both or neither
	is considered a fatal compile error. This define is not supplied by
	Burgerlib, it must be defined by the build project.

	\note This flag is not used to determine if optimizations have been enabled.
	It's only used to disable the generation of debugging code.

	\code
	#if defined(NDEBUG)
		CrashGameIfDataNotFound();
	#endif
	\endcode

	\sa _DEBUG

***************************************/

/*! ************************************

	\def BURGER_NULL_MACRO_PARAM
	\brief An empty macro parameter.

	\#define BURGER_NULL_MACRO_PARAM /&lowast;&lowast;/

	To trick the preprocessor to accept an empty parameter in a macro, use this
	macro which inserts a "C" style empty comment to force data separation so an
	empty parameter can be passed to a macro.

	\sa BURGER_LEFT_PARENTHESIS, and BURGER_RIGHT_PARENTHESIS

***************************************/

/*! ************************************

	\def BURGER_LEFT_PARENTHESIS
	\brief The character (

	If a macro contains a left parenthesis, it will be treated as an enclosure
	for a parameter list. Use this macro inside another macro to output a left
	parenthesis without actually invoking parameters.

	\sa BURGER_NULL_MACRO_PARAM, and BURGER_RIGHT_PARENTHESIS

***************************************/

/*! ************************************

	\def BURGER_RIGHT_PARENTHESIS
	\brief The character )

	If a macro contains a right parenthesis, it will be treated as an enclosure
	for a parameter list. Use this macro inside another macro to output a right
	parenthesis without actually invoking parameters.

	\sa BURGER_NULL_MACRO_PARAM, and BURGER_LEFT_PARENTHESIS

***************************************/

/*! ************************************

	\def BURGER_STRINGIZE
	\brief Convert a macro into a string literal.

	Using deep magic in the form of \ref BURGER_STRINGIZE2,	convert the macro
	into an 8 bit quoted string literal.

	\param x Name of the macro to turn into a string

	\code
	#define foo 4
	printf(BURGER_STRINGIZE(foo));

	// Prints "4" (Without the quotes)

	\endcode
	\sa BURGER_STRINGIZE2

***************************************/

/*! ************************************

	\def BURGER_STRINGIZE2
	\brief Convert the parameter into a string literal.

	Using the # token, convert the macro parameter into an 8 bit quoted string
	literal. That is, the string does not have the "L" qualfier for UTF-16.

	\param x Name of the item to turn into a string

	\code
	int foo = 4;
	printf(BURGER_STRINGIZE2(foo));

	// Prints "foo" (Without the quotes)

	\endcode

	\sa BURGER_STRINGIZE

***************************************/

/*! ************************************

	\def BURGER_JOIN
	\brief Concatenates two strings, even if they are macros.

	Using deep magic in the form of \ref BURGER_JOIN2 and \ref BURGER_JOIN3,
	join the two macros into one. If a macro is passed, it must resolve into a
	string literal.

	\code
	printf("%s\n", BURGER_JOIN("Language ", BURGER_STDCPP_NAME));
	\endcode

	\param x The first macro to join.
	\param y The second macro to join.

	\sa BURGER_JOIN2 or BURGER_JOIN3

***************************************/

/*! ************************************

	\def BURGER_JOIN2
	\brief Concatenates two strings or macros that directly become strings.

	Invoke \ref BURGER_JOIN3 to join macros with ##.

	\code
	printf("%s\n", BURGER_JOIN("Language ", BURGER_STDCPP_NAME));
	\endcode

	\param x The first macro to join.
	\param y The second macro to join.

	\sa BURGER_JOIN or BURGER_JOIN3

***************************************/

/*! ************************************

	\def BURGER_JOIN3
	\brief Combine two stringa with ##.

	Use ## to join two string literals. Does not work with macros.

	\code
	printf("%s\n", BURGER_JOIN3("Language ", "Forth"));
	\endcode

	\param x The first string to join.
	\param y The second string to join.

	\sa BURGER_JOIN or BURGER_JOIN2

***************************************/

/*! ************************************

	\def NULL
	\brief Define of the number 0 for pointer invalidation.

	This is only present for older codebases. Burgerlib supports nullptr and
	will define \ref nullptr as a macro for older compilers, so to use the
	robust error checking available on modern compilers, use \ref nullptr
	instead.

	\note Since this a common \#define, Burgerlib will first check if it's
	already present before defining.

	\sa \ref nullptr, FALSE or TRUE

***************************************/

/*! ************************************

	\def TRUE
	\brief Define of the number 1 for boolean operations.

	\note Since this a common \#define, Burgerlib will first check if it's
	already present before defining.

	\sa FALSE or NULL

***************************************/

/*! ************************************

	\def FALSE
	\brief Define of the number 0 for boolean operations.

	\note Since this a common \#define, Burgerlib will first check if it's
	already present before defining.

	\sa TRUE or NULL

***************************************/

/*! ************************************

	\def BURGER_CPP89
	\brief Define to determine if compiler has feature level C++89 (ANSI C)

	If this define exists, then code is bring created with a compiler that has a
	minimum feature set found in ANSI C++ compilers.

	\note Burgerlib requires this as a baseline. It will likely not compile
	using a compiler that isn't at least ANSI C89 level of compatibility.

	\sa BURGER_STDCPP_NAME, BURGER_CPP98, BURGER_CPP11, BURGER_CPP14,
		BURGER_CPP17, BURGER_CPP20, and BURGER_CPP23

***************************************/

/*! ************************************

	\def BURGER_CPP98
	\brief Define to determine if compiler has feature level C++98

	If this define exists, then code is bring created with a compiler that has a
	minimum feature set found in C++98 compilers.

	\sa BURGER_STDCPP_NAME, BURGER_CPP89, BURGER_CPP11, BURGER_CPP14,
		BURGER_CPP17, BURGER_CPP20, and BURGER_CPP23

***************************************/

/*! ************************************

	\def BURGER_CPP11
	\brief Define to determine if compiler has feature level C++11

	If this define exists, then code is bring created with a compiler that has a
	minimum feature set found in C++11 compilers.

	\sa BURGER_STDCPP_NAME, BURGER_CPP89, BURGER_CPP98, BURGER_CPP14,
		BURGER_CPP17, BURGER_CPP20, and BURGER_CPP23

***************************************/

/*! ************************************

	\def BURGER_CPP14
	\brief Define to determine if compiler has feature level C++14

	If this define exists, then code is bring created with a compiler that has a
	minimum feature set found in C++14 compilers.

	\sa BURGER_STDCPP_NAME, BURGER_CPP89, BURGER_CPP98, BURGER_CPP11,
		BURGER_CPP17, BURGER_CPP20, and BURGER_CPP23

***************************************/

/*! ************************************

	\def BURGER_CPP17
	\brief Define to determine if compiler has feature level C++17

	If this define exists, then code is bring created with a compiler that has a
	minimum feature set found in C++17 compilers.

	\sa BURGER_STDCPP_NAME, BURGER_CPP89, BURGER_CPP98, BURGER_CPP11,
		BURGER_CPP14, BURGER_CPP20, and BURGER_CPP23

***************************************/

/*! ************************************

	\def BURGER_CPP20
	\brief Define to determine if compiler has feature level C++20

	If this define exists, then code is bring created with a compiler that has a
	minimum feature set found in C++20 compilers.

	\sa BURGER_STDCPP_NAME, BURGER_CPP89, BURGER_CPP98, BURGER_CPP11,
		BURGER_CPP14, BURGER_CPP17, and BURGER_CPP23

***************************************/

/*! ************************************

	\def BURGER_CPP23
	\brief Define to determine if compiler has feature level C++23

	If this define exists, then code is bring created with a compiler that has a
	minimum feature set found in C++23 compilers.

	\sa BURGER_STDCPP_NAME, BURGER_CPP89, BURGER_CPP98, BURGER_CPP11,
		BURGER_CPP14, BURGER_CPP17, and BURGER_CPP20

***************************************/

/*! ************************************

	\def BURGER_STDCPP_NAME
	\brief String of the C++ feature level.

	This define is of a string naming the compiler feature level.

	\sa BURGER_CPP89, BURGER_CPP98, BURGER_CPP11, BURGER_CPP14, BURGER_CPP17,
		BURGER_CPP20, or BURGER_CPP23

***************************************/

/*! ************************************

	\def BURGER_HAS_64_BIT_SUPPORT
	\brief Compiler supports 64 bit integers.

	Most compilers support 64 bit integers, but older compilers only support 32
	bit integers. To allow limited 64 bit integer support, this define exists
	and ``Burger::longlong_t`` and ``Burger::ulonglong_t`` are used to preform
	64 bit integer math.

	\sa Burger::longlong_t, Burger::ulonglong_t, BURGER_APPLE_SC, or BURGER_MRC

***************************************/

/*! ************************************

	\def BURGER_HAS_SFINAE
	\brief Compiler supports SFINAE

	Most compilers support "Substitution Failure Is Not An Error" (SFINAE), for
	those few old compilers that don't support it, this macro doesn't exist.

***************************************/

/*! ************************************

	\def BURGER_COMPILER_NAME
	\brief String of the name of the C++ compiler.

	This define is of a string naming the compiler.

	\sa BURGER_COMPILER_VERSION, BURGER_INTEL_COMPILER, BURGER_MSVC,
		BURGER_WATCOM, BURGER_CLANG, BURGER_GNUC, or BURGER_METROWERKS

***************************************/

/*! ************************************

	\def BURGER_COMPILER_VERSION
	\brief Integer of the version of the current compiler.

	This define is of an integer for the version of the compiler.

	The value is compiler specific, and must be checked for the specific
	compiler before testing this number against a constant.

	\sa BURGER_COMPILER_NAME, BURGER_INTEL_COMPILER, BURGER_MSVC,
		BURGER_WATCOM, BURGER_CLANG, BURGER_GNUC, or BURGER_METROWERKS

***************************************/

/*! ************************************

	\def BURGER_WATCOM
	\brief Define to determine if code is being built using Open Watcom
		compiler.

	If this define exists, then the code is being compiled with the [Open Watcom
		compiler](http://www.openwatcom.com "Open Watcom Project")

	## Known issues with the Open Watcom Compiler version 1.9.

	1. Multicharacter ASCII such as ``'RSRC'`` or ``'ABCD'`` are not supported.
		Any use of this data type will have the compiler generate an ASCII
		character of the first letter found and ignore the rest and assume a
		data type of ``char``.
	2. Templates are not fully supported. So several high level templates are
		not available. Look to the documentation of the Burgerlib templates to
		determine if it is supported on Watcom.

	\sa BURGER_INTEL_COMPILER, BURGER_METROWERKS, BURGER_MSVC and BURGER_X86

***************************************/

// clang-format off
/*! ************************************

	\def BURGER_MRC
	\brief Define to determine if code is being built using Apple/Motorola MrC
		compiler.

	If this define exists, then the code is being compiled with the [MrC compiler](
	https://web.archive.org/web/20240914083349/https://staticky.com/mirrors/ftp.apple.com/developer/Tool_Chest/Core_Mac_OS_Tools/MPW_Website_Downloads/ "MrC Compiler download page on archive.org")

	## Known issues with the Apple MrC compiler.

	1. Templates are poorly supported. Do not use templates.
	2. Version 5.0 or higher is required for Burgerlib.

	\sa BURGER_GNUC, BURGER_METROWERKS, BURGER_APPLE_SC and BURGER_POWERPC

***************************************/

/*! ************************************

	\def BURGER_APPLE_SC
	\brief Define to determine if code is being built using Apple/Motorola SC
		compiler.

	If this define exists, then the code is being compiled with the
	[Apple SC compiler](
	https://web.archive.org/web/20240914083349/https://staticky.com/mirrors/ftp.apple.com/developer/Tool_Chest/Core_Mac_OS_Tools/MPW_Website_Downloads/ "Apple SC Compiler download page on archive.org")

	## Known issues with the Apple SC compiler.

	1. Templates are poorly supported. Do not use templates.
	2. Version 8.9 or higher is required for Burgerlib.
	3. 64 bit integers are NOT supported. Use Metrowerks for 68K instead.

	\sa BURGER_GNUC, BURGER_METROWERKS, BURGER_MRC and BURGER_68K

***************************************/
// clang-format on

/*! ************************************

	\def BURGER_BORLAND
	\brief Define to determine if code is being built using the Borland C
		compiler.

	If this define exists, then the code is being compiled with the [Borland C
	compiler](https://www.embarcadero.com/free-tools/ccompiler "Borland C free
	version download")

	\sa BURGER_MSDOS, or BURGER_X86

***************************************/

/*! ************************************

	\def BURGER_DJGPP
	\brief Define to determine if code is being built using DJGPP compiler.

	If this define exists, then the code is being compiled with the
	[DJGPP compiler](http://www.delorie.com/djgpp/ "DJGPP Compiler home page")

	\sa BURGER_MSDOS, or BURGER_X86

***************************************/

// clang-format off
/*! ************************************

	\def BURGER_SNSYSTEMS
	\brief Define to determine if code is being built using Green Hills
		compiler.

	If this define exists, then the code is being compiled with the [SN Systems 
	compiler](https://web.archive.org/web/20140908162510/http://www.snsys.com/ "SN Systems home page on archive.org")

	\note This compiler is used by Sony for the Playstation 3, and Vita.

	\sa BURGER_METROWERKS, BURGER_POWERPC, BURGER_MIPS32 and BURGER_AMD64

***************************************/
// clang-format on

/*! ************************************

	\def BURGER_GHS
	\brief Define to determine if code is being built using Green Hills
		compiler.

	If this define exists, then the code is being compiled with the
	[Green Hills Software compiler](https://www.ghs.com/ "Green Hills Software
	home page")

	\sa BURGER_METROWERKS, BURGER_POWERPC, BURGER_WIIU and BURGER_WII

***************************************/

/*! ************************************

	\def BURGER_INTEL_COMPILER
	\brief Define to determine if code is being built using Intel compiler.

	If this define exists, then the code is being compiled with the [Intel
	compiler](https://software.intel.com/en-us/c-compilers "Intel Compiler Home
	page")

	\sa BURGER_MSVC, BURGER_METROWERKS, BURGER_AMD64 and BURGER_X86

***************************************/

/*! ************************************

	\def BURGER_MINGW
	\brief Define to determine if code is being built using MinGW compilers.

	If this define exists, then the code is being compiled with the [MinGW
	compiler](https://osdn.net/projects/mingw/ "MinGW home page")

	\sa BURGER_WATCOM, BURGER_INTEL_COMPILER, BURGER_METROWERKS, BURGER_MSVC and
		BURGER_X86

***************************************/

/*! ************************************

	\def BURGER_CLANG
	\brief Define to determine if code is being built using the clang compiler.

	If this define exists, then the code is being compiled with the [clang
	compiler](https://clang.llvm.org/ "Clang home page")

	\sa BURGER_GNUC, BURGER_METROWERKS, BURGER_POWERPC, BURGER_ARM32,
		BURGER_AMD64 and BURGER_X86

***************************************/

/*! ************************************

	\def BURGER_GNUC
	\brief Define to determine if code is being built using the GNU C family of
		compilers.

	If this define exists, then the code is being compiled with the [GNUC
	compiler](https://gcc.gnu.org/ "GCC home page")

	\sa BURGER_CLANG, BURGER_METROWERKS, BURGER_POWERPC, BURGER_ARM32,
		BURGER_AMD64 and BURGER_X86

***************************************/

// clang-format off
/*! ************************************

	\def BURGER_METROWERKS
	\brief Define to determine if code is being built using Freescale/Metrowerks
		family of compilers.

	If this define exists, then the code is being compiled with the [Freescale /
	Metrowerks compiler](https://www.nxp.com/support/developer-resources/software-development-tools/codewarrior-development-tools:CW_HOME)

	\sa BURGER_MRC, BURGER_POWERPC, BURGER_MIPS32 and BURGER_X86

***************************************/

/*! ************************************

	\def BURGER_ARM_COMPILER
	\brief Define to determine if code is being built using
		Advanced RISC Machines family of compilers.

	If this define exists, then the code is being compiled with the [ARM
	compiler](https://developer.arm.com/products/software-development-tools/compilers/arm-compiler)

	\sa BURGER_MRC, BURGER_POWERPC, BURGER_MIPS32 and BURGER_X86

***************************************/
// clang-format on

/*! ************************************

	\def BURGER_MSVC
	\brief Define to determine if code is being built using Microsoft Visual C
		family of compilers.

	If this define exists, then the code is being compiled with the [Microsoft
	Visual C compiler.](https://visualstudio.microsoft.com/)

	\sa BURGER_INTEL_COMPILER, BURGER_METROWERKS, BURGER_ARM32, BURGER_AMD64 and
		BURGER_X86

***************************************/

/*! ************************************

	\def BURGER_CPU_NAME
	\brief String of the name of the CPU.

	"C" string of the name of the CPU.

	\sa BURGER_X86, BURGER_AMD64, BURGER_ARM32, or BURGER_SPARC32

***************************************/

/*! ************************************

	\def BURGER_X86
	\brief Define to determine if code is being built for 32 bit Intel
		processors.

	If this define exists, then the code runs on the 32 bit Intel, AMD and other
	compatible processors. The Microsoft XBox, Mac OSX Intel and Win32 platforms
	which are running on Pentium, 386, 486, and AMD CPUs will have this define
	present.

	\sa BURGER_CPU_NAME, BURGER_INTEL, BURGER_AMD64, BURGER_WIN32, BURGER_BEOS,
		BURGER_MSDOS, BURGER_MACOSX or BURGER_XBOX

***************************************/

/*! ************************************

	\def BURGER_AMD64
	\brief Define to determine if code is being built for AMD64 (Intel 64 bit)
		compatible processors.

	If this define exists, then the code runs on the AMD 64 and other compatible
	processors. The Microsoft Win64 and Mac OSX 64 platforms which are running
	on Pentium and AMD CPUs in 64 bit mode will have this define present.

	\sa BURGER_CPU_NAME, BURGER_INTEL, BURGER_WIN64, BURGER_MACOSX,
		BURGER_XBOXONE, BURGER_PS4, BURGER_PS5, or BURGER_X86

***************************************/

/*! ************************************

	\def BURGER_ITANIUM
	\brief Define to determine if code is being built for Intel Itanium
		compatible processors.

	If this define exists, then the code runs on the Intel Itanium compatible
	processors. The Microsoft Win64 and Linux which are running on Itanium
	compatible processors will have this define present.

	\sa BURGER_CPU_NAME, BURGER_WIN64 or BURGER_LINUX

***************************************/

/*! ************************************

	\def BURGER_ARM32
	\brief Define to determine if code is being built for 32 bit Advanced RISC
		Machine processors.

	If this define exists, then the code runs on the Advanced RISC Machines 32
	bit line of processors. The Gameboy Advanced, Nintendo DS, Nokia NGage,
	Apple iPad/iPhone/iPod and certain cell phones will have this define
	present.

	\sa BURGER_CPU_NAME, BURGER_ARM64, BURGER_ARM, BURGER_GBA, BURGER_ANDROID,
		BURGER_SHIELD, BURGER_OUYA, BURGER_DS, BURGER_3DS, BURGER_IOS,
		BURGER_NGAGE, or BURGER_SYMBIAN

***************************************/

/*! ************************************

	\def BURGER_ARM64
	\brief Define to determine if code is being built for 64 bit Advanced RISC
		Machine processors.

	If this define exists, then the code runs on the Advanced RISC Machines 64
	bit line of processors. Apple iPad/iPhone/iPod/Mac, Nintendo Switch, and
	certain cell phones will have this define present.

	\sa BURGER_CPU_NAME, BURGER_ARM32, BURGER_ARM, BURGER_ANDROID or BURGER_IOS

***************************************/

/*! ************************************

	\def BURGER_MIPS32
	\brief Define to determine if code is being built for MIPS 32 bit
		processors.

	If this define exists, then the code runs on the MIPS 32 bit line of
	processors. R5900 (PS2), R3300 (PS1) and R4400 (PSP) all will have this
	define present.

	\sa BURGER_CPU_NAME, BURGER_MIPS64, BURGER_PS2, BURGER_PSP, BURGER_PS1 or
		BURGER_MIPS

***************************************/

/*! ************************************

	\def BURGER_MIPS64
	\brief Define to determine if code is being built for MIPS 64 bit
		processors.

	If this define exists, then the code runs on the MIPS 64 bit line of 64 bit
	processors.

	\sa BURGER_CPU_NAME, BURGER_MIPS32, BURGER_MIPS

***************************************/

/*! ************************************

	\def BURGER_POWERPC
	\brief Define to determine if code is being built for 32 bit PowerPC
		processors.

	If this define exists, then the code runs on a 32 bit PowerPC processor. The
	Nintendo GameCube, Power Macintosh and the Nintendo Wii all will have this
	define present.

	\sa BURGER_CPU_NAME, BURGER_PPC, BURGER_WIIU, BURGER_WII, BURGER_MAC,
		BURGER_MACOSX, BURGER_BEOS or BURGER_GAMECUBE

***************************************/

/*! ************************************

	\def BURGER_POWERPC64
	\brief Define to determine if code is being built for 64 bit PowerPC
		processors.

	If this define exists, then the code runs on a 64 bit PowerPC processor. The
	G5 Power Macintosh, Sony Playstation 3 and Microsoft XBox 360 all will have
	this define present.

	\sa BURGER_CPU_NAME, BURGER_PPC, BURGER_XBOX360, BURGER_PS3 or BURGER_MACOSX

***************************************/

/*! ************************************

	\def BURGER_68K
	\brief Define to determine if code is being built for Motorola 680x0
		processors.

	If this define exists, then the code runs on the Motorola 680x0 line of
	processors. The classic Macintosh, Commodore Amiga, and certain PDAs will
	have this define present.

	\sa BURGER_CPU_NAME, BURGER_68881, BURGER_MAC or BURGER_CFM

***************************************/

/*! ************************************

	\def BURGER_RISCV
	\brief Define to determine if code is being built for 32 bit RISC-V
		processors.

	If this define exists, then the code runs on the RISC-V line of
	processors. Micro controllers will have this define present.

	\sa BURGER_CPU_NAME, BURGER_MSP430, BURGER_ARM64, BURGER_ARM32 or BURGER_AVR

***************************************/

/*! ************************************

	\def BURGER_AVR
	\brief Define to determine if code is being built for 32 bit Atmel AVR
		processors.

	If this define exists, then the code runs on the Atmel AVR line of
	processors. Micro controllers will have this define present.

	\sa BURGER_CPU_NAME, BURGER_ARM64, BURGER_ARM32 or BURGER_RISCV

***************************************/

/*! ************************************

	\def BURGER_MSP430
	\brief Define to determine if code is being built for 16 bit MSP430
		processors from Texas Instruments.

	If this define exists, then the code runs on the MSP430 line of 16 bit
	processors. Micro controllers will have this define present.

	\sa BURGER_CPU_NAME, BURGER_RISCV, BURGER_ARM32 or BURGER_AVR

***************************************/

/*! ************************************

	\def BURGER_SPARC32
	\brief Define to determine if code is being built for 32 bit Sparc
		processors from Sun.

	If this define exists, then the code runs on the Sparc line of 32 bit
	processors. Sun servers will have this define present.

	\sa BURGER_CPU_NAME, BURGER_LINUX, or BURGER_SPARC64

***************************************/

/*! ************************************

	\def BURGER_SPARC64
	\brief Define to determine if code is being built for 64 bit Sparc
		processors from Sun.

	If this define exists, then the code runs on the Sparc line of 64 bit
	processors. Sun servers will have this define present.

	\sa BURGER_CPU_NAME, BURGER_LINUX, or BURGER_SPARC32

***************************************/

/*! ************************************

	\def BURGER_NANOMIPS32
	\brief Define to determine if code is being built for 32 bit nanoMIPS
	processors from Sun.

	If this define exists, then the code runs on the nanoMIPS line of 32 bit
	processors.

	\sa BURGER_CPU_NAME, BURGER_LINUX, or BURGER_RISCV

***************************************/

/*! ************************************

	\def BURGER_SH32
	\brief Define to determine if code is being built for 32 bit SuperH
	processors from HItachi.

	If this define exists, then the code runs on the SuperH line of 32 bit
	processors. Used by the Sega Dreamcast

	\sa BURGER_CPU_NAME, or BURGER_LINUX

***************************************/

/*! ************************************

	\def BURGER_S390
	\brief Define to determine if code is being built for 64 bit IBM
	System/390 processors.

	If this define exists, then the code runs on the IBM Systemp/390 line of 64
	bit processors. IBM servers will have this define present.

	\sa BURGER_CPU_NAME, BURGER_LINUX, or BURGER_AMD64

***************************************/

/*! ************************************

	\def BURGER_KVX
	\brief Define to determine if code is being built for 32 bit Kalray
	KVX processor.

	If this define exists, then the code runs on the KVX line of 32 bit
	processors from Kalray.

	\sa BURGER_CPU_NAME, or BURGER_LINUX

***************************************/

/*! ************************************

	\def BURGER_BPF
	\brief Define to determine if code is being built for 64 bit eBPF
	pseudocode.

	If this define exists, then the code runs on the eBPF jit virtual CPU.

	\sa BURGER_CPU_NAME, or BURGER_LINUX

***************************************/

/*! ************************************

	\def BURGER_MRISC32
	\brief Define to determine if code is being built for 32 bit vector
	CPU by Marcus Geelnard.

	If this define exists, then the code runs on the MRISC32 CPU.

	More information [can be found here](https://gitlab.com/mrisc32)
	\sa BURGER_CPU_NAME, or BURGER_LINUX

***************************************/

/*! ************************************

	\def BURGER_INTEL
	\brief Define to determine if the CPU is either X86 or AMD64

	On \ref BURGER_AMD64 or \ref BURGER_X86 CPUs, this define will also be
	present to allow Intel Architecture instructions. Some instructions may or
	may not be present, so call \ref Burger::CPUID(Burger::CPUID_t *) to test
	for all features and determine which functions to use for performance.

	\sa BURGER_XBOXONE, BURGER_PS4, BURGER_PS5, BURGER_WINDOWS, BURGER_MACOSX,
		BURGER_X86, or BURGER_AMD64

***************************************/

/*! ************************************

	\def BURGER_PPC
	\brief Define to determine if code is being built for PowerPC processors.

	If this define exists, then code is created to run on the PowerPC line of
	processors. The Nintendo GameCube, Power Macintosh, Nintendo Wii, Sony
	Playstation 3 and Microsoft XBox 360 all will have this define present.

	\sa BURGER_POWERPC, BURGER_POWERPC64, BURGER_XBOX360,
		BURGER_WII, BURGER_PS3, BURGER_MAC, BURGER_MACOSX, BURGER_BEOS or
		BURGER_GAMECUBE

***************************************/

/*! ************************************

	\def BURGER_ARM
	\brief Define to determine if code is being built for Advanced RISC Machine
		processors.

	If this define exists, then code is created to run on on the Advanced RISC
	Machines line of processors.

	\sa BURGER_ARM32, BURGER_ARM64, BURGER_ANDROID, BURGER_SWITCH, or BURGER_IOS

***************************************/

/*! ************************************

	\def BURGER_MIPS
	\brief Define to determine if code is being built for MIPS
		processors.

	If this define exists, then code is created to run on the MIPS line of
	processors.

	\sa BURGER_MIPS32, BURGER_MIPS64, or BURGER_VITA

***************************************/

/*! ************************************

	\def BURGER_SPARC
	\brief Define to determine if code is being built for SPARC
		processors.

	If this define exists, then code is created to run on the SPARC line of
	processors.

	\sa BURGER_SPARC32 or BURGER_SPARC64

***************************************/

/*! ************************************

	\def BURGER_64BITCPU
	\brief Define to determine if the CPU has 64 bit integer registers.

	On \ref BURGER_XBOXONE, \ref BURGER_PS4, \ref BURGER_AMD64, \ref BURGER_PS3
	and \ref BURGER_XBOX360 platforms, the CPU has integer registers that are
	64-bits wide. Using this define, code can be written that takes advantage of
	this. Mostly useful with \ref fixed16_16_t and other high precision fixed
	point calculations.

	\sa BURGER_XBOXONE, BURGER_PS4, BURGER_PS3, BURGER_XBOX360, or BURGER_AMD64

***************************************/

/*! ************************************

	\def BURGER_NEON

	\brief Define to determine if code is being built with Advanced RISC
		Machines NEON instruction set extensions.

	If this define exists, then code is created to run on the ARM line of
	processors with NEON instructions.

	\sa BURGER_ARM, BURGER_ARM32, or BURGER_ARM64

***************************************/

/*! ************************************

	\def BURGER_68881

	\brief Define to determine if code is being built with Motorola 680x0
		processors with a 68881 floating point unit.

	If this define exists, then code is created to run on the Motorola 680x0
	line of processors with floating point hardware.

	\sa BURGER_68K

***************************************/

/*! ************************************

	\def BURGER_ALTIVEC

	\brief Define to determine if code is being built with PowerPC Altivec
		instruction set extensions.

	If this define exists, then code is created to run on the PowerPC line of
	processors with Altivec instructions.

	\sa BURGER_PPC, BURGER_POWERPC, or BURGER_POWERPC64

***************************************/

/*! ************************************

	\def BURGER_3DNOW

	\brief Define to determine if code is being built with AMD
		3D Now! instruction set extensions.

	If this define exists, then code is created to run on the AMD line of
	processors with 3D Now! instructions.

	[Click here to read the 3D Now! instruction set](3dnow.pdf)

	\sa BURGER_SSE, BURGER_SSE2, BURGER_AVX, BURGER_AVX2, BURGER_INTEL,
		BURGER_X86, or BURGER_AMD64

***************************************/

/*! ************************************

	\def BURGER_SSE

	\brief Define to determine if code is being built with SSE instruction set
		extensions.

	If this define exists, then code is created to run on a processor that has
	SSE instructions.

	[Click here to read the SSE through AVX2 instruction set](avx.pdf)

	\sa BURGER_3DNOW, BURGER_SSE2, BURGER_AVX, BURGER_AVX2, BURGER_INTEL,
		BURGER_X86, or BURGER_AMD64

***************************************/

/*! ************************************

	\def BURGER_SSE2

	\brief Define to determine if code is being built with SSE2 instruction set
		extensions.

	If this define exists, then code is created to run on a processor that has
	SSE2 instructions.

	[Click here to read the SSE through AVX2 instruction set](avx.pdf)

	\sa BURGER_3DNOW, BURGER_SSE, BURGER_AVX, BURGER_AVX2, BURGER_INTEL,
		BURGER_X86, or BURGER_AMD64

***************************************/

/*! ************************************

	\def BURGER_AVX

	\brief Define to determine if code is being built with AVX instruction set
		extensions.

	If this define exists, then code is created to run on a processor that has
	AVX instructions.

	[Click here to read the SSE through AVX2 instruction set](avx.pdf)

	\sa BURGER_3DNOW, BURGER_SSE, BURGER_SSE2, BURGER_AVX2, BURGER_INTEL,
		BURGER_X86, or BURGER_AMD64

***************************************/

/*! ************************************

	\def BURGER_AVX2

	\brief Define to determine if code is being built with AVX2 instruction set
		extensions.

	If this define exists, then code is created to run on a processor that has
	AVX2 instructions.

	[Click here to read the SSE through AVX2 instruction set](avx.pdf)

	\sa BURGER_3DNOW, BURGER_SSE, BURGER_SSE2, BURGER_AVX, BURGER_INTEL,
		BURGER_X86, or BURGER_AMD64

***************************************/

/*! ************************************

	\def BURGER_PLATFORM_NAME
	\brief String with the operating system's name.

	This string is of the operating system the code is being compiled for.

	\sa BURGER_WINDOWS, BURGER_LINUX or BURGER_MACOS

***************************************/

/*! ************************************

	\def BURGER_XBOX
	\brief Define to determine if code is being built for the Microsoft XBox.

	If this define exists, then code is created to run on the Intel
	Pentium III Coppermine for a Microsoft XBox.

	\sa BURGER_X86, BURGER_XBOX360, or BURGER_XBOXONE

***************************************/

/*! ************************************

	\def BURGER_XBOX360
	\brief Define to determine if code is being built for the Microsoft XBox
		360.

	If this define exists, then code is created to run on the PowerPC 64 with
	VMX extensions on the Microsoft XBox360.

	\sa BURGER_POWERPC, BURGER_XBOX, or BURGER_XBOXONE

***************************************/

/*! ************************************

	\def BURGER_XBOXONE
	\brief Define to determine if code is being built for the Microsoft XBox
		One.

	If this define exists, then code is created to run on the AMD Jaguar 8 core
	CPU on the Microsoft XBox One and compatible consoles.

	\sa BURGER_AMD64, BURGER_XBOX, or BURGER_XBOX360

***************************************/

/*! ************************************

	\def BURGER_MSDOS
	\brief Define to determine if code is being built for Microsoft MS-DOS.

	If this define exists, then code is created to run on classic MS-DOS.

	Check for the defines \ref BURGER_X32 or \ref BURGER_DOS4G to determine
	which dos extender is being used.

	\sa BURGER_X86, BURGER_X32, or BURGER_DOS4G

***************************************/

/*! ************************************

	\def BURGER_X32
	\brief Define to determine if MS-DOS code is being built with the X32 DOS
		extender.

	If this define exists, then code is created to run on classic MS-DOS with
	the X32 DOS extender.

	\note Some projects may have to define \__X32__ to enable the X32 DOS
	extender.

	\sa BURGER_X86, BURGER_MSDOS, or BURGER_DOS4G

***************************************/

/*! ************************************

	\def BURGER_DOS4G
	\brief Define to determine if MS-DOS code is being built with the DOS4G DOS
		extender.

	If this define exists, then code is created to run on classic MS-DOS with
	the DOS4G DOS extender.

	\sa BURGER_X86, BURGER_MSDOS, or BURGER_X32

***************************************/

/*! ************************************

	\def BURGER_WINDOWS
	\brief Define to determine if code is being built for Microsoft 32 or 64 bit
		Windows.

	If this define exists, then code is created to run on Microsoft Windows for
	either 32 or 64 bit CPUs.

	\note Check the processor define if necessary due to Windows being available
	on Intel and ARM processors.

	\sa BURGER_WIN32, BURGER_WIN64, or BURGER_MSDOS

***************************************/

/*! ************************************

	\def BURGER_WIN32
	\brief Define to determine if code is being built for Microsoft 32 bit
		Windows.

	If this define exists, then code is created to run on Microsoft Windows for
	32 bit CPUs.

	\note Check the processor define if necessary due to Windows being available
	on Intel and ARM processors.

	\sa BURGER_WINDOWS,BURGER_WIN64, BURGER_MSDOS, BURGER_X86, or BURGER_ARM32

***************************************/

/*! ************************************

	\def BURGER_WIN64
	\brief Define to determine if code is being built for Microsoft 64 bit
		Windows.

	If this define exists, then code is created to run on Microsoft Windows for
	64 bit CPUs.

	\note Check the processor define if necessary due to Windows being available
	on Intel and ARM processors.

	\sa BURGER_WINDOWS, BURGER_WIN32, BURGER_MSDOS, BURGER_AMD64,
		or BURGER_ARM64

***************************************/

/*! ************************************

	\def BURGER_PS1
	\brief Define to determine if code is being built for the Sony Playstation.

	If this define exists, then code is created to run on the Sony Playstation.

	\note There is no floating point support on this platform.

	\sa BURGER_MIPS32, BURGER_PS2, BURGER_PS3, BURGER_PS4, or BURGER_PS5

***************************************/

/*! ************************************

	\def BURGER_PS2
	\brief Define to determine if code is being built for the Emotion Engine CPU
		for a PS2.

	If this define exists, then code is created to run on the main processor for
	the Sony Playstation 2.

	\sa BURGER_MIPS32, BURGER_PS1, BURGER_PS3, BURGER_PS4, or BURGER_PS5

***************************************/

/*! ************************************

	\def BURGER_PS3
	\brief Define to determine if code is being built for the PS3.

	If this define exists, then code is created to run on the main processor for
	the Sony Playstation 3.

	\note This define is only present for compiling code on the primary CPU.
	Code targeted for the cell units must be written and compiled separately.

	\sa BURGER_POWERPC, BURGER_PS1, BURGER_PS2, BURGER_PS4, or BURGER_PS5

***************************************/

/*! ************************************

	\def BURGER_PS4
	\brief Define to determine if code is being built for the PS4.

	If this define exists, then code is created to run on the Sony
	Playstation 4.

	\sa BURGER_AMD64, BURGER_PS1, BURGER_PS2, BURGER_PS3, or BURGER_PS5

***************************************/

/*! ************************************

	\def BURGER_PS5
	\brief Define to determine if code is being built for the PS5.

	If this define exists, then code is created to run on the Sony
	Playstation 5.

	\sa BURGER_AMD64, BURGER_PS1, BURGER_PS2, BURGER_PS3, or BURGER_PS4

***************************************/

/*! ************************************

	\def BURGER_PSP
	\brief Define to determine if code is being built for the Sony PSP.

	If this define exists, then code is created to run on the Sony Playstation
	Portable (PSP).

	\sa BURGER_MIPS32, BURGER_VITA, BURGER_PS1, BURGER_PS2, BURGER_PS3, or
		BURGER_PS4

***************************************/

/*! ************************************

	\def BURGER_VITA
	\brief Define to determine if code is being built for the Sony VITA.

	If this define exists, then code is created to run on the Sony Playstation
	Portable 2 (PSP2 / Vita).

	\sa BURGER_ARM32, BURGER_PSP, BURGER_PS1, BURGER_PS2, BURGER_PS3, or
		BURGER_PS4

***************************************/

/*! ************************************

	\def BURGER_STADIA
	\brief Define to determine if code is being built for Google Stadia.

	If this define exists, then code is created to run on the Google Stadia
	streaming console.

	\note As of 2023, Stadia has been shut down, so this target exists only for
		archive purposes only.

	\sa BURGER_UNIX, BURGER_LINUX, BURGER_ANDROID, or BURGER_AMD64

***************************************/

/*! ************************************

	\def BURGER_ANDROID
	\brief Define to determine if code is being built for Google Android
		devices.

	If this define exists, then code is created to run on Google Android
	devices.

	\sa BURGER_IOS, BURGER_SHIELD, BURGER_OUYA, BURGER_ARM32 or BURGER_MACOSX

***************************************/

/*! ************************************

	\def BURGER_SHIELD
	\brief Define to determine if code is being built for nVidia SHIELD devices.

	If this define exists, then code is created to run on nVidia SHIELD Android
	devices.

	\sa BURGER_IOS, BURGER_ANDROID, BURGER_OUYA, BURGER_ARM32 or BURGER_MACOSX

***************************************/

/*! ************************************

	\def BURGER_GBA
	\brief Define to determine if code is being built for the Nintendo Gameboy
	Advanced.

	If this define exists, then code is created to run on the Nintendo Gameboy
	Advanced.

	\note In addition to defining \ref NDEBUG or \ref _DEBUG, SDK_DEBUG needs to
	be defined in the project for the Nintendo GBA SDK to generate debugging
	information

	\sa BURGER_ARM32, BURGER_DS or BURGER_3DS

***************************************/

/*! ************************************

	\def BURGER_DS
	\brief Define to determine if code is being built for the Nintendo DS.

	If this define exists, then code is created to run on the Nintendo DS.

	\note In addition to defining \ref NDEBUG or \ref _DEBUG, SDK_DEBUG needs to
	be defined in the project for the Nintendo DS SDK to generate debugging
	information

	\sa BURGER_ARM32, BURGER_3DS or BURGER_GBA

***************************************/

/*! ************************************

	\def BURGER_3DS
	\brief Define to determine if code is being built for the Nintendo 3DS.

	If this define exists, then code is created to run on the Nintendo 3DS.

	\note In addition to defining \ref NDEBUG or \ref _DEBUG,  SDK_DEBUG needs
	to be defined in the project for the Nintendo 3DS SDK to generate debugging
	information

	\sa BURGER_ARM32, BURGER_DS or BURGER_GBA

***************************************/

/*! ************************************

	\def BURGER_GAMECUBE
	\brief Define to determine if code is being built for the Nintendo GameCube.

	If this define exists, then code is created to run on the Gekko processor
	for the Nintendo GameCube.

	\sa BURGER_POWERPC, BURGER_WII or BURGER_WIIU

***************************************/

/*! ************************************

	\def BURGER_WII
	\brief Define to determine if code is being built for the Nintendo Wii.

	If this define exists, then code is created to run on the Broadway processor
	for the Nintendo Wii.

	\sa BURGER_SWITCH, BURGER_POWERPC, BURGER_WIIU or BURGER_GAMECUBE

***************************************/

/*! ************************************

	\def BURGER_WIIU
	\brief Define to determine if code is being built for the Nintendo WiiU.

	If this define exists, then code is created to run on the Broadway processor
	for the Nintendo WiiU.

	\sa BURGER_SWITCH, BURGER_POWERPC, BURGER_WII or BURGER_GAMECUBE

***************************************/

/*! ************************************

	\def BURGER_SWITCH
	\brief Define to determine if code is being built for the Nintendo Switch.

	If this define exists, then code is created to run on the Arm64 processor
	for the Nintendo Switch.

	\sa BURGER_ARM64, BURGER_WII, BURGER_WII or BURGER_3DS

***************************************/

/*! ************************************

	\def BURGER_BEOS
	\brief Define to determine if code is being built for BeOS.

	If this define exists, then code is created to run on BeOS for Intel or
	PowerPC.

	\sa BURGER_POWERPC or BURGER_X86

***************************************/

/*! ************************************

	\def BURGER_SYMBIAN
	\brief Define to determine if code is being built for the Symbian OS.

	If this define exists, then code is created to run on the Symbian OS,
	usually Nokia brand cell phones and PDAs.

	\note There is no floating point support on this platform.

	\sa BURGER_ARM32, or BURGER_NGAGE

***************************************/

/*! ************************************

	\def BURGER_AMIGA
	\brief Define to determine if code is being built for the Commodore Amiga.

	If this define exists, then code is created to run on the Amiga for 68K or
	PowerPC.

	\sa BURGER_POWERPC or BURGER_68K

***************************************/

/*! ************************************

	\def BURGER_MAC
	\brief Define to determine if code is being built for Apple Mac OS 7.5.3
		through 9.2.

	If this define exists, then code is created to run on Apple	MacOS
	version 7.5.3 through 9.2. Since MacOS runs on the 680x0 or the PowerPC
	architectures, a check \ref BURGER_68K or \ref BURGER_POWERPC for any CPU
	specific code or features. Also, on 680x0 platforms, the code may be
	compiling with the Code Fragment Manager format, and to detect that, check
	for the presence of \ref BURGER_CFM.

	\note To compile for Carbon under a PowerPC CPU, the project must define
	\ref TARGET_API_MAC_CARBON so the MacOS headers will properly be adjusted
	for Carbon. If the code is using Burgerlib for Carbon, it will have access
	to MacOS X features if the application is running on a MacOS X system.

	\code
	// Include this define in your CodeWarrior C/C++ Preprocessor preferences
	// settings to build for MacOS 9 Carbon.

	#define TARGET_API_MAC_CARBON 1
	\endcode

	\sa BURGER_MACOS, BURGER_MACOSX, BURGER_MACCLASSIC, BURGER_MACCARBON,
		BURGER_68K, BURGER_POWERPC or TARGET_API_MAC_CARBON

***************************************/

/*! ************************************

	\def TARGET_API_MAC_CARBON
	\brief Define to determine if Mac OS 9 code is being built with the Carbon
		libraries.

	Define this macro to 1 so code that runs under classic Mac OS will be
	compiled to link with the Carbon libraries.

	\note This define is declared by the project, not by Burgerlib. If this
	macro is missing, the code will default to classic MacOS 7.5.3 - 9.2.

	\sa BURGER_MAC, BURGER_CFM or BURGER_MACOSX

***************************************/

/*! ************************************

	\def BURGER_CFM
	\brief Define to determine if Mac OS 9 code is being built in Code Fragment
		Manger format.

	If this define exists, then code is created in which the final binary
	is in PEF format for use with the Code Fragment Manager. All Power PC code
	for Classic or Carbon is in this format and 68k also can be compiled this
	way.

	\note To be able to use DrawSprocket in 68k code, you must build your
	application using CFM. The classic format can't link to DrawSprocket 68k.

	\note Power PC code can be built in Mach-O format, but if that is the case,
	then the code can only run in Mac OS X.

	\sa BURGER_MAC, BURGER_68K or BURGER_MACOSX

***************************************/

/*! ************************************

	\def BURGER_MACCLASSIC
	\brief Define to determine if code is being built exclusively for Apple Mac
		OS9.

	If this define exists, then code is created to run on Apple	MacOS 7.5
	through 9 for either PowerPC or 68K. There is no support for MacOS X
	operating systems under this build target. The CPU can be either \ref
	BURGER_POWERPC or \ref BURGER_68K for this platform, so no CPU specific code
	should be written without checking for the CPU type. Since no MacOS Carbon
	support is enabled, accessing the low memory operating system variables is
	allowed.

	\sa BURGER_POWERPC, BURGER_68K, BURGER_MACOS or BURGER_MAC

***************************************/

/*! ************************************

	\def BURGER_MACCARBON
	\brief Define to determine if code is being built for both Apple Mac OS9 and
		MacOS X.

	If this define exists, then code is created to run on Apple	MacOS 9 and for
	Apple MacOS X. Only the PowerPC is supported. Since MacOS Carbon support is
	enabled, accessing the low memory operating system variables is **NOT**
	allowed.

	\sa BURGER_POWERPC, BURGER_MACOSX, BURGER_MACOS or BURGER_MAC

***************************************/

/*! ************************************

	\def BURGER_IOS
	\brief Define to determine if code is being built for Apple iOS devices.

	If this define exists, then code is created to run on Apple iOS devices.

	\sa BURGER_ANDROID, BURGER_ARM, BURGER_DARWIN or BURGER_MACOSX

***************************************/

/*! ************************************

	\def BURGER_MACOSX
	\brief Define to determine if code is being built exclusively for Apple Mac
		OS X.

	If this define exists, then code is created to run on Apple MacOS X using
	Mach-O. There is no support for pre-MacOS X operating systems under this
	build target. The CPU can be either \ref BURGER_POWERPC, \ref BURGER_AMD64,
	\ref BURGER_X86, or \ref BURGER_ARM64 for this platform, so no CPU specific
	code should be written without checking for the CPU type.

	\sa BURGER_POWERPC, BURGER_AMD64, BURGER_X86, BURGER_ARM64, BURGER_MACOS,
		BURGER_DARWIN or BURGER_MAC

***************************************/

/*! ************************************

	\def BURGER_LINUX
	\brief Define to determine if code is being built for Linux.

	If this define exists, then code is created to run on Linux.

	\sa BURGER_POWERPC, BURGER_AMD64 or BURGER_X86

***************************************/

/*! ************************************

	\def BURGER_ARDUINO
	\brief Define to determine if code is being built for Arduino.

	If this define exists, then code is created to run on Arduino devices.

	\sa BURGER_POWERPC, BURGER_AMD64 or BURGER_X86

***************************************/

/*! ************************************

	\def BURGER_OUYA
	\brief Define to determine if code is being built for the OUYA.

	If this define exists, then code is created to run on OUYA Android devices.

	\sa BURGER_IOS, BURGER_ANDROID, BURGER_SHIELD, BURGER_ARM32 or BURGER_MACOSX

***************************************/

/*! ************************************

	\def BURGER_NGAGE
	\brief Define to determine if code is being built for the Nokia Ngage.

	If this define exists, then code is created to run on the Nokia NGage.

	\note There is no floating point support on this platform.

	\sa BURGER_ARM32, or BURGER_SYMBIAN

***************************************/

/*! ************************************

	\def BURGER_MACOS
	\brief Define to determine if code is being built for any Apple Mac OS
		platform.

	If this define exists, then code is created to run on Apple MacOS.
	Currently, 68K, PowerPC, ARM and Intel are supported. Since it's not known
	which version of MacOS is currently being targeted, accessing the low memory
	operating system variables is **NOT** allowed.

	\sa BURGER_POWERPC, BURGER_X86, BURGER_68K, BURGER_MACOSX,
		BURGER_MACCLASSIC, BURGER_MACCARBON or BURGER_MAC

***************************************/

/*! ************************************

	\def BURGER_DARWIN
	\brief Define to determine if code is being built on a Darwin platorm.

	If this define exists, then code is created to run on macOS X, tvOS,
	watchOS, or iOS running on top of Darwin.

	\sa BURGER_IOS, or BURGER_MACOSX

***************************************/

/*! ************************************

	\def BURGER_UNIX
	\brief Define to determine if code is being built on a unix platorm.

	If this define exists, then code is created to run on macOS X, tvOS,
	watchOS, iOS, Android, or Linux so the Unix/BSD APIs are available.

	\note This will also be present if the Posix APIs are present.

	\sa BURGER_LINUX, BURGER_ANDROID, BURGER_IOS, or BURGER_MACOSX

***************************************/

/*! ************************************

	\def BURGER_OPENGL
	\brief Define for the presence of OpenGL

	If this define exists, then OpenGL is supported.

	\sa BURGER_OPENGLES, or BURGER_VULKAN

***************************************/

/*! ************************************

	\def BURGER_OPENGLES
	\brief Define for the presence of OpenGL ES

	If this define exists, then OpenGL ES is supported.

	\sa BURGER_OPENGL, or BURGER_VULKAN

***************************************/

/*! ************************************

	\def BURGER_VULKAN
	\brief Define for the presence of Vulkan

	If this define exists, then Vulkan is supported.

	\sa BURGER_OPENGL, or BURGER_OPENGLES

***************************************/

/*! ************************************

	\def BURGER_TCPIP
	\brief Define for the presence of TCP/IP

	If this define exists, then the TCP/IP networking protocol is supported.
s
	\sa BURGER_APPLETALK, BURGER_IPX, BURGER_STEAM, or BURGER_XBOXLIVE

***************************************/

/*! ************************************

	\def BURGER_APPLETALK
	\brief Define for the presence of AppleTalk

	If this define exists, then the AppleTalk networking protocol is supported.
	This is usually found on older MacOS computers and some Windows XP
	platforms.

	\sa BURGER_TCPIP, BURGER_IPX, BURGER_STEAM, or BURGER_XBOXLIVE

***************************************/

/*! ************************************

	\def BURGER_IPX
	\brief Define for the presence of IPX/SPX

	If this define exists, then the IPX/SPX networking protocol is supported.
	It's supported on Windows and older MacOS platforms.

	\sa BURGER_TCPIP, BURGER_APPLETALK, BURGER_STEAM, or BURGER_XBOXLIVE

***************************************/

/*! ************************************

	\def BURGER_STEAM
	\brief Define for the presence of Valve's Steam platform

	If this define exists, then the Steam platform is supported. TCP/IP can be
	assumed to be available with this define present.

	\note Steam is not available on PowerPC MacOSX and old Windows compilers

	\sa BURGER_TCPIP, BURGER_APPLETALK, BURGER_IPX, or BURGER_XBOXLIVE

***************************************/

/*! ************************************

	\def BURGER_XBOXLIVE
	\brief Define for the presence of Microsoft's Xbox Live platform.

	If this define exists, then the Xbox Live platform is supported. TCP/IP can
	be assumed to be available with this define present.

	\sa BURGER_TCPIP, BURGER_APPLETALK, BURGER_IPX, or BURGER_STEAM

***************************************/

/*! ************************************

	\def BURGER_CONSOLE
	\brief Define for the presence of a text console.

	If this define exists, then the platform supports a terminal console such as
	CMD.COM, bash, sh etc. Test for this macro if the code requires a console,
	such as code for a command line tool.

	\sa BURGER_MSDOS, BURGER_LINUX, BURGER_MACOS or BURGER_WINDOWS

***************************************/

/*! ************************************

	\def BURGER_LITTLEENDIAN
	\brief Define to flag code running on a little endian machine.

	This define or \ref BURGER_BIGENDIAN will be present to allow the programmer
	to create code that can properly parse endian specific data. Only one macro
	will be defined.

	If this macro is present, the machine the code is being built for is little
	endian.

	\sa BURGER_BIGENDIAN, BURGER_ENDIANINDEX_LOW or BURGER_ENDIANINDEX_HIGH

***************************************/

/*! ************************************

	\def BURGER_BIGENDIAN
	\brief Define to flag code running on a big endian machine.

	This define or \ref BURGER_LITTLEENDIAN will be present to allow the
	programmer to create code that can properly parse endian specific data. Only
	one macro will be defined.

	If this macro is present, the machine the code is being built for is big
	endian.

	\sa BURGER_LITTLEENDIAN, BURGER_ENDIANINDEX_LOW or BURGER_ENDIANINDEX_HIGH

***************************************/

/*! ************************************

	\def BURGER_ENDIANINDEX_LOW
	\brief Index to the "low" word of a 64 bit value as a 32 value

	If a 64 bit value is stored in memory in an array of 32 bit integers, which
	index references the low 32 bits of the 64 bit integer. On little endian
	machines, this value is 0, otherwise it's 1 for big endian.

	\sa BURGER_ENDIANINDEX_HIGH, BURGER_LITTLEENDIAN or BURGER_BIGENDIAN

***************************************/

/*! ************************************

	\def BURGER_ENDIANINDEX_HIGH
	\brief Index to the "high" word of a 64 bit value as a 32 value

	If a 64 bit value is stored in memory in an array of 32 bit integers, which
	index references the low 32 bits of the 64 bit integer. On little endian
	machines, this value is 1, otherwise it's 0 for big endian.

	\sa BURGER_ENDIANINDEX_LOW, BURGER_LITTLEENDIAN or BURGER_BIGENDIAN

***************************************/

// clang-format off
/*! ************************************

	\def __has_builtin
	\brief Clang feature macro for __has_builtin.

	On non-clang compilers, this compiles to 0.

	[Clang documentation on __has_builtin](https://clang.llvm.org/docs/LanguageExtensions.html#has-builtin)

	\sa BURGER_CLANG

***************************************/

/*! ************************************

	\def __has_feature
	\brief Clang feature macro for __has_feature.

	On non-clang compilers, this compiles to 0.

	[Clang documentation on __has_feature](https://clang.llvm.org/docs/LanguageExtensions.html#has-feature-and-has-extension)

	\sa BURGER_CLANG

***************************************/

/*! ************************************

	\def __has_extension
	\brief Clang feature macro for __has_extension.

	On non-clang compilers, this remaps to \ref __has_feature, which could also
	be 0.

	[Clang documentation on __has_extension](https://clang.llvm.org/docs/LanguageExtensions.html#has-feature-and-has-extension)

	\sa BURGER_CLANG

***************************************/

/*! ************************************

	\def __has_cpp_attribute
	\brief Clang feature macro for __has_cpp_attribute.

	On non-clang compilers, this compiles to 0.

	[Clang documentation on __has_cpp_attribute](https://clang.llvm.org/docs/LanguageExtensions.html#has-cpp-attribute)

	\sa BURGER_CLANG

***************************************/

/*! ************************************

	\def __has_declspec_attribute
	\brief Clang feature macro for __has_declspec_attribute.

	On non-clang compilers, this compiles to 0.

	[Clang documentation on __has_declspec_attribute](https://clang.llvm.org/docs/LanguageExtensions.html#has-declspec-attribute)

	\sa BURGER_CLANG

***************************************/

/*! ************************************

	\def __has_attribute
	\brief Clang feature macro for __has_attribute.

	On non-clang compilers, this compiles to 0.

	[Clang documentation on __has_attribute](https://clang.llvm.org/docs/LanguageExtensions.html#has-attribute)

	\sa BURGER_CLANG

***************************************/

/*! ************************************

	\def __has_warning
	\brief Clang feature macro for __has_warning.

	On non-clang compilers, this compiles to 0.

	[Clang documentation on __has_warning](https://clang.llvm.org/docs/LanguageExtensions.html#has-warning)

	\sa BURGER_CLANG

***************************************/

/*! ************************************

	\def __has_include
	\brief Clang feature macro for __has_include.

	On non-clang compilers, this compiles to 0.

	[Clang documentation on __has_include](https://clang.llvm.org/docs/LanguageExtensions.html#has-include)

	\sa BURGER_CLANG

***************************************/

/*! ************************************

	\def __has_include_next
	\brief Clang feature macro for __has_include_next.

	On non-clang compilers, this compiles to 0.

	[Clang documentation on __has_include_next](https://clang.llvm.org/docs/LanguageExtensions.html#has-include-next)

	\sa BURGER_CLANG

***************************************/

/*! ************************************

	\def __GNUC_PREREQ
	\brief GNUC version test macro

	On non GNUC compilers, this compiles to 0. This was taken from features.h

	[features.h documentation for this macro](https://sites.uclouvain.be/SystInfo/usr/include/features.h.html)

	\sa BURGER_GNUC

***************************************/
// clang-format on

/*! ************************************

	\def BURGER_API
	\brief Default parameter passing type for Burgerlib functions.

	On Intel platforms, there are several methods of passing parameters.
	Burgerlib uses the `__fastcall` method and this define will evaluate to
	`__fastcall` on compilers that support it. Otherwise, this will evaluate
	to nothing on non-Intel platforms.

	For non Intel platforms, this resolves into a blank \#define.

	\note Open Watcom uses __watcall

	\sa BURGER_ANSIAPI or BURGER_FASTCALLENABLED

***************************************/

/*! ************************************

	\def BURGER_ANSIAPI
	\brief Default parameter passing type for ANSI library functions.

	On Intel platforms, there are several methods of passing parameters. ANSI
	uses the __cdecl keyword, so any ANSI function like qsort() or atexit()
	that needs a callback function should have this keyword before the user
supplied callback. It will ensure that the linkage will be correct. This will
	evaluate to nothing on non-Intel platforms.

	\code
	static int BURGER_ANSIAPI Compare(const void *a,const void *b)
	{
		return strcmp((char *)a,(char *)b);
	}

	void SortData(void)
	{
		qsort(array,arraycount,sizeof(array[0]),Compare);
	}

	\endcode

	\sa BURGER_API

***************************************/

/*! ************************************

	\def BURGER_FASTCALLENABLED
	\brief Flag for determining Intel assembly generation

	This flag is defined only if the target platform is Intel and the parameter
	passing is in the `__fastcall` format.

	Some targets require parameters to be passed in the `__cdecl` format, so
	this will allow the selection of the proper assembly header and footer
	without having to create complicated \#ifdefs

	\sa BURGER_API, BURGER_ANSIAPI or BURGER_DECLSPECNAKED

***************************************/

/*! ************************************

	\def BURGER_INLINE
	\brief Define to create inline code.

	Since pre-C++ compilers created different keywords to force inlining of a
	function, this define standardizes the keyword. Normally, it will evaluate
	to inline, but it can become `__inline` or `__forceinline` or some other
	token that is compiler specific.

	In all cases, the behavior of the C++ inline keyword is what is expected.

	\sa BURGER_NO_INLINE or BURGER_NORETURN

***************************************/

/*! ************************************

	\def BURGER_NO_INLINE
	\brief Define to disable inline code.

	Since pre-C++ compilers created different keywords to disable inlining of a
	function, this define standardizes the keyword. Normally, it will evaluate
	to `__declspec(noinline)`, but it can become `__attribute__((noinline))`
	or some other token that is compiler specific.

	Some compilers don't support this keyword, for those compilers, this define
	resolves into nothing.

	\sa BURGER_INLINE

***************************************/

/*! ************************************

	\def BURGER_NORETURN
	\brief Define to alert the compiler the function does not return.

	Some compilers offer optimizations if it was aware that a function never
	returns. For functions that do not return, add this macro to the beginning
	of its definition to take advantage of this compiler feature.

	For C++ 11 or higher, this resolves to `[[noreturn]]`

	\sa BURGER_INLINE

***************************************/

// clang-format off
/*! ************************************

	\def BURGER_PRINTF_ATTRIBUTE
	\brief Define to declare \__attribute__((format)).

	Some compilers offer validation for ``printf`` string syntax. If a function
	accepts ``printf`` style input strings, the input can be validated with this
	macro.

	[Documentation on the compiler feature](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-format-function-attribute)

	\param _index 1 based index to the parameter that has the format string.
	\param _check 1 based index to the first variable argument.

	\sa BURGER_CLANG or BURGER_GNUC

***************************************/
// clang-format on

/*! ************************************

	\def BURGER_ASM
	\brief "C" compiler keyword for inline assembly.

	No one could agree on what would the keyword be to start a block of inline
	assembly. This will map to asm or _asm depending on the compiler.

	It's preferred that this keyword is used for Intel assembly.

	\sa BURGER_DECLSPECNAKED or BURGER_ANSIAPI

***************************************/

/*! ************************************

	\def BURGER_DECLSPECNAKED
	\brief "C" compiler keyword for a pure assembly function.

	No one could agree on what would the keyword be to declare a "C" function
	that is 100% assembly. This will map to asm or __declspec(naked) depending
	on the compiler.

	It's preferred that this keyword is used for Intel assembly only.

	\note Use of this keyword implies that there is no "C" glue code so setup,
	tear down and exiting must all be present in the assembly code.

	\code
	BURGER_DECLSPECNAKED uint32_t BURGER_API SwapEndian(uint32_t Val)
	{
		BURGER_ASM {
			mov		eax,ecx
			bswap	eax
			ret
		}
	}
	\endcode

	\sa BURGER_X86 or BURGER_FASTCALLENABLED

***************************************/

/*! ************************************

	\def BURGER_68K_A0
	\brief Macro for register passing for 68K compilers

	Use this macro for a pointer passed in the 68000 register A0.

	Metrowerks and the Apple SC compilers allow register passing if a register
	is specifically appended to a function parameter or a function declaration.
	On all other platforms, these macros will convert to nothing.

	\code

	// Have the function accept the parameter in D1 on 68K
	extern void foobar(int input BURGER_68K_D1);

	uint32_t bar(uint32_t *pInput BURGER_68K_A1) BURGER_68K_D1
	{
		// Input is passed in A1, and loaded into D1 for return
		// This compiles to a single instruction and a RET
		return *pInput;
	}

	\endcode

	\sa BURGER_68K_A1, BURGER_68K_D0, or BURGER_68K_D1

***************************************/

/*! ************************************

	\def BURGER_68K_A1
	\brief Macro for register passing for 68K compilers

	Use this macro for a pointer passed in the 68000 register A1.

	Metrowerks and the Apple SC compilers allow register passing if a register
	is specifically appended to a function parameter or a function declaration.
	On all other platforms, these macros will convert to nothing.

	\code

	// Have the function accept the parameter in D1 on 68K
	extern void foobar(int input BURGER_68K_D1);

	uint32_t bar(uint32_t *pInput BURGER_68K_A1) BURGER_68K_D1
	{
		// Input is passed in A1, and loaded into D1 for return
		// This compiles to a single instruction and a RET
		return *pInput;
	}

	\endcode

	\sa BURGER_68K_A0, BURGER_68K_D0, or BURGER_68K_D1

***************************************/

/*! ************************************

	\def BURGER_68K_D0
	\brief Macro for register passing for 68K compilers

	Use this macro for 8, 16, or 32 bit integer passed in the 68000 register D0.

	Metrowerks and the Apple SC compilers allow register passing if a register
	is specifically appended to a function parameter or a function declaration.
	On all other platforms, these macros will convert to nothing.

	\code

	// Have the function accept the parameter in D0 on 68K
	extern void foobar(int input BURGER_68K_D0);

	uint32_t bar(uint32_t *pInput BURGER_68K_A1) BURGER_68K_D0
	{
		// Input is passed in A1, and loaded into D0 for return
		// This compiles to a single instruction and a RET
		return *pInput;
	}

	\endcode

	\sa BURGER_68K_A0, BURGER_68K_A1, or BURGER_68K_D1

***************************************/

/*! ************************************

	\def BURGER_68K_D1
	\brief Macro for register passing for 68K compilers

	Use this macro for 8, 16, or 32 bit integer passed in the 68000 register D1.

	Metrowerks and the Apple SC compilers allow register passing if a register
	is specifically appended to a function parameter or a function declaration.
	On all other platforms, these macros will convert to nothing.

	\code

	// Have the function accept the parameter in D1 on 68K
	extern void foobar(int input BURGER_68K_D1);

	uint32_t bar(uint32_t *pInput BURGER_68K_A1) BURGER_68K_D1
	{
		// Input is passed in A1, and loaded into D1 for return
		// This compiles to a single instruction and a RET
		return *pInput;
	}

	\endcode

	\sa BURGER_68K_A0, BURGER_68K_A1, or BURGER_68K_D0

***************************************/

/*! ************************************

	\def BURGER_ALIGN
	\brief Macro to force data alignment.

	Some data needs to be aligned differently than native alignment. Use this
	macro to have a compiler agnostic way to ensure data is aligned in a method
	that is desired by the programmer.

	\note The size must be a power of two. Acceptable sizes are 1, 2, 4, 8, 16
	and 32. Larger are acceptable, as long as they are powers of two and
	supported by the compiler.

	\code
		BURGER_ALIGN(extern int, foo, 8);
		BURGER_ALIGN(CoolClass, MyClass, 1);
		BURGER_ALIGN(int, DataArray[4], 8) = {1,2,3,4};
	\endcode

	\param __type Type of the data to declare
	\param __name Name of the object
	\param __a Alignment value, must be power of 2

	\sa BURGER_NO_ALIGN, BURGER_PREALIGN and BURGER_POSTALIGN

***************************************/

/*! ************************************

	\def BURGER_PREALIGN
	\brief First half of a macro to force data alignment.

	Some data needs to be aligned differently than native alignment. However,
	it's sometimes impractical to use the \ref BURGER_ALIGN macro so this macro
	and its compliment are used to align complex objects like classes and
	structures. Use the code example below for proper use.

	\note The size must be a power of two. Acceptable sizes are 1, 2, 4, 8, 16
	and 32. Larger are acceptable, as long as they are powers of two.

	\code
	BURGER_PREALIGN(32) class foo {
	public:
		foo();
		int m_Data;		// This class is 32 byte aligned
	} BURGER_POSTALIGN(32);
	\endcode

	\param __a Alignment value, must be power of 2

	\sa BURGER_NO_ALIGN, BURGER_ALIGN and BURGER_POSTALIGN

***************************************/

/*! ************************************

	\def BURGER_POSTALIGN
	\brief Second half of a macro to force data alignment.

	Some data needs to be aligned differently than native alignment. However,
	it's sometimes impractical to use the \ref BURGER_ALIGN macro so this macro
	and its compliment are used to align complex objects like classes and
	structures. Use the code example below for proper use.

	\note The size must be a power of two. Acceptable sizes are 1, 2, 4, 8, 16
	and 32. Larger are acceptable, as long as they are powers of two.

	\code
	BURGER_PREALIGN(32) class foo {
	public:
		foo();
		int m_Data;		// This class is 32 byte aligned
	} BURGER_POSTALIGN(32);
	\endcode

	\param __a Alignment value, must be power of 2

	\sa BURGER_NO_ALIGN, BURGER_PREALIGN and BURGER_ALIGN

***************************************/

/*! ************************************

	\def BURGER_NO_ALIGN
	\brief Per object alignment is not supported.

	On older compilers, the ability to set the alignment of individual object is
	not supported. This macro exists on those compilers to alert code to be
	written to handle this case.

	\sa BURGER_PREALIGN, BURGER_POSTALIGN, BURGER_ALIGN, or BURGER_WATCOM

***************************************/

/*! ************************************

	\def BURGER_STRUCT_ALIGN
	\brief Defined if the compiler uses Macintosh style data alignment commands.

	Compilers can be set to change the default alignment of data structures. If
	alignment for a specific header needs to be set, you will need to insert
	this code as a prefix and a postfix to make sure that the compiler settings
	are overridden.

	This is useful for data structures that are mapped to the contents of a
	loaded file.

	\note Care must be exercised when changing alignment to byte or short. Some
	CPUs will generate a page fault if a 32 bit value is not 4 byte aligned.

	\code

	// Prefix for 4 byte alignment
	#if defined(BURGER_STRUCT_ALIGN)
	#pragma options align=power
	#elif defined(BURGER_STRUCT_PACKPUSH)
	#pragma pack(push,4)
	#elif defined(BURGER_STRUCT_PACK)
	#pragma pack(4)
	#endif

	// Postfix to restore compiler setting alignment
	#if defined(BURGER_STRUCT_ALIGN)
	#pragma options align=reset
	#elif defined(BURGER_STRUCT_PACKPUSH)
	#pragma pack(pop)
	#elif defined(BURGER_STRUCT_PACK)
	#pragma pack()
	#endif

	\endcode

	\sa BURGER_STRUCT_PACK or BURGER_STRUCT_PACKPUSH

***************************************/

/*! ************************************

	\def BURGER_STRUCT_PACKPUSH
	\brief Defined if the compiler uses Microsoft style data alignment commands.

	Compilers can be set to change the default alignment of data structures. If
	alignment for a specific header needs to be set, you will need to insert
	this code as a prefix and a postfix to make sure that the compiler settings
	are overridden.

	This is useful for data structures that are mapped to the contents of a
	loaded file.

	\note Care must be exercised when changing alignment to byte or short. Some
	CPUs will generate a page fault if a 32 bit value is not 4 byte aligned.

	\code

	// Prefix for 4 byte alignment
	#if defined(BURGER_STRUCT_ALIGN)
	#pragma options align=power
	#elif defined(BURGER_STRUCT_PACKPUSH)
	#pragma pack(push,4)
	#elif defined(BURGER_STRUCT_PACK)
	#pragma pack(4)
	#endif

	// Postfix to restore compiler setting alignment
	#if defined(BURGER_STRUCT_ALIGN)
	#pragma options align=reset
	#elif defined(BURGER_STRUCT_PACKPUSH)
	#pragma pack(pop)
	#elif defined(BURGER_STRUCT_PACK)
	#pragma pack()
	#endif

	\endcode

	\sa BURGER_STRUCT_ALIGN or BURGER_STRUCT_PACK

***************************************/

/*! ************************************

	\def BURGER_STRUCT_PACK
	\brief Defined if the compiler uses GNU style data alignment commands.

	Compilers can be set to change the default alignment of data structures. If
	alignment for a specific header needs to be set, you will need to insert
	this code as a prefix and a postfix to make sure that the compiler settings
	are overridden.

	This is useful for data structures that are mapped to the contents of a
	loaded file.

	\note Care must be exercised when changing alignment to byte or short. Some
	CPUs will generate a page fault if a 32 bit value is not 4 byte aligned.

	\code

	// Prefix for 4 byte alignment
	#if defined(BURGER_STRUCT_ALIGN)
	#pragma options align=power
	#elif defined(BURGER_STRUCT_PACKPUSH)
	#pragma pack(push,4)
	#elif defined(BURGER_STRUCT_PACK)
	#pragma pack(4)
	#endif

	// Postfix to restore compiler setting alignment
	#if defined(BURGER_STRUCT_ALIGN)
	#pragma options align=reset
	#elif defined(BURGER_STRUCT_PACKPUSH)
	#pragma pack(pop)
	#elif defined(BURGER_STRUCT_PACK)
	#pragma pack()
	#endif

	\endcode

	\sa BURGER_STRUCT_ALIGN or BURGER_STRUCT_PACKPUSH

***************************************/

/*! ************************************

	\def nullptr
	\brief Defined to nothing if the feature is not available.

	On compilers that support nullptr, this macro does not exist. To support
	this feature on older compilers, this macro exists to simulate the feature

	\sa NULL, BURGER_CPP11, BURGER_OVERRIDE or BURGER_CONSTEXPR

***************************************/

/*! ************************************

	\def __underlying_type
	\brief Defined if __underlying_type is not available.

	If the feature __underlying_type isn't supported by the compiler, this
	macro will be defined to `int` to allow code to compile on older
	compilers.

	\sa BURGER_CPP11, nullptr or BURGER_CONSTEXPR

***************************************/

/*! ************************************

	\def BURGER_STATIC_ASSERT(x)
	\brief Test a compile time condition and if it's false, force a compiler
		error

	Use either typedef int ThisIsABogusTypeDef ## __LINE__ [(x) * 2 - 1] or
	static_assert() to determine if a conditional that can be determined at
	compile time should trigger an assert. This macro does not generate any
	runtime code.

	\note This macro will always invoke the conditional in all builds. The
	conditional must be one that is resolved at compile time.

	\note If this macro is used in a template, it can only be used once if an
		old GNUC compiler (4.2 or earlier) is used because the compiler doesn't
		change the __LINE__ number which can cause issues. If the compiler
		supports the static_assert keyword, this is not a problem.

	\param x A boolean that evaluates to \ref FALSE to force a compile time
		error

	\code
	// Will succeed
	BURGER_STATIC_ASSERT(sizeof(uint32_t)==sizeof(int32_t));
	// Will fail
	BURGER_STATIC_ASSERT(sizeof(char)==sizeof(int));
	// Will fail if sizeof(eType) != sizeof(int)
	enum eType {
		TEST
	};
	BURGER_STATIC_ASSERT(sizeof(eType)==sizeof(int));

	// Use BURGER_ASSERT(x) instead, since this example is resolved at runtime
	BURGER_STATIC_ASSERT(Burger::StringLength("EpicFail")==8);
	\endcode

***************************************/

/*! ************************************

	\def BURGER_CONSTEXPR
	\brief Defined to nothing if constexpr is not supported.

	On compilers that support the keyword constexpr, this define will
	be set to constexpr, otherwise this define will be defined to nothing.

	\code

	// Foo is code that can generate a runtime constant
	// with this keyword. If the input values are
	// constants, the entire function will be optimized
	// away and a new constant will be created

	BURGER_CONSTEXPR int Foo(int a,int b)
	{
		return a*b;
	}

	void Sample(void)
	{
		// This will compile to iNewConstant = 6 on C++14 compilers
		// or a call to Foo(2,3) on older compilers that
		// don't perform Link Time Code Generation

		int iNewConstant = Foo(2,3);
	}

	\endcode

	\sa BURGER_CPP11, BURGER_FINAL, BURGER_OVERRIDE or nullptr

***************************************/

/*! ************************************

	\def BURGER_NOEXCEPT
	\brief Defined to nothing if noexcept is not supported.

	On compilers that support the keyword noexcept, this define will
	be set to noexcept, otherwise this define will be defined to nothing.

	\sa BURGER_CPP11, BURGER_FINAL, BURGER_OVERRIDE or nullptr

***************************************/

/*! ************************************

	\def BURGER_OVERRIDE
	\brief Defined to nothing if the feature is not available.

	On compilers that support override, this macro is set to override. To
	support this feature on older compilers, this macro is set to nothing.

	\sa BURGER_CPP11, nullptr, BURGER_FINAL or BURGER_CONSTEXPR

***************************************/

/*! ************************************

	\def BURGER_FINAL
	\brief Defined to nothing if the feature is not available.

	On compilers that support `final`, this macro is set to `final`. To
	support this feature on older compilers, this macro is set to nothing.

	Microsoft Visual C can set this to `sealed`, or for GNU compilers it is
	set to `__final`.

	\sa BURGER_CPP11, nullptr, BURGER_OVERRIDE or BURGER_CONSTEXPR

***************************************/

/*! ************************************

	\def BURGER_EQUALS_DELETE
	\brief Declare "= delete" on C++ 11 compilers

	Some class functions can be disabled in C++11 by the use of the
	term "= delete" suffix. Of the compiler supports this feature, this
	macro will map to "= delete", otherwise the macro will be blank.

	\code
	class DontCopyMe {
		// Disallow copying of this class
		DontCopyMe(const &) BURGER_EQUALS_DELETE;
		DontCopyMe& operator=(DontCopyMe const&) BURGER_EQUALS_DELETE;
	public:
		void DoStuff(void);

	\endcode

	\sa BURGER_DISABLE_COPY, or BURGER_EQUALS_DEFAULT

***************************************/

/*! ************************************

	\def BURGER_EQUALS_DEFAULT
	\brief Declare "= default" on C++ 11 compilers

	Some default constructors can be created in C++11 by the use of the
	term "= default" suffix. Of the compiler supports this feature, this
	macro will map to "= default", otherwise the macro will be blank.

	\code
	class DefaultClass {
		// Default constructor
		DefaultClass() BURGER_EQUALS_DEFAULT;

	public:
		void DoStuff(void);

	\endcode

	\sa BURGER_EQUALS_DELETE or BURGER_DEFAULT_DESTRUCTOR

***************************************/

/*! ************************************

	\def BURGER_DEFAULT_CONSTRUCTOR
	\brief Declare "= default" on C++ 11 compilers

	For a default constructor, "= default" is the preferred method for C++ 11 or
	higher compilers, while earlier compilers prefer {} for default
	constructors. This macro will expand to the preferred format for creating
	default constructors.

	\code
	class DefaultClass {
		// Default constructor
		DefaultClass() BURGER_DEFAULT_CONSTRUCTOR;

	public:
		void DoStuff(void);

	\endcode

	\sa BURGER_EQUALS_DEFAULT or BURGER_DEFAULT_DESTRUCTOR

***************************************/

/*! ************************************

	\def BURGER_DEFAULT_DESTRUCTOR
	\brief Declare "= default" on C++ 11 compilers

	For a default destructor, "= default" is the preferred method for C++ 11 or
	higher compilers, while earlier compilers prefer {} for default destructors.
	This macro will expand to the preferred format for creating default
	destructors.

	\code
	class DefaultClass {
		// Default destructor
		~DefaultClass() BURGER_DEFAULT_DESTRUCTOR;

	public:
		void DoStuff(void);

	\endcode

	\sa BURGER_DEFAULT_CONSTRUCTOR, BURGER_EQUALS_DEFAULT or
		BURGER_EQUALS_DELETE

***************************************/

/*! ************************************

	\def BURGER_RVALUE_REFERENCES
	\brief Defined if move semantics are available.

	On compilers that support move constructors, this define
	will exist.

	\sa BURGER_CPP11, nullptr, BURGER_OVERRIDE or BURGER_CONSTEXPR

***************************************/

/*! ************************************

	\def BURGER_ADDRESS_SANITIZER
	\brief Detect if address sanitization is active.

	If the code is being built with Address Sanitization, this macro will exist.

	\sa BURGER_DISABLE_ASAN, BURGER_MSVC or BURGER_CLANG

***************************************/

/*! ************************************

	\def BURGER_DISABLE_ASAN
	\brief Disable address sanitization.

	If the code is being built with Address Sanitization, this macro will
	disable testing any function that is marked. It's to work around false
	positives.

	\sa BURGER_ADDRESS_SANITIZER, BURGER_DISABLE_MSAN, BURGER_MSVC, or
		BURGER_CLANG

***************************************/

/*! ************************************

	\def BURGER_MEMORY_SANITIZER
	\brief Detect if memory sanitization is active.

	If the code is being built with Memory Sanitization, this macro will exist.

	\sa BURGER_DISABLE_MSAN or BURGER_CLANG

***************************************/

/*! ************************************

	\def BURGER_DISABLE_MSAN
	\brief Disable memory sanitization.

	If the code is being built with Memory Sanitization, this macro will disable
	testing any function that is marked. It's to work around false positives.

	\sa BURGER_MEMORY_SANITIZER, BURGER_DISABLE_ASAN or BURGER_CLANG

***************************************/

/*! ************************************

	\def BURGER_MAYBE_UNUSED
	\brief Mark with [[maybe_unused]]

	For C++17 or higher compilers, or those that support the equivalent, insert
	the [[maybe_unused]] attribute to the code.

	[Docs are found
	here.](https://en.cppreference.com/w/cpp/language/attributes/maybe_unused)

	\sa BURGER_NODISCARD, BURGER_USED or BURGER_FALLTHROUGH

***************************************/

/*! ************************************

	\def BURGER_NODISCARD
	\brief Mark with [[nodiscard]]

	For C++17 or higher compilers, or those that support the equivalent, insert
	the [[nodiscard]] attribute to the code.

	[Docs are found
	here.](https://en.cppreference.com/w/cpp/language/attributes/nodiscard)

	\sa BURGER_MAYBE_UNUSED, BURGER_USED or BURGER_FALLTHROUGH

***************************************/

/*! ************************************

	\def BURGER_FALLTHROUGH
	\brief Mark with [[fallthrough]]

	For C++17 or higher compilers, or those that support the equivalent, insert
	the [[fallthrough]] attribute to the code.

	[Docs are found
	here.](https://en.cppreference.com/w/cpp/language/attributes/fallthrough)

	\sa BURGER_NODISCARD or BURGER_MAYBE_UNUSED

***************************************/

/*! ************************************

	\def BURGER_USED
	\brief Mark with __attribute__((used))

	Some compilers remove functions and variables that are not used by the
	program. To force a function or variable to be compiled and linked into the
	final product, mark it with this macro.

	[Docs are found here
	](https://gcc.gnu.org/onlinedocs/gcc-4.8.5/gcc/Variable-Attributes.html)

	\sa BURGER_NODISCARD or BURGER_MAYBE_UNUSED

***************************************/

/*! ************************************

	\def BURGER_DEPRECATED
	\brief Mark with __attribute__((deprecated))

	If a function or class is considered to be deprecated, use this macro to
	alert that the function or class should no longer be used in the future.

	[Docs are found here
	](https://en.cppreference.com/w/cpp/language/attributes/deprecated)

	\note If the compiler doesn't support this keyword, it will do nothing.

	\param __x String with a message to explain why this object is deprecated.

	\sa BURGER_NODISCARD or BURGER_MAYBE_UNUSED

***************************************/

/*! ************************************

	\def BURGER_OFFSETOF
	\brief Define to return the offset of a member variable.

	Return the byte offset of a member variable from a class or struct.

	\param __type Name of the class / struct type
	\param __member Name of the member in the type to determine the offset of.

	\code
	struct foo {
		int32_t m_member;
		int32_t m_member2;
	};

	// Will equal to 4
	intptr_t offsetofmember2 = BURGER_OFFSETOF(foo,m_member2);

	\endcode

	\sa BURGER_GET_BASE_PTR, or BURGER_CONST_GET_BASE_PTR

***************************************/

/*! ************************************

	\def BURGER_GET_BASE_PTR
	\brief Define to return the base pointer of a class from a class member

	Given a pointer to a member variable of a class, calculate the base pointer
	of the class using the name of the member variable.

	\code
	struct foo {
		int m_member;
		int m_member2;
	};

	struct bar {
		int m_Temp;
		int m_Temp2;
		foo m_Foo;		// Member for the test
	};

	// Structure to work with
	bar Test;

	// Get pointer to a member variable
	foo *pFoo = &Test.m_Foo;

	// Use the macro to convert the member pointer back into a bar *
	bar *pBar = BURGER_GET_BASE_PTR(pFoo, bar, m_Foo);

	\endcode

	\param x Pointer to the member variable that needs fix up
	\param __type Name of the class / struct type
	\param __member Name of the member in the type to determine the offset of.

	\sa BURGER_OFFSETOF, BURGER_CONST_GET_BASE_PTR

***************************************/

/*! ************************************

	\def BURGER_CONST_GET_BASE_PTR
	\brief Define to return the const base pointer of a class from a class
		member

	Given a const pointer to a member variable of a class, calculate the const
	base pointer of the class using the name of the member variable.

	\code
	struct foo {
		int m_member;
		int m_member2;
	};

	struct bar {
		int m_Temp;
		int m_Temp2;
		foo m_Foo;		// Member for the test
	};

	// Structure to work with
	bar Test;

	// Get pointer to a member variable
	const foo *pFoo = &Test.m_Foo;

	// Use the macro to convert the member pointer back into a bar *
	const bar *pBar = BURGER_CONST_GET_BASE_PTR(pFoo, bar, m_Foo);

	\endcode

	\param x Const pointer to the member variable that needs fix up
	\param __type Name of the class / struct type
	\param __member Name of the member in the type to determine the offset of.

	\sa BURGER_OFFSETOF, BURGER_GET_BASE_PTR

***************************************/

/*! ************************************

	\def BURGER_UNUSED
	\brief Define to disable the unused variable warning.

	In cases where a variable is defined, but not used, such as in testing error
	conditions with asserts, this macro will alert the compiler that the
	existence of this variable was intentional.

	\param x Name of the variable to allow to be unused

	\code

	// Ignore errors on release builds
	#if defined(NDEBUG)
	#define I_FEAR_NOTHING(x)
	#else
	#define I_FEAR_NOTHING(x) x
	#endif

	int iError = FunctionThatCanFail();
	// Disable unused variable warning
	BURGER_UNUSED(iError);

	I_FEAR_NOTHING(printf("Error code %d", iError));

	\endcode

***************************************/

/*! ************************************

	\def BURGER_ROUNDUP
	\brief Define to round up an integer to the next power of 2.

	For compile time constants, use this macro to take a sizeof() value and a
	power of 2 alignment value to round up the value to the desired power of 2.

	If the value is already at that power of 2, it will be returned unchanged.

	\note If __alignment is not a power of 2, undefined behavior will occur.

	\code

	struct V {
		uint8_t Foo;
	} Value;

	// Ensure the structure size is a power of 8
	uintptr_t uNewSize = BURGER_ROUNDUP(sizeof(Value), 8);

	\endcode

	\param __value Integer value to apply the alignment
	\param __alignment Integer power of 2.

	\sa Burger::power_of_two(uint32_t), or Burger::power_of_two(uint64_t)

***************************************/

/*! ************************************

	\def BURGER_MSVC_SUPPRESS
	\brief Invoke \#pragma warning(suppress:) in Microsoft Compilers

	Microsoft compilers are very thorough in finding issues with code, but
	sometimes it flags warnings on perfectly acceptable code. This macro will
	suppress the warning on the following line of code after this macro and then
	allow the warning to be invoked on the rest of the code.

	\note On non-Microsoft compilers, this macro does nothing.

	\code

	// Suppress the warning on the variable below
	// Global initializer calls a non-constexpr function 'symbol'
	BURGER_MSVC_SUPPRESS(26426)

	// Init value on startup
	static DWORD gStorage = TlsAlloc();

	\endcode

	\note Visual Studio 2003 doesn't support the suppress keyword, so disable is
		used instead.

	\param __T Integer warning number to suppress

	\sa BURGER_MSVC

***************************************/

/*! ************************************

	\def BURGER_DISABLE_COPY
	\brief Declares copy constructors to disable invocation at runtime.

	Some classes by their nature shouldn't be or can't be copied, by declaring
	this macro, the x(x const &) constructor and the x &operator = (x const &)
	operator are declared. If compiled with C++11 or higher, the x(x &&) and
	x& operator = (x &&) are also declared as '= delete'.

	\note This macro MUST be invoked in a private section of a class or it will
	inadvertently declare that there are copy constructors in existence which
	could lead to mysterious link errors.

	\code
	class DontCopyMe {
		// Disallow copying of this class
		BURGER_DISABLE_COPY(DontCopyMe);
	public:
		void DoStuff(void);

	\endcode

	\note This macro will set the class setting to "private"

	\param x Name of the class that the macro resides in

	\sa BURGER_EQUALS_DELETE

***************************************/

/*! ************************************

	\def BURGER_ENUM_TYPE(x,y)
	\brief Creates a cross platform ``enum eSample : uint32_t {`` instance

	This macro maps to ``enum eSample {`` on older compilers.

	Newer compilers support enum types where the data type can be declared for
	space or if signed. Older compilers declare enum as ``int`` or data type
	size. To allow the usage of the ``enum eSample : int {``, use this macro.

	\note The enum is unscoped, hence all members are declared in the current
		namespace.

	\code

	// Declare eTestEnum members taking uint_t size.

	BURGER_ENUM_TYPE(eTestEnum, uint_t) {
		VALUE0,
		VALUE1,
		VALUE2
	};

	// Call this function with the declared enum type of eTestEnum

	void foo(eTestEnum uValue)
	{
		// Test each value with eTestEnum:: prefixes
		switch (uValue) {
		case VALUE0:
			printf("VALUE0\n");
			break;
		case VALUE1:
			printf("VALUE1\n");
			break;
		case VALUE0:
			printf("VALUE2\n");
			break;
		}
	}

	\endcode

	\param x Name of the new enum
	\param y Desired data type for the new enum on modern compilers

	\sa BURGER_ENUM_CLASS_START and BURGER_ENUM_MATH

***************************************/

/*! ************************************

	\def BURGER_ENUM_CLASS_START(x,y)
	\brief Creates a cross platform "enum class : int {" instance

	This macro maps to "struct x { enum Type {" on older compilers.

	Newer compilers support strong enum types where it's impossible to test one
	enum type with another due to the implicit conversion to integer. This is
	called "enum class". This macro offers this support while still offering
	backwards compatibility with C89 and C99 compilers. The solution was to use
	"enum class" on compilers that support it, and use a namespace to
	encapsulate the enum members on older compilers (Or compilers that have
	extensions turned off).

	There were compromises made, enums need to be generated with a start and end
	macro so they could ensure the members are either inside a namespace or an
	enum class. However, this caused a difference in the final type, with it
	either being a global type name or a name embedded in a namespace. To get
	around this, a unified type was generated called "???Type" where the ??? is
	replaces with the name of the new enum and it's declared as a typedef to
	either the enum class or "enum Type" that resides in the namespace.

	\note Always use the declared enum type to reference the members and ???
	Type to declare the enum as a member variable or parameter. Secondly, the
	size of the data is only enforced on compilers that offer "enum class foo :
	char" or "enum foo : char" type declarations, otherwise, the compiler will
	likely map the enum to a signed integer.

	\code

	// Declare eTestEnum members and the eTestEnumType

	BURGER_ENUM_CLASS_START(eTestEnum,uint_t) {
		value0,
		value1,
		value2
	} BURGER_ENUM_CLASS_END(eTestEnum)

	// Call this function with the global enum type of eTestEnumType

	void foo(eTestEnumType uValue)
	{
		// Test each value with eTestEnum:: prefixes
		switch (uValue) {
		case eTestEnum::value0:
			printf("value0\n");
			break;
		case eTestEnum::value1:
			printf("value1\n");
			break;
		case eTestEnum::value0:
			printf("value2\n");
			break;
		}
	}

	\endcode

	\param x Name of the new enum
	\param y Desired data type for the new enum

	\sa BURGER_ENUM_TYPE, BURGER_ENUM_CLASS_END and BURGER_ENUM_CLASS_END_MATH

***************************************/

/*! ************************************

	\def BURGER_ENUM_CLASS_END(x)
	\brief Finish a cross platform "enum class : int {" instance

	This macro maps to "; x() {}; }" on older compilers.

	\note Read the description of \ref BURGER_ENUM_CLASS_START for full details.

	\code

	// Declare eTestEnum members and the eTestEnumType

	BURGER_ENUM_CLASS_START(eTestEnum,uint_t) {
		value0,
		value1,
		value2
	} BURGER_ENUM_CLASS_END(eTestEnum)

	// Call this function with the global enum type of eTestEnumType

	void foo(eTestEnumType uValue)
	{
		// Test each value with eTestEnum:: prefixes
		switch (uValue) {
		case eTestEnum::value0:
			printf("value0\n");
			break;
		case eTestEnum::value1:
			printf("value1\n");
			break;
		case eTestEnum::value0:
			printf("value2\n");
			break;
		}
	}

	\endcode

	\param x Name of the new enum (Must match the name used in preceding \ref
		BURGER_ENUM_CLASS_START(x)

	\sa BURGER_ENUM_CLASS_START and BURGER_ENUM_CLASS_END_MATH

***************************************/

/*! ************************************

	\def BURGER_ENUM_MATH(x,y)
	\brief Create math functions to handle an enum.

	This macro will create a list of operator overloads so the bit values can be
	merged and tested with strong type safety.

	Binary operators that are considered valid for flags are AND (&), OR (|) and
	XOR (^) and unary operations NOT (!) and bitwise XOR (~).

	The operation overload will enforce the data type declared for the enum, so
	operators such as bitwise XOR (~) will only be as wide as the declared
	underlying type.

	\note Read the description of \ref BURGER_ENUM_CLASS_START for full details.

	\code

	// Declare eTestFlags members and the eTestFlagsType

	BURGER_ENUM_TYPE(eTestFlags, uint8_t) {
		None=0,					// No flag present
		Flag1 = 0x01,			// First bit flag
		Flag2 = 0x02,			// Second bit flag
		Flag3 = 0x04			// You get the drift
	};

	BURGER_ENUM_MATH(eTestFlags, uint8_t)

	// Call this function with the enum

	void foo(eTestFlags uFlags)
	{
		// A flag was set?
		if (uFlags!=None) {
			// Test the flags and print
			if ((uFlags&Flag1)!=None) {
				printf("Flag1 found\n");
			}
			if ((uFlags&Flag2)!=None) {
				printf("Flag2 found\n");
			}
			if ((uFlags&Flag3)!=None) {
				printf("Flag3 found\n");
			}
			if ((uFlags&(Flag2|Flag3))!=None) {
				printf("Flag2 or Flag3 found\n");
			}
			if ((uFlags&(Flag2|Flag3))==(Flag2|Flag3)) {
				printf("Flag2 and Flag3 found\n");
			}
		}
	}

	void bar(void)
	{
		foo(None);
		foo(Flag2);
		foo(Flag2|Flag3);
	}

	\endcode

	\param x Name of the enum to accept as input.

	\param y Data type of the enum, must match the type used on the enum's
		declaration.

	\sa BURGER_ENUM_TYPE and BURGER_ENUM_CLASS_END_MATH

***************************************/

/*! ************************************

	\def BURGER_ENUM_CLASS_END_MATH(x,y)
	\brief Finish a cross platform "enum class : int {" instance for flags.

	\details This macro will invoke \ref BURGER_ENUM_CLASS_END and then add a
	list of operator overloads so the bit values can be merged and tested with
	strong type safety.

	Binary operators that are considered valid for flags are AND (&), OR (|) and
	XOR (^) and unary operations NOT (!) and bitwise XOR (~).

	The operation overload will enforce the data type declared for the enum, so
	operators such as bitwise XOR (~) will only be as wide as the declared
	underlying type.

	\note Read the description of \ref BURGER_ENUM_CLASS_START for full details.

	\code

	// Declare eTestFlags members and the eTestFlagsType

	BURGER_ENUM_CLASS_START(eTestFlags,Byte) {
		None=0,					// No flag present
		Flag1 = 0x01,			// First bit flag
		Flag2 = 0x02,			// Second bit flag
		Flag3 = 0x04			// You get the drift
	} BURGER_ENUM_CLASS_END_MATH(eTestFlags,Byte)

	// Call this function with the global enum type of eTestFlagsType

	void foo(eTestFlagsType uFlags)
	{
		// A flag was set?
		if (uFlags!=eTestFlags::None) {
			// Test the flags and print
			if ((uFlags&eTestFlags::Flag1)!=eTestFlags::None) {
				printf("Flag1 found\n");
			}
			if ((uFlags&eTestFlags::Flag2)!=eTestFlags::None) {
				printf("Flag2 found\n");
			}
			if ((uFlags&eTestFlags::Flag3)!=eTestFlags::None) {
				printf("Flag3 found\n");
			}
			if ((uFlags&(eTestFlags::Flag2|eTestFlags::Flag3))!=
				eTestFlags::None) {
				printf("Flag2 or Flag3 found\n");
			}
			if ((uFlags&(eTestFlags::Flag2|eTestFlags::Flag3))==
				(eTestFlags::Flag2|eTestFlags::Flag3)) {
				printf("Flag2 and Flag3 found\n");
			}
		}
	}

	void bar(void)
	{
		foo(eTestFlags::None);
		foo(eTestFlags::Flag2);
		foo(eTestFlags::Flag2|eTestFlags::Flag3);
	}

	\endcode

	\param x Name of the new enum (Must match the name used in preceding \ref
		BURGER_ENUM_CLASS_START(z)

	\param y Desired data type for the new enum

	\sa BURGER_ENUM_CLASS_START and BURGER_ENUM_CLASS_END_MATH

***************************************/

/*! ************************************

	\def BURGER_SIZEOF_INT
	\brief Size in bytes of the intrinsic int

	Define that has the number of bytes an `int` occupies. This value can be 2,
	4 or 8. Default is 4. This exists so `sizeof(int)` can be used as a macro
	parameter on older compilers.

	\sa BURGER_SIZEOF_LONG

***************************************/

/*! ************************************

	\def BURGER_LONGLONG
	\brief Signed 64 bit integer type specific to the current compiler.

	Define that has the compiler keyword that defines a 64 bit signed integer.
	Examples include `__int64`, `long`, or `long long`.

	\sa BURGER_ULONGLONG

***************************************/

/*! ************************************

	\def BURGER_ULONGLONG
	\brief Unsigned 64 bit integer type specific to the current compiler.

	Define that has the compiler keyword that defines a 64 bit unsigned integer.
	Examples include `unsigned __int64`, `unsigned long`, or unsigned `long
	long`.

	\sa BURGER_LONGLONG

***************************************/

/*! ************************************

	\def BURGER_SIZEOF_LONG
	\brief Size in bytes of the intrinsic long

	Define that has the number of bytes a `long` occupies. This value can be 4
	or 8. Default is 4. This exists so `sizeof(long)` can be used as a macro
	parameter on older compilers.

	\sa BURGER_SIZEOF_INT

***************************************/

/*! ************************************

	\def BURGER_INT_NOT_IN_STDINT
	\brief Is `int` used in stdint.h

	If this define is present, `int` isn't used in the int32_t family of data
	types, so if a series of functions wish to allow a function to be
	automatically called using `int` as a parameter, a function specifically
	using `int` as an input must be declared.

	\sa BURGER_LONG_NOT_IN_STDINT

***************************************/

/*! ************************************

	\def BURGER_LONG_NOT_IN_STDINT
	\brief Is `long` used in stdint.h

	If this define is present, `long` isn't used in the int32_t family of
	data types, so if a series of functions wish to allow a function to be
	automatically called using `long` as a parameter, a function specifically
	using `long` as an input must be declared.

	\sa BURGER_INT_NOT_IN_STDINT

***************************************/

/*! ************************************

	\typedef int2int_t
	\brief `int` to int??_t.

	Data type to convert native `int` to the `int32_t`, `int16_t`
	or other compatible type that reflects the proper size.

	\sa uint2uint_t

***************************************/

/*! ************************************

	\typedef uint2uint_t
	\brief `unsigned int` to uint??_t.

	Data type to convert native `unsigned int` to the `uint32_t`,
	`uint16_t` or other compatible type that reflects the proper size.

	\sa int2int_t

***************************************/

/*! ************************************

	\typedef long2int_t
	\brief `long` to int??_t.

	Data type to convert native `long` to the `int32_t`, `int64_t`
	or other compatible type that reflects the proper size.

	\sa ulong2uint_t

***************************************/

/*! ************************************

	\typedef ulong2uint_t
	\brief `unsigned long` to uint??_t.

	Data type to convert native `unsigned long` to the `uint32_t`,
	`uint64_t` or other compatible type that reflects the proper size.

	\sa long2int_t

***************************************/

/*! ************************************

	\def BURGER_HAS_WCHAR_T
	\brief Data type `wchar_t` is native

	If this define is present, `wchar_t` is a native type for the compiler,
	otherwise, it's a typedef cast from an `unsigned short`, which may cause
	collisions for classes that want to treat `wchar_t` and `uint16_t` as unique
	data types.

	\sa BURGER_HAS_CHAR8_T or BURGER_HAS_CHAR16_T

***************************************/

/*! ************************************

	\def BURGER_HAS_CHAR8_T
	\brief Data type `char8_t` is native

	If this define is present, `char8_t` is a native type for the compiler.

	\sa BURGER_CPP20, BURGER_HAS_WCHAR_T or BURGER_HAS_CHAR16_T

***************************************/

/*! ************************************

	\def BURGER_HAS_CHAR16_T
	\brief Data type `char16_t` and `char32_t` are native

	If this define is present, `char16_t` and `char32_t` are a native types for
	the compiler.

	\sa BURGER_CPP11, BURGER_HAS_CHAR8_T or BURGER_HAS_WCHAR_T

***************************************/

/*! ************************************

	\struct Burger::longlong_t
	\brief 64 bit signed integer as 2 32 bit integers.

	To fake 64 bit support on old compilers that don't natively support 64 bit
	integers, this structure is declared as a replacement to create limited 64
	bit support.

	\sa ulonglong_t

***************************************/

/*! ************************************

	\fn Burger::longlong_t::operator ulonglong_t&()
	\brief Convert unsigned to signed fake 64 bit integer

	To fake 64 bit support on old compilers that don't natively support 64 bit
	integers, this structure is declared as a replacement to create limited 64
	bit support.

	\sa ulonglong_t::operator longlong_t&()

***************************************/

/*! ************************************

	\struct Burger::ulonglong_t
	\brief 64 bit unsigned integer as 2 32 bit integers.

	To fake 64 bit support on old compilers that don't natively support 64 bit
	integers, this structure is declared as a replacement to create limited 64
	bit support.

	\sa longlong_t

***************************************/

/*! ************************************

	\fn Burger::ulonglong_t::operator longlong_t&()
	\brief Convert signed to unsigned fake 64 bit integer

	To fake 64 bit support on old compilers that don't natively support 64 bit
	integers, this structure is declared as a replacement to create limited 64
	bit support.

	\sa longlong_t::operator ulonglong_t&()

***************************************/

/*! ************************************

	\def BURGER_ARRAYSIZE
	\brief Define to return the number of elements in an array.

	For arrays where the size is known at compile time, this macro will return
	the number of elements found in the array

	\note On most compilers, this is implemented as a template. On older
	compilers it's declared as a simple division operation.

	\code

	int Foo[100];

	// Will equal to 100
	int ElementsInFoo = BURGER_ARRAYSIZE(Foo);

	\endcode

	\param x Name of the array to determine the size

	\sa _BurgerArraySize

***************************************/

/*! ************************************

	\fn const char(*_BurgerArraySize(T(&)[N]))[N]
	\brief Helper for \ref BURGER_ARRAYSIZE

	Non-existent array for determining array element size.

	\tparam T Data type of the elements of the array/
	\tparam N Number of elements in the array

	\sa BURGER_ARRAYSIZE

***************************************/

/*! ************************************

	\typedef fixed16_16_t
	\brief Signed 32 bit integer for fixed point operations.

	All 16.16 fixed point integer math uses this data type to alert the
	programmer of the special nature of the 32 bits. The upper 16 bits is an
	integer of the range of -32768 through 32767 and the lower 16 bits is a
	fraction of x/65536.

	\sa \ref BURGER_FLOAT_TO_FIXED, \ref BURGER_FIXED_TO_FLOAT, fixed2_30_t.

***************************************/

/*! ************************************

	\typedef fixed2_30_t
	\brief Signed 32 bit integer for fixed point operations.

	All 2.30 fixed point integer math uses this data type to alert the
	programmer of the special nature of the 32 bits. The upper 2 bits is an
	integer of the range of -2 through 1 and the lower 30 bits is a fraction of
	x/1,073,741,824.

	\sa fixed16_16_t

***************************************/

/*! ************************************

	\typedef uint_t
	\brief Unsigned integer.

	This is the most common data type in BurgerLib. On most platforms, this is
	an unsigned 32 bit integer but it can be 64 bit if the processor handles 64
	bit data more efficiently than 32 bit. To date, all CPUs handle 32 bit data
	with no penalty so it's locked at 32 bits.

	\sa int_t

***************************************/

/*! ************************************

	\typedef int_t
	\brief Signed integer.

	This is the second most common data type in BurgerLib. On most platforms,
	this is an signed 32 bit integer but it can be 64 bit if the processor
	handles 64 bit data more efficiently than 32 bit. To date, all CPUs handle
	32 bit data with no penalty so it's locked at 32 bits.

	\sa uint_t

***************************************/

#endif
