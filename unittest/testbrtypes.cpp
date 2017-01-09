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

#include "testbrtypes.h"
#include "bratomic.h"
#include "brglobals.h"
#include "common.h"
#include <stdio.h>

#if defined(BURGER_MSVC)
#pragma warning(disable:4003)	// Disable not enough actual parameters for macro
#pragma warning(disable:4324)	// Disable structure was padded due to __declspec(align())
#endif

/***************************************

	Test the endian macros

***************************************/

static Word BURGER_API TestEndianMacros(void)
{
	union {
		Word32 m_uWord;				// 32 bit integer to store
		Word8 m_uBytes[4];			// Byte array to test the endian with
	} EndianTest;

	Word uFailure = 0;
	
#if defined(BURGER_LITTLEENDIAN) && defined(BURGER_BIGENDIAN)
	Message("Error: Both BURGER_LITTLEENDIAN and BURGER_BIGENDIAN are defined!");
	uFailure = 10;
#endif

#if !defined(BURGER_LITTLEENDIAN) && !defined(BURGER_BIGENDIAN)
	Message("Error: Neither BURGER_LITTLEENDIAN or BURGER_BIGENDIAN are defined!");
	uFailure = 10;
#endif

	//
	// Test the endian ACTUALLY matches
	//

	EndianTest.m_uWord = 0x12345678U;
	Word uTest;

#if defined(BURGER_BIGENDIAN)
	uTest = (EndianTest.m_uBytes[0]==0x78);
	uFailure |= uTest;
	ReportFailure("BURGER_BIGENDIAN was defined on a Little endian machine!",uTest);
#endif

#if defined(BURGER_LITTLEENDIAN)
	uTest = (EndianTest.m_uBytes[0]==0x12);
	uFailure |= uTest;
	ReportFailure("BURGER_LITTLEENDIAN was defined on a Big endian machine!",uTest);
#endif
	return uFailure;
}


/***************************************

	Test the debug flag macros

***************************************/

static Word BURGER_API TestDebugMacros(Word bVerbose)
{
	Word uFailure = 0;

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

	if (bVerbose) {
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

static Word BURGER_API TestCompilerMacros(Word bVerbose)
{
	Word uFailure = 0;
	Word uTest = 0;

#if defined(BURGER_WATCOM)
	if (bVerbose) {
		Message("BURGER_WATCOM is defined");
	}
	++uTest;
#endif

#if defined(BURGER_MRC)
	if (bVerbose) {
		Message("BURGER_MRC is defined");
	}
	++uTest;
#endif

#if defined(BURGER_APPLESC)
	if (bVerbose) {
		Message("BURGER_APPLESC is defined");
	}
	++uTest;
#endif

#if defined(BURGER_INTELCOMPILER)
	if (bVerbose) {
		Message("BURGER_INTELCOMPILER is defined");
	}
	++uTest;
#endif

#if defined(BURGER_SNSYSTEMS)
	if (bVerbose) {
		Message("BURGER_SNSYSTEMS is defined");
	}
	++uTest;
#endif

#if defined(BURGER_METROWERKS)
	if (bVerbose) {
		Message("BURGER_METROWERKS is defined");
	}
	++uTest;
#endif

#if defined(BURGER_MSVC)
	if (bVerbose) {
		Message("BURGER_MSVC is defined");
	}
	++uTest;
#endif

#if defined(BURGER_GNUC)
	if (bVerbose) {
		Message("BURGER_GNUC is defined");
	}
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
	return uFailure;
}

/***************************************

	Test the CPU macros

***************************************/

static Word BURGER_API TestCPUMacros(Word bVerbose)
{
	Word uFailure = 0;
	Word uTest = 0;

#if defined(BURGER_68K)
	if (bVerbose) {
		Message("BURGER_68K is defined");
	}
	++uTest;
#endif

#if defined(BURGER_POWERPC)
	if (bVerbose) {
		Message("BURGER_POWERPC is defined");
	}
	++uTest;
#endif

#if defined(BURGER_POWERPC64)
	if (bVerbose) {
		Message("BURGER_POWERPC64 is defined");
	}
	++uTest;
#endif

#if defined(BURGER_ARM)
	if (bVerbose) {
		Message("BURGER_ARM is defined");
	}
	++uTest;
#endif

#if defined(BURGER_X86)
	if (bVerbose) {
		Message("BURGER_X86 is defined");
	}
	++uTest;
#endif

#if defined(BURGER_AMD64)
	if (bVerbose) {
		Message("BURGER_AMD64 is defined");
	}
	++uTest;
#endif

#if defined(BURGER_MIPS)
	if (bVerbose) {
		Message("BURGER_MIPS is defined");
	}
	++uTest;
#endif

#if defined(BURGER_GEKKO)
	if (bVerbose) {
		Message("BURGER_GEKKO is defined");
	}
	++uTest;
#endif

	// This is an enhancement, not a CPU type, so it's okay to
	// be defined with a CPU type

	if (bVerbose) {
#if defined(BURGER_64BITCPU)
		Message("BURGER_64BITCPU is defined");
#endif

#if defined(BURGER_INTELARCHITECTURE)
		Message("BURGER_INTELARCHITECTURE is defined");
#endif

#if defined(BURGER_NEON)
		Message("BURGER_NEON is defined");
#endif

	}

	if (uTest!=1) {
		Message("Multiple CPUs have been defined!");
		uFailure = 10;
	}
	if (uTest==0) {
		Message("Unknown CPU detected!");
		uFailure = 10;
	}
	return uFailure;
}


/***************************************

	Test the platform macros

***************************************/

static Word BURGER_API TestPlatformMacros(Word bVerbose)
{
	Word uFailure = 0;
	Word uTest = 0;

	//
	// Handle MSDOS
	//

#if defined(BURGER_MSDOS)
	if (bVerbose) {
		Message("BURGER_MSDOS is defined");
	}
	++uTest;
#endif

#if defined(__DOS4G__)
	if (bVerbose) {
		Message("__DOS4G__ is defined");
	}
#endif

#if defined(__X32__)
	if (bVerbose) {
		Message("__X32__ is defined");
	}
#endif

	//
	// Handle Windows
	//

#if defined(BURGER_WIN32)
	if (bVerbose) {
		Message("BURGER_WIN32 is defined");
	}
	++uTest;
#endif

#if defined(BURGER_WIN64)
	if (bVerbose) {
		Message("BURGER_WIN64 is defined");
	}
	++uTest;
#endif

#if defined(BURGER_WINDOWS)
	if (bVerbose) {
		Message("BURGER_WINDOWS is defined");
	}
#endif

	//
	// Dead platforms
	//

#if defined(BURGER_BEOS)
	if (bVerbose) {
		Message("BURGER_BEOS is defined");
	}
	++uTest;
#endif

#if defined(BURGER_NGAGE)
	if (bVerbose) {
		Message("BURGER_NGAGE is defined");
	}
	++uTest;
#endif

#if defined(BURGER_SYMBIAN)
	if (bVerbose) {
		Message("BURGER_SYMBIAN is defined");
	}
	++uTest;
#endif


	//
	// MacOS
	//

#if defined(BURGER_MAC)
	if (bVerbose) {
		Message("BURGER_MAC is defined");
	}
	++uTest;
#endif

#if defined(BURGER_MACOSX)
	if (bVerbose) {
		Message("BURGER_MACOSX is defined");
	}
	++uTest;
#endif

	if (bVerbose) {

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
	if (bVerbose) {
		Message("BURGER_IOS is defined");
	}
	++uTest;
#endif

	//
	// Android and their sub platforms
	//
#if defined(BURGER_ANDROID)
	if (bVerbose) {
		Message("BURGER_ANDROID is defined");
	}
	++uTest;
#endif

#if defined(BURGER_OUYA)
	if (bVerbose) {
		Message("BURGER_OUYA is defined");
	}
	++uTest;
#endif

#if defined(BURGER_SHIELD)
	if (bVerbose) {
		Message("BURGER_SHIELD is defined");
	}
	++uTest;
#endif

	//
	// Video game consoles
	//

#if defined(BURGER_XBOX)
	if (bVerbose) {
		Message("BURGER_XBOX is defined");
	}
	++uTest;
#endif

#if defined(BURGER_XBOX360)
	if (bVerbose) {
		Message("BURGER_XBOX360 is defined");
	}
	++uTest;
#endif

#if defined(BURGER_XBOXONE)
	if (bVerbose) {
		Message("BURGER_XBOXONE is defined");
	}
	++uTest;
#endif

#if defined(BURGER_PS2)
	if (bVerbose) {
		Message("BURGER_PS2 is defined");
	}
	++uTest;
#endif

#if defined(BURGER_PS3)
	if (bVerbose) {
		Message("BURGER_PS3 is defined");
	}
	++uTest;
#endif

#if defined(BURGER_PS4)
	if (bVerbose) {
		Message("BURGER_PS4 is defined");
	}
	++uTest;
#endif

#if defined(BURGER_VITA)
	if (bVerbose) {
		Message("BURGER_VITA is defined");
	}
	++uTest;
#endif

#if defined(BURGER_GAMECUBE)
	if (bVerbose) {
		Message("BURGER_GAMECUBE is defined");
	}
	++uTest;
#endif

#if defined(BURGER_WII)
	if (bVerbose) {
		Message("BURGER_WII is defined");
	}
	++uTest;
#endif

#if defined(BURGER_DS)
	if (bVerbose) {
		Message("BURGER_DS is defined");
	}
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
	return uFailure;
}

/***************************************

	Print the contents of these macros so they
	can be manually checked for correctness

***************************************/

static void BURGER_API ShowMacros(Word bVerbose)
{
	if (bVerbose) {
		Message("NULL is defined as \"" BURGER_MACRO_TO_STRING(NULL) "\"");
		Message("TRUE is defined as \"" BURGER_MACRO_TO_STRING(TRUE) "\"");
		Message("FALSE is defined as \"" BURGER_MACRO_TO_STRING(FALSE) "\"");

		Message("BURGER_ALIGN is defined as \"" BURGER_MACRO_TO_STRING(BURGER_ALIGN(x,s)) "\"");
		Message("BURGER_PREALIGN is defined as \"" BURGER_MACRO_TO_STRING(BURGER_PREALIGN(s)) "\"");
		Message("BURGER_POSTALIGN is defined as \"" BURGER_MACRO_TO_STRING(BURGER_POSTALIGN(s)) "\"");

		Message("BURGER_API is defined as \"" BURGER_MACRO_TO_STRING(BURGER_API) "\"");
		Message("BURGER_ANSIAPI is defined as \"" BURGER_MACRO_TO_STRING(BURGER_ANSIAPI) "\"");
		Message("BURGER_INLINE is defined as \"" BURGER_MACRO_TO_STRING(BURGER_INLINE) "\"");
		Message("BURGER_DECLSPECNAKED is defined as \"" BURGER_MACRO_TO_STRING(BURGER_DECLSPECNAKED) "\"");
		Message("BURGER_ASM is defined as \"" BURGER_MACRO_TO_STRING(BURGER_ASM) "\"");
		Message("BURGER_MAXWORDPTR is defined as \"" BURGER_MACRO_TO_STRING(BURGER_MAXWORDPTR) "\"");
		Message("BURGER_MININT is defined as \"" BURGER_MACRO_TO_STRING(BURGER_MININT) "\"");
		Message("BURGER_MAXINT is defined as \"" BURGER_MACRO_TO_STRING(BURGER_MAXINT) "\"");
		Message("BURGER_MAXUINT is defined as \"" BURGER_MACRO_TO_STRING(BURGER_MAXUINT) "\"");
		Message("BURGER_MININT64 is defined as \"" BURGER_MACRO_TO_STRING(BURGER_MININT64) "\"");
		Message("BURGER_MAXINT64 is defined as \"" BURGER_MACRO_TO_STRING(BURGER_MAXINT64) "\"");
		Message("BURGER_MAXUINT64 is defined as \"" BURGER_MACRO_TO_STRING(BURGER_MAXUINT64) "\"");

		// 64 bit supported?

#if defined(BURGER_LONGLONG)
		Message("BURGER_LONGLONG is defined as \"" BURGER_MACRO_TO_STRING(BURGER_LONGLONG) "\"");
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

	Test default data chunk sizes

***************************************/

static Word BURGER_API TestDataChunkSize(Word bVerbose)
{
	Word uTest = sizeof(Word8)!=1;
	Word uFailure = uTest;
	ReportFailure("sizeof(Word8) is %u instead of 1.",uTest,static_cast<Word>(sizeof(Word8)));

	uTest = sizeof(Int8)!=1;
	uFailure |= uTest;
	ReportFailure("sizeof(Int8) is %u instead of 1.",uTest,static_cast<Word>(sizeof(Int8)));


	uTest = sizeof(Word16)!=2;
	uFailure |= uTest;
	ReportFailure("sizeof(Word16) is %u instead of 2.",uTest,static_cast<Word>(sizeof(Word16)));
	
	uTest = sizeof(Int16)!=2;
	uFailure |= uTest;
	ReportFailure("sizeof(Int16) is %u instead of 2.",uTest,static_cast<Word>(sizeof(Int16)));
	

	uTest = sizeof(Word32)!=4;
	uFailure |= uTest;
	ReportFailure("sizeof(Word32) is %u instead of 4.",uTest,static_cast<Word>(sizeof(Word32)));
	
	uTest = sizeof(Int32)!=4;
	uFailure |= uTest;
	ReportFailure("sizeof(Int32) is %u instead of 4.",uTest,static_cast<Word>(sizeof(Int32)));


	uTest = sizeof(Word64)!=8;
	uFailure |= uTest;
	ReportFailure("sizeof(Word64) is %u instead of 8.",uTest,static_cast<Word>(sizeof(Word64)));
	
	uTest = sizeof(Int64)!=8;
	uFailure |= uTest;
	ReportFailure("sizeof(Int64) is %u instead of 8.",uTest,static_cast<Word>(sizeof(Int64)));
	

	uTest = sizeof(void *)!=sizeof(WordPtr);
	uFailure |= uTest;
	ReportFailure("sizeof(void *) is %u instead of sizeof(WordPtr).",uTest,static_cast<Word>(sizeof(void *)));
	

	uTest = sizeof(Bool)!=1;
	uFailure |= uTest;
	ReportFailure("sizeof(Bool) is %u instead of 1.",uTest,static_cast<Word>(sizeof(Bool)));
	
	uTest = sizeof(Frac32)!=4;
	uFailure |= uTest;
	ReportFailure("sizeof(Frac32) is %u instead of 4.",uTest,static_cast<Word>(sizeof(Frac32)));
	
	uTest = sizeof(Fixed32)!=4;
	uFailure |= uTest;
	ReportFailure("sizeof(Fixed32) is %u instead of 4.",uTest,static_cast<Word>(sizeof(Fixed32)));
	
	uTest = sizeof(Word)!=4;
	uFailure |= uTest;
	ReportFailure("sizeof(Word) is %u instead of 4.",uTest,static_cast<Word>(sizeof(Word)));
	
	uTest = sizeof(Int)!=4;
	uFailure |= uTest;
	ReportFailure("sizeof(Int) is %u instead of 4.",uTest,static_cast<Word>(sizeof(Int)));
	
	uTest = sizeof(Vector_128)!=16;
	uFailure |= uTest;
	ReportFailure("sizeof(Vector_128) is %u instead of 16.",uTest,static_cast<Word>(sizeof(Vector_128)));
	

	//
	// Test BURGER_LONGIS64BIT
	//

#if defined(BURGER_LONGIS64BIT)
	uTest = sizeof(long)!=8;
	uFailure |= uTest;
	ReportFailure("sizeof(long) is %u instead of 8 because BURGER_LONGIS64BIT was defined.",uTest,static_cast<Word>(sizeof(long)));
#else
	uTest = sizeof(long)!=4;
	uFailure |= uTest;
	ReportFailure("sizeof(long) is %u instead of 4 because BURGER_LONGIS64BIT was not defined.",uTest,static_cast<Word>(sizeof(long)));
#endif

	uTest = sizeof(long long)!=8;
	uFailure |= uTest;
	ReportFailure("sizeof(long long) is %u instead of 8.",uTest,static_cast<Word>(sizeof(long long)));

	if (bVerbose && !uFailure) {
		Message("Data chunk size test passed.");
	}
	return uFailure;
}



/***************************************

	Test structure alignment

***************************************/

static Word BURGER_API TestStructureAlignment(Word bVerbose)
{

	//
	// Test for native entry alignment of structure elements
	//

	struct NativeAlign_t {
		Int8 m_0;					// Offset 0
		Int16 m_2;					// Offset 2 (Align to short)
		Int8 m_4;					// Offset 4
		Int32 m_8;					// Offset 8 (Align to int32)
		Int8 m_12;					// Offset 12
		Int64 m_16;					// Offset 16 (Align to int64 or int32) 
		Int8 m_24;					// Offset 24
		Int64 m_32;					// Offset 32 (Align to int64 only)
		Int8 m_40;					// Offset 40
		float m_44;					// Offset 44 (Align to int32)
		Int8 m_48;					// Offset 48
		double m_56;				// Offset 56 (Align to int64 only)
	};

	Word uTest = BURGER_OFFSETOF(NativeAlign_t,m_0)!=0;
	Word uFailure = uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_0) is %u instead of 0.",uTest,static_cast<Word>(BURGER_OFFSETOF(NativeAlign_t,m_0)));

	uTest = BURGER_OFFSETOF(NativeAlign_t,m_2)!=2;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_2) is %u instead of 2.",uTest,static_cast<Word>(BURGER_OFFSETOF(NativeAlign_t,m_2)));

	uTest = BURGER_OFFSETOF(NativeAlign_t,m_4)!=4;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_4) is %u instead of 4.",uTest,static_cast<Word>(BURGER_OFFSETOF(NativeAlign_t,m_4)));

	uTest = BURGER_OFFSETOF(NativeAlign_t,m_8)!=8;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_8) is %u instead of 8.",uTest,static_cast<Word>(BURGER_OFFSETOF(NativeAlign_t,m_8)));

	uTest = BURGER_OFFSETOF(NativeAlign_t,m_12)!=12;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_12) is %u instead of 12.",uTest,static_cast<Word>(BURGER_OFFSETOF(NativeAlign_t,m_12)));

	uTest = BURGER_OFFSETOF(NativeAlign_t,m_16)!=16;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_16) is %u instead of 16.",uTest,static_cast<Word>(BURGER_OFFSETOF(NativeAlign_t,m_16)));

	uTest = BURGER_OFFSETOF(NativeAlign_t,m_24)!=24;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_24) is %u instead of 24.",uTest,static_cast<Word>(BURGER_OFFSETOF(NativeAlign_t,m_24)));

	uTest = BURGER_OFFSETOF(NativeAlign_t,m_32)!=32;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_32) is %u instead of 32.",uTest,static_cast<Word>(BURGER_OFFSETOF(NativeAlign_t,m_32)));

	uTest = BURGER_OFFSETOF(NativeAlign_t,m_40)!=40;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_40) is %u instead of 40.",uTest,static_cast<Word>(BURGER_OFFSETOF(NativeAlign_t,m_40)));

	uTest = BURGER_OFFSETOF(NativeAlign_t,m_44)!=44;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_44) is %u instead of 44.",uTest,static_cast<Word>(BURGER_OFFSETOF(NativeAlign_t,m_44)));

	uTest = BURGER_OFFSETOF(NativeAlign_t,m_48)!=48;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_48) is %u instead of 48.",uTest,static_cast<Word>(BURGER_OFFSETOF(NativeAlign_t,m_48)));

	uTest = BURGER_OFFSETOF(NativeAlign_t,m_56)!=56;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,56) is %u instead of 56.",uTest,static_cast<Word>(BURGER_OFFSETOF(NativeAlign_t,m_56)));

	//
	// Test for pointer alignment
	// Alignment is to match the value found in sizeof(WordPtr)
	// since pointers could be 4 or 8 bytes in size depending on CPU / platform
	//

	struct PointerAlign_t {
		Int8 m_0;					// Offset 0
		Int8 *m_ptr;				// Offset 4 or 8 (Align to WordPtr)
	};

	uTest = BURGER_OFFSETOF(PointerAlign_t,m_0)!=0;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(PointerAlign_t,m_0) is %u instead of 0.",uTest,static_cast<Word>(BURGER_OFFSETOF(PointerAlign_t,m_0)));

	uTest = BURGER_OFFSETOF(PointerAlign_t,m_ptr)!=sizeof(WordPtr);
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(PointerAlign_t,m_ptr) is %u instead of %u.",uTest,static_cast<Word>(BURGER_OFFSETOF(PointerAlign_t,m_ptr)),static_cast<Word>(sizeof(WordPtr)));

	//
	// Test the BURGER_ALIGN() macro for alignment overrides
	//

	struct ElementAlign_t {
		Int8 m_0;					// Offset 0
		Int8 BURGER_ALIGN(m_2,2);	// Offset 2
		Int8 m_3;					// Offset 3
		Int8 m_4;					// Offset 4
		Int8 BURGER_ALIGN(m_8,4);	// Offset 8
		Int8 m_9;					// Offset 9
		Int8 BURGER_ALIGN(m_16,8);	// Offset 16
		Int8 m_17;					// Offset 17
		Int8 BURGER_ALIGN(m_32,16);	// Offset 32
	};

	uTest = BURGER_OFFSETOF(ElementAlign_t,m_0)!=0;
	Word uAlignFailure = uTest;
	ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_0) is %u instead of 0.",uTest,static_cast<Word>(BURGER_OFFSETOF(ElementAlign_t,m_0)));

	uTest = BURGER_OFFSETOF(ElementAlign_t,m_2)!=2;
	uAlignFailure |= uTest;
	ReportFailure("BURGER_ALIGN(m_2,2) is %u instead of 2.",uTest,static_cast<Word>(BURGER_OFFSETOF(ElementAlign_t,m_2)));

	uTest = BURGER_OFFSETOF(ElementAlign_t,m_3)!=3;
	uAlignFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_3) is %u instead of 3.",uTest,static_cast<Word>(BURGER_OFFSETOF(ElementAlign_t,m_3)));

	uTest = BURGER_OFFSETOF(ElementAlign_t,m_4)!=4;
	uAlignFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_4) is %u instead of 4.",uTest,static_cast<Word>(BURGER_OFFSETOF(ElementAlign_t,m_4)));

	uTest = BURGER_OFFSETOF(ElementAlign_t,m_8)!=8;
	uAlignFailure |= uTest;
	ReportFailure("BURGER_ALIGN(m_8,4) is %u instead of 8.",uTest,static_cast<Word>(BURGER_OFFSETOF(ElementAlign_t,m_8)));

	uTest = BURGER_OFFSETOF(ElementAlign_t,m_9)!=9;
	uAlignFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_9) is %u instead of 9.",uTest,static_cast<Word>(BURGER_OFFSETOF(ElementAlign_t,m_9)));

	uTest = BURGER_OFFSETOF(ElementAlign_t,m_16)!=16;
	uAlignFailure |= uTest;
	ReportFailure("BURGER_ALIGN(m_16,8) is %u instead of 16.",uTest,static_cast<Word>(BURGER_OFFSETOF(ElementAlign_t,m_16)));

	uTest = BURGER_OFFSETOF(ElementAlign_t,m_17)!=17;
	uAlignFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_17) is %u instead of 17.",uTest,static_cast<Word>(BURGER_OFFSETOF(ElementAlign_t,m_17)));

	uTest = BURGER_OFFSETOF(ElementAlign_t,m_32)!=32;
	uAlignFailure |= uTest;
	ReportFailure("BURGER_ALIGN(m_32,16) is %u instead of 32.",uTest,static_cast<Word>(BURGER_OFFSETOF(ElementAlign_t,m_32)));

	if (uAlignFailure) {
		Message("FAILED BURGER_ALIGN() macro is broken on this compiler.");
	}
	uFailure |= uAlignFailure;

	//
	// Wrap up
	//
	if (!uFailure && bVerbose) {
		Message("Structure alignment test passed");
	}
	return uFailure;
}

/***************************************

	Show CPU features

***************************************/

static void BURGER_API ShowCPUFeatures(Word bVerbose)
{
	Word uTest = Burger::IsCPUIDPresent();
	if (bVerbose) {
		Message("Burger::IsCPUIDPresent() = %u",uTest);
	}

	if (uTest) {
		if (bVerbose) {
			Burger::CPUID_t MyID;
			Burger::CPUID(&MyID);
			Message("CPUID_t.m_uHighestCPUID = 0x%08X",MyID.m_uHighestCPUID);
			Message("CPUID_t.m_uHighestCPUIDExtended = 0x%08X",MyID.m_uHighestCPUIDExtended);
			Message("CPUID_t.m_uHighestCPUIDHyperVisor = 0x%08X",MyID.m_uHighestCPUIDHyperVisor);
			Message("CPUID_t.m_uModel = 0x%08X",MyID.m_uModel);
			Message("CPUID_t.m_uBrand = 0x%08X",MyID.m_uBrand);
			Message("CPUID_t.m_uCPUID1ECX = 0x%08X",MyID.m_uCPUID1ECX);
			Message("CPUID_t.m_uCPUID1EDX = 0x%08X",MyID.m_uCPUID1EDX);
			Message("CPUID_t.m_uCPUID80000001ECX = 0x%08X",MyID.m_uCPUID80000001ECX);
			Message("CPUID_t.m_uCPUID80000001EDX = 0x%08X",MyID.m_uCPUID80000001EDX);
			Message("CPUID_t.m_uCPUID7EBX = 0x%08X",MyID.m_uCPUID7EBX);
			Message("CPUID_t.m_uCPUID7ECX = 0x%08X",MyID.m_uCPUID7ECX);
			Message("CPUID_t.m_uCPUID7EDX = 0x%08X",MyID.m_uCPUID7EDX);
			Message("CPUID_t.m_uCPUType = %u",static_cast<Word>(MyID.m_uCPUType));
			Message("CPUID_t.m_CPUName = %s",MyID.m_CPUName);
			Message("CPUID_t.m_BrandName = %s",MyID.m_BrandName);
			Message("CPUID_t.m_HypervisorName = %s",MyID.m_HypervisorName);
			Message("CPUID_t.m_HypervisorSignature = %s",MyID.m_HypervisorSignature);

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
		}
	} else {
		
		// CPUID not available on a 32 bit CPU is not a failure,
		// but it is a warning that your CPU is pretty old.

#if defined(BURGER_INTELARCHITECTURE)
		Message("Burger::CPUID() is not available");
#endif
	}

	//
	// Handle AltiVec (MacOS / MacOSX) PowerPC
	//

	if (bVerbose) {
		uTest = Burger::HasAltiVec();
		Message("Burger::HasAltiVec() = %u",uTest);
	}
}


/***************************************

	Show Platform features

***************************************/

static void BURGER_API ShowPlatformFeatures(Word bVerbose)
{
	Word uTest = 0;

	BURGER_UNUSED(bVerbose);
	BURGER_UNUSED(uTest);

	//
	// Handle Windows features
	//

#if defined(BURGER_WINDOWS)
	if (bVerbose) {

		// Test operating system versions

		uTest = Burger::Globals::IsWin95orWin98();
		Message("Burger::Globals::IsWin95orWin98() = %u",uTest);

		uTest = Burger::Globals::IsWinXPOrGreater();
		Message("Burger::Globals::IsWinXPOrGreater() = %u",uTest);

		uTest = Burger::Globals::IsVistaOrGreater();
		Message("Burger::Globals::IsVistaOrGreater() = %u",uTest);

		uTest = Burger::Globals::IsWin7OrGreater();
		Message("Burger::Globals::IsWin7OrGreater() = %u",uTest);

		uTest = Burger::Globals::IsWin8OrGreater();
		Message("Burger::Globals::IsWin8OrGreater() = %u",uTest);

		uTest = Burger::Globals::IsWin10OrGreater();
		Message("Burger::Globals::IsWin10OrGreater() = %u",uTest);

		// Is 32 bit code running in 64 bit windows?
		uTest = Burger::Globals::IsWindows64Bit();
		Message("Burger::Globals::IsWindows64Bit() = %u",uTest);

		// DirectX versions
		uTest = Burger::Globals::GetDirectXVersion();
		Message("Burger::Globals::GetDirectXVersion() = 0x%04X",uTest);

		uTest = Burger::Globals::IsDirectInputPresent();
		Message("Burger::Globals::IsDirectInputPresent() = %u",uTest);
		uTest = Burger::Globals::IsDirectInput8Present();
		Message("Burger::Globals::IsDirectInput8Present() = %u",uTest);
		uTest = Burger::Globals::IsXInputPresent();
		Message("Burger::Globals::IsXInputPresent() = %u",uTest);

		uTest = Burger::Globals::IsDirectDrawPresent();
		Message("Burger::Globals::IsDirectDrawPresent() = %u",uTest);

		uTest = Burger::Globals::IsD3D9Present();
		Message("Burger::Globals::IsD3D9Present() = %u",uTest);

		uTest = Burger::Globals::IsDirectSoundPresent();
		Message("Burger::Globals::IsDirectSoundPresent() = %u",uTest);

		uTest = Burger::Globals::IsDirectPlayPresent();
		Message("Burger::Globals::IsDirectPlayPresent() = %u",uTest);

		uTest = Burger::Globals::IsXAudio2Present();
		Message("Burger::Globals::IsXAudio2Present() = %u",uTest);
		
		char QTBuffer[300];

		uTest = Burger::Globals::GetPathToQuickTimeFolder(QTBuffer,sizeof(QTBuffer),NULL);
		Message("Burger::Globals::GetPathToQuickTimeFolder() = %u, %s",uTest,QTBuffer);
		uTest = Burger::Globals::GetQTSystemDirectoryA(QTBuffer,sizeof(QTBuffer));
		Message("Burger::Globals::GetQTSystemDirectoryA() = %u, %s",uTest,QTBuffer);
		uTest = Burger::Globals::GetQTApplicationDirectoryA(QTBuffer,sizeof(QTBuffer));
		Message("Burger::Globals::GetQTApplicationDirectoryA() = %u, %s",uTest,QTBuffer);
		uTest = Burger::Globals::GetQTExtensionDirectoryA(QTBuffer,sizeof(QTBuffer));
		Message("Burger::Globals::GetQTExtensionDirectoryA() = %u, %s",uTest,QTBuffer);
		uTest = Burger::Globals::GetQTComponentDirectoryA(QTBuffer,sizeof(QTBuffer));
		Message("Burger::Globals::GetQTComponentDirectoryA() = %u, %s",uTest,QTBuffer);
	}

#endif

#if defined(BURGER_MACOS) || defined(BURGER_WINDOWS)
	if (bVerbose) {
		uTest = Burger::Globals::GetQuickTimeVersion();
		Message("Burger::Globals::GetQuickTimeVersion() = 0x%04X",uTest);
	}
#endif


	//
	// Handle MacOS Version
	//

#if defined(BURGER_MACOS)
	if (bVerbose) {
		uTest = Burger::Globals::GetMacOSVersion();
		Message("Burger::Globals::GetMacOSVersion() = %04X",uTest);
	}
#endif

	//
	// Mac OS functions

#if defined(BURGER_MAC)
	// _MemoryDispatch = 0xA05C
	if (bVerbose) {
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

		uTest = Burger::Globals::GetSoundManagerVersion();
		Message("Burger::Globals::GetSoundManagerVersion() = %04X",uTest);
	}
#endif
}

/***************************************

	Perform the tests for the macros and compiler
	settings

***************************************/

int BURGER_API TestBrtypes(Word bVerbose)
{
	Word uFailure = 0;

	if (bVerbose) {
		Message("Testing macros");
	}
	ShowMacros(bVerbose);

	uFailure = TestEndianMacros();
	uFailure |= TestDebugMacros(bVerbose);
	uFailure |= TestCompilerMacros(bVerbose);
	uFailure |= TestCPUMacros(bVerbose);
	uFailure |= TestPlatformMacros(bVerbose);
	uFailure |= TestDataChunkSize(bVerbose);
	uFailure |= TestStructureAlignment(bVerbose);

	// Print messages about features found on the platform

	ShowCPUFeatures(bVerbose);
	ShowPlatformFeatures(bVerbose);

	return static_cast<int>(uFailure);
}