/***************************************

	Determine which compiler is being used and
	create standardized typedefs and macros
	so generic code can be created cross platform

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrtypes.h"
#include <stdio.h>

#if defined(BURGER_WATCOM)
#pragma disable_message(13)		// Disable unreachable code warning
#pragma disable_message(367)	// Disable conditional expression is constant warning
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

struct aligntest {
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


int BURGER_API TestBrtypes(void)
{
	int iError = 0;

	printf("Testing macros\n");

	//
	// Endian macros
	//

#if defined(BURGER_BIGENDIAN)
	printf("BURGER_BIGENDIAN is defined\n");
#endif
#if defined(BURGER_LITTLEENDIAN)
	printf("BURGER_LITTLEENDIAN is defined\n");
#endif

#if defined(BURGER_LITTLEENDIAN) && defined(BURGER_BIGENDIAN)
	printf("Error: Both BURGER_LITTLEENDIAN and BURGER_BIGENDIAN are defined!\n");
	iError = 10;
#endif

#if !defined(BURGER_LITTLEENDIAN) && !defined(BURGER_BIGENDIAN)
	printf("Error: Neither BURGER_LITTLEENDIAN or BURGER_BIGENDIAN are defined!\n");
	iError = 10;
#endif

#if defined(NDEBUG)
	printf("NDEBUG is defined\n");
#endif
#if defined(_DEBUG)
	printf("_DEBUG is defined\n");
#endif
#if defined(NDEBUG) && defined(_DEBUG)
	printf("Error: Both NDEBUG and _DEBUG are defined!\n");
	iError = 10;
#endif
#if !defined(NDEBUG) && !defined(_DEBUG)
	printf("Error: Neither NDEBUG or _DEBUG are defined!\n");
	iError = 10;
#endif

	//
	// Compiler macros
	//

	int iTest = 0;
#if defined(BURGER_WATCOM)
	printf("BURGER_WATCOM is defined\n");
	++iTest;
#endif
#if defined(BURGER_MRC)
	printf("BURGER_WATCOM is defined\n");
	++iTest;
#endif
#if defined(BURGER_APPLESC)
	printf("BURGER_WATCOM is defined\n");
	++iTest;
#endif
#if defined(BURGER_INTELCOMPILER)
	printf("BURGER_INTELCOMPILER is defined\n");
	++iTest;	
#endif
#if defined(BURGER_SNSYSTEMS)
	printf("BURGER_SNSYSTEMS is defined\n");
	++iTest;
#endif
#if defined(BURGER_METROWERKS)
	printf("BURGER_METROWERKS is defined\n");
	++iTest;
#endif
#if defined(BURGER_MSVC)
	printf("BURGER_MSVC is defined\n");
	++iTest;
#endif
#if defined(BURGER_GNUC)
	printf("BURGER_GNUC is defined\n");
	++iTest;
#endif
	if (iTest!=1) {
		printf("Multiple compilers have been defined!\n");
		iError = 10;
	}
	if (iTest==0) {
		printf("Unknown compiler detected!\n");
		iError = 10;
	}
	
	// CPU macros
	iTest = 0;
#if defined(BURGER_68K)
	printf("BURGER_68K is defined\n");
	++iTest;
#endif
#if defined(BURGER_POWERPC)
	printf("BURGER_POWERPC is defined\n");
	++iTest;
#endif
#if defined(BURGER_POWERPC64)
	printf("BURGER_POWERPC64 is defined\n");
	++iTest;
#endif
#if defined(BURGER_ARM)
	printf("BURGER_ARM is defined\n");
	++iTest;
#endif
#if defined(BURGER_X86)
	printf("BURGER_X86 is defined\n");
	++iTest;
#endif
#if defined(BURGER_AMD64)
	printf("BURGER_AMD64 is defined\n");
	++iTest;
#endif
#if defined(BURGER_MIPS)
	printf("BURGER_MIPS is defined\n");
	++iTest;
#endif
#if defined(BURGER_GEKKO)
	printf("BURGER_GEKKO is defined\n");
	++iTest;
#endif
#if defined(BURGER_64BITCPU)
	printf("BURGER_64BITCPU is defined\n");
#endif
	if (iTest!=1) {
		printf("Multiple CPUs have been defined!\n");
		iError = 10;
	}
	if (iTest==0) {
		printf("Unknown CPU detected!\n");
		iError = 10;
	}

	// Platform macros
	
	iTest = 0;
#if defined(BURGER_MSDOS)
	printf("BURGER_MSDOS is defined\n");
	++iTest;
#endif
#if defined(__DOS4G__)
	printf("__DOS4G__ is defined\n");
#endif
#if defined(__X32__)
	printf("__X32__ is defined\n");
#endif

#if defined(BURGER_WIN32)
	printf("BURGER_WIN32 is defined\n");
	++iTest;
#endif
#if defined(BURGER_WIN64)
	printf("BURGER_WIN64 is defined\n");
	++iTest;
#endif
#if defined(BURGER_WINDOWS)
	printf("BURGER_WINDOWS is defined\n");
#endif

#if defined(BURGER_BEOS)
	printf("BURGER_BEOS is defined\n");
	++iTest;
#endif

#if defined(BURGER_MAC)
	printf("BURGER_MAC is defined\n");
	++iTest;
#endif
#if defined(BURGER_MACOSX)
	printf("BURGER_MACOSX is defined\n");
	++iTest;
#endif
#if defined(BURGER_MACOS)
	printf("BURGER_MACOS is defined\n");
#endif
#if defined(BURGER_MACCLASSIC)
	printf("BURGER_MACCLASSIC is defined\n");
#endif
#if defined(BURGER_MACCARBON)
	printf("BURGER_MACCARBON is defined\n");
#endif
#if defined(TARGET_API_MAC_CARBON)
	printf("TARGET_API_MAC_CARBON is defined\n");
#endif
#if defined(BURGER_CFM)
	printf("BURGER_CFM is defined\n");
#endif


#if defined(BURGER_IOS)
	printf("BURGER_IOS is defined\n");
	++iTest;
#endif
#if defined(BURGER_ANDROID)
	printf("BURGER_ANDROID is defined\n");
	++iTest;
#endif
#if defined(BURGER_NGAGE)
	printf("BURGER_NGAGE is defined\n");
	++iTest;
#endif
#if defined(BURGER_SYMBIAN)
	printf("BURGER_SYMBIAN is defined\n");
	++iTest;
#endif


#if defined(BURGER_XBOX)
	printf("BURGER_XBOX is defined\n");
	++iTest;
#endif
#if defined(BURGER_XBOX360)
	printf("BURGER_XBOX360 is defined\n");
	++iTest;
#endif

#if defined(BURGER_PS2)
	printf("BURGER_PS2 is defined\n");
	++iTest;
#endif
#if defined(BURGER_PS3)
	printf("BURGER_PS3 is defined\n");
	++iTest;
#endif
#if defined(BURGER_PS4)
	printf("BURGER_PS4 is defined\n");
	++iTest;
#endif

#if defined(BURGER_GAMECUBE)
	printf("BURGER_GAMECUBE is defined\n");
	++iTest;
#endif
#if defined(BURGER_WII)
	printf("BURGER_WII is defined\n");
	++iTest;
#endif
#if defined(BURGER_DS)
	printf("BURGER_DS is defined\n");
	++iTest;
#endif
	if (iTest!=1) {
		printf("Multiple Platforms have been defined!\n");
		iError = 10;
	}
	if (iTest==0) {
		printf("Unknown Platform detected!\n");
		iError = 10;
	}

	// Display standard macros

	printf("BURGER_ALIGN is defined at \"" BURGER_MACRO_TO_STRING(BURGER_ALIGN(x,s)) "\"\n");
	printf("BURGER_PREALIGN is defined at \"" BURGER_MACRO_TO_STRING(BURGER_PREALIGN(s)) "\"\n");
	printf("BURGER_POSTALIGN is defined at \"" BURGER_MACRO_TO_STRING(BURGER_POSTALIGN(s)) "\"\n");

	printf("BURGER_API is defined at \"" BURGER_MACRO_TO_STRING(BURGER_API) "\"\n");
	printf("BURGER_ANSIAPI is defined at \"" BURGER_MACRO_TO_STRING(BURGER_ANSIAPI) "\"\n");
	printf("BURGER_INLINE is defined at \"" BURGER_MACRO_TO_STRING(BURGER_INLINE) "\"\n");
	printf("BURGER_DECLSPECNAKED is defined at \"" BURGER_MACRO_TO_STRING(BURGER_DECLSPECNAKED) "\"\n");
	printf("BURGER_ASM is defined at \"" BURGER_MACRO_TO_STRING(BURGER_ASM) "\"\n");
	printf("NULL is defined at \"" BURGER_MACRO_TO_STRING(NULL) "\"\n");
	printf("TRUE is defined at \"" BURGER_MACRO_TO_STRING(TRUE) "\"\n");
	printf("FALSE is defined at \"" BURGER_MACRO_TO_STRING(FALSE) "\"\n");
	printf("BURGER_MAXWORDPTR is defined at \"" BURGER_MACRO_TO_STRING(BURGER_MAXWORDPTR) "\"\n");
	printf("BURGER_MAXINT is defined at \"" BURGER_MACRO_TO_STRING(BURGER_MAXINT) "\"\n");
	printf("BURGER_MAXUINT is defined at \"" BURGER_MACRO_TO_STRING(BURGER_MAXUINT) "\"\n");

// 64 bit supported?

#if defined(BURGER_LONGLONG)
	printf("BURGER_LONGLONG is defined at \"" BURGER_MACRO_TO_STRING(BURGER_LONGLONG) "\"\n");
#endif
	
	// Display optional macros

#if defined(BURGER_STRUCT_PACKPUSH)
	printf("BURGER_STRUCT_PACKPUSH is defined\n");
#endif
#if defined(BURGER_STRUCT_ALIGN)
	printf("BURGER_STRUCT_ALIGN is defined\n");
#endif
#if defined(BURGER_STRUCT_PACK)
	printf("BURGER_STRUCT_PACK is defined\n");
#endif
#if defined(BURGER_FASTCALLENABLED)
	printf("BURGER_FASTCALLENABLED is defined\n");
#endif
#if defined(BURGER_NO_USING_NAMESPACE)
	printf("BURGER_NO_USING_NAMESPACE is defined\n");
#endif

	// Test structure alignment

	if ((BURGER_OFFSETOF(aligntest,m_0)!=0) ||
		(BURGER_OFFSETOF(aligntest,m_16)!=16) ||
		(BURGER_OFFSETOF(aligntest,m_17)!=17) ||
		(BURGER_OFFSETOF(aligntest,m_20)!=20) ||
		(BURGER_OFFSETOF(aligntest,m_21)!=21) ||
		(BURGER_OFFSETOF(aligntest,m_32)!=32) ||
		(BURGER_OFFSETOF(aligntest,m_33)!=33) ||
		(BURGER_OFFSETOF(aligntest,m_40)!=40) ||
		(BURGER_OFFSETOF(aligntest,m_48)!=48) ||
		(BURGER_OFFSETOF(aligntest,m_50)!=50) ||
		(BURGER_OFFSETOF(aligntest,m_52)!=52) ||
		(BURGER_OFFSETOF(aligntest,m_56)!=56)) {
		printf("Offset m_0 = %u\n",static_cast<Word>(BURGER_OFFSETOF(aligntest,m_0)));
		printf("Offset m_16 = %u (align 16)\n",static_cast<Word>(BURGER_OFFSETOF(aligntest,m_16)));
		printf("Offset m_17 = %u\n",static_cast<Word>(BURGER_OFFSETOF(aligntest,m_17)));
		printf("Offset m_20 = %u (align 4)\n",static_cast<Word>(BURGER_OFFSETOF(aligntest,m_20)));
		printf("Offset m_21 = %u\n",static_cast<Word>(BURGER_OFFSETOF(aligntest,m_21)));
		printf("Offset m_32 = %u (align 16)\n",static_cast<Word>(BURGER_OFFSETOF(aligntest,m_32)));
		printf("Offset m_33 = %u\n",static_cast<Word>(BURGER_OFFSETOF(aligntest,m_33)));
		printf("Offset m_40 = %u (Int64)\n",static_cast<Word>(BURGER_OFFSETOF(aligntest,m_40)));
		printf("Offset m_48 = %u\n",static_cast<Word>(BURGER_OFFSETOF(aligntest,m_48)));
		printf("Offset m_50 = %u (Int16)\n",static_cast<Word>(BURGER_OFFSETOF(aligntest,m_50)));
		printf("Offset m_52 = %u\n",static_cast<Word>(BURGER_OFFSETOF(aligntest,m_52)));
		printf("Offset m_56 = %u (Int32)\n",static_cast<Word>(BURGER_OFFSETOF(aligntest,m_56)));
		iError = 10;
	} else {
		printf("Structure alignment test passed\n");
	}
	return iError;
}