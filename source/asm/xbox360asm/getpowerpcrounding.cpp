/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	Burger::ePowerPCRounding BURGER_API Burger::get_PowerPC_rounding(void)
	r0, r3-12, fp0-fp13 are volatile
	Result in r3

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) Burger::ePowerPCRounding BURGER_API Burger::get_PowerPC_rounding(void)
{
__asm {

// Get the floating point control register
	mffs fp0

// Store it in memory
	stfd fp0, -8(r1)

// Load into an integer register
	lwz r3, -4(r1)

// Mask off the lowest 2 bits (AND & 3)
	clrlwi	r3, r3, 30

// Exit
	blr
	}
}
