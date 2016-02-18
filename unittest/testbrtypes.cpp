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

#include "testbrtypes.h"
#include "bratomic.h"
#include "brglobals.h"
#include "common.h"
#include <stdio.h>

#if defined(BURGER_WATCOM)
#pragma disable_message(13)		// Disable unreachable code warning
#pragma disable_message(367)	// Disable conditional expression is constant warning
#pragma disable_message(368)	// Disable conditional expression in if statement is always false
#endif

#if defined(BURGER_MSVC)
#pragma warning(disable:4003)	// Disable not enough actual parameters for macro
#pragma warning(disable:4127)	// Disable conditional expression is constant warning
#pragma warning(disable:4324)	// Disable structure was padded due to __declspec(align())
#endif

//
// This structure is used to test if alignment settings of the compiler works
// with certain assumptions about data alignment
// Of course, assuming the padding of data by a compiler is a no-no
// since padding is wasted space
//

struct Aligntest_t {
	Int8 m_0;					// Initial value
	Int8 BURGER_ALIGN(m_16,16);	// Should be at offset 16
	Int8 m_17;					// Offset 17
	Int8 BURGER_ALIGN(m_20,4);	// Offset 20
	Int8 m_21;					// Offset 21
	Int8 BURGER_ALIGN(m_32,16);	// Offset 32
	Int8 m_33;					// Offset 33
	Int64 m_40;					// Offset 40
	Int8 m_48;					// Offset 48
	Int16 m_50;					// Offset 50
	Int8 m_52;					// Offset 52
	Int32 m_56;					// Offset 56
};

union Endiantest_t {
	Word32 m_uWord;
	Word8 m_uBytes[4];
};

//
// Perform the tests for the macros and compiler
// settings
//

int BURGER_API TestBrtypes(void)
{
	Word uFailure = 0;
	Word uTest;

	Message("Testing macros");

	//
	// Endian macros
	//

#if defined(BURGER_LITTLEENDIAN) && defined(BURGER_BIGENDIAN)
	Message("Error: Both BURGER_LITTLEENDIAN and BURGER_BIGENDIAN are defined!");
	uFailure = 10;
#endif

#if !defined(BURGER_LITTLEENDIAN) && !defined(BURGER_BIGENDIAN)
	Message("Error: Neither BURGER_LITTLEENDIAN or BURGER_BIGENDIAN are defined!");
	uFailure = 10;
#endif

	Endiantest_t EndianTest;
	EndianTest.m_uWord = 0x12345678U;

#if defined(BURGER_BIGENDIAN)
	uTest = (EndianTest.m_uBytes[0] == 0x78);
	uFailure |= uTest;
	ReportFailure("BURGER_BIGENDIAN was defined on a Little endian machine!",uTest);
#endif
#if defined(BURGER_LITTLEENDIAN)
	uTest = (EndianTest.m_uBytes[0] == 0x12);
	uFailure |= uTest;
	ReportFailure("BURGER_LITTLEENDIAN was defined on a Big endian machine!",uTest);
#endif

	// Display the debug/release macro

#if defined(NDEBUG) && defined(_DEBUG)
	Message("Error: Both NDEBUG and _DEBUG are defined!");
	uFailure = 10;
#endif
#if !defined(NDEBUG) && !defined(_DEBUG)
	Message("Error: Neither NDEBUG or _DEBUG are defined!");
	uFailure = 10;
#endif

#if defined(NDEBUG)
	Message("NDEBUG is defined");
#endif
#if defined(_DEBUG)
	Message("_DEBUG is defined");
#endif

	//
	// Compiler macros
	//

	uTest = 0;
#if defined(BURGER_WATCOM)
	Message("BURGER_WATCOM is defined");
	++uTest;
#endif
#if defined(BURGER_MRC)
	Message("BURGER_MRC is defined");
	++uTest;
#endif
#if defined(BURGER_APPLESC)
	Message("BURGER_APPLESC is defined");
	++uTest;
#endif
#if defined(BURGER_INTELCOMPILER)
	Message("BURGER_INTELCOMPILER is defined");
	++uTest;	
#endif
#if defined(BURGER_SNSYSTEMS)
	Message("BURGER_SNSYSTEMS is defined");
	++uTest;
#endif
#if defined(BURGER_METROWERKS)
	Message("BURGER_METROWERKS is defined");
	++uTest;
#endif
#if defined(BURGER_MSVC)
	Message("BURGER_MSVC is defined");
	++uTest;
#endif
#if defined(BURGER_GNUC)
	Message("BURGER_GNUC is defined");
	++uTest;
#endif
	if (uTest>=2) {
		Message("Multiple compilers have been defined!");
		uFailure = 10;
	}
	if (uTest==0) {
		Message("Unknown compiler detected!");
		uFailure = 10;
	}
	
	// CPU macros
	uTest = 0;
#if defined(BURGER_68K)
	Message("BURGER_68K is defined");
	++uTest;
#endif
#if defined(BURGER_POWERPC)
	Message("BURGER_POWERPC is defined");
	++uTest;
#endif
#if defined(BURGER_POWERPC64)
	Message("BURGER_POWERPC64 is defined");
	++uTest;
#endif
#if defined(BURGER_ARM)
	Message("BURGER_ARM is defined");
	++uTest;
#endif
#if defined(BURGER_X86)
	Message("BURGER_X86 is defined");
	++uTest;
#endif
#if defined(BURGER_AMD64)
	Message("BURGER_AMD64 is defined");
	++uTest;
#endif
#if defined(BURGER_MIPS)
	Message("BURGER_MIPS is defined");
	++uTest;
#endif
#if defined(BURGER_GEKKO)
	Message("BURGER_GEKKO is defined");
	++uTest;
#endif

// This is an enhancement, not a CPU type, so it's okay to
// be defined with a CPU type

#if defined(BURGER_64BITCPU)
	Message("BURGER_64BITCPU is defined");
#endif
	if (uTest!=1) {
		Message("Multiple CPUs have been defined!");
		uFailure = 10;
	}
	if (uTest==0) {
		Message("Unknown CPU detected!");
		uFailure = 10;
	}

// Platform macros
	
	uTest = 0;
#if defined(BURGER_MSDOS)
	Message("BURGER_MSDOS is defined");
	++uTest;
#endif
#if defined(__DOS4G__)
	Message("__DOS4G__ is defined");
#endif
#if defined(__X32__)
	Message("__X32__ is defined");
#endif

#if defined(BURGER_WIN32)
	Message("BURGER_WIN32 is defined");
	++uTest;
#endif
#if defined(BURGER_WIN64)
	Message("BURGER_WIN64 is defined");
	++uTest;
#endif
#if defined(BURGER_WINDOWS)
	Message("BURGER_WINDOWS is defined");
#endif

#if defined(BURGER_BEOS)
	Message("BURGER_BEOS is defined");
	++uTest;
#endif

#if defined(BURGER_MAC)
	Message("BURGER_MAC is defined");
	++uTest;
#endif
#if defined(BURGER_MACOSX)
	Message("BURGER_MACOSX is defined");
	++uTest;
#endif
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


#if defined(BURGER_IOS)
	Message("BURGER_IOS is defined");
	++uTest;
#endif
#if defined(BURGER_ANDROID)
	Message("BURGER_ANDROID is defined");
	++uTest;
#endif
#if defined(BURGER_NGAGE)
	Message("BURGER_NGAGE is defined");
	++uTest;
#endif
#if defined(BURGER_SYMBIAN)
	Message("BURGER_SYMBIAN is defined");
	++uTest;
#endif
#if defined(BURGER_OUYA)
	Message("BURGER_OUYA is defined");
	++uTest;
#endif
#if defined(BURGER_SHIELD)
	Message("BURGER_SHIELD is defined");
	++uTest;
#endif

#if defined(BURGER_XBOX)
	Message("BURGER_XBOX is defined");
	++uTest;
#endif
#if defined(BURGER_XBOX360)
	Message("BURGER_XBOX360 is defined");
	++uTest;
#endif
#if defined(BURGER_XBOXONE)
	Message("BURGER_XBOXONE is defined");
	++uTest;
#endif

#if defined(BURGER_PS2)
	Message("BURGER_PS2 is defined");
	++uTest;
#endif
#if defined(BURGER_PS3)
	Message("BURGER_PS3 is defined");
	++uTest;
#endif
#if defined(BURGER_PS4)
	Message("BURGER_PS4 is defined");
	++uTest;
#endif
#if defined(BURGER_VITA)
	Message("BURGER_VITA is defined");
	++uTest;
#endif

#if defined(BURGER_GAMECUBE)
	Message("BURGER_GAMECUBE is defined");
	++uTest;
#endif
#if defined(BURGER_WII)
	Message("BURGER_WII is defined");
	++uTest;
#endif
#if defined(BURGER_DS)
	Message("BURGER_DS is defined");
	++uTest;
#endif
	if (uTest>=2) {
		Message("Multiple Platforms have been defined!");
		uFailure = 10;
	}
	if (uTest==0) {
		Message("Unknown Platform detected!");
		uFailure = 10;
	}

	// Display standard macros

	Message("BURGER_ALIGN is defined at \"" BURGER_MACRO_TO_STRING(BURGER_ALIGN(x,s)) "\"");
	Message("BURGER_PREALIGN is defined at \"" BURGER_MACRO_TO_STRING(BURGER_PREALIGN(s)) "\"");
	Message("BURGER_POSTALIGN is defined at \"" BURGER_MACRO_TO_STRING(BURGER_POSTALIGN(s)) "\"");

	Message("BURGER_API is defined at \"" BURGER_MACRO_TO_STRING(BURGER_API) "\"");
	Message("BURGER_ANSIAPI is defined at \"" BURGER_MACRO_TO_STRING(BURGER_ANSIAPI) "\"");
	Message("BURGER_INLINE is defined at \"" BURGER_MACRO_TO_STRING(BURGER_INLINE) "\"");
	Message("BURGER_DECLSPECNAKED is defined at \"" BURGER_MACRO_TO_STRING(BURGER_DECLSPECNAKED) "\"");
	Message("BURGER_ASM is defined at \"" BURGER_MACRO_TO_STRING(BURGER_ASM) "\"");
	Message("NULL is defined at \"" BURGER_MACRO_TO_STRING(NULL) "\"");
	Message("TRUE is defined at \"" BURGER_MACRO_TO_STRING(TRUE) "\"");
	Message("FALSE is defined at \"" BURGER_MACRO_TO_STRING(FALSE) "\"");
	Message("BURGER_MAXWORDPTR is defined at \"" BURGER_MACRO_TO_STRING(BURGER_MAXWORDPTR) "\"");
	Message("BURGER_MAXINT is defined at \"" BURGER_MACRO_TO_STRING(BURGER_MAXINT) "\"");
	Message("BURGER_MAXUINT is defined at \"" BURGER_MACRO_TO_STRING(BURGER_MAXUINT) "\"");

// 64 bit supported?

#if defined(BURGER_LONGLONG)
	Message("BURGER_LONGLONG is defined at \"" BURGER_MACRO_TO_STRING(BURGER_LONGLONG) "\"");
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

	// Test structure alignment

	if ((BURGER_OFFSETOF(Aligntest_t,m_0)!=0) ||
		(BURGER_OFFSETOF(Aligntest_t,m_16)!=16) ||
		(BURGER_OFFSETOF(Aligntest_t,m_17)!=17) ||
		(BURGER_OFFSETOF(Aligntest_t,m_20)!=20) ||
		(BURGER_OFFSETOF(Aligntest_t,m_21)!=21) ||
		(BURGER_OFFSETOF(Aligntest_t,m_32)!=32) ||
		(BURGER_OFFSETOF(Aligntest_t,m_33)!=33) ||
		(BURGER_OFFSETOF(Aligntest_t,m_40)!=40) ||
		(BURGER_OFFSETOF(Aligntest_t,m_48)!=48) ||
		(BURGER_OFFSETOF(Aligntest_t,m_50)!=50) ||
		(BURGER_OFFSETOF(Aligntest_t,m_52)!=52) ||
		(BURGER_OFFSETOF(Aligntest_t,m_56)!=56)) {
		Message("Offset m_0 = %u",static_cast<Word>(BURGER_OFFSETOF(Aligntest_t,m_0)));
		Message("Offset m_16 = %u (align 16)",static_cast<Word>(BURGER_OFFSETOF(Aligntest_t,m_16)));
		Message("Offset m_17 = %u",static_cast<Word>(BURGER_OFFSETOF(Aligntest_t,m_17)));
		Message("Offset m_20 = %u (align 4)",static_cast<Word>(BURGER_OFFSETOF(Aligntest_t,m_20)));
		Message("Offset m_21 = %u",static_cast<Word>(BURGER_OFFSETOF(Aligntest_t,m_21)));
		Message("Offset m_32 = %u (align 16)",static_cast<Word>(BURGER_OFFSETOF(Aligntest_t,m_32)));
		Message("Offset m_33 = %u",static_cast<Word>(BURGER_OFFSETOF(Aligntest_t,m_33)));
		Message("Offset m_40 = %u (Int64)",static_cast<Word>(BURGER_OFFSETOF(Aligntest_t,m_40)));
		Message("Offset m_48 = %u",static_cast<Word>(BURGER_OFFSETOF(Aligntest_t,m_48)));
		Message("Offset m_50 = %u (Int16)",static_cast<Word>(BURGER_OFFSETOF(Aligntest_t,m_50)));
		Message("Offset m_52 = %u",static_cast<Word>(BURGER_OFFSETOF(Aligntest_t,m_52)));
		Message("Offset m_56 = %u (Int32)",static_cast<Word>(BURGER_OFFSETOF(Aligntest_t,m_56)));
		uFailure = 10;
	} else {
		Message("Structure alignment test passed");
	}

	//
	// Test default data chunk sizes
	//

	if (sizeof(Word8)!=1) {
		ReportFailure("sizeof(Word8) is %u instead of 1.",TRUE,static_cast<Word>(sizeof(Word8)));
		uFailure = 10;
	}
	if (sizeof(Int8)!=1) {
		ReportFailure("sizeof(Int8) is %u instead of 1.",TRUE,static_cast<Word>(sizeof(Int8)));
		uFailure = 10;
	}

	if (sizeof(Word16)!=2) {
		ReportFailure("sizeof(Word16) is %u instead of 2.",TRUE,static_cast<Word>(sizeof(Word16)));
		uFailure = 10;
	}
	if (sizeof(Int16)!=2) {
		ReportFailure("sizeof(Int16) is %u instead of 2.",TRUE,static_cast<Word>(sizeof(Int16)));
		uFailure = 10;
	}

	if (sizeof(Word32)!=4) {
		ReportFailure("sizeof(Word32) is %u instead of 4.",TRUE,static_cast<Word>(sizeof(Word32)));
		uFailure = 10;
	}
	if (sizeof(Int32)!=4) {
		ReportFailure("sizeof(Int32) is %u instead of 4.",TRUE,static_cast<Word>(sizeof(Int32)));
		uFailure = 10;
	}

	if (sizeof(Word64)!=8) {
		ReportFailure("sizeof(Word64) is %u instead of 8.",TRUE,static_cast<Word>(sizeof(Word64)));
		uFailure = 10;
	}
	if (sizeof(Int64)!=8) {
		ReportFailure("sizeof(Int64) is %u instead of 8.",TRUE,static_cast<Word>(sizeof(Int64)));
		uFailure = 10;
	}

	if (sizeof(void *)!=sizeof(WordPtr)) {
		ReportFailure("sizeof(void *) is %u instead of sizeof(WordPtr).",TRUE,static_cast<Word>(sizeof(void *)));
		uFailure = 10;
	}

	if (sizeof(Bool)!=1) {
		ReportFailure("sizeof(Bool) is %u instead of 1.",TRUE,static_cast<Word>(sizeof(Bool)));
		uFailure = 10;
	}
	if (sizeof(Frac32)!=4) {
		ReportFailure("sizeof(Frac32) is %u instead of 4.",TRUE,static_cast<Word>(sizeof(Frac32)));
		uFailure = 10;
	}
	if (sizeof(Fixed32)!=4) {
		ReportFailure("sizeof(Fixed32) is %u instead of 4.",TRUE,static_cast<Word>(sizeof(Fixed32)));
		uFailure = 10;
	}
	if (sizeof(Word)!=4) {
		ReportFailure("sizeof(Word) is %u instead of 4.",TRUE,static_cast<Word>(sizeof(Word)));
		uFailure = 10;
	}
	if (sizeof(Int)!=4) {
		ReportFailure("sizeof(Int) is %u instead of 4.",TRUE,static_cast<Word>(sizeof(Int)));
		uFailure = 10;
	}

	if (sizeof(Vector_128)!=16) {
		ReportFailure("sizeof(Vector_128) is %u instead of 16.",TRUE,static_cast<Word>(sizeof(Vector_128)));
		uFailure = 10;
	}

	//
	// Handle CPUID
	//

	uTest = Burger::IsCPUIDPresent();
	Message("Burger::IsCPUIDPresent() = %u",uTest);

	Burger::CPUID_t MyID;
	Burger::CPUID(&MyID);
	if (MyID.m_uHighestCPUID) {
		Message("CPUID_t.m_uHighestCPUID = %08X",MyID.m_uHighestCPUID);
		Message("CPUID_t.m_uHighestCPUIDExtended = %08X",MyID.m_uHighestCPUIDExtended);
		Message("CPUID_t.m_uHighestCPUIDHyperVisor = %08X",MyID.m_uHighestCPUIDHyperVisor);
		Message("CPUID_t.m_uModel = %08X",MyID.m_uModel);
		Message("CPUID_t.m_uBrand = %08X",MyID.m_uBrand);
		Message("CPUID_t.m_uFeatureInformation = %08X",MyID.m_uFeatureInformation);
		Message("CPUID_t.m_uExtendedFeatureInformation = %08X",MyID.m_uExtendedFeatureInformation);
		Message("CPUID_t.m_uExtendedProcessorInformation = %08X",MyID.m_uExtendedProcessorInformation);
		Message("CPUID_t.m_uExtendedFeatureBits = %08X",MyID.m_uExtendedFeatureBits);
		Message("CPUID_t.m_uCPUType = %u",static_cast<Word>(MyID.m_uCPUType));
		Message("CPUID_t.m_CPUName = %s",MyID.m_CPUName);
		Message("CPUID_t.m_BrandName = %s",MyID.m_BrandName);
		Message("CPUID_t.m_HypervisorName = %s",MyID.m_HypervisorName);
		Message("CPUID_t.m_HypervisorSignature = %s",MyID.m_HypervisorSignature);
	} else {
		Message("Burger::CPUID() is not available");
	}

	//
	// Handle AltiVec
	//

	uTest = Burger::HasAltiVec();
	Message("Burger::HasAltiVec() = %u",uTest);

	//
	// Handle MacOS Version
	//

#if defined(BURGER_MACOS)
	uTest = Burger::Globals::GetMacOSVersion();
	Message("Burger::Globals::GetMacOSVersion() = %04X",uTest);
#endif

	//
	// Mac OS functions
	
#if defined(BURGER_MAC)
	// _MemoryDispatch = 0xA05C
	uTest = Burger::Globals::IsTrapAvailable(0xA05C);
	Message("Burger::Globals::IsTrapAvailable(0xA05C) = %u",uTest);

	uTest = Burger::Globals::IsQuickTimePowerPlugAvailable();
	Message("Burger::Globals::IsQuickTimePowerPlugAvailable() = %u",uTest);

	uTest = Burger::Globals::GetAppleShareVersion();
	Message("Burger::Globals::GetAppleShareVersion() = %04X",uTest);

	uTest = Burger::Globals::GetInputSprocketVersion();
	Message("Burger::Globals::GetInputSprocketVersion() = %04X",uTest);

	uTest = Burger::Globals::GetDrawSprocketVersion();
	Message("Burger::Globals::GetDrawSprocketVersion() = %04X",uTest);

#endif

	return static_cast<int>(uFailure);
}