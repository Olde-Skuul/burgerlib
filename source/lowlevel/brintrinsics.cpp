/***************************************

	Compiler intrinsics.

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brintrinsics.h"
#include "brvectortypes.h"

/*! ************************************

	\fn _xgetbv(uint_t xcr) BURGER_NOEXCEPT
	\brief Intrinsic _xgetbv for Intel CPUs

	This intrinsic is included in most modern "C" compilers that target Intel
	CPUs. For some older compilers, the intrinsic is implemented in Burgerlib
	to ensure it's always available.

	\note Only available on Intel platforms, however test if the instruction is
		available.

	\param xcr Value for the ECX register

	\return Value returned by the xgetbv instruction

***************************************/

#if defined(DOXYGEN)
extern "C" uint64_t _xgetbv(uint_t xcr) BURGER_NOEXCEPT;
#endif

#if defined(BURGER_INTEL)

#if defined(BURGER_LINUX) || (defined(BURGER_CLANG) && defined(__XSAVE__)) || \
	__has_builtin(_xgetbv)

#elif defined(BURGER_CLANG) || defined(BURGER_GNUC)
uint64_t _xgetbv(uint_t xcr) BURGER_NOEXCEPT
{
	uint32_t eax, edx;
	// xgetbv instruction
	__asm__ volatile(".byte 0x0F, 0x01, 0xD0"
					 : "=a"(eax), "=d"(edx)
					 : "c"(xcr));
	return eax | (static_cast<uint64_t>(edx) << 32U);
}
#endif

#endif

/*! ************************************

	\def BURGER_MEMORYBARRIER
	\brief Invoke a memory barrier with the compiled code.

	When writing multi-threaded code, sometimes it's necessary to instruct the
	compiler to not reorder read/write operations to ensure values are read or
	written as the code is listed on the source code.

	Depending on compiler, this is a define or a function call.

	In most cases, this generates no code, only an instruction to the compiler.

	\note Visual Studio maps to _ReadWriteBarrier()

***************************************/

/*! ************************************

	\def BURGER_PAUSEOPCODE
	\brief Insert a yield instruction.

	On modern CPUs, a yield instruction exists that will cause the current
	thread to pause until an interrupt occurs. This is a quick way to get a
	thread to sleep and instantly restart to test for a mutex or other thread
	blocker.

	Depending on compiler, this is a define or a function call.

	On older CPUs, or systems that do not have threading, this does no
	operation. An example are 680x0 systems

***************************************/

#if 0
extern void totally_bogus(void);
void totally_bogus(void)
{
	// Only enable for compilation testing, don't enable for final builds
	BURGER_MEMORYBARRIER();
	BURGER_PAUSEOPCODE();
}

#endif
