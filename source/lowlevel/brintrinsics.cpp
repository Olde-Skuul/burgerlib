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

	\fn _swapendian64(uint64_t uInput)
	\brief Swap endian of a 64 bit integer.

	64 bit operations for endian swap are specialized on different platforms
	since some forms require the return value to be in a structure.

	\param uInput 64 integer to swap endian.

	\return Input with all 8 bytes reversed.

***************************************/

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
