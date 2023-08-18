#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# double BURGER_API Burger::get_round(double dInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# dInput = fp1
# Result in fp1
#

	.align 2

	.text

	.type	._ZN6Burger9get_roundEd,@function
	.globl	._ZN6Burger9get_roundEd
._ZN6Burger9get_roundEd:

# fAbs = absolute(fInput)
	fabs	5, 1

# Prep for loading 0.5f
	lis		3, _ZN6Burger7g_fHalfE@ha

# Convert the absolute to an integer, round to zero
	fctidz	2, 5

# Prep for loading 4503599627370496.0f
	lis		4, _ZN6Burger15g_dMinNoIntegerE@ha

# fAnswer = absolute(fInput) without fraction
	fcfid	2, 2

# Fetch 0.5f in fp3
	lfs		3, _ZN6Burger7g_fHalfE@l(3)

# Fetch 8388608.0f in fp4
	lfd		4, _ZN6Burger15g_dMinNoIntegerE@l(4)

# Create 1.0f in fp6
	fadds	6, 3, 3

# Create 0.0f in fp0
	fsubs	0, 3, 3

# fTemp = fAnswer - fAbs
	fsub	8, 5, 2

# fTemp = fTemp - 0.5f
	fsub	8, 8, 3

# fTemp = (fTemp>=0) ? 1 : 0
	fsel	8, 8, 6, 0

# fAnswer += fTemp
	fadd	2, 2, 8

# if (fInput<0) neg(fAnswer)
	fneg	8, 2
	fsel	2, 1, 2, 8

# fTemp = fAbs - 4503599627370496.0f
	fsub	5, 5, 4

# fp1 = (fAbs - 8388608)>=0 ? fInput, fAnswer
	fsel	1, 5, 1, 2
	blr

	.size	._ZN6Burger9get_roundEd,.-._ZN6Burger9get_roundEd
