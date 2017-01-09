/***************************************

	Determine which compiler is being used and
	create standardized typedefs and macros
	so generic code can be created cross platform

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#if defined(DOXYGEN)
#define _DEBUG
#define NDEBUG

#define BURGER_X86
#define BURGER_AMD64
#define BURGER_MIPS
#define BURGER_POWERPC
#define BURGER_68K
#define BURGER_ARM
#define BURGER_64BITCPU
#define BURGER_INTELARCHITECTURE

//#define BURGER_BIGENDIAN
#define BURGER_LITTLEENDIAN

#define BURGER_WATCOM
#define BURGER_MRC
#define BURGER_APPLESC
#define BURGER_INTELCOMPILER
#define BURGER_SNSYSTEMS
#define BURGER_METROWERKS
#define BURGER_MSVC
#define BURGER_GNUC
#define BURGER_LLVM

#define BURGER_WIN32
#define BURGER_WIN64
#define BURGER_WINDOWS
#define BURGER_MSDOS
#define __DOS4G__
#define __X32__
#define BURGER_MAC
#define BURGER_MACOSX
#define BURGER_MACCARBON
#define BURGER_MACCLASSIC
#define BURGER_MACOS
#define TARGET_API_MAC_CARBON 1
#define BURGER_CFM
#define BURGER_IOS
#define BURGER_NGAGE
#define BURGER_SYMBIAN
#define BURGER_PS1
#define BURGER_PS2
#define BURGER_PS3
#define BURGER_PS4
#define BURGER_PSP
#define BURGER_VITA
#define BURGER_GAMECUBE
#define BURGER_WII
#define BURGER_WIIU
#define BURGER_GBA
#define BURGER_DS
#define BURGER_3DS
#define BURGER_XBOX
#define BURGER_XBOX360
#define BURGER_XBOXONE
#define BURGER_ANDROID
#define BURGER_SHIELD
#define BURGER_OUYA
#define BURGER_BEOS

//#define TRUE 1
//#define FALSE 0
//#define NULL 0
//#define BURGER_MAXINT 0x7FFFFFFF
//#define BURGER_MAXUINT 0xFFFFFFFFU
//#define BURGER_MAXWORDPTR 0xFFFFFFFFFFFFFFFFULL
//#define BURGER_MAXINTPTR 0x7FFFFFFFFFFFFFFFLL

//#define BURGER_INLINE __forceinline
//#define BURGER_API __fastcall
//#define BURGER_ANSIAPI __cdecl
//#define BURGER_ASM asm
#define BURGER_FASTCALLENABLED
//#define BURGER_DECLSPECNAKED __declspec(naked)
//#define BURGER_ALIGN(x,s) __align(s) (x)
//#define BURGER_PREALIGN(s) __align(s)
//#define BURGER_POSTALIGN(s)
#define BURGER_STRUCT_ALIGN
#define BURGER_STRUCT_PACK
#define BURGER_STRUCT_PACKPUSH
#define BURGER_NO_USING_NAMESPACE
//#define BURGER_OFFSETOF(type, member) reinterpret_cast<WordPtr>(&(reinterpret_cast<const type *>(0)->member))
//#define BURGER_HASHMACRO(x) #x
//#define BURGER_MACRO_TO_STRING(x) BURGER_HASHMACRO(x)

#define BURGER_LONGLONG __int64
#define BURGER_LONGIS64BIT
#define BURGER_HASWCHAR_T
//typedef signed char Int8;
//typedef unsigned char Word8;
//typedef signed short Int16;
//typedef unsigned short Word16;
//typedef signed int Int32;
//typedef unsigned int Word32;
//typedef signed __int64 Int64;
//typedef unsigned __int64 Word64;
//typedef __vector4 Vector_128;
//typedef Word64 WordPtr;
//typedef Int64 IntPtr;
//typedef Word8 Bool;
//typedef Int32 Frac32;
//typedef Int32 Fixed32;
//typedef unsigned int Word;
//typedef signed int Int;

/*! ************************************

	\mainpage Welcome to Burgerlib.
	<center><h1>The only low level library you'll ever need.</h1></center>

	BurgerLib is copyright 1995-2017 by <a href="mailto:becky@burgerbecky.com">Rebecca Ann Heineman</a>.
	Email all suggestions, corrections, optimizations and insults to her and she'll be eternally grateful.
	
	You can always find the latest version of <a href="https://github.com/Olde-Skuul/burgerlib" target="_blank">Burgerlib's source code here</a>
	or the <a href="https://github.com/Olde-Skuul/KitchenSink" target="_blank">precompiled version here</a>, also check out Rebecca's personal website
	<a href="http://www.burgerbecky.com" target="_blank">http://www.burgerbecky.com</a>.

	<a href="http://www.burgerbecky.com/burgerlib/docs/burgerlib.chm" target="_blank">Offline documentation for Windows can be downloaded here.</a>

	Sections of interest:
	\li \subpage pageintroduction
	\li \subpage pagewindowsinstall
	\li \subpage pagemacosinstall
	\li \subpage pageusingburgerlib
	\li \subpage pagedefines
	\li \subpage pagetypes
	\li \subpage pagewhitepapers
	
	<center><h2>Credits</h2></center>
	<b><ul>
	<li>Rebecca Ann Heineman - most of the codebase</li>
	<li>Gary S. Brown - CRC32B</li>
	<li>Jean-loup Gailly - Original Zlib compressor</li>
	<li>Mark Adler - Original Zlib decompressor</li>
	<li>Thatcher Ulrich - Hash template</li>
	<li>Daniel Julius Bernstein - djb2 hash algorithm</li>
	<li>Matt Pritchard - SafePrint</li>
	</ul></b>
	
***************************************/

/*! ************************************

	\page pageintroduction Introduction to Burgerlib.
	
	\section intro_sec Introduction
	
	BurgerLib is a set of subroutines that are 
	designed to work the same across multiple
	target platforms. Proper use of this library
	will allow rapid development and deployment 
	of game and application software across many
	different operating systems and computers.
	
	All aspects of the virtual OS are standardized so most interaction with the host
	OS an be done transparently so the application needs minimal work for platform
	localization.
	
	The library was not designed to be a game engine. It was designed to be a set of foundation
	functions that will allow the creation of game engines that are cross platform
	and will take advantage of most features of the underlying hardware without
	being aware that the features are even present.
	
	Compilers supported under a Win32 host:
	\li <a href="http://www.openwatcom.com" target="_blank">Open Watcom 1.9</a> for <a href="http://www.dosextender.com/" target="_blank">FlashTek X-32</a> Intel.
	\li <a href="http://www.openwatcom.com" target="_blank">Open Watcom 1.9</a> for <a href="http://www.tenberry.com/dos4g/watcom/" target="_blank">DOS/4GW</a> Intel.
	\li <a href="http://www.openwatcom.com" target="_blank">Open Watcom 1.9</a> for <a href="http://msdn.microsoft.com" target="_blank">Win32</a> Intel.
	\li <a href="http://www.intel.com/software/products/compilers/cwin/" target="_blank">Intel C++ 10.0</a> for <a href="http://msdn.microsoft.com" target="_blank">Win32</a> Intel.
	\li <a href="http://msdn2.microsoft.com/en-us/vstudio/aa973782.aspx" target="_blank">Visual Studio .NET 2005</a> for <a href="http://msdn.microsoft.com" target="_blank">Win32</a> Intel.
	\li <a href="http://msdn2.microsoft.com/en-us/vstudio/aa973782.aspx" target="_blank">Visual Studio .NET 2005</a> for <a href="http://msdn.microsoft.com" target="_blank">Win64</a> AMD64.
	\li <a href="http://msdn2.microsoft.com/en-us/vstudio/aa700867.aspx" target="_blank">Visual Studio .NET 2003</a> for <a href="http://www.xbox.com" target="_blank">XBox</a> Intel Celeron.
	\li <a href="http://msdn2.microsoft.com/en-us/vstudio/aa973782.aspx" target="_blank">Visual Studio .NET 2005</a> for <a href="http://www.xbox.com" target="_blank">XBox 360</a> PowerPC.
	\li <a href="http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=CW-SONY-PS2" target="_blank">Metrowerks CodeWarrior 3.8.1</a> for <a href="http://www.playstation.com/" target="_blank">Sony Playstation 2</a> MIPS R5900.
	\li <a href="http://www.snsys.com/PlayStation2/ProDG.htm" target="_blank">SNSystems GCC</a> for <a href="http://www.playstation.com/" target="_blank">Sony Playstation 2</a> MIPS R5900.
	\li <a href="http://www.lukasz.dk/ps2toolchain.html" target="_blank">PS2Dev</a> for <a href="http://www.playstation.com/" target="_blank">Sony Playstation 2</a> MIPS R5900.
	\li <a href="http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=CW-NINTENDO-GC" target="_blank">Metrowerks CodeWarrior 2.7</a> for <a href="http://www.nintendo.com/systemsgcn" target="_blank">Nintendo GameCube</a> PowerPC Gekko.
	\li <a href="http://www.snsys.com/GameCube/ProDG.htm" target="_blank">SNSystems GCC</a> for <a href="http://www.nintendo.com/systemsgcn" target="_blank">Nintendo GameCube</a> PowerPC Gekko.
	\li <a href="http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=CW-NINTENDODS" target="_blank">Metrowerks CodeWarrior 2.0</a> for <a href="http://www.nintendo.com/systemsds" target="_blank">Nintendo DS</a> ARM 9.
	\li <a href="http://www.warioworld.com" target="_blank">Metrowerks CodeWarrior 3.0</a> for <a href="http://www.nintendo.com/systemswii" target="_blank">Nintendo Wii</a> PowerPC Gekko 2.

	Compilers supported under a MacOSX host:
	\li <a href="http://developer.apple.com/tools/mpw-tools/" target="_blank">MrC 5.0d3 MPW</a> for <a href="http://www.info.apple.com/usen/macos9/" target="_blank">Mac OS Classic</a> PowerPC.
	\li <a href="http://www.metrowerks.com/MW/Develop/Desktop/Windows/Professional/Default.htm" target="_blank">Metrowerks CodeWarrior 9.6</a> for <a href="http://msdn.microsoft.com" target="_blank">Win32</a> Intel.
	\li <a href="http://www.metrowerks.com/MW/Develop/Desktop/Macintosh/Professional/Mac9.htm" target="_blank">Metrowerks CodeWarrior 7.1</a> for <a href="http://www.info.apple.com/usen/macos9/" target="_blank">MacOS Classic 8.6/9.2</a> Motorola 68K.
	\li <a href="http://www.metrowerks.com/MW/Develop/Desktop/Macintosh/Professional/Mac9.htm" target="_blank">Metrowerks CodeWarrior 10.0</a> for <a href="http://www.info.apple.com/usen/macos9/" target="_blank">MacOS Classic 8.6/9.2</a> PowerPC.
	\li <a href="http://www.metrowerks.com/MW/Develop/Desktop/Macintosh/Professional/Mac9.htm" target="_blank">Metrowerks CodeWarrior 10.0</a> for <a href="http://www.apple.com/macosx/" target="_blank">MacOS Carbon 8.6/9.2/X</a> PowerPC.
	\li <a href="http://www.metrowerks.com/MW/Develop/Desktop/Macintosh/Professional/Mac9.htm" target="_blank">Metrowerks CodeWarrior 10.0</a> for <a href="http://www.apple.com/macosx/" target="_blank">MacOS X</a> Mach PowerPC.
	\li <a href="http://www.apple.com/xcode" target="_blank">Apple XCode 3.0</a> for <a href="http://www.apple.com/macosx/" target="_blank">MacOS X</a> Mach PowerPC.
	\li <a href="http://www.apple.com/xcode" target="_blank">Apple XCode 3.0</a> for <a href="http://www.apple.com/macosx/" target="_blank">MacOS X</a> Mach Intel.
	
***************************************/

/*! ************************************

	\page pagewindowsinstall Installing Burgerlib on a Win32 machine.
	
	\section win32copysdk Step 1: Copying the SDKs onto your host machine.
	
	Create a folder on your hard drive to copy all of the SDKs
	that you will be using. The usual place is C:\\SDKs but you
	can place them anywhere you wish.
	
	Once the folder is created, you need to copy and unzip these
	basic SDKs into their respective folders. The links below will download
	the current stable versions of the respective folder.
	
	\li <a href="http://www.burgerbecky.com/burgerlib/w32burger.zip">Burgerlib for Win32</a> into w32burger.
	\li <a href="http://www.burgerbecky.com/burgerlib/w32directx9.zip">DirectX 9 and 10 headers for Win32</a> into w32directx9.
	\li <a href="http://www.burgerbecky.com/burgerlib/w32platformsdk.zip">Win32 headers from Microsoft for all compilers</a> into w32platformsdk.
	\li <a href="http://www.burgerbecky.com/burgerlib/w32opengl.zip">OpenGL for Win32</a> into w32opengl.
	\li <a href="http://www.burgerbecky.com/burgerlib/w32qt7sdk.zip">Quicktime 7 headers for Win32</a> into w32qt7sdk.
	
	For best use, an environment variable needs to be set up to
	point to the SDKs directory. The easiest way is to edit your AUTOEXEC.BAT
	file. Editing your system registry is acceptable as well, but
	this way it is easier to modify if you decide to move the folder somewhere else.
	
	Insert this line into your C:\\AUTOEXEC.BAT file.
	\code SET BURGER_SDKS=C:\SDKs
	\endcode
	
	\section win32accesssdk Step 2: Accessing the SDKs from the build environment.
	
	For each target in your Win32 project, you need to add these folders into the include
	and link include directories so the headers and the libraries can be found. Please note the use
	of quotes, it will allow the variable \$(BURGER_SDKS) to contain a space.

	\subsection win32vc71 Include directories for Visual Studio .NET 2003.
	In Configuration Properties: C/C++ : General : Additional Include Directories...
	\code "$(BURGER_SDKS)\windows\burger";"$(BURGER_SDKS)\windows\opengl";"$(BURGER_SDKS)\windows\directx9";"$(BURGER_SDKS)\windows\windows5"
	\endcode
	In Configuration Properties: Linker : General : Additional Library Directories...
	\code "$(BURGER_SDKS)\windows\burger";"$(BURGER_SDKS)\windows\opengl";"$(BURGER_SDKS)\windows\directx9"
	\endcode
	
	\subsection win32cw9 Access paths for Codewarrior 9.4.
	In the menu bar, select Edit : Preferences... : General : Source Trees...<br>
	Create the name of "BURGER_SDKS" and set the path to the folder where the SDKS are.<br>
	The typical combination is Name = BURGER_SDKS and the environment variable is SDKS which usually
	resolves to C:\\SDKS.<br>
	For each project, in Settings : Target Settings Panels: Target: Access Paths<br>
	\code
	{BURGER_SDKS}windows\burger
	{BURGER_SDKS}windows\opengl
	{BURGER_SDKS}windows\directx9
	{BURGER_SDKS}windows\windows5
	\endcode
	
	\section win32selectlib Step 3: Include the proper library for your target.
	
	Include only one of these libraries for the build target.

	\subsection win32selectlibvc71 Libraries for Visual Studio .NET 2003.
	\li burgerv71w32dbg.lib for no optimizations and debug information.
	\li burgerv71w32dbf.lib for full optimizations and debug information.
	\li burgerv71w32rel.lib for full optimizations and no debug information.
	
	\subsection win32selectlibvc8 Libraries for Visual Studio .NET 2005.
	\li burgervc8w32dbg.lib for no optimizations and debug information.
	\li burgervc8w32dbf.lib for full optimizations and debug information.
	\li burgervc8w32rel.lib for full optimizations and no debug information.

	\subsection win32selectlibcw9 Libraries for Codewarrior 9.4.
	\li burgercw9w32dbg.lib for no optimizations and debug information.
	\li burgercw9w32dbf.lib for full optimizations and debug information.
	\li burgercw9w32del.lib for full optimizations and no debug information.
	
	\subsection win32selectlibwat Libraries for Open Watcom 1.9.
	\li burgerwatw32dbg.lib for no optimizations and debug information.
	\li burgerwatw32dbf.lib for full optimizations and debug information.
	\li burgerwatw32rel.lib for full optimizations and no debug information.

***************************************/

/*! ************************************

	\page pagemacosinstall Installing Burgerlib on a MacOS machine.
	
	\section macosinstallsoftware Step 1: Installing development software
	
	\subsection powermacinstall Using a PowerPC Mac running with MacOSX Leopard (10.5.8)
	
	Install these applications
	\li <a href="http://support.apple.com/downloads/DL761/en_US/QuickTime770_Leopard.dmg">Quicktime 7.7 for Leopard</a>
	\li <a href="ftp://ftp.perforce.com/perforce/r11.1/bin.macosx105u/P4V.dmg">Perforce Visual Client 11.1 for Power Mac</a>
	\li <a href="http://adcdownload.apple.com/Developer_Tools/xcode_3.1.4_developer_tools/xcode314_2809_developerdvd.dmg">XCode 3.1.4 for MacOSX Leopard PowerPC</a>, may need to be logged into Apple's Developer Connection to get access
	\li <a href="https://www.python.org/ftp/python/2.7.8/python-2.7.8-macosx10.5.dmg">Python 2.7.8 for MacOSX Leopard PowerPC</a>
	\li <a href="http://pine.barebones.com/freeware/TextWrangler_3.5.3.dmg">Text Wrangler for MacOSX Leopard PowerPC</a>

	\subsection intelmacinstall Using an Intel Mac running with MacOSX Mavericks or higher
	
	Install these applications
	\li <a href="ftp://ftp.perforce.com/perforce/r11.1/bin.macosx105u/P4V.dmg">Perforce Visual Client 11.1 for Power Mac</a>
	\li XCode for MacOSX via the app store
	\li <a href="https://www.python.org/ftp/python/2.7.8/python-2.7.8-macosx10.5.dmg">Python 2.7.8 for MacOSX Leopard PowerPC</a>
	\li Text Wrangler for MacOSX Via the app store

	\section macoscopysdk Step 2: Copying the SDKs onto your host machine.
	
	Create a folder on your hard drive to copy all of the SDKs
	that you will be using. The usual place is SDKS off of your boot volume, but you
	can place them anywhere you wish.
	
	Once the folder is created, you need to copy and uncompress these
	basic SDKS into their respective folders. The links below will download the
	current stable versions of the respective folder.
	
	\li <a href="http://www.burgerbecky.com/burgerlib/macburger.sitx">Burgerlib for MacOS</a> into macburger.
	\li <a href="http://www.burgerbecky.com/burgerlib/macgamesprockets.sitx">Gamesprockets for MacOS 9</a> into macgamesprockets.
	\li <a href="http://www.burgerbecky.com/burgerlib/macglide.sitx">Glide for MacOS 9 (Obsolete)</a> into macglide.
	\li <a href="http://www.burgerbecky.com/burgerlib/macopengl.sitx">OpenGL for MacOS 9</a> into macopengl.
	\li <a href="http://www.burgerbecky.com/burgerlib/macqt6sdk.sitx">Quicktime for MacOS 9</a> into macqt6sdk.
	\li <a href="http://www.burgerbecky.com/burgerlib/macstdclib.sitx">StdCLib support for Classic and OSX.</a> into macstdclib.
	
	\section macosaccesssdk Step 3: Accessing the SDKs from the build environment.

	\subsection macoscw9 Access paths for Codewarrior 8.3 or 10.0.
	In the menu bar, select Edit : Preferences... : General : Source Trees...<br>
	Create the name of "BURGER_SDKS" and set the path to the folder where the SDKS are.<br>
	The typical combination is Name = BURGER_SDKS and the Path = /SDKs.<br>
	For each project, in Settings : Target Settings Panels: Target: Access Paths<br>
	\code
	{BURGER_SDKS}macburger
	{BURGER_SDKSBURGER_SDKSmacopengl
	{SDKs}macgamesprockets
	\endcode
	
	\section macosselectlib Step 4: Include the proper library for your target.
	
	Include only one of these libraries for the build target.

	\subsection macosselectlibc10 Libraries for Codewarrior 10.0 / PowerPC.
	\li burgerc10macppcdbg.lib for no optimizations and debug information (Classic only).
	\li burgerc10macppcdbf.lib for full optimizations and debug information (Classic only).
	\li burgerc10macppcrel.lib for full optimizations and no debug information (Classic only).
	\li burgerc10carppcdbg.lib for no optimizations and debug information (Carbon for OS9 and OSX).
	\li burgerc10carppcdbf.lib for full optimizations and debug information (Carbon for OS9 and OSX).
	\li burgerc10carppcrel.lib for full optimizations and no debug information (Carbon for OS9 and OSX).
	\li burgerc10osxdbg.lib for no optimizations and debug information (OSX Mach-O).
	\li burgerc10osxdbf.lib for full optimizations and debug information (OSX Mach-O).
	\li burgerc10osxrel.lib for full optimizations and no debug information (OSX Mach-O).

	\subsection macosselectlibcw8 Libraries for Codewarrior 8.3 / 68k classic only.
	\li burgercw868kdbg.lib for no optimizations and debug information.
	\li burgercw868kdbf.lib for full optimizations and debug information.
	\li burgercw868krel.lib for full optimizations and no debug information.
	\li burgercw868kdbgfar.lib for no optimizations and debug information (Far data).
	\li burgercw868kdbffar.lib for full optimizations and debug information (Far data).
	\li burgercw868krelfar.lib for full optimizations and no debug information (Far data).
	\li burgercw868kcfmdbg.lib for no optimizations and debug information (Code fragment).
	\li burgercw868kcfmdbf.lib for full optimizations and debug information (Code fragment).
	\li burgercw868kcfmrel.lib for full optimizations and no debug information (Code fragment).
	\li burgercw868kcfmdbgfar.lib for no optimizations and debug information (Code fragment, Far data).
	\li burgercw868kcfmdbffar.lib for full optimizations and debug information (Code fragment, Far data).
	\li burgercw868kcfmrelfar.lib for full optimizations and no debug information (Code fragment, Far data).

***************************************/

/*! ************************************

	\page pageusingburgerlib Using Burgerlib.
	
	\section introduction Introduction
	
	To use burgerlib is simplicity itself. Just add this to the beginning of your
	source files or headers (Or both).
	
	\code
	// The #ifndef __BURGER__ is optional, but useful to prevent double includes
	// if included from a user supplied header.
	#ifndef __BURGER__			
	#include <burger.h>
	#endif
	\endcode

	<h3>That's it!</h3>

***************************************/

/*! ************************************

	\page pagedefines Platform defines
	
	This is the mainstay of all of Burgerlib. You include the file
	burger.h in your project which will determine the compiler, target,
	CPU, and system specific features so you can write platform
	neutral code.

	Debug defines, one is required MUST be present
	and is supplied by the build project. This is not created
	by default by Burgerlib. If both exist or both are missing, burger.h
	will force a compile error.

	\li \ref _DEBUG Debugging asserts are enabled.
	\li \ref NDEBUG	Debugging asserts are disabled.
	
	CPU defines, only one is enabled on each compile.
	
	\li \ref BURGER_X86 CPU is 32 bit Intel compatible.
	\li \ref BURGER_AMD64 CPU is 64 bit Intel compatible.
	\li \ref BURGER_POWERPC CPU is part of the PowerPC family.
	\li \ref BURGER_ARM CPU is part of the Advanced RISC Machines family.
	\li \ref BURGER_68K CPU is part of the Motorola 68000 family.
	\li \ref BURGER_MIPS CPU is part of the SGI MIPS family.

	Type of CPU defines, they are defined to help determine the type
	of CPU is running

	\li \ref BURGER_64BITCPU The CPU has native 64 bit registers (AMD64, PPC64, ARM64).
	\li \ref BURGER_INTELARCHITECTURE The CPU is either AMD64 or X86

	Endian defines, only one is enabled on each compile. Do not
	use the CPU to determine the endian, because it may change on
	some platforms.
	
	\li \ref BURGER_BIGENDIAN
	\li \ref BURGER_LITTLEENDIAN
	
	Platform defines, only one is enabled on each compile.
	
	\li \ref BURGER_WIN32 The underlying OS is 32 bit Windows (XP/NT/7/8 Etc)
	\li \ref BURGER_WIN64 The underlying OS is 64 bit Windows
	\li \ref BURGER_WINDOWS The underlying OS is any flavor of Microsoft Windows
	\li \ref BURGER_MSDOS The underlying OS is MSDOS
	\li \ref BURGER_MAC The underlying OS is MacOS (Pre-X)
	\li \ref BURGER_MACOSX The underlying OS is a version of MacOSX
	\li \ref BURGER_MACCLASSIC 
	\li \ref BURGER_MACCARBON
	\li \ref BURGER_MACOS The underlying OS is ANY version of MacOS
	\li \ref BURGER_IOS
	\li \ref BURGER_NGAGE
	\li \ref BURGER_SYMBIAN
	\li \ref BURGER_PS1
	\li \ref BURGER_PS2
	\li \ref BURGER_PS3
	\li \ref BURGER_PS4
	\li \ref BURGER_PSP
	\li \ref BURGER_VITA
	\li \ref BURGER_GAMECUBE
	\li \ref BURGER_WII
	\li \ref BURGER_WIIU
	\li \ref BURGER_GBA Gameboy Advanced
	\li \ref BURGER_DS Nintendo DS and DSi
	\li \ref BURGER_3DS
	\li \ref BURGER_XBOX Refers to the original XBox
	\li \ref BURGER_XBOX360
	\li \ref BURGER_XBOXONE
	\li \ref BURGER_ANDROID The underlying OS is Android
	\li \ref BURGER_SHIELD
	\li \ref BURGER_OUYA
	\li \ref BURGER_BEOS has been deprecated.
	\li BURGER_APPLEIIGS has been removed.
	\li BURGER_3DO has been removed.
	
	Compiler defines, to determine any compiler specific features are available

	\li \ref BURGER_WATCOM Open Watcom
	\li \ref BURGER_MRC Apple's PowerPC MPW compiler
	\li \ref BURGER_APPLESC Apple's 68000 MPW compiler
	\li \ref BURGER_METROWERKS Metrowerks Codewarrior (PowerPC, Intel, 68000)
	\li \ref BURGER_MSVC Microsoft Visual Studio
	\li \ref BURGER_INTELCOMPILER Intel's own compiler
	\li \ref BURGER_LLVM LLVM (Usually found in Apple's XCode)
	\li \ref BURGER_GNUC GNU C, found on Linux and other platforms

	Standard defines present on all build targets
	
	\li \ref TRUE
	\li \ref FALSE
	\li \ref NULL
	\li \ref BURGER_MAXINT
	\li \ref BURGER_MININT
	\li \ref BURGER_MAXUINT
	\li \ref BURGER_INLINE
	\li \ref BURGER_API
	\li \ref BURGER_ANSIAPI
	\li \ref BURGER_ASM
	\li \ref BURGER_CFM
	\li \ref BURGER_FASTCALLENABLED Used for Intel assembly functions to use the __fastcall ABI
	\li \ref BURGER_DECLSPECNAKED Used to create a function without a prologue or epilogue
	\li \ref BURGER_ALIGN
	\li \ref BURGER_PREALIGN
	\li \ref BURGER_POSTALIGN
	\li \ref BURGER_STRUCT_ALIGN
	\li \ref BURGER_STRUCT_PACK
	\li \ref BURGER_STRUCT_PACKPUSH

	Defines to denote if features, compilation switches or high level APIs are present

	\li \ref BURGER_OPENGL_SUPPORTED
	\li \ref BURGER_OPENGLES

	Handy macros

	\li \ref BURGER_OFFSETOF
	\li \ref BURGER_ARRAYSIZE
	\li \ref BURGER_UNUSED
	\li \ref BURGER_MACRO_TO_STRING
	\li \ref BURGER_DISABLECOPYCONSTRUCTORS
	
***************************************/


/*! ************************************

	\page pagetypes Platform types
	
	These data types are stable and will generate the exact same size on
	any platform.

	\li \ref Word8 8 bit unsigned integer.
	\li \ref Int8 8 bit signed integer.
	\li \ref Word16 16 bit unsigned integer.
	\li \ref Int16 16 bit signed integer.
	\li \ref Word32 32 bit unsigned integer.
	\li \ref Int32 32 bit signed integer.
	\li \ref Word64 64 bit unsigned integer.
	\li \ref Int64 64 bit signed integer.
	\li \ref Vector_128 128 bit vector intrinsic.
	\li \ref WordPtr Unsigned integer that matches in size to a void *.
	\li \ref IntPtr Signed integer that matches in size to a void *.
	\li \ref Bool Unsigned 8 bit integer for boolean operations.
	\li \ref Frac32 Signed 2.30 32 bit integer for fixed point operations.
	\li \ref Fixed32 Signed 16.16 32 bit integer for fixed point operations.
	\li \ref Word Unsigned integer.
	\li \ref Int Signed integer.
	
***************************************/

/*! ************************************

	\page pagewhitepapers White Papers
	
	These white papers were used to generate some of the code
	in Burgerlib

	\li <a href="isolatin1.htm">ISO Latin 1 character set</a>
	\li <a href="macromanus.htm">MacOS Roman US character set</a>
	\li <a href="windows1252.htm">Windows 1252 character set</a>
	\li <a href="spec-gif89a.txt">GIF File format</a>
	\li <a href="avm2overview.pdf">ActionScript Virtual Machine 2 Overview</a>
	\li <a href="swf-file-format-spec.pdf">SWF File format specification Version 19</a>
	
***************************************/


/*! ************************************

	\file burger.h
	\brief Determines the build environment.
	
	This header contains the base types
	used in all of Burgerlib. It will also
	try to determine via compiler defines 
	exactly what features are supported in
	the compiler and what target OS you're
	building for.
		
***************************************/


/*! ************************************

	\def _DEBUG
	\brief Defined to enable the creation of debug code.
	
	If this define exists, you can assume that you're compiling
	a debug build. This is used to flag whether debug code is
	to be included in the build and/or \ref BURGER_ASSERT is to operate.
	Only \ref _DEBUG or \ref NDEBUG can exist at a time. Specifying both
	or neither is considered a fatal compile error. This define is
	not supplied by burgerlib, it must be defined by the build project.
	
	\note This flag is not used to determine if optimizations have
	been disabled. It's only used to enable the generation of
	debugging code.
	
	\code
	#if defined(_DEBUG)
		CheckMemoryHeap();
	#endif
	\endcode
	
***************************************/

/*! ************************************

	\def NDEBUG
	\brief Defined to disable the creation of debug code.
	
	If this define exists, then you are creating a release build.
	This is only present to force \ref BURGER_ASSERT to vanish. Only
	\ref _DEBUG or \ref NDEBUG can exist at a time. Specifying both
	or neither is considered a fatal compile error. This define is
	not supplied by burgerlib, it must be defined by the build project.
	
	\note This flag is not used to determine if optimizations have
	been enabled. It's only used to disable the generation of
	debugging code.
	
	\code
	#if defined(NDEBUG)
		CrashGameIfDataNotFound();
	#endif
	\endcode
	
	\sa _DEBUG
	
***************************************/



/*! ************************************

	\def BURGER_X86
	\brief Define to determine if code is being built for 32 bit Intel processors.
	
	If this define exists, then you are creating code that runs on
	the Intel, AMD and other compatible processors. The Microsoft
	XBox, Mac OSX Intel and Win32 platforms which are running on
	Pentium, 386, 486, and AMD CPUs will have this define present.
	
	\sa BURGER_INTELARCHITECTURE, BURGER_AMD64, BURGER_WIN32, BURGER_BEOS, BURGER_MSDOS, BURGER_MACOSX or BURGER_XBOX
	
***************************************/

/*! ************************************

	\def BURGER_AMD64
	\brief Define to determine if code is being built for AMD64 (Intel 64 bit) compatible processors.
	
	If this define exists, then you are creating code that runs on
	the AMD 64 and other compatible processors. The Microsoft Win64
	and Mac OSX 64 platforms which are running on
	Pentium and AMD CPUs in 64 bit mode will have this define present.
	
	\sa BURGER_INTELARCHITECTURE, BURGER_WIN64, BURGER_MACOSX, BURGER_XBOXONE, BURGER_PS4 or BURGER_X86
	
***************************************/

/*! ************************************

	\def BURGER_POWERPC
	\brief Define to determine if code is being built for PowerPC processors.
	
	If this define exists, then you are creating code that runs on
	the PowerPC line of processors. The Nintendo GameCube, Power
	Macintosh, Nintendo Wii, Sony Playstation 3 and Microsoft
	XBox 360 all will have this define present.
	
	\sa BURGER_XBOX360, BURGER_WII, BURGER_PS3, BURGER_MAC, BURGER_MACOSX, BURGER_BEOS or BURGER_GAMECUBE
	
***************************************/

/*! ************************************

	\def BURGER_ARM
	\brief Define to determine if code is being built for Advanced RISC Machine processors.
	
	If this define exists, then you are creating code that runs on
	the Advanced RISC Machines line of processors. The Gameboy Advanced, Nintendo DS, Nokia NGage,
	Apple iPad/iPhone/iPod and certain cell phones will have this define present.
	
	\sa BURGER_GBA, BURGER_ANDROID, BURGER_SHIELD, BURGER_OUYA, BURGER_DS, BURGER_3DS, BURGER_IOS, BURGER_NGAGE, or BURGER_SYMBIAN
	
***************************************/

/*! ************************************

	\def BURGER_68K
	\brief Define to determine if code is being built for Motorola 680x0 processors.
	
	If this define exists, then you are creating code that runs on
	the Motorola 680x0 line of processors. The classic Macintosh and certain
	PDAs will have this define present.
	
	\sa BURGER_MAC or BURGER_CFM
	
***************************************/

/*! ************************************

	\def BURGER_MIPS
	\brief Define to determine if code is being built for MIPS processors.
	
	If this define exists, then you are creating code that runs on
	the MIPS line of processors. R5900 (PS2), R3300 (PS1) and R4400 (PSP)
	all will have this define present.
	
	\sa BURGER_PS2, BURGER_PSP, BURGER_PS1
	
***************************************/

/*! ************************************

	\def BURGER_64BITCPU
	\brief Define to determine if the CPU has 64 bit integer registers.
	
	On \ref BURGER_XBOXONE, \ref BURGER_PS4, \ref BURGER_AMD64,
	\ref BURGER_PS3 and \ref BURGER_XBOX360 platforms, the cpu
	has integer registers that are 64-bits wide. Using this define, code
	can be written that takes advantage of this. Mostly useful with
	\ref Fixed32 and other high precision fixed point calculations.
	
	\sa BURGER_XBOXONE, BURGER_PS4, BURGER_PS3, BURGER_XBOX360, or BURGER_AMD64
	
***************************************/

/*! ************************************

	\def BURGER_INTELARCHITECTURE
	\brief Define to determine if the CPU is either X86 or AMD64
	
	On \ref BURGER_AMD64 or \ref BURGER_X86 CPUs, this define
	will also be present to allow Intel Architecture instructions.
	Some instructions may or may not be present, so call \ref Burger::CPUID(CPUID_t *)
	to test for all features and determine which functions to use
	for performance.
	
	\sa BURGER_XBOXONE, BURGER_PS4, BURGER_WINDOWS, BURGER_MACOSX, BURGER_X86, or BURGER_AMD64
	
***************************************/



/*! ************************************

	\def BURGER_LITTLEENDIAN
	\brief Define to flag code running on a little endian machine.
	
	This define or \ref BURGER_BIGENDIAN will be present to allow the
	programmer to create code that can properly parse endian
	specific data. Only one macro will be defined.
	
	If this macro is present, the machine the code is being built
	for is little endian.
	
	\sa BURGER_BIGENDIAN
	
***************************************/

/*! ************************************

	\def BURGER_BIGENDIAN
	\brief Define to flag code running on a big endian machine.
	
	This define or \ref BURGER_LITTLEENDIAN will be present to allow the
	programmer to create code that can properly parse endian
	specific data. Only one macro will be defined.

	If this macro is present, the machine the code is being built
	for is big endian.
	
	\sa BURGER_LITTLEENDIAN
	
***************************************/


/*! ************************************

	\def BURGER_WATCOM
	\brief Define to determine if code is being built using Open Watcom compilers.
	
	If this define exists, then you are creating code with the 
	<a href="http://www.openwatcom.com" target="_blank">Open Watcom compiler</a>
	
	\sa BURGER_INTELCOMPILER, BURGER_METROWERKS, BURGER_MSVC and BURGER_X86
	
***************************************/

/*! ************************************

	\def BURGER_MRC
	\brief Define to determine if code is being built using Apple/Motorola MrC compiler.
	
	If this define exists, then you are creating code with the 
	<a href="http://mirrors.muleslow.net/ftp.apple.com/developer/Tool_Chest/Core_Mac_OS_Tools/MPW_Website_Downloads/" target="_blank">MrC compiler</a>
	
	\sa BURGER_GNUC, BURGER_METROWERKS, BURGER_APPLESC and BURGER_POWERPC
	
***************************************/

/*! ************************************

	\def BURGER_APPLESC
	\brief Define to determine if code is being built using Apple/Motorola SC compiler.
	
	If this define exists, then you are creating code with the 
	<a href="http://mirrors.muleslow.net/ftp.apple.com/developer/Tool_Chest/Core_Mac_OS_Tools/MPW_Website_Downloads/" target="_blank">Apple SC compiler</a>
	
	\sa BURGER_GNUC, BURGER_METROWERKS, BURGER_MRC and BURGER_68K
	
***************************************/

/*! ************************************

	\def BURGER_INTELCOMPILER
	\brief Define to determine if code is being built using Intel compiler.
	
	If this define exists, then you are creating code with the 
	<a href="http://software.intel.com/en-us/intel-compilers" target="_blank">Intel compiler</a>
	
	\sa BURGER_MSVC, BURGER_METROWERKS, BURGER_AMD64 and BURGER_X86
	
***************************************/

/*! ************************************

	\def BURGER_SNSYSTEMS
	\brief Define to determine if code is being built using SN Systems compiler.
	
	If this define exists, then you are creating code with the 
	<a href="http://www.snsys.com/" target="_blank">SN Systems compiler</a>
	
	\sa BURGER_METROWERKS, BURGER_POWERPC, BURGER_MIPS and BURGER_AMD64
	
***************************************/

/*! ************************************

	\def BURGER_METROWERKS
	\brief Define to determine if code is being built using Freescale/Metrowerks family of compilers.
	
	If this define exists, then you are creating code with the 
	<a href="http://www.freescale.com/webapp/sps/site/homepage.jsp?code=CW_HOME" target="_blank">Freescale / Metrowerks compiler</a>
	
	\sa BURGER_MRC, BURGER_POWERPC, BURGER_MIPS and BURGER_X86
	
***************************************/

/*! ************************************

	\def BURGER_MSVC
	\brief Define to determine if code is being built using Microsoft Visual C family of compilers.
	
	If this define exists, then you are creating code with the Microsoft compiler.
	
	\sa BURGER_INTELCOMPILER, BURGER_METROWERKS, BURGER_ARM, BURGER_AMD64 and BURGER_X86
	
***************************************/

/*! ************************************

	\def BURGER_GNUC
	\brief Define to determine if code is being built using the GNU C family of compilers.
	
	If this define exists, then you are creating code with the GNU compiler.
	
	\sa BURGER_LLVM, BURGER_METROWERKS, BURGER_POWERPC, BURGER_ARM, BURGER_AMD64 and BURGER_X86
	
***************************************/

/*! ************************************

	\def BURGER_LLVM
	\brief Define to determine if code is being built using the LLVM family of compilers.
	
	If this define exists, then you are creating code with the LLVM compiler.
	
	\sa BURGER_GNUC, BURGER_METROWERKS, BURGER_POWERPC, BURGER_ARM, BURGER_AMD64 and BURGER_X86
	
***************************************/



/*! ************************************

	\def BURGER_WIN32
	\brief Define to determine if code is being built for Microsoft 32 bit Windows.
	
	If this define exists, then you are creating code that runs on
	Microsoft Windows for 32 bit CPUs.
	
	\note You may need to check the processor define, since future versions
	of Windows may be running on non-Pentium type processors.
	
	\sa BURGER_WINDOWS,BURGER_WIN64, BURGER_MSDOS and BURGER_X86
	
***************************************/

/*! ************************************

	\def BURGER_WIN64
	\brief Define to determine if code is being built for Microsoft 64 bit Windows.
	
	If this define exists, then you are creating code that runs on
	Microsoft Windows for 64 bit CPUs.
	
	\note You may need to check the processor define, since future versions
	of Windows may be running on non-Pentium type processors.
	
	\sa BURGER_WINDOWS, BURGER_WIN32, BURGER_MSDOS and BURGER_AMD64
	
***************************************/

/*! ************************************

	\def BURGER_WINDOWS
	\brief Define to determine if code is being built for Microsoft 32 or 64 bit Windows.
	
	If this define exists, then you are creating code that runs on
	Microsoft Windows for either 32 or 64 bit CPUs.
	
	\note You may need to check the processor define, since future versions
	of Windows may be running on non-Pentium type processors.
	
	\sa BURGER_WIN32, BURGER_WIN64, BURGER_MSDOS and BURGER_X86
	
***************************************/

/*! ************************************

	\def BURGER_MSDOS
	\brief Define to determine if code is being built for Microsoft MS-DOS.
	
	If this define exists, then you are creating code that runs
	under classic MS-DOS.
	
	Check for the defines __X32__ or __DOS4G__ if you wish to determine
	which dos extender you are using.
	
	\sa BURGER_X86
	
***************************************/

/*! ************************************

	\def __X32__
	\brief Define to determine if MS-DOS code is being built with the X32 DOS extender.
	
	If this define exists, then you are creating code that runs
	under classic MS-DOS with the X32 DOS extender.
	
	This define is declared by the project, not by Burgerlib. Either __X32__
	or __DOS4G__ must be present.
	
	\sa BURGER_MSDOS or __DOS4G__
	
***************************************/

/*! ************************************

	\def __DOS4G__
	\brief Define to determine if MS-DOS code is being built with the DOS4G DOS extender.
	
	If this define exists, then you are creating code that runs
	under classic MS-DOS with the DOS4G DOS extender.
	
	This define is declared by the project, not by Burgerlib. Either __DOS4G__
	or __X32__ must be present.
	
	\sa BURGER_MSDOS or __X32__
	
***************************************/


/*! ************************************

	\def BURGER_MAC
	\brief Define to determine if code is being built for Apple Mac OS 8.6 through 9.2.
	
	If this define exists, then you are creating code that runs 
	under Apple MacOS version 8.6 through 9.2. Since MacOS runs on the 680x0 or
	the PowerPC architectures, you may need to check \ref BURGER_68K or \ref BURGER_POWERPC
	for any CPU specific code or features. Also, on 680x0 platforms, you may be
	compiling under the Code Fragment Manager format, and to detect that, check
	for the presence of \ref BURGER_CFM.
	
	\note To compile for Carbon under a PowerPC CPU, your project must define \ref TARGET_API_MAC_CARBON
	so the MacOS headers will properly be adjusted for Carbon. If you are using Burgerlib for Carbon,
	you have access to MacOS X features if your application is running on a MacOS X system.
	
	\code
	// Include this define in your CodeWarrior C/C++ Preprocessor preferences settings
	// to build for MacOS 9 Carbon.
	
	#define TARGET_API_MAC_CARBON 1
	\endcode
	
	\sa BURGER_MACOS, BURGER_MACOSX, BURGER_MACCLASSIC, BURGER_MACCARBON, BURGER_68K, BURGER_POWERPC or TARGET_API_MAC_CARBON
	
***************************************/

/*! ************************************

	\def BURGER_MACOSX
	\brief Define to determine if code is being built exclusively for Apple Mac OS X.
	
	If this define exists, then you are creating code that runs under
	Apple MacOS X using Mach-O. There is no support for pre-MacOS X
	operating systems under this build target. The CPU can be either
	\ref BURGER_POWERPC, \ref BURGER_AMD64 or
	\ref BURGER_X86 for this platform, so no CPU specific code should be written
	without checking for the CPU type.
	
	\sa BURGER_POWERPC, BURGER_AMD64, BURGER_X86, BURGER_MACOS or BURGER_MAC
	
***************************************/

/*! ************************************

	\def BURGER_MACCARBON
	\brief Define to determine if code is being built for both Apple Mac OS9 and MacOS X.
	
	If this define exists, then you are creating code that runs under
	Apple MacOS 9 and for Apple MacOS X. Only the PowerPC is supported.
	Since MacOS Carbon support is enabled,
	accessing the low memory operating system variables is <b>NOT</b> allowed.
	
	\sa BURGER_POWERPC, BURGER_MACOSX, BURGER_MACOS or BURGER_MAC
	
***************************************/

/*! ************************************

	\def BURGER_MACCLASSIC
	\brief Define to determine if code is being built exclusively for Apple Mac OS9.
	
	If this define exists, then you are creating code that runs under
	Apple MacOS 9 for either PowerPC or 68K. There is no support for MacOS X
	operating systems under this build target. The CPU can be either \ref BURGER_POWERPC or
	\ref BURGER_68K for this platform, so no CPU specific code should be written
	without checking for the CPU type. Since no MacOS Carbon support is enabled,
	accessing the low memory operating system variables is allowed.
	
	\sa BURGER_POWERPC, BURGER_68K, BURGER_MACOS or BURGER_MAC
	
***************************************/

/*! ************************************

	\def BURGER_MACOS
	\brief Define to determine if code is being built for any Apple Mac OS platform.
	
	If this define exists, then you are creating code that runs under
	Apple MacOS. Currently, 68K, PowerPC and Intel are supported.
	Since it's not known which version of MacOS is currently being targeted,
	accessing the low memory operating system variables is <b>NOT</b> allowed.
	
	\sa BURGER_POWERPC, BURGER_X86, BURGER_68K, BURGER_MACOSX, BURGER_MACCLASSIC, BURGER_MACCARBON or BURGER_MAC
	
***************************************/

/*! ************************************

	\def TARGET_API_MAC_CARBON
	\brief Define to determine if Mac OS 9 code is being built with the Carbon libraries.
	
	If this define exists, then you are creating code that runs
	under classic Mac OS with the Carbon libaries.
	
	This define is declared by the project, not by Burgerlib. If this define
	is missing, the code will default to classic MacOS 8.6 - 9.2.
	
	\sa BURGER_MAC, BURGER_CFM or BURGER_MACOSX
	
***************************************/

/*! ************************************

	\def BURGER_CFM
	\brief Define to determine if Mac OS 9 code is being built in Code Fragment Manger format.
	
	If this define exists, then you are creating code in which the final binary is
	in PEF format for use with the Code Fragment Manager. All Power PC code for Classic
	or Carbon is in this format and 68k also can be compiled this way.
	
	\note To be able to use DrawSprocket in 68k code, you must build your application using CFM. The
	classic format can't link to DrawSprocket 68k.
	
	\note Power PC code can be built in Mach-O format, but if that is the case, then the code
	can only run in Mac OS X.
	
	\sa BURGER_MAC, BURGER_68K or BURGER_MACOSX
	
***************************************/


/*! ************************************

	\def BURGER_IOS
	\brief Define to determine if code is being built for Apple iOS devices.
	
	If this define exists, then you are creating code that runs on
	Apple iOS devices.
	
	\sa BURGER_ANDROID, BURGER_ARM or BURGER_MACOSX
	
***************************************/

/*! ************************************

	\def BURGER_ANDROID
	\brief Define to determine if code is being built for Google Android devices.
	
	If this define exists, then you are creating code that runs on
	Google Android devices.
	
	\sa BURGER_IOS, BURGER_SHIELD, BURGER_OUYA, BURGER_ARM or BURGER_MACOSX
	
***************************************/

/*! ************************************

	\def BURGER_SHIELD
	\brief Define to determine if code is being built for nVidia SHIELD devices.
	
	If this define exists, then you are creating code that runs on
	nVidia SHIELD Android devices.
	
	\sa BURGER_IOS, BURGER_ANDROID, BURGER_OUYA, BURGER_ARM or BURGER_MACOSX
	
***************************************/

/*! ************************************

	\def BURGER_OUYA
	\brief Define to determine if code is being built for the OUYA.
	
	If this define exists, then you are creating code that runs on
	OUYA Android devices.
	
	\sa BURGER_IOS, BURGER_ANDROID, BURGER_SHIELD, BURGER_ARM or BURGER_MACOSX
	
***************************************/

/*! ************************************

	\def BURGER_NGAGE
	\brief Define to determine if code is being built for the Nokia Ngage.
	
	If this define exists, then you are creating code that runs on the
	Nokia NGage.
	
	\note There is no floating point support on this platform.

	\sa BURGER_ARM, or BURGER_SYMBIAN
	
***************************************/

/*! ************************************

	\def BURGER_SYMBIAN
	\brief Define to determine if code is being built for the Symbian OS.
	
	If this define exists, then you are creating code that runs under the Symbian
	OS, usually Nokia brand cell phones and PDAs.
	
	\note There is no floating point support on this platform.

	\sa BURGER_ARM, or BURGER_NGAGE
	
***************************************/


/*! ************************************

	\def BURGER_PS1
	\brief Define to determine if code is being built for the Sony Playstation.
	
	If this define exists, then you are creating code that runs on
	the Sony Playstation.
	
	\note There is no floating point support on this platform.

	\sa BURGER_MIPS, BURGER_PS2, BURGER_PS3, or BURGER_PS4
	
***************************************/

/*! ************************************

	\def BURGER_PS2
	\brief Define to determine if code is being built for the Emotion Engine CPU for a PS2.
	
	If this define exists, then you are creating code that runs on
	the main processor for a Sony Playstation 2.
	
	\sa BURGER_MIPS, BURGER_PS1, BURGER_PS3, or BURGER_PS4
	
***************************************/

/*! ************************************

	\def BURGER_PS3
	\brief Define to determine if code is being built for the PS3.
	
	If this define exists, then you are creating code that runs on
	the main processor for a Sony Playstation 3.
	
	\note This define is only present for compiling code on the primary CPU.
	Code targeted for the cell units must be written and compiled seperately.
	
	\sa BURGER_POWERPC, BURGER_PS1, BURGER_PS2, or BURGER_PS4
	
***************************************/

/*! ************************************

	\def BURGER_PS4
	\brief Define to determine if code is being built for the PS4.
	
	If this define exists, then you are creating code that runs on
	the main processor for a Sony Playstation 4.
		
	\sa BURGER_AMD64, BURGER_PS1, BURGER_PS2, or BURGER_PS3
	
***************************************/


/*! ************************************

	\def BURGER_PSP
	\brief Define to determine if code is being built for the Sony PSP.
	
	If this define exists, then you are creating code that runs on
	the Sony Playstation Portable (PSP).
	
	\sa BURGER_MIPS, BURGER_VITA, BURGER_PS1, BURGER_PS2, BURGER_PS3, or BURGER_PS4
	
***************************************/

/*! ************************************

	\def BURGER_VITA
	\brief Define to determine if code is being built for the Sony VITA.
	
	If this define exists, then you are creating code that runs on
	the Sony Playstation Portable 2 (PSP2 / Vita).
	
	\sa BURGER_ARM, BURGER_PSP, BURGER_PS1, BURGER_PS2, BURGER_PS3, or BURGER_PS4
	
***************************************/

/*! ************************************

	\def BURGER_GAMECUBE
	\brief Define to determine if code is being built for the Nintendo GameCube.
	
	If this define exists, then you are creating code that runs on
	the Gekko processor for a Nintendo GameCube.
	
	\sa BURGER_POWERPC, BURGER_WII or BURGER_WIIU
	
***************************************/

/*! ************************************

	\def BURGER_WII
	\brief Define to determine if code is being built for the Nintendo Wii.
	
	If this define exists, then you are creating code that runs on
	the Broadway processor for a Nintendo Wii.
	
	\sa BURGER_POWERPC, BURGER_WIIU or BURGER_GAMECUBE
	
***************************************/

/*! ************************************

	\def BURGER_WIIU
	\brief Define to determine if code is being built for the Nintendo WiiU.
	
	If this define exists, then you are creating code that runs on
	the Broadway processor for a Nintendo WiiU.
	
	\sa BURGER_POWERPC, BURGER_WII or BURGER_GAMECUBE
	
***************************************/

/*! ************************************

	\def BURGER_GBA
	\brief Define to determine if code is being built for the Nintendo Gameboy Advanced.
	
	If this define exists, then you are creating code that runs on
	the Nintendo Gameboy Advanced.
	
	\note In addition to defining \ref NDEBUG or \ref _DEBUG, you need to define SDK_DEBUG
	in your project if you wish for the Nintendo GBA SDK to generate debugging information
	
	\sa BURGER_ARM, BURGER_DS or BURGER_3DS
	
***************************************/

/*! ************************************

	\def BURGER_DS
	\brief Define to determine if code is being built for the Nintendo DS.
	
	If this define exists, then you are creating code that runs on
	the Nintendo DS.
	
	\note In addition to defining \ref NDEBUG or \ref _DEBUG, you need to define SDK_DEBUG
	in your project if you wish for the Nintendo DS SDK to generate debugging information
	
	\sa BURGER_ARM, BURGER_3DS or BURGER_GBA
	
***************************************/

/*! ************************************

	\def BURGER_3DS
	\brief Define to determine if code is being built for the Nintendo 3DS.
	
	If this define exists, then you are creating code that runs on
	the Nintendo 3DS.
	
	\note In addition to defining \ref NDEBUG or \ref _DEBUG, you need to define SDK_DEBUG
	in your project if you wish for the Nintendo 3DS SDK to generate debugging information
	
	\sa BURGER_ARM, BURGER_DS or BURGER_GBA
	
***************************************/

/*! ************************************

	\def BURGER_XBOX
	\brief Define to determine if code is being built for the Microsoft XBox.
	
	If this define exists, then you are creating code that runs on
	the Intel Celeron for a Microsoft XBox.
	
	\sa BURGER_X86, BURGER_XBOX360, or BURGER_XBOXONE
	
***************************************/

/*! ************************************

	\def BURGER_XBOX360
	\brief Define to determine if code is being built for the Microsoft XBox 360.
	
	If this define exists, then you are creating code that runs on
	the Microsoft XBox360.
	
	\note You can use Burgerlib functions from any thread, but caution should be
	exercised to prevent thread stalls and multithreading problems. Burgerlib's
	higher level classes may not be reentrant. See the documentation for higher level
	classes to determine multi-threading safety.
	
	\sa BURGER_POWERPC, BURGER_XBOX, or BURGER_XBOXONE
	
***************************************/

/*! ************************************

	\def BURGER_XBOXONE
	\brief Define to determine if code is being built for the Microsoft XBox One.
	
	If this define exists, then you are creating code that runs on
	the Microsoft XBox One.
	
	\note You can use Burgerlib functions from any thread, but caution should be
	exercised to prevent thread stalls and multithreading problems. Burgerlib's
	higher level classes may not be reentrant. See the documentation for higher level
	classes to determine multi-threading safety.
	
	\sa BURGER_AMD64, BURGER_XBOX, or BURGER_XBOX360
	
***************************************/


/*! ************************************

	\def BURGER_BEOS
	\brief Define to determine if code is being built for Be OS.
	
	If this define exists, then you are creating code that runs on
	Be OS for Intel or PowerPC.
	
	\sa BURGER_POWERPC or BURGER_X86
	
***************************************/


/*! ************************************

	\def TRUE
	\brief Define of the number 1 for boolean operations.

	\note Since this a common \#define, it will first check if it's already present
	before defining.
	
	\sa FALSE or NULL

***************************************/

/*! ************************************

	\def FALSE
	\brief Define of the number 0 for boolean operations.

	\note Since this a common \#define, it will first check if it's already present
	before defining.
	
	\sa TRUE or NULL

***************************************/

/*! ************************************

	\def NULL
	\brief Define of the number 0 for pointer invalidation.

	\note Since this a common \#define, it will first check if it's already present
	before defining.
	
	\sa FALSE or TRUE

***************************************/

/*! ************************************

	\def BURGER_MININT
	\brief Minimum value of a signed integer.
	
	This is a replacement for the ANSI macro MIN_INT.
	Normally, this is (-0x7FFFFFFF)-1, but it could be a 64
	or 128 bit value on future processors.
	\sa BURGER_MININT64 or BURGER_MAXINT
	
***************************************/

/*! ************************************

	\def BURGER_MAXINT
	\brief Maximum value of a signed integer.
	
	This is a replacement for the ANSI macro MAX_INT.
	Normally, this is 0x7FFFFFFF, but it could be a 64
	or 128 bit value on future processors.
	\sa BURGER_MININT, BURGER_MAXWORDPTR, BURGER_MAXINTPTR or BURGER_MAXUINT
	
***************************************/

/*! ************************************

	\def BURGER_MAXUINT
	\brief Maximum value of an unsigned integer.
	
	This is a replacement for the ANSI macro MAX_UINT.
	Normally, this is 0xFFFFFFFFU, but it could be a 64
	or 128 bit value on future processors.
	\sa BURGER_MAXWORDPTR, BURGER_MAXINTPTR or BURGER_MAXINT
	
***************************************/

/*! ************************************

	\def BURGER_MININT64
	\brief Minimum value of a \ref Int64.
	
	This is a replacement for the ANSI macro MIN_INT but
	it's meant for 64 bit values defined as \ref Int64.
	Normally, this is ((-0x7FFFFFFFFFFFFFFFLL)-1).

	\sa BURGER_MAXINT64, BURGER_MAXUINT64, or BURGER_MAXINT
	
***************************************/

/*! ************************************

	\def BURGER_MAXINT64
	\brief Maximum value of a \ref Int64.
	
	This is a replacement for the ANSI macro MAX_INT but
	it's meant for 64 bit values defined as \ref Int64.
	Normally, this is 0x7FFFFFFFFFFFFFFFLL.

	\sa BURGER_MININT64, BURGER_MAXUINT64, or BURGER_MAXINT
	
***************************************/

/*! ************************************

	\def BURGER_MAXUINT64
	\brief Maximum value of a \ref Word64.
	
	This is a replacement for the ANSI macro MAX_UINT but
	it's meant for 64 bit values defined as \ref Word64.
	Normally, this is 0xFFFFFFFFFFFFFFFFULL.

	\sa BURGER_MAXINT64, or BURGER_MAXUINT
	
***************************************/

/*! ************************************

	\def BURGER_MAXWORDPTR
	\brief Maximum value of a unsigned address space integer.
	
	This is a constant value of the largest allowable
	address. Normally, this is 0xFFFFFFFF on 32 bit CPUs
	and 0xFFFFFFFFFFFFFFFFULL on 64 bit CPUs. It may
	be a 128 bit value on future processors.
	\sa BURGER_MAXINTPTR, BURGER_MAXINT or BURGER_MAXUINT
	
***************************************/

/*! ************************************

	\def BURGER_MAXINTPTR
	\brief Maximum value of a signed address space integer.
	
	This is a constant value of the largest allowable
	address. Normally, this is 0x7FFFFFFF on 32 bit CPUs
	and 0x7FFFFFFFFFFFFFFFLL on 64 bit CPUs. It may
	be a 128 bit value on future processors.
	\sa BURGER_MAXWORDPTR, BURGER_MAXINT or BURGER_MAXUINT
	
***************************************/

/*! ************************************

	\def BURGER_INLINE
	\brief Define to create inline code in ANSI C source files.
	
	Since pre-C++ compilers created different keywords to
	force inlining of a function, this define standardizes
	the keyword. Normally, it will evaluate to inline, but
	it can become __inline or __forceinline or some other token that is
	compiler specific.
	
	In all cases, the behavior of the C++ inline keyword is
	what is expected.
	
***************************************/

/*! ************************************

	\def BURGER_API
	\brief Default parameter passing type for Burgerlib functions.
	
	On Intel platforms, there are several methods of passing parameters.
	Burgerlib uses the __fastcall method and this define will evaluate
	to __fastcall on compilers that support it. Otherwise, this will
	evaluate to nothing on non-intel platforms.
	
	For non Intel platforms, this resolves into a blank \#define.
	
	\sa BURGER_ANSIAPI or BURGER_FASTCALLENABLED
	
***************************************/

/*! ************************************

	\def BURGER_ANSIAPI
	\brief Default parameter passing type for ANSI library functions.
	
	On Intel platforms, there are several methods of passing parameters.
	ANSI uses the __cdecl keyword, so any ANSI function like qsort() or atexit()
	that needs a callback function should have this keyword before
	the user supplied callback. It will ensure
	that the linkage will be correct. This will
	evaluate to nothing on non-intel platforms.
	
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

	\def BURGER_ASM
	\brief "C" compiler keyword for inline assembly.
	
	No one could agree on what would the keyword be to start a
	block of inline assembly. This will map to asm or _asm depending
	on the compiler.
	
	It's preferred that this keyword is used for Intel assembly.
	
	\sa BURGER_DECLSPECNAKED or BURGER_ANSIAPI
	
***************************************/

/*! ************************************

	\def BURGER_FASTCALLENABLED
	\brief Flag for determining Intel assembly generation
	
	This flag is defined only if the target platform is intel
	and the parameter passing is in the "__fastcall" format.
	
	Some targets require parameters to be passed in the __cdecl
	format, so this will allow the selection of the proper
	assembly header and footer without having to create
	complicated \#ifdefs
	
	\sa BURGER_API, BURGER_ANSIAPI or BURGER_DECLSPECNAKED
	
***************************************/


/*! ************************************

	\def BURGER_DECLSPECNAKED
	\brief "C" compiler keyword for a pure assembly function.
	
	No one could agree on what would the keyword be to declare
	a "C" function that is 100% assembly. This will map to
	asm or __declspec(naked) depending on the compiler.
	
	It's preferred that this keyword is used for Intel assembly only.
	
	\note Use of this keyword implies that there is no "C" glue code so
	setup, teardown and exiting must all be present in the assembly code.
	
	\code
	BURGER_DECLSPECNAKED Word32 BURGER_API SwapEndian(Word32 Val)
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

	\def BURGER_ALIGN
	\brief Macro to force data alignment.
	
	Some data needs to be aligned differently than native alignment. Use this macro
	to have a compiler agnostic way to ensure data is aligned in a method that
	is desired by the programmer.
	
	\note The size must be a power of two. Acceptable sizes are 1, 2, 4, 8, 16 and 32. Larger
	are acceptable, as long as they are powers of two.
	
	\code
	extern int BURGER_ALIGN(foo,8);
	CoolClass BURGER_ALIGN(MyClass,1);
	int BURGER_ALIGN(DataArray[4],8) = {1,2,3,4};
	\endcode

	\sa BURGER_PREALIGN and BURGER_POSTALIGN
		
***************************************/

/*! ************************************

	\def BURGER_PREALIGN
	\brief First half of a macro to force data alignment.
	
	Some data needs to be aligned differently than native alignment. However,
	it's sometimes impractical to use the \ref BURGER_ALIGN macro
	so this macro and its compliment are used to align complex objects
	like classes and structures. Use the code example below for proper use.
	
	\note The size must be a power of two. Acceptable sizes are 1, 2, 4, 8, 16 and 32. Larger
	are acceptable, as long as they are powers of two.
	
	\code
	BURGER_PREALIGN(32) class foo {
	public:
		foo();
		int m_Data;		// This class is 32 byte aligned
	} BURGER_POSTALIGN(32);
	\endcode

	\sa BURGER_ALIGN and BURGER_POSTALIGN
		
***************************************/

/*! ************************************

	\def BURGER_POSTALIGN
	\brief Second half of a macro to force data alignment.
	
	Some data needs to be aligned differently than native alignment. However,
	it's sometimes impractical to use the \ref BURGER_ALIGN macro
	so this macro and its compliment are used to align complex objects
	like classes and structures. Use the code example below for proper use.
	
	\note The size must be a power of two. Acceptable sizes are 1, 2, 4, 8, 16 and 32. Larger
	are acceptable, as long as they are powers of two.
	
	\code
	BURGER_PREALIGN(32) class foo {
	public:
		foo();
		int m_Data;		// This class is 32 byte aligned
	} BURGER_POSTALIGN(32);
	\endcode

	\sa BURGER_PREALIGN and BURGER_ALIGN
		
***************************************/

/*! ************************************

	\def BURGER_STRUCT_ALIGN
	\brief Defined if the compiler uses Macintosh style data alignment commands.
	
	Compilers can be set to change the default alignment of data structures. If alignment
	for a specific header needs to be set, you will need to insert this code as a prefix
	and a postfix to make sure that the compiler settings are overridden. 
	
	This is useful for data structures that are mapped to the contents of a loaded file.
	
	\note Care must be exercised when changing alignment to byte or short. Some CPUs will 
	generate a page fault if a 32 bit value is not 4 byte aligned.
	
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

	\def BURGER_STRUCT_PACK
	\brief Defined if the compiler uses GNU style data alignment commands.
	
	Compilers can be set to change the default alignment of data structures. If alignment
	for a specific header needs to be set, you will need to insert this code as a prefix
	and a postfix to make sure that the compiler settings are overridden. 
	
	This is useful for data structures that are mapped to the contents of a loaded file.
	
	\note Care must be exercised when changing alignment to byte or short. Some CPUs will 
	generate a page fault if a 32 bit value is not 4 byte aligned.
	
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

	\def BURGER_STRUCT_PACKPUSH
	\brief Defined if the compiler uses Microsoft style data alignment commands.
	
	Compilers can be set to change the default alignment of data structures. If alignment
	for a specific header needs to be set, you will need to insert this code as a prefix
	and a postfix to make sure that the compiler settings are overridden. 
	
	This is useful for data structures that are mapped to the contents of a loaded file.
	
	\note Care must be exercised when changing alignment to byte or short. Some CPUs will 
	generate a page fault if a 32 bit value is not 4 byte aligned.
	
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

	\def BURGER_NO_USING_NAMESPACE
	\brief Define to disable using namespace Burger;
	
	If this define exists, then code found in burger.h "using namespace Burger;"
	will be disabled. The default behavior when using burgerlib is to assume
	the namespace Burger is enabled. For some codebases, there may be name
	collisions and this is to remove the collisions.

	\note Define this manually BEFORE the inclusion of burger.h or define it in the
	compiler command line.
	
	\code
	// Code in burger.h
	#if !defined(BURGER_NO_USING_NAMESPACE)
	using namespace Burger;
	#endif

	// Code in a user C file
	#define BURGER_NO_USING_NAMESPACE
	#include <burger.h>

	// From here on out, use Burgerlib by invoking the Burger namespace
	\endcode
	
***************************************/

/*! ************************************

	\def BURGER_OPENGL_SUPPORTED
	\brief Define for the presence of OpenGL
	
	If this define exists, then OpenGL is supported.
	\sa BURGER_OPENGLES

***************************************/

/*! ************************************

	\def BURGER_OPENGLES
	\brief Define for the presence of OpenGL ES
	
	If this define exists, then OpenGL ES is supported.
	\sa BURGER_OPENGL_SUPPORTED

***************************************/

/*! ************************************

	\def BURGER_OFFSETOF
	\brief Define to return the offset of a member variable.
	
	Return the byte offset of a member variable from a class or struct.
	
	\param __type Name of the class / struct type
	\param member Name of the member in the type to determine the offset of.
	
	\code
	struct foo {
		int m_member;
		int m_member2;
	};

	// Will equal to 4 (Usually)	
	int offsetofmember2 = BURGER_OFFSETOF(foo,m_member2);

	\endcode
	
***************************************/

/*! ************************************

	\def BURGER_ARRAYSIZE
	\brief Define to return the number of elements in an array.
	
	For arrays where the size is known at compile time, this macro
	will return the number of elements found in the array
	
	\param x Name of the array to determine the size
	
	\code
	int Foo[100];

	// Will equal to 100	
	int ElementsInFoo = BURGER_ARRAYSIZE(Foo);

	\endcode
	
***************************************/

/*! ************************************

	\def BURGER_UNUSED
	\brief Define to disable the unused variable warning.
	
	In cases where a variable is defined, but not used, such
	as in testing error conditions with asserts, this
	macro will alert the compiler that the existence
	of this varible was intentional.
	
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
	
	I_FEAR_NOTHING(printf("Error code %d",iError));
	
	\endcode
	
***************************************/


/*! ************************************

	\def BURGER_HASHMACRO
	\brief Convert the parameter into a string literal.
	
	Using the # token, convert the macro parameter into an 8 bit quoted string literal.
	
	\param x Name of the item to turn into a string
	
	\code
	int foo = 4;
	printf(BURGER_HASHMACRO(foo));
	
	// Prints "foo" (Without the quotes)

	\endcode
	
***************************************/

/*! ************************************

	\def BURGER_MACRO_TO_STRING
	\brief Convert a macro into a string literal.
	
	Using deep magic, convert the macro into an 8 bit quoted string literal.
	
	\param x Name of the macro to turn into a string
	
	\code
	#define foo 4
	printf(BURGER_HASHMACRO(foo));
	
	// Prints "4" (Without the quotes)

	\endcode
	
***************************************/

/*! ************************************

	\def BURGER_DISABLECOPYCONSTRUCTORS
	\brief Declares copy constructors to disable invocation at runtime
	
	Some classes by their nature should not be or can't be copied,
	by declaring this macro, the x(x const &) constructor and
	the x &operator = (x const &) operator are declared.
	
	\note This macro MUST be invoked in a private section of a class
	or it will inadvertently declare that there are copy constructors
	in existence which could lead to mysterious link errors.
	
	\param x Name of the class that the macro resides in
		
	\code
	class DontCopyMe {
		// Disallow copying of this class
		BURGER_DISABLECOPYCONSTRUCTORS(DontCopyMe);
	public:
		void DoStuff(void);

	\endcode

	\note This macro will set the class setting to "private"
	
***************************************/

/*! ************************************

	\def BURGER_LONGLONG
	\brief 64 bit type specific to the current compiler.
	
	Define that has the compiler keyword that defines a 64 bit integer data type.
	
	\sa Int64 and Word64
	
***************************************/

/*! ************************************

	\def BURGER_LONGIS64BIT
	\brief Integer type "long" is 64 bits wide.

	Under normal circumstances, long is a 32 bit wide integer
	but on some compilers, this is a 64 bit integer. This
	define is present on platforms where the compiler
	is treating a "long" as a 64 bit integer

	\sa BURGER_PS2
	
***************************************/

/*! ************************************

	\def BURGER_HASWCHAR_T
	\brief Data type wchar_t is native

	If this define is present, wchar_t is a native type 
	for the compiler, otherwise, it's a typedef cast from
	an unsigned short, which may cause collisions for classes
	that want to treat wchar_t and Word16 as unique data types.
	
***************************************/

/*! ************************************

	\typedef Word8
	\brief 8 bit unsigned integer.
	
	This integer can contain the number 0 through 255. It
	is compiler switch setting safe.
	
	\sa Int8
	
***************************************/

/*! ************************************

	\typedef Int8
	\brief 8 bit signed integer.
	
	This integer can contain the number -128 through 127. It
	is compiler switch setting safe.
	
	\sa Word8
	
***************************************/

/*! ************************************

	\typedef Word16
	\brief 16bit unsigned integer.
	
	This integer can contain the number 0 through 65535. It
	is compiler switch setting safe.
	
	\sa Int16
	
***************************************/

/*! ************************************

	\typedef Int16
	\brief 16 bit signed integer.
	
	This integer can contain the number -32768 through 32767. It
	is compiler switch setting safe.
	
	\sa Word16
	
***************************************/

/*! ************************************

	\typedef Int32
	\brief 32 bit signed integer.
	
	This integer can contain the number -2,147,483,648 through 2,147,483,647. It
	is compiler switch setting safe.
	
	\note This is cast as a signed int on the Playstation 2.
	
	\sa Word32 or Word
	
***************************************/

/*! ************************************

	\typedef Word32
	\brief 32 bit unsigned integer.
	
	This integer can contain the number 0 through 4,294,967,295. It
	is compiler switch setting safe.

	\note This is cast as an unsigned int on the Playstation 2.
	
	\sa Int32 or Int
	
***************************************/

/*! ************************************

	\typedef Int64
	\brief 64 bit signed integer.
	
	This integer can contain the number -9,223,372,036,854,775,808 through 9,223,372,036,854,775,807. It
	is compiler switch setting safe.
	
	\note This is cast as a signed long on the Playstation 2.
	
	\note Some processors like the 680x0 do not have registers this large
	and as a result it will incur a performance penalty. Use this type carefully.
	
	\sa Word64
	
***************************************/

/*! ************************************

	\typedef Word64
	\brief 64 bit unsigned integer.
	
	This integer can contain the number 0 through 
	18,446,744,073,709,551,615. It
	is compiler switch setting safe.

	\note This is cast as an unsigned long on the Playstation 2.
	
	\note Some processors like the 680x0 do not have registers this large
	and as a result it will incur a performance penalty. Use this type carefully.
		
	\sa Int64
	
***************************************/

/*! ************************************

	\struct Vector_128
	\brief 128 bit vector intrinsic.
	
	This integer is used for register remapping to the vector units
	on several CPUs. It is not to be used for 128 bit integer math.
	Currently, only processors with vector units like the Playstation 2, 3, 4,
	Xbox 360, XBox ONE, Pentium with SSE, ARM with NEON and PowerPC with Alti-Vec can
	use this intrinsic data type.

	\sa Burger::Vector_128Float32, Burger::Vector_128Word32, Burger::Vector_128Int32,
		Burger::Vector_128Word16, Burger::Vector_128Word8,
		BURGER_ARM, BURGER_POWERPC or BURGER_X86

***************************************/

/*! ************************************

	\typedef WordPtr
	\brief Unsigned integer that matches in size to a void *.
	
	Since address pointers can be 64 bit on some platforms,
	this type allows the casting and storing of this type
	into an unsigned integer,

	\note Caution should be used when using this type in
	data structures due to it being 64 or 32 bit on different
	platforms.
	
	\sa IntPtr 
	
***************************************/

/*! ************************************

	\typedef IntPtr
	\brief Signed integer that matches in size to a void *.
	
	Since address pointers can be 64 bit on some platforms,
	this type allows the casting and storing of this type
	into an signed integer,

	\note Caution should be used when using this type in
	data structures due to it being 64 or 32 bit on different
	platforms.
	
	\sa WordPtr 
	
***************************************/


/*! ************************************

	\typedef Bool
	\brief Unsigned 8 bit integer for boolean operations.
	
	When data storage is at a premium, this data type can be used
	to alert programmers that only a true or not true can
	be stored inside. Using the Word8 data type could imply that 
	the data has a wider range.
	
	\sa Word8 or Int8.
	
***************************************/

/*! ************************************

	\typedef Fixed32
	\brief Signed 32 bit integer for fixed point operations.
	
	All 16.16 fixed point integer math uses this data type to
	alert the programmer of the special nature of the 32 bits.
	The upper 16 bits is an integer of the range of -32768 through
	32767 and the lower 16 bits is a fraction of x/65536.
	
	\sa \ref FLOATTOFIXED, \ref FIXEDTOFLOAT, Int32 or Frac32.
	
***************************************/

/*! ************************************

	\typedef Frac32
	\brief Signed 32 bit integer for fixed point operations.
	
	All 2.30 fixed point integer math uses this data type to
	alert the programmer of the special nature of the 32 bits.
	The upper 2 bits is an integer of the range of -2 through
	1 and the lower 30 bits is a fraction of x/1,073,741,824.
	
	\sa Int32 or Fixed32.
	
***************************************/

/*! ************************************

	\typedef Word
	\brief Unsigned integer.
	
	This is the most common data type in BurgerLib. On
	most platforms, this is an unsigned 32 bit integer but it
	can be 64 bit if the processor handles 64 bit data more
	efficently than 32 bit. To date, all CPUs handle 32 bit
	data with no penalty so it's locked at 32 bits.
	
	\sa Int 
	
***************************************/

/*! ************************************

	\typedef Int
	\brief Signed integer.
	
	This is the second most common data type in BurgerLib. On
	most platforms, this is an signed 32 bit integer but it
	can be 64 bit if the processor handles 64 bit data more
	efficently than 32 bit. To date, all CPUs handle 32 bit
	data with no penalty so it's locked at 32 bits.
	
	\sa Word
	
***************************************/

/*! ************************************

	\fn void Burger::SwapVariables(T *pA,T *pB)
	\brief Swap two variables of the same kind
	
	Given pointers to two like variables, swap the contents with
	each other.
	
	\note This template will fail if the T value is of a class
	that doesn't allow copying.

	\param pA Pointer to the first variable to swap.
	\param pB Pointer to the second variable to swap.

***************************************/

#endif
