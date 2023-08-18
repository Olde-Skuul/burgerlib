#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# double BURGER_API Burger::get_ceiling(double dInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# dInput = fp1
# Result in fp1
#

	.align 2

	.text

	.type	._ZN6Burger11get_ceilingEd,@function
	.globl	._ZN6Burger11get_ceilingEd
._ZN6Burger11get_ceilingEd:

# Convert the float to an integer, round to zero
	fctidz	2, 1

# Prep for loading 1.0f
	lis		3, _ZN6Burger6g_fOneE@ha

# Get abs fInput
	fabs	6, 1

# Convert back to float without fraction, possible final answer
	fcfid	2, 2

# Prep for loading 4503599627370496.0
	lis		4, _ZN6Burger15g_dMinNoIntegerE@ha

# Fetch 1.0f in fp3
	lfs		3, _ZN6Burger6g_fOneE@l(3)

# Fetch 4503599627370496.0 in fp4
	lfd		4, _ZN6Burger15g_dMinNoIntegerE@l(4)

# Take the answer and add 1.0f
	fadd	3, 3, 2

# fp6 has abs(fInput) - 4503599627370496.0f
	fsub	6, 6, 4

# fp5 has answer - fInput, to test if it overflowed
	fsub	5, 2, 1

# answer = ((answer-fInput)>=0) ? answer : answer + 1.0f
	fsel	2, 5, 2, 3

# answer = (fabs(fInput) - 8388608)>=0 ? fInput, answer
	fsel	1, 6, 1, 2
	blr

	.size	._ZN6Burger11get_ceilingEd,.-._ZN6Burger11get_ceilingEd
