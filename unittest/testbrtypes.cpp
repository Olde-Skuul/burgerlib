/***************************************

	Determine which compiler is being used and create standardized typedefs and
	macros so generic code can be created cross platform

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrtypes.h"
#include "bralgorithm.h"
#include "bratomic.h"
#include "brfloatingpoint.h"
#include "brglobals.h"
#include "common.h"

#if defined(BURGER_UNIX)
#include "brunixtypes.h"
#endif

#if defined(BURGER_MAC)
#include "brnetmanager.h"
#include "mac_gamma.h"
#include "mac_kernel.h"
#include "mac_quickdraw.h"
#include "mac_sprockets.h"
#include "mac_version.h"
#endif

#if defined(BURGER_WINDOWS)
#include "win_platformshims.h"
#include "win_registry.h"
#include "win_version.h"
#endif

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

#if defined(BURGER_WATCOM)
// No reference to symbol in structure
#pragma warning 14 9
#endif

#if defined(BURGER_MSVC)
// Disable not enough actual parameters for macro
#pragma warning(disable : 4003)
// Disable structure was padded due to __declspec(align())
#pragma warning(disable : 4324)
// VC 2005 or higher
#if (BURGER_MSVC >= 140000000)
// Disable Potential comparison of a comparison of a constant with another
// constant
#pragma warning(disable : 6326)
#endif
#endif

/***************************************

	Test default data chunk sizes

***************************************/

static uint_t BURGER_API TestDataChunkSize(uint_t uVerbose) BURGER_NOEXCEPT
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
	// Test BURGER_SIZEOF_LONG
	//

#if BURGER_SIZEOF_LONG == 8
	uTest = sizeof(long) != 8;
	uFailure |= uTest;
	ReportFailure(
		"sizeof(long) is %u instead of 8 because BURGER_SIZEOF_LONG was defined.",
		uTest, static_cast<uint_t>(sizeof(long)));
#else
	uTest = sizeof(long) != 4;
	uFailure |= uTest;
	ReportFailure(
		"sizeof(long) is %u instead of 4 because BURGER_SIZEOF_LONG was not defined.",
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

static uint_t BURGER_API TestStructureAlignment(uint_t uVerbose) BURGER_NOEXCEPT
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

	uint_t uAlignMark = 0;
	uint_t uTest = BURGER_OFFSETOF(NativeAlign_t, m_0) != uAlignMark;
	uint_t uFailure = uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_0) is %u instead of %u.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_0)),
		uAlignMark);

	uAlignMark += 2;
	uTest = BURGER_OFFSETOF(NativeAlign_t, m_2) != uAlignMark;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_2) is %u instead of %u.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_2)),
		uAlignMark);

	uAlignMark += 2;
	uTest = BURGER_OFFSETOF(NativeAlign_t, m_4) != uAlignMark;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_4) is %u instead of %u.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_4)),
		uAlignMark);

	uAlignMark += 4;
	uTest = BURGER_OFFSETOF(NativeAlign_t, m_8) != uAlignMark;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_8) is %u instead of %u.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_8)),
		uAlignMark);

	uAlignMark += 4;
	uTest = BURGER_OFFSETOF(NativeAlign_t, m_12) != uAlignMark;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_12) is %u instead of %u.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_12)),
		uAlignMark);

	uAlignMark += 4;
	uTest = BURGER_OFFSETOF(NativeAlign_t, m_16) != uAlignMark;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_16) is %u instead of %u.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_16)),
		uAlignMark);

	uAlignMark += 8;
	uTest = BURGER_OFFSETOF(NativeAlign_t, m_24) != uAlignMark;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_24) is %u instead of %u.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_24)),
		uAlignMark);

	uAlignMark += Burger::alignment_of<int64_t>::value;
	uTest = BURGER_OFFSETOF(NativeAlign_t, m_32) != uAlignMark;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_32) is %u instead of %u.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_32)),
		uAlignMark);

	uAlignMark += 8;
	uTest = BURGER_OFFSETOF(NativeAlign_t, m_40) != uAlignMark;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_40) is %u instead of %u.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_40)),
		uAlignMark);

	uAlignMark += 4;
	uTest = BURGER_OFFSETOF(NativeAlign_t, m_44) != uAlignMark;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_44) is %u instead of %u.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_44)),
		uAlignMark);

	uAlignMark += 4;
	uTest = BURGER_OFFSETOF(NativeAlign_t, m_48) != uAlignMark;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,m_48) is %u instead of %u.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_48)),
		uAlignMark);

	uAlignMark =
		(uAlignMark + 4 +
			static_cast<uint_t>(Burger::alignment_of<double>::value) - 1) &
		~(static_cast<uint_t>(Burger::alignment_of<double>::value) - 1);
	uTest = BURGER_OFFSETOF(NativeAlign_t, m_56) != uAlignMark;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(NativeAlign_t,56) is %u instead of %u.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(NativeAlign_t, m_56)),
		uAlignMark);

	//
	// Test for pointer alignment
	// Alignment is to match the value found in sizeof(uintptr_t)
	// since pointers could be 4 or 8 bytes in size depending on CPU / platform
	//

	struct PointerAlign_t {
		int8_t m_0;    // Offset 0
		int8_t* m_ptr; // Offset 4 or 8 (Align to uintptr_t)
	};

	uTest = BURGER_OFFSETOF(PointerAlign_t, m_0) != 0;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(PointerAlign_t,m_0) is %u instead of 0.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(PointerAlign_t, m_0)));

	uTest = BURGER_OFFSETOF(PointerAlign_t, m_ptr) != sizeof(uintptr_t);
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(PointerAlign_t,m_ptr) is %u instead of %u.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(PointerAlign_t, m_ptr)),
		static_cast<uint_t>(sizeof(uintptr_t)));

#if defined(BURGER_NO_ALIGN)
	Message("Compiler doesn't support per object alignment");

#else
	//
	// Test the BURGER_ALIGN() macro for alignment overrides
	//

	struct ElementAlign_t {
		int8_t m_0;                     // Offset 0
		BURGER_ALIGN(int8_t, m_2, 2);   // Offset 2
		int8_t m_3;                     // Offset 3
		int8_t m_4;                     // Offset 4
		BURGER_ALIGN(int8_t, m_8, 4);   // Offset 8
		int8_t m_9;                     // Offset 9
		BURGER_ALIGN(int8_t, m_16, 8);  // Offset 16
		int8_t m_17;                    // Offset 17
		BURGER_ALIGN(int8_t, m_32, 16); // Offset 32
	};

	uTest = BURGER_OFFSETOF(ElementAlign_t, m_0) != 0;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_0) is %u instead of 0.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_0)));

	uTest = BURGER_OFFSETOF(ElementAlign_t, m_2) != 2;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_2,2) is %u instead of 2.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_2)));

	uTest = BURGER_OFFSETOF(ElementAlign_t, m_3) != 3;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_3) is %u instead of 3.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_3)));

	uTest = BURGER_OFFSETOF(ElementAlign_t, m_4) != 4;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_4) is %u instead of 4.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_4)));

	uTest = BURGER_OFFSETOF(ElementAlign_t, m_8) != 8;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_8,4) is %u instead of 8.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_8)));

	uTest = BURGER_OFFSETOF(ElementAlign_t, m_9) != 9;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_9) is %u instead of 9.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_9)));

	uTest = BURGER_OFFSETOF(ElementAlign_t, m_16) != 16;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_16,8) is %u instead of 16.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_16)));

	uTest = BURGER_OFFSETOF(ElementAlign_t, m_17) != 17;
	uFailure |= uTest;
	ReportFailure("BURGER_OFFSETOF(ElementAlign_t,m_17) is %u instead of 17.",
		uTest, static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_17)));

	uTest = BURGER_OFFSETOF(ElementAlign_t, m_32) != 32;
	uFailure |= uTest;
	ReportFailure(
		"BURGER_OFFSETOF(ElementAlign_t,m_32,16) is %u instead of 32.", uTest,
		static_cast<uint_t>(BURGER_OFFSETOF(ElementAlign_t, m_32)));

#endif

	//
	// Test alignment with template
	//
	uTest = Burger::alignment_of<int>::value != sizeof(int);
	uFailure |= uTest;
	ReportFailure("Burger::alignment_of<int>::value %u instead of 2 or 4.",
		uTest, static_cast<uint_t>(Burger::alignment_of<int>::value));

	uTest = Burger::alignment_of<float>::value != sizeof(float);
	uFailure |= uTest;
	ReportFailure("Burger::alignment_of<float>::value %u instead of 4.", uTest,
		static_cast<uint_t>(Burger::alignment_of<float>::value));

	struct aligndouble_t {
		double a;
		char b;
	};
	uintptr_t uAlign = sizeof(aligndouble_t) - sizeof(double);
	uTest = Burger::alignment_of<double>::value != uAlign;
	uFailure |= uTest;
	ReportFailure("Burger::alignment_of<double>::value %u instead of %u.",
		uTest, static_cast<uint_t>(Burger::alignment_of<double>::value),
		static_cast<uint_t>(uAlign));

	uTest = Burger::alignment_of<void*>::value != sizeof(void*);
	uFailure |= uTest;
	ReportFailure("Burger::alignment_of<void *>::value %u instead of 4 or 8.",
		uTest, static_cast<uint_t>(Burger::alignment_of<void*>::value));

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

static void BURGER_API ShowCPUFeatures(uint_t uVerbose) BURGER_NOEXCEPT
{
	// Test for an FPU
	if (uVerbose & VERBOSE_MSG) {
		Message("Burger::has_68kFPU() = %u", Burger::has_68kFPU());
	}

	// Test CPUID
#if defined(BURGER_INTEL)
	uint_t uCPUIDPresent = Burger::has_CPUID();
	if (uVerbose & VERBOSE_MSG) {
		Message("Burger::has_CPUID() = %u", uCPUIDPresent);
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
			Message("CPUID_t.m_uMXCSR = 0x%08X", MyID.m_uMXCSR);
			Message("CPUID_t.m_uXGETBV = 0x%08X%08X",
				static_cast<uint32_t>(MyID.m_uXGETBV >> 32U),
				static_cast<uint32_t>(MyID.m_uXGETBV));
			Message("CPUID_t.m_uCPUType = %u",
				static_cast<uint_t>(MyID.m_uCPUType));
			Message("CPUID_t.m_CPUName = %s", MyID.m_CPUName);
			Message("CPUID_t.m_BrandName = %s", MyID.m_BrandName);
			Message("CPUID_t.m_HypervisorName = %s", MyID.m_HypervisorName);
			Message("CPUID_t.m_HypervisorSignature = %s",
				MyID.m_HypervisorSignature);

			// Dump the CPU flags detects
			char Detected[256];
			Detected[0] = 0;
			if (MyID.has_RTSC()) {
				Burger::StringConcatenate(Detected, ", RTSC");
			}

			if (MyID.has_CMOV()) {
				Burger::StringConcatenate(Detected, ", CMOV");
			}

			if (MyID.has_MMX()) {
				Burger::StringConcatenate(Detected, ", MMX");
			}
			if (MyID.has_SSE()) {
				Burger::StringConcatenate(Detected, ", SSE");
			}
			if (MyID.has_SSE2()) {
				Burger::StringConcatenate(Detected, ", SSE2");
			}
			if (MyID.has_SSE3()) {
				Burger::StringConcatenate(Detected, ", SSE3");
			}
			if (MyID.has_SSSE3()) {
				Burger::StringConcatenate(Detected, ", SSSE3");
			}
			if (MyID.has_SSE4a()) {
				Burger::StringConcatenate(Detected, ", SSE4a");
			}
			if (MyID.has_SSE41()) {
				Burger::StringConcatenate(Detected, ", SSE41");
			}
			if (MyID.has_SSE42()) {
				Burger::StringConcatenate(Detected, ", SSE42");
			}
			if (MyID.has_MOVBE()) {
				Burger::StringConcatenate(Detected, ", MOVBE");
			}
			if (MyID.has_AES()) {
				Burger::StringConcatenate(Detected, ", AES");
			}
			if (MyID.has_AVX()) {
				Burger::StringConcatenate(Detected, ", AVX");
			}
			if (MyID.has_AVX2()) {
				Burger::StringConcatenate(Detected, ", AVX2");
			}
			if (MyID.has_CMPXCHG16B()) {
				Burger::StringConcatenate(Detected, ", CMPXCHG16B");
			}
			if (MyID.has_F16C()) {
				Burger::StringConcatenate(Detected, ", F16C");
			}
			if (MyID.has_FMA3()) {
				Burger::StringConcatenate(Detected, ", FMA3");
			}
			if (MyID.has_FMA4()) {
				Burger::StringConcatenate(Detected, ", FMA4");
			}
			if (MyID.has_LAHFSAHF()) {
				Burger::StringConcatenate(Detected, ", LAHFSAHF");
			}
			if (MyID.has_PrefetchW()) {
				Burger::StringConcatenate(Detected, ", PrefetchW");
			}
			if (MyID.has_3DNOW()) {
				Burger::StringConcatenate(Detected, ", 3DNOW");
			}
			if (MyID.has_extended3DNOW()) {
				Burger::StringConcatenate(Detected, ", Extended3DNOW");
			}
			Message("Features found %s", &Detected[2]);
		} else {

			// CPUID not available on a 32 bit CPU is not a failure,
			// but it is a warning that your CPU is pretty old.

			Message("Burger::CPUID() is not available");
		}
	}
#endif

	//
	// Display 32 bit Intel special registers
	//

#if defined(BURGER_X86)
	if (uVerbose & VERBOSE_MSG) {
		Burger::e8087Precision uPrecision = Burger::get_8087_precision();
		Burger::set_8087_precision(uPrecision);
		Message("Burger::get_8087_precision() = %u",
			static_cast<uint_t>(uPrecision));
		Burger::e8087Rounding uRounding = Burger::get_8087_rounding();
		Message(
			"Burger::get_8087_rounding() = %u", static_cast<uint_t>(uRounding));
		Burger::set_8087_rounding(uRounding);
	}
#endif

	//
	// Handle AltiVec (MacOS / MacOSX) PowerPC
	//

	if (uVerbose & VERBOSE_MSG) {
		Message("Burger::has_AltiVec() = %u", Burger::has_AltiVec());
		Message("Burger::has_PPC_fsqrt() = %u", Burger::has_PPC_fsqrt());
	}

	//
	// Handle PowerPC special registers
	//

#if defined(BURGER_PPC)
	if (uVerbose & VERBOSE_MSG) {

		Burger::ePowerPCRounding uRounding = Burger::get_PowerPC_rounding();
		Message("Burger::get_PowerPC_rounding() = %u",
			static_cast<uint_t>(uRounding));

		Burger::ePowerPCRounding uRounding2 =
			Burger::set_PowerPC_rounding(Burger::kPPCRoundingDown);
		uRounding2 = Burger::get_PowerPC_rounding();
		Message("Burger::get_PowerPC_rounding() = %u",
			static_cast<uint_t>(uRounding2));
		Burger::set_PowerPC_rounding(Burger::kPPCRoundingUp);
		uRounding2 = Burger::get_PowerPC_rounding();
		Message("Burger::get_PowerPC_rounding() = %u",
			static_cast<uint_t>(uRounding2));
		Burger::set_PowerPC_rounding(uRounding);
	}
#endif

	//
	// Handle 68K special registers
	//

#if defined(BURGER_68881)
	if (uVerbose & VERBOSE_MSG) {
		uint32_t uFPCR = Burger::get_68881_FPCR();
		Message("Burger::get_68881_FPCR() = %08X",
			static_cast<uint_t>(uFPCR));		
	}
#endif

}

/***************************************

	Show Platform features

***************************************/

static void BURGER_API ShowPlatformFeatures(uint_t uVerbose) BURGER_NOEXCEPT
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

		uTest = Burger::Win32::is_XP_or_higher();
		Message("Burger::Win32::is_XP_or_higher() = %u", uTest);

		uTest = Burger::Win32::is_XP3_or_higher();
		Message("Burger::Win32::is_XP3_or_higher() = %u", uTest);

		uTest = Burger::Win32::is_vista_or_higher();
		Message("Burger::Win32::is_vista_or_higher() = %u", uTest);

		uTest = Burger::Win32::is_7_or_higher();
		Message("Burger::Win32::is_7_or_higher() = %u", uTest);

		uTest = Burger::Win32::is_8_or_higher();
		Message("Burger::Win32::is_8_or_higher() = %u", uTest);

		uTest = Burger::Win32::is_10_or_higher();
		Message("Burger::Win32::is_10_or_higher() = %u", uTest);

		// Is 32 bit code running in 64 bit windows?
		uTest = Burger::Win32::is_windows64Bit();
		Message("Burger::Win32::is_windows64Bit() = %u", uTest);

		// DirectX versions
		uTest = Burger::Win32::get_DirectX_version();
		Message("Burger::Win32::get_DirectX_version() = 0x%04X", uTest);

		uTest = Burger::Win32::has_DirectInput();
		Message("Burger::Win32::has_DirectInput() = %u", uTest);
		uTest = Burger::Win32::has_DirectInput8();
		Message("Burger::Win32::has_DirectInput8() = %u", uTest);
		uTest = Burger::Win32::has_XInput();
		Message("Burger::Win32::has_XInput() = %u", uTest);

		uTest = Burger::Win32::has_DirectDraw();
		Message("Burger::Win32::has_DirectDraw() = %u", uTest);

		uTest = Burger::Win32::has_D3D9();
		Message("Burger::Win32::has_D3D9() = %u", uTest);

		uTest = Burger::Win32::has_DirectSound();
		Message("Burger::Win32::has_DirectSound() = %u", uTest);

		uTest = Burger::Win32::has_DirectPlay();
		Message("Burger::Win32::has_DirectPlay() = %u", uTest);

		uTest = Burger::Win32::has_XAudio2();
		Message("Burger::Win32::has_XAudio2() = %u", uTest);

		char QTBuffer[300];

		uTest = Burger::Win32::GetPathToQuickTimeFolder(
			QTBuffer, sizeof(QTBuffer), nullptr);
		Message("Burger::Win32::GetPathToQuickTimeFolder() = %u, %s", uTest,
			QTBuffer);
		uTest =
			Burger::Win32::GetQTSystemDirectoryA(QTBuffer, sizeof(QTBuffer));
		Message(
			"Burger::Win32::GetQTSystemDirectoryA() = %u, %s", uTest, QTBuffer);
		uTest = Burger::Win32::GetQTApplicationDirectoryA(
			QTBuffer, sizeof(QTBuffer));
		Message("Burger::Win32::GetQTApplicationDirectoryA() = %u, %s", uTest,
			QTBuffer);
		uTest =
			Burger::Win32::GetQTExtensionDirectoryA(QTBuffer, sizeof(QTBuffer));
		Message("Burger::Win32::GetQTExtensionDirectoryA() = %u, %s", uTest,
			QTBuffer);
		uTest =
			Burger::Win32::GetQTComponentDirectoryA(QTBuffer, sizeof(QTBuffer));
		Message("Burger::Win32::GetQTComponentDirectoryA() = %u, %s", uTest,
			QTBuffer);
	}

#endif

#if defined(BURGER_MACOSX) || defined(BURGER_WINDOWS)
	if (uVerbose & VERBOSE_MSG) {
		uTest = Burger::Globals::GetQuickTimeVersion();
		Message("Burger::Globals::GetQuickTimeVersion() = 0x%04X", uTest);
	}
#endif

	if (uVerbose & VERBOSE_MSG) {
		// Is Elevated privileges?
		uTest = Burger::IsElevated();
		Message("Burger::IsElevated() = %u", uTest);
	}

#if defined(BURGER_UNIX)
	if (uVerbose & VERBOSE_MSG) {
		// Has uname?
		Burger::Uname TheUname;
		TheUname.Init();
		Message("Burger::Uname.m_sysnam = %s", TheUname.m_sysnam.c_str());
		Message("Burger::Uname.m_nodename = %s", TheUname.m_nodename.c_str());
		Message("Burger::Uname.m_release = %s", TheUname.m_release.c_str());
		Message("Burger::Uname.m_version = %s", TheUname.m_version.c_str());
		Message("Burger::Uname.m_machine = %s", TheUname.m_machine.c_str());
		Message(
			"Burger::Uname.m_domainname = %s", TheUname.m_domainname.c_str());

		static const char* gUnixFlavors[] = {"kUnixUnknown", "kUnixNative",
			"kUnixMSYS2", "kUnixWSL", "kUnixCygwin", "kUnixMacOSX", "kUnixiOS",
			"kUnixAndroid", "kUnixStadia"};

		// Get the Linux flavor
		const Burger::eUnixFlavor uFlavor = Burger::UnixGetFlavor();
		Message("Burger::UnixGetFlavor() = %s", gUnixFlavors[uFlavor]);
	}
#endif

	//
	// Handle MacOS Version
	//

#if defined(BURGER_MACOSX)
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
		uTest = Burger::MacOS::get_os_version();
		Message("Burger::MacOS::get_os_version() = %04X", uTest);

		uTest = Burger::MacOS::is_trap_available(0xA05C);
		Message("Burger::MacOS::is_trap_available(0xA05C) = %u", uTest);

		uTest = Burger::MacOS::get_quicktime_version();
		Message("Burger::MacOS::get_quicktime_version() = 0x%04X", uTest);

		uTest = Burger::MacOS::has_quicktime_power_plug();
		Message("Burger::MacOS::has_quicktime_power_plug() = %u", uTest);

		uTest = Burger::MacOS::get_quickdraw_version();
		Message("Burger::MacOS::get_quickdraw_version() = %04X", uTest);

		uTest = Burger::MacOS::get_appleshare_version();
		Message("Burger::MacOS::get_appleshare_version() = %04X", uTest);

		uTest = Burger::MacOS::get_input_sprocket_version();
		Message("Burger::MacOS::get_input_sprocket_version() = %04X", uTest);

		uTest = Burger::MacOS::get_draw_sprocket_version();
		Message("Burger::MacOS::get_draw_sprocket_version() = %04X", uTest);

		uTest = Burger::MacOS::get_sound_manager_version();
		Message("Burger::MacOS::get_sound_manager_version() = %04X", uTest);

		uTest = Burger::MacOS::get_nav_services_version();
		Message("Burger::MacOS::get_nav_services_version() = %08X", uTest);

		uTest = Burger::MacOS::get_control_strip_version();
		Message("Burger::MacOS::get_control_strip_version() = %08X", uTest);

		Burger::MacOS::ePowerMacType uType =
			Burger::MacOS::get_power_mac_type();
		Message("Burger::MacOS::get_power_mac_type() = %u",
			static_cast<uint_t>(uType));
		Burger::MacAddress_t Address;
		Burger::get_default_mac_address(&Address);
		Message("Burger::get_default_mac_address() = %02X%02X%02X%02X%02X%02X",
			Address.m_Node[0], Address.m_Node[1], Address.m_Node[2],
			Address.m_Node[3], Address.m_Node[4], Address.m_Node[5]);

		uTest = Burger::MacOS::has_GetDeviceList();
		Message("Burger::MacOS::has_GetDeviceList() = %08X", uTest);
		uTest = Burger::MacOS::is_gamma_available();
		Message("Burger::MacOS::is_gamma_available() = %08X", uTest);
	}
#endif
}

/***************************************

	Perform the tests for the macros and compiler
	settings

***************************************/

int BURGER_API TestBrtypes(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Testing Burger Macros");
	}

	uint_t uFailure = TestDataChunkSize(uVerbose);
	uFailure |= TestStructureAlignment(uVerbose);

	// Print messages about features found on the platform

	ShowCPUFeatures(uVerbose);
	ShowPlatformFeatures(uVerbose);

	return static_cast<int>(uFailure);
}
