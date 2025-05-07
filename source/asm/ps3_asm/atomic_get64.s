#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# uint64_t BURGER_API Burger::atomic_get(
#	volatile uint64_t* pInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# uInput = r3
# Result in r3
#

	.align 2

	.text

	.type	._ZN6Burger10atomic_getEPVm,@function
	.globl	._ZN6Burger10atomic_getEPVm
._ZN6Burger10atomic_getEPVm:

# Perform an atomic read of pInput (r3)
retry:
	ldarx	0,0,3

# Store it back into the variable
	stdcx.	0,0,3

# Did the store "take", that is, it was atomic?
	bne-	retry

# Return the loaded value
	mr		3,0
	blr

	.size	._ZN6Burger10atomic_getEPVm,.-._ZN6Burger10atomic_getEPVm
