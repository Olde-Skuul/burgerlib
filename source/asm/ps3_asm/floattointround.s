#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# int32_t BURGER_API Burger::float_to_int_round(float fInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# fInput = fp1
# Result in r3
#

	.align 2

	.text

	.type	._ZN6Burger18float_to_int_roundEf,@function
	.globl	._ZN6Burger18float_to_int_roundEf
._ZN6Burger18float_to_int_roundEf:

# Get the address of g_fMinNoInteger
	lis		3, _ZN6Burger15g_fMinNoIntegerE@ha

# Get the abs value to test
	fabs	0, 1

# Load 8388608.0f
	lfs		2, _ZN6Burger15g_fMinNoIntegerE@l(3)

# Pointer to 0.5f
	lis		3, _ZN6Burger7g_fHalfE@ha

# Push the positive number to highest value without fraction
# (Removes fraction)
	fadds	5, 0, 2

# Load 0.5f
	lfs		3, _ZN6Burger7g_fHalfE@l(3)

# Undo the push (Fraction is gone)
	fsubs	5, 5, 2

# Subtract original from rounded to get the fraction
	fsubs	6, 0, 5

# Test against 0.5
	fsubs	6, 6, 3

# Set to one
	fadds	3, 3, 3

# Set to zero
	fsubs	4, 5, 5

# Set to zero or one depending on the test
	fsel	0, 6, 3, 4

# Add 0 for no rounding, 1 for round up
	fadds	5, 5, 0

# Get the negative value
	fnabs	2, 5

# Which one to use? Positive or negative?
	fsel	1, 1, 5, 2

# Convert to integer
	fctiwz	1, 1

# Pointer to temp on the stack
	subi	3, 1, 8

# Store the integer
	stfiwx	1, 0, 3

# Return the integer
	lwa		3, 0(3)
	blr

	.size	._ZN6Burger18float_to_int_roundEf,.-._ZN6Burger18float_to_int_roundEf
