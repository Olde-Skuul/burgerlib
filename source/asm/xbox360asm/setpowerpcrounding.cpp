/***************************************

	PowerPC 64 assembly for Xbox 360

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the masm.exe for PowerPC

	Burger::ePowerPCRounding BURGER_API Burger::set_PowerPC_rounding(ePowerPCRounding uInput)
	r0, r3-12, fp0-fp13 are volatile
	Result in r3
	uInput = r3

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) Burger::ePowerPCRounding BURGER_API
	Burger::set_PowerPC_rounding(ePowerPCRounding /* uInput */)
{
	__asm {
// Get the floating point control register
	mffs fp0

// Store it in memory
	stfd fp0, -8(r1)

// Load into an integer register
	lwz r4, -4(r1)

// Get the return value, mask off the lowest 2 bits (AND & 3)
	extrwi r5, r4, 2, 30

// Move the lowest 2 bits into the existing value
	rlwimi r4, r3, 0, 30, 31

// Store it in memory
	stw r4, -4(r1)

// Reload the fp register
	lfd fp0, -8(r1)

// Set the flags
	mtfsf 255, fp0

// Return the previous value
	mr r3, r5

// Exit
	blr
	}
}
