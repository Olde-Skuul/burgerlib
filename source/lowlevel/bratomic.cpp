/***************************************

	Atomic memory

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "bratomic.h"
#include "brstringfunctions.h"

#if (defined(BURGER_POWERPC) && defined(BURGER_MACOSX))
#include <sys/sysctl.h>
#endif

#if (defined(BURGER_POWERPC) && defined(BURGER_MAC))
#include <Gestalt.h>
#endif

/*! ************************************

	\fn Word32 Burger::AtomicSwap(volatile Word32 *pOutput,Word32 uInput)
	\brief Atomically swap a 32 bit value for one in memory

	For multi-CPU machines, this function will atomically swap a value
	with one stored in memory using appropriate locking for all CPUs

	\param pOutput Pointer to a 32 bit aligned memory location swap with
	\param uInput 32 bit value to exchange with the memory location
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word32 Burger::AtomicPreIncrement(volatile Word32 *pInput)
	\brief Atomically pre-increment a 32 bit variable in memory

	For multi-CPU machines, this function will atomically increment a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value after the increment was applied.

	\param pInput Pointer to a 32 bit aligned memory location to increment
	\return Value that was stored in the variable previously plus one

***************************************/

/*! ************************************

	\fn Word32 Burger::AtomicPostIncrement(volatile Word32 *pInput)
	\brief Atomically post-increment a 32 bit variable in memory

	For multi-CPU machines, this function will atomically increment a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value before the increment was applied.

	\param pInput Pointer to a 32 bit aligned memory location to increment
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word32 Burger::AtomicPreDecrement(volatile Word32 *pInput)
	\brief Atomically pre-decrement a 32 bit variable in memory

	For multi-CPU machines, this function will atomically decrement a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value after the decrement was applied.

	\param pInput Pointer to a 32 bit aligned memory location to decrement
	\return Value that was stored in the variable previously minus one

***************************************/

/*! ************************************

	\fn Word32 Burger::AtomicPostDecrement(volatile Word32 *pInput)
	\brief Atomically post-decrement a 32 bit variable in memory

	For multi-CPU machines, this function will atomically decrement a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value before the decrement was applied.

	\param pInput Pointer to a 32 bit aligned memory location to decrement
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word32 Burger::AtomicAdd(volatile Word32 *pInput,Word32 uValue)
	\brief Atomically add a 32 bit value to a variable in memory

	For multi-CPU machines, this function will atomically add a value
	to one stored in memory using appropriate locking for all CPUs. It will return
	the value before the addition was applied.

	\param pInput Pointer to a 32 bit aligned memory location to add a value to
	\param uValue 32 bit value to add
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word32 Burger::AtomicSubtract(volatile Word32 *pInput,Word32 uValue)
	\brief Atomically subtract a 32 bit value from a variable in memory

	For multi-CPU machines, this function will atomically subtract a value
	from one stored in memory using appropriate locking for all CPUs. It will return
	the value before the subtraction was applied.

	\param pInput Pointer to a 32 bit aligned memory location to subtract a value from
	\param uValue 32 bit value to subtract
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word32 Burger::AtomicSetIfMatch(volatile Word32 *pInput,Word32 uBefore,Word32 uAfter)
	\brief Atomically set a 32 bit value from a variable in memory if it matches a requested value

	For multi-CPU machines, this function will atomically test a value
	with one stored in memory using appropriate locking for all CPUs. If the
	value is a match, a new value will be written to the memory value.
	It will return \ref TRUE if the swap was successful.

	\param pInput Pointer to a 32 bit aligned memory location to test and swap from
	\param uBefore 32 bit value to test the memory variable with
	\param uAfter 32 bit value to write into the memory variable if the test succeeded
	\return \ref TRUE if the exchange occurred, \ref FALSE if not

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicSwap(volatile Word64 *pOutput,Word64 uInput)
	\brief Atomically swap a 64 bit value for one in memory

	For multi-CPU machines, this function will atomically swap a value
	with one stored in memory using appropriate locking for all CPUs

	\note This function is only available on systems with a 64 bit CPU
	\param pOutput Pointer to a 64 bit aligned memory location swap with
	\param uInput 64 bit value to exchange with the memory location
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicPreIncrement(volatile Word64 *pInput)
	\brief Atomically pre-increment a 64 bit variable in memory

	For multi-CPU machines, this function will atomically increment a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value after the increment was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to increment
	\return Value that was stored in the variable previously plus one

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicPostIncrement(volatile Word64 *pInput)
	\brief Atomically post-increment a 64 bit variable in memory

	For multi-CPU machines, this function will atomically increment a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value before the increment was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to increment
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicPreDecrement(volatile Word64 *pInput)
	\brief Atomically pre-decrement a 64 bit variable in memory

	For multi-CPU machines, this function will atomically decrement a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value after the decrement was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to decrement
	\return Value that was stored in the variable previously minus one

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicPostDecrement(volatile Word64 *pInput)
	\brief Atomically post-decrement a 64 bit variable in memory

	For multi-CPU machines, this function will atomically decrement a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value before the decrement was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to decrement
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicAdd(volatile Word64 *pInput,Word64 uValue) 
	\brief Atomically add a 64 bit value to a variable in memory

	For multi-CPU machines, this function will atomically add a value
	to one stored in memory using appropriate locking for all CPUs. It will return
	the value before the addition was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to add a value to
	\param uValue 64 bit value to add
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicSubtract(volatile Word64 *pInput,Word64 uValue)
	\brief Atomically subtract a 64 bit value from a variable in memory

	For multi-CPU machines, this function will atomically subtract a value
	from one stored in memory using appropriate locking for all CPUs. It will return
	the value before the subtraction was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to subtract a value from
	\param uValue 64 bit value to subtract
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicSetIfMatch(volatile Word64 *pInput,Word64 uBefore,Word64 uAfter)
	\brief Atomically set a 64 bit value from a variable in memory if it matches a requested value

	For multi-CPU machines, this function will atomically test a value
	with one stored in memory using appropriate locking for all CPUs. If the
	value is a match, a new value will be written to the memory value.
	It will return \ref TRUE if the swap was successful.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to test and swap from
	\param uBefore 64 bit value to test the memory variable with
	\param uAfter 64 bit value to write into the memory variable if the test succeeded
	\return \ref TRUE if the exchange occurred, \ref FALSE if not

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
	\sa BURGER_INTELARCHITECTURE

***************************************/

#if defined(BURGER_X86) || defined(DOXYGEN)
#if defined(BURGER_GNUC)
__asm__(
"	.align	4,0x90\n"
"	.globl __ZN6Burger14IsCPUIDPresentEv\n"
"__ZN6Burger14IsCPUIDPresentEv:\n"
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
BURGER_DECLSPECNAKED Word BURGER_API Burger::IsCPUIDPresent(void)
{
	BURGER_ASM {

		// See of flags bit 21 can be changed.
		// If it can, it's higher than 486, which
		// implies that CPUID is available

		pushfd					// save flags on the stack
		mov		eax,dword ptr[esp]	// Get in register and leave on stack
		xor		eax,0x00200000	// Switch bit 21 for the test
		push	eax				// Set the flags with the new value
		popfd
		pushfd					// Read in the flags into a register
		pop		eax
		xor		eax,dword ptr[esp]	// Did the change "take"
		shr		eax,21			// Move to the lowest bit
		and		eax,1			// Set to TRUE or FALSE
		popfd					// Restore the flags to the original state
		ret
	}
}
#endif
#endif

/*! ************************************

	\struct Burger::CPUID_t
	\brief Structure that holds the features of the CPU

	On x86 and x64 CPUs, this structure will be filled by
	the use of the instruction CPUID on Intel
	architecture CPUs.

	\note This structure only matters on systems with an x86 or x64 CPU

	\sa void CPUID(CPUID_t *) or BURGER_INTELARCHITECTURE

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::CPUID_t::HasRTSC(void) const
	\brief Returns non-zero if the RTSC instruction is available

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instruction is available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTELARCHITECTURE

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::CPUID_t::HasCMOV(void) const
	\brief Returns non-zero if the CMOV instructions are available

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTELARCHITECTURE

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::CPUID_t::HasMMX(void) const
	\brief Returns non-zero if the MMX instructions are available

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTELARCHITECTURE

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::CPUID_t::HasSSE(void) const
	\brief Returns non-zero if SSE instructions are available

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTELARCHITECTURE

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::CPUID_t::HasSSE2(void) const
	\brief Returns non-zero if SSE2 instructions are available

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTELARCHITECTURE

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::CPUID_t::HasSSE3(void) const
	\brief Returns non-zero if SSE3 instructions are available

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTELARCHITECTURE

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::CPUID_t::HasSSSE3(void) const
	\brief Returns non-zero if SSSE3 instructions are available

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTELARCHITECTURE

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::CPUID_t::HasSSE41(void) const
	\brief Returns non-zero if SSE4.1 instructions are available

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTELARCHITECTURE

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::CPUID_t::HasSSE42(void) const
	\brief Returns non-zero if SSE4.2 instructions are available

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTELARCHITECTURE

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::CPUID_t::HasAVX(void) const
	\brief Returns non-zero if AVX instructions are available

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTELARCHITECTURE

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::CPUID_t::Has3DNOW(void) const
	\brief Returns non-zero if AMD 3DNow! instructions are available

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTELARCHITECTURE

***************************************/





/*! ************************************

	\brief Fill in a \ref CPUID_t structure

	On x86 and x64 CPUs, this function will call CPUID several
	times to extract the most commonly requested data
	from the CPU and fills in the CPUID_t structure. On
	non x86/x64 platforms, this function will only zero
	out the structure and immediately return.

	\note This function only matters on systems with an x86 or x64 CPU
	\param pOutput Pointer to a CPUID_t to initialize
	\sa BURGER_INTELARCHITECTURE

***************************************/

#if defined(BURGER_INTELARCHITECTURE) && !defined(DOXYGEN)

struct CPUHashes_t {
	char m_CPUName[16];		// CPU ID
	Burger::CPUID_t::eCPU m_uCPU;	// CPU type to map to
};

static const CPUHashes_t g_CPUHashes[] = {
	{"GenuineIntel",Burger::CPUID_t::CPU_INTEL},
	{"AMDisbetter!",Burger::CPUID_t::CPU_AMD},
	{"AuthenticAMD",Burger::CPUID_t::CPU_AMD},
	{"CentaurHauls",Burger::CPUID_t::CPU_CENTAUR},
	{"CyrixInstead",Burger::CPUID_t::CPU_CYRIX},
	{"TransmetaCPU",Burger::CPUID_t::CPU_TRANSMETA},
	{"GenuineTMx86",Burger::CPUID_t::CPU_TRANSMETA},
	{"Geode by NSC",Burger::CPUID_t::CPU_NATIONALSEMICONDUCTOR},
	{"NexGenDriven",Burger::CPUID_t::CPU_NEXGEN},
	{"RiseRiseRise",Burger::CPUID_t::CPU_RISE},
	{"SiS SiS SiS ",Burger::CPUID_t::CPU_SIS},
	{"UMC UMC UMC ",Burger::CPUID_t::CPU_UMC},
	{"VIA VIA VIA ",Burger::CPUID_t::CPU_VIA},
	{"Vortex86 SoC",Burger::CPUID_t::CPU_VORTEX}
};

//
// Lovely, due to some older CPUs not aware of the CPUID
// ranges, do some sanity checking to ensure that the
// ranges are actually good values and not the result
// of a request of data that the chip isn't aware of
//

static Word32 FixCount(Word32 uBase,int iValue)
{
	// Invalid if too small
	if (static_cast<Word32>(iValue)<=uBase) {
		iValue = 0;
 	} else {
		// Invalid if too big
		if ((static_cast<Word32>(iValue)-uBase)>1024) {
			iValue = 0;
		}
	}
	return static_cast<Word32>(iValue);
}

#endif

void BURGER_API Burger::CPUID(CPUID_t *pOutput)
{
	//
	// Clear out the result
	//
	MemoryClear(pOutput,sizeof(CPUID_t));

#if defined(BURGER_INTELARCHITECTURE)

	//
	// Buffer to get data from CPUID
	//
	
	int Results[4];

	//
	// Get the CPU name and highest allowable call
	//

	__cpuid(Results,0);

	//
	// Highest allowable value
	//

	Word32 uHighestID = FixCount(0,Results[0]);
	pOutput->m_uHighestCPUID = uHighestID;

	//
	// Convert to a string
	//
	int *pWork = static_cast<int *>(static_cast<void *>(pOutput->m_CPUName));
	pWork[0] = Results[1];
	pWork[1] = Results[3];
	pWork[2] = Results[2];
	pOutput->m_CPUName[12] = 0;

	//
	// Iterate over known CPU names to create the enumeration
	//

	WordPtr i = BURGER_ARRAYSIZE(g_CPUHashes);
	const CPUHashes_t *pHash = g_CPUHashes;
	pOutput->m_uCPUType = CPUID_t::CPU_UNKNOWN;
	do {
		if (!StringCompare(pHash->m_CPUName,pOutput->m_CPUName)) {
			pOutput->m_uCPUType = pHash->m_uCPU;
			break;
		}
	} while (--i);

	//
	// Extended info okay?
	//

	if (uHighestID) {
		__cpuid(Results,1);
		pOutput->m_uModel = static_cast<Word32>(Results[0]);
		pOutput->m_uBrand = static_cast<Word32>(Results[1]);
		pOutput->m_uExtendedFeatureInformation = static_cast<Word32>(Results[2]);
		pOutput->m_uFeatureInformation = static_cast<Word32>(Results[3]);
	}

	//
	// Is there extended information?
	//

	__cpuid(Results,0x80000000U);
	Word32 uExtendedID = FixCount(0x80000000U,Results[0]);
	pOutput->m_uHighestCPUIDExtended = uExtendedID;

	//
	// Get the extended information (AMD)
	//

	if (uExtendedID>=0x80000001U) {
		__cpuid(Results,0x80000001U);
		pOutput->m_uExtendedProcessorInformation = static_cast<Word32>(Results[3]);
		pOutput->m_uExtendedFeatureBits = static_cast<Word32>(Results[2]);

		//
		// Check if Processor Brand String is available
		// Opcodes 0x80000002-0x80000004 are needed
		//
		if (uExtendedID>=0x80000004U) {
			//
			// Pull down the string
			//
			i = 0;
			pWork = static_cast<int *>(static_cast<void *>(pOutput->m_BrandName));
			do {
				__cpuid(Results,static_cast<int>(0x80000002+i));
				pWork[0] = Results[0];
				pWork[1] = Results[1];
				pWork[2] = Results[2];
				pWork[3] = Results[3];
				pWork+=4;
			} while (++i<3);
			// Ensure it's zero terminated
			pOutput->m_BrandName[48] = 0;
		}
	}

	//
	// Is there hypervisor information?
	//

	__cpuid(Results,0x40000000U);
	Word32 uHyperVisorID = FixCount(0x40000000U,Results[0]);
	pOutput->m_uHighestCPUIDHyperVisor = uHyperVisorID;
	if (uHyperVisorID>=0x40000001U) {
		//
		// Get the hypervisor name and signature
		//

		pWork = static_cast<int *>(static_cast<void *>(pOutput->m_HypervisorName));
		pWork[0] = Results[1];
		pWork[1] = Results[2];
		pWork[2] = Results[3];
		pOutput->m_HypervisorName[12] = 0;
		__cpuid(Results,0x40000001U);
		pWork = static_cast<int *>(static_cast<void *>(pOutput->m_HypervisorSignature));
		pWork[0] = Results[0];
		pOutput->m_HypervisorSignature[4] = 0;
	}
#endif
}

/*! ************************************

	\brief Return \ref TRUE if the AltiVec instruction set is present

	On PowerPC CPUs, this function will perform a test to determine
	if the AltiVec instruction is present. It will return \ref TRUE
	if it does, and \ref FALSE if not.

	On XBox 360 and PS3 machines, this will always return \ref TRUE and on
	non PowerPC CPUs it will always return \ref FALSE.

	\note This function only matters on systems with an PowerPC CPU
	\return \ref TRUE if AltiVec is available, \ref FALSE if not
	\sa BURGER_POWERPC

***************************************/

#if (defined(BURGER_POWERPC) && defined(BURGER_MACOSX)) || defined(DOXYGEN)

Word BURGER_API Burger::HasAltiVec(void)
{
	int selector[2];
	int gHasAltivec;

	// Create a machine information block

	selector[0] = CTL_HW;
	selector[1] = HW_VECTORUNIT;
	size_t uLength = sizeof(gHasAltivec);
	int iError = sysctl(selector,2,&gHasAltivec,&uLength,NULL,0);	// Ask for Altivec
	Word uResult = FALSE;
	if (!iError) {
		uResult = static_cast<Word>(gHasAltivec);		// Found flag
	}
	return uResult;
}

#elif (defined(BURGER_POWERPC) && defined(BURGER_MAC))

Word BURGER_API Burger::HasAltiVec(void)
{
	// Carbon/Classic version

	long gestaltAnswer;
	Word uResult = 0;
	if (!Gestalt(gestaltPowerPCProcessorFeatures,&gestaltAnswer)) {
		// Move the flag to the lowest bit
		uResult = static_cast<Word>(gestaltAnswer>>gestaltPowerPCHasVectorInstructions)&1U;
	}
	return uResult;		// Return zero or non-zero
}

#endif

