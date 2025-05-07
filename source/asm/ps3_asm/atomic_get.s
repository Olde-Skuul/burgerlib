#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# uint32_t BURGER_API Burger::atomic_get(
#	volatile uint32_t* pInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# uInput = r3
# Result in r3
#

	.align 2

	.text

	.type	._ZN6Burger10atomic_getEPVj,@function
	.globl	._ZN6Burger10atomic_getEPVj
._ZN6Burger10atomic_getEPVj:

# Perform an atomic read of pInput (r3)
retry:
	lwarx	0,0,3

# Store it back into the variable
	stwcx.	0,0,3

# Did the store "take", that is, it was atomic?
	bne-	retry

# Return the loaded value
	mr		3,0
	blr

	.size	._ZN6Burger10atomic_getEPVj,.-._ZN6Burger10atomic_getEPVj
