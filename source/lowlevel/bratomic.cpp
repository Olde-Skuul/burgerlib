/***************************************

	Atomic memory

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "bratomic.h"
#include "brintrinsics.h"
#include "brmemoryfunctions.h"
#include "brstringfunctions.h"
#include "brtick.h"

// Used for MacOSX CPU tests
#if (defined(BURGER_PPC) && defined(BURGER_MACOSX))
#include <sys/sysctl.h>
#endif

// Gestalt for MacOS
#if defined(BURGER_MAC)
#include <Gestalt.h>
#include <OpenTransport.h>
#endif

#if defined(BURGER_VITA)
#include <sce_atomic.h>
#endif

#if defined(BURGER_PS3)
#include <cell/atomic.h>
#endif

#if defined(BURGER_WIIU)
#include <cafe/os.h>
#endif

#if defined(BURGER_MSVC)

// Perform an intrinsic test (Should not cause compile errors)
#include "brvisualstudio.h"

#if defined(BURGER_WINDOWS)
#if (defined(_MSC_VER) && (_MSC_VER < 1400))
#include <mmintrin.h>
#else
#include <intrin.h>
#endif

#elif defined(BURGER_XBOX360)
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>

#elif defined(BURGER_XBOXONE)
#include <immintrin.h>
#endif

#endif

//
// If MacOSX was build with an earlier version of GNU C than 4.0, the atomic
// intrinsics are missing. Use the OSAtomic.h functions in that case.
//

#if defined(BURGER_DARWIN)
#include <libkern/OSAtomic.h>
#endif

/*! ************************************

	\struct Burger::CPUID_t
	\brief Structure that holds the features of the CPU

	On x86 and x64 CPUs, this structure will be filled by
	the use of the instruction CPUID on Intel
	architecture CPUs.

	\note This structure only matters on systems with an x86 or x64 CPU

	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\enum Burger::CPUID_t::eCPU
	\brief Intel CPU enumeration

	Returned by CPUID(CPUID_t*) to list the manufacturer of the Intel type CPU
	current executing the application's code.

	\note This structure only matters on systems with an x86 or x64 CPU

	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_RTSC(void) const
	\brief Returns non-zero if the RTSC instruction is available

	https://en.wikipedia.org/wiki/Time_Stamp_Counter

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instruction is available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_CMOV(void) const
	\brief Returns non-zero if the CMOV instructions are available

	https://en.wikipedia.org/wiki/FCMOV

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_MMX(void) const
	\brief Returns non-zero if the MMX instructions are available

	https://en.wikipedia.org/wiki/MMX_(instruction_set)

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_SSE(void) const
	\brief Returns non-zero if SSE instructions are available

	https://en.wikipedia.org/wiki/Streaming_SIMD_Extensions

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_SSE2(void) const
	\brief Returns non-zero if SSE2 instructions are available

	https://en.wikipedia.org/wiki/SSE2

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_SSE3(void) const
	\brief Returns non-zero if SSE3 instructions are available

	https://en.wikipedia.org/wiki/SSE3

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_SSSE3(void) const
	\brief Returns non-zero if SSSE3 instructions are available

	https://en.wikipedia.org/wiki/SSSE3

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_SSE4a(void) const
	\brief Returns non-zero if SSE4a instructions are available

	https://en.wikipedia.org/wiki/SSE4#SSE4a

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_SSE41(void) const
	\brief Returns non-zero if SSE4.1 instructions are available

	https://en.wikipedia.org/wiki/SSE4#SSE4.1

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_SSE42(void) const
	\brief Returns non-zero if SSE4.2 instructions are available

	https://en.wikipedia.org/wiki/SSE4#SSE4.2

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_MOVBE(void) const
	\brief Returns non-zero if the MOVBE instruction is available

	https://www.tptp.cc/mirrors/siyobik.info/instruction/MOVBE.html

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instruction is available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_AES(void) const
	\brief Returns non-zero if AES instructions are available

	https://en.wikipedia.org/wiki/AES_instruction_set

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_AVX(void) const
	\brief Returns non-zero if AVX instructions are available

	https://en.wikipedia.org/wiki/Advanced_Vector_Extensions

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_AVX2(void) const
	\brief Returns non-zero if AVX2 instructions are available

	https://en.wikipedia.org/wiki/Advanced_Vector_Extensions

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_CMPXCHG16B(void) const
	\brief Returns non-zero if the CMPXCHG16B instruction is available

	http://www.felixcloutier.com/x86/CMPXCHG8B:CMPXCHG16B.html

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the CMPXCHG16B instruction is available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_F16C(void) const
	\brief Returns non-zero if the F16C data type is supported

	https://en.wikipedia.org/wiki/F16C

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the F16C data type is supported, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_FMA3(void) const
	\brief Returns non-zero if FMA3 instructions are available

	https://en.wikipedia.org/wiki/FMA_instruction_set

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_FMA4(void) const
	\brief Returns non-zero if FMA4 instructions are available

	https://en.wikipedia.org/wiki/FMA_instruction_set#FMA4_instruction_set

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_LAHFSAHF(void) const
	\brief Returns non-zero if LAHF and SAHF instructions support long mode

	http://www.tptp.cc/mirrors/siyobik.info/instruction/LAHF.html

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_PrefetchW(void) const
	\brief Returns non-zero if PrefetchW instructions are available

	http://www.felixcloutier.com/x86/PREFETCHW.html

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_3DNOW(void) const
	\brief Returns non-zero if AMD 3DNow! instructions are available

	https://en.wikipedia.org/wiki/3DNow!

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::has_extended3DNOW(void) const
	\brief Returns non-zero if extended AMD 3DNow! instructions are available

	https://en.wikipedia.org/wiki/3DNow!#3DNow_extensions

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\brief Return \ref TRUE if the instruction CPUID is present

	On x86 CPUs, this function will perform a test to determine
	if the CPUID instruction is present. It will return \ref TRUE
	if it does, and \ref FALSE if not. It's very likely this
	will return \ref TRUE on x86 CPUs due to the introduction
	of the instruction occurred in 1993

	On x64 machines, this will always return \ref TRUE and on
	non Intel CPUs it will always return \ref FALSE.

	\note This function only matters on systems with an x86 CPU

	\return \ref TRUE if the instruction is available, \ref FALSE if not

	\sa BURGER_INTEL

***************************************/

#if defined(BURGER_INTEL) || defined(DOXYGEN)
#if !(defined(BURGER_AMD64) || defined(BURGER_XBOX)) || defined(DOXYGEN)

#if defined(BURGER_GNUC) || defined(BURGER_CLANG)
// clang-format off
__asm__(
"	.align	4,0x90\n"
"	.globl __ZN6Burger9has_CPUIDEv\n"
"__ZN6Burger9has_CPUIDEv:\n"
"	pushfl\n"
"	movl	(%esp),%eax\n"
"	xorl	$0x00200000,%eax\n"
"	pushl	%eax\n"
"	popfl\n"
"	pushfl\n"
"	popl	%eax\n"
"	xorl	(%esp),%eax\n"
"	shrl	$21,%eax\n"
"	andl	$1,%eax\n"
"	popfl\n"
"	retl\n"
);

#else

BURGER_DECLSPECNAKED uint_t BURGER_API Burger::has_CPUID(void) BURGER_NOEXCEPT
{
	BURGER_ASM {

		// See of flags bit 21 can be changed.
		// If it can, it's higher than 486, which
		// implies that CPUID is available

		pushfd						// save flags on the stack
		mov		eax,dword ptr[esp]	// Get in register and leave on stack
		xor		eax,0x00200000		// Switch bit 21 for the test
		push	eax					// Set the flags with the new value
		popfd
		pushfd						// Read in the flags into a register
		pop		eax
		xor		eax,dword ptr[esp]	// Did the change "take"
		shr		eax,21				// Move to the lowest bit
		and		eax,1				// Set to TRUE or FALSE
		popfd						// Restore the flags to the original state
		ret
	}
}
#endif
#endif
// clang-format on

/*! ************************************

	\brief Fill in a \ref CPUID_t structure

	On x86 and x64 CPUs, this function will call CPUID several times to extract
	the most commonly requested data from the CPU and fills in the CPUID_t
	structure. On non x86/x64 platforms, this function does not exist.

	\note This function only matters on systems with an x86 or x64 CPU

	\param pOutput Pointer to a CPUID_t to initialize

	\sa BURGER_INTEL

***************************************/

#if !defined(DOXYGEN)

struct CPUHashes_t {
	char m_CPUName[16];           // CPU ID
	Burger::CPUID_t::eCPU m_uCPU; // CPU type to map to
};

static const CPUHashes_t g_CPUHashes[] = {
	{"GenuineIntel", Burger::CPUID_t::kCPUIntel},
	{"AMDisbetter!", Burger::CPUID_t::kCPUAMD},
	{"AuthenticAMD", Burger::CPUID_t::kCPUAMD},
	{"CentaurHauls", Burger::CPUID_t::kCPUCentaur},
	{"CyrixInstead", Burger::CPUID_t::kCPUCyrix},
	{"TransmetaCPU", Burger::CPUID_t::kCPUTransmeta},
	{"GenuineTMx86", Burger::CPUID_t::kCPUTransmeta},
	{"Geode by NSC", Burger::CPUID_t::kCPUNationalSemiconductor},
	{"NexGenDriven", Burger::CPUID_t::kCPUNexgen},
	{"RiseRiseRise", Burger::CPUID_t::kCPURise},
	{"SiS SiS SiS ", Burger::CPUID_t::kCPUSIS},
	{"UMC UMC UMC ", Burger::CPUID_t::kCPUUMC},
	{"VIA VIA VIA ", Burger::CPUID_t::kCPUVIA},
	{"Vortex86 SoC", Burger::CPUID_t::kCPUVortex},
	{"  Shanghai  ", Burger::CPUID_t::kCPUShanghai},
	{"HygonGenuine", Burger::CPUID_t::kCPUHygon},
	{"Genuine  RDC", Burger::CPUID_t::kCPURDC},
	{"E2K MACHINE ", Burger::CPUID_t::kCPUElbrus2000},
	{"MiSTer AO486", Burger::CPUID_t::kCPUMiSTer486},
	{"KVMKVMKVM   ", Burger::CPUID_t::kCPUKVM},
	{"Microsoft Hv", Burger::CPUID_t::kCPUMicrosoftVirtualPC},
	{" lrpepyh  vr", Burger::CPUID_t::kCPUParallels},
	{"prl hyperv  ", Burger::CPUID_t::kCPUParallels},
	{"VMwareVMware", Burger::CPUID_t::kCPUVMWare},
	{"XenVMMXenVMM", Burger::CPUID_t::kCPUXen},
	{"VirtualApple", Burger::CPUID_t::kCPURosetta2}};

//
// Lovely, due to some older CPUs not aware of the CPUID
// ranges, do some sanity checking to ensure that the
// ranges are actually good values and not the result
// of a request of data that the chip isn't aware of
//

static uint32_t BURGER_API FixCount(uint32_t uBase, int iValue) BURGER_NOEXCEPT
{
	// Invalid if too small
	if (static_cast<uint32_t>(iValue) <= uBase) {
		iValue = 0;
	} else {
		// Invalid if too big
		if ((static_cast<uint32_t>(iValue) - uBase) > 1024) {
			iValue = 0;
		}
	}
	return static_cast<uint32_t>(iValue);
}

#endif

void BURGER_API Burger::CPUID(CPUID_t* pOutput) BURGER_NOEXCEPT
{
	//
	// Clear out the result
	//
	MemoryClear(pOutput, sizeof(CPUID_t));
	pOutput->m_uCPUType = CPUID_t::kCPUUnknown;

	//
	// Buffer to get data from CPUID
	//

	int Results[4];

	//
	// Get the CPU name and highest allowable call
	//

	__cpuid(Results, 0);

	//
	// Highest allowable value
	//

	uint32_t uHighestID = FixCount(0, Results[0]);
	pOutput->m_uHighestCPUID = uHighestID;

	//
	// Convert to a string
	//
	int* pWork = static_cast<int*>(static_cast<void*>(pOutput->m_CPUName));
	pWork[0] = Results[1];
	pWork[1] = Results[3];
	pWork[2] = Results[2];
	pOutput->m_CPUName[12] = 0;

	//
	// Iterate over known CPU names to create the enumeration
	//

	uintptr_t i = BURGER_ARRAYSIZE(g_CPUHashes);
	const CPUHashes_t* pHash = g_CPUHashes;
	do {
		if (!StringCompare(pHash->m_CPUName, pOutput->m_CPUName)) {
			pOutput->m_uCPUType = pHash->m_uCPU;
			break;
		}
		++pHash;
	} while (--i);

	//
	// Extended info okay?
	//

	if (uHighestID) {
		__cpuid(Results, 1);
		pOutput->m_uModel = static_cast<uint32_t>(Results[0]);
		pOutput->m_uBrand = static_cast<uint32_t>(Results[1]);
		pOutput->m_uCPUID1ECX = static_cast<uint32_t>(Results[2]);
		pOutput->m_uCPUID1EDX = static_cast<uint32_t>(Results[3]);

		// Test if xsave is allowed by the OS
		if (pOutput->m_uCPUID1ECX & 0x08000000U) {
			// Execute xgetbv if present
			pOutput->m_uXGETBV = _xgetbv(0);
		}

		// Even more features?
		if (uHighestID >= 7) {
			__cpuidex(Results, 7, 0);
			pOutput->m_uCPUID7EBX = static_cast<uint32_t>(Results[1]);
			pOutput->m_uCPUID7ECX = static_cast<uint32_t>(Results[2]);
			pOutput->m_uCPUID7EDX = static_cast<uint32_t>(Results[3]);
		}
	}

	//
	// Is there extended information?
	//

	__cpuid(Results, 0x80000000U);
	uint32_t uExtendedID = FixCount(0x80000000U, Results[0]);
	pOutput->m_uHighestCPUIDExtended = uExtendedID;

	//
	// Get the extended information (AMD)
	//

	if (uExtendedID >= 0x80000001U) {
		__cpuid(Results, 0x80000001U);
		pOutput->m_uCPUID80000001ECX = static_cast<uint32_t>(Results[2]);
		pOutput->m_uCPUID80000001EDX = static_cast<uint32_t>(Results[3]);

		//
		// Check if Processor Brand String is available
		// Opcodes 0x80000002-0x80000004 are needed
		//
		if (uExtendedID >= 0x80000004U) {
			//
			// Pull down the string
			//
			i = 0;
			pWork = static_cast<int*>(static_cast<void*>(pOutput->m_BrandName));
			do {
				__cpuid(Results, static_cast<int>(0x80000002 + i));
				pWork[0] = Results[0];
				pWork[1] = Results[1];
				pWork[2] = Results[2];
				pWork[3] = Results[3];
				pWork += 4;
			} while (++i < 3);
			// Ensure it's zero terminated
			pOutput->m_BrandName[48] = 0;
		}
	}

	//
	// Is there hypervisor information?
	//

	__cpuid(Results, 0x40000000U);
	uint32_t uHyperVisorID = FixCount(0x40000000U, Results[0]);
	pOutput->m_uHighestCPUIDHyperVisor = uHyperVisorID;
	if (uHyperVisorID >= 0x40000001U) {
		//
		// Get the hypervisor name and signature
		//

		pWork =
			static_cast<int*>(static_cast<void*>(pOutput->m_HypervisorName));
		pWork[0] = Results[1];
		pWork[1] = Results[2];
		pWork[2] = Results[3];
		pOutput->m_HypervisorName[12] = 0;
		__cpuid(Results, 0x40000001U);
		pWork = static_cast<int*>(
			static_cast<void*>(pOutput->m_HypervisorSignature));
		pWork[0] = Results[0];
		pOutput->m_HypervisorSignature[4] = 0;
	}
}

#endif

/***************************************

	Power PC feature detection

***************************************/

/*! ************************************

	\brief Return \ref TRUE if the AltiVec instruction set is present

	On PowerPC CPUs, this function will perform a test to determine
	if the AltiVec instruction is present. It will return \ref TRUE
	if it does, and \ref FALSE if not.

	On XBox 360 and PS3 machines, this will always return \ref TRUE and on
	non PowerPC CPUs it will always return \ref FALSE.

	\note This function only matters on systems with an PowerPC CPU
	\return \ref TRUE if AltiVec is available, \ref FALSE if not
	\sa BURGER_PPC

***************************************/

#if (defined(BURGER_PPC) && defined(BURGER_MACOSX)) || defined(DOXYGEN)

uint_t BURGER_API Burger::has_AltiVec(void) BURGER_NOEXCEPT
{
	int selector[2];
	int gHasAltivec;

	// Create a machine information block

	selector[0] = CTL_HW;
	selector[1] = HW_VECTORUNIT;
	size_t uLength = sizeof(gHasAltivec);

	// Ask for Altivec
	int iError = sysctl(selector, 2, &gHasAltivec, &uLength, NULL, 0);
	uint_t uResult = FALSE;
	if (!iError) {

		// Found flag
		uResult = static_cast<uint_t>(gHasAltivec);
	}
	return uResult;
}

#elif (defined(BURGER_PPC) && defined(BURGER_MAC))

uint_t BURGER_API Burger::has_AltiVec(void) BURGER_NOEXCEPT
{
	// Carbon/Classic version

	long gestaltAnswer;
	uint_t uResult = 0;
	if (!Gestalt(gestaltPowerPCProcessorFeatures, &gestaltAnswer)) {
		// Move the flag to the lowest bit
		uResult = static_cast<uint_t>(
					  gestaltAnswer >> gestaltPowerPCHasVectorInstructions) &
			1U;
	}

	// Return zero or non-zero
	return uResult;
}

#endif

/*! ************************************

	\brief Return \ref TRUE if floating point sqrt is supported in hardware

	Most modern CPUs have floating point in hardware, and on most platforms,
	this function will return \ref TRUE. On 680x0 and PowerPC platforms, this
	function will test for the existance of an instruction to perform a floating
	point square root and will return non-zero if one is found.

	\return Non-zero if fsqrt() is available, \ref FALSE if not.
	\sa BURGER_68K and BURGER_PPC

***************************************/

#if (defined(BURGER_PPC) && defined(BURGER_MACOSX)) || defined(DOXYGEN)

uint_t BURGER_API Burger::has_PPC_fsqrt(void) BURGER_NOEXCEPT
{
	size_t uLength = 4;
	uint32_t uBuffer = 0;

	// Call the OS to get the flag for the fsqrt instruction
	long lResult =
		sysctlbyname("hw.optional.fsqrt", &uBuffer, &uLength, nullptr, 0);
	return (!lResult && uBuffer) ? TRUE : FALSE;
}

#elif (defined(BURGER_PPC) && defined(BURGER_MAC))

uint_t BURGER_API Burger::has_PPC_fsqrt(void) BURGER_NOEXCEPT
{
	// Carbon/Classic version

	long gestaltAnswer;
	uint_t uResult = 0;
	if (!Gestalt(gestaltPowerPCProcessorFeatures, &gestaltAnswer)) {
		// Return the FPU type
		uResult = static_cast<uint_t>(gestaltAnswer >>
					  gestaltPowerPCHasSquareRootInstructions) &
			1;
	}

	// Return zero or non-zero
	return uResult;
}

#endif

/*! ************************************

	\brief Return \ref TRUE if floating point is supported in hardware

	Most modern CPUs have floating point in hardware, and on most platforms,
	this function will return \ref TRUE. On 680x0 platforms, this function will
	test for the existance of an FPU and will return non-zero if one is found
	and zero if there is no FPU.

	\note If a 680x0 project is running on the MacOS 9 680x0 CPU emulator, this
	function will always return \ref FALSE to help prevent runtime code from
	executing FPU instructions which would cause a runtime exception.

	\return Non-zero if an FPU is available, \ref FALSE if not.

	\sa BURGER_68K

***************************************/

#if (defined(BURGER_MAC) && defined(BURGER_68K)) || defined(DOXYGEN)

uint_t BURGER_API Burger::has_68kFPU(void) BURGER_NOEXCEPT
{
	// Carbon/Classic version

	long gestaltAnswer;
	uint_t uResult = 0;
	if (!Gestalt(gestaltFPUType, &gestaltAnswer)) {
		// Return the FPU type
		uResult = static_cast<uint_t>(gestaltAnswer);
	}

	// Return zero or non-zero
	return uResult;
}

#endif

/*! ************************************

	\brief Atomically swaps a 32 bit value for one in memory

	Atomically swap a value in a memory only if the memory matches the previous
	value.

	\param pOutput Pointer to a 32 bit aligned memory location to swap with
	\param uOld 32 bit value to compare with the memory location
	\param uNew 32 bit value to store if the comparison succeeds

	\return \ref TRUE if successful, \ref FALSE if the memory is unstable

***************************************/

uint_t BURGER_API Burger::atomic_compare_and_set(
	volatile uint32_t* pOutput, uint32_t uOld, uint32_t uNew) BURGER_NOEXCEPT
{
	// MacOS PowerPC version
#if defined(BURGER_METROWERKS) && defined(BURGER_PPC)
	register uint32_t uOld2 = uOld;
	register uint32_t uNew2 = uNew;
	register volatile uint32_t* pOutput2 = pOutput;

	register uint32_t uTemp;
	asm {
loop:	lwarx	uTemp,0,pOutput2
		cmplw	uTemp,uOld2
		bne-	abort
		stwcx.	uNew2,0,pOutput2
		bne-	loop
abort:
	}
	return uTemp == uOld;

	// MacOS 68k version using the OpenTransport API (Inline assembly)
#elif defined(BURGER_MAC)
	return OTCompareAndSwap32(uOld, uNew, const_cast<uint32_t*>(pOutput));

	// Playstation Vita version
#elif defined(BURGER_VITA)
	return sceAtomicCompareAndSwap32(
			   reinterpret_cast<volatile int32_t*>(pOutput), uOld, uNew) ==
		uOld;

	// Playstation 3 version
#elif defined(BURGER_PS3)
	uint_t uResult;
	do {
		uint32_t uTemp = __lwarx(pOutput);
		uResult = (uTemp == uOld);
		if (!uResult) {
			break;
		}
	} while (__stwcx(pOutput, uNew) == 0);
	return uResult;

	// Nintendo WiiU
#elif defined(BURGER_WIIU)
	uint_t uResult;
	do {
		uint32_t uTemp = __LWARX(const_cast<uint32_t*>(pOutput), 0);
		uResult = (uTemp == uOld);
		if (!uResult) {
			break;
		}
		__DCBST(const_cast<uint32_t*>(pOutput), 0);
	} while (__STWCX(const_cast<uint32_t*>(pOutput), 0, uNew) == 0);
	return uResult;

// Microsoft/Codewarrior/Watcom
#elif defined(BURGER_MSVC) || \
	((defined(BURGER_WATCOM) || defined(BURGER_METROWERKS)) && \
		defined(BURGER_INTEL))

	return _InterlockedCompareExchange(
			   reinterpret_cast<volatile long*>(pOutput),
			   static_cast<long>(uNew),
			   static_cast<long>(uOld)) == static_cast<long>(uOld);

	// If GnuC 4.0 or higher. use the intrinsics
#elif (BURGER_GNUC > 40100) || defined(BURGER_CLANG)
	return static_cast<uint_t>(
		__sync_bool_compare_and_swap(pOutput, uOld, uNew));

	// If the compiler doesn't have intrinsics, use the Darwin API
#elif defined(BURGER_DARWIN)
	return static_cast<uint_t>(OSAtomicCompareAndSwap32Barrier(
		static_cast<int32_t>(uOld), static_cast<int32_t>(uNew),
		reinterpret_cast<volatile int32_t*>(pOutput)));

	// Generic version for REALLY old CPUs
#else
#warning Update atomic_compare_and_set(volatile uint32_t*, uint32_t, uint32_t)
	uint_t bResult = *pOutput == uOld;
	if (bResult) {
		*pOutput = uNew;
	}
	return bResult;
#endif
}

/*! ************************************

	\brief Atomically sets a 32 bit value to memory

	For multi-CPU machines, this function will atomically store a value
	with using appropriate locking for all CPUs

	\param pOutput Pointer to a 32 bit aligned memory location to set
	\param uInput 32 bit value to set in the memory location

	\return Value that was stored in the variable previously

***************************************/

uint32_t BURGER_API Burger::atomic_set(
	volatile uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT
{
#if defined(BURGER_PPC) && defined(BURGER_METROWERKS)
	register
#endif
		uint32_t uTemp;

	// Metrowerks for PowerPC MacOS
#if defined(BURGER_PPC) && defined(BURGER_METROWERKS)
	// Assign to registers
	register volatile uint32_t* pOutput2 = pOutput;
	register uint32_t uInput2 = uInput;
	asm {
loop:	lwarx	uTemp,0,pOutput2
		stwcx.	uInput2,0,pOutput2
		bne-	loop
	}

	// MacOS 68k version using the OpenTransport API (Inline assembly)
#elif defined(BURGER_MAC)
	do {
		uTemp = pOutput[0];
	} while (
		!OTCompareAndSwap32(uTemp, uInput, const_cast<uint32_t*>(pOutput)));

	// Playstation Vita version
#elif defined(BURGER_VITA)
	uTemp = static_cast<uint32_t>(
		sceAtomicExchange32(reinterpret_cast<volatile int32_t*>(pOutput),
			static_cast<int32_t>(uInput)));

	// Playstation 3 PowerPC
#elif defined(BURGER_PS3)
	do {
		uTemp = __lwarx(pOutput);
	} while (__stwcx(pOutput, uInput) == 0);

	// Nintendo WiiU
#elif defined(BURGER_WIIU)
	do {
		uTemp = __LWARX(const_cast<uint32_t*>(pOutput), 0);
		__DCBST(const_cast<uint32_t*>(pOutput), 0);
	} while (__STWCX(const_cast<uint32_t*>(pOutput), 0, uInput) == 0);

// Microsoft/Codewarrior/Watcom
#elif defined(BURGER_MSVC) || \
	((defined(BURGER_WATCOM) || defined(BURGER_METROWERKS)) && \
		defined(BURGER_INTEL))
	uTemp = static_cast<uint32_t>(_InterlockedExchange(
		reinterpret_cast<volatile long*>(pOutput), static_cast<long>(uInput)));

	// If GnuC 4.0 or higher. use the intrinsics
#elif (BURGER_GNUC > 40100) || defined(BURGER_CLANG)
	uTemp = __sync_lock_test_and_set(pOutput, uInput);

	// If the compiler doesn't have intrinsics, use the Darwin API
#elif defined(BURGER_DARWIN)
	do {
		uTemp = pOutput[0];
	} while (!OSAtomicCompareAndSwap32Barrier(static_cast<int32_t>(uTemp),
		static_cast<int32_t>(uInput),
		static_cast<volatile int32_t*>(static_cast<volatile void*>(pOutput))));

	// Generic version
#else
#warning Update atomic_set(volatile uint32_t*, uint32_t)
	uTemp = pOutput[0];
	pOutput[0] = uInput;
#endif

	return uTemp;
}

/*! ************************************

	\brief Atomically gets a 32 bit value from memory

	For multi-CPU machines, this function will atomically load a value
	with using appropriate locking for all CPUs

	\param pInput Pointer to a 32 bit aligned memory location to load from

	\return Value that was stored in the variable

***************************************/

uint32_t BURGER_API Burger::atomic_get(
	volatile uint32_t* pInput) BURGER_NOEXCEPT
{
	// Metrowerks for PowerPC MacOS
#if defined(BURGER_PPC) && defined(BURGER_METROWERKS)
	// Assign to registers
	register uint32_t uTemp;
	register volatile uint32_t* pInput2 = pInput;
	asm {
loop:	lwarx	uTemp,0,pInput2
		stwcx.	uTemp,0,pInput2
		bne-	loop
	}
	return uTemp;

	// MacOS 68k version using the OpenTransport API (Inline assembly)
#elif defined(BURGER_MAC)
	return static_cast<uint32_t>(OTAtomicAdd32(
		0, reinterpret_cast<SInt32*>(const_cast<uint32_t*>(pInput))));

	// Playstation Vita version
#elif defined(BURGER_VITA)
	return static_cast<uint32_t>(
		sceAtomicLoad32(reinterpret_cast<volatile int32_t*>(pInput)));

	// Playstation 3 PowerPC
#elif defined(BURGER_PS3)
	uint32_t uTemp;
	do {
		uTemp = __lwarx(pInput);
	} while (__stwcx(pInput, uTemp) == 0);
	return uTemp;

	// Nintendo WiiU
#elif defined(BURGER_WIIU)
	uint32_t uTemp;
	do {
		uTemp = __LWARX(const_cast<uint32_t*>(pInput), 0);
		__DCBST(const_cast<uint32_t*>(pInput), 0);
	} while (__STWCX(const_cast<uint32_t*>(pInput), 0, uTemp) == 0);
	return uTemp;

// Microsoft/Codewarrior/Watcom
#elif defined(BURGER_MSVC) || \
	((defined(BURGER_WATCOM) || defined(BURGER_METROWERKS)) && \
		defined(BURGER_INTEL))
	return static_cast<uint32_t>(
		_InterlockedExchangeAdd(reinterpret_cast<volatile long*>(pInput), 0));

#elif (BURGER_GNUC > 50000) || __has_builtin(__atomic_load_n)
	return __atomic_load_n(pInput, __ATOMIC_SEQ_CST);

	// If the compiler doesn't have intrinsics, use the Darwin API
#elif defined(BURGER_DARWIN)
	return static_cast<uint32_t>(
		OSAtomicAdd32Barrier(0, reinterpret_cast<volatile int32_t*>(pInput)));

	// Generic code
#else
#warning atomic_get(uint32_t*)
	return pInput[0];
#endif
}

/*! ************************************

	\brief Atomically add a 32 bit value to a variable in memory

	For multi-CPU machines, this function will atomically add a value
	to one stored in memory using appropriate locking for all CPUs. It will
	return the value before the addition was applied.

	\param pOutput Pointer to a 32 bit aligned memory location to add a value
		to
	\param uInput 32 bit value to add

	\return Value that was stored in the variable previously

***************************************/

uint32_t BURGER_API Burger::atomic_add(
	volatile uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT
{
	// Metrowerks for PowerPC MacOS
#if defined(BURGER_PPC) && defined(BURGER_METROWERKS)
	// Assign to registers
	register uint32_t uResult;
	register uint32_t uValue = uInput;
	register volatile uint32_t* pInput = pOutput;
	asm {
loop:	lwarx	uResult,0,pInput
		add		uResult,uResult,uValue
		stwcx.	uResult,0,pInput
		bne-	loop
	}
	return uResult;

	// MacOS 68k version using the OpenTransport API (Inline assembly)
#elif defined(BURGER_MAC)
	return static_cast<uint32_t>(OTAtomicAdd32(static_cast<SInt32>(uInput),
		reinterpret_cast<SInt32*>(const_cast<uint32_t*>(pOutput))));

	// Playstation Vita version
#elif defined(BURGER_VITA)
	return static_cast<uint32_t>(
		sceAtomicAdd32(reinterpret_cast<volatile int32_t*>(pOutput),
			static_cast<int32_t>(uInput)));

	// Playstation 3 PowerPC
#elif defined(BURGER_PS3)
	uint32_t uTemp;
	do {
		uTemp = __lwarx(pOutput);
	} while (__stwcx(pOutput, uTemp + uInput) == 0);
	return uTemp;

	// Nintendo WiiU
#elif defined(BURGER_WIIU)
	uint32_t uTemp;
	do {
		uTemp = __LWARX(const_cast<uint32_t*>(pOutput), 0);
		__DCBST(const_cast<uint32_t*>(pOutput), 0);
	} while (__STWCX(const_cast<uint32_t*>(pOutput), 0, uTemp + uInput) == 0);
	return uTemp;

// Microsoft/Codewarrior/Watcom
#elif defined(BURGER_MSVC) || \
	((defined(BURGER_WATCOM) || defined(BURGER_METROWERKS)) && \
		defined(BURGER_INTEL))
	return static_cast<uint32_t>(_InterlockedExchangeAdd(
		reinterpret_cast<volatile long*>(pOutput), static_cast<long>(uInput)));

	// If GnuC 4.0 or higher. use the intrinsics
#elif (BURGER_GNUC > 40100) || defined(BURGER_CLANG)
	return __sync_fetch_and_add(pOutput, uInput);

	// If the compiler doesn't have intrinsics, use the Darwin API
#elif defined(BURGER_DARWIN)
	return static_cast<uint32_t>(OSAtomicAdd32Barrier(
		uInput, reinterpret_cast<volatile int32_t*>(pOutput)));

	// Generic code
#else
#warning atomic_add(uint32_t*, uint32_t)
	uint32_t uTemp = pInput[0];
	pInput[0] = uTemp + uInput;
	return uTemp;
#endif
}

/*! ************************************

	\brief Attempt to lock a spin lock

	Try to atomically set a lock to \ref TRUE. If the lock is already set to
	\ref TRUE, then return \ref FALSE to not it was not successful. If the lock
	was \ref FALSE and was successfully set to \ref TRUE, return \ref TRUE

	\param pInput Pointer to a 32 bit aligned memory location to lock

	\return \ref TRUE if successful, \ref FALSE if it's already locked

***************************************/

uint_t BURGER_API Burger::atomic_trylock(
	volatile uint32_t* pInput) BURGER_NOEXCEPT
{
	// Darwin has an API for this
#if defined(BURGER_DARWIN)
	BURGER_STATIC_ASSERT(sizeof(OSSpinLock) == sizeof(uint32_t));
	return OSSpinLockTry(reinterpret_cast<volatile OSSpinLock*>(pInput));

	// MacOS PowerPC version
#elif defined(BURGER_METROWERKS) && defined(BURGER_PPC)
	register uint32_t uNew = 1;
	register volatile uint32_t* pInput2 = pInput;

	register uint32_t uResult;
	asm {
loop:	lwarx	uResult,0,pInput2
		cmpwi	uResult,0
		bne-	abort
		stwcx.	uNew,0,pInput2
		bne-	loop
abort:
	}
	return !uResult;

	// MacOS 68k version using the OpenTransport API (Inline assembly)
#elif defined(BURGER_MAC)
	return static_cast<uint32_t>(OTAtomicSetBit(
			   reinterpret_cast<UInt8*>(const_cast<uint32_t*>(pInput)), 0)) ==
		0;

	// Playstation Vita version
#elif defined(BURGER_VITA)
	return static_cast<uint32_t>(sceAtomicCompareAndSwap32(
		reinterpret_cast<volatile int32_t*>(pInput), 0, 1));

	// Playstation 3 version
#elif defined(BURGER_PS3)
	uint_t uResult;
	do {
		uResult = __lwarx(pInput);
		if (uResult) {
			break;
		}
	} while (__stwcx(pInput, 1) == 0);
	return !uResult;

	// Nintendo WiiU
#elif defined(BURGER_WIIU)
	uint_t uResult;
	do {
		uResult = __LWARX(const_cast<uint32_t*>(pInput), 0);
		if (uResult) {
			break;
		}
		__DCBST(const_cast<uint32_t*>(pInput), 0);
	} while (__STWCX(const_cast<uint32_t*>(pInput), 0, 1) == 0);
	return !uResult;

// Microsoft for ARM is a special case
#elif defined(BURGER_MSVC) && defined(BURGER_ARM)
	return _InterlockedExchange_acq(
			   reinterpret_cast<volatile long*>(pInput), 1) == 0;

// Microsoft/Codewarrior/Watcom
#elif defined(BURGER_MSVC) || \
	((defined(BURGER_WATCOM) || defined(BURGER_METROWERKS)) && \
		defined(BURGER_INTEL))
	return _InterlockedExchange(reinterpret_cast<volatile long*>(pInput), 1) ==
		0;

	// If GnuC 4.0 or higher. use the intrinsics
#elif (BURGER_GNUC > 40100) || defined(BURGER_CLANG)
	return !__sync_lock_test_and_set(pInput, 1);

	// Generic code
#else
#warning atomic_trylock(uint32_t*)
	uint32_t uTemp = pInput[0];
	pInput[0] = 1;
	return !uTemp;
#endif
}

/*! ************************************

	\brief Lock a spin lock

	Wait until a spin lock is released and then lock it. If the lock is already
	in possession by another thread, let this thread sleep for a moment to let
	the other thread do it's job and release the lock for this thread's use.

	\note This is NOT a mutex, it's an atomic value. A thread can lock forever
		if a thread locks a spin lock twice, since the thread won't exit this
		function to release the thread.

	\param pInput Pointer to a 32 bit aligned memory location to lock

	\sa CriticalSection, or atomic_unlock()

***************************************/

void BURGER_API Burger::atomic_lock(volatile uint32_t* pInput) BURGER_NOEXCEPT
{
	// Darwin has an API for this
#if defined(BURGER_DARWIN)
	OSSpinLockLock(reinterpret_cast<volatile OSSpinLock*>(pInput));

#else
	uint32_t uCount = 0;
	while (!atomic_trylock(pInput)) {

		// Start with a simple pause/yield instruction
		if (++uCount < 40U) {
			// Issue a pause instruction, or yield the CPU
			BURGER_PAUSEOPCODE();
		} else {
			// Okay, yield the thread for a moment
			// or on really old systems, allow cooperative multitasking
			sleep_ms(0);
		}
	}
#endif
}

/*! ************************************

	\brief Unlock a spin lock

	Set a spin lock to zero, atomically.

	\param pInput Pointer to a 32 bit aligned memory location to unlock

	\sa atomic_lock(), CriticalSection

***************************************/

void BURGER_API Burger::atomic_unlock(volatile uint32_t* pInput) BURGER_NOEXCEPT
{
	// Darwin has an API for this
#if defined(BURGER_DARWIN)
	OSSpinLockUnlock(reinterpret_cast<volatile OSSpinLock*>(pInput));

// MacOS PowerPC version
#elif defined(BURGER_METROWERKS) && defined(BURGER_PPC)
	*pInput = 0;

	// MacOS 68k version using the OpenTransport API (Inline assembly)
#elif defined(BURGER_MAC)
	*pInput = 0;

	// Playstation Vita version
#elif defined(BURGER_VITA)
	sceAtomicStore32(reinterpret_cast<volatile int32_t*>(pInput), 0);

	// Playstation 3 version
#elif defined(BURGER_PS3)
	cellAtomicStore32(const_cast<uint32_t*>(pInput), 0);

	// Nintendo WiiU
#elif defined(BURGER_WIIU)
	OSMemoryBarrier();
	*pInput = 0;

#elif defined(BURGER_MSVC) && defined(BURGER_ARM)
	_InterlockedExchange_rel(reinterpret_cast<volatile long*>(pInput), 0);

#elif defined(BURGER_MSVC) || \
	((defined(BURGER_METROWERKS) || defined(BURGER_WATCOM)) && \
		defined(BURGER_INTEL))
	BURGER_MEMORYBARRIER();
	*pInput = 0;

	// If GnuC 4.0 or higher. use the intrinsics
#elif (BURGER_GNUC > 40100) || defined(BURGER_CLANG)
	__sync_lock_release(pInput);

// Generic code
#else
#warning atomic_unlock(uint32_t*)
	*pInput = 0;
#endif
}

/*! ************************************

	\brief Atomically swaps a 64 bit value for one in memory

	Atomically swap a value in a memory only if the memory matches the previous
	value.

	\param pOutput Pointer to a 64 bit aligned memory location to swap with
	\param uOld 64 bit value to compare with the memory location
	\param uNew 64 bit value to store if the comparison succeeds

	\return \ref TRUE if successful, \ref FALSE if the memory is unstable

***************************************/

#if defined(BURGER_64BITCPU) || defined(DOXYGEN)
uint_t BURGER_API Burger::atomic_compare_and_set(
	volatile uint64_t* pOutput, uint64_t uOld, uint64_t uNew) BURGER_NOEXCEPT
{
	// Playstation 3 version
#if defined(BURGER_PS3)
	uint_t uResult;
	do {
		uint64_t uTemp = __ldarx(pOutput);
		uResult = (uTemp == uOld);
		if (!uResult) {
			break;
		}
	} while (__stdcx(pOutput, uNew) == 0);
	return uResult;

// Microsoft/Codewarrior/Watcom
#elif defined(BURGER_MSVC)

	return _InterlockedCompareExchange64(
			   reinterpret_cast<volatile __int64*>(pOutput),
			   static_cast<__int64>(uNew),
			   static_cast<__int64>(uOld)) == static_cast<__int64>(uOld);

	// If GnuC 4.0 or higher. use the intrinsics
#elif (BURGER_GNUC > 40100) || defined(BURGER_CLANG)
	return static_cast<uint_t>(
		__sync_bool_compare_and_swap(pOutput, uOld, uNew));

	// If the compiler doesn't have intrinsics, use the Darwin API
#elif defined(BURGER_DARWIN)
	return static_cast<uint_t>(OSAtomicCompareAndSwap64Barrier(
		static_cast<int64_t>(uOld), static_cast<int64_t>(uNew),
		reinterpret_cast<volatile int64_t*>(pOutput)));

	// Generic version for REALLY old CPUs
#else
#warning Update atomic_compare_and_set(volatile uint64_t*, uint64_t, uint64_t)
	uint_t bResult = *pOutput == uOld;
	if (bResult) {
		*pOutput = uNew;
	}
	return bResult;
#endif
}

/*! ************************************

	\brief Atomically sets a 64 bit value for one in memory

	For multi-CPU machines, this function will atomically swap a value
	with one stored in memory using appropriate locking for all CPUs

	\note This function is only available on systems with a 64 bit CPU

	\param pOutput Pointer to a 64 bit aligned memory location to set
	\param uInput 64 bit value to set in the memory location

	\return Value that was stored in the variable previously

***************************************/

uint64_t BURGER_API Burger::atomic_set(
	volatile uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT
{
	uint64_t uTemp;

	// Playstation 3 PowerPC
#if defined(BURGER_PS3)
	do {
		uTemp = __ldarx(pOutput);
	} while (__stdcx(pOutput, uInput) == 0);

#elif defined(BURGER_MSVC)
	uTemp = _InterlockedExchange64(
		reinterpret_cast<volatile __int64*>(pOutput), uInput);

	// If GnuC 4.0 or higher. use the intrinsics
#elif (BURGER_GNUC > 40100) || defined(BURGER_CLANG)
	do {
		uTemp = pOutput[0];
	} while (__sync_val_compare_and_swap(pOutput, uTemp, uInput) != uTemp);

	// If the compiler doesn't have intrinsics, use the Darwin API
#elif defined(BURGER_DARWIN)
	do {
		uTemp = pOutput[0];
	} while (!OSAtomicCompareAndSwap64Barrier(static_cast<int64_t>(uTemp),
		static_cast<int64_t>(uInput),
		static_cast<volatile int64_t*>(static_cast<volatile void*>(pOutput))));

	// Generic version
#else
#warning Update atomic_set(volatile uint64_t*, uint64_t)
	uTemp = pOutput[0];
	pOutput[0] = uInput;
#endif

	return uTemp;
}

/*! ************************************

	\brief Atomically gets a 64 bit value from memory

	For multi-CPU machines, this function will atomically load a value
	with using appropriate locking for all CPUs

	\param pInput Pointer to a 64 bit aligned memory location to load from

	\return Value that was stored in the variable

***************************************/

uint64_t BURGER_API Burger::atomic_get(
	volatile uint64_t* pInput) BURGER_NOEXCEPT
{
	// Playstation 3 PowerPC
#if defined(BURGER_PS3)
	uint64_t uTemp;
	do {
		uTemp = __ldarx(pInput);
	} while (__stdcx(pInput, uTemp) == 0);
	return uTemp;

// Microsoft/Codewarrior/Watcom
#elif defined(BURGER_MSVC)
	return static_cast<uint64_t>(_InterlockedExchangeAdd64(
		reinterpret_cast<volatile __int64*>(pInput), 0));

#elif (BURGER_GNUC > 50000) || __has_builtin(__atomic_load_n)
	return __atomic_load_n(pInput, __ATOMIC_SEQ_CST);

// If the compiler doesn't have intrinsics, use the Darwin API
#elif defined(BURGER_DARWIN)
	return static_cast<uint64_t>(
		OSAtomicAdd64Barrier(0, reinterpret_cast<volatile int64_t*>(pInput)));

#else
#warning atomic_get(uint64_t*)
	return pInput[0];
#endif
}

/*! ************************************

	\brief Atomically add a 64 bit value to a variable in memory

	For multi-CPU machines, this function will atomically add a value
	to one stored in memory using appropriate locking for all CPUs. It will
	return the value before the addition was applied.

	\param pOutput Pointer to a 64 bit aligned memory location to add a value
		to
	\param uInput 64 bit value to add

	\return Value that was stored in the variable previously

***************************************/

uint64_t BURGER_API Burger::atomic_add(
	volatile uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT
{
	// Playstation 3 PowerPC
#if defined(BURGER_PS3)
	uint64_t uTemp;
	do {
		uTemp = __ldarx(pOutput);
	} while (__stdcx(pOutput, uTemp + uInput) == 0);
	return uTemp;

// Microsoft/Codewarrior/Watcom
#elif defined(BURGER_MSVC)
	return static_cast<uint64_t>(_InterlockedExchangeAdd64(
		reinterpret_cast<volatile __int64*>(pOutput), uInput));

	// If GnuC 4.0 or higher. use the intrinsics
#elif (BURGER_GNUC > 40100) || defined(BURGER_CLANG)
	return __sync_fetch_and_add(pOutput, uInput);

	// If the compiler doesn't have intrinsics, use the Darwin API
#elif defined(BURGER_DARWIN)
	return static_cast<uint64_t>(OSAtomicAdd64Barrier(
		uInput, reinterpret_cast<volatile int64_t*>(pOutput)));

	// Generic code
#else
#warning atomic_add(uint64_t*, uint64_t)
	uint64_t uTemp = pInput[0];
	pInput[0] = uTemp + uInput;
	return uTemp;
#endif
}
#endif

/***************************************

	\brief Intrinsic _xgetbv for clang and GNU

	This instrinsic is not supported by older versions of clang and GNU, it's
	added here for forward compatibility.

	\param xcr Value for the ECX register

	\return Value returned byt the xgetbv instruction

***************************************/

#if defined(BURGER_INTEL) && (defined(BURGER_CLANG) || defined(BURGER_GNUC))
#if !__has_builtin(_xgetbv)
uint64_t _xgetbv(uint_t xcr) BURGER_NOEXCEPT
{
	uint32_t eax, edx;
	// xgetbv instruction
	__asm__ volatile(".byte 0x0F, 0x01, 0xD0" : "=a"(eax), "=d"(edx) : "c"(xcr));
	return eax | (static_cast<uint64_t>(edx) << 32U);
}
#endif
#endif
