#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# double BURGER_API Burger::get_sign(double dInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# dInput = fp1
# Result in fp1
#

	.align 2

	.text

	.type	._ZN6Burger8get_signEd,@function
	.globl	._ZN6Burger8get_signEd
._ZN6Burger8get_signEd:

# Test for NaN first, to avoid stalls later
	fcmpu	cr1, 1, 1

# Get 0.5
	lis		3, _ZN6Burger7g_fHalfE@ha

# -fInput
	fneg	2, 1
	lfs		3, _ZN6Burger7g_fHalfE@l(3)

# -0.5
	fneg	4, 3

# Test twice, to also find 0.0f
	fsel	0, 1, 3, 4
	fsel	4, 2, 4, 3

# Yields 0.0f, 1.0f, or -1.0f
	fadd	1, 4, 0

# If not NaN, return now
	beqlr	cr1

# NaN detected, you are boned
# Store the -NaN
	stfd	2, -8(1)

# Assume result is 1.0f (0.5*2)
	fadd	1, 3, 3

# Test for negative with integer registers
	lwz		3, -8(1)
	andis.	3, 3, 0x8000

# Note: Testing on -NaN so not equal is positive
	bnelr

# Return negative
	fneg	1, 1
	blr

	.size	._ZN6Burger8get_signEd,.-._ZN6Burger8get_signEd
