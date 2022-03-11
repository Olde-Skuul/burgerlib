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
			Message("CPUID_t.m_uCPUType = %u",
				static_cast<uint_t>(MyID.m_uCPUType));
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
		Message(
			"Burger::Get8087Rounding() = %u", static_cast<uint_t>(uRounding));
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
		Message("Burger::GetPowerPCRounding() = %u",
			static_cast<uint_t>(uRounding));

		Burger::ePowerPCRounding uRounding2 =
			Burger::SetPowerPCRounding(Burger::ROUNDDOWN);
		uRounding2 = Burger::GetPowerPCRounding();
		Message("Burger::GetPowerPCRounding() = %u",
			static_cast<uint_t>(uRounding2));
		Burger::SetPowerPCRounding(Burger::ROUNDUP);
		uRounding2 = Burger::GetPowerPCRounding();
		Message("Burger::GetPowerPCRounding() = %u",
			static_cast<uint_t>(uRounding2));
		Burger::SetPowerPCRounding(uRounding);
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

#if defined(BURGER_LINUX) || defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)
	if (uVerbose & VERBOSE_MSG) {
		// Is Elevated privliges?
		uTest = Burger::IsElevated();
		Message("Burger::IsElevated() = %u", uTest);
	}
#endif

#if defined(BURGER_LINUX)
	if (uVerbose & VERBOSE_MSG) {
		// Has uname?
		const Burger::Globals::Uname_t* pUname = Burger::Globals::GetUname();
		Message(
			"Burger::Globals::Uname_t.m_sysnam = %s", pUname->m_sysnam.c_str());
		Message("Burger::Globals::Uname_t.m_nodename = %s",
			pUname->m_nodename.c_str());
		Message("Burger::Globals::Uname_t.m_release = %s",
			pUname->m_release.c_str());
		Message("Burger::Globals::Uname_t.m_version = %s",
			pUname->m_version.c_str());
		Message("Burger::Globals::Uname_t.m_machine = %s",
			pUname->m_machine.c_str());
		Message("Burger::Globals::Uname_t.m_domainname = %s",
			pUname->m_domainname.c_str());

		static const char* gLinuxFlavors[] = {"kLinuxUnknown", "kLinuxNative",
			"kLinuxMSYS2", "kLinuxWSL", "kLinuxCygwin"};

		// Get the Linux flavor
		const Burger::Globals::eLinuxFlavor uFlavor =
			Burger::Globals::GetLinuxFlavor();
		Message(
			"Burger::Globals::GetLinuxFlavor() = %s", gLinuxFlavors[uFlavor]);
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

		uTest = Burger::Mac::GetQuickdrawVersion();
		Message("Burger::Mac::GetQuickdrawVersion() = %04X", uTest);

		uTest = Burger::Mac::GetAppleShareVersion();
		Message("Burger::Mac::GetAppleShareVersion() = %04X", uTest);

		uTest = Burger::Mac::GetInputSprocketVersion();
		Message("Burger::Mac::GetInputSprocketVersion() = %04X", uTest);

		uTest = Burger::Mac::GetDrawSprocketVersion();
		Message("Burger::Mac::GetDrawSprocketVersion() = %04X", uTest);

		uTest = Burger::Mac::GetSoundManagerVersion();
		Message("Burger::Mac::GetSoundManagerVersion() = %04X", uTest);

		uTest = Burger::Mac::GetNavServicesVersion();
		Message("Burger::Mac::GetNavServicesVersion() = %08X", uTest);

		uTest = Burger::Mac::GetControlStripVersion();
		Message("Burger::Mac::GetControlStripVersion() = %08X", uTest);
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
