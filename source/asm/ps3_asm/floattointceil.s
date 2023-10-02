#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# int32_t BURGER_API Burger::float_to_int_ceil(float fInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# fInput = fp1
# Result in r3
#

	.align 2

	.text

	.type	._ZN6Burger17float_to_int_ceilEf,@function
	.globl	._ZN6Burger17float_to_int_ceilEf
._ZN6Burger17float_to_int_ceilEf:

# Get the address of g_fMinNoInteger
	lis		3, _ZN6Burger15g_fMinNoIntegerE@ha

# Save the rounding register
	mffs	4

# Load 8388608.0f
	lfs		2, _ZN6Burger15g_fMinNoIntegerE@l(3)

# Set bits 30 and 31 to Round toward +infinity
	mtfsfi	7, 0x02

# Push the positive number to highest value without fraction
#(Removes fraction)
	fadds	3, 1, 2

# Push the negative number to the lowest value without fraction
# (Removes fraction)
	fsubs	0, 1, 2

# Undo the push (Fraction is gone)
	fsubs	3, 3, 2

# Undo the push (Fraction is gone)
	fadds	0, 0, 2

# Which one to use? Positive or negative?
	fsel	1, 1, 3, 0

# Restore rounding
	mtfsf	255, 4

# Convert to integer
	fctiwz	1, 1

# Pointer to temp on the stack
	subi	3, 1, 8

# Store the integer
	stfiwx	1, 0, 3

# Return the integer
	lwa		3, 0(3)
	blr

	.size	._ZN6Burger17float_to_int_ceilEf,.-._ZN6Burger17float_to_int_ceilEf
