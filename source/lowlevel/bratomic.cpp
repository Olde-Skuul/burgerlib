/***************************************

    Atomic memory

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "bratomic.h"
#include "brmemoryfunctions.h"
#include "brstringfunctions.h"

// Used for MacOSX CPU tests
#if (defined(BURGER_PPC) && defined(BURGER_MACOSX))
#include <sys/sysctl.h>
#endif

// Gestalt for MacOS
#if defined(BURGER_MAC)
#include <Gestalt.h>
#endif

#if defined(BURGER_MSVC)

// Perform an intrinsic test (Should not cause compile errors)
#include "brvisualstudio.h"

#if defined(BURGER_WINDOWS)
#if (defined(_MSC_VER) && (_MSC_VER<1400))
#include <mmintrin.h>
#else
#include <intrin.h>
#endif
#elif defined(BURGER_XBOX360)
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#endif

#endif

//
// If MacOSX was build with an earlier version of GNU C than 4.0, the atomic
// intrinsics are missing. Use the OSAtomic.h functions in that case.
//

#if ((__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) <= 40100) && defined(BURGER_MACOSX)
#include <libkern/OSAtomic.h>
#endif

#if (((__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) <= 40100) && defined(BURGER_MACOSX)) || defined(DOXYGEN)

/*! ************************************

	\brief Atomically swap a 32 bit value for one in memory

	For multi-CPU machines, this function will atomically swap a value
	with one stored in memory using appropriate locking for all CPUs

	\param pOutput Pointer to a 32 bit aligned memory location swap with
	\param uInput 32 bit value to exchange with the memory location
	\return Value that was stored in the variable previously

***************************************/

uint32_t BURGER_API Burger::AtomicSwap(volatile uint32_t *pOutput,uint32_t uInput)
{
	uint32_t uOldValue;
	do {
		uOldValue = pOutput[0];
	} while (!OSAtomicCompareAndSwap32Barrier(static_cast<int32_t>(uOldValue),static_cast<int32_t>(uInput),
		static_cast<volatile int32_t *>(static_cast<volatile void *>(pOutput))));
	return uOldValue;
}

/*! ************************************

	\brief Atomically pre-increment a 32 bit variable in memory

	For multi-CPU machines, this function will atomically increment a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value after the increment was applied.

	\param pInput Pointer to a 32 bit aligned memory location to increment
	\return Value that was stored in the variable previously plus one

***************************************/

uint32_t BURGER_API Burger::AtomicPreIncrement(volatile uint32_t *pInput) BURGER_NOEXCEPT
{
	return static_cast<uint32_t>(OSAtomicIncrement32Barrier(static_cast<volatile int32_t *>(static_cast<volatile void *>(pInput))));
}

/*! ************************************

	\brief Atomically post-increment a 32 bit variable in memory

	For multi-CPU machines, this function will atomically increment a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value before the increment was applied.

	\param pInput Pointer to a 32 bit aligned memory location to increment
	\return Value that was stored in the variable previously

***************************************/

uint32_t BURGER_API Burger::AtomicPostIncrement(volatile uint32_t *pInput)
{
	return static_cast<uint32_t>(OSAtomicIncrement32Barrier(static_cast<volatile int32_t *>(static_cast<volatile void *>(pInput)))-1);
}

/*! ************************************

	\brief Atomically pre-decrement a 32 bit variable in memory

	For multi-CPU machines, this function will atomically decrement a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value after the decrement was applied.

	\param pInput Pointer to a 32 bit aligned memory location to decrement
	\return Value that was stored in the variable previously minus one

***************************************/

uint32_t BURGER_API Burger::AtomicPreDecrement(volatile uint32_t *pInput) BURGER_NOEXCEPT
{
	return static_cast<uint32_t>(OSAtomicDecrement32Barrier(static_cast<volatile int32_t *>(static_cast<volatile void *>(pInput))));
}

/*! ************************************

	\brief Atomically post-decrement a 32 bit variable in memory

	For multi-CPU machines, this function will atomically decrement a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value before the decrement was applied.

	\param pInput Pointer to a 32 bit aligned memory location to decrement
	\return Value that was stored in the variable previously

***************************************/

uint32_t BURGER_API Burger::AtomicPostDecrement(volatile uint32_t *pInput)
{
	return static_cast<uint32_t>(OSAtomicDecrement32Barrier(static_cast<volatile int32_t *>(static_cast<volatile void *>(pInput)))+1);
}

/*! ************************************

	\brief Atomically add a 32 bit value to a variable in memory

	For multi-CPU machines, this function will atomically add a value
	to one stored in memory using appropriate locking for all CPUs. It will return
	the value before the addition was applied.

	\param pInput Pointer to a 32 bit aligned memory location to add a value to
	\param uValue 32 bit value to add
	\return Value that was stored in the variable previously

***************************************/

uint32_t BURGER_API Burger::AtomicAdd(volatile uint32_t *pInput,uint32_t uValue)
{
	return static_cast<uint32_t>(OSAtomicAdd32Barrier(static_cast<int32_t>(uValue),static_cast<volatile int32_t *>(static_cast<volatile void *>(pInput))));
}

/*! ************************************

	\brief Atomically subtract a 32 bit value from a variable in memory

	For multi-CPU machines, this function will atomically subtract a value
	from one stored in memory using appropriate locking for all CPUs. It will return
	the value before the subtraction was applied.

	\param pInput Pointer to a 32 bit aligned memory location to subtract a value from
	\param uValue 32 bit value to subtract
	\return Value that was stored in the variable previously

***************************************/

uint32_t BURGER_API Burger::AtomicSubtract(volatile uint32_t *pInput,uint32_t uValue)
{
	return static_cast<uint32_t>(OSAtomicAdd32Barrier(-static_cast<int32_t>(uValue),static_cast<volatile int32_t *>(static_cast<volatile void *>(pInput))));
}

/*! ************************************

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

uint_t BURGER_API Burger::AtomicSetIfMatch(volatile uint32_t *pInput,uint32_t uBefore,uint32_t uAfter)
{
	return OSAtomicCompareAndSwap32Barrier(static_cast<int32_t>(uBefore),static_cast<int32_t>(uAfter),
		static_cast<volatile int32_t *>(static_cast<volatile void *>(pInput)));
}

/*! ************************************

	\brief Atomically swap a 64 bit value for one in memory

	For multi-CPU machines, this function will atomically swap a value
	with one stored in memory using appropriate locking for all CPUs

	\note This function is only available on systems with a 64 bit CPU
	\param pOutput Pointer to a 64 bit aligned memory location swap with
	\param uInput 64 bit value to exchange with the memory location
	\return Value that was stored in the variable previously

***************************************/

#if defined(BURGER_64BITCPU) || defined(DOXYGEN)
uint64_t BURGER_API Burger::AtomicSwap(volatile uint64_t *pOutput,uint64_t uInput)
{
	uint64_t uOldValue;
	do {
		uOldValue = pOutput[0];
	} while (!OSAtomicCompareAndSwap64Barrier(static_cast<int64_t>(uOldValue),static_cast<int64_t>(uInput),
		static_cast<volatile int64_t *>(static_cast<volatile void *>(pOutput))));
	return uOldValue;
}

/*! ************************************

	\brief Atomically pre-increment a 64 bit variable in memory

	For multi-CPU machines, this function will atomically increment a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value after the increment was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to increment
	\return Value that was stored in the variable previously plus one

***************************************/

uint64_t BURGER_API Burger::AtomicPreIncrement(volatile uint64_t *pInput) BURGER_NOEXCEPT
{
	return static_cast<uint64_t>(OSAtomicIncrement64Barrier(static_cast<volatile int64_t *>(static_cast<volatile void *>(pInput))));
}

/*! ************************************

	\brief Atomically post-increment a 64 bit variable in memory

	For multi-CPU machines, this function will atomically increment a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value before the increment was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to increment
	\return Value that was stored in the variable previously

***************************************/

uint64_t BURGER_API Burger::AtomicPostIncrement(volatile uint64_t *pInput)
{
	return static_cast<uint64_t>(OSAtomicIncrement64Barrier(static_cast<volatile int64_t *>(static_cast<volatile void *>(pInput)))-1);
}

/*! ************************************

	\brief Atomically pre-decrement a 64 bit variable in memory

	For multi-CPU machines, this function will atomically decrement a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value after the decrement was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to decrement
	\return Value that was stored in the variable previously minus one

***************************************/

uint64_t BURGER_API Burger::AtomicPreDecrement(volatile uint64_t *pInput) BURGER_NOEXCEPT
{
	return static_cast<uint64_t>(OSAtomicDecrement64Barrier(static_cast<volatile int64_t *>(static_cast<volatile void *>(pInput))));
}

/*! ************************************

	\brief Atomically post-decrement a 64 bit variable in memory

	For multi-CPU machines, this function will atomically decrement a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value before the decrement was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to decrement
	\return Value that was stored in the variable previously

***************************************/

uint64_t BURGER_API Burger::AtomicPostDecrement(volatile uint64_t *pInput)
{
	return static_cast<uint64_t>(OSAtomicDecrement64Barrier(static_cast<volatile int64_t *>(static_cast<volatile void *>(pInput)))+1);
}

/*! ************************************

	\brief Atomically add a 64 bit value to a variable in memory

	For multi-CPU machines, this function will atomically add a value
	to one stored in memory using appropriate locking for all CPUs. It will return
	the value before the addition was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to add a value to
	\param uValue 64 bit value to add
	\return Value that was stored in the variable previously

***************************************/

uint64_t BURGER_API Burger::AtomicAdd(volatile uint64_t *pInput,uint64_t uValue)
{
	return static_cast<uint64_t>(OSAtomicAdd64Barrier(static_cast<int64_t>(uValue),static_cast<volatile int64_t *>(static_cast<volatile void *>(pInput))));
}

/*! ************************************

	\brief Atomically subtract a 64 bit value from a variable in memory

	For multi-CPU machines, this function will atomically subtract a value
	from one stored in memory using appropriate locking for all CPUs. It will return
	the value before the subtraction was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to subtract a value from
	\param uValue 64 bit value to subtract
	\return Value that was stored in the variable previously

***************************************/

uint64_t BURGER_API Burger::AtomicSubtract(volatile uint64_t *pInput,uint64_t uValue)
{
	return static_cast<uint64_t>(OSAtomicAdd64Barrier(-static_cast<int64_t>(uValue),static_cast<volatile int64_t *>(static_cast<volatile void *>(pInput))));
}

/*! ************************************

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

uint_t BURGER_API Burger::AtomicSetIfMatch(volatile uint64_t *pInput,uint64_t uBefore,uint64_t uAfter)
{
	return OSAtomicCompareAndSwap64Barrier(static_cast<int64_t>(uBefore),static_cast<int64_t>(uAfter),
		static_cast<volatile int64_t *>(static_cast<volatile void *>(pInput)));
}

#endif
#endif

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

#if (defined(BURGER_X86) && !defined(BURGER_XBOX)) || defined(DOXYGEN)

#if defined(BURGER_GNUC) || defined(BURGER_CLANG)
// clang-format off
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

BURGER_DECLSPECNAKED uint_t BURGER_API Burger::IsCPUIDPresent(void) BURGER_NOEXCEPT
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

	\struct Burger::CPUID_t
	\brief Structure that holds the features of the CPU

	On x86 and x64 CPUs, this structure will be filled by
	the use of the instruction CPUID on Intel
	architecture CPUs.

	\note This structure only matters on systems with an x86 or x64 CPU

	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasRTSC(void) const
	\brief Returns non-zero if the RTSC instruction is available

	https://en.wikipedia.org/wiki/Time_Stamp_Counter

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instruction is available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasCMOV(void) const
	\brief Returns non-zero if the CMOV instructions are available

	https://en.wikipedia.org/wiki/FCMOV

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasMMX(void) const
	\brief Returns non-zero if the MMX instructions are available

	https://en.wikipedia.org/wiki/MMX_(instruction_set)

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasSSE(void) const
	\brief Returns non-zero if SSE instructions are available

	https://en.wikipedia.org/wiki/Streaming_SIMD_Extensions

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasSSE2(void) const
	\brief Returns non-zero if SSE2 instructions are available

	https://en.wikipedia.org/wiki/SSE2

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasSSE3(void) const
	\brief Returns non-zero if SSE3 instructions are available

	https://en.wikipedia.org/wiki/SSE3

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasSSSE3(void) const
	\brief Returns non-zero if SSSE3 instructions are available

	https://en.wikipedia.org/wiki/SSSE3

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasSSE4a(void) const
	\brief Returns non-zero if HasSSE4a instructions are available

	https://en.wikipedia.org/wiki/SSE4#SSE4a

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasSSE41(void) const
	\brief Returns non-zero if SSE4.1 instructions are available

	https://en.wikipedia.org/wiki/SSE4#SSE4.1

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasSSE42(void) const
	\brief Returns non-zero if SSE4.2 instructions are available

	https://en.wikipedia.org/wiki/SSE4#SSE4.2

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasMOVBE(void) const
	\brief Returns non-zero if the MOVBE instruction is available

	https://www.tptp.cc/mirrors/siyobik.info/instruction/MOVBE.html

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instruction is available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasAES(void) const
	\brief Returns non-zero if AES instructions are available

	https://en.wikipedia.org/wiki/AES_instruction_set

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasAVX(void) const
	\brief Returns non-zero if AVX instructions are available

	https://en.wikipedia.org/wiki/Advanced_Vector_Extensions

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasCMPXCHG16B(void) const
	\brief Returns non-zero if the CMPXCHG16B instruction is available

	http://www.felixcloutier.com/x86/CMPXCHG8B:CMPXCHG16B.html

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the CMPXCHG16B instruction is available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasF16C(void) const
	\brief Returns non-zero if the F16C data type is supported

	https://en.wikipedia.org/wiki/F16C

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the F16C data type is supported, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasFMA3(void) const
	\brief Returns non-zero if FMA3 instructions are available

	https://en.wikipedia.org/wiki/FMA_instruction_set

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasFMA4(void) const
	\brief Returns non-zero if FMA4 instructions are available

	https://en.wikipedia.org/wiki/FMA_instruction_set#FMA4_instruction_set

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasLAHFSAHF(void) const
	\brief Returns non-zero if LAHF and SAHF instructions support long mode

	http://www.tptp.cc/mirrors/siyobik.info/instruction/LAHF.html

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasPrefetchW(void) const
	\brief Returns non-zero if PrefetchW instructions are available

	http://www.felixcloutier.com/x86/PREFETCHW.html

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::Has3DNOW(void) const
	\brief Returns non-zero if AMD 3DNow! instructions are available

	https://en.wikipedia.org/wiki/3DNow!

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

***************************************/

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::CPUID_t::HasExtended3DNOW(void) const
	\brief Returns non-zero if extended AMD 3DNow! instructions are available

	https://en.wikipedia.org/wiki/3DNow!#3DNow_extensions

	\note This structure only matters on systems with an x86 or x64 CPU

	\return Non-zero if the instructions are available, zero if not.
	\sa void CPUID(CPUID_t *) or BURGER_INTEL

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
	\sa BURGER_INTEL

***************************************/

#if defined(BURGER_INTEL) && !defined(DOXYGEN)

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
	{"Vortex86 SoC",Burger::CPUID_t::CPU_VORTEX},
	{"KVMKVMKVM   ",Burger::CPUID_t::CPU_KVM},
	{"Microsoft Hv",Burger::CPUID_t::CPU_MICROSOFT_VIRTUAL_PC},
	{" lrpepyh  vr",Burger::CPUID_t::CPU_PARALLELS},
	{"prl hyperv  ",Burger::CPUID_t::CPU_PARALLELS},
	{"VMwareVMware",Burger::CPUID_t::CPU_VMWARE},
	{"XenVMMXenVMM",Burger::CPUID_t::CPU_XEN}
};

//
// Lovely, due to some older CPUs not aware of the CPUID
// ranges, do some sanity checking to ensure that the
// ranges are actually good values and not the result
// of a request of data that the chip isn't aware of
//

static uint32_t BURGER_API FixCount(uint32_t uBase,int iValue) BURGER_NOEXCEPT
{
	// Invalid if too small
	if (static_cast<uint32_t>(iValue)<=uBase) {
		iValue = 0;
 	} else {
		// Invalid if too big
		if ((static_cast<uint32_t>(iValue)-uBase)>1024) {
			iValue = 0;
		}
	}
	return static_cast<uint32_t>(iValue);
}

#endif

void BURGER_API Burger::CPUID(CPUID_t *pOutput) BURGER_NOEXCEPT
{
	//
	// Clear out the result
	//
	MemoryClear(pOutput,sizeof(CPUID_t));
	pOutput->m_uCPUType = CPUID_t::CPU_UNKNOWN;

#if defined(BURGER_INTEL)

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

	uint32_t uHighestID = FixCount(0,Results[0]);
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

	uintptr_t i = BURGER_ARRAYSIZE(g_CPUHashes);
	const CPUHashes_t *pHash = g_CPUHashes;
	do {
		if (!StringCompare(pHash->m_CPUName,pOutput->m_CPUName)) {
			pOutput->m_uCPUType = pHash->m_uCPU;
			break;
		}
        ++pHash;
	} while (--i);

	//
	// Extended info okay?
	//

	if (uHighestID) {
		__cpuid(Results,1);
		pOutput->m_uModel = static_cast<uint32_t>(Results[0]);
		pOutput->m_uBrand = static_cast<uint32_t>(Results[1]);
		pOutput->m_uCPUID1ECX = static_cast<uint32_t>(Results[2]);
		pOutput->m_uCPUID1EDX = static_cast<uint32_t>(Results[3]);

		// Even more features?
		if (uHighestID>=7) {
#if defined(BURGER_MSVC) && (_MSC_VER<1500)
			__cpuid(Results,7);
#else
			__cpuidex(Results,7,0);
#endif
			pOutput->m_uCPUID7EBX = static_cast<uint32_t>(Results[1]);
			pOutput->m_uCPUID7ECX = static_cast<uint32_t>(Results[2]);
			pOutput->m_uCPUID7EDX = static_cast<uint32_t>(Results[3]);
		}
	}

	//
	// Is there extended information?
	//

	__cpuid(Results,0x80000000U);
	uint32_t uExtendedID = FixCount(0x80000000U,Results[0]);
	pOutput->m_uHighestCPUIDExtended = uExtendedID;

	//
	// Get the extended information (AMD)
	//

	if (uExtendedID>=0x80000001U) {
		__cpuid(Results,0x80000001U);
		pOutput->m_uCPUID80000001ECX = static_cast<uint32_t>(Results[2]);
		pOutput->m_uCPUID80000001EDX = static_cast<uint32_t>(Results[3]);

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
	uint32_t uHyperVisorID = FixCount(0x40000000U,Results[0]);
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
	\sa BURGER_PPC

***************************************/

#if (defined(BURGER_PPC) && defined(BURGER_MACOSX)) || defined(DOXYGEN)

uint_t BURGER_API Burger::HasAltiVec(void) BURGER_NOEXCEPT
{
	int selector[2];
	int gHasAltivec;

	// Create a machine information block

	selector[0] = CTL_HW;
	selector[1] = HW_VECTORUNIT;
	size_t uLength = sizeof(gHasAltivec);
	int iError = sysctl(selector,2,&gHasAltivec,&uLength,NULL,0);	// Ask for Altivec
	uint_t uResult = FALSE;
	if (!iError) {
		uResult = static_cast<uint_t>(gHasAltivec);		// Found flag
	}
	return uResult;
}

#elif (defined(BURGER_PPC) && defined(BURGER_MAC))

uint_t BURGER_API Burger::HasAltiVec(void) BURGER_NOEXCEPT
{
	// Carbon/Classic version

	long gestaltAnswer;
	uint_t uResult = 0;
	if (!Gestalt(gestaltPowerPCProcessorFeatures,&gestaltAnswer)) {
		// Move the flag to the lowest bit
		uResult = static_cast<uint_t>(gestaltAnswer>>gestaltPowerPCHasVectorInstructions)&1U;
	}
	return uResult;		// Return zero or non-zero
}

#endif

/*! ************************************

	\brief Return \ref TRUE if floating point sqrt is supported in hardware

	Most modern CPUs have floating point in hardware, and on most platforms,
	this function will return \ref TRUE. On 680x0 and PowerPC platforms, this function will
	test for the existance of an instruction to perform a floating point square root
	and will return non-zero if one is found.

	\return Non-zero if fsqrt() is available, \ref FALSE if not.
	\sa BURGER_68K and BURGER_PPC

***************************************/

#if (defined(BURGER_PPC) && defined(BURGER_MACOSX)) || defined(DOXYGEN)

uint_t BURGER_API Burger::HasFSqrt(void) BURGER_NOEXCEPT
{
    size_t uLength = 4;
    uint32_t uBuffer = 0;

    // Call the OS to get the flag for the fsqrt instruction
    long lResult = sysctlbyname("hw.optional.fsqrt", &uBuffer, &uLength, nullptr, 0);
    return (!lResult && uBuffer) ? TRUE : FALSE;
}


#elif (defined(BURGER_PPC) && defined(BURGER_MAC))

uint_t BURGER_API Burger::HasFSqrt(void) BURGER_NOEXCEPT
{
	// Carbon/Classic version

	long gestaltAnswer;
	uint_t uResult = 0;
	if (!Gestalt(gestaltPowerPCProcessorFeatures,&gestaltAnswer)) {
		// Return the FPU type
		uResult = static_cast<uint_t>(gestaltAnswer>>gestaltPowerPCHasSquareRootInstructions)&1;
	}
	return uResult;		// Return zero or non-zero
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

uint_t BURGER_API Burger::HasFPU(void) BURGER_NOEXCEPT
{
	// Carbon/Classic version

	long gestaltAnswer;
	uint_t uResult = 0;
	if (!Gestalt(gestaltFPUType,&gestaltAnswer)) {
		// Return the FPU type
		uResult = static_cast<uint_t>(gestaltAnswer);
	}
	return uResult;		// Return zero or non-zero
}

#endif
