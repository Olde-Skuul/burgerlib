#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# ePowerPCRounding BURGER_API Burger::get_PowerPC_rounding(void) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# Result in r3
#

	.align 2

	.text

	.type	._ZN6Burger20get_PowerPC_roundingEv,@function
	.globl	._ZN6Burger20get_PowerPC_roundingEv
._ZN6Burger20get_PowerPC_roundingEv:

# Get the floating point control register
	mffs	0

# Store it in memory
	stfd	0, -8(1)

# Load into an integer register
	lwz		3, -4(1)

# Mask off the lowest 2 bits (AND & 3)
	clrlwi	3, 3, 30
	blr

	.size	._ZN6Burger20get_PowerPC_roundingEv,.-._ZN6Burger20get_PowerPC_roundingEv
