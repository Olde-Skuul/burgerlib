#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# uint_t atomic_compare_and_set(volatile uint32_t* pOutput, uint32_t uOld,
#	uint32_t uNew) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# uInput = r3/r4/r5
# Result in r3
#

	.align 2

	.text

	.type	._ZN6Burger22atomic_compare_and_setEPVjjj,@function
	.globl	._ZN6Burger22atomic_compare_and_setEPVjjj
._ZN6Burger22atomic_compare_and_setEPVjjj:

# Perform an atomic read of pOutput (r3)
retry:
	lwarx	0,0,3

# Compare with uOld
	cmplw	0,4

# Exit if the value is not what was expected
	bne-	abort

# Store uNew in the variable
	stwcx.	5,0,3

# Did the store "take", that is, it was atomic?
	bne-	retry

abort:
# Create uTemp == uOld

# Zero if equal
	subf		0,0,4

# 32 if zero, less than 32 if non zero
	cntlzw		0,0

# Use the high bit, so 1 for zero, 0 for non zero
	srwi		3,0,5
	blr

	.size	._ZN6Burger22atomic_compare_and_setEPVjjj,.-._ZN6Burger22atomic_compare_and_setEPVjjj
