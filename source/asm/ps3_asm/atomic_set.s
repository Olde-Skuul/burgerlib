#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# uint32_t BURGER_API Burger::atomic_set(volatile uint32_t* pOutput,
#	uint32_t uInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# uInput = r3/r4
# Result in r3
#

	.align 2

	.text

	.type	._ZN6Burger10atomic_setEPVjj,@function
	.globl	._ZN6Burger10atomic_setEPVjj
._ZN6Burger10atomic_setEPVjj:

# Perform an atomic read of pOutput (r3)
retry:
	lwarx	0,0,3

# Store uInput in the variable
	stwcx.	4,0,3

# Did the store "take", that is, it was atomic?
	bne-	retry

# Return the old value
	mr		3,0
	blr

	.size	._ZN6Burger10atomic_setEPVjj,.-._ZN6Burger10atomic_setEPVjj
