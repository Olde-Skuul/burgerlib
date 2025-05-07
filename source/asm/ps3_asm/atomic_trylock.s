#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# uint_t BURGER_API Burger::atomic_trylock(
#	volatile uint32_t* pInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# uInput = r3
# Result in r3
#

	.align 2

	.text

	.type	._ZN6Burger14atomic_trylockEPVj,@function
	.globl	._ZN6Burger14atomic_trylockEPVj
._ZN6Burger14atomic_trylockEPVj:

# Perform an atomic read of pInput (r3)
retry:
	lwarx	4,0,3

# Check if it is already locked
	cmpwi	4,0
	
# In delay slot, load 1
	li		0,1
	bne-	abort

# Store 1 in the variable
	stwcx.	0,0,3

# Did the store "take", that is, it was atomic?
	bne-	retry

# Return true if the lock was acquired
abort:
	cntlzw	0,4

# If zero, return 1, else 0 (32 = 1, all else = 0)
	srwi	3,0,5
	blr

	.size	._ZN6Burger14atomic_trylockEPVj,.-._ZN6Burger14atomic_trylockEPVj
