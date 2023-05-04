/***************************************

	MacOS specific timer code

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "mac_timer.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)

#if defined(BURGER_PPC) || defined(DOXYGEN)

/*! ************************************

	\fn Burger::MacOS::PollRTC601(void)
	\brief Poll the PowerPC 601 Real Time Clock register

	Poll the PowerPC 601 Real Time Clock register and debounce the value. Once
	debounced, return the 64 bit value.

	Done in assembly, because that's how you know it's done right

	\maconly

	\return 64 bit Real Time Clock register

***************************************/

// clang-format off
asm uint64_t Burger::MacOS::PollRTC601(void) BURGER_NOEXCEPT
{
	machine 601
@XLoop:
	mfrtcu	r3			// RTCU (High)
	mfrtcl	r4			// RTCL (Low)
	mfrtcu	r5			// Did the upper 32 bits change?
	cmpw	r3,r5		// Match?
	bne		@XLoop		// Try again
	blr					// Exit (Result in r3,r4)
}
// clang-format on

/*! ************************************

	\fn Burger::MacOS::PollTBR603(void)
	\brief Poll the PowerPC 603 timer register

	Poll the PowerPC 603 Time Base Register and debounce the value. Once
	debounced, return the 64 bit value.

	\maconly

	\return 64 bit Time Base Register

***************************************/

// clang-format off
asm uint64_t Burger::MacOS::PollTBR603(void) BURGER_NOEXCEPT
{
	machine 604
@XLoop2:
	mftbu	r3			// Upper 32 bits
	mftb	r4 			// Lower 32 bits
	mftbu	r5			// Did the upper 32 bits change?
	cmpw	r3,r5		// Match?
	bne		@XLoop2		// Try again
	blr					// Exit (Result in r3, r4)
}
// clang-format off

#endif
#endif